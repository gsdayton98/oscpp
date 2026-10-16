// -*- mode:C++; c-basic-offset:2; indent-tabs-mode:nil -*-
// Copyright 2026 Glen S. Dayton. Rights reserved according to terms of included license.

#ifndef OSCPP_THOUSANDS_SEPARATOR_HPP
#define OSCPP_THOUSANDS_SEPARATOR_HPP

#include <locale>
#include <string>

/**
 * Thousands separators for numbers written to a stream: 1234567 becomes 1,234,567.
 *
 * This is a header-only locale facet. A stream writes numbers according to its locale, and the facet changes only the
 * grouping of digits and the separator; the decimal point and everything else come from the locale it is added to. It
 * affects only the stream that is given the locale. Use it by imbuing a stream:
 *
 * @code
 * std::cout.imbue(oscpp::withThousandsSeparator(std::cout.getloc()));
 * std::cout << 1234567 << "\n";   // 1,234,567
 * @endcode
 *
 * @author Glen Dayton
 */
namespace oscpp {

/**
 * A `std::numpunct<char>` facet that groups the digits left of the decimal point and separates the groups with a
 * character. It is usually made through withThousandsSeparator() rather than constructed directly.
 */
class ThousandsSeparator final : public std::numpunct<char> {
 public:
  /**
   * @param separator  The character between groups of digits.
   * @param groupSize  How many digits in each group, counted from the decimal point. Zero means no grouping.
   * @param refs       Passed to `std::numpunct`. Leave it at 0 when the facet is given to a `std::locale`, which then
   *                   owns it and destroys it when the last copy of the locale goes away.
   */
  explicit ThousandsSeparator(char separator = ',', char groupSize = 3, std::size_t refs = 0)
      : std::numpunct<char>(refs), separator_(separator), grouping_(groupSize == 0 ? "" : std::string(1, groupSize)) {}

 protected:
  /// The character between groups of digits.
  [[nodiscard]] char_type do_thousands_sep() const override { return separator_; }

  /// The size of each group of digits, as one character holding the count: "\3" is groups of three, repeated.
  [[nodiscard]] string_type do_grouping() const override { return grouping_; }

 private:
  char separator_;
  std::string grouping_;
};

/**
 * Make a locale that is `base` with digits grouped and separated. Imbue it in a stream to write its numbers with
 * separators.
 *
 * @param base       The locale to start from, for example the stream's own, `stream.getloc()`.
 * @param separator  The character between groups of digits.
 * @param groupSize  How many digits in each group. Zero means no grouping.
 * @return A locale that owns the facet, so there is nothing to delete.
 */
[[nodiscard]] inline auto withThousandsSeparator(const std::locale &base, char separator = ',', char groupSize = 3)
    -> std::locale {
  // The locale takes ownership of the facet (its reference count is 0).
  return {base, new ThousandsSeparator(separator, groupSize)}; // NOLINT(cppcoreguidelines-owning-memory)
}

} // namespace oscpp
#endif // OSCPP_THOUSANDS_SEPARATOR_HPP
