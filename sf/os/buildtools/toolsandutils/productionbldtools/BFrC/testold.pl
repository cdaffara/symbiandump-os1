#!/usr/bin/perl

# Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# testold.pl - check the last modified date of all files under a path to see if
# the last modified date is after 00:00 1 Jan 1970
# 
#

$date = 500000000;

my ($path) = readOpts(@ARGV);

print "*DIR:$path\n";

testOld($path, "", $date);

exit 0;

sub readOpts(@)
	{
	my (@args) = @_;

	my $path = undef;

	foreach my $arg (@args)
		{
		if ($arg =~ /^-/)
			{
			if ((lc($arg) eq "--help")
			  ||(lc($arg) eq "-h")
			   )
			   	{
				showHelp();
				exit 0;
				}
			else
				{
				print STDERR "Option '$arg' not recognised.\n\n";
				print STDERR "Try 'testold.pl --help' for help.\n";
				exit 1;
				}
			}
		else
			{
			if (defined($path))
				{
				print STDERR "Testold accepts only one argument.\n\n";
				print STDERR "Try 'testold.pl --help' for help.\n";
				exit 1;
				}
			else
				{
				$path = $arg;
				}
			}
		}
	
	if (!defined($path))
		{
		print STDERR "Testold must be given a path to set.\n\n";
		print STDERR "Try 'testold.pl --help' for help.\n";
		exit 1;
		}
	
	return ($path);
	}

sub testOld($$$)
	{
	my ($root, $path, $date) = @_;
	
	opendir(PATH, $root."\\".$path);
	my @dir = readdir(PATH);
	closedir(PATH);

	foreach $entry (@dir)
		{
		if ($entry =~ /^\.\.?$/)
			{
			}
		elsif (-d $root."\\".$path."\\".$entry)
			{
			testOld($root, $path."\\".$entry, $date);
			}
		else
			{
			# Check file's last modified stamp
			my ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size, $atime, $mtime, $ctime, $blksize, $blocks) = stat($root."\\".$path."\\".$entry);

			if ($mtime > $date)
				{
				print $path."\\".$entry."\n";
				}
			}
		}
	}

sub showHelp()
	{
	print "testold.pl [options] Path\n";
	print " - checks the last modified date of all files under a path to see\n";
	print "   if it's newer than ".localtime($date)."\n\n";
	print "  Path - The path to the root of the files to be modified\n";
	print "Options:\n";
	print "  --help or -h - Display this message\n\n";
	}

