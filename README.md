# Smart Home Collector on CentoOS 7 Raspberry Pi 3B+


## Image link:
http://mirror.telepoint.bg/centos-altarch/7.9.2009/isos/armhfp/CentOS-Userland-7-armv7hl-RaspberryPI-Minimal-2009-sda.raw.xz

## Resize .raw image
Link to tutorial: https://www.linux.com/news/how-modify-raw-disk-image-your-custom-linux-distro/
```
mount -o loop,offset=815792128 CentOS-Userland-7-armv7hl-RaspberryPI-Minimal-2009-sda.raw 123/
dd if=/dev/zero bs=6M count=512 >> CentOS-Userland-7-armv7hl-RaspberryPI-Minimal-2009-sda.raw
parted CentOS-Userland-7-armv7hl-RaspberryPI-Minimal-2009-sda.raw
	- print free
	- resizepart 3 5816MB
kpartx -a CentOS-Userland-7-armv7hl-RaspberryPI-Minimal-2009-sda.raw
e2fsck -f -y -v -C 0 /dev/mapper/loop0p3
resize2fs -p /dev/mapper/loop0p3
kpartx -d CentOS-Userland-7-armv7hl-RaspberryPI-Minimal-2009-sda.raw
```

## First boot
```
hostnamectl set-hostname SmartHome-Collector.psychotm.info
mkdir -p /data
fdisk /dev/mmcblk0 -> p -> n -> p -> w
partprobe /dev/mmcblk0
mkfs.ext4 -L all-data /dev/mmcblk0p4
echo "LABEL=all-data /data ext4 defaults 0 2" >> /etc/fstab
mount -a
```

## Disable sleep
```
systemctl mask sleep.target suspend.target hibernate.target hybrid-sleep.target
echo -e "[connection]\nwifi.powersave = 2" > /etc/NetworkManager/conf.d/default-wifi-powersave-off.conf
systemctl restart NetworkManager
```

## Install the necessary packages
```
yum install -y ntp ntpdate make gcc strace python3 python3-devel NetworkManager-tui httpd postgresql-server postgresql-contrib mariadb-server file vim wget git nc lsof cpan perl-DBD-Pg perl-File-HomeDir perl-JSON perl-Try-Tiny
```

## Sync system clock
```
systemctl enable ntpd
systemctl start ntpd
```

## Prepare the directory structure for MySQL and PostgreSQL and move them to the deparate mounted partition
```
cd /data ; git clone git@github.com:zdrafku/smcd.git .
mv /var/lib/{pgsql,mysql} /data/
mkdir /var/lib/{pgsql,mysql}
chown postgres:postgres /var/lib/pgsql
chmod 700 /var/lib/pgsql
echo "/data/pgsql /var/lib/pgsql none defaults,bind 0 0" >> /etc/fstab
chown mysql:mysql /var/lib/mysql
echo "/data/mysql /var/lib/mysql none defaults,bind 0 0" >> /etc/fstab
mount -a
postgresql-setup initdb
systemctl start postgresql
systemctl enable postgresql
systemctl enable mariadb
systemctl start mariadb
```

## Install Perl modules
```
cpan Proc::Daemon
#cpan Test::More
#cpan ExtUtils::CBuilder
```

## Peprare Smart Home Collector's directory structure and service
```
useradd -d /data/sm-collector sm-collector
su - sm-collector -c 'echo "localhost:5432:sm_collector:collector:collector" > ~/.pgpass; chmod 600 ~/.pgpass'
sed -i '/^local\s*all\s*all/ s/peer$/md5/' /var/lib/pgsql/data/pg_hba.conf
su - postgres -c 'createdb sm_collector'
su - postgres
psql -c "CREATE USER collector WITH LOGIN ENCRYPTED PASSWORD 'collector';"
su - postgres -c 'psql -c "ALTER DATABASE sm_collector OWNER TO collector;"'
systemctl restart postgresql
firewall-cmd --zone=public --add-port=8123/tcp --permanent
firewall-cmd --zone=public --add-port=80/tcp --permanent
firewall-cmd --reload
echo 'if [ -e "/data/sm-collector/SmartHome-collectorD.pid" ]; then unlink /data/sm-collector/SmartHome-collectorD.pid; fi' >> /etc/rc.d/rc.local
chmod +x /etc/rc.d/rc.local
echo -e "unlink /data/sm-collector/SmartHome-collectorD.pid\nservice smcd start" >> /etc/rc.local
ln -s /data/sm-collector/service_smcd /etc/init.d/smcd
chkconfig --add smcd
```

## PostgreSQL structure
```
psql -Ucollector sm_collector -W

create table devices    ( id serial, identifier varchar(32) unique, description varchar(64), create_date timestamp with time zone default now() );
create table status 	( id serial, device integer not null, pin integer, status bool, data json, update_date date );
create table logs       ( id serial, ip_address inet, device integer not null, update_date timestamp with time zone default now(), pin integer, data json );
create table pins 	( id serial, device integer not null, pin smallint, type smallint, pin_defaults bool, description text );
create table sensors	( id serial, device integer not null, pin smallint, type smallint, description text );

# \du - list users
# \l - list databases
```

## connect WiFi
```
nmtui
```
