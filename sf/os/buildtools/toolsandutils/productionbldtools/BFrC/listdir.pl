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
# listdir.pl - displays the contents of a directory and all its subdirectories
# 
#

my ($path, $files) = readOpts(@ARGV);

list($path, $files);

exit 0;

sub readOpts(@)
	{
	my (@args) = @_;

	my $path = undef;
	my $files = 0;

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
			if ((lc($arg) eq "--files")
			  ||(lc($arg) eq "-f")
			  )
			  	{
				$files = 1;
				}
			else
				{
				print STDERR "Option '$arg' not recognised.\n\n";
				print STDERR "Try 'listdir.pl --help' for help.\n";
				exit 1;
				}
			}
		else
			{
			if (defined($path))
				{
				print STDERR "Listdir accepts only one argument.\n\n";
				print STDERR "Try 'listdir.pl --help' for help.\n";
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
		print STDERR "Listdir must be given a path to list.\n\n";
		print STDERR "Try 'listdir.pl --help' for help.\n";
		exit 1;
		}
	
	return ($path, $files);
	}

sub list($)
	{
	my ($path, $files) = @_;
	
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
			if (!$files)
				{
				print $path."\\".$entry."\\\n";
				}
			list($path."\\".$entry, $files);
			}
		else
			{
			print $path."\\".$entry."\n";
			}
		}
	}

sub showHelp()
	{
	my ($date) = @_;
	
	print "listdir.pl [options] Path\n";
	print " - displays the contents of a directory and all its subdirectories\n\n";
	print "   As dir /S /B, except only the path below the current dir is given\n\n";
	print "  Path - The path to the root of the files to be modified\n";
	print "Options:\n";
	print "  --files or -f - Only list the files in each subdir, not the dirs themselves\n";
	print "  --help or -h - Display this message\n\n";
	}

