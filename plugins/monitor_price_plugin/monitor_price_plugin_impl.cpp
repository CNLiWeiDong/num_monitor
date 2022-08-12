//
// Created by 李卫东 on 2019-02-19.
//
#include <hb/dingtalk_plugin/dingtalk_plugin.h>
#include <hb/http/http.h>
#include <hb/https/https.h>
#include <hb/monitor_price_plugin/monitor_price_plugin_impl.h>
#include <math.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#include <sstream>

namespace hb {
    namespace plugin {

        monitor_price_plugin_impl::~monitor_price_plugin_impl() {}

        void monitor_price_plugin_impl::deal_monitor() {
            hb_try {
                log_info("begin deal_monitor");
                singles_->deal(targets_);
                pairs_->deal(targets_);
                log_info("end deal_monitor");
            }
            hb_catch([](const auto &e) { log_throw("deal_monitor", e); });
        }

        void monitor_price_plugin_impl::deal_target_info() {
            log_info("begin deal_target_info");
            if (targets_->last_notice_time() + send_target_info_seconds_ < hb::time::timestamp()) {
                auto self = shared_from_this();
                auto &io = app().get_io_service();
                // 保证请求都返回了，做一个完整的信息展示
                deadline_after_deal_ = make_shared<boost::asio::deadline_timer>(
                    io, boost::posix_time::milliseconds(5000));
                deadline_after_deal_->async_wait([self](const boost::system::error_code &ec) {
                    self->targets_->notice_target_info();
                });
            }
            log_info("end deal_target_info");
        }

        void monitor_price_plugin_impl::loop() {
            log_info("monitor_price_plugin_impl::loop");
            auto self = shared_from_this();
            auto &io = app().get_io_service();
            hb_try {
                targets_->deal_query();
                // 不可以使用sleep, sleep了导致本线程请求的http也不返回。除非使用其他线程请求http。
                // 其他线程请求http会导出target锁
                // boost::this_thread::sleep(boost::posix_time::milliseconds(1000)); //
                // 尽量所有请求都返回
                deadline_after_query_ = make_shared<boost::asio::deadline_timer>(
                    io, boost::posix_time::milliseconds(1000));
                deadline_after_query_->async_wait([self, &io](const boost::system::error_code &ec) {
                    self->deal_monitor();
                    self->deal_target_info();

                    // 生成下一次定时
                    uint32_t millisec = 100;
                    if (self->intervals_seconds_ > 0) {
                        millisec = self->intervals_seconds_ * 1000;
                    }
                    self->deadline_updater_ = make_shared<boost::asio::deadline_timer>(
                        io, boost::posix_time::milliseconds(millisec));
                    self->deadline_updater_->async_wait(
                        [self](const boost::system::error_code &ec) { self->loop(); });
                });
            }
            hb_catch([&](const auto &err) { log_throw("monitor_price_plugin_impl::loop", err); });
        }
        void monitor_price_plugin_impl::start() { loop(); }
    }  // namespace plugin
}  // namespace hb