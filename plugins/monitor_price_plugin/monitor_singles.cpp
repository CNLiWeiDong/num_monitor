//
// Created by 李卫东 on 2019-02-19.
//
#include <hb/http/http.h>
#include <hb/https/https.h>
#include <hb/monitor_price_plugin/monitor_singles.h>
#include <hb/send_mail_plugin/send_mail_plugin.h>
#include <math.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <sstream>

namespace hb {
    namespace plugin {

        void monitor_singles::deal(const shared_ptr<monitor_targets> &targets) {
            log_info("begin monitor_single");
            auto &all_targets_list = targets->all_tagets();
            for (auto &it : singles_list_) {
                if (!it.active) continue;
                auto &target = all_targets_list[it.id];
                if (target.status != target_status::target_status_ok) {
                    log_info("【single %s】status wrong %d", target.id, target.status);
                    continue;
                }
                std::ostringstream contents_streame;
                contents_streame << "(";
                for (auto &it : target.contents) {
                    contents_streame << " " << it << " ";
                }
                contents_streame << ")";
                if (it.min < target.value && target.value < it.max) {
                    log_info("【single %s】normal %f %s", target.id, target.value,
                             contents_streame.str());
                } else {
                    log_info("【single %s】attent %f %s", target.id, target.value,
                             contents_streame.str());
                    auto cur_seconds = hb::time::timestamp();
                    if (cur_seconds - it.send_msg_time >= sendmsg_seconds_) {
                        it.send_msg_time = cur_seconds;
                        log_info("【single %s】send msg single value is %f %s", target.id,
                                 target.value, contents_streame.str());
                        auto &mail_plugin = app().get_plugin<send_mail_plugin>();
                        std::ostringstream subject;
                        subject << "[single " << target.id << "]" << target.value
                                << contents_streame.str();
                        mail_plugin.send_mail(
                            it.mail, subject.str(),
                            subject.str() + "\n\rPlease pay attention to the single !!!");
                    }
                }
            }
        }
    }  // namespace plugin
}  // namespace hb