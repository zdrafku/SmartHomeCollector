#!/bin/bash
echo -e "Content-Type: text/html\n"
date
echo "   br0: 35432400056 81703057    0 57478    0     0          0         0 152919183230 132127342    0    0    0     0       0          0"

# conf_file="/data/sm-collector/smcd.conf"
# while IFS= read -r line
# do
# 	declare "`awk -F= '{print $1}' <<<$line`"="`awk -F= '{print $2}' <<<$line`"
# done < "$conf_file"

#mysql -N -u${mysql_user} -p${mysql_pass} ${mysql_dbase} -e "select update_date, ip_address, devices.description as device, pin, data from logs left join devices on logs.device = devices.id  where device = $QUERY_STRING and pin != 'null' order by update_date asc;" # >> /data/web/logs/123.txt 2>&1