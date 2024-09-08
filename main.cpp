#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>

void thread_getCPUUsage();
void thread_getMemoryUsage();

int CpuLimit = 5;

int main()
{
    // Create a new thread and run `threadFunction` on it
    std::thread cpuMonitorThread(thread_getCPUUsage);
    std::thread memoryMonitorThread(thread_getMemoryUsage);
    // std::thread slogThread(threadTwo);

    // Wait for the thread to finish
    cpuMonitorThread.join();
    memoryMonitorThread.join();
    // slogThread.join();

    std::cout << "Hello from the main thread!" << std::endl;
    return 0;
}

void thread_getCPUUsage()
{
    while (true)
    {
        long long user1, nice1, system1, idle1;
        long long user2, nice2, system2, idle2;

        // Helper function to read CPU times from /proc/stat
        auto readCpuTimes = [](long long &user, long long &nice, long long &system, long long &idle)
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
        };

        // Read initial CPU times
        readCpuTimes(user1, nice1, system1, idle1);

        // Sleep for a while to get a comparison period
        sleep(1);

        // Read CPU times again after the sleep
        readCpuTimes(user2, nice2, system2, idle2);

        // Calculate differences
        long long total1 = user1 + nice1 + system1 + idle1;
        long long total2 = user2 + nice2 + system2 + idle2;

        long long totalDiff = total2 - total1;
        long long idleDiff = idle2 - idle1;

        // Calculate CPU usage percentage
        double cpuUsage = 100.0 * (totalDiff - idleDiff) / totalDiff;

        if (CpuLimit != 0 && cpuUsage >= CpuLimit)
        {
            std::cout << "CPU Usage: " << cpuUsage << "%" << std::endl;
        }

        // Sleep for 1 second before the next measurement
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void thread_getMemoryUsage()
{
    while (true)
    {
        std::ifstream memInfoFile("/proc/meminfo");
        if (!memInfoFile.is_open())
        {
            std::cerr << "Error opening /proc/meminfo" << std::endl;
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
            else if (key == "MemFree:")
            {
                freeMemory = value;
            }
        }

        memInfoFile.close();

        if (totalMemory == 0)
        {
            std::cerr << "Total memory not found in /proc/meminfo" << std::endl;
            return;
        }

        // Calculate used memory and memory usage percentage
        long long usedMemory = totalMemory - freeMemory;
        double memoryUsagePercent = 100.0 * usedMemory / totalMemory;

        std::cout << "Memory Usage: " << memoryUsagePercent << "%" << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}