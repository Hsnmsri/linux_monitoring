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

# Clean up residual configuration and dependencies
sudo apt autoremove -y
sudo apt autoclean

# Remove log file if exists
sudo rm -f /var/log/linuxMonitoring.log

echo "Linux Monitoring and all associated files have been removed successfully!"
