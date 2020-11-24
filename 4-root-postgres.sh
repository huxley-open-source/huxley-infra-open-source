echo 'instalando postgres'
apt-get -qq install -y postgresql-9.5

echo "criando usuario huxley"
su - postgres -c "createuser -U postgres -D -A huxley"
su - postgres -c "psql -U postgres -d postgres -c \"alter user huxley with password '123456';\""

echo 'Copiando arquivo de senha'
echo "localhost:*:*:huxley:123456" > /home/huxley/.pgpass
chown huxley:huxley /home/huxley/.pgpass
chmod 600 /home/huxley/.pgpass
