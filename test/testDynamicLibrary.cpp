#include <string>
#include "sysexception.hpp"
#include "dynamiclibrary.hpp"
#include "CppUnitXLite/CppUnitXLite.hpp"
using std::string;


TEST(oscpp, testSystemDynamicLibrary) {
    try {
        const auto target = "/usr/lib/libc++.1.dylib";
        oscpp::DynamicLibrary dynamicLibrary {target};
        auto queryResult = dynamicLibrary.symbol("strlen");
        CHECK_EQUAL(reinterpret_cast<void *>(strlen), queryResult);

    }
    catch (const std::exception &ex) {
        FAIL(ex.what());
    }
}
