# Arquitetura Huxley
Componentes da arquitetura do The Huxley

## Fail2Ban

## Kernel Grub 2

## API

Servidor Java responsável por implementar a API do The Huxley.

A API roda na porta 8080. Na porta 80 a url */api* é redirecionada para essa porta. Se um request para esse endereço retornar o código http **503** quer dizer que a API não está rodando.

**Repositório**: https://github.com/r0drigopaes/huxley-api

### Comandos

```bash
# Iniciando a api
sudo systemd start huxley

# Parando a api
sudo systemd stop huxley

# Atualizando a API
# Pode ser executada de qualquer maquina que possua o projeto e o grails instalado
# Irá compilar o que está na branch master e implantar no servidor. É preciso manualmente parar a api antes de fazer o deploy e iniciar em seguida

node_modules/.bin/shipit test deploy # em dev
node_modules/.bin/shipit production deploy # em prod

# Visualizando os logs

tailf ~/data/log/huxley-api.log
```

## Rabbit

Componente responsável pela fila de submissões para serem avaliadas, códigos para executar e submissões do oráculo. Esse componente roda no servidor.

- **Logs**: */var/log/rabbitmq*
- **Gerenciador**: http://www.thehuxley.com:15672/
  - **Usuário**: ???
  - **Senha**: ???

### Comandos

```bash

# O rabbit inicia junto com o servidor. Se por um acaso ele não iniciou ou você deseja reinicia-lo execute o comando abaixo.

service rabbitmq-server start
service rabbitmq-server restart

# Adicionar usuário
sudo rabbitmqctl add_user [usuario] [senha]

# Criar filas.
# Apenas se as filas não estiverem criadas.
# O arquivo rabbit.json deve encontra-se no repositorio do huxley-evaluator
rabbitmqadmin -q import rabbit.json
```

### Instalação

```bash
echo 'deb http://www.rabbitmq.com/debian/ testing main' | sudo tee /etc/apt/sources.list.d/rabbitmq.list

wget -O- https://www.rabbitmq.com/rabbitmq-release-signing-key.asc | sudo apt-key add -

sudo apt-get update
sudo apt-get -y install rabbitmq-server

rabbitmq-plugins enable rabbitmq_management

cd /usr/local/bin
wget https://raw.githubusercontent.com/rabbitmq/rabbitmq-management/rabbitmq_v3_6_12/bin/rabbitmqadmin
chmod +x rabbitmqadmin
```

### Filas

- code_queue, durável
- evaluation_queue, durável, max-priority=10
- submission_queue, durável, max-priority=10
- code_result_queue, durável
- reevaluation_queue, durável
- oracle_queue, durável
- oracle_result_queue, durável

## NFS

O NFS compartilha a pasta de casos de teste entre o servidor da API e o servidor onde o avaliador é executado.

O servidor nfs roda no mesmo servidor da API e o cliente deve rodar no mesmo servidor onde o avaliador é executado.

O diretório compartilhado é o */home/huxley/data/testcases* e ele é montado no cliente no diretório */mnt/nfs/var/testacases*

O NFS também deve executar sempre que o servidor inicia. Caso algum problema ocorra, talvez seja preciso iniciar o serviço no servidor da API ou montar o diretório no servidor cliente.

### Instalação

```bash
# Servidor onde roda a API

sudo apt-get install nfs-kernel-server
/home/huxley/data/testcases 10.0.2.15(ro,sync)
sudo exportfs -a

## Iniciar serviço
sudo service nfs-kernel-server start

# Servidor onde roda o avaliador

sudo apt-get install nfs-common
sudo mkdir -p /mnt/nfs/var/testcases

# Montando o diretório
sudo mount 104.237.135.224:/home/huxley/data/testcases /mnt/nfs/var/testcases

# Instalação do cache no servidor onde roda o avaliador.
# Não é obrigatório, mas melhora a performance do avaliador
sudo apt-get install cachefilesd
edit fstab includ
edit /etc/default/cachefilesd and set RUN=yes
sudo /etc/init.d/cachefilesd start

```

## WEB

O projeto WEB é javascript e html puro servidos pelo Apache (ver configuração mais abaixo).

**Repositório**: https://github.com/r0drigopaes/huxley-web

O deploy apenas atualiza o repositório no servidor e executa o build.

```bash
git pull origin master
grunt build
```

### Comandos

```bash
# Atualizando a API
# Pode ser executada de qualquer maquina que possua o projeto e o node instalado

node_modules/.bin/shipit test deploy # em dev
node_modules/.bin/shipit production deploy # em prod

```

## Avaliador

Projeto Java que espera submissões na fila do Rabbit, executa a avaliação e coloca a resposta de volta na fila.

**Repositório**: https://github.com/r0drigopaes/huxley-evaluator

### Comandos

```bash
# Atualizando o avaliador
# Pode ser executada de qualquer maquina que possua java instalado

node_modules/.bin/shipit test deploy # em dev
node_modules/.bin/shipit production deploy # em prod

```

## App Armor

### Comandos

```bash
apt-get -y install apparmor apparmor-profiles apparmor-utils /etc/init.d/apparmor start

mkdir -p /home/huxley/data
cp -R [dir-do-avaliador]/apparmor/safe_scripts /home/huxley/data/scripts
chown -R huxley:huxley /home/huxley/data

cp /home/huxley/data/scripts/app_armor.huxley_jail /etc/apparmor.d/home.huxley.data.scripts.huxley_jail

## Enforcing App Armor
aa-enforce /etc/apparmor.d/home.huxley.data.scripts.huxley_jail
```


## PostgreSQL

Banco de dados utilizado pela API. As tabelas não precisam ser criadas, pois os arquivos de migração da API executam sempre que a api é iniciada.

**Versão**: 9.5

### Instalação

```bash
apt-get -qq install -y postgresql-9.5

# criando usuario huxley
su - postgres -c "createuser -U postgres -D -A huxley"
su - postgres -c "psql -U postgres -d postgres -c \"alter user huxley with password '123456';\""

# Copiando arquivo de senha
echo "localhost:*:*:huxley:123456" > /home/huxley/.pgpass
chown huxley:huxley /home/huxley/.pgpass
chmod 600 /home/huxley/.pgpass
```

## Apache

sudo apt-get -qq install -y apache2
a2enmod deflate
a2enmod headers
a2enmod proxy
a2enmod proxy_http
a2enmod expires
a2enmod rewrite
service apache2 restart


## Grafana
