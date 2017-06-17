#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>
#include "Arduino.h"

class StopWatch
{
public:
	/**
	 * Initializes stop watch and if true is passed, starts the watch
	 */
	StopWatch(bool start = true)
	{
		reset(start);
	}

	void start()
	{
		is_running = true;
		start_time = std::chrono::high_resolution_clock::now();
	}

	void stop()
	{
		is_running = false;
		stop_time = std::chrono::high_resolution_clock::now();
	}

	bool isRunning()
	{
		return is_running;
	}

	void reset(bool start = true)
	{
		is_running = start;
		start_time = std::chrono::high_resolution_clock::now();
		stop_time = std::chrono::high_resolution_clock::now();
	}

	int getMs()
	{
		if (is_running)
			stop_time = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count();
	}

	int getUs()
	{
		if (is_running)
			stop_time = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count();
	}

private:
	typedef typename std::chrono::high_resolution_clock::time_point time_point;
	bool is_running;
	time_point start_time;
	time_point stop_time;
};

#endif //STOPWATCH_H
