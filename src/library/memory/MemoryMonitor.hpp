#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include "settings/Settings.hpp" // Include your Settings class header
#include "log/Log.hpp"      // Include your Log class header

class MemoryMonitor
{
public:
    MemoryMonitor(int durationTimeToCheckMS);
    void startMonitoring();
    void stopMonitoring();
    double getLastMemoryUsage() const;

private:
    void thread_getMemoryUsage();

    bool monitoringMemoryStatus;
    int durationTimeToCheckMS;
    double lastMemoryUsage;
    std::thread monitorThread;
};
