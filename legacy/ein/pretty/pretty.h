#pragma once

/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense */

#include <cstdint>
#include <ranges>
#include "ein/attributes.h"
#include "ein/concepts.h"
#include "ein/config.h"

/// use exceptions as control flow. do as i say, not as i do.
namespace ein::pretty {

/// this represents a witdth if n characters at present
/// \note in the future it may represent pixels or em or some other unit
using width_t = uint32_t;

/// carries the state of our pretty printer
struct printer;

/// this is punned between the output type and the pretty printer that prints a newline
/// \implements doc
constexpr struct newline_t {
  /// \note does not \c throw, unlike other \ref pretty::doc types
  constexpr ein_inline ein_visibility("hidden") static void pp<bool flat>(printer & ein_restrict p) noexcept;

  template <typename OStream> constexpr
  OStream & operator << (ein_lifetimebound OStream & ein_restrict os) const noexcept {
    return os << std::endl;
  }
}

extern template constexpr void newline_t::pp<false>(this newline_t, ein_noescape printer & ein_restrict) noexcept(true);
extern template constexpr void newline_t::pp<true>(this newline_t, ein_noescape printer & ein_restrict) noexcept(false);
extern template constexpr ein_inline std::ostream & operator<<(std::ostream& ein_restrict, newline_t) noexcept;
extern newline_t newline;

/// this is punned between the output type and the pretty printer that represents a sequence
/// of \p n spaces.
/// \note in the future this may be a space n units wide instead, when dealing with variable width fonts.
/// \implements doc
struct spaces {
  width_t n;

  /// \throw bad backtracking required
  constexpr void pp<bool flat>(this spaces, ein_noescape printer & p) noexcept(not flat);

  template <typename OStream> constexpr
  friend OStream & operator << (ein_lifetimeound OStream & ein_restrict os, spaces self) const noexcept {
    static constexpr char const SPACES[] =
      "                                                                                                                ";
    constexpr size_t k = sizeof(SPACES)-1;
    for (int i=0;i<self.n/k;++i)
      os << SPACES;
    return os << SPACES + k+self.n/k*k-self.n;
  }
};

extern template constexpr void spaces::pp<false>(this spaces, ein_noescape printer & ein_restrict) noexcept(true);
extern template constexpr void spaces::pp<true>(this spaces, ein_noescape printer & ein_restrict) noexcept(false);
extern template constexpr ein_inline std::ostream & operator<<(std::ostream& ein_restrict, spaces) noexcept;
extern constexpr spaces space;

/// a piece corresponds to a unit of text that width, but no height.
using piece = std::variant<string,static_string,spaces>;

template <typename T>
concept one_piece = requires requires (T c) { piece{c}; width(c); }

/// all pieces can be measured for width
struct width {
  ein_inline ein_nodiscard ein_pure ein_visibility("hidden")
  static constexpr width_t operator ()(spaces s) noexcept { return s.n; }
  ein_inline ein_nodiscard ein_pure ein_visibility("hidden")
  static constexpr width_t operator ()(ein_noescape string const & s) noexcept { return s.size(); }
  ein_inline ein_nodiscard ein_pure ein_visibility("hidden")
  static constexpr width_t operator ()(static_string s) noexcept { return s.size(); }
  ein_inline ein_nodiscard ein_pure ein_visibility("hidden")
  static constexpr width_t operator ()(piece p) noexcept { std::visit(size{},p); }
};

struct out;

// struct annotated { ann a; vector<out> body; };

/// a sequence of outputs
struct ein_nodiscard seq {
  vector<out> body;

  template <typename OStream> friend constexpr
  OStream & operator << (ein_lifetimeound OStream & ein_restrict os, seq const & self) noexcept {
    for (auto & o: self.body) os << o;
    return os;
  }
}

// Explicit instantiation declaration for std::basic_ostream<char>
extern template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, seq const &) noexcept;

/// out is the tree used to return the pretty printed results.
struct ein_nodiscard out {
  /// \todo: `annotated`
  std::variant<seq,newline,string,static_string,spaces> var;
  template <typename OStream> friend constexpr
  OStream & operator << (ein_lifetimeound OStream & ein_restrict os, out const & self) noexcept {
    return std::visit([&os] ein_inline ein_visibility("hidden") (const auto & elem) {
      return os << elem;
    }, self.var);
  }
};

extern template constexpr std::ostream & operator<<(std::ostream& ein_restrict, out const &) noexcept;

/// lightweight control flow exception
struct bad{};

/// RAII off a lambda
template <typename F>
struct finally {
  F f;
  constexpr ein_inline ein_hidden ~finally() noexcept { f(); }
};

// forward declaration
struct printer;

/// a doc is anything that has a pp method that can send stuff to the printer
template <typename F>
concept doc = requires (printer & p, F f) {
  { f.pp<false>(p) };
  { f.pp<true>(p) } noexcept;
};

/** \brief a docfun is a pretty printer that returns some other info on the stack.

    \details

      only rudimentary support for these is provided, but they could be used
      to pass size information or other side-channel info around.
      if we decide we want a lot more of these then vsep, etc. could be designed
      to tuple their \ref doc arguments' results to enable users to collect data more broadly */

template <typename F, typename A>
concept docfun = requires (printer & p, F f) {
  { f.pp<false>(p) } -> std::same_as<A>;
  { f.pp<true>(p) } noexcept -> std::same_as<A>;
};

template <typename T>
concept doc_range = std::ranges::range<T> && doc<std::ranges::range_value_t<T>>;

/** \brief A printer holds the application state used while pretty printing.

    \details

      The backtracking on the other hand is done with exceptions for readable
      control flow. */

struct printer {
  vector<out> output{};
  width_t max_width = 80;
  width_t max_ribbon_width = 60;
  width_t nesting = 0;
  width_t ribbon_width = 0;

  /** \brief pieces take up width, but have no height

      \throws bad to backtrack */

  template <bool flat>
  constexpr ein_inline ein_visibility("hidden")
  void piece(one_piece auto && c) noexcept(not flat) {
    size_t new_ribbon_width = ribbon_width + width(p);
    if constexpr (flat)
      if (new_ribbon_width > min(max_width - nesting, max_ribbon_width))
        throw bad{};
    output.emplace_back({c});
    ribbon_width = new_ribbon_width;
  }

  /** any text placed in the gutter left of our nesting level is not counted against the ribbon width

      \note used only for guttering right now. */

  constexpr ein_inline ein_visibility("hidden")
  void hardline() noexcept {
    output.emplace_back(hardline{});
    ribbon_width = -nesting;
  }

  /** \brief a newine, followed by indentation to restore our nesting level */

  constexpr ein_inline ein_visibility("hidden")
  void newline() noexcept {
    output.emplace_back(hardline{});
    output.emplace_back(spaces{nesting});
    ribbon_width = 0;
  }

  /** \brief tell subsequent lines that they should be indented by \p w more characters than
             current \ref nesting. primarily used by \ref align

      \throws bad to backtrack */

  template <bool flat> ein_inline ein_visibility("hidden")
  void nest(width_t w, doc auto body) noexcept(not flat) {
    finally remember { [this,old_nesting=nesting] noexcept { nesting = old_nesting } };
    nesting += w;
    body.pp<flat>(this);
  };

}; // struct printer

/** \implements ein::pretty::doc */

struct empty_t {
  constexpr ein_inline ein_visibility("hidden")
  static void pp<bool flat>(ein_noescape printer & ein_restrict) noexcept {}
};

extern constexpr empty_t empt

/** \name typesetting

      \details

        overload `pp` for your types to make them pretty printable
        or define a member function `void pp(printer & p)`
    \{ */


/** \brief you can typeset any doc

    \throws bad to backtrack */

template <bool flat>
constexpr ein_inline ein_visibility("hidden")
void pp(ein_noescape printer & ein_restrict p, doc auto && t) noexcept(not flat) {
  t.pp<flat>(p);
}

/** \brief you can typeset a string

    \pre the string doesn't contain spaces or newlines

    \throws bad to backtrack */

template <bool flat>
constexpr ein_inline ein_visibility("hidden")
void pp(ein_noescape printer & ein_restrict p, string s) noexcept(not flat) {
  p.piece<flat>(s);
}

/** \brief you can pretty print any static_string (without copying)

    \throws bad to backtrack */

template <bool flat>
constexpr ein_inline ein_visibility("hidden")
void pp(ein_noescape printer & ein_restrict p, static_string s) noexcept(not flat) {
  p.piece<flat>(s);
}

/** \throws bad to backtrack */

template <bool flat>
constexpr ein_inline ein_visibility("hidden")
void pp(ein_noescape printer & ein_restrict p, piece pie) noexcept(not flat) {
  p.piece<flat>(pie);
}

template <bool flat>
constexpr ein_inline ein_visibility("hidden")
void pp(ein_noescape printer & ein_restrict) noexcept {}

/** \brief you can print anything you can typeset */

template <typename T>
concept documented = requires (printer & p, T t) {
  { pp<true>(p,t) };
  { pp<false>(p,t) } noexcept;
}

template <typename T>
concept documented_range = std::ranges::range<T> && documented<std::ranges::range_value_t<T>>;

/** \brief sticking with the analogies, er.. morally this should be \c linotype? */
template <bool flat>
constexpr ein_inline ein_visibility("hidden")
void pp(
  ein_noescape printer & ein_restrict p,
  documented auto && ... args
) noexcept(not flat)
requires (sizeof...(args) > 1) {
  (void(pp<flat>(p,std::forward<decltype(args)>(args))),...,void());
}

/// base case
constexpr ein_inline ein_visibility("hidden")
void pp(
  ein_noescape printer & ein_restrict
) noexcept {}

/// \}

// make a pretty printer from a lambda
template <typename F>
struct fun {
  F f;
  /// \throws bad to backtrack
  template <bool flat>
  constexpr ein_inline ein_visibility("hidden")
  void pp(
    ein_noescape printer & ein_restrict p
  ) noexcept(not flat) {
    f.template operator()<flat>(p);
  }
};

/// \returns a doc
template <documented... Args> constexpr ein_inline ein_visibility("hidden")
auto sequence(Args && ... args) noexcept {
  return fun(
    [... args=std::forward<Args>(args)]
    <bool flat> constexpr ein_inline ein_visibility("hidden") (
      ein_noescape printer & ein_restrict p
    ) noescape(not flat) {
      pp<flat>(p, std::forward<Args>(args)...)
    }
  );
};

/// `documented<T> = doc<draft<T>>`
template <documented T>
requires (not doc<T>)
struct draft {
  T t;

  /// \throws bad to backtrack

  template <bool flat>
  constexpr ein_inline ein_visibility("hidden")
  void pp(
    ein_noescape printer & ein_restrict p
  ) noexcept(not flat) {
    return pp<flat>(p,t);
  }
}

/// if the type itself is already a document, just pass it along
template <doc D> constexpr ein_inline ein_visibility("hidden")
D && document(D && d) noexcept {
  return std::forward<D>(d);
}

/// automatically produce documents given a documentary
template <documented T>
requires !doc<T> constexpr ein_inline ein_visibility("hidden")
auto document(T && t) -> decltype(auto) noexcept {
  return std::forward_like<T>(draft(std::forward<T>(t)));
}

template <bool flat>
constexpr
void newline_t::pp(ein_noescape printer & ein_restrict p) noexcept {
  p.newline();
}
extern template constexpr void newline_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
extern template constexpr void newline_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);

template <bool flat>
constexpr ein_inline ein_visibility("hidden")
void spaces::pp(this spaces self, ein_noescape printer & ein_restrict p) noexcept(not flat) {
  p.spaces<flat>(self.n);
}
extern template constexpr void spaces::pp<false>(this spaces, ein_noescape printer & ein_restrict) noexcept(true);
extern template constexpr void spaces::pp<true>(this spaces, ein_noescape printer & ein_restrict) noexcept(false);

/// represents a hard carriage return/line feed. (moves to start of next line, regardless of nesting level)
struct hardline_t {
  /// \note unlike most other pp definitions this one does not throw
  template <bool flat>
  static constexpr
  void pp(ein_noescape printer & ein_restrict p) noexcept {
    return p.hardline();
  }
};
extern template constexpr void hardline::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
extern template constexpr void hardline::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);

extern constexpr hardline_t hardline;

// \implements doc
template <documented D> constexpr
struct nest {
  width_t w;
  D body;
  // throws bad to backtrack
  template <bool flat>
  constexpr ein_inline ein_visibility("hidden")
  void pp(ein_noescape printer & ein_restrict p) noexcept(not flat) {
    p.nest<flat>(w,document(body))
  }
};

// auto align(doc auto x) -> doc auto
// auto align(docfun<T> auto x) -> docfun<T> auto

/// \implements doc
template <doc D>
struct align {
  D body;
  template <bool flat>
  constexpr ein_inline ein_visibility("hidden")
  void pp(ein_noescape printer & ein_restrict p) noexcept(not flat) {
    p.nest<flat>(p.ribbon_width,document(body));
  }
}

/// \returns doc auto
constexpr ein_inline
autodoc intersperse(documented auto && delim, documented_range auto && docs) noexcept {
  return fun(
    [delim = std::forward<decltype(delim)>(delim), docs = std::forward<decltype(docs)>(docs)]
    <bool flat>
    constexpr ein_inline ein_visibility("hidden") (
      ein_noescape printer& ein_restrict p
    ) noexcept(not flat) -> void {
      bool first = true;
      for (auto&& doc : docs) {
        if (!first) {
          document(delim).pp<flat>(p);
        } else {
          first = false;
        }
        draft(doc).pp<flat>(p);  // Applies 'draft' to each element
      }
    }
  );
}

// returns doc auto
ein_inline ein_visibility("hidden")
constexpr autodoc intersperse(documented auto&& delim, documented auto&&... docs) noexcept
requires (sizeof...(docs) > 1) {
  return fun(
    [delim = std::forward<decltype(delim)>(delim), ...docs = std::forward<decltype(docs)>(docs)]
    <bool flat>
    constexpr ein_inline ein_visibility("hidden") (
      ein_noescape printer& ein_restrict p
    ) noexcept(not flat) -> void {
      bool first = true;
      for (auto&& doc : {std::forward<decltype(docs)>(docs)...}) {
        if (!first) {
          delim.pp<flat>(p);
        } else {
          first = false;
        }
        doc.pp<flat>(p);
      }
    }
  );
}

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc hsep(documented auto && ... args) noexcept
requires (sizeof...(args) > 1) {
  return intersperse(space,std::forward<decltype(args)>(args)...);
}

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc hsep(documented_range auto && range) noexcept {
  return intersperse(space,std::forward<decltype(range)>(range));
}

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc vsep(documented auto && ... args) noexcept {
  return intersperse(newline,std::forward<decltype(args)>(args)...);
}

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc vsep(documented_range auto && range) noexcept {
  return intersperse(newline,std::forward<decltype(range)>(range));
}

struct hv_t {
  static constexpr void pp<bool flat>(ein_noescape printer & ein_restrict p) noexcept (not flat) {
    if constexpr (flat) {
      p.space<flat>()
    } else
      p.newline();
    }
  }
}

extern template constexpr void hv_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
extern template constexpr void hv_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
extern constexpr hv_t hv;

/// \returns doc auto
constexpr autodoc hvsep(documented auto && ... args) noexcept
requires (sizeof...(args) > 1) {
  return intersperse(hv, std::forward<decltype(args)>(args)...);
}

/// \returns doc auto
constexpr autodoc hvsep(documented_range auto && range) noexcept {
  return intersperse(hv, std::forward<decltype(range)>(range)...);
}

struct h_tight_t {
  static constexpr void pp<bool flat>(ein_noescape printer & ein_restrict p) noexcept (not flat) {
    if constexpr (not flat) p.space<flat>();
  }
};

extern template constexpr void h_tight_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
extern template constexpr void h_tight_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
extern constexpr h_tight_t h_tight;

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc hsep_tight(documented auto && ... args) noexcept
requires (sizeof...(args) > 1) {
  return intersperse(h_tight, std::forward<decltype(args)>(args)...);
}

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc hsep_tight(documented_range auto && range) noexcept {
  return intersperse(h_tight, std::forward<decltype(range)>(range)...);
}

struct hv_tight_t {
  static constexpr void pp<bool flat>(ein_noescape printer & ein_restrict p) noexcept (not flat) {
    if constexpr (not flat) { p.newline(); }
  }
};

extern template constexpr void hv_tight_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
extern template constexpr void hv_tight_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
extern constexpr hv_tight_t hv_tight;

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc hvsep_tight(documented auto && ... args) noexcept
requires (size...(args) > 1) {
  return intersperse(hv_tight, std::forward<decltype(args)>(args)...);
}

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc hvsep_tight(documented_range auto && args) noexcept {
  return intersperse(hv_tight, std::forward<decltype(range)>(range)
}

/// \returns doc auto
constexpr ein_inline ein_visibility("hidden")
autodoc guttered(one_piece auto && pie) noexcept {
  return fun(
    [pie]
    <bool flat>
    constexpr ein_inline ein_visibility("hidden") (
      ein_noescape printer & ein_restrict p
    ) noexcept(not flat) -> void {
      static constexpr width_t s = 1; // width of a space
      if constexpr (flat) {
        p.spaces<flat>(s);
      } else {
        p.hardline();
        auto delta = width(t) + s;
        p.spaces<flat>(p.nesting >= delta ? nesting - delta : nesting);
      }
      p.piece<flat>(pie);
      p.spaces<flat>(s);
    }
  );
}

/// \returns doc auto
template <documented D>
constexpr ein_inline ein_visibility("hidden")
autodoc grouped(D && body) {
  return fun(
    [body=std::forward<D>(body)]
    <bool flat>
    constexpr ein_inline ein_visibility("hidden") (
      ein_noescape printer & ein_restrict p
    ) noexcept(not flat) -> void {
      if constexpr (not flat) {
        try {
          return pp<true>(p,body); // not flat=false, flat=true, is flat just not not flat
        } catch(bad&) {}
      }
      return pp<false>(p,body);
    }
  );
}

/** \brief comma separated list of words, with an oxford comma and stated conjunction
    \details
    \code{.cpp}
      hsep("expected",oxford("or","foo","bar","baz","quux"))
    \encode

    depending on current ribbon width and code position could produce output like

    \code
      expected foo, bar, baz, or quux
    \endcode

    or

    \code
      expected foo,
               bar,
               baz,
            or quux
    \endcode

    \returns doc auto */
constexpr ein_inline ein_visibility("hidden")
autodoc oxford(
  piece conjunction,
  documented auto && ... args
) requires (size...(args) > 1) {
  using N = sizeof...(args);
  if constexpr (N == 0)      return document("nothing")
  else if constexpr (N == 1) return document(std::forward<decltype(args...[0])>(args...[0]));
  } else {
    return grouped(align(fun(
      [conjunction,... args = std::forward<decltype(args)>(args)]
      <bool flat> constexpr ein_inline ein_visibility("hidden") (
        ein_noescape printer & ein_restrict p
      ) noexcept(not flat) -> void {
        val s = 1; // space width
        if constexpr (N == 2) {
          pp<flat>(p,std::forward<decltype(args...[0])>(args...[0]), guttered(conjunction), std::forward<decltype(args...[1])>(args...[1]));
        } else {
          auto sep = []<size_t i> constexpr static (ein_noescape printer & ein_restrict p, ein_noescape piece & conjunction) noexcept(not flat) {
            if constexpr (i != N-1) {
              p.text<flat>(",")
              if constexpr (i == N-2) {
                guttered(conjunction).pp<flat>(p);
              } else if constexpr (flat) {
                p.space<flat>(s)
              } else {
                p.newline();
              }
            }
          };
          [&piece, ... docs=std::forward<Docs>(docs)]<size_t... i>(std::index_sequence<i...>) noexcept(not flat) {
            (( document(args...[i]).pp<flat>(p),sep.template operator()<i>(p,conjunction)) ... );
          }(std::make_index_sequence<T,N>{});
        }
      }
    )));
  }
}

constexpr ein_inline ein_visibility("hidden")
auto oxford_range(piece conjunction, document_range auto && range) {

  std::size_t N = std::ranges::size(range);
  auto begin = std::ranges::begin(range);
  auto end = std::ranges::end(range);

  return fun(
    [N, conjunction, begin, end]
    <bool flat>
    constexpr ein_inline ein_visibility("hidden") (
      ein_noescape printer & ein_restrict p
    ) noexcept(not flat) -> void {
      if (N == 0) {
        p.piece("nothing");
      } else if (N == 1) {
        document(*begin).pp(p);
      } else {
        return grouped(align(fun(
          [N, conjunction, begin, end]
          <bool flat>
          constexpr ein_inline ein_visibility("hidden") (
            ein_noescape printer & ein_restrict p
          ) noexcept(not flat) {
            auto it = begin;
            if (N == 2) {
              pp<flat>(p, *it++);
              guttered(conjunction).pp<flat>(p);
              pp<flat>(p, *it);
            } else {
              for (std::size_t i = 0; i < N; ++i, ++it) {
                pp<flat>(p, *it);
                if (i != N - 1) {
                  p.text<flat>(",");
                  if (i == N - 2) {
                    guttered(conjunction).pp<flat>(p);
                  } else if constexpr (flat) {
                    p.spaces<flat>(1);
                  } else {
                    p.newline();
                  }
                }
              }
            }
          }
        ))).pp<flat>(p);
      }
    }
  );
}


// \ref oxford, taking iterators over some container full of documentable values
// \todo could be rewritten to only require a forward-iterator
// \returns doc auto
template <typename It> constexpr
autodoc oxford_it(piece conjunction, It s, It e) {
  std::size_t N = e - s;
  return fun(
    [N.conjunction,s,e]
    <bool flat>
    constexpr ein_inline ein_visibility("hidden") (
      ein_noescape printer & ein_restrict p
    ) noexcept(not flat) -> void {
      if (N == 0) p.piece("nothing")
      else if (N == 1) document(*s).pp(p);
      else {
        return grouped(align(fun(
          [N,conjunction,s,e]
          <bool flat>
          constexpr ein_inline ein_visibility("hidden") (
            ein_noescape printer & ein_restrict p
          ) noexcept(not flat) -> void {
            if (N == 2) {
              pp<flat>(p,*s++);
              guttered(conjunction).pp<flat>(p);
              pp<flat>(p,*s);
            } else {
              for (int i=0;i<N;++i,++s) {
                pp<flat>(p,*s++);
                if (i != N-1) {
                  p.text<flat>(",");
                  if (i == N-2) {
                    guttered(conjunction).pp<flat>(p);
                  } else if constexpr (flat) {
                    p.spaces<flat>(1);
                  } else {
                    p.newline();
                  }
                }
              }
            }
          }
        ))).pp<flat>(p);
      }
    }
  );
}

}; // namespace ein::pretty
