#!/bin/bash

echo "Uninstalling Linux Monitoring..."

# Stop and disable the linux monitoring service
sudo systemctl stop linuxmonitoring
sudo systemctl disable linuxmonitoring

# Remove the linuxmonitoring systemd service
sudo rm /etc/systemd/system/linuxmonitoring.service
sudo systemctl daemon-reload

# Remove the Linux Monitoring binary and directory
sudo rm -rf /opt/linux_monitoring

# Uninstall tgbot-cpp
sudo rm -rf /opt/tgbot
sudo make uninstall -C /opt/tgbot || echo "tg-bot may have already been uninstalled"

# Uninstall dependencies
sudo apt remove --purge -y wget unzip g++ make binutils cmake libboost-system-dev libssl-dev zlib1g-dev libcurl4-openssl-dev nlohmann-json3-dev

# Clean up residual configuration and dependencies
sudo apt autoremove -y
sudo apt autoclean

# Remove log file if exists
sudo rm -f /var/log/linuxMonitoring.log

echo "Linux Monitoring and all associated files have been removed successfully!"
