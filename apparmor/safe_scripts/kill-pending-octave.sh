find /proc -maxdepth 1 -user huxley -type d -mmin +1  -exec basename {} \; | xargs ps | grep octave | awk '{ print $1 }' | xargs kill -9
