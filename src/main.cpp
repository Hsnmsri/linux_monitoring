#include <Log.hpp>
#include <Settings.hpp>
#include <CpuMonitor.hpp>
#include <MemoryMonitor.hpp>
#include <TelegramMonitor.hpp>

int main()
{
    bool isMonitoringEnable = true;

    Log logger;
    Settings settings;

    logger.logToConsole("Linux Monitoring Service Started");

    // Get settings from setting file
    if (!settings.getSetting())
    {
        logger.logToConsole("Failed to set settings!");
        return 0;
    }

    // Monitoring Objects
    CpuMonitor cpu(settings.getCpuCheckDuration());
    MemoryMonitor memory(settings.getMemoryCheckDuration());
    TelegramMonitor telegram(isMonitoringEnable, cpu, memory, settings, logger);

    // Start Monitoring
    cpu.startMonitoring();
    memory.startMonitoring();
    telegram.startTelegramNotificationWatchThread();
    telegram.startTelegramRequestThread();

    while (isMonitoringEnable)
    {
        continue;
    }

    return 0;
}