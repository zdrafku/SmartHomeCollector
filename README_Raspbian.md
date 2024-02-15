# Smart Home Collector on Raspbian 11 Raspberry Pi Zero W

## Disable network sleep and IPv6
```
sudo systemctl mask sleep.target suspend.target hibernate.target hybrid-sleep.target
sysctl -w net.ipv6.conf.all.disable_ipv6=1
sysctl -w net.ipv6.conf.default.disable_ipv6=1
sysctl -w net.ipv6.conf.tun0.disable_ipv6=1
```

## Allow root login for easier development
```
sudo sed -ri 's/^(#PermitRootLogin.*$)/\1\nPermitRootLogin yes/' /etc/ssh/sshd_config
sudo service sshd restart
sudo passwd root
deluser --remove-home pi
```

## Set correct timezone and locales
```
ln -fs /usr/share/zoneinfo/Europe/Sofia /etc/localtime
echo "LC_ALL=en_US.UTF-8" | tee -a /etc/environment > /dev/null
echo "en_US.UTF-8 UTF-8" | tee -a /etc/locale.gen > /dev/null
echo "LANG=en_US.UTF-8" | tee /etc/locale.conf > /dev/null
locale-gen en_US.UTF-8
```

## Install the necessary packages
```
echo 'alias ll="ls -la"' >> /root/.bashrc
apt-get -y upgrade
apt-get -y install vim git jq mariadb-server
apt-get -y install libproc-daemon-perl libjson-perl libtry-tiny-perl libfile-homedir-perl libaio1 libio-socket-timeout-perl libdbd-mysql-perl libdbd-mariadb-perl mosquitto mosquitto-clients
cpan Net::MQTT::Simple

#wget http://raspbian.raspberrypi.org/raspbian/pool/main/libd/libdbi-perl/libdbi-perl_1.642-1+deb10u2_armhf.deb
#dpkg -i libdbi-perl_1.642-1+deb10u2_armhf.deb
```

## Install Web service with PHP-FPM and MySQL modules
```
apt-get -y install lighttpd php7.4 php7.4-fpm php7.4-cgi php7.4-mysql
lighttpd-enable-mod fastcgi-php-fpm cgi accesslog
sed -i '/static-file.exclude-extensions/ s/^/#/' /etc/lighttpd/lighttpd.conf
sed -i '/use-ipv6.pl/ s/^/#/' /etc/lighttpd/lighttpd.conf
sed -i '/^;cgi.fix_pathinfo/ s/^;//' /etc/php/7.4/fpm/php.ini
sed -ri '/^\s+alias/ s#/usr/lib/cgi-bin/#/data/web/cgi-bin/#'  /etc/lighttpd/conf-enabled/10-cgi.conf
sed -i '/^accesslog.filename/ s#^.*$#accesslog.filename = "/data/web/logs/access.log"#' /etc/lighttpd/conf-available/10-accesslog.conf
sed -i '/^server.errorlog/ s#^.*$#server.errorlog = "/data/web/logs/error.log"#' /etc/lighttpd/lighttpd.conf
sed -i '/^short_open_tag/ s/off/on/i' /etc/php/7.4/fpm/php.ini
mkdir -p /data/web/logs
chown www-data:www-data /data/web/logs/ -R
service lighttpd restart
service php7.4-fpm restart
```

## Prepare MySQL to work from project's location
```
yes | perl -MCPAN -e 'install Bundle::DBI'
systemctl stop mysql
mkdir /data
mv /var/lib/mysql/ /data/
mkdir /var/lib/mysql/
chown mysql:mysql /var/lib/mysql/
echo "/data/mysql /var/lib/mysql none defaults,bind 0 0" >> /etc/fstab
rm -rf /var/www/html
echo "/data/web /var/www none defaults,bind 0 0" >> /etc/fstab
mount -a
systemctl start mysql
mysqladmin create sm_collector
mysql -e "CREATE USER 'collector'@'localhost' IDENTIFIED BY 'collector';"
mysql -e "GRANT ALL PRIVILEGES ON sm_collector.* TO 'collector'@'localhost';"
mysql -e "FLUSH PRIVILEGES;"
```

## Set up Smart Home Collector service
```
useradd -m -d /data/sm-collector sm-collector
chmod 755 /data/sm-collector

cd /data/ ; git clone git@github.com:zdrafku/smcd.git
echo -e ".gitignore\nmysql/\nsm-collector/.*\nsm-collector/*.pid\nsm-collector/logs/" >> /data/.gitignore
ln -s /data/sm-collector/service_smcd /etc/init.d/smcd
sed -i '/^exit 0/d' /etc/rc.local
echo 'if [ -e "/data/sm-collector/SmartHome-collectorD.pid" ]; then unlink /data/sm-collector/SmartHome-collectorD.pid; fi' >> /etc/rc.local
echo "exit 0" >> /etc/rc.local
chmod +x /etc/rc.local
systemctl enable smcd
ln -sf /data/Linux/ping_loss.cron /etc/cron.d/ping_loss.cron
````

## Preprate MySQL database
```
create table devices    ( id serial, identifier varchar(32) unique, description varchar(64), create_date timestamp default CURRENT_TIMESTAMP );
create table status     ( id serial, device integer not null, pin integer, status bool, data text CHECK (JSON_VALID(data)), update_date date );
create table logs       ( id serial, ip_address varchar(15), device integer not null, update_date timestamp default CURRENT_TIMESTAMP, pin smallint, data text check (json_valid(data)) );
create table pins       ( id serial, device integer not null, pin smallint, type smallint comment 'Relayy: 1\nSwitch: 2\nSensor: 3', pin_defaults bool, description text );
create table sensors    ( id serial, device integer not null, pin smallint, type smallint, description text );
```
