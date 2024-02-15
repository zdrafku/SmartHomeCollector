<?php
ini_set("display_errors", "On");

$conn = mysqli_connect("localhost", "collector", "collector", "sm_collector");

if ( isset($_GET['device']) && isset($_GET['edit_device']) && isset($_POST['edit_device']) && $_GET['edit_device'] == $_POST['edit_device'] ) {
    mysqli_query($conn, "update devices set identifier='".$_POST['edit_identifier']."', description='".$_POST['edit_description']."' where id='".$_GET['edit_device']."'");
}

$sql_devices = mysqli_query($conn, "select * from devices");
if ( !mysqli_num_rows($sql_devices) ) echo ("No devices found<br>");
while ( $all_devices = mysqli_fetch_assoc($sql_devices) ) {
    $arr_devices[$all_devices['id']] = $all_devices;
}
exec("free -m", $mem);
$free_mem = explode(" ", $mem[1]);
// free mem's index floats
$free_mem = array_filter($free_mem);
$free_mem = array_values($free_mem);
echo "Free RAM: ".$free_mem[3]."M of ".$free_mem[1]."M<br>";



foreach ($arr_devices as $key => $value) {
    echo $value['identifier'].' - <a href="/edit.php?device='.$value['id'].'">'.$value['description']."</a><br>";
}

if ( !isset($_GET['device']) ) die("Select device");
$device_id = $_GET['device'];
$device_description = $arr_devices[$device_id]['description'];
$device_identifier = $arr_devices[$device_id]['identifier'];

if ( isset($_GET['device']) && isset($_GET['add_pin']) && isset($_POST['add_pin']) ) {
    if ( !$_POST['add_pin_defaults'] ) $_POST['add_pin_defaults'] = 0;
    mysqli_query($conn, "insert into pins (device, pin, type, pin_defaults, description) values ('".$device_id."','".$_POST['add_pin']."', '".$_POST['add_type']."', '".$_POST['add_pin_defaults']."', '".$_POST['add_description']."')");
}

if ( isset($_GET['device']) && isset($_GET['edit_pin']) && isset($_POST['edit_pin_id']) && $_POST['edit_pin_id'] == $_GET['edit_pin']) {
    if ( !$_POST['edit_pin'] ) {
        mysqli_query($conn, "delete from pins where device='".$_GET['device']."' and id='".$_POST['edit_pin_id']."'");
        echo "Pin with ID ".$_GET['edit_pin']." is DELETED";
    } else {
        mysqli_query($conn, "update pins set pin='".$_POST['edit_pin']."', type='".$_POST['edit_type']."', pin_defaults='".$_POST['edit_pin_defaults']."', description='".$_POST['edit_description']."' where device='".$_GET['device']."' and id='".$_POST['edit_pin_id']."'");
    }
}
echo '<form method="POST" action="/edit.php?device='.$device_id.'&edit_device='.$device_id.'">';
echo '<input name="edit_device" type="hidden" value="'.$device_id.'">';
echo '<input name="edit_identifier" type="text" value="'.$device_identifier.'">';
echo '<input name="edit_description" type="text" value="'.$device_description.'">';
echo '<input type="submit" value="Save" size="5">';
echo "</form>";

// Relay: 1
// Switch: 2
// Sensor: 3
$sql_pins = mysqli_query($conn, "select id, pin, type, pin_defaults, description from pins where device = '".$device_id."' order by type asc, pin asc");
while ( $all_pins = mysqli_fetch_assoc($sql_pins) ) {
    echo '<form method="POST" action="/edit.php?device='.$device_id.'&edit_pin='.$all_pins['id'].'">';
    echo '<input name="edit_pin_id" type="hidden" value="'.$all_pins['id'].'">';
    echo '<input name="edit_pin" type="text" value="'.$all_pins['pin'].'" maxlength="2" size="2">';
    echo '<select name="edit_type"><option value="1"';
    if ( $all_pins['type'] == 1 ) echo ' selected="selected"';
    echo '>Relay</option><option value="2"';
    if ( $all_pins['type'] == 2 ) echo ' selected="selected"';
    echo '>Switch</option><option value="3"';
    if ( $all_pins['type'] == 3 ) echo ' selected="selected"';
    echo '>Sensor</option></select>';
    echo '<input name="edit_pin_defaults" type="text" value="'.$all_pins['pin_defaults'].'" maxlength="1" size="1">';
    echo '<input name="edit_description" type="text" value="'.$all_pins['description'].'">';
    echo '<input type="submit" value="Save" size="5">';
    echo "</form>";
}

echo '<form method="POST" action="/edit.php?device='.$device_id.'&add_pin">';
echo '<input name="add_pin" type="text" value="pin#" maxlength="2" size="2">';
echo '<select name="add_type"><option value="1">Relay</option><option value="2">Switch</option><option value="3">Sensor</option></select>';
echo '<input name="add_pin_defaults" type="text" value="default" maxlength="1" size="1">';
echo '<input name="add_description" type="text" value="description">';
echo '<input type="submit" value="Add" size="5">';
echo "</form>";
?>