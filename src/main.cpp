#include <Log.hpp>
#include <Settings.hpp>
#include <CpuMonitor.hpp>
#include <MemoryMonitor.hpp>
#include <TelegramMonitor.hpp>

int main()
{
    // Monitoring Status
    bool isMonitoringEnable = false;

    // Objects
    Log logger;
    Settings settings;

    // Get settings from setting file
    if (!settings.getSetting())
    {
        logger.logToConsole("Failed to set settings!");
        return 0;
    }

    logger.logToConsole("Linux Monitoring v" + settings.getAppVersion() + " Service Started");
    logger.logToConsole(settings.getServerName());
    // Monitoring Objects
    CpuMonitor cpu(settings.getCpuCheckDuration(), isMonitoringEnable);
    MemoryMonitor memory(settings.getMemoryCheckDuration(), isMonitoringEnable);
    TelegramMonitor telegram(isMonitoringEnable, cpu, memory, settings, logger);

    // Start Monitoring
    cpu.startMonitoring();
    memory.startMonitoring();
    telegram.startTelegramNotificationWatchThread();
    telegram.startTelegramRequestThread();

    // App holder
    while (true)
    {
        sleep(60);
        continue;
    }

    return 0;
}