sudo ln -s /etc/apparmor.d/home.huxley.data.scripts.huxley_jail /etc/apparmor.d/disable/
sudo apparmor_parser -R /etc/apparmor.d/home.huxley.data.scripts.huxley_jail
