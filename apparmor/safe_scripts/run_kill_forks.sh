#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $DIR/config.properties

echo "kill_forks is running"
while true
do
    $scripts_dir/kill_forks >> $base_dir/log/kill_forks.log 2>&1
    sleep $kill_interval
done
echo "kill_forks stopped"