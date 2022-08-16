//
// Created by 李卫东 on 2019-02-18.
//
#pragma once

#include <hb/dingtalk_plugin/dingtalk_plugin.h>
#include <hb/log/log.h>
#include <hb/qdii_monitor_plugin/qdii_monitor_error.h>
#include <hb/time/time.h>

#include <appbase/application.hpp>
#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace hb {
    namespace plugin {
        using namespace std;
        using namespace appbase;

        enum target_status {
            target_status_begin,
            target_status_ok,
            target_status_request_error,
            target_status_regex_value_error,
            target_status_regex_date_error,
            target_status_regex_content_error,
            target_status_throw_error,
            target_status_week_error,
            target_status_day_minutes_error
        };
        struct target_type {
            string id;
            string name;
            int min_week_day;
            int max_week_day;
            int min_day_minutes;
            int max_day_minutes;
            string format_value;
            // int format_value_index = 1;
            string format_date;
            // int format_date_index = 1;
            string format_contents;
            double value;
            string date;
            vector<string> contents;
            target_status status;
            uint64_t update_time;
            bool active;
            int request_error_times = 0;
            uint64_t last_send_error_time = 0;
        };

        class monitor_targets : public std::enable_shared_from_this<monitor_targets> {
            map<string, target_type> all_targets_list_;
            uint32_t http_request_expire_ = {30};
            uint32_t max_request_error_time_ = {5};
            uint64_t senderror_seconds_ = {1 * 60 * 60};
            uint64_t send_target_info_seconds_ = {1 * 60 * 60};
            uint64_t last_notice_time_ = 0;
          public:
            string server_type_;
            string host_;
            string port_;
            string target_;
          public:
            void http_request_expire(const int &t) { http_request_expire_ = t; }
            void max_request_error_time(const int &t) { max_request_error_time_ = t; }
            void send_target_info_seconds(const int &t) { send_target_info_seconds_ = t; }
            void senderror_seconds(int t) { senderror_seconds_ = t; }
            map<string, target_type> &all_tagets() { return all_targets_list_; }
            void add_target(const string &id, const target_type &target) {
                auto iter = all_targets_list_.find(id);
                if (iter != all_targets_list_.end()) {
                    hb::plugin::qdii_monitor_error e;
                    e.msg("%s target already existed!", id);
                    hb_throw(e);
                }
                all_targets_list_.insert(
                    decltype(all_targets_list_)::value_type(id, std::move(target)));
            }

          public:
            void deal_query();
            void notice_target_info();
            void set_target(target_type &item, const int &status, const std::string &res);
            void add_request_error_times(target_type &item);
        };
    }  // namespace plugin
}  // namespace hb
