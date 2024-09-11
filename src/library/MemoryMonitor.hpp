#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include "Settings.hpp" // Include your Settings class header
#include "Log.hpp"      // Include your Log class header

class MemoryMonitor
{
public:
    MemoryMonitor(int durationTimeToCheckMS, bool &isMonitoringEnable);
    void startMonitoring();
    double getLastMemoryUsage() const;

private:
    void thread_getMemoryUsage();

    int durationTimeToCheckMS;
    bool &isMonitoringEnable;
    double lastMemoryUsage;
    std::thread monitorThread;
};
