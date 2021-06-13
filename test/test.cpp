#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cmath>

#include <json5/json5.hpp>

TEST_CASE("JSON5_Parser_spaces") {
    SECTION("Skip spaces") {
        const char* s = "  asd";
        json5::value::skip_spaces_and_comments(&s);
        REQUIRE(*s == 'a');
    }
    SECTION("Skip single line comment") {
        const char* s = "//zxc\na";
        json5::value::skip_spaces_and_comments(&s);
        REQUIRE(*s == 'a');
    }
    SECTION("Skip single line comment and spaces") {
        const char* s = "//zxc\n  a";
        json5::value::skip_spaces_and_comments(&s);
        REQUIRE(*s == 'a');
    }
    SECTION("Skip multi line comment") {
        const char* s = "/*qwe\nasd\nzxc*/a";
        json5::value::skip_spaces_and_comments(&s);
        REQUIRE(*s == 'a');
    }
    SECTION("Skip multi line comment with *") {
        const char* s = "/*qwe\nas*d\nzxc*/a";
        json5::value::skip_spaces_and_comments(&s);
        REQUIRE(*s == 'a');
    }
    SECTION("Skip multi line comment and spaces") {
        const char* s = "/*qwe\nasd\nzxc*/  a";
        json5::value::skip_spaces_and_comments(&s);
        REQUIRE(*s == 'a');
    }
}

TEST_CASE("JSON5_Parser_string") {
    SECTION("Single quote") {
        const char* s = "'ab'";
        json5::value val;
        json5::value::parse_string(&s, val);
        REQUIRE(*s == '\0');
    }
    SECTION("Single quote comma") {
        const char* s = "'ab',";
        json5::value val;
        json5::value::parse_string(&s, val);
        REQUIRE(*s == ',');
    }
    SECTION("Double quote") {
        const char* s = "\"ab\"";
        json5::value val;
        json5::value::parse_string(&s, val);
        REQUIRE(*s == '\0');
    }

    SECTION("Double quotes in single quote") {
        const char* s = "'a\"b'";
        json5::value val;
        json5::value::parse_string(&s, val);
        REQUIRE(*s == '\0');
    }
}

TEST_CASE("JSON5_Parser_object") {
    const char* s = "{}";
    json5::value val;
    json5::value::parse_object(&s, val);
    REQUIRE(*s == '\0');
}

TEST_CASE("JSON5_Default") {

    SECTION("Default values") {
        json5::value j;
        REQUIRE(j.is_null() == true);
        REQUIRE(j.is_boolean() == false);
        REQUIRE(j.is_number_integer() == false);
        REQUIRE(j.is_number() == false);
        REQUIRE(j.is_string() == false);
        REQUIRE(j.is_object() == false);
        REQUIRE(j.is_array() == false);
    }

    SECTION("Default true value") {
        json5::value j;
        REQUIRE(j.value_or(true) == true);
    }
    SECTION("Default false value") {
        json5::value j;
        REQUIRE(j.value_or(false) == false);
    }
    SECTION("Default int value") {
        json5::value j;
        REQUIRE(j.value_or(999) == 999);
    }
    SECTION("Default float value") {
        json5::value j;
        REQUIRE(j.value_or(3.1415) == 3.1415);
    }
    SECTION("Default string value") {
        json5::value j;
        REQUIRE(j.value_or(std::string { "qwerty" }) == "qwerty");
        REQUIRE(j.value_or(std::string { "qwerty" }) == std::string { "qwerty" });
    }
    SECTION("Default string_view value") {
        json5::value j;
        REQUIRE(j.value_or(std::string_view { "qwerty" }) == "qwerty");
        REQUIRE(j.value_or(std::string_view { "qwerty" }) == std::string_view { "qwerty" });
    }
    SECTION("Default c_str value") {
        json5::value j;
        REQUIRE(j.value_or("qwerty") == std::string { "qwerty" });
        REQUIRE(j.value_or("qwerty") == std::string_view { "qwerty" });
    }
}

TEST_CASE("JSON5_ParseEmpty") {
    auto j = json5::value::parse("");
    REQUIRE(j.is_null());
}

TEST_CASE("JSON5_ParseNull") {
    SECTION("Null only") {
        auto j = json5::value::parse("null");
        REQUIRE(j.is_null());
    }

    SECTION("with spaces") {
        auto j = json5::value::parse(" null ");
        REQUIRE(j.is_null());
    }
}

TEST_CASE("JSON5_ParseBoolean") {

    SECTION("True") {
        auto j = json5::value::parse("true");
        REQUIRE(j.is_boolean());
        REQUIRE(j.get<bool>() == true);
    }

    SECTION("False") {
        auto j = json5::value::parse("false");
        REQUIRE(j.is_boolean());
        REQUIRE(j.get<bool>() == false);
    }
}

TEST_CASE("JSON5_ParseString") {
    SECTION("Single quotes string") {
        auto j = json5::value::parse("\"asd\"");
        REQUIRE(j.is_string());
        REQUIRE(j.get<std::string>() == "asd");
    }

    SECTION("Single quotes string") {
        auto j = json5::value::parse("'asd'");
        REQUIRE(j.is_string());
        REQUIRE(j.get<std::string>() == "asd");
    }

    SECTION("Single quotes string with spaces") {
        auto j = json5::value::parse("  'a s d'  ");
        REQUIRE(j.is_string());
        REQUIRE(j.get<std::string>() == "a s d");
    }

    SECTION("Single quotes string_view") {
        auto j = json5::value::parse("'asd'");
        REQUIRE(j.is_string());
        REQUIRE(j.get<std::string_view>() == "asd");
    }

    SECTION("Double quotes string_view") {
        auto j = json5::value::parse("\"asd\"");
        REQUIRE(j.is_string());
        REQUIRE(j.get<std::string_view>() == "asd");
    }

    SECTION("Escape") {
        auto j = json5::value::parse("'a\b\f\ts\r\nd\'");
        REQUIRE(j.is_string());
        REQUIRE(j.get<std::string_view>() == "a\b\f\ts\r\nd");
    }
}

TEST_CASE("JSON5_ParseNumber") {
    SECTION("Int value") {
        auto j = json5::value::parse("123");
        REQUIRE(j.is_number_integer());
        REQUIRE(j.get<int>() == 123);
    }

    SECTION("Int value with spaces") {
        auto j = json5::value::parse(" 123 ");
        REQUIRE(j.is_number_integer());
        REQUIRE(j.get<int>() == 123);
    }

    SECTION("Int positive value") {
        auto j = json5::value::parse("+123");
        REQUIRE(j.is_number_integer());
        REQUIRE(j.get<int>() == 123);
    }

    SECTION("Int negative value") {
        auto j = json5::value::parse("-123");
        REQUIRE(j.is_number_integer());
        REQUIRE(j.get<int>() == -123);
    }

    SECTION("Int hex value") {
        auto j = json5::value::parse("0xdecaf");
        REQUIRE(j.is_number_integer());
        REQUIRE(j.get<int>() == 912559);
    }

    SECTION("Int hex value") {
        auto j = json5::value::parse("-0xC0FFEE");
        REQUIRE(j.is_number_integer());
        REQUIRE(j.get<int>() == -12648430);
    }

    SECTION("Double with fraction part") {
        auto j = json5::value::parse("123.456");
        REQUIRE(j.is_number());
        REQUIRE(j.get<double>() == 123.456);
    }

    SECTION("Double with fraction part") {
        auto j = json5::value::parse("-123.456");
        REQUIRE(j.is_number());
        REQUIRE(j.get<double>() == -123.456);
    }

    SECTION("Double only fraction part") {
        auto j = json5::value::parse(".456");
        REQUIRE(j.is_number());
        REQUIRE(j.get<double>() == 0.456);
    }

    SECTION("Nan value") {
        auto j = json5::value::parse("NaN");
        REQUIRE(j.is_number());
        REQUIRE(std::isnan(j.get<double>()));
    }

    SECTION("Infinity value") {
        auto j = json5::value::parse("Infinity");
        REQUIRE(j.is_number());
        REQUIRE(std::isinf(j.get<double>()));
    }

    SECTION("Positive infinity value") {
        auto j = json5::value::parse("+Infinity");
        REQUIRE(j.is_number());
        REQUIRE(std::isinf(j.get<double>()));
    }

    SECTION("Negative infinity value") {
        auto j = json5::value::parse("-Infinity");
        REQUIRE(j.is_number());
        REQUIRE(std::isinf(j.get<double>()));
    }
}

TEST_CASE("JSON5_ParseObject") {
    SECTION("Empty object") {
        auto j = json5::value::parse("{}");
        REQUIRE(j.is_object());
    }

    SECTION("Empty object with spaces") {
        auto j = json5::value::parse(" {  } ");
        REQUIRE(j.is_object());
    }

    SECTION("Object with single member") {
        auto j = json5::value::parse("{ ab: 123 }");
        REQUIRE(j.is_object());
        REQUIRE(j.at("ab").get<int>() == 123);
        REQUIRE(j["ab"].get<int>() == 123);
        REQUIRE(j.size() == 1);
    }

    SECTION("Object with two member") {
        auto j = json5::value::parse("{ aa: 123, bb: \"xyz\" }");
        REQUIRE(j.is_object());
        REQUIRE(j.at("aa").get<int>() == 123);
        REQUIRE(j["aa"].get<int>() == 123);
        REQUIRE(j.at("bb").get<std::string_view>() == "xyz");
        REQUIRE(j["bb"].get<std::string_view>() == "xyz");
        REQUIRE(j.size() == 2);
    }

    SECTION("Nested objects") {
        auto j = json5::value::parse("{ d: {a: 1, b: 'asd', c: true} }");
        REQUIRE(j.is_object());
        REQUIRE(j.size() == 1);
        REQUIRE(j[0].is_object());
        REQUIRE(j["d"].is_object());
        REQUIRE(j[0].size() == 3);
        REQUIRE(j["d"]["a"].get<int>() == 1);
        REQUIRE(j["d"][0].get<int>() == 1);
        REQUIRE(j["d"]["b"].get<std::string>() == "asd");
        REQUIRE(j[0][2].get<bool>() == true);
    }
}

TEST_CASE("JSON5_ParseArray") {
    SECTION("Empty array") {
        auto j = json5::value::parse("[]");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 0);
    }

    SECTION("Single int element") {
        const int arr[1] = { 1 };
        auto j = json5::value::parse("[1]");
        REQUIRE(j.is_array());
        REQUIRE(j[0].get<int>() == arr[0]);
        REQUIRE(j.size() == 1);
    }

    SECTION("Single int element with spaces") {
        const int arr[1] = { 1 };
        auto j = json5::value::parse(" [ 1 ] ");
        REQUIRE(j.is_array());
        REQUIRE(j[0].get<int>() == arr[0]);
        REQUIRE(j.size() == 1);
    }

    SECTION("Two int elements") {
        const int arr[2] = { 1, 2 };
        auto j = json5::value::parse("[1, 2]");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 2);
        REQUIRE(j[0].get<int>() == arr[0]);
        REQUIRE(j[1].get<int>() == arr[1]);
    }

    SECTION("Two int elements with spaces") {
        const int arr[2] = { 1, 2 };
        auto j = json5::value::parse(" [ 1 , 2 ] ");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 2);
        REQUIRE(j[0].get<int>() == arr[0]);
        REQUIRE(j[1].get<int>() == arr[1]);
    }

    SECTION("Array with three elements") {
        auto j = json5::value::parse("[1, true, 'three']");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 3);
        REQUIRE(j[0].is_number_integer());
        REQUIRE(j[0].get<int>() == 1);
        REQUIRE(j[1].is_boolean());
        REQUIRE(j[1].get<bool>() == true);
        REQUIRE(j[2].is_string());
        REQUIRE(j[2].get<std::string_view>() == "three");
    }

    SECTION("at int array in range") {
        const int arr[3] = { 1, 2, 3 };
        auto j = json5::value::parse("[1, 2, 3]");
        REQUIRE(j.is_array());
        for (size_t i = 0; i < 3; i++) {
            REQUIRE(arr[i] == j.at(i).get<int>());
        }
    }

    SECTION("[] int array in range") {
        const int arr[3] = { 1, 2, 3 };
        auto j = json5::value::parse("[1, 2, 3]");
        REQUIRE(j.is_array());
        for (size_t i = 0; i < 3; i++) {
            REQUIRE(arr[i] == j[i].get<int>());
            REQUIRE(j.size() == 3);
        }
    }

    SECTION("at int array out of range") {
        auto j = json5::value::parse("[1, 2, 3]");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 3);
        auto k = j.at_opt(5);
        REQUIRE(k.is_null());
        REQUIRE(k.value_or(42) == 42);
    }

    SECTION("Trailing comma") {
        const int arr[2] = { 1, 2 };
        auto j = json5::value::parse("[1, 2,]");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 2);
        REQUIRE(j[0].get<int>() == arr[0]);
        REQUIRE(j[1].get<int>() == arr[1]);
    }

    SECTION("Nested int array") {
        auto j = json5::value::parse("[[1,2]]");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 1);
        REQUIRE(j[0].is_array());
        REQUIRE(j[0].size() == 2);
    }

    SECTION("Nested two int array") {
        auto j = json5::value::parse("[[1,2],[3,4,5]]");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 2);
        REQUIRE(j[0].is_array());
        REQUIRE(j[0].size() == 2);
        REQUIRE(j[1].is_array());
        REQUIRE(j[1].size() == 3);
    }

    SECTION("Nested array") {
        auto j = json5::value::parse("[[1, false, 'three'],[4.23, \"five\", 0x6]]");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 2);
        REQUIRE(j[0].is_array());
        REQUIRE(j[1].is_array());
    }

    SECTION("An array of objects") {
        auto j = json5::value::parse("[ { name: 'Joe', age: 27 }, { name: 'Jane', age: 32 }]");
        REQUIRE(j.is_array());
        REQUIRE(j.size() == 2);
    }
}