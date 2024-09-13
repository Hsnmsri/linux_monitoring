# update apt package list
sudo apt update

# install wget
sudo apt install -y wget

# install lib curl
sudo apt install -y libcurl4-openssl-dev

# install and build tg-bot lib
sudo apt install -y make binutils libboost-system-dev libssl-dev zlib1g-dev

# install nlohmann json
sudo apt install -y nlohmann-json3-dev

# install Linux Monitoring
mkdir -p /opt/linux_monitoring
cd /opt/linux_monitoring
wget https://github.com/Hsnmsri/linux_monitoring/releases/download/v1.1.0/LinuxMonitoring
sudo chmod +x /opt/linux_monitoring/LinuxMonitoring

# create settings
read -p "Enter your Telegram bot api token (from @BotFather): " bot_token
read -p "Enter your chat ID (from @useridinfobot): " chat_id
read -p "Enter CPU check duration (in milliseconds, default: 500): " cpu_check_duration
read -p "Enter memory check duration (in milliseconds, default: 500): " memory_check_duration
read -p "Enter CPU usage limit percentage for send notification (default: 30): " cpu_limit
read -p "Enter memory usage limit percentage for send notification (default: 30): " memory_limit

# set deafult settings
cpu_check_duration=${cpu_check_duration:-500}
memory_check_duration=${memory_check_duration:-500}
cpu_limit=${cpu_limit:-30}
memory_limit=${memory_limit:-30}

# create settings.json file
cd /opt/linux_monitoring
cat <<EOF >settings.json
{
  "version":"1.1.0",
  "bot_token": "$bot_token",
  "chat_id": $chat_id,
  "cpu_check_duration": $cpu_check_duration,
  "memory_check_duration": $memory_check_duration,
  "cpu_limit": $cpu_limit,
  "memory_limit": $memory_limit
}
EOF

# initializing systemd service
echo "[Unit]
Description=Linux Monitoring Service
After=network.target

[Service]
ExecStart=/opt/linux_monitoring/LinuxMonitoring
Restart=always
RestartSec=10
WorkingDirectory=/opt/linux_monitoring
StandardOutput=append:/var/log/linuxMonitoring.log
StandardError=append:/var/log/linuxMonitoring.log
User=root
Environment=PATH=/usr/bin:/usr/local/bin
Environment=NODE_ENV=production

[Install]
WantedBy=multi-user.target" | sudo tee /etc/systemd/system/linuxmonitoring.service >/dev/null

# reload daemon and enable service
sudo systemctl daemon-reload
sudo systemctl enable linuxmonitoring

# start linux monitoring service
sudo systemctl start linuxmonitoring

echo "Install Linux Monitoring Successfully!"