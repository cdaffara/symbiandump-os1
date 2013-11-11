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
# diffold.pl - Takes the output of testold.pl (or multiple concatenated outputs) and removed
# entries from it which are present in a second output of testold.pl (or multiple
# concatenated outputs).
# 
#

my ($file, $basefile) = readOpts(@ARGV);

diffOld($file, $basefile);

exit 0;

sub readOpts(@)
	{
	my (@args) = @_;

	my @paths = ();

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
				print STDERR "Try 'mergeold.pl --help' for help.\n";
				exit 1;
				}
			}
		else
			{
			if (scalar(@paths)>=2)
				{
				print STDERR "Diffold accepts only two arguments.\n\n";
				print STDERR "Try 'mergeold.pl --help' for help.\n";
				exit 1;
				}
			else
				{
				push @paths, $arg;
				}
			}
		}
	
	if (scalar(@paths)<2)
		{
		print STDERR "Diffold must be given two files to compare.\n\n";
		print STDERR "Try 'diffold.pl --help' for help.\n";
		exit 1;
		}
	
	return ($paths[0], $paths[1]);
	}

sub diffOld($$)
	{
	my ($file, $base) = @_;

	# Load $file

	my %files = ();

	open(FILE, $file);

	my $dir = undef;

	foreach my $line (<FILE>)
		{
		chomp($line);

		if ($line =~ /^\*/)
			{
			if ($line =~ /^\*DIR:/)
				{
				$dir = $line;
				$dir =~ s/^\*DIR:\s*//;

				$dir =~ s/[\/\\]*$//; # Remove trailing \/
				}
			else
				{
				close(FILE);
				die "'$file' is not a valid input.\n('$line' not recognised)\n";
				}
			}
		else
			{
			if (defined($dir))
				{
				$line =~ s/^[\/\\]*//; # Remove preceding \/

				$files{$dir."\\".$line}=1;
				}
			else
				{
				close(FILE);
				die "'$file' is not a valid input.\n(DIR must be set before '$line')\n";
				}
			}
		}
	
	close(FILE);

	# Compare against $base
	
	open (BASE, $base);

	$dir = undef;

	foreach my $line (<BASE>)
		{
		chomp($line);

		if ($line =~ /^\*/)
			{
			if ($line =~ /^\*DIR:/)
				{
				$dir = $line;
				$dir =~ s/^\*DIR:\s*//;

				$dir =~ s/[\/\\]*$//; # Remove trailing \/
				}
			else
				{
				close(BASE);
				die "'$base' is not a valid input.\n('$line' not recognised)\n";
				}
			}
		else
			{
			if (defined($dir))
				{
				$line =~ s/^[\/\\]*//; # Remove preceding \/

				delete $files{$dir."\\".$line};
				}
			else
				{
				close(BASE);
				die "'$base' is not a valid input.\n(DIR must be set before '$line')\n";
				}
			}
		}
	
	close(BASE);

	# Output comparison

	my $root = undef;

	foreach my $file (keys(%files))
		{
		if (defined($root))
			{
			do
				{
				$file =~ s/[^\/\\]*[\/\\]?$//; # Remove last dir/filename
				}
			until ((index($root, $file) == 0) || ($file !~ /[\/\\]/));
			
			if (index($root, $file) == 0)
				{
				$root = $file;
				}
			else
				{
				$root = "";
				}
			}
		else
			{
			$root = $file;
			$root =~ s/[^\/\\]*$// # Remove filename to leave path
			}
		}

	print "*DIR:$root\n";

	my $lenroot = length($root);

	foreach my $file (sort(keys(%files)))
		{
		print substr($file, $lenroot)."\n";
		}
	}

sub showHelp()
	{
	print "diffold.pl [options] File Base-file\n";
	print " - Takes file and removes any entries which are also\n";
	print "   in the base-file. Both files are assumed to be\n";
	print "   the output (or a concatenation of outputs) of testold.pl\n\n";
	print "Options:\n";
	print "  --help or -h - Display this message\n\n";
	}

