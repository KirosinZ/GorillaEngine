#ifndef BUILD_H
#define BUILD_H

namespace build
{

enum class build_type
{
    unknown,
    release,
    debug
};

#ifdef GORILLA_BUILD_TYPE_Release
constexpr build_type type = build_type::release;
#endif
#ifdef GORILLA_BUILD_TYPE_Debug
constexpr build_type type = build_type::debug;
#endif

}

#endif
