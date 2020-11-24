#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $DIR/config.properties

file_name=$scripts_dir

# define o número maximo de processos como sendo o numero atual de processos mais 500. Evitando um forkbomb
nproc_value=`ps aux | wc -l`
nproc_value=$(($nproc_value + 400))

# remove a primeira barra  ${string/substring/replacement}
file_name=${file_name/\//}

# troca as barras seguintes por ponto ${string//substring/replacement}
file_name=${file_name//\//.}

file_name=$file_name.$file_to_jail

echo "#include <tunables/global>
$scripts_dir/$file_to_jail {
    #include <abstractions/base>

    $scripts_dir/$file_to_jail mixr,

	$temp_dir/** ixmr,
	# 100 segundos caso o huxley_jail falhe
	set rlimit cpu <= 100,
	
	# no máximo 30 megas para cada execucao

	
	# Não consegui setar esse valor porque a JVM
	# dá pau se vc colocar qualquer valor.
	# set rlimit as <= 512M,

	set rlimit nproc <= $nproc_value,



    /dev/tty mrw,

    # OCTAVE
    /usr/bin/octave* ixr,
    /usr/share/octave/** r,
    /usr/lib/@{multiarch}/octave/** mr,


    # PYTHON
    /usr/bin/python* ixr,
    /usr/local/lib/python*/dist-packages/ r,
    /etc/python*/sitecustomize.py r,

    # JAVA
    /usr/lib/jvm/java*/jre/bin/java mixr,
    /home/huxley/tools/java/jdk*/** mixr,
    /etc/java-*/** r,
    /sys/devices/system/cpu/ r,
    /proc/*/coredump_filter rw,
    /etc/nsswitch.conf r,
    / r,
    /tmp/** wr,
    /usr/share/java/** r,


}" > app_armor.huxley_jail

if [ -r $apparmor_dir/$file_name ]; then
	sudo rm $apparmor_dir/$file_name
fi

sudo cp app_armor.huxley_jail $apparmor_dir/$file_name


echo "sudo aa-enforce $apparmor_dir/$file_name" > start_jail.sh
chmod +x start_jail.sh

echo "sudo aa-complain $apparmor_dir/$file_name" > stop_jail.sh
chmod +x stop_jail.sh

echo "sudo apparmor_parser -r $apparmor_dir/$file_name" > reload_jail.sh
chmod +x reload_jail.sh

echo "sudo ln -s $apparmor_dir/$file_name $apparmor_dir/disable/
sudo apparmor_parser -R $apparmor_dir/$file_name" > disable_jail.sh
chmod +x disable_jail.sh
