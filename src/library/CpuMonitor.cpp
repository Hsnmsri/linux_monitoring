#include "CpuMonitor.hpp"

CpuMonitor::CpuMonitor(int durationTimeToCheckMS) : durationTimeToCheckMS(durationTimeToCheckMS), lastCpuUsage(0.0) {}

void CpuMonitor::startMonitoring()
{
    monitorThread = std::thread(&CpuMonitor::thread_getCPUUsage, this);
    monitorThread.detach(); // Detach the thread so it runs in the background
}

void CpuMonitor::readCpuTimes(long long &user, long long &nice, long long &system, long long &idle)
{
    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open())
    {
        std::cerr << "Error opening /proc/stat" << std::endl;
        exit(1);
    }

    std::string line;
    std::getline(statFile, line); // Read the first line (the "cpu" line)
    std::istringstream iss(line);
    std::string cpuLabel;
    iss >> cpuLabel; // Skip the "cpu" label
    iss >> user >> nice >> system >> idle;

    statFile.close();
}

void CpuMonitor::thread_getCPUUsage()
{
    while (monitoringActive)
    {
        long long user1, nice1, system1, idle1;
        long long user2, nice2, system2, idle2;

        // Read initial CPU times
        readCpuTimes(user1, nice1, system1, idle1);

        // Sleep for a while to get a comparison period
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Read CPU times again after the sleep
        readCpuTimes(user2, nice2, system2, idle2);

        // Calculate differences
        long long total1 = user1 + nice1 + system1 + idle1;
        long long total2 = user2 + nice2 + system2 + idle2;

        long long totalDiff = total2 - total1;
        long long idleDiff = idle2 - idle1;

        // Calculate CPU usage percentage
        double cpuUsage = 100.0 * (totalDiff - idleDiff) / totalDiff;

        // Update the last CPU usage
        lastCpuUsage = cpuUsage;

        // Sleep for the CPU check duration from the settings
        std::this_thread::sleep_for(std::chrono::milliseconds(this->durationTimeToCheckMS));
    }
}
