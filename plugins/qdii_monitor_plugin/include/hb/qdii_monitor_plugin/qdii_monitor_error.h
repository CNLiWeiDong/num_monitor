#pragma once

#include <hb/error/exception.h>

namespace hb {
    namespace plugin {
        using namespace hb::error;

        class qdii_monitor_error
            : public hb_exception<qdii_monitor_error, SN("qdiierror")> {
          public:
            qdii_monitor_error()
                : hb_exception<qdii_monitor_error, SN("qdiierror")>() {}
        };
    }  // namespace plugin
}  // namespace hb