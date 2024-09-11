#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include "Settings.hpp" // Include the settings class

class CpuMonitor
{
public:
    CpuMonitor(int durationTimeToCheckMS, bool &isMonitoringEnable);

    // Starts the thread to monitor CPU usage
    void startMonitoring();

    // Gets the last recorded CPU usage
    double getLastCpuUsage() const { return lastCpuUsage; }

private:
    // Function that runs in a thread to get CPU usage
    void thread_getCPUUsage();

    // Reads CPU times from /proc/stat
    void readCpuTimes(long long &user, long long &nice, long long &system, long long &idle);

    bool &isMonitoringEnable;
    int durationTimeToCheckMS;
    std::atomic<double> lastCpuUsage;
    std::thread monitorThread;
};
