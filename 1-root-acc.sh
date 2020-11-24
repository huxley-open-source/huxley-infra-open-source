#!/bin/bash
set -e
source variables.sh

if [ -z "$HUXLEY_PASSWORD" ]; then
 echo 'password zuq nao informado'
 exit 1
fi

echo 'create huxley user'
adduser --disabled-password --gecos "HUXLEY,21,," huxley
echo "huxley:$HUXLEY_PASSWORD"|chpasswd

echo 'create ssh keys'
su - huxley -c 'mkdir -p ~/.ssh && chmod -R 700 ~/.ssh'
su - huxley -c 'ssh-keygen -t rsa -N "" -f ~/.ssh/id_rsa'

adduser huxley sudo

echo 'configurando ssh'
cp sshd_config /etc/ssh/
service ssh restart

#./2-root-rabbit.sh: line 19: /usr/local/bin/rabbitmqadmin: Permission denied
#./configure-dev.sh: line 3: ./3-root-nsf-client.sh: No such file or directory
