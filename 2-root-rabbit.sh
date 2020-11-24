if ! hash rabbitmq-server 2>/dev/null; then
	echo 'deb http://www.rabbitmq.com/debian/ testing main' |
     sudo tee /etc/apt/sources.list.d/rabbitmq.list

	wget -O- https://www.rabbitmq.com/rabbitmq-release-signing-key.asc |
	     sudo apt-key add -

	sudo apt-get update

	sudo apt-get -y install rabbitmq-server
else
	echo 'rabbit already installed'
fi

rabbitmq-plugins enable rabbitmq_management

cd /usr/local/bin
wget https://raw.githubusercontent.com/rabbitmq/rabbitmq-management/rabbitmq_v3_6_12/bin/rabbitmqadmin
chmod +x rabbitmqadmin
cd -
rabbitmqadmin -q import rabbit.json
