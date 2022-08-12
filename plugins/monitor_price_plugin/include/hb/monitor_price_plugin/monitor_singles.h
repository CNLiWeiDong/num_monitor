//
// Created by 李卫东 on 2019-02-18.
//
#pragma once

#include <hb/log/log.h>
#include <hb/monitor_price_plugin/monitor_targets.h>

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
        struct singles_type {
            string id;
            double min;
            double max;
            string phone;
            string mail;
            uint64_t send_msg_time;
            bool active;
        };

        class monitor_singles : public std::enable_shared_from_this<monitor_singles> {
            std::map<string, singles_type> singles_list_;
            uint64_t sendmsg_seconds_ = {10 * 60};
            uint64_t send_singles_msg_time_ = 0;

          public:
            void add_single(const singles_type &single) { 
                auto iter = singles_list_.find(single.id);
                if (iter != singles_list_.end()) {
                    hb::plugin::monitor_price_exception e;
                    e.msg("%s single already existed!", single.id);
                    hb_throw(e);
                }
                singles_list_.insert(
                    decltype(singles_list_)::value_type(single.id, std::move(single)));
            }
            void sendmsg_seconds(int t) { sendmsg_seconds_ = t; }

          public:
            void deal(const shared_ptr<monitor_targets> &targets);
        };
    }  // namespace plugin
}  // namespace hb
