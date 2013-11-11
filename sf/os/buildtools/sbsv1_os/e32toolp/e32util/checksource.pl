# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Front-end to routines checking that source matches filename policy constraints
# 
#

use strict;
use FindBin;
use Cwd;
use Getopt::Long qw(:config posix_default);		#permits "+" in arguments, rather than as an option identifier
use lib $FindBin::Bin;
use CheckSource;
use Pathutl;

my $preprocessCall = "cpp.exe -w -undef -nostdinc -+ -dI";

my $verbose = 0;

my $preprocess = 0;
my $parsefile = 0;
my $metadata = 0;

GetOptions ('preprocess|pp' => \$preprocess, 'parsefile|pf' => \$parsefile, 'metadata|md' => \$metadata);

my %warnings;

$verbose = 1 if $ENV{CHECKSOURCE_VERBOSE};


if (!@ARGV)
	{
	print ("\nchecksource.pl --preprocess|--parsefile|--metadata args\n\n");
	}
elsif ($preprocess)
	{
	my $PREPROCESSPIPE;
	open PREPROCESSPIPE,"$preprocessCall @ARGV |" or die "ERROR: Can't invoke preprocessor for checksource processing.\n";

	my %processedIncludes;
	
	my %homelessInclude;
	$homelessInclude{REFERENCE} = "";

	my $lineNumber = 0;
	my $currentFile = "";

	print "Preprocessing file    : ".$ARGV[scalar(@ARGV)-1]."\n" if ($verbose);

	while (<PREPROCESSPIPE>)
		{
		if (/^\s*$/o)
			{
			$lineNumber++;
			next;
			}	
		elsif (/^# (\d+) "(.*)"( \d+)?/o)
			{
			$lineNumber = scalar $1;
			$currentFile=$2;
			$currentFile=~s-\\\\-\\-go;
			$currentFile=~s-\\\/-\\-go;
			$currentFile=~s-\/\\-\\-go;
			$currentFile=~s-\/\/-\\-go;
			$currentFile=~s/\//\\/g;
			$currentFile=~s/^\w://;
			}

		if ($homelessInclude{REFERENCE})
			{
			# Resolve #include references using "locating" comments in CPP output.  These may either follow the reference directly,
			# or may have appeared earlier in prior CPP processing
			if ($currentFile =~ /$homelessInclude{SEARCH}$/i)
				{
				CheckSource_Physical (%warnings, $homelessInclude{SOURCEFILE}, "#include", $homelessInclude{REFERENCE}, $homelessInclude{LINENUMBER}, $currentFile, $verbose);
				$processedIncludes{lc($currentFile)} = 1;
				}
			else
				{
				my @foundProcessedIncludes;
				foreach my $processedInclude (keys %processedIncludes)
					{
					push @foundProcessedIncludes, $processedInclude if ($processedInclude =~ /$homelessInclude{SEARCH}$/i);
					}
		
				if (@foundProcessedIncludes == 1)
					{
					CheckSource_Physical  (%warnings, $homelessInclude{SOURCEFILE}, "#include", $homelessInclude{REFERENCE}, $homelessInclude{LINENUMBER}, $foundProcessedIncludes[0], $verbose);
					}
				elsif ((@foundProcessedIncludes > 1) && $verbose)
					{
					print ("WARNING: Multiple matches for #include reference $homelessInclude{REFERENCE} : $homelessInclude{SOURCEFILE}($homelessInclude{LINENUMBER})\n");
					print "\t$_\n" foreach (@foundProcessedIncludes);
					}
				elsif ($verbose)
					{
					print ("WARNING: Couldn't find #include reference $homelessInclude{REFERENCE} : $homelessInclude{SOURCEFILE}($homelessInclude{LINENUMBER})\n");
					}
				}
			$homelessInclude{REFERENCE} = "";
			}

		if (/^\s*#include\s*[\"|\<]{1}(.*)[\"|\>]{1}/)
			{
			CheckSource_UnixSlash (%warnings, $currentFile, "#include", $1, $lineNumber, $verbose);

			$homelessInclude{SOURCEFILE} = $currentFile;
			$homelessInclude{REFERENCE} = $1;
			$homelessInclude{LINENUMBER} = $lineNumber;
			$homelessInclude{SEARCH} = "\\".$homelessInclude{REFERENCE};
			$homelessInclude{SEARCH} =~ s/\//\\/g;
			$homelessInclude{SEARCH} = quotemeta($homelessInclude{SEARCH});
			}

		$lineNumber++ if (!/^# (\d+) "(.*)"( \d+)?/o);
		}

	close PREPROCESSPIPE;
	}
elsif ($parsefile)
	{
	my $SOURCE_FILE = $ARGV[0];		
	open SOURCE_FILE, "< $SOURCE_FILE" or die "ERROR: Can't open $SOURCE_FILE for \"-checksource\".\n";

	print "Parsing include file  : $SOURCE_FILE\n" if ($verbose);

	my $lineNumber = 0;
	
	while (<SOURCE_FILE>)
		{
		$lineNumber++;

		if (/^\s*#include\s*[\"|\<]{1}(.*)[\"|\>]{1}/)
			{
			CheckSource_UnixSlash (%warnings, $SOURCE_FILE, "#include", $1, $lineNumber, $verbose);
			CheckSource_Lowercase (%warnings, $SOURCE_FILE, "#include", $1, $lineNumber, $verbose);
			}		
		}

	close SOURCE_FILE;
	}
elsif ($metadata)
	{
	my ($sourcefile, $item, $reference, $linenumber, $physical, $offset) = @ARGV;

	$physical = 0 if (!$physical);
	$offset = undef if (!$offset);

	CheckSource_UnixSlash (%warnings, $sourcefile, $item, $reference, $linenumber, $verbose);

	if ($physical)
		{
		CheckSource_Physical (%warnings, $sourcefile, $item, $reference, $linenumber, $offset, $verbose);
		}
	else
		{
		CheckSource_Lowercase (%warnings, $sourcefile, $item, $reference, $linenumber, $verbose);
		}
	}

print "$_\n" foreach (keys %warnings);
