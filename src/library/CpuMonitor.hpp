#ifndef CPUMONITOR_HPP
#define CPUMONITOR_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include "Settings.hpp" // Include the settings class

class CpuMonitor {
public:
    CpuMonitor(Settings& settings);

    // Starts the thread to monitor CPU usage
    void startMonitoring();

    // Gets the last recorded CPU usage
    double getLastCpuUsage() const { return lastCpuUsage; }

private:
    // Function that runs in a thread to get CPU usage
    void thread_getCPUUsage();

    // Reads CPU times from /proc/stat
    void readCpuTimes(long long& user, long long& nice, long long& system, long long& idle);

    std::atomic<double> lastCpuUsage;
    Settings& settings;  // Reference to settings object
    std::thread monitorThread;
    bool monitoringActive = true;  // Controls the monitoring loop
};

#endif // CPUMONITOR_HPP
