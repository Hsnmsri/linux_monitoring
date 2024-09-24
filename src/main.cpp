#include <Log.hpp>
#include <Settings.hpp>
#include <CpuMonitor.hpp>
#include <MemoryMonitor.hpp>
#include <TelegramMonitor.hpp>

int app();

int main()
{
    try
    {
        return app();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}

int app()
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
        return 1;
    }

    logger.logToConsole("Linux Monitoring v" + settings.getAppVersion() + " Service Started");
    logger.logToConsole(settings.getServerName());
    // Monitoring Objects
    CpuMonitor cpu(settings.getCpuCheckDuration());
    MemoryMonitor memory(settings.getMemoryCheckDuration());
    TelegramMonitor telegram(isMonitoringEnable, cpu, memory, settings, logger);

    // Start Monitoring
    telegram.startTelegramRequestThread();

    // App holder
    while (true)
    {
        // Start Monitoring
        if (isMonitoringEnable)
        {
            cpu.startMonitoring();
            memory.startMonitoring();
            telegram.startTelegramNotificationWatchThread();
        }
        // Stop Monitoring
        if (!isMonitoringEnable)
        {
            cpu.stopMonitoring();
            memory.stopMonitoring();
            telegram.stopTelegramNotificationWatchThread();
        }
        sleep(1);
        continue;
    }
}