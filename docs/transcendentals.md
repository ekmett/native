# Math kernels

The promoted `math` API accepts a binary32 scalar, `simd`, an array or a `wide`
pack and preserves that shape. A pack advances all independent registers through
each stage. Constants are shared SIMD values, not packs of repeated constants.
Math kernels may contain multiple instructions; primitive instruction wrappers
retain their separate instruction-level contract.

Accuracy and cross-platform agreement are separate checks. Native approximations
follow the caller's floating-point environment and use separately rounded
multiply/add on baseline Wasm. FTZ may choose the same polynomial where useful,
but its admitted implementations must share one result graph, except for NaN
payloads. A sampled accuracy result does not establish bit-for-bit agreement.

## Polynomials

`math::horner(c0, c1, ...)(z)` evaluates a polynomial from highest power to
constant term: `horner(2.f, 3.f, 4.f)(z)` computes `(2*z + 3)*z + 4`.
The returned callable owns its coefficients and can be reused at different input
shapes when its coefficient types permit them.
It preserves the scalar, SIMD, array or `wide` shape of `z`. Coefficients are
`float` values or SIMD values matching the pack's register type, shared across
all registers, or arrays and `wide` packs of those values matching `z`'s extent.
Packed coefficients can vary by register as well as by lane, as when a mask
selects coefficients for different intervals. Packed `float` coefficients
broadcast within each corresponding SIMD register. Array and `wide` coefficient
containers can be mixed; the result keeps `z`'s shape. Packed coefficients require
a packed input, and mismatched extents or SIMD types are rejected.
At least one coefficient is required; a single coefficient
returns a constant polynomial without evaluating an arithmetic operation on `z`.
Each additional coefficient adds one multiply-add stage across the pack, fused
where available and separately rounded on baseline Wasm SIMD. Leading zeros remain
part of the evaluation, including their behavior with infinities and NaNs.
The same helper is available as `wide::horner` through `import native.math;`.

`math::exp<Flush = false, Degree = 6>(x)` uses nearest-even range reduction,
two split-logarithm multiply-adds, and a Horner polynomial with `Degree` stages.
Degrees one through seven are available; six is the default. The SIMD and
`native::wide<simd<...>, N>` entry points accept the same options, for example
`native::exp<false, 5>(vectors)`. Invalid degrees are rejected at compile time.
Changing degree selects only the polynomial: reduction, range masks, exponent
scaling and `Flush` behavior are shared, with no runtime degree selection.

Every polynomial has constant coefficient one, so `exp(0)` remains exactly one.
Degrees six and seven also have linear coefficient one. Degree one is piecewise
affine after reconstruction, not rational; neighboring fitted pieces need not
join continuously.

On the same 4,969,601 sampled normal-output inputs, MPFR256 comparisons give:

| Degree | Maximum float steps from correctly rounded result | Approximate maximum relative error | Nominal join, ppm |
| --- | ---: | ---: | ---: |
| 1 | 678,492 | 5.72% | +1.714 |
| 2 | 27,525 | 0.1964% | +3,934 |
| 3 | 1,345 | 0.01014% | +0.0164 |
| 4 | 43 | 2.91 parts per million | +5.640 |
| 5 | 2 | 0.186 parts per million | +0.000466 |
| 6 | 1 | 0.0981 parts per million | +0.00620 |
| 7 | 1 | 0.0973 parts per million | −0.00111 |

The table takes the worse of fused and separately rounded evaluation. These
are sampled results, not bounds over every input or subnormal output. The join
column measures `2*P(-ln(2)/2)/P(ln(2)/2)-1` for the real coefficient polynomial,
excluding operation rounding and the spacing between adjacent float inputs.
Positive values jump upward; continuity is not enforced. Lower degrees save
polynomial stages at the cost of the accuracy shown above.

`exp2` and `log2` use direct base-two reductions, with no conversion through
natural exponential or logarithm. Normal integer powers of two are exact in
both directions. `exp2` deliberately
returns infinity starting at 127.5 and shares exp's backend-specific treatment
of subnormal outputs; `log2` treats subnormal inputs as signed zero.

The native `tanh` graph performs fourteen coefficient-table lookups and twelve
polynomial FMAs per register, plus range reduction and result classification.
The native `atan2` graph uses one packed divide and eight polynomial FMAs, plus
two multiplies, input classification and quadrant reconstruction. These are
multi-instruction kernels. Baseline Wasm replaces each polynomial FMA with a
separately rounded multiply and add.

Wide evaluation exposes independent arithmetic chains, but larger packs can
spill registers. More registers do not imply proportionally better throughput.

## Register-count recommendations

Each implemented transcendental has a `native::name_width<T,K,A>` variable
template, also exposed through `math` and `native::math`. Use its value as the
extent of `wide<simd<T,K,A>, N>` or a register array. The
[value guide](modules.md#choosing-a-register-count) gives the complete default
table and usage. Recommendations use the explicit ISA and logical lane count;
they neither observe the running CPU nor enable instructions.

The defaults reflect each kernel's register pressure. `exp` recommends six
registers for full-width AVX2 and AVX-512 vectors and four for NEON; `atan2`
recommends two. Larger `atan2` packs keep too many classification and quadrant
temporaries live at once. Short vectors and Wasm use conservative defaults.

These are starting points. Measure the complete loop: surrounding live values,
memory traffic and compiler scheduling can change the best batch size.
