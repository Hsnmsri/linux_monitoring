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
    MemoryMonitor(int durationTimeToCheckMS);
    void startMonitoring();
    double getLastMemoryUsage() const;

private:
    void thread_getMemoryUsage();

    int durationTimeToCheckMS;
    double lastMemoryUsage;
    std::thread monitorThread;
    bool monitoringActive = true;
};
