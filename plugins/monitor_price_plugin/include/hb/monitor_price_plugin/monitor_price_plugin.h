//
// Created by 李卫东 on 2019-02-19.
//
#pragma once

#include <functional>
#include <string>
#include <appbase/application.hpp>
#include <hb/log_plugin/log_plugin.h>
#include <hb/send_mail_plugin/send_mail_plugin.h>
#include <hb/monitor_price_plugin/monitor_price_plugin_impl.h>

using namespace std;

namespace hb{ namespace plugin {
    using namespace appbase;
    class monitor_price_plugin : public appbase::plugin<monitor_price_plugin> {
        public:
            APPBASE_PLUGIN_REQUIRES((log_plugin)(send_mail_plugin))
            monitor_price_plugin();
            virtual ~monitor_price_plugin();
            virtual void set_program_options(options_description&, options_description&) override;
            void plugin_initialize(const variables_map&);
            void plugin_startup();
            void plugin_shutdown();
        private:
            shared_ptr<monitor_price_plugin_impl> my;
    };
}}
