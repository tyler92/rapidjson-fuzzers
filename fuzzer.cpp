/* By Guido Vranken <guidovranken@gmail.com> */

#include <cstdint>
#include <cstddef>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#ifdef MSAN
extern "C" {
    void __msan_check_mem_is_initialized(const volatile void *x, size_t size);
}
#endif

template<unsigned parseFlags>
void fuzzWithFlags(const std::string &s)
{
    /* Parse input to rapidjson::Document */
    rapidjson::Document document;
    rapidjson::ParseResult pr = document.Parse<parseFlags>(s.c_str());
    if ( !pr ) {
        return;
    }

    /* Convert from rapidjson::Document to string */
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);
    std::string str = sb.GetString();
#ifdef MSAN
    if ( str.size() ) {
        __msan_check_mem_is_initialized(str.data(), str.size());
    }
#endif
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const std::string s(data, data + size);

    fuzzWithFlags<rapidjson::kParseDefaultFlags>(s);\
    fuzzWithFlags<rapidjson::kParseFullPrecisionFlag>(s);
    fuzzWithFlags<rapidjson::kParseNumbersAsStringsFlag>(s);
    fuzzWithFlags<rapidjson::kParseCommentsFlag>(s);

    return 0;
}
