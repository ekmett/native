# Transcendental kernels

The promoted `math` API accepts a binary32 scalar, `simd`, an array or a `wide`
pack and preserves that shape. A pack advances all independent registers through
each stage. Constants are shared SIMD values, not packs of repeated constants.
Math kernels may contain multiple instructions; primitive instruction wrappers
retain their separate instruction-level contract.

Accuracy and cross-platform agreement are separate checks. Native approximations
follow the caller's floating-point environment and use separately rounded
multiply/add on baseline Wasm. FTZ may choose the same polynomial where useful,
but its admitted implementations must share one result graph, except for NaN
payloads. Neither a matching packet nor a sampled ULP bound proves the other.

## Current implementation cost

The native `tanh` graph performs fourteen coefficient-table lookups and twelve
polynomial FMAs per register, plus range reduction and result classification.
The native `atan2` graph uses one packed divide and eight polynomial FMAs, plus
two multiplies, input classification and quadrant reconstruction. These are
multi-instruction kernels. Baseline Wasm replaces each polynomial FMA with a
separately rounded multiply and add.

Packed division was selected over three normalized reciprocal Newton refinements
using accuracy, assembly and throughput evidence. The optional
[atan2 benchmark and results](../tests/transcendentals/README.md) compare one to
eight registers on Apple ARM and AVX2. Wide evaluation exposes independent
arithmetic chains, but larger packs cause substantial register spills; more
registers do not imply proportionally better throughput. The benchmark retains
those costs and its sampling limits separately from numerical qualification.

## Candidates after tanh and atan2

These are proposed additions, not available entry points.

| Priority | Operations | Why they are useful | Main work |
| --- | --- | --- | --- |
| First | `exp2`, `log2` | Natural partners for binary exponent reconstruction and mantissa reduction. | Fit or scale the reduced polynomial directly; avoid wrappers that unnecessarily convert through natural-log units. Check integer powers of two and range endpoints. |
| First | `atan` | Can reuse the reduced atan polynomial without atan2's two-input axis handling. | Reciprocal reduction for large magnitudes; signed zero, infinities and the tiny interval. |
| Next | `sinh`, `cosh`, paired `sinhcosh` | Share exponential work when both are needed. | Use a cancellation-safe small-input form; choose their own overflow range instead of inheriting exp's cutoff accidentally. |
| Next | `sigmoid`, `softplus` | Common stable compositions of exp and log1p. | Work with `exp(-abs(x))` to avoid positive overflow, retain useful tiny corrections and measure the cost of each divide or selection. |
| Later | `asinh`, `acosh`, `atanh` | Complete the useful inverse hyperbolic set. | Stable log1p-based reductions near zero/one and explicit domains, with no avoidable overflow in intermediate squares. |
| Later | `erf`, `erfc` | Useful for Gaussian probabilities and smooth transitions. | Separate central and tail approximations; `erfc` must not lose its tail by subtracting `erf` from one. |
| Separate design | general `pow` | Widely used, but has a large exceptional-value surface. | Negative bases, integer-exponent admission, signed zero, overflow and cancellation; not simply `exp(y*log(x))`. |

`log10` is straightforward to offer once logarithmic reconstruction is factored
appropriately, but adds less new machinery than the first group. Dedicated
integer-power helpers should precede a general real `pow` when that is the actual
application need.

## Acceptance checks

Use MPFR where available for sampled mathematical accuracy, and retain a small
fixed regression bank without a mandatory external dependency. Record worst
inputs and distinguish ULP distance from absolute error near zero. Check dense
windows at reduction boundaries, signed zeros, finite extremes and nonfinite
inputs. Require constant evaluation to follow the selected backend's fused or
nonfused graph.

Inspect full-register and wide output for scalarized comparisons, scalar lane
loops, libm calls and spills. Benchmark several register counts on Apple ARM and
x86 before selecting a schedule. Hardware division versus reciprocal refinement
is a measured choice, not an assumption that fewer source operations are faster.
Baseline Wasm needs its own accuracy and bytecode checks; native SIMD instructions
in bytecode do not by themselves establish the engine's machine-code performance.

The FTZ implementation and its cross-platform packet checks remain the reference
for arithmetic-policy work. It need not freeze historical output words: a changed
graph is acceptable when every admitted platform implements the same contract.
