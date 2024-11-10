#pragma once

#include <tgbot/tgbot.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include "settings/Settings.hpp"
#include "log/Log.hpp"
#include "cpu/CpuMonitor.hpp"
#include "memory/MemoryMonitor.hpp"

class TelegramMonitor
{
public:
    TelegramMonitor(bool &isMonitoringEnable, CpuMonitor &cpu, MemoryMonitor &memory, const Settings settings, Log logger);

    void startTelegramRequestThread();
    void startTelegramNotificationWatchThread();
    void stopTelegramNotificationWatchThread();

private:
    void thread_telegramBot();
    void thread_telegramNotification();
    void handleStartCommand(TgBot::Message::Ptr message);
    void handleStopCommand(TgBot::Message::Ptr message);
    void handleUsageCommand(TgBot::Message::Ptr message);
    void handleHelpCommand(TgBot::Message::Ptr message);
    void handleStatusCommand(TgBot::Message::Ptr message);

    Log logger;
    Settings settings;
    CpuMonitor &cpu;
    MemoryMonitor &memory;
    bool &isMonitoringEnable;
    bool tgNotificationStatus;
    std::thread botRequestThread;
    std::thread notificationThread;
    TgBot::Bot bot;
};
