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
# setold.pl - sets the last modified date of all files under a path to 1970
# 
#

$date = 500000000; # e.g 0 = 00:00 1st Jan 1970, 1000000000 (10^9) = 01:46 9th Sep 2001 

my ($path, $batch) = readOpts(@ARGV);

if (!$batch)
	{
	print "Setting all files under path '$path' to be last modified at ".localtime($date).".\nAre you sure (Y/N)?\n";

	if (lc(getc(STDIN)) ne "y")
		{
		print "Aborting.\n";
		exit 0;
		}
	}

setOld($path,$date);

exit 0;

sub readOpts(@)
	{
	my (@args) = @_;

	my $path = undef;
	my $batch = 0;

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
			elsif (lc($arg) eq "--batch")
				{
				$batch = 1;
				}
			else
				{
				print STDERR "Option '$arg' not recognised.\n\n";
				print STDERR "Try 'setold.pl --help' for help.\n";
				exit 1;
				}
			}
		else
			{
			if (defined($path))
				{
				print STDERR "Setold accepts only one argument.\n\n";
				print STDERR "Try 'setold.pl --help' for help.\n";
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
		print STDERR "Setold must be given a path to set.\n\n";
		print STDERR "Try 'setold.pl --help' for help.\n";
		exit 1;
		}
	
	return ($path, $batch);
	}

sub setOld($$)
	{
	my ($path,$date) = @_;
	
	opendir(PATH, $path);
	my @dir = readdir(PATH);
	closedir(PATH);

	foreach $entry (@dir)
		{
		if ($entry =~ /^\.\.?$/)
			{
			}
		elsif (-d $path."\\".$entry)
			{
			setOld($path."\\".$entry,$date);
			}
		else
			{
			# Set file's last modified stamp
			my ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size, $atime, $mtime, $ctime, $blksize, $blocks) = stat($path."\\".$entry);

			utime $atime, $date, ($path."\\".$entry) or print "Failed to set $path\\$entry\n";
			}
		}
	}

sub showHelp()
	{
	my ($date) = @_;
	
	print "setold.pl [options] Path\n";
	print " - sets the last modified date of all files under a path to ".localtime($date)."\n\n";
	print "  Path - The path to the root of the files to be modified\n";
	print "Options:\n";
	print "  --batch - Don't prompt for confirmation (use with care)\n";
	print "  --help or -h - Display this message\n\n";
	print "Note that this tool always operates on all subdirectories of the given path\n";
	}

