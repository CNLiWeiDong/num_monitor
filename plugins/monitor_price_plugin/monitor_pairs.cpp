//
// Created by 李卫东 on 2019-02-19.
//
#include <hb/dingtalk_plugin/dingtalk_plugin.h>
#include <hb/http/http.h>
#include <hb/https/https.h>
#include <hb/monitor_price_plugin/monitor_pairs.h>
#include <hb/time/time.h>
#include <math.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <sstream>

namespace hb {
    namespace plugin {

        void monitor_pairs::deal(const shared_ptr<monitor_targets> &targets) {
            log_info("begin monitor_pair");
            auto &all_targets_list = targets->all_tagets();
            for (auto &it : pairs_list_) {
                if (!it.active) continue;
                auto &target1 = all_targets_list[it.id1];
                auto &target2 = all_targets_list[it.id2];
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
                std::ostringstream contents1;
                contents1 << "(";
                for (auto &it : target1.contents) {
                    contents1 << " " << it << " ";
                }
                contents1 << ")";
                std::ostringstream contents2;
                contents2 << "(";
                for (auto &it : target2.contents) {
                    contents2 << " " << it << " ";
                }
                contents2 << ")";
                if (it.min < val && val < it.max) {
                    log_info("【pair %s~%s】normal %s~%s %f", target1.id, target2.id,
                             contents1.str(), contents2.str(), val);
                } else {
                    log_info("【pair %s~%s】attent %s~%s %f", target1.id, target2.id,
                             contents1.str(), contents2.str(), val);
                    // snd msg small
                    auto cur_seconds = hb::time::timestamp();
                    if (cur_seconds - it.send_msg_time >= sendmsg_seconds_) {
                        it.send_msg_time = cur_seconds;
                        log_info("【pair %s~%s】send msg value is %f", target1.id, target2.id, val);
                        // snd msg
                        auto &dingtalk = app().get_plugin<dingtalk_plugin>();
                        std::ostringstream subject;
                        subject << "[pair " << target1.id << "~" << target2.id << "]" << val
                                << contents1.str() << contents2.str();
                        dingtalk.send(subject.str() + "\n\rPlease pay attention to the pair !!!");
                    }
                }
            }
        }
    }  // namespace plugin
}  // namespace hb