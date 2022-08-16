//
// Created by 李卫东 on 2019-02-19.
//
#include <hb/dingtalk_plugin/dingtalk_plugin.h>
#include <hb/http/http.h>
#include <hb/https/https.h>
#include <hb/qdii_monitor_plugin/qdii_monitor_plugin_impl.h>
#include <math.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <sstream>

namespace hb {
    namespace plugin {
        const char* green_color = "#3DDC84";
        const char* red_color = "#FF0000";
        qdii_monitor_plugin_impl::~qdii_monitor_plugin_impl() {}

        void qdii_monitor_plugin_impl::loop() {
            log_info("qdii_monitor_plugin_impl::loop");
            auto self = shared_from_this();
            auto &io = app().get_io_service();
            hb_try {
                targets_->deal_query();
                boost::this_thread::sleep(boost::posix_time::milliseconds(100)); // 保证通知的顺序
                targets_->notice_target_info();
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                singles_->deal(targets_);
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                pairs_->deal(targets_);

                // 生成下一次定时
                uint32_t millisec = 100;
                if (intervals_seconds_ > 0) {
                    millisec = intervals_seconds_ * 1000;
                }
                deadline_updater_ = make_shared<boost::asio::deadline_timer>(
                    io, boost::posix_time::milliseconds(millisec));
                deadline_updater_->async_wait(
                    [self](const boost::system::error_code &ec) { self->loop(); });
            }
            hb_catch([&](const auto &err) { log_throw("qdii_monitor_plugin_impl::loop", err); });
        }
        void qdii_monitor_plugin_impl::start() { loop(); }
    }  // namespace plugin
}  // namespace hb