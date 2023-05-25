#pragma once
#include "common.hpp"
namespace push
{
	constexpr auto LOGGER_PRETTY_TIME_FORMAT = "%Y-%m-%d %H:%M";
	constexpr auto LOGGER_PRETTY_MS_FORMAT = ".%03d";
	constexpr auto BUFFER_SIZE = 4096;

	// convert current time to milliseconds since unix epoch
	template <typename T>
	static int to_ms(const std::chrono::time_point<T>& tp)
	{
		using namespace std::chrono;
		auto dur = tp.time_since_epoch();
		return static_cast<int>(duration_cast<milliseconds>(dur).count());
	}

	// format it in two parts: main part with date and time and part with milliseconds
	static std::string pretty_time()
	{
		auto tp = std::chrono::system_clock::now();
		std::time_t current_time = std::chrono::system_clock::to_time_t(tp);

		std::tm time_info;
		localtime_s(&time_info, &current_time);

		char buffer[BUFFER_SIZE];

		int string_size = strftime(
			buffer, sizeof(buffer),
			LOGGER_PRETTY_TIME_FORMAT,
			&time_info
		);

		int ms = to_ms(tp) % 1000;

		string_size += snprintf(
			buffer + string_size, sizeof(buffer) - string_size,
			LOGGER_PRETTY_MS_FORMAT, ms
		);

		return std::string(buffer, buffer + string_size);
	}

	// General logging function
	inline void log(const std::string& level, const std::string& msg)
	{
		std::stringstream ss;
		ss << pretty_time() << " -- [" << level << "] " << msg << "\n";

		std::cout << ss.str();
	}

	// Specific logging functions
	inline void Log(const std::string& msg)
	{
		log("Info", msg);
	}

	inline void Log_warn(const std::string& msg)
	{
		log("Warn", msg);
	}

	inline void Log_debug(const std::string& msg)
	{
		log("Debug", msg);
	}

	inline void Log_Error(const std::string& msg)
	{
		log("Error", msg);
	}
}
