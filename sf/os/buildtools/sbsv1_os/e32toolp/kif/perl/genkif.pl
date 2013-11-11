# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

use XML::Simple;
use File::Path;
use File::Copy;
use Win32::File;
use Getopt::Long;
use strict;

my $outfile;
my $verbose;
GetOptions("o=s", \$outfile, "v+", \$verbose);

if (!defined $outfile) {
	die "ERROR: Syntax: genkif.pl -o <kif filename>\n";
} elsif (-f $outfile) {
	print STDOUT "REMARK: KIF generation skipped due to it already existing (in '$outfile').\n";
	exit 0;
} elsif (-e $outfile) {
	die "ERROR: KIF output file '$outfile' must not be a directory or other non-file type\n";
} elsif ($verbose) {
	print "Got output file of '$outfile'\n";
}

my $buildnum = $ENV{'BuildNumber'};
if (!defined $buildnum) {
	die "ERROR: KIF generation skipped due to the BuildNumber environment variable not being set.\n";
} elsif ($verbose) {
	print "Got build number of '$buildnum'\n";
}

my $namespace = "ki";
my $nsuri = "http://www.symbian.com/kif1";
my $schema = "kif1.xsd";

my $build;
my $version;
if ($buildnum =~ /^(.*)_Symbian_OS_v(.*)$/) {
	$build = $1;
	$version = $2;
	print "Parsed build number as $build, os version $version\n" if $verbose;
} else {
	die "ERROR: Build number '$buildnum' is not a valid build number\n";
}

# Generate the Kit Information File

print "Constructing the KIF data\n" if $verbose;

my $mapper = new XML::Simple('rootname' => $namespace.':kitinfo', 'searchpath' => '.');

my $hash = {
	'xsi:schemaLocation' => "$nsuri $schema",
	"xmlns:".$namespace => $nsuri,
	'xmlns:xsi' => 'http://www.w3.org/2001/XMLSchema-instance',
	'label' => $buildnum,
	$namespace.':release' => {
		'build' => $build,
		'version' => $version
	}
};

print "Writing the KIF data to '$outfile'\n" if $verbose;

open(my $out, ">$outfile") or die "ERROR: Couldn't open $outfile for writing: $!\n";

print $out $mapper->XMLout($hash, 'xmldecl' => "<?xml version='1.0'?>");

close($out) or die "ERROR: Couldn't write to $outfile".": $!\n";

print "Generated KIF in $outfile\n";
