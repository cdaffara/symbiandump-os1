#!perl

# InstallSupplementaryKit.pl - Source Code Integration Script

# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Versions:
# 1.0  Initial
# 1.1  No warnings about pre-existing directories
# 1.2  MCL version, adds -buildfromclean flag to move epoc32 directory and -platform flag
# 
#


use Getopt::Long;
use File::Path;
use File::CheckTree;


# ---------------------------- Global variables -------------------------------

my $JarFile = "";
my $TargetDirectory = "";
my $BuildFromClean = 0;
my $Platform = "";
my $Options = "";

# start the program
main();




# ---------------------------- Start of Main() --------------------------------
sub main()
{
	if (@ARGV)
	{
		CommandLineInterface();
	}
	else
	{
		UserInterface();
	}


	# extract any *.sdkpkg files from the Jar file into the target directory
	system("unzip -o  \"$JarFile\"  \"*.sdkpkg\"  -d \"$TargetDirectory\"");

	print "\n";

	# install each Jar file and delete it when done
	foreach my $Package (<$TargetDirectory\\*.sdkpkg>)
	{
		# if -o specified, only install the required options
		# (if -b specified, combine up into \bin\generic and \bin\techview)

		if ( ($Options ne "") )
		{	
			if ( ($Package =~ /bin\.GT-/i) && ($Options =~ /G/i) )
			{
				# GT binaries
				print "  installing $Package\n";
				if ($BuildFromClean == 1)
				{
					system("unzip -o -q \"$Package\"  -x \"package.xml\"  -d \"$TargetDirectory\"");
				}
				else
				{
					# install to temporary location then move
					system("unzip -o -q \"$Package\"  -x \"package.xml\"  -d \"$TargetDirectory\\tmp\"");
			 		MoveDirectory("$TargetDirectory\\tmp\\bin\\techview", "$TargetDirectory\\bin\\TechView\\generic");
					rmtree "$TargetDirectory\\tmp" or print "Unable to remove \"$TargetDirectory\\tmp\" - $!\n";
				}
			}
			elsif ( ($Package =~ /bin\.GT-/i) && ($Options !~ /G/i) )
			{
				print "  excluded   $Package\n";
			}

			elsif ( ($Package =~ /bin\.TechView-/i) && ($Options =~ /U/i) )
			{
				# TechView binaries
				print "  installing $Package\n";
				if ($BuildFromClean == 1)
				{
					system("unzip -o -q \"$Package\"  -x \"package.xml\"  -d \"$TargetDirectory\"");
				}
				else
				{
					# install to temporary location then move
					system("unzip -o -q \"$Package\"  -x \"package.xml\"  -d \"$TargetDirectory\\tmp\"");
			 		MoveDirectory("$TargetDirectory\\tmp\\bin\\techview", "$TargetDirectory\\bin\\TechView\\TechView");
					rmtree "$TargetDirectory\\tmp" or print "Unable to remove \"$TargetDirectory\\tmp\" - $!\n";
				}
			}
			elsif ( ($Package =~ /bin\.TechView-/i) && ($Options !~ /U/i) )
			{
				print "  excluded   $Package\n";
			}

			elsif ( ($Package =~ /doc\./i) && ($Options =~ /D/i) )
			{
				# documentation - install to 'doc' location then move
				print "  installing $Package\n";
				system("unzip -o -q \"$Package\"  -x \"package.xml\"  -d \"$TargetDirectory\"");
			}
			elsif ( ($Package =~ /doc/i) && ($Options !~ /D/i) )
			{
				print "  excluded   $Package\n";
			}

			elsif ( ( ($Package =~ /\.bin\./i) && ($Options =~ /(G|U)/i) ) ||
				( ($Package =~ /\.src\./i) && ($Options =~ /S/i) ) ||
				( ($Package !~ /\.src\./i) && ($Package !~ /\.bin\./i) ) )
			{
				# non-specific binaries or sources or non-binaries, non-sources
				system("unzip -o -q \"$Package\"  -x \"package.xml\"  -d \"$TargetDirectory\"");
				print "  installing $Package\n";
			}
			elsif ( ( ($Package =~ /\.src\./i) && ($Options !~ /S/i) ) ||
				( ($Package =~ /\.bin\./i) && ($Options !~ /(G|U)/i) ) )
			{
				print "  excluded   $Package\n";
			}
			else
			{
				print "Failed to install $Package\n";
			}
		}
	
		# otherwise install everything else regardless (non-BFC or no options specified)
		else
		{
			# sources in BuildFromClean installation, everything if BFC with no options, or non-BFC
			print "  installing $Package\n";
			system("unzip -o -q \"$Package\"  -x \"package.xml\"  -d \"$TargetDirectory\"");
		}
		unlink $Package or print "Unable to delete $Package file from \"$TargetDirectory\"\n";
	}
	

	# move files from [sdkroot]\src to src\, delete [sdkroot] directory
	if ( -e "$TargetDirectory\\\[sdkroot\]" )
	{
 		MoveDirectory("$TargetDirectory\\\[sdkroot\]", "$TargetDirectory");
		rmdir "$TargetDirectory\\\[sdkroot\]" or print "Unable to remove \"$TargetDirectory\\\[sdkroot\]\" - $!\n";
	}


	# rename 'doc' directory to 'documentation' 
	if ( -e "$TargetDirectory\\doc") 
	{
 		MoveDirectory("$TargetDirectory\\doc", "$TargetDirectory\\documentation");
		rmdir "$TargetDirectory\\doc" or print "Unable to remove \"$TargetDirectory\\doc\" - $!\n";
	}


	# move binary directories if platform defined for BuildFromClean - moves epoc32 tree from bin\TechView to \bin\%platform%
	if ( ($BuildFromClean == 1) && (-e "$TargetDirectory\\bin\\TechView") && ($Platform ne "") )
	{
	 	MoveDirectory("$TargetDirectory\\bin\\TechView", "$TargetDirectory\\bin\\$Platform");
		rmdir "$TargetDirectory\\bin\\TechView" or print "Unable to remove \"$TargetDirectory\\bin\\TechView\" - $!\n";
	}


	# remove the [meta] tree (usually only contains licence text)
	rmtree ("$TargetDirectory\\\[meta\]", 0, 1);
}

# ------------------- Start ofCommandLineInterface() -------------------------


sub CommandLineInterface()
{
	%CmdLineOptions = ();
	my $help;
	if ( (GetOptions( \%CmdLineOptions, "jarfile|j=s" => \$JarFile, "target|t=s" => \$TargetDirectory, "help|h|?" => \$help, "buildfromclean|b" => \$BuildFromClean, "platform|p=s" => \$Platform, "options|o=s" => \$Options) == 0) || ($JarFile eq "") || ($TargetDirectory eq "") || ($help == 1) )
	{
		Usage();
		exit; 
	}


	if ( ($BuildFromClean == 1) && ($Platform eq "") && ($ENV{'Platform'} eq "") )
	{
		Usage();
		print "\nN.B. -platform required if -b specified\n";
		exit; 
	}
	elsif ( ($BuildFromClean == 1) && ($Platform eq "") )
	{
		$Platform = $ENV{'Platform'} ;
	}

	# NAHEED --- Prevent stopping of the scripts if the Suplementary
	# product is not present. Rather than die just display a 
	# WARNING ....
	# check that Kit zips are in stated directory
	if (! -e $JarFile)
	{
		print "Warning: Jar file $JarFile not found.\n";
		exit;
	}
	
	
	# ensure target directory exists or can be created
	if (! -d $TargetDirectory)
	{
		mkpath ($TargetDirectory) or die "Could not find or create $TargetDirectory\n";
	}
	
}


# ------------------------ Start of UserInterface() ---------------------------


sub UserInterface()
{
	# get name of Supplementary Kit to install
	do	# ensure file exists
	{
		print "Enter name of Supplementary Kit (*.jar) : ";
		chomp( $JarFile = <STDIN> );
	} while (! -e $JarFile) ;


	# get location to write extracted files to
	do	# ensure directory exists or can be created
	{
		print "Enter directory to extract files to : ";
		chomp( $TargetDirectory = <STDIN> );
	} while ( (! -d $TargetDirectory) and (! mkpath ($TargetDirectory)) ) ;


	# get Platform name - try environment, else ask user
	if ($ENV{'Platform'} eq "")
	{
		print "Enter platform name  : ";
		chomp( $Platform = <STDIN> );
	}
	else
	{
		$Platform = $ENV{'Platform'} ;
	}
	
	
	# get build from clean flag
	my $answer = "";
	do
	{
		print "Build from clean (epoc32 tree combination) (Y/N) ? ";
		chomp ($answer = <STDIN>);
	} while ($answer !~ /[YN]/i);
	if ($answer =~ /Y/i)
	{
		$BuildFromClean = 1;
	}
	
	
	# get installation options
	print "Enter installation options : ";
	chomp($Options = <STDIN>);
}


# ------------------------ Start of MoveDirectory() ---------------------------


# Recursively moves directory tree

sub MoveDirectory($$)
{
	my ($aFrom, $aTo) = @_;

	opendir(DIR, $aFrom) or die "Path $aFrom not found!\n";

	foreach my $entry (readdir(DIR))
	{
		if (($entry ne ".") and ($entry ne ".."))
		{
			if (-d $aFrom."/".$entry)
			{ 
				if (!(makepath ($aTo."/".$entry)))
				{
					print "Warning: Could not create $aTo/$entry - $!\n";
				}
				MoveDirectory($aFrom."/".$entry, $aTo."/".$entry);
				rmdir $aFrom."/".$entry or print "Warning: Unable to remove $aFrom/$entry - $!\n";
			}
			else
			{
				rename $aFrom."/".$entry, $aTo."/".$entry or print "Warning: Unable to move $aFrom/$entry - $!\n";
			}
		}
	}

	closedir(DIR);
}

# -------------------------- Start of makepath() ------------------------------

sub makepath($)
	{
	my ($path) = @_;

	if (-d $path)
		{
		return -1;
		}
	else
		{
		return mkpath($path);
		}
	}

# ---------------------------- Start of Usage() -------------------------------

sub Usage()
{
	print <<ENDOFUSAGETEXT;
	
INSTALLSUPPLEMENTARYKIT.PL    Version 1.2    Copyright (c) 2003 Symbian Ltd
                                             All Rights Reserved
                                  
Usage:
  perl InstallSupplementaryKit.pl  [options]
 
where options are:
  -b[uildfromclean]       combines unpacked files into build from clean locations
  -j[arfile] <path>       fully qualified file name of Kit's .jar
  -o[ptions] DGUS         installation options ( D=documentation,  G=GT, U=UI(TechView),  S=source )
  -p[latform] <platform>  build platform - used with -b to create binaries installation directory path
                          (moves epoc32 tree from \\bin\TechView into \\bin\\<platform>)
  -t[arget] <path>        path to directory to unpack Kit into
   
ENDOFUSAGETEXT
}
