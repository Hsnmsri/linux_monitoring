#include "MemoryMonitor.hpp"

MemoryMonitor::MemoryMonitor(int durationTimeToCheckMS, bool &isMonitoringEnable)
    : durationTimeToCheckMS(durationTimeToCheckMS), isMonitoringEnable(isMonitoringEnable), lastMemoryUsage(0.0) {}

/**
 * @brief Starts monitoring memory usage by launching a background thread.
 *
 * This function creates a new thread that executes the `thread_getMemoryUsage` method,
 * which is responsible for continuously monitoring memory usage. The thread is detached
 * to allow it to run independently in the background without blocking the main thread.
 *
 * The `thread_getMemoryUsage` method performs periodic checks on the system's memory usage
 * and updates relevant metrics. By detaching the thread, the function ensures that the
 * monitoring continues in the background as long as the application is running, without
 * requiring explicit thread management or synchronization from the main thread.
 *
 * Note:
 * - The `monitorThread` is a member variable of the `MemoryMonitor` class that holds the
 *   thread object.
 * - The `thread_getMemoryUsage` method should handle its own error checking and ensure
 *   proper termination conditions to avoid potential issues like resource leaks.
 *
 * Example:
 * @code
 * MemoryMonitor memoryMonitor;
 * memoryMonitor.startMonitoring();
 * // Memory monitoring starts in the background and continues until the application exits.
 * @endcode
 */
void MemoryMonitor::startMonitoring()
{
    monitorThread = std::thread(&MemoryMonitor::thread_getMemoryUsage, this);
    monitorThread.detach(); // Detach the thread so it runs in the background
}

/**
 * @brief Retrieves the most recent memory usage percentage.
 *
 * This method returns the last recorded memory usage percentage as a `double`.
 * It provides a way to access the most up-to-date memory usage value that has been
 * calculated and stored by the `MemoryMonitor` class.
 *
 * The value returned by this method is the result of the most recent memory
 * monitoring operation performed by the `thread_getMemoryUsage` method. This
 * method is useful for getting the current state of memory usage for reporting or
 * analysis purposes.
 *
 * @return The last recorded memory usage percentage as a `double`.
 *
 * Example:
 * @code
 * MemoryMonitor memoryMonitor;
 * // Assume memoryMonitor is started and running
 * double currentMemoryUsage = memoryMonitor.getLastMemoryUsage();
 * std::cout << "Current Memory Usage: " << currentMemoryUsage << "%" << std::endl;
 * @endcode
 */
double MemoryMonitor::getLastMemoryUsage() const
{
    return lastMemoryUsage;
}

/**
 * @brief Continuously monitors the memory usage of the system.
 *
 * This method runs in a separate thread and periodically reads memory statistics
 * from the `/proc/meminfo` file to calculate the current memory usage percentage.
 * It performs the following tasks:
 *
 * 1. Checks if monitoring is enabled. If not, it sleeps for a specified duration
 *    and continues to the next iteration.
 * 2. Opens the `/proc/meminfo` file to read memory information.
 * 3. Parses the file to extract the total and available memory.
 * 4. Calculates the used memory and the memory usage percentage.
 * 5. Updates the `lastMemoryUsage` member variable with the calculated percentage.
 * 6. Sleeps for a specified duration between checks, as defined by `durationTimeToCheckMS`.
 *
 * This method will run indefinitely in a loop until the program terminates. If the file
 * cannot be opened or the total memory cannot be found, an error message is printed,
 * and the method returns.
 *
 * Note: This method uses blocking operations (file I/O and sleep) and should be run in
 * a separate thread to avoid blocking the main application.
 *
 * Example usage:
 * @code
 * MemoryMonitor memoryMonitor;
 * // Start monitoring in a separate thread
 * std::thread monitorThread(&MemoryMonitor::thread_getMemoryUsage, &memoryMonitor);
 * monitorThread.detach();
 * // Continue with other tasks
 * @endcode
 */
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
