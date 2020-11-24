#Execution scripts exit codes

the return code show what happened:

101 missing parameter
102 source file was not found or it is not readable 
103 huxley_jail was not found or it is not readable
104 gcc was not found or it is not readable
105 compilation error
106 javac was not found or it is not readable
107 java was not found or it is not readable
108 python was not found or it is not readable
109 octave was not found or it is not readable
110 g++ was not found or it is not readable
111 pascal (fpc) was not found or it is not readable

0 Normal execution
1 INCORRECT_INVOCATION,  (this is when the huxley_jails is called using the wrong parameters)
2 RUNTIME_ERROR,
3 TIME_OUT,
4 CPU_LIMIT_ERROR, (when it is not possible to set time limit constraints)
5 UNABLE_TO_EXECUTE_ERROR

Important: if there is a change of any order on these codes, the SoluctionRunner should be revised.


# Apparmor configuration instructions:

* Make sure that all scripts (*.run) files are in the home/huxley/data/scripts folder
* Open "configure_apparmor.sh" and check if the variables are pointing to the right location
* run "configure_apparmor.sh"
* Open another terminal and type tail -f /var/log/kern.log to see the apparmor log
* run ./start_jail.sh
* Open C.run.tests.sh and check the variable "dest_dir"
* run C.run.tests.sh to perform some tests on your just intalled huxley_jail


* Install a crontab task:
crontab -e
The content must be:
* * * * * /home/huxley/data/scripts/kill_forks >> /home/huxley/data/log/kill_forks.log 2>&1
