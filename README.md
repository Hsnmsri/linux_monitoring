# Linux Monitoring Service

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Recommendations](#recommendations)
4. [Development Environment Setup](#development-environment-setup)
5. [Running the Service](#running-the-service)
6. [Updating `settings.json`](#updating-settingsjson)
7. [Uninstalling the Program](#uninstalling-the-program)

---

## Introduction

The **Linux Monitoring Service** provides real-time monitoring of CPU and memory usage, with notifications sent via a Telegram bot. This service is designed to run in the background on Linux-based systems and automatically restart upon failure. Configuration settings are stored in the `settings.json` file, and users can modify thresholds and other parameters as needed.

---

## Installation

To install the Linux Monitoring Service, follow these steps:

1. Open a terminal.
2. Run the following command to download and install the service from GitHub:

   ```bash
   bash <(curl -Ls https://raw.githubusercontent.com/Hsnmsri/linux_monitoring/main/install.sh)
   ```

3. During installation, you will be prompted to provide the following details:
   - Telegram Bot Token: The token for your Telegram bot.
   - Chat ID: The chat ID where notifications will be sent.
   - CPU Check Duration (default: 500 ms): Interval for checking CPU usage.
   - Memory Check Duration (default: 500 ms): Interval for checking memory usage.
   - CPU Usage Limit (default: 30%): CPU usage threshold for sending alerts.
   - Memory Usage Limit (default: 30%): Memory usage threshold for sending alerts.
4. After installation, the service will automatically start and monitor your system's CPU and memory usage, sending alerts based on the configured thresholds.

## Recommendations

- Default Configuration: It is recommended to start with the default thresholds for CPU and memory usage (30%) and adjust them as needed based on your system's workload.
- Notifications: Ensure that your Telegram bot and chat ID are correctly configured to receive real-time alerts.
- System Resource Monitoring: You can view the resource usage via Telegram commands such as `/usage` to check CPU and memory status.

## Development Environment Setup

For developers looking to contribute to or modify the Linux Monitoring Service, here are the steps to set up the development environment:

1.  Clone the Repository: Clone the repository from GitHub:
    ```bash
    git clone https://github.com/Hsnmsri/linux_monitoring.git
    cd linux_monitoring
    ```
2.  Install Dependencies: Install the required dependencies using the following command:

3.  Build the Project: To build the project, you can use g++ or CMake (if a CMakeLists.txt is provided):
    ```bash
    g++ -o linux_monitoring main.cpp -lcurl -lboost_system -lssl -ltgbot -lpthread
    ```
4.  Run the Program: You can run the program locally using:
    ```bash
    ./linux_monitoring
    ```

## Running the Service

Once installed, the Linux Monitoring Service will automatically run as a background service. The service is managed via `systemd`, and you can interact with it using the following commands:

- Start the Service:

  ```bash
  sudo systemctl start linuxmonitoring
  ```

- Stop the Service:

  ```bash
  sudo systemctl stop linuxmonitoring
  ```

- Restart the Service:

  ```bash
  sudo systemctl restart linuxmonitoring
  ```

- Check Service Status:

  ```bash
  sudo systemctl status linuxmonitoring
  ```

## Updating `settings.json`

If you make changes to the `settings.json` file, such as updating thresholds or changing the bot token, you must restart the service for the changes to take effect.

1. dit the `settings.json` file:
   ```bash
   sudo nano /opt/linuxmonitoring/settings.json
   ```
2. After saving your changes, restart the service:
   ```bash
   sudo systemctl restart linuxmonitoring
   ```
   This ensures that the service picks up the latest changes.

## Uninstalling the Program

To completely remove the Linux Monitoring Service from your system, follow these steps:

1. Stop the Service:
   ```bash
   sudo systemctl stop linuxmonitoring
   ```
2. Disable the Service (to prevent it from starting on boot):
   ```bash
   sudo systemctl disable linuxmonitoring
   ```
3. Remove the Service Files:
   ```bash
   sudo rm /etc/systemd/system/linuxmonitoring.service
   sudo rm -r /opt/linuxmonitoring
   ```
4. Reload `systemd` to apply the changes:
   ```bash
   sudo systemctl daemon-reload
   ```
   This will remove all traces of the service from your system.
