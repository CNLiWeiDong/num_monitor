//
// Created by 李卫东 on 2019-02-18.
//
#pragma once

#include <hb/log/log.h>
#include <hb/qdii_monitor_plugin/monitor_pairs.h>
#include <hb/qdii_monitor_plugin/monitor_singles.h>
#include <hb/qdii_monitor_plugin/monitor_targets.h>

#include <appbase/application.hpp>
#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace hb {
    namespace plugin {
        using namespace std;
        using namespace appbase;

        class qdii_monitor_plugin_impl
            : public std::enable_shared_from_this<qdii_monitor_plugin_impl> {
            shared_ptr<monitor_targets> targets_;
            shared_ptr<monitor_singles> singles_;
            shared_ptr<monitor_pairs> pairs_;
            uint32_t intervals_seconds_ = {60};
            shared_ptr<boost::asio::deadline_timer> deadline_updater_;

          public:

            shared_ptr<monitor_targets> targets() { return targets_; }
            void add_target(const string &id, const target_type &target) { targets_->add_target(id, target); }
            void add_single(const singles_type &single) { singles_->add_single(single); }
            void add_pairs(const pairs_type &p) { pairs_->add_pairs(p); }
            void sendmsg_seconds(int t) {
                singles_->sendmsg_seconds(t);
                pairs_->sendmsg_seconds(t);
            }
            void send_target_info_seconds(const int &t) {  targets_->send_target_info_seconds(t); }
            void senderror_seconds(const int &t) { targets_->senderror_seconds(t); }
            void intervals_seconds(int t) { intervals_seconds_ = t; }
            void http_request_expire(int t) { targets_->http_request_expire(t); }
            void max_request_error_time(int t) { targets_->max_request_error_time(t); }
          public:
            qdii_monitor_plugin_impl() {
                targets_ = make_shared<monitor_targets>();
                singles_ = make_shared<monitor_singles>();
                pairs_ = make_shared<monitor_pairs>();
            }
            ~qdii_monitor_plugin_impl();
            void start();
            void loop();
        };
    }  // namespace plugin
}  // namespace hb
