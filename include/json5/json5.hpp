// MIT License

// Copyright (c) 2021 Michael Poddubny

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include <iostream>

namespace json5 {

// https://semver.org/
namespace version {
    static constexpr auto major = 1;
    static constexpr auto minor = 0;
    static constexpr auto patch = 0;
} // namespace version

namespace syntax_error {
    enum error_code {};
}

namespace detail {
    template <class> inline constexpr bool always_false_v = false;

    // https://spec.json5.org/#escapes
    inline auto is_escape(const char* p) -> bool {
        if (*p == '\\') {
            switch (*(p + 1)) {
            case 'b':
                [[fallthrough]];
            case 'f':
                [[fallthrough]];
            case 'n':
                [[fallthrough]];
            case 'r':
                [[fallthrough]];
            case 't':
                [[fallthrough]];
            case 'v':
                return true;
            }
        }

        return false;
    }
} // namespace detail

///
/// JSON5 value
///
template <template <typename... Args> typename VariantType = std::variant,
    template <typename U, typename V, typename... Args> typename ObjectType = std::map,
    template <typename U, typename... Args> typename DynArrayType = std::vector, typename StringType = std::string,
    typename StringViewType = std::string_view, typename NumberIntType = std::int64_t, typename NumberFloatType = double>
class basic_json_value {
public:
    using value_type = basic_json_value;
    using size_type = std::size_t;

    struct null_type { };
    using string_type = StringType;
    using string_view_type = StringViewType;
    using object_type = ObjectType<StringType, value_type>;
    using boolean_type = bool;
    using number_type = NumberFloatType;
    using int_type = NumberIntType;
    using array_type = DynArrayType<value_type>;
    using json_value = VariantType<null_type, boolean_type, string_type, number_type, int_type, object_type, array_type>;

    // ctor

    basic_json_value() = default;

    basic_json_value(null_type) {
    }

    basic_json_value(boolean_type val)
        : _value { val } {
    }

    basic_json_value(string_view_type val)
        : _value { string_type { val } } {
    }

    basic_json_value(string_type val)
        : _value { val } {
    }

    basic_json_value(int_type val)
        : _value { val } {
    }

    basic_json_value(number_type val)
        : _value { val } {
    }

    basic_json_value(object_type val)
        : _value { val } {
    }

    basic_json_value(const array_type& val)
        : _value { val } {
    }

    /// object inspection

    constexpr bool is_null() const noexcept {
        return std::holds_alternative<null_type>(_value);
    }

    constexpr bool is_boolean() const noexcept {
        return std::holds_alternative<boolean_type>(_value);
    }

    constexpr bool is_number_integer() const noexcept {
        return std::holds_alternative<int_type>(_value);
    }

    constexpr bool is_number() const noexcept {
        return std::holds_alternative<number_type>(_value);
    }

    constexpr bool is_string() const noexcept {
        return std::holds_alternative<string_type>(_value);
    }

    constexpr bool is_object() const noexcept {
        return std::holds_alternative<object_type>(_value);
    }

    constexpr bool is_array() const noexcept {
        return std::holds_alternative<array_type>(_value);
    }

    /// value access

    template <typename T> constexpr auto get() -> T const {
        if constexpr (std::is_same_v<T, boolean_type>) {
            return static_cast<T>(std::get<boolean_type>(_value));
        } else if constexpr (std::is_integral_v<T>) {
            return static_cast<T>(std::get<int_type>(_value));
        } else if constexpr (std::is_floating_point_v<T>) {
            return static_cast<T>(std::get<number_type>(_value));
        } else if constexpr (std::is_same_v<T, string_type>) {
            return std::get<string_type>(_value);
        } else if constexpr (std::is_same_v<T, string_view_type>) {
            return std::get<string_type>(_value);
        } else {
            static_assert(detail::always_false_v<T>, "unsupported type!");
        }
    }

    template <typename T> constexpr auto value_or(T&& default_value) const -> T {
        if constexpr (std::is_same_v<T, boolean_type>) {
            if (std::holds_alternative<boolean_type>(_value)) {
                return static_cast<T>(std::get<boolean_type>(_value));
            } else {
                return std::move(default_value);
            }
        } else if constexpr (std::is_integral_v<T>) {
            if (std::holds_alternative<int_type>(_value)) {
                return static_cast<T>(std::get<int_type>(_value));
            } else {
                return std::move(default_value);
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            if (std::holds_alternative<number_type>(_value)) {
                return static_cast<T>(std::get<number_type>(_value));
            } else {
                return std::move(default_value);
            }
        } else if constexpr (std::is_same_v<T, string_type>) {
            if (std::holds_alternative<string_type>(_value)) {
                return std::get<string_type>(_value);
            } else {
                return std::move(default_value);
            }
        } else if constexpr (std::is_same_v<T, string_view_type>) {
            if (std::holds_alternative<string_type>(_value)) {
                return std::get<string_type>(_value);
            } else {
                return std::move(default_value);
            }
        }

        return std::move(default_value);
    }

    /// element access

    auto at(size_type idx) {
        if (std::holds_alternative<array_type>(_value)) {
            return std::get<array_type>(_value)[idx];
        } else if (std::holds_alternative<object_type>(_value)) {
            auto it = std::get<object_type>(_value).begin();
            std::advance(it, idx);
            return it->second;
        }

        return basic_json_value { null_type {} };
    }

    auto at(size_type idx) const {
        if (std::holds_alternative<array_type>(_value)) {
            return std::get<array_type>(_value)[idx];
        } else if (std::holds_alternative<object_type>(_value)) {
            auto it = std::get<object_type>(_value).begin();
            std::advance(it, idx);
            return it->second;
        }

        return basic_json_value { null_type {} };
    }

    auto at_opt(size_type idx) {
        if (std::holds_alternative<array_type>(_value)) {
            auto& arr = std::get<array_type>(_value);
            if (idx < arr.size()) {
                return arr[idx];
            }
        }

        return basic_json_value { null_type {} };
    }

    auto at(const typename object_type::key_type& key) {
        if (std::holds_alternative<object_type>(_value)) {
            return std::get<object_type>(_value).at(key);
        }

        return basic_json_value { null_type {} };
    }

    auto at(const typename object_type::key_type& key) const {
        if (std::holds_alternative<object_type>(_value)) {
            return std::get<object_type>(_value).at(key);
        }

        return basic_json_value { null_type {} };
    }

    auto operator[](size_type idx) {
        return at(idx);
    }

    auto operator[](size_type idx) const {
        return at(idx);
    }

    auto operator[](const typename object_type::key_type& key) {
        return at(key);
    }

    auto operator[](const typename object_type::key_type& key) const {
        return at(key);
    }

    size_type size() const {
        if (std::holds_alternative<array_type>(_value)) {
            return std::get<array_type>(_value).size();
        } else if (std::holds_alternative<object_type>(_value)) {
            return std::get<object_type>(_value).size();
        }

        return 0;
    }

    /// dump
    string_type dump() {
        string_type s;
        return s;
    }

    /// value
    json_value _value;

    /// parser
    static auto skip_spaces_and_comments(const char** p) {
        do {
            if (!isspace(**p)) {
                break;
            }

            (*p)++;

        } while (**p);
    }

    static auto parse_string(const char** p, value_type& value) {
        string_type res;
        const auto quote = **p;
        auto b = *p + 1;
        auto e = b;
        while (*e) {
            if (*e == '\\' && *(e + 1) == quote) {
                e += 2;
                continue;
            } else if (detail::is_escape(e)) {
                res += *e;
                e++;
                continue;
            } else if (*e == quote) {
                break;
            }
            res += *e;
            ++e;
        }
        value = res;
        *p = e + 1;
    }

    static auto parse_array(const char** p, value_type& value) {
        value = array_type {};

        (*p)++;

        while (true) {
            skip_spaces_and_comments(p);

            if (**p == ',') {
                (*p)++;
                continue;
            }

            if (**p == ']') {
                (*p)++;
                break;
            }

            auto& arr = std::get<array_type>(value._value);
            arr.emplace_back(null_type {});
            parse_value(p, arr.back());
        }
    }

    static auto parse_boolean(const char** p, value_type& value) {
        if (strncmp(*p, "true", 4) == 0) {
            value = true;
            *p += 4;
        } else if (strncmp(*p, "false", 5) == 0) {
            value = false;
            *p += 5;
        }
    }

    static auto parse_null(const char** p, value_type& value) {
        if (strncmp(*p, "null", 4) == 0) {
            *p += 4;
            value = null_type {};
        }
    }

    static auto parse_number(const char** p, value_type& value) {
        if (strncmp(*p, "NaN", 3) == 0) {
            *p += 3;
            value = std::numeric_limits<number_type>::quiet_NaN();
        } else if (strncmp(*p, "+NaN", 4) == 0) {
            *p += 4;
            value = std::numeric_limits<number_type>::quiet_NaN();
        } else if (strncmp(*p, "-NaN", 4) == 0) {
            *p += 4;
            value = -std::numeric_limits<number_type>::quiet_NaN();
        } else if (strncmp(*p, "Infinity", 8) == 0) {
            *p += 8;
            value = std::numeric_limits<number_type>::infinity();
        } else if (strncmp(*p, "+Infinity", 9) == 0) {
            *p += 9;
            value = +std::numeric_limits<number_type>::infinity();
        } else if (strncmp(*p, "-Infinity", 9) == 0) {
            *p += 9;
            value = -std::numeric_limits<number_type>::infinity();
        } else {
            int base = 10;
            auto is_float_point = false;
            auto start = *p;
            if ((**p == '-') || (**p == '+') || (**p == '.') || isdigit(**p)) {
                do {
                    if (**p == '.') {
                        is_float_point = true;
                    }
                    if ((**p == 'x') || (**p == 'X')) {
                        base = 16;
                    }
                    (*p)++;
                } while (isdigit(**p) || isxdigit(**p) || (**p == '.') || (**p == 'x'));
            }
            char* end;
            if (is_float_point) {
                value = static_cast<number_type>(strtod(start, &end));
            } else {
                value = static_cast<int_type>(strtoll(start, &end, base));
            }
        }
    }

    static auto parse_key(const char** p) {
        if (isalpha(**p) || (**p == '_') || **p == '$') {
            auto b = *p;
            do {
                (*p)++;
            } while (**p && (**p == '_' || isalpha(**p) || isdigit(**p)));

            if (**p == ':') {
                return string_type { b, *p };
            } else {
                while (**p) {
                    if (!isspace(**p) || **p == ':') {
                        break;
                    }
                    (*p)++;
                }

                return string_type { b, *p };
            }
        }

        return string_type {};
    }

    static auto parse_object(const char** p, value_type& value) {
        value = object_type {};

        while (true) {
            skip_spaces_and_comments(p);
            if (**p == '}') {
                (*p)++;
                break;
            }

            const auto key = parse_key(p);

            (*p)++;

            if (!std::empty(key)) {
                auto& obj = std::get<object_type>(value._value);
                auto [it, success] = obj.emplace(key, null_type {});
                if (success) {
                    parse_value(p, it->second);
                }
            }
        }
    }

    static auto parse_value(const char** p, value_type& value) -> void {
        skip_spaces_and_comments(p);
        const auto ch = **p;

        switch (ch) {
        case '{':
            parse_object(p, value);
            break;
        case '[':
            parse_array(p, value);
            break;
        case '"':
        case '\'':
            parse_string(p, value);
            break;
        case 'n':
            parse_null(p, value);
            break;
        case 't':
        case 'f':
            parse_boolean(p, value);
            break;
        default:
            if (isdigit(ch) || (ch == '-') || (ch == '+') || (ch == '.') || (ch == 'I') || (ch == 'N')) {
                parse_number(p, value);
            }
        }
    }

    static auto parse(string_view_type str) -> basic_json_value {
        if (str.empty()) {
            return value_type {};
        }

        const char* p = std::data(str);

        value_type val;
        parse_value(&p, val);
        return val;
    }
}; // namespace json5

using value = basic_json_value<std::variant, std::map, std::vector, std::string, std::string_view, std::int64_t, double>;

} // namespace json5
