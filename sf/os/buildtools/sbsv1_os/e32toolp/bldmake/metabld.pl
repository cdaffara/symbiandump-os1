# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# all variables called *Path* are set up to end with a backslash
# all variables called *Path or *File are stored as absolute (file)paths within makmake
# all variables called UpPath* are stored as relative paths within makmake
# 
#

use strict;

use FindBin;		# for FindBin::Bin
use File::Find;
use Cwd;

my $PerlLibPath;    # fully qualified pathname of the directory containing our Perl modules

BEGIN {
# check user has a version of perl that will cope
	require 5.005_03;
# establish the path to the Perl libraries: currently the same directory as this script
	$PerlLibPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlLibPath =~ s/\//\\/g;	# X:\epoc32\tools
	$PerlLibPath .= "\\";
}

use lib $PerlLibPath;
use E32env;
use Prepfile;
use Pathutl;

# autoflush prevents our ouptut from getting jumbled up
use FileHandle;
autoflush STDOUT 1;
autoflush STDERR 1;

my (@Commands, @BldInfDirs);
# should end in .mbc where .mbc means for "MetaBld Configuration file"
# must end in .mbc if passed on command-line
my $ConfigFile;

my $StartDir=&cwd();
my $relative;
	{
	# Process command-line
	
  	unless (@ARGV)
		{
		&Usage();
		}
	else
		{
		# check to see if we are using a relative path 
	  	if ($ARGV[0] eq "-r")
			{
			$relative = 1;
			shift @ARGV;
			}

		# check for a root from a config file in EPOCROOT dir as first argument
	  	if (-e "$ENV{EPOCROOT}$ARGV[0].mbc")
			{
		  	$ConfigFile="$ENV{EPOCROOT}$ARGV[0].mbc";
		  	shift @ARGV;
			}
		# check for a config file as the first argument
	  	elsif ($ARGV[0]=~/.mbc$/io)
			{
			$ConfigFile=shift @ARGV;
			if ($ConfigFile=~/^.:/o)
				{
				&Error("Config file can't be specified with a drive letter, as \"$ConfigFile\" is");
				}
			unless (-e $ConfigFile)
				{
				&Error("Can't find file $ConfigFile");
				}
			}
		if (@ARGV)
			{
			# pass any other arguments as commands
			@Commands=("@ARGV");
			}
		}
	}

	{
	# scan config file if necessary
	
	if ($ConfigFile)
		{
		# make config file path absolute
		$ConfigFile=&Path_AbsToWork($ConfigFile);
		
		my @Config2D;
		eval { &Prepfile_ProcessL(\@Config2D, $ConfigFile); };
		&Error($@) if $@;
		
		my %CheckDir;
		
		my @ConfigCommands;
		my $Section='';
		my @Death;
		my $Line;
		my $CurFile=$ConfigFile;
	  LINE: foreach $Line (@Config2D)
			{
			my $LineNum=shift @$Line;
			$_=shift @$Line;
			if ($LineNum eq '#')
				{
				$CurFile=$_;
				next;
				}
			if (/^SECTION_(\w*)$/io)
				{
				$Section=uc $1;
				if ($Section=~/^(COMMANDS|DIRS|OPTIONALDIRS)$/o)
					{
					if (@$Line)
						{
						push @Death, "$CurFile($LineNum) : Can't specify anything on the same line as a section header\n";
						}
					next LINE;
					}
				push @Death, "$CurFile($LineNum) : Unknown section header - $_\n";
				$Section=0;
				next LINE;
				}
			unshift @$Line, $_;
			if ($Section eq 'COMMANDS')
				{
				if ($$Line[0]=~/^ONEOFF$/io)
					{
					# check syntax for oneoff commands
					unless (@$Line>=3)
						{
						push @Death, "$CurFile($LineNum) : Too few arguments for oneoff command\n";
						}
					# resolve oneoff dir relative to .mb file location
					$$Line[1]=~s-^.*[^\\]$-$&\\-o; # add trailing backslash if necessary
					$$Line[1]=&Path_MakeAbs($CurFile, $$Line[1]);
					unless (-d $$Line[1])
						{
						warn "$CurFile($LineNum) : Can't find dir $$Line[1]\n";
						}
					}
				push @ConfigCommands, "@$Line";
				next LINE;
				}
			if ($Section eq 'DIRS' || $Section eq 'OPTIONALDIRS')
				{
				my $Dir;
				foreach $Dir (@$Line)
					{
					if ($Dir=~/^.:/o)
						{
						push @Death, "$CurFile($LineNum) : directory $Dir is specified with a drive letter\n";
						next;
						}
					$Dir=~s-^.*[^\\]$-$&\\-o;
					$Dir=&Path_MakeAbs($CurFile, $Dir); # dirs must be the same for check
					if ($CheckDir{uc $Dir})
						{
						# Silently ignore duplicate directories - #including of several .mbc files
						# will often cause directory duplication.
						# We can't do the same for duplicate commands because the order in which
						# the commands are executed might be significant.
						# push @Death, "$CurFile($LineNum) : Directory $Dir already specified\n";
						next;
						}
					print "$Dir\n";
					
					unless (-d $Dir)
						{
						if ($Section ne 'OPTIONALDIRS')
							{
							push @Death, "$CurFile($LineNum) : Can't find directory $Dir\n";
							}
						next;
						}
					push @BldInfDirs, $Dir;
					$CheckDir{uc $Dir}=$LineNum;
					}
				next LINE;
				}
			else
				{
				push @Death, "$CurFile($LineNum) : No section specified\n";
				}
			}
		
		if (@Death)
			{
			chomp $Death[$#Death];
			&Error(@Death);
			}
		
		# apply the commands unless already collected
		unless (@Commands)
			{
			&Error("$ConfigFile : No Commands specified") unless @ConfigCommands;
			@Commands=@ConfigCommands;
			}
		}
	
	# Should have commands now
	&Usage() unless @Commands;
	}

	{
	# Search for the BLD.INF files if necessary

	my $mystartdir;
	if ($relative)
	{
		$mystartdir = substr($StartDir, 2);
	}

	$mystartdir=~s:\/:\\:g;

	if ($mystartdir ne "\\")
		{
		$mystartdir=$mystartdir."\\";
		}

	unless (@BldInfDirs)
		{
		# find the files in the source directories - skip the EPOCROOT directory
		
		my $EPOCROOTDir=$E32env::Data{EPOCPath};
		$EPOCROOTDir=~s/^\\([^\\]*).*$/$1/o;
		
		opendir DIR, $mystartdir or &Error("Can't open dir: $!");
		# remove ., .. and EPOCROOT dir
		my @SrcDirs=grep !/^(\.\.?|$EPOCROOTDir|RECYCLER|System Volume Information)$/i, readdir DIR; 
		foreach (@SrcDirs)
			{
			# prepend with current path
			$_=$mystartdir.$_;
			}
		@SrcDirs=grep -d, @SrcDirs;
		find(\&BldInfDirs, @SrcDirs);
		@BldInfDirs=sort @BldInfDirs;

		# if we are doing it relative to current location, need to include current dir
		# if it contains a bld.inf
		if (-f "BLD.INF" && $relative)
			{
			push @BldInfDirs, $mystartdir;
			}

		}
	}

	{
	# Execute the commands
	
	my $Time=localtime;
	print "=== metabld started $Time.\n";
	my $Command;
	foreach $Command (@Commands)
		{
		
		$Time=localtime;
		# Check if we should execute this command just the once
		if ($Command=~s/^\s*ONEOFF\s+(\S+)\s+(.*)$/$2/io)
			{
			my $OneOffDir=$1;
			# execute the command once rather than for each BLD.INF directory
			chdir $OneOffDir or &Error("Can't change dir to $OneOffDir: $!");
			print
			(
			 "===-------------------------------------------------\n",
			 "=== $Command\n",
			 "===-------------------------------------------------\n",
			 "=== $Command started $Time.\n",
			 "=== $Command == $OneOffDir\n"
			);
			system( "$Command");
			}
		else
			{
			# execute the command for each BLD.INF directory
			print
			(
			 "===-------------------------------------------------\n",
			 "=== $Command\n",
			 "===-------------------------------------------------\n",
			 "=== $Command started $Time.\n",
			);
			my $Dir;
			foreach $Dir (@BldInfDirs)
				{
				chdir $Dir or &Error("Can't change dir to $Dir: $!");
				print "=== $Command == $Dir\n";
				system( "$Command");
				}
			}
		chdir $StartDir or &Error("Can't change dir to $StartDir: $!");
		$Time=localtime;
		print "=== $Command finished $Time.\n";
		}
	}


#################################################
#	SUBROUTINES
#################################################

sub Usage
	{
	print <<ENDHERESTRING;
usage: metabld [EPOCROOT .mbc file basename|.mbc config file] [command]
MetaBld is a tool for carrying out build commands across several components.
A .mbc config file contains directories and commands, eg:

SECTION_COMMANDS
bldmake bldfiles
abld target wins udeb
// "oneoff" means carry out command just once, not for each directory.
// First arg after "oneoff" must be a start dir, and can be "."
oneoff \\e32\\rombuild rom xxx
oneoff . \\e32test\\group\\abld test build wins urel
SECTION_DIRS
\\e32
\\f32\\group

It's possible to #include lists of dirs from other files if necessary,
and may be useful for carrying out different sets of commands on the same set
of directories via different .mbc files.
If a command is specified on the command-line, it will be executed instead
of any commands specified in a .mbc file.
If no directories are specified in a .mbc file, then all the directories
containing a bld.inf file on the current drive will be searched for instead.
ENDHERESTRING
exit 1;
}

sub Error (@)
	{
	
	die
	@_, "\n",
	"Error executing metabld.bat\n"
	;
	}

sub BldInfDirs
	{
	s-\/-\\-go;
	if (/^BLD.INF$/io)
		{
		$File::Find::dir=~s-\/-\\-go;
		push @BldInfDirs, $File::Find::dir;
		}
	}

