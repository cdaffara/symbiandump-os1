#!perl
# cloggercolour.pl
# 
# Copyright (c) 2006 - 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#

use strict;
use Getopt::Long;
use IO::File;
use IO::Handle;

sub ParseCommandLineArgs();
sub GetNextColour();

my $clogFile;
my $outFile;
my $plainText;
my $colour = 0;
my $deltaTimeStamps = 0;
my $verbose;

ParseCommandLineArgs();

my $logfile;
my $htmlfile;

if (defined $clogFile) {
	print "Opening $clogFile for reading\n" if ($verbose);
	$logfile = new IO::File;
	$logfile->open("<$clogFile") or die "ERROR: Can't open $clogFile for reading: $!\n";
} else {
	print "Reading from STDIN\n" if ($verbose);
	#$logfile = new IO::Handle;
	#$logfile->fdopen(fileno(STDIN), "r") or die "ERROR: can't open STDIN: $!\n";
	$logfile = *STDIN;
}
if (defined $outFile) {
	print "Opening $outFile for writing\n" if ($verbose);
	$htmlfile = new IO::File;
	$htmlfile->open(">$outFile") or die "ERROR: Can't open $outFile for writing: $!\n";
} else {
	print "Writing to STDOUT\n" if ($verbose);
	$htmlfile = *STDOUT;
}
unless ($plainText) {
	print $htmlfile "<html><body><pre>\n";
}

my %tagColours = {};
my $colours = 0;
my $lastMillis = -1;

while (my $line = <$logfile>) {
	chomp $line;
	if ($line =~ m/^(\d{4}-\d{2}-\d{2} \d{2}\:\d{2}\:\d{2}\.\d{3})\: \[([^]]+)\] (.*)$/ ) {
		my ($time, $tag, $text) = ($1, $2, $3);
		my $deltaStr = "";
		unless (exists $tagColours{$tag}) {
			$tagColours{$tag} = GetNextColour();
		}
		if ($deltaTimeStamps) {
			$time =~ m|(\d{4})-(\d{2})-(\d{2}) (\d{2})\:(\d{2})\:(\d{2})\.(\d{3})|;
			my ($year, $month, $day, $hour, $minute, $second, $millisecond) = ($1, $2, $3, $4, $5, $6, $7);
			my $millis = ((((((((((($year*12)+$month)*31)+$day)*24)+$hour)*60)+$minute)*60)+$second)*1000)+$millisecond;
			my $delta = 0;
			if ($lastMillis != -1) {
				$delta = $millis - $lastMillis;
			}
			$lastMillis = $millis;
			$deltaStr = sprintf("(+% 4d)", $delta);
		}

		print $htmlfile "<font color=$tagColours{$tag}>" unless ($plainText) ;
		print $htmlfile "$time:$deltaStr [$tag] $text";
		print $htmlfile "</font>" unless ($plainText);
		print $htmlfile "\n";
	}
	else {
		print STDERR "WARNING: could not parse line $. of $clogFile\n";
	}
}


unless ($plainText) {
	print $htmlfile "</pre></body></html>\n"
}
undef $logfile;
undef $htmlfile;

sub ParseCommandLineArgs() {
	Getopt::Long::Configure ("bundling");
	GetOptions('v+' => \$verbose, 'c' => \$colour, 'd' => \$deltaTimeStamps, 'p'=>\$plainText);
	if ($#ARGV != -1) {
		$clogFile = shift @ARGV;
	}
	if ($#ARGV != -1) {
		$outFile = shift @ARGV;
	} else {
		if (defined $clogFile) {
			$outFile = "$clogFile.html";
		}
	}
}

my $colours = 0;

sub GetNextColour() {
	if ($colour) {
		# make a new colour
		my $pattern = $colours % 6;
		my $newColour;
		++$pattern;
		if ($pattern > 3) {
			$newColour = "#FF";
			$pattern-=4;
		} else {
			$newColour = "#00";
		}
		if ($pattern > 1) {
			$newColour = "${newColour}80";
			$pattern-=2;
		} else {
			$newColour = "${newColour}00";
		}
		if ($pattern > 0) {
			$newColour = "${newColour}FF";
		} else {
			$newColour = "${newColour}00";
		}
		++$colours;
		return $newColour;
	} else { 
		return "#000000";
	}
}

__END__

=head1 NAME

cloggerproc - Post-process clogger logs to add colour or timestamps

=head1 SYNOPSIS

	cloggerproc [options] [infile [outfile]]

options:

=over 4

=item -d

Add a millisecond delta to timestamps

=item -c

Colour logging lines based on the tag

=item -v

verbose output

=item -h

Show this help

=back

=head1 DESCRIPTION


=head1 KNOWN BUGS

If the timestamp month or year rolls over during the log, the delta will be incorrect for the first logging line in the new month/year.

=head1 COPYRIGHT

Copyright (c) 2008-2010 Accenture. All rights reserved.

=cut
