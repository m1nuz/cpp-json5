#include <fstream>
#include <iostream>

#include <json5/json5.hpp>

template <typename Buffer> inline auto read_contents(std::string_view filepath) -> std::optional<Buffer> {
    using namespace std;

    ifstream fs(filepath.data(), ios::in | ios::binary);

    if (!fs.is_open())
        return {};

    Buffer contents;
    fs.seekg(0, ios::end);
    const auto sz = fs.tellg();
    contents.resize(static_cast<size_t>(sz));
    fs.seekg(0, ios::beg);
    fs.read(reinterpret_cast<char*>(std::data(contents)), static_cast<std::streamsize>(std::size(contents)));
    fs.close();

    return { contents };
}

int main(int, char**) {
    if (auto contents = read_contents<std::string>("../sample/sample.json5"); contents) {
        auto j = json5::value::parse(*contents);
        if (j.is_object() && j.size() == 3 && j["witharray"].is_array() && j["witharray"][0]["name"].get<std::string_view>() == "Joe"
            && j["withNestedArray"][1][0].get<int>() == 4) {
            std::cout << "Success" << std::endl;
        }
        return 0;
    }

    return 1;
}
