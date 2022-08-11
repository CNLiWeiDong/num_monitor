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

        struct pairs_type {
            string id1;
            string id2;
            double min;
            double max;
            string phone;
            string mail;
            uint64_t send_msg_time;
            bool active;
        };

        class monitor_pairs : public std::enable_shared_from_this<monitor_pairs> {
            vector<pairs_type> pairs_list_;
            uint64_t sendmsg_seconds_ = {2 * 60 * 60};

          public:
            void add_pairs(const pairs_type &p) { pairs_list_.push_back(p); }
            void sendmsg_seconds(int t) { sendmsg_seconds_ = t; }

          public:
            void deal(const shared_ptr<monitor_targets> &targets);
        };
    }  // namespace plugin
}  // namespace hb
