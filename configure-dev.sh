./1-root-acc.sh
./2-root-rabbit.sh
./3-root-nfs-client.sh
./4-root-postgres.sh

echo "criando banco de dados $DB_USER"
su - postgres -c "createdb -O huxley huxley-test"

./5-root-apparmor.sh
./6-root-languages.sh

./do-as-huxley.sh
