apt-get install nfs-common -y
mkdir -p /mnt/nfs/var/testcases

if [ -z "$PROD_IP" ]; then
 echo 'IP de producao para NFS nao informado'
 exit 1
fi

mount $PROD_IP:/home/huxley/data/testcases /mnt/nfs/var/testcases
echo "$PROD_IP:/home/huxley/data/testcases    /mnt/nfs/var/testcases   nfs auto,noatime,nolock,bg,nfsvers=4,sec=krb5p,intr,tcp,actimeo=1800,fsc" >> /etc/fstab


# https://askubuntu.com/questions/4572/how-can-i-cache-nfs-shares-on-a-local-disk
# No mount fstab tem no final o fsc pra garantir o cache
apt-get install cachefilesd -y
echo 'RUN=yes' > /etc/default/cachefilesd
echo 'DAEMON_OPTS=""' >> /etc/default/cachefilesd
/etc/init.d/cachefilesd start
