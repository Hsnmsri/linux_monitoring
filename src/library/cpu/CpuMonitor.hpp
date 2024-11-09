#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include "settings/Settings.hpp"

class CpuMonitor
{
public:
    CpuMonitor(int durationTimeToCheckMS);

    // Starts the thread to monitor CPU usage
    void startMonitoring();
    
    // Stop the thread to monitor CPU usage
    void stopMonitoring();

    // Gets the last recorded CPU usage
    double getLastCpuUsage() const { return lastCpuUsage; }

private:
    // Function that runs in a thread to get CPU usage
    void thread_getCPUUsage();

    // Reads CPU times from /proc/stat
    void readCpuTimes(long long &user, long long &nice, long long &system, long long &idle);

    bool monitoringCpuStatus;
    int durationTimeToCheckMS;
    std::atomic<double> lastCpuUsage;
    std::thread monitorThread;
};
