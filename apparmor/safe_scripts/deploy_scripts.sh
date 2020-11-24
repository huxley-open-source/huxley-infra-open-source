#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $DIR/config.properties

cp -f config.properties $scripts_dir
cp -f config.properties $daemons_dir
cp -f *.run $scripts_dir
gcc -O3 -w huxley_jail.c -o huxley_jail
cp -f huxley_jail $scripts_dir
gcc -O3 -w kill_forks.c -o kill_forks `pkg-config --cflags glib-2.0 --libs glib-2.0`
cp -f kill_forks $scripts_dir

cp -f run_kill_forks.sh $daemons_dir

