mkdir -p ~/tools
cd ~/tools

echo 'downloading'
wget http://zuq01.zuq.com.br/files/grafana-4.0.2-1481203731.linux-x64.tar.gz
wget http://zuq01.zuq.com.br/files/influxdb-1.1.1_linux_amd64.tar.gz

echo 'extracting grafana'
tar -xzf grafana-4.0.2-1481203731.linux-x64.tar.gz

echo 'extracting influx'
tar -xzf influxdb-1.1.1_linux_amd64.tar.gz

echo 'rename influx folder'
mv influxdb-1.1.1-1 influxdb

echo 'rename grafana folder'
mv grafana-4.0.2-1481203731 grafana
