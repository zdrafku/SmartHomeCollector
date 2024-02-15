#!/usr/bin/perl
use strict;
use warnings;

package register;

use Data::Dumper;

use common qw($conf logs smc_mysql);
use Exporter;
use JSON;
our @ISA = qw( Exporter );
our @EXPORT = qw( reg );

sub reg {
    my $data_json = shift;
    my $client_address = shift;

    my $devices_info = smc_mysql("select * from devices where identifier = '$data_json->{device}{identifier}';", "identifier");
    my $device = $devices_info->{$data_json->{device}{identifier}};

    if ( !$device->{'id'} ) {
        if ( defined($data_json->{register}{identifier}) && defined($data_json->{register}{description}) ) {
            smc_mysql("insert into devices (identifier, description) values ('$data_json->{register}{identifier}', '$data_json->{register}{description}');");
            logs("Inserted device: $data_json->{register}{identifier} - $client_address - $data_json->{register}{description}");
            # logs($data_json->pretty->encode($data_json));
        } else {
            logs("Needs restart to register: $data_json->{device}{identifier} - $client_address - ".encode_json($data_json), "UNKNOWN-DEVICE");
        }
    } else {
        # logs(Dumper($data_json), "DEBUG");
        if ( defined($data_json->{register}{identifier}) ) {
            smc_mysql("insert into logs (device, data, ip_address) values ('$device->{id}', '{\"restart\": 1}', '$client_address');");
            logs("Restarted device: $device->{identifier} - $client_address - ($device->{description}) - ".encode_json($data_json->{register}));
        } elsif ( defined($data_json->{relays}) && $data_json->{relays}{pin} ) {
            smc_mysql("insert into logs (device, pin, data, ip_address) values ('$device->{id}', '$data_json->{relays}{pin}', '{\"status\": $data_json->{relays}{status}}', '$client_address');");
            logs("Relay changes: $device->{identifier} - $client_address - ".encode_json($data_json->{relays}));
            # logs($data_json->pretty->encode($data_json));
        } elsif ( defined($data_json->{switches}) && $data_json->{switches}{pin} ) {
            smc_mysql("insert into logs (device, pin, data, ip_address) values ('$device->{id}', '$data_json->{switches}{pin}', '{\"status\": $data_json->{switches}{status}}', '$client_address');");
            logs("Switch changes: $device->{identifier} - $client_address - ".encode_json($data_json->{switches}));
            # logs($data_json->pretty->encode($data_json));
        } elsif ( defined($data_json->{sensor}) ) {
            smc_mysql("insert into logs (device, data, ip_address) values ('$device->{id}', '{\"temp\": \"$data_json->{sensor}{temperature}\", \"hum\": \"$data_json->{sensor}{humidity}\"}', '$client_address');");
            logs("DHT update: $device->{identifier} - $client_address - ".encode_json($data_json->{sensor}));
            # logs($data_json->pretty->encode($data_json));
        } elsif ( defined($data_json->{get_pins}) ) {
            # logs("Pin configs: $device->{identifier} - $client_address - ($device->{description})");
        } else {
            logs("Probably missing pin data in device's setup: $device->{identifier} - $client_address - ".encode_json($data_json), "Unknown-Request");
        }
    }
}

1;
