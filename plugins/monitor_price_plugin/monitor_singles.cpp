//
// Created by 李卫东 on 2019-02-19.
//
#include <hb/dingtalk_plugin/dingtalk_plugin.h>
#include <hb/http/http.h>
#include <hb/https/https.h>
#include <hb/monitor_price_plugin/monitor_singles.h>
#include <math.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <sstream>

namespace hb {
    namespace plugin {
        extern const char* green_color;
        extern const char* red_color;
        void monitor_singles::deal(const shared_ptr<monitor_targets> &targets) {
            log_info("begin monitor_single");
            auto cur_seconds = hb::time::timestamp();
            if (cur_seconds - send_singles_msg_time_ < sendmsg_seconds_) {
                return;
            }
            auto &all_targets_list = targets->all_tagets();
            vector<string> notice_ids;
            for (auto &it : singles_list_) {
                auto &item = it.second;
                if (!item.active) continue;
                auto &target = all_targets_list[item.id];
                if (!target.active) continue;
                if (target.status != target_status::target_status_ok) {
                    log_info("【single %s】status wrong %d", target.id, target.status);
                    continue;
                }
                
                if (item.min < target.value && target.value < item.max) {
                    log_info("【single %s】normal %。2f", target.id, target.value);
                } else {
                    log_info("【single %s】attent %。2f", target.id, target.value);
                    notice_ids.push_back(item.id);
                }
            }
            if (notice_ids.size() == 0) {
                return;
            }
            std::sort(notice_ids.begin(), notice_ids.end(),[&](const string &a, const string &b){
                auto it1 = all_targets_list.find(a);
                auto it2 = all_targets_list.find(b);
                return it1->second.name < it2->second.name;
            });
            string notice_text = "## 指标数值超范提示\n\n<font color=\"#FF0000\">==========请留意==========</font>\n";
            for (auto &id : notice_ids) {
                auto it1 = all_targets_list.find(id);
                if (it1 == all_targets_list.end()) continue;
                auto it2 = singles_list_.find(id);
                if (it2 == singles_list_.end()) continue;
                auto &target = it1->second;
                auto &single = it2->second;
                boost::format target_fmt("### %s(%s)\n- 溢价: **<font color=\"%s\">%.2f%%</font>**\n- 设定范围: %.2f~%.2f\n\n");
                target_fmt 
                    % target.name 
                    % target.id 
                    % (target.value>0 ? red_color : green_color)
                    % target.value 
                    % single.min 
                    % single.max;
                notice_text += target_fmt.str();
            }
            auto &dingtalk = app().get_plugin<dingtalk_plugin>();
            dingtalk.send(notice_text);
            send_singles_msg_time_ = hb::time::timestamp();
        }
    }  // namespace plugin
}  // namespace hb