#include <iostream>
#include <string>

#include "acp/Allocator.hpp"
#include "acp/Cache.hpp"

namespace {

struct String {
    std::string data;
    bool marked{false};

    String(std::string const& key) : data(key) {}

    bool operator==(std::string const& other) const { return data == other; }

    [[maybe_unused]] friend std::ostream& operator<<(std::ostream& strm, String const& str) { return strm << str.data; }
};

using TestCache = Cache<std::string, String, AllocatorWithPool>;

}  // anonymous namespace

int main() {
    TestCache cache(9, 18 * sizeof(String), std::initializer_list<std::size_t>{sizeof(String)});
    std::string line;
    while (std::getline(std::cin, line)) {
        auto& s = cache.get<String>(line);
        if (s.marked) {
            std::cout << "known" << std::endl;
        }
        s.marked = true;
    }
    std::cout << "\n" << cache << std::endl;
}
