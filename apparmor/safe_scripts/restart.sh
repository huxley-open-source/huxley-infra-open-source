#!/bin/bash

./configure_apparmor.sh
./reload_jail.sh
./start_jail.sh
/home/rodrigo/data/scripts/Java.run ~/data/submissions/2/53/Java/166/tres_numeros_correct.java /home/rodrigo/data/temp/3numeros.in 2
#/home/rodrigo/data/scripts/huxley_jail -t1 -i/home/rodrigo/data/temp/1405600884522Input53 /home/huxley/data/submissions/17/53/Octave/46/run_octave_6d011c83c9f7069d98f624ce9e98da45.sh