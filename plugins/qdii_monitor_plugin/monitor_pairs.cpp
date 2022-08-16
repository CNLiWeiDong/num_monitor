//
// Created by 李卫东 on 2019-02-19.
//
#include <hb/dingtalk_plugin/dingtalk_plugin.h>
#include <hb/http/http.h>
#include <hb/https/https.h>
#include <hb/qdii_monitor_plugin/monitor_pairs.h>
#include <hb/time/time.h>
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
        void monitor_pairs::deal(const shared_ptr<monitor_targets> &targets) {
            log_info("begin monitor_pair");
            auto cur_seconds = hb::time::timestamp();
            if (cur_seconds - send_pair_msg_time_ < sendmsg_seconds_) {
                log_info("end monitor_pair: time is not up");
                return;
            }
            auto &all_targets_list = targets->all_tagets();
            vector<int> notice_indexs;
            vector<double> sub_values;
            for (int i=0; i<pairs_list_.size(); i++) {
                auto &it = pairs_list_[i];
                if (!it.active) continue;
                auto &target1 = all_targets_list[it.id1];
                auto &target2 = all_targets_list[it.id2];
                if (!target1.active || !target2.active) continue;

                if (target1.status != target_status::target_status_ok
                    || target2.status != target_status::target_status_ok) {
                    log_info("【pair %s~%s】status is wrong %d~%d", target1.id, target2.id,
                             target1.status, target2.status);
                    continue;
                }
                
                if (target1.date != target2.date) {
                    log_info("【pair %s~%s】date is wrong %s~%s", target1.id, target2.id,
                             target1.date, target2.date);
                    continue;
                }
                // auto val = std::fabs(target2.value-target1.value);
                auto val = target2.value - target1.value;
                
                if (it.min < val && val < it.max) {
                    log_info("【pair %s~%s】normal %f", target1.id, target2.id, val);
                } else {
                    log_info("【pair %s~%s】attent %f", target1.id, target2.id, val);
                    notice_indexs.push_back(i);
                    sub_values.push_back(val);
                }
            }
            if (notice_indexs.size() == 0) {
                log_info("end monitor_pair: no pair to notice");
                return;
            }
            string notice_text = "## 两指标差值超范提示\n\n<font color=\"#FF0000\">====================</font>\n";
            for (int i=0; i<notice_indexs.size(); i++) {
                auto &it = pairs_list_[notice_indexs[i]];
                auto &target1 = all_targets_list[it.id1];
                auto &target2 = all_targets_list[it.id2];
                boost::format target_fmt("### %s~%s\n- 设定范围: <font color=\"#0000FF\">%.2f~%.2f</font>\n- 两者差值: **<font color=\"%s\">%.2f%%</font>**\n\n");
                target_fmt 
                    % target1.id 
                    % target2.id
                    % it.min
                    % it.max 
                    % (sub_values[i]>0 ? red_color : green_color)
                    % sub_values[i];
                notice_text += target_fmt.str();
            }
            notice_text += "<font color=\"#FF0000\">====================</font>\n";
            auto &dingtalk = app().get_plugin<dingtalk_plugin>();
            dingtalk.send(notice_text);
            send_pair_msg_time_ = hb::time::timestamp();
            log_info("end monitor_pair");
        }
    }  // namespace plugin
}  // namespace hb