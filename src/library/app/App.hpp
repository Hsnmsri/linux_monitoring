#pragma once

#include "log/Log.hpp"
#include "settings/Settings.hpp"
#include "cpu/CpuMonitor.hpp"
#include "memory/MemoryMonitor.hpp"
#include "telegram/TelegramMonitor.hpp"

class App
{
public:
    App();
    int execute();
    bool checkSetting();
    void printWelcome();

private:
    Log logger;
    Settings settings;
    bool isMonitoringEnable;

    void hold(CpuMonitor &cpu, MemoryMonitor &memory, TelegramMonitor &telegram);
};
