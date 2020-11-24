#!/bin/bash
set -e
source variables.sh

apt-get install nfs-kernel-server -y
echo "/home/huxley/data/testcases $DEV_IP(ro,sync)" > /etc/exports
exportfs -a
service nfs-kernel-server start
