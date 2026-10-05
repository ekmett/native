# x86 WAITPKG/MWAITX: waiting and spin loops

[x86 instruction sets](x86.md)

## Why use it

A busy loop consumes execution resources while it waits for another thread.
`PAUSE` gives the processor a hint; monitored waits can suspend until there is
activity near an address, and `TPAUSE` can wait for a TSC deadline without
monitoring memory. These are useful building blocks for short waits where a
full scheduler handoff would cost too much.

## Operations

Import `native.x86.wait` and link `native::minimal`. The x86 and main hubs also
export the module.

| Operation | Behavior |
| --- | --- |
| `spin::mwait()` | Issue one `PAUSE`; ignore the timer argument |
| `mwaitx::monitor(p)` | Arm `MONITORX` for the address |
| `mwaitx::mwait(timer)` | Issue `MWAITX` with a 32-bit timer operand |
| `umwait::monitor(p)` | Arm `UMONITOR` for the address |
| `umwait::mwait(deadline)` | Issue `UMWAIT` with an absolute 64-bit TSC deadline; return its status byte |
| `tpause<Arch, Control>(deadline)` | Wait on an absolute 64-bit TSC deadline without arming a monitor; return its status byte |
| `wait_until<W>(p, predicate)` | Check the predicate, arm the monitor, check again, and wait; repeat until the predicate holds |
| `with_waiter(callback)` | Invoke the callback with a supported waiter type: prefer `mwaitx`, then `umwait`, then `spin` |

`mwaitx::supported` and `umwait::supported` report admission of their respective
features. `spin::supported` is true. `spin::monitor(p)` does nothing.

```cpp
native::with_waiter([&]<native::waiter W> {
  native::wait_until<W>(p, predicate);
});
```

## Caveats

These operations provide no memory ordering. The predicate must perform the
atomic loads and synchronization your algorithm needs; `volatile` is not
inter-thread synchronization. Wakeups may be spurious, so always recheck the
condition. The OS can limit a wait, and monitor activity need not mean that the
particular value you care about has changed.

MWAITX and WAITPKG are independent features and require no SIMD register state.
Check `supported` before using a waiter directly. The MWAITX timer is not a TSC
deadline. The wrapper passes zero for its extension flags, so passing a nonzero
timer alone does not enable a timed wait.

For `tpause`, `Arch` defaults to the module's compiler baseline and must contain
`x86_feature::waitpkg`; the caller must enable the `waitpkg` target and admit it
before execution. `Control=0` permits C0.2; the default `Control=1` requests
C0.1. Waits have no constant-evaluation substitute.

<!-- SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 -->
