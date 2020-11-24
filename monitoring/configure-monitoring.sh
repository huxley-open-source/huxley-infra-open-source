echo 'create monitoring user'
adduser --disabled-password --gecos "Monitoring,21,," monitoring
echo "monitoring:huxley123!@#"|chpasswd

echo 'create ssh keys'
su - monitoring -c 'mkdir -p ~/.ssh && chmod -R 700 ~/.ssh'
su - monitoring -c 'ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa'

#ufw allow 3000/tcp
#ufw allow 8087/tcp

cp install-tools.sh /home/monitoring
chown monitoring:monitoring /home/monitoring/install-tools.sh

su monitoring -c 'cd ~/ && ./install-tools.sh'

echo 'coping grafana-server.env'
cp grafana-server.env /home/monitoring/tools/grafana
chown monitoring:monitoring /home/monitoring/tools/grafana/grafana-server.env

echo 'coping grafana.ini'
mkdir -p /home/monitoring/tools/grafana/conf
cp grafana.ini /home/monitoring/tools/grafana/conf
chown monitoring:monitoring /home/monitoring/tools/grafana/conf/grafana.ini

echo 'coping influxdb.conf'
cp influxdb.conf /home/monitoring/tools/influxdb/
chown monitoring:monitoring /home/monitoring/tools/influxdb/influxdb.conf

echo 'coping services'
cp grafana.service /etc/systemd/system
cp influxdb.service /etc/systemd/system

systemctl enable grafana
systemctl enable influxdb

systemctl start grafana
systemctl start influxdb

sleep 5

curl -i -XPOST http://localhost:8087/query --data-urlencode "q=CREATE USER user WITH PASSWORD '' WITH ALL PRIVILEGES"
curl -i -XPOST http://localhost:8087/query -u monitoring:senha-huxley-123 --data-urlencode "q=CREATE DATABASE huxley_monitoring"
curl -i -XPOST http://localhost:8087/query -u monitoring:senha-huxley-123 --data-urlencode "q=CREATE RETENTION POLICY "twenty_days" ON "huxley_monitoring" DURATION 20d REPLICATION 1 DEFAULT"
