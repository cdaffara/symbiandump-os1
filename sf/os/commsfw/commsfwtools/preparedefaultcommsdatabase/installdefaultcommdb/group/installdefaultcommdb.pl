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

#!perl

use strict;
use File::Copy;
use File::Path;
use File::stat;
use Getopt::Long;
use Cwd;

if ($^O =~ /MSWin32/)
{
	require Win32::Mutex;
}

$::emulatorCfg = 'emulator.cfg';
$::targetCfg = 'target.cfg';

GetOptions (
           'command=s' => \$::command,
           'platform=s' => \$::platform,
           'variant=s' => \$::variant,
           'targetcfg=s' => \$::targetCfg,
           'emulatorcfg=s' => \$::emulatorCfg,
           'platsec!' => \$::platsec
		   );

#
# Constants.
#

my $epocRoot = GetEpocRoot();

#
# Main.
#
unless ($::command and $::platform and $::variant and (scalar(@ARGV) == 0)) 
{
	die "InstallDefaultCommdb Error: Invalid arguments\n";
}

my $platType;
if ($::platform =~ /^(wins|wincw)/i) 
{
	$platType = 'emulator';
}
else 
{
	$platType = 'target';
}


my $cccccc00Root;
my $cccccc00NameCre;

$cccccc00Root = "/private/10202be9/";
$cccccc00NameCre = "cccccc00.cre";

my $data = 
{
	target => 
	{
		destinationCdb => $epocRoot . 'epoc32/data/z' . $cccccc00Root . $cccccc00NameCre,
	},
	emulator => 
	{
		destinationCdb => $epocRoot . "epoc32/release/$::platform/$::variant/z" . $cccccc00Root . $cccccc00NameCre,
	}
};

my $CdbDest;
my $cdbOut; 
my $cdbOutPath;
my $cdbDatFile = $ENV{DEFAULTCOMMSDATDATAFILE};

$cdbOut = '/c/defaultcommdb/';

$cdbOutPath = $epocRoot. 'epoc32/winscw' . $cdbOut;
$cdbDatFile = $cdbOutPath . 'CommsDb.dat' if (! defined $cdbDatFile);

$CdbDest = $data->{$platType}->{destinationCdb};

##
## NOTE: all copying and releasing is now done from defaultcommdb - this installdefaultcommdb should be entirely deleted, system model updated, etc
##

#if ($::command eq 'build') 
#{
#	PreBuild();
#}
#elsif ($::command eq 'clean')
#{
#	Clean();
#}
#elsif ($::command eq 'releasables') 
#{
#	Releasables();
#}

#
# Subs.
#

sub PreBuild
{
	if ($^O =~ /MSWin32/)
	{
		# We cannot have more than one instance of this script's activites running at the same time on Windows, so
		# we safeguarded everything with a Windows mutex.  Mutex name needs to be unique for every build,
		# so we add the current working directory and EPOCROOT to the mutex name
		
		my $drive = Win32::GetCwd();
		$drive =~ s/^(\D:).*/$1/;
		my $mutexname = $drive.lc($ENV{'EPOCROOT'});
		$mutexname =~ s/[:\\\/]+/_/g;
		
		my $mutex = Win32::Mutex->new(1,"CEDMUTEX_".$mutexname);
		die "Unable to create mutex, installdefaultcommdb.pl not run" if not $mutex;
		if ($mutex->wait())
		{
			Build();
			$mutex->release();
		}
		else
		{
			die "Unable to access mutex, installdefaultcommdb.pl not run";
		}
	}
	else
	{
		Build();
	}
}

sub Build
{
	unless (-e $cdbDatFile) 
	{
		die "InstallDefaultCommdb Error: $cdbDatFile cannot be found: $!\n";
	}

	if (FileIsYounger($cdbDatFile, $CdbDest) or not -e $CdbDest)
	{
		print ("Copy $cdbDatFile to $CdbDest\n");
		CopyFile($cdbDatFile, $CdbDest);
	}
}


sub Clean 
{
	print("Deleting $CdbDest..\n");

	# Temporary workaround as default commsdat databases are not being created.
	#DeleteFile($CdbDest);
}

sub Releasables 
{
	print("$CdbDest\n");
}

sub CopyFile 
{
	my $file1 = shift;
	my $file2 = shift;
	(my $path) = SplitFileName($file2);
	
	unless (-e $path) 
	{
		mkpath ($path) or die "InstallDefaultCommdb Error: Couldn't make path \"$path\": $!\n";
	}
	
	if (-e $file2 and not -w $file2) 
	{
		system "attrib -r $file2";
	}
	copy ($file1, $file2) or die "InstallDefaultCommdb Error: Couldn't copy \"$file1\" to  \"$file2\": $!\n";
}

sub MoveFile 
{
	my $file1 = shift;
	my $file2 = shift;
	(my $path) = SplitFileName($file2);
	
	unless (-e $path) 
	{
		mkpath ($path) or die "InstallDefaultCommdb Error: Couldn't make path \"$path\": $!\n";
	}
	move ($file1, $file2) or die "InstallDefaultCommdb Error: Couldn't move \"$file1\" to  \"$file2\": $!\n";
}

sub DeleteFile 
{
	my $file = shift;
	if (-e $file) 
	{
		unlink ($file) or die "InstallDefaultCommdb Error: Couldn't delete \"$file\": $!\n";
	}
}

sub SplitFileName 
{
	my $fileName = shift;
	my $path = '';
	my $base = '';
	my $ext = '';
	
	if ($fileName =~ /\/?([^\/]*?)(\.[^\/\.]*)?$/) 
	{
		$base = $1;
	}
	if ($fileName =~ /^(.*\/)/) 
	{
		$path = $1;
	}
	if ($fileName =~ /(\.[^\/\.]*)$/o) 
	{
		$ext =  $1;
	}
	
	die unless ($fileName eq "$path$base$ext");
	return ($path, $base, $ext);
}

sub CopyIfYounger 
{
	my $from = shift;
	my $to = shift;
	unless (-e $from) 
	{
		die "Error: \"$from\" not found\n";
	}
	
	if (FileIsYounger($from, $to)) 
	{
		CopyFile($from, $to);
	}
}

sub FileIsYounger 
{
	my $file1 = shift;
	my $file2 = shift;
	return (FileModifiedTime($file1) > FileModifiedTime($file2));
}

sub FileModifiedTime 
{
	my $file = shift;
	if (-e $file) 
	{
		my $st = stat($file) or return 0;
		return $st->mtime;
	}
	return 0;
}

sub RenameFile
{
	my $file1 = shift;
	my $file2 = shift;
	
	rename ($file1, $file2) or die "InstallDefaultCommdb Error: Couldn't rename \"$file1\" to \"$file2\": $!\n";
}

sub GetEpocRoot
{
	my $path = $ENV{EPOCROOT};

	# replace to forward slashes
	$path =~ s/\\/\//g;

	# append trailing slash if one isn't already present
	if($path =~ m/.*\/$/)
	{
		return $path;
	}
	else
	{
		return $path."\/";
	}
}

