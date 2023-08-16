#pragma once

#include <tegia/tegia.h>
#include "Helper.h"

#if defined(DEVELOPER_VERSION)
#define DOUT(text) std::cout << text << std::endl;     
#else
#define DOUT(text)     
#endif

#define EOUT(text) DOUT(_ERR_TEXT_ << __func__  << ": " << text)
#define NOUT(text) DOUT(_OK_TEXT_ << __func__  << ": " << text)

namespace tegia::sockets
{
    inline void publish_user_notification(int const code, std::optional<std::string> const& message)
    {
        if (code == SUCCESS)
        {
            if (message.has_value())
            {
                LNOTICE(*message)
                std::cout << _OK_TEXT_ << *message << '\n';
            }
        }
        else if (message.has_value())
        {
            LERROR(*message)
            std::cout << _ERR_TEXT_ << *message << '\n';
        }
        else
        {
            auto const str = fmt::format("Error #{} happened.", code);
            LERROR(str)
            std::cout << _ERR_TEXT_ << str << '\n';
        }
    }

    inline void publish_user_notification(error_data const& notification)
    {
        publish_user_notification(notification.first, notification.second);
    }

}