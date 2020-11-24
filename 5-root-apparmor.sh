apt-get -y install apparmor apparmor-profiles apparmor-utils
/etc/init.d/apparmor start

mkdir -p /home/huxley/data
cp -R apparmor/safe_scripts /home/huxley/data/scripts
chown -R huxley:huxley /home/huxley/data

cp /home/huxley/data/scripts/app_armor.huxley_jail /etc/apparmor.d/home.huxley.data.scripts.huxley_jail
aa-enforce /etc/apparmor.d/home.huxley.data.scripts.huxley_jail
