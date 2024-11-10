#include "CpuMonitor.hpp"

CpuMonitor::CpuMonitor(int durationTimeToCheckMS) : durationTimeToCheckMS(durationTimeToCheckMS), lastCpuUsage(0.0), monitoringCpuStatus(false) {}

/**
 * @brief Starts monitoring the CPU usage.
 *
 * This function initializes a new thread that runs the `thread_getCPUUsage` method.
 * The `thread_getCPUUsage` method will continuously check and record CPU usage in the background.
 * The thread is detached so it continues running independently of the main program flow.
 *
 * The `std::thread` object `monitorThread` is used to handle the concurrent execution of the
 * CPU monitoring task. By detaching the thread, we allow it to run asynchronously, which means
 * it operates in the background and does not block the main program.
 *
 * It is important to ensure that the `thread_getCPUUsage` method is thread-safe and does not
 * access any shared resources without proper synchronization to avoid data races.
 */
void CpuMonitor::startMonitoring()
{
    // if monitoring enabled
    if (this->monitoringCpuStatus)
    {
        return;
    }

    // if monitoring disabled
    this->monitoringCpuStatus = true;
    monitorThread = std::thread(&CpuMonitor::thread_getCPUUsage, this);

    // Detach the thread so it runs in the background
    monitorThread.detach();
}

/**
 * @brief Reads CPU time statistics from /proc/stat.
 *
 * This function reads and parses CPU time information from the file `/proc/stat`, which is a
 * virtual file that provides detailed information about the CPU usage.
 *
 * The function extracts the following values:
 * - `user`: Time the CPU has spent in user mode.
 * - `nice`: Time the CPU has spent in user mode with low priority (nice).
 * - `system`: Time the CPU has spent in system mode (kernel).
 * - `idle`: Time the CPU has spent in idle mode.
 *
 * It opens the `/proc/stat` file, reads the first line which starts with the label "cpu",
 * and then parses the subsequent values into the provided reference parameters.
 *
 * If the file cannot be opened, it prints an error message to `std::cerr` and exits the program
 * with a non-zero status code.
 *
 * @param user Reference to a long long where the user CPU time will be stored.
 * @param nice Reference to a long long where the nice CPU time will be stored.
 * @param system Reference to a long long where the system CPU time will be stored.
 * @param idle Reference to a long long where the idle CPU time will be stored.
 */
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

/**
 * @brief Monitors CPU usage in a separate thread.
 *
 * This function continuously monitors the CPU usage in a background thread. It performs the following steps:
 *
 * 1. Checks if monitoring is enabled. If not, it waits for 1 second before checking again.
 *
 * 2. Reads the initial CPU times from `/proc/stat` using the `readCpuTimes` function.
 *
 * 3. Sleeps for a specified period (1 second) to allow time to pass for comparison.
 *
 * 4. Reads the CPU times again after the sleep period.
 *
 * 5. Calculates the difference in CPU times to determine the amount of time the CPU was idle and the total CPU time.
 *
 * 6. Computes the CPU usage percentage using the formula:
 *    \[
 *    \text{cpuUsage} = 100.0 \times \frac{(\text{totalDiff} - \text{idleDiff})}{\text{totalDiff}}
 *    \]
 *    where `totalDiff` is the difference in total CPU time and `idleDiff` is the difference in idle time.
 *
 * 7. Updates the `lastCpuUsage` variable with the calculated CPU usage percentage.
 *
 * 8. Sleeps for the duration specified in `this->durationTimeToCheckMS` before repeating the process.
 *
 * The function runs in an infinite loop, continuously updating the CPU usage until monitoring is disabled.
 *
 * The monitoring interval and CPU check duration are controlled by settings provided to the `CpuMonitor` object.
 */
void CpuMonitor::thread_getCPUUsage()
{
    while (this->monitoringCpuStatus)
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

/**
 * @brief Stops the CPU monitoring process.
 *
 * This function sets the `monitoringCpuStatus` flag to `false`,
 * indicating that the CPU monitoring should be stopped. Any ongoing
 * monitoring activities will cease, and the system will no longer
 * collect or process CPU usage data.
 *
 * This method should be called when monitoring is no longer needed
 * to ensure that resources are released and the monitoring process
 * is gracefully terminated.
 */
void CpuMonitor::stopMonitoring()
{
    this->monitoringCpuStatus = false;
}