//
// Created by 李卫东 on 2019-02-19.
//
#include <hb/dingtalk_plugin/dingtalk_plugin.h>
#include <hb/hb.h>
#include <hb/qdii_monitor_plugin/monitor_targets.h>
#include <hb/thread_pool_plugin/thread_pool_plugin.h>
#include <math.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <sstream>

namespace hb::plugin {
    extern const char* green_color;
    extern const char* red_color;
    bool regex_value(boost::regex &reg, boost::smatch &what, const std::string &res) {
        if (boost::regex_search(res.begin(), res.end(), what, reg)) {
            if (what.size() < 2) {
                LOG_ERROR("regex_search what.size error %lu", what.size());
                return false;
            }
            if (!what[1].matched) {
                LOG_ERROR("regex_search what[1].matched error %s", ((string)what[1]).c_str());
                return false;
            }
        } else {
            LOG_ERROR("regex_search error");
            return false;
        }
        return true;
    }
    void monitor_targets::add_request_error_times(target_type &item) {
        item.request_error_times++;
        if (item.request_error_times >= max_request_error_time_) {
            item.request_error_times = 0;
            auto cur_seconds = hb::time::timestamp();
            if (cur_seconds - item.last_send_error_time >= senderror_seconds_) {
                item.last_send_error_time = cur_seconds;
                log_info("【target request error】%s %d ", item.id, item.request_error_times);
                auto &dingtalk = app().get_plugin<dingtalk_plugin>();
                string notice_text = "## 指标更新异常\n";
                boost::format target_fmt("### %s(%s)\n指标多次请求错误, 请开发人员排除问题.\n\n");
                target_fmt 
                    % item.name 
                    % item.id ;
                notice_text += target_fmt.str();
                dingtalk.send(notice_text);
            }
        }
    }
    void monitor_targets::set_target(target_type &item, const int &status,
                                        const std::string &res) {
        hb_try {
            LOG_INFO("set_target %s, %s, %d", item.id.c_str(),
                        item.name.c_str(),  status);
            if (status != 200) {
                LOG_ERROR("[%s, %s] set_target %d %s", item.id.c_str(),
                            item.name.c_str(), status, res.c_str());
                item.status = target_status::target_status_request_error;
                add_request_error_times(item);
                return;
            }
            boost::regex reg(item.format_value);
            boost::smatch what;
            if (regex_value(reg, what, res)) {
                item.update_time = hb::time::timestamp();
                const string val = what[1];
                item.value = std::atof(val.c_str());
                LOG_INFO("set target value info %s, %f, %llu", item.id.c_str(), item.value,
                            item.update_time);
                item.status = target_status::target_status_ok;  // 设置成功放在最后，失败放前面
            } else {
                item.status = target_status::target_status_regex_value_error;
                add_request_error_times(item);
                return;
            }
            if (item.format_date != "") {
                boost::regex reg(item.format_date);
                boost::smatch what;
                if (regex_value(reg, what, res)) {
                    item.update_time = hb::time::timestamp();
                    item.date = what[1];
                    LOG_INFO("set target date info %s, %s, %llu", item.id.c_str(),
                                item.date.c_str(), item.update_time);
                    item.status = target_status::target_status_ok;
                } else {
                    item.status = target_status::target_status_regex_date_error;
                    add_request_error_times(item);
                    return;
                }
            }
            if (item.format_contents != "") {
                boost::regex reg(item.format_contents);
                boost::smatch what;
                if (regex_value(reg, what, res)) {
                    item.update_time = hb::time::timestamp();
                    vector<string> contents;
                    for (int i = 1; i < what.size(); i++) {
                        contents.push_back(what[i]);
                        log_info("content %d value %s", i, what[i]);
                    }
                    item.contents = contents;
                    LOG_INFO("set target contents %s, size: %u", item.id.c_str(),
                                item.contents.size());
                    item.status = target_status::target_status_ok;
                } else {
                    item.status = target_status::target_status_regex_content_error;
                    add_request_error_times(item);
                    return;
                }
            }
            item.request_error_times = 0;
        }
        hb_catch([&](const auto &e) {
            item.status = target_status::target_status_throw_error;
            add_request_error_times(item);
            log_throw("monitor_targets::set_target", e);
        });
    }
    void monitor_targets::deal_query() {
        log_info("begin deal_query");
        hb_try {
            // 重置每个状态, 防止请求没返回, 使用上次结果
            const int week_day = hb::time::day_of_week();
            const int day_minutes = hb::time::hours_of_day() * 60 + hb::time::minutes_of_day();
            bool is_need_request = false;
            for (auto &it : all_targets_list_) {
                auto &item = it.second;
                item.status = target_status::target_status_begin;
                if (!item.active) continue;
                if (!(item.min_week_day <= week_day && item.max_week_day >= week_day)) {
                    log_info("%s is not in right week.", item.id);
                    item.status = target_status::target_status_week_error;
                    continue;
                }
                if (!(item.min_day_minutes <= day_minutes
                        && item.max_day_minutes >= day_minutes)) {
                    log_info("%s is not in right day minutes.", item.id);
                    item.status = target_status::target_status_day_minutes_error;
                    continue;
                }
                is_need_request = true;
            }
            if (!is_need_request) {
                log_info("end deal_query: no need to request.");
                return;
            }
            auto &thread_plugin = app().get_plugin<thread_pool_plugin>();
            auto abi = thread_plugin.get_api();
            auto ioc = abi->get_io_context();
            string response;
            int status;
            if (server_type_ == "https") {
                hb::https::https hs(host_, port_, target_,http_request_expire_); 
                status = hs.get(*ioc);
                response = hs.res_body();
            } else {
                hb::http::http h(host_, port_, target_,http_request_expire_); 
                status = h.get(*ioc);
                response = h.res_body();
            }
            boost::replace_all(response, "\n", "");

            for (auto &it : all_targets_list_) {
                auto &item = it.second;
                if (!item.active) continue;
                if (!(item.min_week_day <= week_day && item.max_week_day >= week_day))
                    continue;
                if (!(item.min_day_minutes <= day_minutes && item.max_day_minutes >= day_minutes)) 
                    continue;
                set_target(item, status, response);
            }
        }
        hb_catch([](const auto &e) { log_throw("monitor_targets::deal_query", e); });
        log_info("end deal_query");
    }

    void monitor_targets::notice_target_info() {
        log_info("begin notice_target_info");
        if (hb::time::timestamp() - last_notice_time_ < send_target_info_seconds_) {
            log_info("end notice_target_info: time is not up");
            return;
        }            
        vector<string> notice_ids;
        for (auto &it : all_targets_list_) {
            auto &item = it.second;
            if (!item.active) continue;
            if (item.status == target_status::target_status_ok) {
                notice_ids.push_back(it.first);
            }
        }
        if (notice_ids.size() == 0) {
            log_info("end notice_target_info: no target to notice");
            return;
        }
        std::sort(notice_ids.begin(), notice_ids.end(),[this](const string &a, const string &b){
            auto it1 = all_targets_list_.find(a);
            auto it2 = all_targets_list_.find(b);
            return it1->second.name < it2->second.name;
        });
        string notice_text = "## 指标基本信息提示\n";
        for (auto &id : notice_ids) {
            auto it = all_targets_list_.find(id);
            if (it == all_targets_list_.end()) continue;
            auto &item = it->second;
            float up = std::atof(item.contents[0].c_str());
            boost::format target_fmt("### %s(%s)\n- 溢价: **<font color=\"%s\">%.2f%%</font>** 涨幅: **<font color=\"%s\">%.2f%%</font>**\n- 估值日期: **<font color=\"#0000FF\">%s</font>**\n\n");
            target_fmt 
                % item.name 
                % item.id 
                % (item.value>0 ? red_color : green_color)
                % item.value 
                % (up>0 ? red_color : green_color)
                % up
                % item.date;
            notice_text += target_fmt.str();
        }
        notice_text += "[数据来源haoetf.com](https://www.haoetf.com/)";
        auto &dingtalk = app().get_plugin<dingtalk_plugin>();
        dingtalk.send(notice_text);
        last_notice_time_ = hb::time::timestamp();
        log_info("end notice_target_info");
    }
}  // namespace hb