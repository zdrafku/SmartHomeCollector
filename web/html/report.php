<?php
ini_set("display_errors", "On");

// $config_file = "/data/sm-collector/smcd.conf";
// $file = fopen($config_file, "r", );
// while ( !feof($file)) {
//     $line = fgets($file);
//     $line_split = explode("=", $line);
//     if ( $line_split[0] ) ${$line_split[0]} = $line_split[1];
// }
// fclose($file);

if ( isset($_GET['hours']) && $_GET['hours'] ) {
    $hours = $_GET['hours'];
} else {
    $hours = 24;
}

$conn = mysqli_connect("localhost", "collector", "collector", "sm_collector");

$sql_devices = mysqli_query($conn, "select * from devices");
if ( !mysqli_num_rows($sql_devices) ) die("No devices found");
while ( $all_devices = mysqli_fetch_assoc($sql_devices) ) {
    $arr_devices[$all_devices['id']] = $all_devices;
}
exec("free -m", $mem);
$free_mem = explode(" ", $mem[1]);
// free mem's index floats
$free_mem = array_filter($free_mem);
$free_mem = array_values($free_mem);
echo "Free RAM: ".$free_mem[3]."M of ".$free_mem[1]."M<br>";

if ( !isset($_GET['device']) || !$_GET['device'] ) {
    foreach ($arr_devices as $key => $value) {
        echo $value['identifier'].' - <a href="/report.php?device='.$value['id'].'">'.$value['description']."</a><br>";
    }
    die("<br><b>Select device</b>");
}

$sql_device_ipddr = mysqli_query($conn, "select ip_address from logs where device='".$_GET['device']."' order by id desc limit 1");
$all_device_ipadr = mysqli_fetch_assoc($sql_device_ipddr);

$device_id = $_GET['device'];
$device_ipaddr = $all_device_ipadr['ip_address'];
$device_description = $arr_devices[$device_id]['description'];
$device_identifier = $arr_devices[$device_id]['identifier'];
echo '<abbr title="Identifier: '.$device_identifier.'"><a href="http://'.$device_ipaddr.'" target="_blank">'.$device_description."</a></abbr> last ".$hours." hours<br>";

// Relay: 1
// Switch: 2
// Sensor: 3
$sql_device_pins = mysqli_query($conn, "select pin, pin_defaults, description from pins where device = '".$device_id."'");
// if ( !mysqli_num_rows($sql_device_pins) ) echo "No pins found<br>";
while ( $all_device_pins = mysqli_fetch_assoc($sql_device_pins) ) {
    $arr_device_pins[$all_device_pins['pin']] = $all_device_pins;
}

if ( !isset($_GET['pins']) && !isset($_GET['switch']) && !isset($_GET['sensors']) && !isset($_GET['restarts']) ) {
    echo '<a href="/report.php?device='.$_GET['device'].'&pins">Relays</a></br>';
    echo '<a href="/report.php?device='.$_GET['device'].'&switch">Switches</a></br>';
    echo '<a href="/report.php?device='.$_GET['device'].'&sensors&hours=2">Sensors</a></br>';
    echo '<a href="/report.php?device='.$_GET['device'].'&restarts">Restarts</a></br>';
    die("<br><b>Select log type</b>");
}

if ( isset($_GET['pins']) ) {
    $sql_logs = mysqli_query($conn, "select ip_address, update_date, pin, data from logs where device = '".$device_id."' and pin != 'NULL' and update_date > DATE_SUB(NOW(), INTERVAL ".$hours." HOUR) order by update_date desc");
    if ( !mysqli_num_rows($sql_logs) ) die("No logs for the past ".$hours." hours");
    echo "<table><tr>";
    if ( isset($_GET['debug']) ) echo "<td>ip_address</td>";
    echo "<td>update_date</td><td>pin</td><td>data</td></tr>";
    while ( $all_logs = mysqli_fetch_assoc($sql_logs) ) {
        $json_logs_data = json_decode($all_logs['data'], true);
        echo "<tr>";
        if ( isset($_GET['debug']) ) echo "<td>".$all_logs['ip_address']."</td>";
        echo "<td>".$all_logs['update_date']."</td>";
        echo '<td><abbr title="Pin: '.$all_logs['pin'].'">'.@$arr_device_pins[$all_logs['pin']]['description']."</abbr></td>";
        if ( $json_logs_data['status'] == @$arr_device_pins[$all_logs['pin']]['pin_defaults'] ) {
            echo '<td bgcolor="silver">Off</td>';
        } else {
            echo '<td bgcolor="yellow">On</td>';
        }
        echo "</tr>";
    }
    echo "</table>";
}

if ( isset($_GET['sensors']) ) {
    $sql_sensors = mysqli_query($conn, "select ip_address, update_date, pin, data from logs where device = '".$device_id."' and pin IS NULL and data not like '%restart%' and update_date > DATE_SUB(NOW(), INTERVAL ".$hours." HOUR) order by update_date desc");
    if ( !mysqli_num_rows($sql_sensors) ) die("No sensor data for the past ".$hours." hours");
    echo "<table><tr>";
    if ( isset($_GET['debug']) ) echo "<td>ip_address</td>";
    echo "<td>update_date</td><td>data</td></tr>";
    while ( $all_sensors = mysqli_fetch_assoc($sql_sensors) ) {
        $json_sensors_data = json_decode($all_sensors['data'], true);
        if ( isset($json_sensors_data['restart']) ) continue;
        echo "<tr>";
        if ( isset($_GET['debug']) ) echo "<td>".$all_sensors['ip_address']."</td>";
        echo "<td>".$all_sensors['update_date']."</td>";
        echo '<td>'.$json_sensors_data['temp'].'°C - '.$json_sensors_data['hum'].'%</td>';
        echo "</tr>";
    }
    echo "</table>";
}

if ( isset($_GET['restarts']) ) {
    $sql_sensors = mysqli_query($conn, "select ip_address, update_date, pin, data from logs where device = '".$device_id."' and pin IS NULL and update_date > DATE_SUB(NOW(), INTERVAL ".$hours." HOUR) and data like '%restart%' order by update_date desc");
    if ( !mysqli_num_rows($sql_sensors) ) die("No restarts for the past ".$hours." hours");
    echo "<table><tr>";
    if ( isset($_GET['debug']) ) echo "<td>ip_address</td>";
    echo "<td>update_date</td></tr>";
    while ( $all_sensors = mysqli_fetch_assoc($sql_sensors) ) {
        $json_sensors_data = json_decode($all_sensors['data'], true);
        if ( !isset($json_sensors_data['restart']) ) continue;
        echo "<tr>";
        if ( isset($_GET['debug']) ) echo "<td>".$all_sensors['ip_address']."</td>";
        echo "<td>".$all_sensors['update_date']."</td>";
        echo "</tr>";
    }
    echo "</table>";
}
?>
