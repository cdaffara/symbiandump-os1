# Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
use Env qw(EPOCROOT);

# literals
my $icl_param  =  "-i";
my $misc_param =  "-c";
my $mmf_param  =  "-m";
my $all_param  =  "-f";
my $test_param =  "-t";
my $testBuild_param = "-b";
my $extra_param = "-x";
my $keep_param =  "-k";
my $gccxml_param = "-g";
my $skipMake_param = "-s";

my $setup_comm = "setup";
my $clean_comm = "clean";
my $build_comm = "build";

my @known_targets = ("wins", "winscw", "arm4", "armi","armv5", "gccxml", "thumb", "mcot", "misa", "mint");

my @Cedar_targets = ("winscw", "armv5");    # gccxml must be added explicitly with -g and even then only build gccxml on main code
my @Beech_targets = ("wins", "winscw", "thumb", "mcot");

my @default_targets = @Cedar_targets;

my @physical_targets = ("mcot", "misa", "mint"); # those we just re-build sound drivers for
my $gccxml_target = "gccxml";
my $x86gcc_target = "x86gcc";
my @stages = ("export", "makefile", "library", "resource", "target", "final");
my @tbstages = ("test export", "test makefile", "test library", "test resource", "test target", "test final");
my @stagesTakingTarget = ("makefile", "library", "resource", "target", "final",
                          "test makefile", "test library", "test resource", "test target", "test final");
my $eka2IdentifyFile  = "$ENV{EPOCROOT}epoc32\\release\\winscw\\udeb\\winsgui.dll"; # if file present, is EKA2 build
my $x86gccIdentifyFile  = "$ENV{EPOCROOT}epoc32\\release\\x86gcc\\udeb\\euser.dll"; # if dir present, x86 environment present

my @targets;
my $component = ""; # default added later
my $extras = "";
my $test = "";
my $testBuild = "";
my $command = "";
my $keep = "";
my $use_gccxml = "";
my $skipMake = "";

my $main_mbc = "__main.mbc";
my $test_mbc = "__test.mbc";
my $testBuild_mbc = "__testBuild.mbc";
my $phys_mbc = "__phys.mbc";
my $metabld = "metabld";

my $targetToolsDir = "..\\..\\..\\..\\..\\TargetTools\\Build";
my $TargetToolsExists = 0;
if (-d $targetToolsDir)
	{
	$TargetToolsExists = 1;
	print "TargetTools directory exists: $targetToolsDir\n"
	}

# main:
	{
	# Process command-line

	unless (@ARGV)
		{
		&Usage();
		}

	&ReadArgs();

	die "EPOCROOT is not set" unless ($EPOCROOT ne "");

	$| = 1; # autoflush stdout.

	if ($command eq $setup_comm)
		{
		&Setup();
		}
	elsif ($command eq $clean_comm)
		{
		&Clean();
		}
	elsif ($command eq $build_comm)
		{
		&Build();
		}
	else
		{
		die "Unknown command";
		}
	exit 0;
	}

#
# Subroutines
#

sub ReadArgs()
	{
	while (@ARGV)
		{
		my $param = @ARGV[0]; shift @ARGV; # grab first param and shift along
		
		if ($param eq "") # ignore blank parameters
			{
			next;
			}

		if ($param eq $icl_param || $param eq $misc_param ||
		    $param eq $mmf_param || $param eq $all_param)
			{
			$component = $param;
			}
		elsif ($param eq $test_param)
			{
			$test = $param;
			}
		elsif ($param eq $testBuild_param)
			{
			$testBuild = $param;
			}
		elsif ($param eq $extra_param)
			{
			$extras = $param;
			}
		elsif ($param eq $keep_param)
			{
			$keep = $param;
			}
		elsif ($param eq $gccxml_param)
			{
			$use_gccxml = $param;
			}
		elsif ($param eq $skipMake_param)
			{
			$skipMake = $param;
			}
		elsif ($param eq $setup_comm || $param eq $clean_comm ||
		       $param eq $build_comm)
			{
			if ($command ne "")
				{
				&Usage; # scream if we try to state more than one command
				}
			$command = $param;
			}
		elsif ($command eq $build_comm)
			{
			# see if the parameter is one of the list of arguments
			my $match = 0;
			foreach my $target (@known_targets)
				{
				if ($target eq $param)
					{
					$match = 1;
					last; # exit loop
					}
				}
			if ($match != 0)
				{
				$targets[++$#targets] = $param; # append
				}
			else
				{
				# was not a valid target
				Usage();
				}
			}
		else
			{
			# unknown setting
			&Usage();
			}
		}
	&CheckArgs();
	}

sub CheckArgs()
	{
	# check that the arguments make sense
	if ($command eq $clean_comm || $command eq $build_comm)
		{
		# for build and cleanup, can't set the required module
		if ($component ne "" || $extras ne "")
			{
			&Usage;
			}
		}
	# for setup, check -t has not been given
	if ($command eq $setup_comm)
		{
		if ($test ne "")
			{
			&Usage;
			}
		}
	if ($command eq "")
		{
		# need to state some command
		&Usage;
		}
	}

sub Setup
	{
	# default settings
	if ($component eq "")
		{
		$component = $all_param;
		}
	# some boolean values
	my $want_mmf = $component eq $mmf_param | $component eq $all_param;
	my $want_icl = $component eq $icl_param | $component eq $all_param;
	my $want_misc = $component eq $misc_param | $component eq $all_param;
	my $want_extra = $extras ne "";
	# generate .mbc files and then do bldmake
	open(OUTFILE, ">".$main_mbc) or die "Can't create file: $!";
	print OUTFILE "//\n//Auto-generated - do not edit!!\n//\n\n";
	print OUTFILE "#include \"mmf.mbc\"\n" if ($want_mmf);
	print OUTFILE "#include \"icl.mbc\"\n" if ($want_icl);
	print OUTFILE "#include \"Misc.mbc\"\n" if ($want_misc);
	print OUTFILE "#include \"mmfOpt.mbc\"\n" if ($want_mmf && $want_extra);
	print OUTFILE "#include \"iclOpt.mbc\"\n" if ($want_icl && $want_extra);
	print OUTFILE "#include \"MiscOpt.mbc\"\n" if ($want_misc && $want_extra);
	close OUTFILE;

	if ($want_mmf)
		{
		open(OUTFILE, ">".$phys_mbc) or die "Can't create file: $!";
		print OUTFILE "//\n//Auto-generated - do not edit!!\n//\n\n";
		print OUTFILE "#include \"mmfPhys.mbc\"\n";
		print OUTFILE "#include \"mmfOptPhys.mbc\"\n" if ($want_extra);
		close OUTFILE;
		}
	else
		{
		# only applicable for mmf - otherwise just delete the file
		if (-f $phys_mbc)
			{
			unlink $phys_mbc or die "Couldn't delete $phys_mbc";
			}
		}

	open(OUTFILE, ">".$test_mbc) or die "Can't create file: $!";
	print OUTFILE "//\n//Auto-generated - do not edit!!\n//\n\n";
	print OUTFILE "#include \"AllTests.mbc\"\n";
	print OUTFILE "#include \"TestFrameworkTest.mbc\"\n";
	print OUTFILE "#include \"mmfTest.mbc\"\n" if ($want_mmf);
	print OUTFILE "#include \"mmfNotOptTest.mbc\"\n" if ($want_mmf && !$want_extra);
	print OUTFILE "#include \"iclTest.mbc\"\n" if ($want_icl);
	print OUTFILE "#include \"MiscTest.mbc\"\n" if ($want_misc);
	print OUTFILE "#include \"mmfOptTest.mbc\"\n" if ($want_mmf && $want_extra);
	print OUTFILE "#include \"iclOptTest.mbc\"\n" if ($want_icl && $want_extra);
	print OUTFILE "#include \"MiscOptTest.mbc\"\n" if ($want_misc && $want_extra);
	if ($TargetToolsExists)
		{
		print OUTFILE "#include \"TargetTools.mbc\"\n";
		}
	close OUTFILE;

	open(OUTFILE, ">".$testBuild_mbc) or die "Can't create file: $!";
	print OUTFILE "//\n//Auto-generated - do not edit!!\n//\n\n";
	print OUTFILE "#include \"mmfTestBuild.mbc\"\n" if ($want_mmf);
	print OUTFILE "#include \"iclTestBuild.mbc\"\n" if ($want_icl);
	print OUTFILE "#include \"MiscTestBuild.mbc\"\n" if ($want_misc);
	print OUTFILE "#include \"mmfOptTestBuild.mbc\"\n" if ($want_mmf && $want_extra);
	print OUTFILE "#include \"iclOptTestBuild.mbc\"\n" if ($want_icl && $want_extra);
	print OUTFILE "#include \"MiscOptTestBuild.mbc\"\n" if ($want_misc && $want_extra);
	close OUTFILE;

	# first getmake does bldmake on the main entries
	my $sysComm = $metabld . " " . $main_mbc . " bldmake " . $keep . " bldfiles";
	my $result = system($sysComm);
	if ($result != 0 && $keep eq "")
		{
		print "Error '$sysComm' failed: $result\n";
		exit ($result);
		}
	my $error = $result;
	# second metabld goes through physical
	if (-f $phys_mbc)
		{
		$sysComm = $metabld . " " . $phys_mbc . " bldmake " . $keep . " bldfiles";
		$result = system($sysComm);
		if ($result != 0 && $keep eq "")
			{
			print "Error '$sysComm' failed: $result\n";
			exit ($result);
			}
		$error |= $result;
		}
	# third metabld goes through tests
	$sysComm = $metabld . " " . $test_mbc . " bldmake " . $keep . " bldfiles";
	$result = system($sysComm);
	if ($result != 0 && $keep eq "")
		{
		print "Error '$sysComm' failed: $result\n";
		exit ($result);
		}
	$error |= $result;
	# fourth metabld goes through test build tests - ignore any dups with the above. Does not make any difference
	$sysComm = $metabld . " " . $testBuild_mbc . " bldmake " . $keep . " bldfiles";
	$result = system($sysComm);
	if ($result != 0 && $keep eq "")
		{
		print "Error '$sysComm' failed: $result\n";
		exit ($result);
		}
	$error |= $result;
	if ($error)
		{
  		print "Errors found during run\n";
		exit ($error)
		}
	}

sub Clean
	{
	# first getmake does makefiles on the main entries
	# note we generally ignore any errors on this
	my $sysComm = $metabld . " " . $main_mbc . " abld " . $keep . " makefile";
	system($sysComm);
	if (-f $phys_mbc)
		{
		my $sysComm = $metabld . " " . $phys_mbc . " abld " . $keep . " makefile";
		system($sysComm);
		}
	if ($test eq $test_param)
		{
		# also do for test files
		$sysComm = $metabld . " " . $test_mbc . " abld " . $keep . " makefile";
		system($sysComm);
		$sysComm = $metabld . " " . $testBuild_mbc . " abld " . $keep . " test makefile";
		system($sysComm);
		}
	my $sysComm = $metabld . " " . $main_mbc . " abld " . $keep . " reallyclean";
	system($sysComm);
	if (-f $phys_mbc)
		{
		my $sysComm = $metabld . " " . $phys_mbc . " abld " . $keep . " reallyclean";
		system($sysComm);
		}
	if ($test eq $test_param)
		{
		# also do for test file
		$sysComm = $metabld . " " . $test_mbc . " abld " . $keep . " reallyclean";
		system($sysComm);
		$sysComm = $metabld . " " . $testBuild_mbc . " abld " . $keep . " test reallyclean";
		system($sysComm);
		}

	}

sub Build
	{
	# defaults
	if (!(-f $eka2IdentifyFile)) 
	    {
		@default_targets = @Beech_targets;
	    }

	if (@targets == 0)
		{
		@targets = @default_targets;
		}
	if ($use_gccxml ne "")
		{
		$targets[++$#targets] = $gccxml_target; # append		
		}
	if ((-f $x86gccIdentifyFile)) 
	        {
		$targets[++$#targets] = $x86gcc_target; # append x86gcc target since Build env present.
		}	
	# for each target we need to go through each stage
	# if -k was given, we make all calls and give error at the end
	my $error = 0;
	foreach my $target (@targets)
		{
		# work out which .mbc files to use. Key is as follows
		#    in physical targets and not test, __test.mbc
		#    in physical targets and test, skip - not currently supported
		#    not in physical targets and not test, __main.mbc
		#    not in physical targets and test, __test.mbc
		my $build_test = $test ne ""; # -t has been given
		my $build_testBuild = $test ne "" && $testBuild ne ""; # -t and -b has been given
		my $physical_target = 0;
		foreach my $phys (@physical_targets)
			{
			if ($phys eq $target)
				{
				$physical_target = 1;
				last; # exit loop
				}
			}
		my $mbc;
		if ($physical_target && !$test)
			{
			if (-f $phys_mbc)
				{
				$mbc = $phys_mbc;
				}
			else
				{
				# we don't want to do anything - skip to next target
				next; 
				}
			}
		elsif ($physical_target && $test)
			{
			next; # skip to next target
			}
		elsif (($target eq "gccxml") && $test) # Build GCCXML on the production code only
			{
			next; # skip to next target
			}
		elsif (!$physical_target && !$test)
			{
			$mbc = $main_mbc;
			}		
		elsif ($test ne "")
			{
			if (!$testBuild)
				{
				$mbc = $test_mbc;
				}
			else
				{
				$mbc = $testBuild_mbc;
				}
			}
			
		# if the target is "wins", then check to see if wins is installed and otherwise just do export
		# this is intended to get around Cedar builds not having wins. Really out to check for VC++ too,
		# but not clear how
		my @reqStages = @stages;
		@reqStages = @tbstages if $build_testBuild;
		if ($target eq "wins")
			{
			my $euser_lib = $EPOCROOT . "epoc32\\release\\wins\\udeb\\euser.lib";
			if (! -f $euser_lib)
				{
				print "no WINS installation found - just export\n";
				@reqStages = ( "export" );
				}
			}


			
		# now the main loop
		foreach my $stage (@reqStages)
			{
			# export does not take target as parameter, others do
			my $takesTarget = 0;
			foreach my $stage2 (@stagesTakingTarget)
				{
				if ($stage eq $stage2)
					{
					$takesTarget = 1;
					last; # exit loop
					}
				}
			if (($stage eq "makefile" || $stage eq "test makefile") && $skipMake ne "")
				{
				print "Skip makefile\n"; 
				next; # skip the makefile stage
				}
			# the command - metabld xxx.mbc command
			my $sysComm = $metabld . " " . $mbc .
			    " abld " . $keep . " " . $stage;
			if ($takesTarget)
				{
				$sysComm .= " " . $target;
				}
			my $result = system($sysComm);
			if ($result != 0)
				{
				if ($keep ne "")
					{
					# remember error and keep going
					$error |= $result;
					}
				else
					{
					print "Error '$sysComm' failed: $result\n";
					exit ($result);
					}
				}
			}
		}
	if ($error)
		{
  		print "Errors found during run\n";
		exit ($error)
		}
	}

sub Usage
	{
	# print usage statement and exit
	print <<ENDHERESTRING;
usage:
	mmbuild {-i|-e|-m|-f} [-x] [-k] setup
	mmbuild [-t] [-k] clean
	mmbuild [-t [-b]] [-k] [-g] [-s] build [targets]

mmbuild is intended to do a full build of the Multimedia source area, both for
use in test builds and by team engineers. The effect will be similar to that
achieved for overnight builds, although the mechanism is slight different.

In typical usage, the various forms of the command - as given above - are used
in turn. "setup" equates roughly to "bldmake bldfiles". It is called to setup
the various files prior to the build proper. The parameters are as follows:

-i) Build ICL only
-m) Build MMF only
-c) Build Misc only (other than ICL or MMF)
-f) Build all Multimedia components (default)

-x) Additionally build any optional components - default is non-optional only

"clean" effectively does a reallyclean operation.

-t) Clean test code, *as well as* main code - default is to clean main code only

"build" does a build operation. Optional parameters give the target list - the
default is "wins", "winscw", "arm4" and "mcot". For physical targets, like mcot,
only the SoundDev mmf folder is built.

-t) Build test code (including testframework), *instead of* main code - default
is to build main code only

-t) Build test code (including testframework), *instead of* main code 
    Note: without -b this is test code built via "abld build"

-b) When given with -t, build test code that is built via "abld test build"

-g) Build code additionally for gccxml. Null operation when used with -t. 

-s) Skip the make stage. Use with care - should not be used if any include, 
source list or library list have been changed, and never on the first time 
called. However, it can save time on a subsequent call.

A typical usage sequence would be:
	mmbuild setup
	mmbuild -t clean
	mmbuild build
	mmbuild -t build
	mmbuild -t -b build

which would do a reallyclean and rebuild of the whole Multimedia sub-system,
except the mcot sound drivers on EKA1.

For each command, the -k flag will try and keep going as much as possible -
setup and build, by default, will stop with the first error.
ENDHERESTRING
	exit 1;
	}

