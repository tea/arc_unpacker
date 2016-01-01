#pragma once

#include "io/istream.h"
#include "res/image.h"

namespace au {
namespace dec {
namespace bgi {
namespace cbg {

    class Cbg2Decoder final
    {
    public:
        std::unique_ptr<res::Image> decode(io::IStream &input_stream) const;
    };

} } } }