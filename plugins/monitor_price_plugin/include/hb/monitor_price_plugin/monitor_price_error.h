#pragma once

#include <hb/error/exception.h>

namespace hb {
    namespace plugin {
        using namespace hb::error;

        class monitor_price_exception
            : public hb_exception<monitor_price_exception, SN("monitorprice")> {
          public:
            monitor_price_exception()
                : hb_exception<monitor_price_exception, SN("monitorprice")>() {}
        };
    }  // namespace plugin
}  // namespace hb