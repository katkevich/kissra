#pragma once
#include "kissra/impl/export.hpp"
#include "kissra/misc/static_string.hpp"
#include "kissra/misc/utility.hpp"

#ifndef KISSRA_MODULE
#include <cmath>
#include <cstddef>
#include <format>
#include <string>
#endif

KISSRA_EXPORT()
namespace kissra {
namespace functor {
template <kissra::static_string FmtString>
struct fmt_c_t {
    template <typename T>
    static constexpr std::string operator()(T&& x) {
        return std::format(std::string_view{ FmtString.str.data(), FmtString.str.size() - 1 }, std::forward<T>(x));
    }
};

struct fmt_t {
    template <typename T>
    constexpr std::string operator()(T&& x) const {
        return std::vformat(fmt, std::make_format_args(KISSRA_FWD(x)));
    }

    std::string fmt;
};

struct fmt_builder_t {
    constexpr auto operator()(const std::string& fmt) const {
        return fmt_t{ fmt };
    }

    constexpr auto operator()(std::string&& fmt) const {
        return fmt_t{ std::move(fmt) };
    }
};

template <int Base = 10>
struct to_chars_c_t {
    template <std::integral T>
    static constexpr std::string operator()(T x) noexcept {
        /**
         * https://math.stackexchange.com/questions/1771457/how-to-determine-the-number-of-digits-needed-to-represent-a-number-in-different
         *
         * DIGITS(int_N, base) = floor[LOG_base(N) + 1] = floor[LOG_2(N) / LOG_2(base) + 1]
         * example:
         * DIGITS(uint_64, 16)  = floor[LOG_16(2^64) + 1]  = floor[LOG_2(2^64) / LOG_2(16) + 1] = 64 / 4 + 1 = 17 [FFFF'FFFF'FFFF'FFFF]
         */

        // TODO: std::log2 isn't constexpr YET
        // constexpr std::size_t digits_count = std::numeric_limits<std::make_unsigned_t<T>>::digits / std::log2(Base) + 1;

        constexpr std::size_t digits_count = Base == 2 ? std::numeric_limits<std::make_unsigned_t<T>>::digits + 1 :
            Base == 10                                 ? std::numeric_limits<std::make_unsigned_t<T>>::digits10 + 1 :
            Base == 16                                 ? std::numeric_limits<std::make_unsigned_t<T>>::digits / 4 + 1 :
                                                         128 + 1;
        std::array<char, digits_count> buffer;

        std::to_chars_result result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), x, Base);
        return std::string(buffer.data(), result.ptr);
    }
};

/**
 * It is impossible to tell in advance how big buffer should be (in case of a fixed format). It could be huge.
 * 64 bytes estimate should be enough in most cases. If not, then pass bigger buffer size manually.
 * Precision being equal to '-1' means no particular precision was specified - the shortest FP representation will be chosen instead.
 */
template <int Precision = -1, std::chars_format CharsFormat = std::chars_format::general, int BufferSize = 64>
struct to_chars_fp_c_t {
    template <std::floating_point T>
    static std::string operator()(T x) noexcept {
        std::array<char, BufferSize> buffer;

        /* "Precision == -1" === shortest representation */
        if constexpr (Precision == -1) {
            std::to_chars_result result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), x, CharsFormat);
            return std::string(buffer.data(), result.ptr);
        } else {
            std::to_chars_result result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), x, CharsFormat, Precision);
            return std::string(buffer.data(), result.ptr);
        }
    }
};
} // namespace functor

namespace fn {
/* Format value using std::format(...) with comptime format string. */
template <kissra::static_string FmtString>
constexpr kissra::functor::fmt_c_t<FmtString> format_c;
/* Format value using std::vformat(...) with runtime format string.*/
constexpr kissra::functor::fmt_builder_t format;

/* Convert integral value into std::string using std::to_chars (Base 10) */
constexpr kissra::functor::to_chars_c_t<10> to_chars;
/* Convert integral value into std::string using std::to_chars (Base 2). Note: no leading '0b'. */
constexpr kissra::functor::to_chars_c_t<2> to_chars_bin;
/* Convert integral value into std::string using std::to_chars (Base 8). Note: no leading '0' */
constexpr kissra::functor::to_chars_c_t<8> to_chars_oct;
/* Convert integral value into std::string using std::to_chars (Base 16). Note: no leading '0x' */
constexpr kissra::functor::to_chars_c_t<16> to_chars_hex;
/* Convert integral value into std::string using std::to_chars in specified 'Base'. */
template <int Base>
constexpr kissra::functor::to_chars_c_t<Base> to_chars_c;

/* Convert floating-point value into std::string with specified precision using std::to_chars. */
template <int Precision = 2, std::chars_format CharsFormat = std::chars_format::general, int BufferSize = 64>
constexpr kissra::functor::to_chars_fp_c_t<Precision, CharsFormat, BufferSize> to_chars_fp;

/* Convert floating-point value into std::string (shortest form) using std::to_chars. */
template <std::chars_format CharsFormat = std::chars_format::general, int BufferSize = 64>
constexpr kissra::functor::to_chars_fp_c_t<-1, CharsFormat, BufferSize> to_chars_fp_shortest;
} // namespace fn
} // namespace kissra