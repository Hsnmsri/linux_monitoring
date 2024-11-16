#pragma once

#include "log/Log.hpp"
#include "settings/Settings.hpp"
#include "cpu/CpuMonitor.hpp"
#include "memory/MemoryMonitor.hpp"
#include "telegram/TelegramMonitor.hpp"
#include "node/Node.hpp"

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
    Node nodes;
    bool isMonitoringEnable;

    void hold(CpuMonitor &cpu, MemoryMonitor &memory, TelegramMonitor &telegram);
};
