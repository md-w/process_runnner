#ifndef elapsed_time_h
#define elapsed_time_h

#include <iostream>
#include <string>
#include <string.h>
#include <chrono>

#include "vlogger.h"

#define LOGGER "elapsed_time"

class elapsed_time
{
public:
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	double &_processingTickTime;
	std::string _moduleName;

	elapsed_time(double &processingTickTime, const std::string &moduleName)
		: _processingTickTime(processingTickTime), _moduleName(moduleName)
	{
		
	}

	~elapsed_time()
	{
		auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
		_processingTickTime += total_duration;

		// log
		char log_buff[LOG_BUFF_MAX_LEN];
		memset(log_buff, 0, LOG_BUFF_MAX_LEN);
		snprintf (log_buff, LOG_BUFF_MAX_LEN, "time taken by module: %s : %lld micros.", _moduleName.data(), total_duration);
		vtpl::vl_log(vtpl::en_vlogger_level::vl_debug, (char*)LOG_FORMAT(LOGGER), log_buff);
		
		
	}
};
#endif // !elapsed_time_h