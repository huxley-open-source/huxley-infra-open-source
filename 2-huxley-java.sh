cd ~/

if [ ! -d tools/jdk ]; then
	mkdir tools
	cd tools
	wget http://zuq01.zuq.com.br/files/jdk-8u111-linux-x64.tar.gz
	tar -xzf jdk-8u111-linux-x64.tar.gz
	mv jdk1.8.0_111 jdk
  wget http://zuq01.zuq.com.br/files/apache-maven-3.3.9-bin.tar.gz
  tar -xzf apache-maven-3.3.9-bin.tar.gz
  cd ../
  mkdir bin
  ln -s /home/huxley/tools/jdk/bin/java java
  ln -s /home/huxley/tools/apache-maven-3.3.9/bin/mvn mvn
  export JAVA_HOME=/home/huxley/tools/jdk
  export PATH=$PATH:/home/huxley/bin
else
	echo 'jdk already installed'
fi
