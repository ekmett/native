#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#if defined(_WIN32)
#include <process.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "attributes/common.hpp"
#include "static_string.hpp"

// tools for producing chrome:://tracing compatible trace files
// see https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview#heading=h.nb4ul0l9tsgk

/// \todo parameterize all of this on a couple of types that can serialize to json
/// for flow id and for the contents of the message, rather than use \c nlohmann::json directly
namespace ein::profiling {

enum class scope : char {
  global = 'g',
  process = 'p',
  thread = 't',
  none = '\0'
};

/// \cond
NLOHMANN_JSON_SERIALIZE_ENUM( scope, {
  { scope::global, "g" },
  { scope::process, "p" },
  { scope::thread, "t" }
  // none is not a scope
})
/// \endcond

enum event_type : char {
  duration_begin = 'B', duration_end = 'E', complete = 'X', instant = 'i',
  counter = 'C', async_start = 'b', async_instant = 'n', async_end = 'e',
  flow_start = 's', flow_step = 't', flow_end = 'f', sample = 'P',
  object_created = 'N', object_snapshot = 'O', object_destroyed = 'D',
  metadata = 'M', memory_dump_global = 'V', memory_dump_local = 'v',
  mark = 'R', clock_sync = 'c', context_enter = '(', context_leave = ')'
};

/// \cond
NLOHMANN_JSON_SERIALIZE_ENUM( event_type, {
  { event_type::duration_begin, "B"},     { event_type::duration_end, "E"},
  { event_type::complete, "X"},           { event_type::instant, "i"},
  { event_type::counter, "C"},            { event_type::async_start, "b"},
  { event_type::async_instant, "n"},      { event_type::async_end, "e"},
  { event_type::flow_start, "s"},         { event_type::flow_step, "t"},
  { event_type::flow_end, "f"},           { event_type::sample, "P"},
  { event_type::object_created, "N"},     { event_type::object_snapshot, "O"},
  { event_type::object_destroyed, "D"},   { event_type::metadata, "M"},
  { event_type::memory_dump_global, "V"}, { event_type::memory_dump_local, "v"},
  { event_type::mark, "R"},               { event_type::clock_sync, "c"},
  { event_type::context_enter, "("},      { event_type::context_leave, ")"}
})
namespace detail {
  template <typename T> struct duration_suffix_impl;
  template <> struct duration_suffix_impl<std::chrono::milliseconds> {
    static constexpr char const * value = "ms";
  };
  template <> struct duration_suffix_impl<std::chrono::nanoseconds> {
    static constexpr char const * value = "ns";
  };
}
/// \endcond
/// \hideinitializer \hideinlinesource
template <typename T>
static constexpr char const * duration_suffix = detail::duration_suffix_impl<T>::value;

// can adjust granularity as required
template <typename Duration = std::chrono::nanoseconds, typename Clock = std::chrono::high_resolution_clock>
struct profile_event {
  using duration = Duration;
  using clock = Clock;
  static_c_string name{}; // null
  static_c_string cat{}; // null
  event_type ph;
  size_t ts = std::chrono::duration_cast<duration>(clock::now().time_since_epoch()).count();
#if defined(_WIN32)
  int pid = ::_getpid();
#else
  pid_t pid = ::getpid();
#endif
  size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
  scope s;
  std::optional<string> id{}; // null // an actual real string
  static_c_string bp{}; // null
  nlohmann::json args{}; // empty object
};

template <typename Duration, typename Clock>
void to_json(nlohmann::json& j, const profile_event<Duration, Clock> & t) {
  if (t.name) j["name"] = t.name;
  if (t.cat) j["cat"] = t.cat;
  j["ph"] = t.ph;
  j["ts"] = t.ts;
  j["pid"] = t.pid;
  j["tid"] = t.tid;
  if (t.s != scope::none) j["s"] = t.s;
  if (t.id.has_value()) j["id"] = *t.id;
  if (t.bp) j["bp"] = t.bp;
  if (not t.args.is_null()) j["args"] = t.args;
}

template <
  typename Mutex = std::mutex, // for globally accessible profilers
  typename Duration = std::chrono::nanoseconds,
  typename Clock = std::chrono::high_resolution_clock
>
struct profile {
  using mutex = Mutex;
  using duration = Duration;
  using clock = Clock;
  using event = profile_event<Duration, Clock>;

  Mutex events_mutex;
  std::vector<event> events;
  nlohmann::json metadata;
  bool saved;

  profile(profile const &) = delete;
  profile(profile &&) = delete;

  std::filesystem::path filename;
  profile() noexcept
  : events_mutex()
  , events()
  , metadata(nlohmann::json{})
  , saved(true) {
  }

  ~profile() noexcept {
    // TODO: complain if not saved
    if (!saved)
      spdlog::warn("Warning: unsaved profile, discarding {} event(s)", events.size());
  }

  profile & operator = (profile const &) = delete;
  profile & operator = (profile &&) = delete;

  ein_inline
  void log(event const & e) noexcept {
    std::lock_guard<Mutex> lock(events_mutex);
    events.emplace_back(e);
    saved = false;
  }

  ein_inline
  void set_process_name(std::string name = "") noexcept {
#if defined(__APPLE__)
    if (name.empty())
        name = getprogname();
#elif defined(__linux__)
    if (name.empty())
        name = program_invocation_short_name;
#endif
    log({.name = "process_name"_ss, .ph = event_type::metadata, .args = { {"name",name}}});
  }

  ein_inline
  void set_thread_name(std::string const & name) noexcept {
    log({.name = "thread_name"_ss, .ph = event_type::metadata, .args = { {"name",name}}});
  }

  ein_inline
  void instant(static_string name, scope s) noexcept {
    log({.name = name, .ph = event_type::instant, .s = s});
  }

  ein_inline
  void counter(static_string name, nlohmann::json args) noexcept {
    log({.name = name, .ph = event_type::counter, .args = std::move(args) });
  }

  ein_inline
  void flow_start(static_string name, static_string category, static_string id) noexcept {
    log({.name = name, .id = id, .ph = event_type::flow_start, .cat = category});
  }

  ein_inline
  void flow_end(std::string name, static_string cat, static_string id) noexcept {
    log({.name = std::move(name), .cat = std::move(cat), .ph = event_type::flow_end, .id = std::move(id), .bp = "e"_ss});
  }

  ein_inline
  void save(std::filesystem::path filename) noexcept {
    spdlog::info("Saving profile: {}",filename.c_str());
    std::ofstream o(filename);
    o << *this;
  }

  template <typename OStream>
  ein_inline
  friend
  OStream & operator << (OStream & o, profile & p) {
    o << std::setw(2) << nlohmann::json(p) << std::endl;
    p.saved = true;
    return o;
  }
};

// for tdnr
template <typename Mutex, typename Duration, typename Clock>
ein_inline
void to_json(nlohmann::json & j, profile<Mutex,Duration,Clock> const & p) noexcept {
  j = {
    {"traceEvents", p.events},
    {"displayTimeUnit", static_cast<const char *>(duration_suffix<Duration>) }
  };
  if (p.metadata.is_object())
    for (const auto &e : p.metadata.template get<nlohmann::json::object_t>())
      j[e.first] = e.second;
};

template <typename Profile>
struct scope_event {
  using duration = typename Profile::duration;
  using clock = typename Profile::clock;
  using mutex = typename Profile::mutex;
  using event = profile_event<duration,clock>;
  Profile & profile;

  ein_inline
  scope_event(Profile & profile, static_string name) noexcept
  : profile(profile) {
    profile.log({
      .name = name,
      .ph = event_type::duration_begin
    });
  }

  ein_inline
  ~scope_event() noexcept {
    profile.log({
      .ph = event_type::duration_end
    });
  }
};

// for manual toggling
template <typename Profile>
struct duration_event {
  using event = typename Profile::event;
  Profile & profile;
  bool started;
  static_string name;

  ein_inline
  duration_event(Profile & profile, static_string name) noexcept
  : profile(profile), started(false), name(name) {}

  ein_inline
  void start() noexcept {
    assert(not started);
    profile.log({
      .name = name,
      .ph = event_type::duration_begin
    });
    started = true;
  }

  ein_inline
  void stop() noexcept {
    assert(started);
    profile.log({
      .name = name,
      .ph = event_type::duration_end
    });;
    started = false;
  }
};

// creates a profile that automatically saves to disk when it goes out of scope

template <
  typename Mutex = std::mutex, // for globally accessible profilers
  typename Duration = std::chrono::nanoseconds,
  typename Clock = std::chrono::high_resolution_clock
> struct profile_scope {
  using duration = Duration;
  using clock = Clock;
  using event = profile_event<Duration,Clock>;
  using profile = ein::profiling::profile<Mutex,Duration,Clock>;

  std::filesystem::path filename;
  profile p;

  profile_scope() = delete;
  profile_scope(profile_scope const &) = delete;
  profile_scope(profile_scope &&) = delete;

  ein_inline
  profile_scope(std::filesystem::path filename) noexcept
  : filename(std::move(filename)) {
  }

  ~profile_scope() noexcept {
    p.save(filename);
  }

  profile_scope & operator = (profile_scope &&) = delete;
  profile_scope & operator = (profile_scope const &) = delete;

  ein_nodiscard ein_inline
  operator profile const & () const noexcept {
    return p;
  }

  ein_nodiscard ein_inline
  operator profile & () noexcept {
    return p;
  }
};

inline void make_args(nlohmann::json &) noexcept {}

template <typename K, typename V, typename ... Args>
inline void make_args(nlohmann::json & j, K && k, V && v, Args && ... args) {
  j[std::forward(k)] = std::forward(v);
  make_args(j,std::forward<Args>(args)...);
}

// every thread would get one of these generated automatically
// extern thread_local profile_scope<no_mutex> current_thread_profile;
// extern profiler<std::mutex> process_profile;

// better still:

// inside forward() method, we can use
// static auto p = profile_scope("forward.json");

// auto fwd = scope_event("forward"); // will place each call as a top level entry in it.

// if we want to reduce locking overhead, you can use something like
// thread_local auto driver_profile = profile_scope<no_mutex>(fmt::format("driver-{}.json", std::hash<std::thread::id>(std::this_thread::get_id())));

// auto fwd = scope_event("forward"); // will place each call as a top level entry in it.
//   ...
// and it'll automatically give you one global profiler that saves on (graceful) program exit
// and traces all invocations together

// with these we should be able to assemble something like:
// #define EIN_PROFILE_FN ein_maybe_unused auto tron_profile_fn = tron::profiling::scope_event (current_thread_profile,__PRETTY_FUNCTION__)

// and in another view, task by task, we should be able to assemble a view using duration_events letting us see the time skew.

} // ein;

#if defined(EIN_TESTING) || defined(EIN_TESTING_PROFILING)
TEST_CASE("profiling","[profiling]") {
  using namespace nlohmann;
  using namespace ein;

  using namespace ein::profiling;
  using std::filesystem::path;
  using namespace std::chrono;

  SECTION("scope enum serialization") {
    json j = scope::global;
    CHECK(j == "g");

    j = scope::process;
    CHECK(j == "p");

    j = scope::thread;
    CHECK(j == "t");
  }

  SECTION("event_type enum serialization") {
    json j = event_type::duration_begin;
    CHECK(j == "B");

    j = event_type::duration_end;
    CHECK(j == "E");

    j = event_type::complete;
    CHECK(j == "X");
  }
}

#if 0
TEST_CASE("profiling logging (broken)","[.profiling]") {
  using namespace nlohmann;
  using namespace ein;
  using namespace ein::profiling;
  using std::filesystem::path;
  using namespace std::chrono;
  SECTION("profile_event JSON serialization") {
    profile_event<nanoseconds> event{
        .name = "test_event"_ss,
        .cat = "test_category"_ss,
        .ph = event_type::instant,
        .s = scope::thread
    };

    json j = event;
    CHECK(j["name"] == "test_event");
    CHECK(j["cat"] == "test_category");
    CHECK(j["ph"] == "i");
    CHECK(j["s"] == "t");
  }

  SECTION("profile logging") {
    profile<> profiler;
    profile_event<nanoseconds> event{
        .name = "log_event"_ss,
        .ph = event_type::instant,
        .s = scope::thread
    };

    profiler.log(event);

    CHECK(profiler.events.size() == 1);
    CHECK(profiler.events[0].name == "log_event"_scs);
    CHECK(profiler.events[0].ph == event_type::instant);
    CHECK(profiler.events[0].s == scope::thread);
  }

  SECTION("profile_scope automatic saving") {
    path test_path = "test_profile.json";
    {
        profile_scope<> profiler(test_path);
        profile_event<nanoseconds> event{
            .name = "scope_event"_ss,
            .ph = event_type::instant,
            .s = scope::thread
        };

        profiler.p.log(event);
    } // `profile_scope` destructor should save the profile here

    CHECK(exists(test_path));

    // Clean up
    remove(test_path);
  }
}
#endif
#endif
