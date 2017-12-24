#pragma once

#include <logging.hpp>
#include <string>
#include <esp_log.h>

using namespace std::string_literals;

template < class StringT >
class ESPLogSinkT : public BaseLogSink < StringT >
{
public:
	ESPLogSinkT() { }


	virtual void log( Verbosity verb, const StringT & tag, const StringT & message,
	                  uint64_t timestamp ) override
	{
		switch (verb){
			case Verbosity::DEBUG: {
				ESP_LOGD(tag, "%", message);
				break;
			}
			case Verbosity::ERROR: {
				ESP_LOGE(tag, "%", message);
				break;
			}
			case Verbosity::WARNING: {
				ESP_LOGW(tag, "%", message);
				break;
			}
			default: {
				ESP_LOGI(tag, ":%", message);
				break;
			}
		}
	}

};

using ESPLogSink = ESPLogSinkT < std::string >;
