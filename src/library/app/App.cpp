#include "App.hpp"

App::App(){}

int App::execute()
{
    this->checkSetting();

    // set monitoring default status
    this->isMonitoringEnable = this->settings.getDefaultMonitoringStatus();

    this->printWelcome();

    // Monitoring Objects
    CpuMonitor cpu(settings.getCpuCheckDuration());
    MemoryMonitor memory(settings.getMemoryCheckDuration());
    TelegramMonitor telegram(isMonitoringEnable, cpu, memory, settings, logger);

    // Start Monitoring
    telegram.startTelegramRequestThread();

    // hold app
    this->hold(cpu, memory, telegram);
}

bool App::checkSetting()
{
    // load settings
    if (settings.getSetting())
    {
        return true;
    }

    logger.logToConsole("Failed to load the settings file. Attempting to rebuild configuration...");

    // build settings file
    if (settings.createSettingsFile())
    {
        logger.logToConsole("Settings initializing successfully!");
        settings.getSetting();
        return true;
    }

    return false;
}

void App::printWelcome()
{
    this->logger.logToConsole("Linux Monitoring v" + this->settings.getAppVersion() + " Service Started");
    this->logger.logToConsole(this->settings.getServerName());
}

void App::hold(CpuMonitor &cpu, MemoryMonitor &memory, TelegramMonitor &telegram)
{
    while (true)
    {
        // Start Monitoring
        if (this->isMonitoringEnable)
        {
            cpu.startMonitoring();
            memory.startMonitoring();
            telegram.startTelegramNotificationWatchThread();
        }
        // Stop Monitoring
        if (!this->isMonitoringEnable)
        {
            cpu.stopMonitoring();
            memory.stopMonitoring();
            telegram.stopTelegramNotificationWatchThread();
        }
        sleep(1);
        continue;
    }
}