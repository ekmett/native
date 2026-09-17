// SPDX-FileCopyrightText: 2024, 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

/** \file
    \brief Compiler attributes for host code, with shader-safe shared modifiers.

    \defgroup attributes Attributes
    Named annotations for optimization, lifetimes, ownership and static analysis.
    Include this header in the global module fragment; macros are not exported
    by C++ module imports. */

#ifdef __cplusplus

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup attribute_detection Attribute Detection
///
///   \brief macros for determining supported attributes
///
/// \ingroup attributes
/// \{

/** \def simd_has_attribute(x)

      \brief `__has_attribute(x)`

      \details

      Used as the primary means for detecting advanced annotation features */

#ifdef __has_attribute
  #define simd_has_attribute(x) __has_attribute(x)
#else
  #define simd_has_attribute(__x) 0
#endif

/** \def simd_has_declspec_attribute(__x)

      \brief portable `__has_declspec_attribute(__x)`

      \details

        Defaults to 1 on MSVC, where we can assume `__declspec(x)`
        is well supported.

        Uses the `__has_declspec_attribute(x)` implementation in Clang

        Defaults to 0 otherwise. */

#if defined(_MSC_VER)
  #define simd_has_declspec_attribute(__x) 1
#elif defined(SIMD_USE_DECLSPEC) && defined(__has_declspec_attribute)
  #define simd_has_declspec_attribute(x) __has_declspec_attribute(x)
#else
  #define simd_has_declspec_attribute(__x) 0
#endif

// MS ABI otherwise allocates space for a second empty accessor base.
#if simd_has_declspec_attribute(empty_bases)
  #define simd_empty_bases __declspec(empty_bases)
#else
  #define simd_empty_bases
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup purity_attributes Purity
///
///   \ingroup attributes
///
/// \{

/** \def simd_const
    \brief `[[const]]` is not `const`

    \details

        Indicates the result is entirely determined by the arguments
        and does not access main memory in any way, including accessing
        members of `this`

        This allows the compiler to easily elide/duplicate calls, because
        it doesn't need to consider aliasing at all when moving this
        function's body around.

        WHEN IN DOUBT USE `simd_pure`, which is a much safer way to annotate
        pure functional code */

#if simd_has_attribute(const)
  #define simd_const __attribute__((const))
#else
  #define simd_const
#endif

/** \def simd_pure

      \brief `[[pure]]`

      \details

        No side-effects other than return value, may inspect globals

        Allows the compiler to easily elide/duplicate calls.
        This can freely commute past other operations that can be shown
        not to affect the memory locations read. */

#if simd_has_attribute(pure)
  #define simd_pure __attribute__((pure))
#else
  #define simd_pure
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup lifetime_attributes Object Lifetimes
///
///   \ingroup attributes
///
/// \{

/** \def simd_lifetimebound

      \brief `[[lifetimebound]]`

      \details

        The argument must be kept alive as long as the result of the
        function is alive. Should be used for functions that return
        references or views into the target object.

        This is a strong hint that this object "owns" the result and
        is just letting you borrow it.

        _Editorial_:

        As a rule of thumb all pointer arguments should be analyzed
        to be either #simd_noescape or #simd_lifetimebound, and
        #simd_lifetimebound should be applied to all methods that
        return a self-reference */

#if simd_has_attribute(lifetimebound)
  #define simd_lifetimebound [[clang::lifetimebound]]
#else
  #define simd_lifetimebound
#endif

/** \def simd_noescape

      \brief portable `__attribute__((noescape))`

      \details

        argument is not captured by the function (rust-style borrow) */

#if simd_has_attribute(noescape)
  #define simd_noescape __attribute__((noescape))
#else
  #define simd_noescape
#endif

/** \def simd_nodiscard

      \brief C++17 `[[nodiscard]]`.

      \details

        The user should explicitly throw away the result rather than let it be silently discarded

        Note: Despite being already standard, this is used primarily to annotate the definition with
        a `[[nodiscard]]` qualifier in DOXYGEN.  */

#define simd_nodiscard [[nodiscard]]

/// \}


///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup inlining_attributes Inlining Control
///
///   \brief attributes that control inlining behavior
///
///   \ingroup attributes
///
/// \{

/** \def simd_inline

      \brief `inline [[always_inline]]` */

#if simd_has_attribute(always_inline)
  #define simd_inline inline __attribute__((always_inline))
#elif defined _WIN32
  #define simd_inline __forceinline
#else
  #define simd_inline inline
#endif

/** \def simd_flatten

      \brief portable `[[flatten]]`

      \details

        the compiler should inline recursively aggressively under
        this definition. use with care, good for expression templates */

#if simd_has_attribute(flatten)
  #define simd_flatten __attribute__((flatten))
#else
  #define simd_flatten
#endif

/** \def simd_artificial

      \brief `[[artificial]]`.

      \details

        The debugger should not single-step into this function.
        Treat it as atomic and associate it with the debug information for the use site instead.  */

#if simd_has_attribute(artificial)
  #define simd_artificial __attribute__((artificial))
#elif simd_has_attribute(__artificial__)
  #define simd_artificial __attribute__((__artificial__))
#else
  #define simd_artificial
  #warning "[[artificial]] is not supported"
#endif

/** \def simd_noinline

      \brief `[[noinline]]`

      \details

        Indicates that the inliner should avoid inlining this function at usage sites */

#if simd_has_attribute(noinline)
  #define simd_noinline __attribute__((noinline))
#else
  #define simd_noinline
#endif

/** \def simd_optnone

      \brief `[[optnone]]`

      \details

        Used to indicate that regardless of optimization level, you shouldn't optimize this one function.
        Useful for local debugging when you can't disable optimization for the entire build. (e.g. ours!) */

#if simd_has_attribute(optnone)
  #define simd_optnone __attribute__((optnone))
#else
  #define simd_optnone
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup linkage_attributes Linkage Control
///
///   \brief see \ref instantiation_attributes for more.
///
///   \ingroup attributes
///
/// \{

/** \def simd_weak

      \brief `[[weak]]`

      \details

        used to generate a symbol that can be easily overriden by the linker. */

#if simd_has_attribute(weak)
  #define simd_weak __attribute__((weak))
#else
  #define simd_weak
#endif

/** \def simd_internal_linkage

      \brief `[[internal_linkage]]`

      \details

        Changes the linkage type of the declaration to internal. This is similar to
        C-style static, but can be used on classes and class methods. When applied
        to a class definition, this attribute affects all methods and static data
        members of that class. This can be used to contain the ABI of a C++ library
        by excluding unwanted class methods from the export tables.

        \warning probably subsumed by simd_visibility("hidden") */

#if simd_has_attribute(internal_linkage)
  #define simd_internal_linkage __attribute__((internal_linkage))
#else
  #define simd_internal_linkage
#endif

/** \def simd_preferred_name(x)

      \brief portable `[[clang::preferred_name(x)]]` annotations

      \details

        Can be applied to a template struct to provide better names
        for some concrete instantiations */

#if __has_cpp_attribute(clang::preferred_name)
  #define simd_preferred_name(x) [[clang::preferred_name(__x)]]
#else
  #define simd_preferred_name(__x)
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup instantiation_attributes Explicit Template Instantiation
///
///   \brief attributes that can control explicit template instantiation
///
///   \ingroup linkage_attributes
///
/// \{

/** \def simd_visibility

      \brief `[[visibility(x)]]`

      \details

        Primarily used for `[[visibility("hidden")]]` to disable inclusion of the symbol in the resulting library */

#if simd_has_attribute(__visibility__)
  #define simd_visibility(...) __attribute__((__visibility__(__VA_ARGS__)))
#else
  #define simd_visibility(...)
  #warning "[[visibility]] is not supported"
#endif

/** \def simd_exclude_from_explicit_instantiation

      \brief exclude this member from explicit template instantiation.

      \details

        Usually paired with #simd_visibility`("hidden")`, see #simd_hidden */

#if simd_has_attribute(exclude_from_explicit_instantiation)
  #define simd_exclude_from_explicit_instantiation __attribute__((exclude_from_explicit_instantiation))
#else
  #define simd_exclude_from_explicit_instantiation
  #warning "[[exclude_from_explicit_instantiation]] is not supported"
#endif

/** \def simd_hidden

      \brief `[[visibility("hidden")]] [[exclude_from_explicit_instantiations]]`

      \details

        Use to exclude from both having to compute in explicit template instantiations
        and expecting it to be stored. This means that the resulting member is safely
        delayed to the usage site, despite the template being explicitly otherwise
        instantiated. Useful for `#simd_inline` and the general sort of SIMD intrinsic
        wrappers where there's no point in emitting a function body that will never be
        called, and will just bloat executable size. */

#define simd_hidden simd_visibility("hidden") simd_exclude_from_explicit_instantiation

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup initialization_attributes Initialization
///
///   \ingroup attributes
///
/// \{

/** \def simd_constinit
    \brief `constinit`

      \details

        `constinit` requires C++20, but `__attribute__((require_constant_initialization))`
        was available slightly earlier. */

#if __cpp_constinit
  #define simd_constinit constinit
#elif simd_has_attribute(require_constant_initialization)
  #define simd_constinit __attribute__((require_constant_initialization))
#else
  #define simd_constinit
#endif

/** \def simd_uninitialized
    \brief `[[clang::uninitialized]]`

      \details

        ensures a stack variable remains uninitialized regardless of
        `-ftrivial-auto-var-init=*` settings passed to the compiler */

#if simd_has_attribute(uninitialized)
  #define simd_uninitialized __attribute__((uninitialized))
#else
  #define simd_uninitialized
#endif

/** \def simd_reinitializes
    \brief `[[clang::reinitializes]]`

      \details

        Indicates to any uninitialised object state sanitizer
        that this restores an object to a fresh state independent
        of its previous state. */

#if __has_cpp_attribute(clang::reinitializes)
  #define simd_reinitializes [[clang::reinitializes]]
#else
  #define simd_reinitializes
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup target_attributes Function Multi-Versioning
///
///   \brief attributes for function multiversioning
///
///   \details
///
///     Multi-versioned functions have their symbols resolved at linkage time by a function
///     that can look at the CPU and make linking decisions. These are theoretically great!
///
///     \warning
///
///       Unfortunately, they don't support `[[flatten]]` and they don't support function
///       templates as implemented at this time. When they do they'll become a powerful tool.
///
///   \ingroup attributes
///
/// \{

/** \def simd_target(x)

      \brief this indicates a required feature set for the current multiversioned function.

      \details

        Overloads will be resolved at load time by the linker based on detected cpu capabilities.
        Unfortunately this cannot be applied to templated functions (or ones that take `auto`) */

#define simd_target(x) __attribute__((target(x)))

/** \def simd_target_clones(...)

      \brief this indicates a required feature set for the current multiversioned function.

      \details

        Overloads will be resolved at load time. */

#define simd_target_clones(...) __attribute__((target_clones(_VA_ARGS_)))

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup calling_convention_attributes Calling Conventions
///
///   \brief alternative calling conventions
///
///   \ingroup attributes
///
/// \{

/** \def simd_hot

      \brief `[[hot]]`

      \details

        calling convention frequent inner loop calls */

#if simd_has_attribute(hot)
  #define simd_hot __attribute__((hot))
#else
  #define simd_hot
#endif

/** \def simd_cold

      \brief `[[cold]]`

      \details

        calling convention for very infrequent calls (e.g. initializers) */

#if simd_has_attribute(cold)
  #define simd_cold __attribute__((cold))
#else
  #define simd_cold
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup typestate_analysis Typestate Analysis
///
///
/// \brief Enabled in `clang` by passing `-fsanitize=consumed`
///
///   \ingroup attributes
///
/// \{

/** \def simd_consumable(x)

      \brief Each class that uses any typestate annotations must first be marked using this attribute.

      \details

        Failure to do so will result in a warning.
        This attribute accepts a single parameter that must be one of the following:
        "unknown", "consumed", or "unconsumed". */

#if simd_has_attribute(consumable)
  #define simd_consumable(__x) __attribute__((consumable(__x)))
#else
  #define simd_consumable(__x)
#endif

/** \def simd_callable_when(...)

      \details

        Use to indicate what states a method may be called in. Valid states
        are `"unconsumed"`, `"consumed"`, or `"unknown"`.
        Each argument to this attribute must be a quoted string. */

#if simd_has_attribute(callable_when)
  #define simd_callable_when(...) __attribute__((callable_when(__VA_ARGS__)))
#else
  #define simd_callable_when(...)
#endif

/** \def simd_param_typestate(...)

      \details

        This attribute specifies expectations about function parameters. Calls to a
        function with annotated parameters will issue a warning if the corresponding
        argument isn’t in the expected state. The attribute is also used to set the
        initial state of the parameter when analyzing the function’s body. */

#if simd_has_attribute(param_typestate)
  #define simd_param_typestate(...) __attribute__((param_typestate(__VA_ARGS__)))
#else
  #define simd_param_typestate(...)
#endif

/** \def simd_return_typestate(x)

      \details

        The `return_typestate` attribute can be applied to functions or parameters.

        When applied to a function the attribute specifies the state of the returned value.
        The function’s body is checked to ensure that it always returns a value in the
        specified state. On the caller side, values returned by the annotated function are
        initialized to the given state.

        When applied to a function parameter it modifies the state of an argument after a
        call to the function returns. The function’s body is checked to ensure that the
        parameter is in the expected state before returning. */

#if simd_has_attribute(return_typestate)
  #define simd_return_typestate(...) __attribute__((return_typestate(__VA_ARGS__)))
#else
  #define simd_return_typestate(...)
#endif

/** \def simd_moving

      \brief `[[return_typestate("consumed")]]` `[[param_typestate("unconsumed")]]`

      \details

        Short-hand for indicating that this argument was moved and is now `"consumed"`.
        Assumes it was `"unconsumed"` before. */

#define simd_moving simd_return_typestate("consumed") simd_param_typestate("unconsumed")

/** \def simd_set_typestate(x)

      \details

        Annotate methods that transition an object into a new state.

        The new state must be `"unconsumed"`, `"consumed"`, or `"unknown"`. */

#if simd_has_attribute(set_typestate)
  #define simd_set_typestate(...) __attribute__((set_typestate(__VA_ARGS__)))
#else
  #define simd_set_typestate(...)
#endif

/** \def simd_test_typestate(x)

      \details

        Use to indicate that a method returns true if the object is in the specified state.

        The state must be `"unconsumed"`, `"consumed"`, or `"unknown"`. */

#if simd_has_attribute(test_typestate)
  #define simd_test_typestate(...) __attribute__((test_typestate(__VA_ARGS__)))
#else
  #define simd_test_typestate
  #define simd_test_typestate(...)
#endif // simd_has_attribute(test_typestate)

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup thread_safety_attributes Thread Safety
///
///   \brief these are supported by `-Wthread-safety`
///
///   \ingroup attributes
///
/// \{

/** \def simd_no_thread_safety_analysis

    \details

    Use on a function declaration to specify that the thread safety analysis
    should not be run on that function. This attribute provides an escape hatch
    (e.g. for situations when it is difficult to annotate the locking policy). */

#if simd_has_attribute(no_thread_safety_analysis)
  #define simd_no_thread_safety_analysis __attribute__((no_thread_safety_analysis))
#else
  #define simd_no_thread_safety_analysis
#endif

/** \def simd_lockable

    \details

    Use on a class definition to specify that it has a
    lockable type (e.g. a Mutex class). This annotation is
    primarily used to check consistency. */

#if simd_has_attribute(lockable)
  #define simd_lockable __attribute__((lockable))
#else
  #define simd_lockable
#endif

/** \def simd_scoped_lockable

      \details

        Use on a class definition to specify that it has a
        “scoped” lockable type. Objects of this type will acquire the
        lock upon construction and release it upon going out of scope.
        This annotation is primarily used to check consistency. */

#if simd_has_attribute(scoped_lockable)
  #define simd_scoped_lockable __attribute__((scoped_lockable))
#else
  #define simd_scoped_lockable
#endif

/** \def simd_guarded_var

      \details

        Use on a variable declaration to specify that the variable
        must be accessed while holding some lock. */

#if simd_has_attribute(guarded_var)
  #define simd_guarded_var __attribute__((guarded_var))
#else
  #define simd_guarded_var
#endif

/** \def simd_pt_guarded_var

      \details

        Use on a pointer declaration to specify that the
        pointer must be dereferenced while holding some lock. */

#if simd_has_attribute(pt_guarded_var)
  #define simd_pt_guarded_var __attribute__((pt_guarded_var))
#else
  #define simd_pt_guarded_var
#endif

/** \def simd_guarded_by(l)

      \details

        Use on a variable declaration to specify that the
        variable must be accessed while holding lock \p l. */

#if simd_has_attribute(guarded_by)
  #define simd_guarded_by(x) __attribute__((guarded_by(x)))
#else
  #define simd_guarded_by(x)
#endif

/** \def simd_pt_guarded_by(l)

      \details

        Use on a pointer declaration to specify that the
        pointer must be dereferenced while holding lock \p l. */

#if simd_has_attribute(pt_guarded_by)
  #define simd_pt_guarded_by(x) __attribute__((pt_guarded_by(x)))
#else
  #define simd_pt_guarded_by(x)
#endif

/** \def simd_acquired_before(...)

      \details

        Use on a declaration of a lockable variable to
        specify that the lock must be acquired
        before all attribute arguments. Arguments must be
        lockable type, and there must be at least one argument. */

#if simd_has_attribute(acquired_before)
  #define simd_acquired_before(...) __attribute__((acquired_before(__VA_ARGS__)))
#else
  #define simd_acquired_before(...)
#endif

/** \def simd_acquired_after(...)

      \details

        Use on a declaration of a lockable variable to specify that
        the lock must be acquired after all attribute arguments.
        Arguments must be lockable type, and there must be at
        least one argument. */

#if simd_has_attribute(acquired_after)
  #define simd_acquired_after(...) __attribute__((acquired_after(__VA_ARGS__)))
#else
  #define simd_acquired_after(...)
#endif

/** \def simd_exclusive_lock_function(...)

      \details

        Use on a function declaration to specify that the function
        acquires all listed locks exclusively. This attribute takes
        zero or more arguments: either of lockable type or integers
        indexing into function parameters of lockable type. If no
        arguments are given, the acquired lock is implicitly that
        of the enclosing object. */

#if simd_has_attribute(exclusive_lock_function)
  #define simd_exclusive_lock_function(...) __attribute__((exclusive_lock_function(__VA_ARGS__)))
#else
  #define simd_exclusive_lock_function(...)
#endif

/** \def simd_shared_trylock_function(...)

      \details

        Use on a function declaration to specify that the function will try
        (without blocking) to acquire all listed locks, although the locks
        may be shared (e.g. read locks). This attribute takes one or more
        arguments. The first argument is an integer or boolean value
        specifying the return value of a successful lock acquisition. The
        remaining arugments are either of lockable type or integers
        indexing into function parameters of lockable type. If only one
        argument is given, the acquired lock is implicitly `this` of the
        enclosing object. */

#if simd_has_attribute(shared_trylock_function)
  #define simd_shared_trylock_function(...) __attribute__((shared_trylock_function(__VA_ARGS__)))
#else
  #define simd_shared_trylock_function(...)
#endif

/** \def simd_unlock_function(...)

      \details

        Use on a function declaration to specify that the function releases
        all listed locks. This attribute takes zero or more arguments: either
        of lockable type or integers indexing into function parameters of
        lockable type. If no arguments are given, the acquired lock is implicitly
        `this` of the enclosing object. */

#if simd_has_attribute(unlock_function)
  #define simd_unlock_function(...) __attribute__((unlock_function(__VA_ARGS__)))
#else
  #define simd_unlock_function(...)
#endif

/** \def simd_lock_returned(l)

      \details

        Use on a function declaration to
        specify that the function returns lock \p l (\p l must be of lockable
        type). This annotation is used to aid in resolving lock
        expressions. */

#if simd_has_attribute(lock_returned)
  #define simd_lock_returned(__x) __attribute__((lock_returned(__x)))
#else
  #define simd_lock_returned(__x)
#endif

/** \def simd_locks_excluded(...)

      \details

        Use on a function declaration to specify that the function must
        not be called with the listed locks. Arguments must be lockable type,
        and there must be at least one argument. */

#if simd_has_attribute(locks_excluded)
  #define simd_locks_excluded(...) __attribute__((locks_excluded(__VA_ARGS__)))
#else
  #define simd_locks_excluded(...)
#endif

/** \def simd_exclusive_locks_required(...)

      \details

        Use on a function declaration to specify that the function must
        be called while holding the listed exclusive locks. Arguments must
        be lockable type, and there must be at least one argument. */

#if simd_has_attribute(exclusive_locks_required)
  #define simd_exclusive_locks_required(...) __attribute__((exclusive_locks_required(__VA_ARGS__)))
#else
  #define simd_exclusive_locks_required(...)
#endif

/** \def simd_shared_locks_required(...)

      \details

        Use on a function declaration to specify that the function must be
        called while holding the listed shared locks. Arguments must be lockable type,
        and there must be at least one argument. */

#if simd_has_attribute(shared_locks_required)
  #define simd_shared_locks_required(...) __attribute__((shared_locks_required(__VA_ARGS__)))
#else
  #define simd_shared_locks_required(...)
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup handle_attributes Handles
///
///   \brief Handles are a way to identify resources like files, sockets, and processes.
///
///   \details
///
///     They are more opaque than pointers and widely used in system programming. They have similar
///     risks such as never releasing a resource associated with a handle, attempting to use a
///     handle that was already released, or trying to release a handle twice.
///
///     Using the annotations below it is possible to make the ownership of the handles clear:
///     whose responsibility is to release them. They can also aid static analysis tools to find bugs.
///
///   \ingroup attributes
///
/// \{                                                                                             */


/** \def simd_acquire_handle(x)

      \brief indicates this method returns an acquired resource

      \details

        Names its type with a tag.

        If applied to a method it refers to the function result
        If applied to an argument it indicates that argument is an out-parameter providing the value */

#if __has_cpp_attribute(acquire_handle)
  #define simd_acquire_handle(__x) [[clang::acquire_handle(__x)]]
#else
  #define simd_acquire_handle(__x)
#endif

/** \def simd_release_handle(x)

      \brief Indicates this argument is an acquired resource that is being released with a tag */

#if __has_cpp_attribute(release_handle)
  #define simd_release_handle(__x) [[clang::release_handle(__x)]]
#else
  #define simd_release_handle(__x)
#endif

/** \def simd_use_handle(x)

      \brief indicates this argument is a use of a resource, but does not release it. */

#if __has_cpp_attribute(use_handle)
  #define simd_use_handle(__x) [[clang::use_handle(__x)]]
#else
  #define simd_use_handle(__x)
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup alignment_attributes Memory Alignment
///
///   \brief see also #simd_alloc_align
///
///   \ingroup attributes
///
/// \{

/** \def simd_assume_aligned(x)

      \brief Indicates a function returns a pointer with alignment at least \p x */

#if simd_has_attribute(assume_aligned)
  #define simd_assume_aligned(x) __attribute__((assume_aligned(x)))
#else
  #define simd_assume_aligned(x)
#endif

/** \def simd_align_value(x)

      \brief `[[align_value(x)]]`

      \details

        the annotated pointer specified has alignment at least \p x */

#if simd_has_attribute(align_value)
  #define simd_align_value(x) __attribute__((align_value(x)))
#else
  #define simd_align_value(x)
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup allocation_attributes Memory Allocation
///
///   \ingroup attributes
///
///    \sa #simd_allocating, #simd_nonallocating, #simd_noalloc
///
/// \{

/** \def simd_malloc

      \brief `[[malloc]]`

      \details

        Indicates the returned memory does not alias with any other pointer */

#if simd_has_attribute(malloc)
  #define simd_malloc __attribute__((malloc))
#else
  #define simd_malloc
#endif

/** \def simd_alloc_align(N)

      \brief `[[alloc_align(N)]]`

      \details

        Indicates the 1-based argument number of a function that indicates the alignment of the returned result

        See #simd_malloc.

    */

#if simd_has_attribute(alloc_align)
  #define simd_alloc_align(N) __attribute__((alloc_align(N)))
#else
  #define simd_alloc_align(__N)
#endif

/** \def simd_alloc_size(N)

      \brief `[[alloc_size(x)]]`

      \details

        Arg # (1-based) of the attribute that tells you the size of the result in bytes.  */

#if simd_has_attribute(alloc_size)
  #define simd_alloc_size(N) __attribute__((alloc_size(N)))
#else
  #define simd_alloc_size(__N)
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup string_safety_attributes String Safety
///
///   \ingroup attributes
///
/// \{

/** \def simd_null_terminated_string_arg(N)

      \brief The \p N th argumnt is a null terminated string.

      \details

        This may be applied to a function that takes a `char *` or `char const *` at referenced argument \p N.

        It indicates that the passed argument must be a C-style null-terminated string.
        Specifically, the presence of the attribute implies that, if the pointer is non-null, the function may
        scan through the referenced buffer looking for the first zero byte. */

#if simd_has_attribute(null_terminated_string_arg)
  #define simd_null_terminated_string_arg(x) __attribute__((null_terminated_string_arg(x)))
#else
  #define simd_null_terminated_string_arg(x)
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup control_flow_attributes Control Flow
///
///   \details
///
///     `[[assume(x)]]` also moraly belongs among these
///
///   \ingroup attributes
///
/// \{

/** \def simd_callback(...)

      \brief `[[callback(...)]]`

      \details

        Indicates the specified argument will be called back with the
        other named arguments. Complicated, see clang docs.
Allows better interprocedural analysis */

#if simd_has_attribute(callback)
  #define simd_callback(...) __attribute__((callback(__VA_ARGS__)))
#else
  #define simd_callback(...)
#endif

/** \def simd_noreturn

      \brief gcc-style `[[noreturn]]`

      \details

        Indicates the method does not return to the caller.

        <a href="https://www.youtube.com/watch?v=Dh994JcEfkI">A relevant testimonial by the Kingston Trio.</a> */

#if simd_has_attribute(noreturn)
#define simd_noreturn __attribute__((noreturn))
#else
#define simd_noreturn
#endif

/// \}
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup null_safety_attributes Null Safety
///
///   \ingroup attributes
///
/// \{

/** \def simd_returns_nonnull

      \brief `[[returns_nonnnull]]`

      \details

        The result of this function are guaranteed be non-null */

#if simd_has_attribute(returns_nonnull)
  #define simd_returns_nonnull __attribute__((returns_nonnull))
#else
  #define simd_returns_nonnull
#endif

/** \def simd_nonnull(...)

      \brief portable `[[nonnnull(...)]]`

      \details

        Indicates the selected (1-based) indexed arguments to this function
        must be non-null. Passing a null pointer to such an argument is
        undefined behavior. GCC-style. */

#if simd_has_attribute(nonnull)
  #define simd_nonnull(...) __attribute__((nonnull(__VA_ARGS__)))
#else
  #define simd_nonnull(...)
#endif

/** \def simd_Nonnull

      \brief `_Nonnull`

      \details

      can be applied to each * in an data type to indicate that argument should never be null.


    \def simd_Nullable

      \brief `_Nullable`

      \details

      can be applied to each * in an data type to indicate that argument might be null.


    \def simd_Null_unspecified

      \brief `_Null_unspecified`

      \details

      applied to each * in an data type to indicate that the nullability of it is unknown or complicated */

#ifdef __clang__
#define simd_Nonnull _Nonnull
#define simd_Nullable _Nullable
#define simd_Null_unspecified _Null_unspecified
#else
#define simd_Nonnull
#define simd_Nullable
#define simd_Null_unspecified
#endif

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup cuda_attributes CUDA
///
///   \ingroup attributes
///
/// \{

/** \def simd_host

      \brief portable `__host__` for CUDA

      \details

      indicates the function should be available on the host

    \def simd_device

      \brief portable `__device__` for CUDA

      \details

      indicates the function should be available on the device

    \def simd_hd

      \brief applies both \ref simd_host and \ref simd_device

      \details

      most functions in the library should be defined this way

      may eventually add similar annotations for sycl and the like

    \def simd_global

      \brief portable `__global__` for CUDA

      \details

      indicates the function is a global entry point for a compute kernel */

#ifdef __CUDACC__
#define simd_host __host__
#define simd_device __device__
#define simd_global __global__
#else
#define simd_host
#define simd_device
#define simd_global
#endif

#define simd_hd simd_host simd_device

/// \}

///////////////////////////////////////////////////////////////////////////////////////////////
/// \defgroup performance_constraint_attributes Performance Constraints
///
///   \details
///
///      - When assigning or otherwise converting to a function pointer of `nonblocking` or
///        `nonallocating` type, the source must also be a function or function pointer of that
///        type, unless it is a null pointer, i.e. the attributes should not be “spoofed”.
///        Conversions that remove the attributes are transparent and valid.
///
///      - An override of a `nonblocking` or `nonallocating` virtual method must also be declared
///        with that same attribute (or a stronger one.) An overriding method may add an attribute.
///
///      - A redeclaration of a `nonblocking` or `nonallocating` function must also be declared
///        with the same attribute (or a stronger one). A redeclaration may add an attribute.
///
///   \note
///
///     #simd_noalloc(x) and #simd_noblock(x) are necessitated by the inability to write a macro that
///     merely _may_ take an argument.
///
/// \ingroup attributes
///
/// \{

/**
    \def simd_allocating

      \brief Declares a function potentially allocates heap memory. Prevents inference of `nonallocating`.

    \def simd_blocking

      \brief Declares a function potentially blocks. Prevents inference of #simd_nonblocking.

    \def simd_nonblocking

      \brief Declares a function does not block.

    \def simd_noblock(x)

      \brief a `bool` parameterized version of #simd_nonblocking

      \details

        - #simd_noblock(false) behaves like #simd_blocking.

        - #simd_noblock(true) behavs like #simd_nonblocking

    \def simd_nonallocating

      \brief Declares a function does *not* allocate heap memory.

    \def simd_noalloc

      \brief a `bool` parameterized version of #simd_nonallocating

      \details

        - #simd_noalloc(false) behaves like #simd_allocating.

        - #simd_noalloc(true) behavs like #simd_nonallocating */

#if simd_has_attribute(nonblocking)
  #define simd_nonblocking [[clang::nonblocking]]
  #define simd_blocking [[clang::blocking]]
  #define simd_nonallocating [[clang::nonallocating]]
  #define simd_allocating [[clang::allocating]]
  #define simd_noalloc(x) [[clang::nonallocating(x)]]
  #define simd_noblock(x) [[clang::nonblocking(x)]]
#else
  #define simd_nonblocking
  #define simd_blocking
  #define simd_nonallocating
  #define simd_allocating
  #define simd_noalloc(__x)
  #define simd_noblock(__x)
#endif

/// \}

#define simd_constexpr constexpr

#else
// The shared shader declarations carry no host compiler promises.
#define simd_nodiscard
#define simd_constexpr
#define simd_inline inline
#define simd_pure
#define simd_const
#define simd_lifetimebound
#define simd_noescape
#define simd_reinitializes
#define simd_artificial
#define simd_flatten
#endif
