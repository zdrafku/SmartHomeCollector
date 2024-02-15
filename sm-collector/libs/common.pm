#!/usr/bin/perl
use strict;
use warnings;

package common;

use Data::Dumper;

use File::HomeDir;
use File::Path qw( make_path );
use FindBin qw($Bin);
use POSIX;
use Exporter;
use DBI; 
our @ISA = qw( Exporter );
our @EXPORT = qw( $conf read_file logs smc_mysql );

sub read_file {
	my $filename = shift;
	my $rows;
	open(my $fh, '<:encoding(UTF-8)', $filename) or die "Could not open file '$filename' $!";
	while (my $row = <$fh>) {
		chomp $row;
		$rows .= $row."\n";
	}
	close($fh);
	return $rows;
}

my $conf_file = read_file($Bin."/smcd.conf");
my @conf_array = split(/\n/, $conf_file);

our $conf;
foreach (@conf_array) {
	(my $key, my $value) = split (/\s?=\s?/, $_, 2);
    $conf->{$key} = $value;
}

if ( !-d $conf->{'logs_dir'} )  {
	make_path($conf->{'logs_dir'}, {
		owner => "$conf->{'user'}",
		group => "$conf->{'user'}",
		mode => 0744,
	}) or die "Failed to create path: $conf->{'logs_dir'}";
}

sub logs {
	my $message = shift;
	my $level = shift;
	my $f_log;
	if ( $level ) {
		open($f_log, ">>", $conf->{'debug_file'}) or die "$0: Can't open file to read: $conf->{'debug_file'} - $!\n";
		print $f_log "[".strftime("%F %R:%S %Z", localtime())."] - [$level] $message\n";
	} else {
		open($f_log, ">>", $conf->{'log_file'}) or die "$0: Can't open file to read: $conf->{'log_file'} - $!\n";
		print $f_log "[".strftime("%F %R:%S %Z", localtime())."] $message\n";
	}
	close($f_log);
}

sub smc_pgsql {
	my $sql_query = shift;
	my $hash_key = shift;
	my $sql_result;
	logs("Query: ".$sql_query, "DEBUG");
	logs("Hash: ".$hash_key, "DEBUG");
    my $sql_connect = DBI->connect("DBI:Pg:dbname=$conf->{'pg_dbase'};host=$conf->{'pg_host'};port=$conf->{'pg_port'}",
								$conf->{'pg_user'},
								$conf->{'pg_pass'},
								{
									AutoCommit => 0,
									RaiseError => 1,
									pg_server_prepare => 0
								}
                            ) or die logs("DB died: ".$DBI::errstr);
	if ( $sql_query =~ /select/i ) {
		my $sql_prepare = $sql_connect->prepare($sql_query);
		my $sql_exec = $sql_prepare->execute() or die logs("DB died: ".$DBI::errstr);
		$sql_result = $sql_prepare->fetchall_hashref($hash_key);
		# logs(Dumper($sql_result));
	} else {
		$sql_connect->do($sql_query);
		$sql_connect->commit();
	}
	$sql_connect->disconnect();
	return $sql_result;
}

sub smc_mysql {
	my $sql_query = shift;
	my $hash_key = shift;
	my $sql_result;
	# logs("Query: ".$sql_query, "SQL");
	# logs("Hash: ".$hash_key, "SQL");
	my %attr = ( PrintError=>0,  # turn off error reporting via warn()
             RaiseError=>1);   # turn on error reporting via die()

	my $sql_connect  = DBI->connect("DBI:mysql:".$conf->{'mysql_dbase'},$conf->{'mysql_user'},$conf->{'mysql_pass'}, \%attr) or die("Error connecting to the database: $DBI::errstr\n");
	if ( $sql_query =~ /select/i ) {
		my $sql_prepare = $sql_connect->prepare($sql_query);
		my $sql_exec = $sql_prepare->execute() or die logs("DB died: ".$DBI::errstr);
		if ( $hash_key eq "array" ) {
			$sql_result = $sql_prepare->fetchall_arrayref();
		} else {
			$sql_result = $sql_prepare->fetchall_hashref($hash_key);
		}
		# logs(Dumper($sql_result), "SQL-L107");
	} else {
		# logs(Dumper($sql_query), "SQL-L109");
		$sql_connect->do($sql_query);
		$sql_connect->commit();
	}
	$sql_connect->disconnect();
	return $sql_result;
}

1;
