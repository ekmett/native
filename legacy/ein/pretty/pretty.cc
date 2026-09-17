
#include "pretty.h"

// output

/// \cond
template constexpr void newline_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
template constexpr void newline_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, newline_t) noexcept;
template constexpr void spaces::pp<false>(this spaces, ein_noescape printer & ein_restrict) noexcept(true);
template constexpr void spaces::pp<true>(this spaces, ein_noescape printer & ein_restrict) noexcept(false);
template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, spaces) noexcept;
template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, seq) noexcept;
template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, out) noexcept;
template constexpr void hardline_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
template constexpr void hardline_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr void hv_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr void hv_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
template constexpr void h_tight_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr void h_tight_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
template constexpr void hv_tight_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr void hv_tight_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
/// \endcond

constexpr newline_t newline;
constexpr spaces space{1};
constexpr hardline_t hardline;
constexpr empty_t empty;
constexpr hv_t hv;
constexpr h_tight_t h_tight;
constexpr hv_tight_t hv_tight;

