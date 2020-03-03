/*
 * Base 64 test cases.
 *
 * These are the examples in from
 * https://en.wikipedia.org/wiki/Base64 on February 5, 2018.
 */

#include <utility>

constexpr std::pair<const char*,const char*> base64TestCases[] = {
    // Raw Data             , Base64 Encoded
    { "Man",                  "TWFu" },
    { "Ma",                   "TWE=" },
    { "M",                    "TQ==" },

    { "any carnal pleasure.", "YW55IGNhcm5hbCBwbGVhc3VyZS4=" },
    { "any carnal pleasure",  "YW55IGNhcm5hbCBwbGVhc3VyZQ==" },
    { "any carnal pleasur",   "YW55IGNhcm5hbCBwbGVhc3Vy" },
    { "any carnal pleasu",    "YW55IGNhcm5hbCBwbGVhc3U=" },
    { "any carnal pleas",     "YW55IGNhcm5hbCBwbGVhcw==" },

    { "pleasure.",            "cGxlYXN1cmUu" },
    { "leasure.",             "bGVhc3VyZS4=" },
    { "easure.",              "ZWFzdXJlLg==" },
    { "asure.",               "YXN1cmUu" },
    { "sure.",                "c3VyZS4=" },
};
