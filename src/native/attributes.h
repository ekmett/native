// SPDX-FileCopyrightText: 2024, 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

/** \file
    \brief Compatibility names for the Hint compiler attribute catalog.

    New code can include <hint.h> and use hint_* directly. These aliases keep
    Native and existing consumers on the same implementation. Attribute
    contracts and compiler feature detection are documented in Hint. */

#if defined(NATIVE_USE_DECLSPEC) && !defined(HINT_USE_DECLSPEC)
#define HINT_USE_DECLSPEC
#endif
#include <hint.h>

#ifdef __cplusplus
#define native_has_attribute hint_has_attribute
#define native_diagnose_if hint_diagnose_if
#define native_has_declspec_attribute hint_has_declspec_attribute
#define native_empty_bases hint_empty_bases
#define native_const hint_const
#define native_pure hint_pure
#define native_lifetimebound hint_lifetimebound
#define native_noescape hint_noescape
#define native_inline hint_inline
#define native_flatten hint_flatten
#define native_artificial hint_artificial
#define native_noinline hint_noinline
#define native_optnone hint_optnone
#define native_weak hint_weak
#define native_internal_linkage hint_internal_linkage
#define native_preferred_name hint_preferred_name
#define native_visibility hint_visibility
#define native_exclude_from_explicit_instantiation hint_exclude_from_explicit_instantiation
#define native_hidden hint_hidden
#define native_constinit hint_constinit
#define native_uninitialized hint_uninitialized
#define native_reinitializes hint_reinitializes
#define native_target hint_target
#define native_target_clones hint_target_clones
#define native_hot hint_hot
#define native_cold hint_cold
#define native_consumable hint_consumable
#define native_callable_when hint_callable_when
#define native_param_typestate hint_param_typestate
#define native_return_typestate hint_return_typestate
#define native_moving hint_moving
#define native_set_typestate hint_set_typestate
#define native_test_typestate hint_test_typestate
#define native_no_thread_safety_analysis hint_no_thread_safety_analysis
#define native_lockable hint_lockable
#define native_scoped_lockable hint_scoped_lockable
#define native_guarded_var hint_guarded_var
#define native_pt_guarded_var hint_pt_guarded_var
#define native_guarded_by hint_guarded_by
#define native_pt_guarded_by hint_pt_guarded_by
#define native_acquired_before hint_acquired_before
#define native_acquired_after hint_acquired_after
#define native_exclusive_lock_function hint_exclusive_lock_function
#define native_shared_trylock_function hint_shared_trylock_function
#define native_unlock_function hint_unlock_function
#define native_lock_returned hint_lock_returned
#define native_locks_excluded hint_locks_excluded
#define native_exclusive_locks_required hint_exclusive_locks_required
#define native_shared_locks_required hint_shared_locks_required
#define native_acquire_handle hint_acquire_handle
#define native_release_handle hint_release_handle
#define native_use_handle hint_use_handle
#define native_align hint_align
#define native_assume_aligned hint_assume_aligned
#define native_align_value hint_align_value
#define native_malloc hint_malloc
#define native_alloc_align hint_alloc_align
#define native_alloc_size hint_alloc_size
#define native_null_terminated_string_arg hint_null_terminated_string_arg
#define native_callback hint_callback
#define native_noreturn hint_noreturn
#define native_returns_nonnull hint_returns_nonnull
#define native_nonnull hint_nonnull
#define native_Nonnull hint_Nonnull
#define native_Nullable hint_Nullable
#define native_Null_unspecified hint_Null_unspecified
#define native_host hint_host
#define native_device hint_device
#define native_global hint_global
#define native_hd hint_hd
#define native_nonblocking hint_nonblocking
#define native_blocking hint_blocking
#define native_nonallocating hint_nonallocating
#define native_allocating hint_allocating
#define native_noalloc hint_noalloc
#define native_noblock hint_noblock
#define native_constexpr hint_constexpr
#else
#define native_constexpr hint_constexpr
#define native_inline hint_inline
#define native_pure hint_pure
#define native_const hint_const
#define native_lifetimebound hint_lifetimebound
#define native_noescape hint_noescape
#define native_reinitializes hint_reinitializes
#define native_artificial hint_artificial
#define native_flatten hint_flatten
#endif
