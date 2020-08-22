#! /bin/sh

git clone http://repo.or.cz/r/openocd.git

cd openocd

./bootstrap
./configure --prefix=/opt/openocd --disable-dependency-tracking \
  --enable-bcm2835gpio --enable-sysfsgpio

make
sudo make install

cd -

sudo cp /opt/openocd/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d/
sudo udevadm control --reload
