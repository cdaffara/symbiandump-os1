#!perl
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

use strict;
use FindBin;
use File::Copy;

# Figure out what user has asked us to do...

my( $script_name ) = $0;
my( $mode ) = $ARGV[0];
my( $backup ) = $ARGV[1];
my( $epocextra ) = $ARGV[2];
my( $wsiniextra ) = $ARGV[3];

my( $usage ) = <<EOF;

This script will append or remove extra options to the epoc.ini and
wsini.ini emulator configuration files. 
It is intended to be used before and after any emulator tests
that require multiple emulator screens.

Usage:

$script_name help
	Displays this message.

$script_name append $backup $epocextra $wsiniextra
	Makes back-up copies of the epoc.ini and wsini.ini config files to files 
	prefixed with $backup and then appends the contents of $epocextra and 
	$wsiniextra to the originals. Note that $backup must be unique or append 
	will fail.

$script_name restore $backup
	Moves the back-up copies of epoc.ini and wsini.ini to their original
	locations from files prefixed with $backup

Notes:
	The wsini.ini file is a unicode text file encoded using UTF-16. The
	wsini.extra file MUST therefore be saved with the same encoding
	otherwise the resulting concatenated file will not work. (You don't need
	to worry about the extra BOM at the start of wsini.extra since this
	script takes care of removing it before appending the file)

	The original epoc.ini and wsini.ini files are NOT checked prior to the
	appending. If you have customised these files (especially with any
	multi-screen options) the resulting concatenated files may be invalid
	or cause the tests to fail.

EOF

if( ($mode eq 'help') || ( $mode eq '' ) ){
	print $usage;
	exit;
}
elsif( ($mode ne 'append') && ($mode ne 'restore') ){
	die "$script_name: ERROR: Invalid argument: \"$mode\". Must be either \"help\", \"append\" or \"restore\"\n".$usage;
}

##########################################

# Append $source to $dest. If $is_utf_16 the BOM marker at the start of $source will be
# stripped off to ensure that the $dest remains a valid UTF-16 file.
sub append_to_file{
	my( $source, $dest, $is_utf_16 ) = @_;
	my( $line, $did_first );
	$did_first = 0;
	
	open SOURCE, $source or die "$script_name: ERROR: Could not open $source ($!)\n";
	open DEST, '>>', $dest or die "$script_name: ERROR: Could not open $dest ($!)\n";

	if( $is_utf_16 ){
		# since our old version of Perl does not have decent Unicode support
		# we'll use binary mode instead...
		binmode SOURCE;
		binmode DEST;
		
		while( read( SOURCE, $line, 1000 ) ){
			if( !$did_first ){
				# strip BOM (first two bytes) off first line, since it is being appended to an
				# existing UTF-16 file
				$line = substr( $line, 2 );
				$did_first = 1;
			}
			print DEST $line;
		}
	}
	else{
		# bog-standard ASCII
		while( $line = <SOURCE> ){		
			print DEST $line;
		}
	}
	
	close SOURCE;
	close DEST;
}

##########################################
# Begin main logic...

# Figure out locations of INI files...
my( $epoc_root, $epocini, $udeb_wsini, $urel_wsini, $extra_epocini, $extra_wsini, $copy_epocini, $copy_udeb_wsini, $copy_urel_wsini );

$epoc_root = $ENV{'EPOCROOT'};
$epoc_root =~ tr:\\:/:; # change to Linux-friendly forward-slashes (Windows Perl compensates for this automagically)
$epoc_root =~ s/\/$//; # remove any trailing slash to avoid double slashes when the paths are appended below

$epocini = $epoc_root.'/epoc32/data/epoc.ini';
#if this is defined we are running Mistral
if($ENV{'EPOC_INI'}) {$epocini = $ENV{'EPOC_INI'};}

$udeb_wsini = $epoc_root.'/epoc32/RELEASE/WINSCW/UDEB/Z/system/data/wsini.ini'; # this file is UTF-16 little-endian!
$urel_wsini = $epoc_root.'/epoc32/RELEASE/WINSCW/UREL/Z/system/data/wsini.ini'; # this file is UTF-16 little-endian!

my $emu_data_dir = $ENV{'EMULATOR_DATA_DIR'};
if($emu_data_dir) 
	{
	#this is mistral so we will overload $urel_wsini with absolute location
	$urel_wsini = $emu_data_dir.'z\system\data\wsini.ini';
	}

$extra_epocini = $FindBin::Bin.$epocextra; # this file is ASCII
$extra_wsini = $FindBin::Bin.$wsiniextra; # this file is UTF-16 little-endian!

$copy_epocini = $FindBin::Bin.'/'.$backup.'_epoc.copy';
$copy_udeb_wsini = $FindBin::Bin.'/'.$backup.'_wsini_udeb.copy';
$copy_urel_wsini = $FindBin::Bin.'/'.$backup.'_wsini_urel.copy';


if( $mode eq 'append' ){
	# Append mode: Append extra options to existing INI files
	
	# first make back-up of existing INI files
	# (without clobbering existing copies)	
	if( -e $copy_epocini ){
		die "$script_name: ERROR: Back-up of epoc.ini already exists at \"$copy_epocini\". Please run \"$script_name restore\" first.\n";
	}
	else{
		copy( $epocini, $copy_epocini ) or die "$script_name: ERROR: Could not copy $epocini ($!)\n";
	}
	# now append extra bits to original INI files
	append_to_file( $extra_epocini, $epocini, 0 );
	print "$script_name: NOTE: Extra settings have been appended to \"$epocini\". If you have customised this file and secondary display tests fail, please check for conflicting settings!\n";
	
	## UREL wsini.ini + mistral universal
	if( -e $copy_urel_wsini ){
		die "$script_name: ERROR: Back-up of UREL wsini.ini already exists at \"$copy_urel_wsini\". Please run \"$script_name restore\" first.\n";
	}
	else{
		copy( $urel_wsini, $copy_urel_wsini ) or die "$script_name: ERROR: Could not copy $urel_wsini ($!)\n";
	}
	#note mistral will provide absolute location dependant on running mode	
	append_to_file( $extra_wsini, $urel_wsini, 1 );
	print "$script_name: NOTE: Extra settings have been appended to \"$urel_wsini\". If you have customised this file and secondary display tests fail, please check for conflicting settings!\n";
	
	
	#unless mistral in which case we dont bother keeping a backup of udeb as we adjust universal copy 
	#under guise of UREL path
	if(!$emu_data_dir)
	{
		if( -e $copy_udeb_wsini ){
			die "$script_name: ERROR: Back-up of UDEB wsini.ini already exists at \"$copy_udeb_wsini\". Please run \"$script_name restore\" first.\n";
		}
		else{
			copy( $udeb_wsini, $copy_udeb_wsini ) or die "$script_name: ERROR: Could not copy $udeb_wsini ($!)\n";
		}
		#
		append_to_file( $extra_wsini, $udeb_wsini, 1 );
		print "$script_name: NOTE: Extra settings have been appended to \"$udeb_wsini\". If you have customised this file and secondary display tests fail, please check for conflicting settings!\n";
	}
	
	
	
}else{
	# Restore mode: Move copies of original INI files back to original locations
	
	move( $copy_epocini, $epocini ) && print "$script_name: NOTE: \"$epocini\" has been restored to previous version.\n" or warn "$script_name: Could not restore $epocini from $copy_epocini ($!)\n";
	
	move( $copy_urel_wsini, $urel_wsini ) && print "$script_name: NOTE: \"$urel_wsini\" has been restored to previous version.\n" or warn "$script_name: Could not restore $urel_wsini from $copy_urel_wsini ($!)\n";
	
	if(!$emu_data_dir)
	{
		move( $copy_udeb_wsini, $udeb_wsini ) && print "$script_name: NOTE: \"$udeb_wsini\" has been restored to previous version.\n" or warn "$script_name: Could not restore $udeb_wsini from $copy_udeb_wsini ($!)\n";		
	}
}