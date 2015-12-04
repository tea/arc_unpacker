#pragma once

#include <algorithm>

namespace au {
namespace util {

    struct Range final
    {
        struct Iterator final
            : std::iterator<std::random_access_iterator_tag, int, int>
        {
            int i, stride;

            constexpr Iterator(int i, int stride) : i(i), stride(stride)
            {
            }

            constexpr Iterator(Iterator it, int stride) : i(*it), stride(stride)
            {
            }

            constexpr int operator *()
            {
                return i;
            }

            inline Iterator operator ++()
            {
                i += stride;
                return *this;
            }

            constexpr bool operator !=(Iterator other)
            {
                return stride < 0 ? i > *other : i < *other;
            }
        };

        constexpr Range(int b, int e, int stride = 1)
            : stride(stride), b(b, stride), e(e, stride)
        {
        }

        constexpr Iterator begin()
        {
            return b;
        }

        constexpr Iterator end()
        {
            return e;
        }

        int stride;
        Iterator b, e;
    };

    constexpr Range range(int b, int e, int stride=1)
    {
        return Range(b, e, stride);
    }

    constexpr Range range(int e)
    {
        return Range(0, e);
    }

} }
