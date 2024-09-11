#include "MemoryMonitor.hpp"

MemoryMonitor::MemoryMonitor(int durationTimeToCheckMS, bool &isMonitoringEnable)
    : durationTimeToCheckMS(durationTimeToCheckMS), isMonitoringEnable(isMonitoringEnable), lastMemoryUsage(0.0) {}

void MemoryMonitor::startMonitoring()
{
    monitorThread = std::thread(&MemoryMonitor::thread_getMemoryUsage, this);
    monitorThread.detach(); // Detach the thread so it runs in the background
}

double MemoryMonitor::getLastMemoryUsage() const
{
    return lastMemoryUsage;
}

void MemoryMonitor::thread_getMemoryUsage()
{
    while (true)
    {
        // if monitoring disable
        if (!this->isMonitoringEnable)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        std::ifstream memInfoFile("/proc/meminfo");
        if (!memInfoFile.is_open())
        {
            std::cout << "Error opening /proc/meminfo" << std::endl;
            return;
        }

        std::string line;
        long long totalMemory = 0;
        long long freeMemory = 0;

        while (std::getline(memInfoFile, line))
        {
            std::istringstream iss(line);
            std::string key;
            long long value;
            std::string unit;

            iss >> key >> value >> unit;

            if (key == "MemTotal:")
            {
                totalMemory = value;
            }
            if (key == "MemAvailable:")
            {
                freeMemory = value;
            }
        }

        memInfoFile.close();

        if (totalMemory == 0)
        {
            std::cout << "Total memory not found in /proc/meminfo" << std::endl;
            return;
        }

        // Calculate used memory and memory usage percentage
        long long usedMemory = totalMemory - freeMemory;
        double memoryUsagePercent = 100.0 * usedMemory / totalMemory;

        // Update memory usage
        lastMemoryUsage = memoryUsagePercent;

        // Sleep for the memory check duration from the settings
        std::this_thread::sleep_for(std::chrono::milliseconds(this->durationTimeToCheckMS));
    }
}
