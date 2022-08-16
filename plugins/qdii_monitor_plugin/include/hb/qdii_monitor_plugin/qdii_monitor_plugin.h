//
// Created by 李卫东 on 2019-02-19.
//
#pragma once

#include <hb/dingtalk_plugin/dingtalk_plugin.h>
#include <hb/thread_pool_plugin/thread_pool_plugin.h>
#include <hb/log_plugin/log_plugin.h>
#include <hb/qdii_monitor_plugin/qdii_monitor_plugin_impl.h>

#include <appbase/application.hpp>
#include <functional>
#include <string>

using namespace std;

namespace hb {
    namespace plugin {
        using namespace appbase;
        class qdii_monitor_plugin : public appbase::plugin<qdii_monitor_plugin> {
          public:
            APPBASE_PLUGIN_REQUIRES((log_plugin)(dingtalk_plugin)(thread_pool_plugin))
            qdii_monitor_plugin();
            virtual ~qdii_monitor_plugin();
            virtual void set_program_options(options_description&, options_description&) override;
            void plugin_initialize(const variables_map&);
            void plugin_startup();
            void plugin_shutdown();

          private:
            shared_ptr<qdii_monitor_plugin_impl> my;
        };
    }  // namespace plugin
}  // namespace hb
