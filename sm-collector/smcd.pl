#!/usr/bin/perl
use strict;
use warnings;
use Data::Dumper;
use Proc::Daemon;
use FindBin qw($Bin);
use IO::Socket;
use IO::Socket::Timeout;
use JSON;
use Try::Tiny;
use POSIX;
use Exporter;

use lib "$Bin/libs";
use common qw($conf logs read_file smc_mysql);
use register qw(reg);

$0 = "$conf->{'process_name'}";
print "$conf->{'name'} is already running!\nCheck PID: ".read_file($conf->{'pid_file'}) and exit 1 if -e "$conf->{'pid_file'}";

logs("$conf->{'name'} daemon is starting");

(my $uid, my $gid) = (getpwnam($conf->{'user'}))[2,3];
# change GID
$) = $gid;
$( = $);
# change UID
$> = $uid;
$< = $>;

my $daemon = Proc::Daemon->new(
	work_dir => "$conf->{'user_home'}",
	pid_file => "$conf->{'pid_file'}",
);
 
my $Kid_1_PID = $daemon->Init;
if ( ! $Kid_1_PID ) {
	# code executed only by the child ...

	#use sigtrap qw/handler sig_trap normal-signals/;
	$SIG{'HUP'} = sub {
		logs("Received SIGHUP o.0");
		logs("Received SIGHUP o.0", "SIGHUP");
	};
	$SIG{'__DIE__'} = sub {
		my $msg = shift;
		logs("Died with message: $msg", "DIED");
		#unlink($conf->{'pid_file'});
	};

	my $sock = IO::Socket::INET->new(
			Listen => 20,
#			LocalAddr => 'localhost',
			LocalAddr => '0.0.0.0',
			LocalPort => 8123,
			Proto => 'tcp',
			Reuse => 1,
			Timeout => 5,
		) or die "Can't open socket: $@";
	IO::Socket::Timeout->enable_timeouts_on($sock);
	$sock->read_timeout(5);    # These will work
	$sock->write_timeout(5);   # These will work


	my $help = "\nYou need help\n";
	$help .= "ls - lists files\n";
	$help .= "quit - kill the listener\n";
	$help .= "\n";

	while (1) {
		my $client = $sock->accept();
		if ( $client ) {
			my $client_address = $client->peerhost();
			$client->recv(my $data, 1024);
			# remove the new line char for easier data parsing
			$data =~ s/\r?\n?$//;
			# logs("$client_address - $data", "ALL-REQUESTS");

			# actions on clinet's request
			my $data_json;
			try {
				$data_json = decode_json($data);
			} catch {
				logs("Tried to die with: $_\t\t\tbecause of $client_address: $data", "DEBUG");
			};
			if ( $data_json->{device}{identifier} ) {
				reg($data_json, $client_address);
				if ( $data_json->{get_pins} ) {
					my $get_pin_info = smc_mysql("select lpad(pin, 2, 0) as pin, pins.description, pin_defaults, pins.type from pins left join devices on pins.device = devices.id where devices.identifier  = '$data_json->{device}{identifier}' and type < 3 order by type asc, pin asc;", "pin");
					# logs(Dumper($get_pin_info), "PINS-SQL-DEBUG");
					logs("Pins config: identifier - $client_address - ".encode_json($get_pin_info));
					$client->send(encode_json($get_pin_info));
				}
				$client->shutdown(SHUT_WR);
				$client->close();
				shutdown($client, 1);
				# update the SQL tables with the data sent by the devices
			} else {
				$client->shutdown(SHUT_WR);
				$client->close();
				shutdown($client, 1);
			}
		}
	}
	$sock->close();
}

#print "Child PID: $Kid_1_PID\n";
