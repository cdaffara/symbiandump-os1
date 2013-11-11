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
# mmrombuild.pl
# 
#

use strict;
use warnings;
use Env qw(EPOCROOT);
use File::Copy;

# literals
my $auto_param =  "-a";
my $dp_param   =  "-d";
my $wdp_param  =  "-w";
my $all_param  =  "-f";
my $help_param =  "-h";
my $icl_param  =  "-i";
my $plus_icl_param = "+i";
my $keep_param =  "-k";
my $mmf_param  =  "-m";
my $nand_param =  "-n";
my $addnl_param = "-p";
my $test_param =  "-t";
my $extra_param = "-x";
my $gce_param  =  "-g";
my $eabi_param =  "-eabi";
my $h2_param =    "-h2";
my $h4_param =    "-h4";
my $h6_param =    "-h6";
my $lab_param =    "-lab";
my $lbk_param =   "-lbk";
my $x86_param =   "-x86pc";
my $ne_param =    "-ne";
my $smp_param =    "-smp";
my $uni_param =    "-uni";

my $KRomDir     = "epoc32\\rom";
my $KLubbockImg = "la_001.techview.IMG";
my $KH2Img =      "om_001.techview.IMG";
my $KH4Img =      "h4hrp_001.techview.IMG";
my $KH4RofsImg =      "h4hrp_001.techview.rofs.IMG";
my $KH6Img =      "34xx_sdp_001.techview.IMG";
my $KH6NandImg =	"34xx_sdp_001.techview.nand.IMG";
my $KH6NandRofsImg =	"34xx_sdp_001.techview.nand.rofs.IMG";
my $KLabImg =      "lab_001.techview.IMG";
my $KLabNandImg =	"lab_001.techview.nand.IMG";
my $KLabNandRofsImg =	"lab_001.techview.nand.rofs.IMG";
my $KH4NandImg =  "h4hrp_001.techview.nand.img";
my $KH4NandRofsImg = "h4hrp_001.techview.nand.rofs.img";
my $KH6NandBootLoader =	"h6.nandloader.reltest.img";
my $KLabNandBootLoader =	"lab_001.nandloader.reltest.img";
my $KH4NandBootLoader = "h4.nandloader.reltest.img";
my $KMmcBootLoader = "h4.mmcloader.reltest.img";
my $CoreImg =     "core.img";
my $Rofs1Img =    "rofs1.img";
my $KBinName	= 'sys$rom.bin'; # be careful about '$'
my $KZipName	= 'sys$rom.zip'; # be careful about '$'
my $KX86Img     =  "x86pc_001.techview.IMG";
my $KNEImg     =  "NE1_TB_001.techview.IMG";

my $extras = "";
my $gce = "";
my $test = "";
my $component = "";
my $keep = "";
my $auto = "";
my $eabi = "";
my $addnl_ibys = "";
my $dp = "";
my $wdp = "";
my $nand = "";
my $img = "";
my $target = ""; # default target is h6 but start as blank so we can distinguish multiple target requests
my $default_target = "h6";
my $arm = "";
my $smp = "";
my $uni = "";
my $wrong_ne_param = 0;
my $include_icl_tests = "";

my $eka2IdentifyFile  = "$ENV{EPOCROOT}epoc32\\release\\winscw\\udeb\\winsgui.dll"; # if file present, is EKA2 build
my $PlatSecIdentifyFile  = "$ENV{EPOCROOT}epoc32\\release\\armv5\\udeb\\mmfdevsoundproxy.dll"; # if file present, is a build with platsec
my $StreamingCafIdentifyFile = "$ENV{EPOCROOT}epoc32\\release\\armv5\\udeb\\sraserver.exe";

my $OptionalFile = "";
# if any of these .iby files are present, then we'll add them to the ROM.
my @OptionalFiles = ("$ENV{EPOCROOT}epoc32\\rom\\include\\XviDPU.iby",
			       "$ENV{EPOCROOT}epoc32\\rom\\include\\XviDhw.iby",
			       "$ENV{EPOCROOT}epoc32\\rom\\include\\oggvorbiscodec.iby",
			       "$ENV{EPOCROOT}epoc32\\rom\\include\\omxoggvorbiscodec.iby",
			       "$ENV{EPOCROOT}epoc32\\rom\\include\\pvopt.iby",
			       "$ENV{EPOCROOT}epoc32\\rom\\include\\pvtestsopt.iby",
			       "$ENV{EPOCROOT}epoc32\\rom\\include\\RtaUtils.iby",
			       "$ENV{EPOCROOT}epoc32\\rom\\include\\TestExecute.iby",
				   "$ENV{EPOCROOT}epoc32\\rom\\include\\streamingcaf.iby");

my $OggVorbisCodecPresent = 0;

my $targetToolsDir = "..\\..\\..\\..\\..\\TargetTools\\Build";
my $targetToolsExists = 0;
if (-d $targetToolsDir)
	{
	$targetToolsExists = 1;
	print "TargetTools directory exists: $targetToolsDir\n"
	}

# main:
	{
	# Process command-line

	&ReadArgs();
	
	&ProcessArgs();
	
	&BuildRom();
	}

#
# Subroutines
#

sub ReadArgs()
	{
	while (@ARGV)
		{
		my $param = $ARGV[0]; shift @ARGV; # grab first param and shift along
		
		if ($param eq "") # ignore blank parameters
			{
			next;
			}

		if ($param eq $icl_param || 
		    $param eq $mmf_param || $param eq $all_param)
			{
			$component = $param;
			}
		elsif ($param eq $plus_icl_param)
			{
			$include_icl_tests = $param;
			}
		elsif ($param eq $test_param)
			{
			$test = $param;
			}
		elsif ($param eq $extra_param)
			{
			$extras = $param;
			}
		elsif ($param eq $gce_param)
			{
			$gce = $param;
			}
		elsif ($param eq $auto_param)
			{
			$auto = $param;
			}
		elsif ($param eq $eabi_param)
			{
			$eabi = $param;
			}
		elsif ($param eq $dp_param)
			{
			$dp = $param;
			$nand = $param; # demand paging implies nand
			}
		elsif ($param eq $wdp_param)
			{
			$wdp = $param;
			$dp = $param; # writable data paging implies standard demand paging
			}
		elsif ($param eq $nand_param)
			{
			$nand = $param;
			}
		elsif ($param eq $addnl_param)
			{
			$addnl_ibys .= " " . $ARGV[0]; shift @ARGV; # grab next param and shift along
			}
		elsif ($param eq $h2_param)
			{
			&Usage() if ($target ne "");
			$target = "h2";
			$eabi = $eabi_param; # H2 always EABI
			}
 		elsif ($param eq $h4_param)
 			{
			&Usage() if ($target ne "");
			$target = "h4";
 			$eabi = $eabi_param; # H4 always EABI
 			}
 		elsif ($param eq $h6_param)
			{
			&Usage() if ($target ne "");
			$target = "h6";
			$eabi = $eabi_param; # H6 always EABI
			}
		elsif ($param eq $lab_param)
			{
			&Usage() if ($target ne "");
			$target = "lab";
			$eabi = $eabi_param; # Lab always EABI
			}
		elsif ($param eq $lbk_param)
			{
			&Usage() if ($target ne "");
			$target = "lbk";
			}
		elsif ($param eq $x86_param)
			{
			&Usage() if ($target ne "");
			&Usage() if (($eabi ne "") || ($nand ne ""));
			$target = "x86";
			}			
		elsif ($param eq $ne_param)
			{
			&Usage() if ($target ne "");
			&Usage() if ($nand ne "");
			$target = "naviengine";
			$eabi = $eabi_param; # NaviEngine always EABI
			}
		elsif ($param eq $smp_param)
			{
			# Only NaviEngine can be configured to build SMP enabled image
			&Usage() if ($target ne "naviengine");
			$smp = "smp";
			}						
		elsif ($param eq $uni_param)
			{
			# '-uni'-flag valid only for NaviEngine
			&Usage() if ($target ne "naviengine");
			$uni = "uni";
			}									
		elsif ($param eq $help_param)
			{
			&Usage();
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
	
	$target = $default_target if ($target eq "");
	
	}
	
sub ProcessArgs()
	{
	if ($target eq "h2")
		{
		$img = $KH2Img;
		}
	elsif ($target eq "lkb")
		{
		$img = $KLubbockImg;
		}
	elsif ($target eq "h4")
		{
		$img = $KH4Img if ($nand eq "");
		$img = $KH4NandImg if ($nand ne "");
		}
	elsif ($target eq "h6")
		{
		$img = $KH6Img;
		}
	elsif ($target eq "lab")
		{
		$img = $KLabImg;
		}
	elsif ($target eq "x86")
		{
		$img = $KX86Img;
		}	
	elsif ($target eq "naviengine")
		{
		$img = $KNEImg;
		}			
	else
		{
		die "Internal error: unknown image type";
		}	
	if($target eq "naviengine" && $smp eq "" && $uni eq "" )
		{
		$wrong_ne_param = 1;
		&Usage();
		}
	if($target eq "h2" || $target eq "lkb" || $target eq "h4" || $target eq "naviengine" || $target eq "h6" || $target eq "lab")
		{
		# procedure to build arm target different so identify a ARM based target
		$arm ="arm";
		}
	}
	
sub BuildRom()
	{
	my $command;
	# note from build 4713 -D_STARTUPMODE5 is replaced with -D_SSMSTARTUPMODE=5. Don't mix
	if($target ne "naviengine")
		{
		$command = "tools\\buildrom -D__NOWATCHER__ -D_DEBUG -D_SSMSTARTUPMODE=5";
		}
	else
		{
		#Currently startupmode flag is not used for NaviEngine.
		$command = "tools\\buildrom -D_DEBUG";
		$command .= " -DSYMBIAN_MDASOUNDADAPTER_FORCE_STEREO";
		}		
	if($target ne "x86" && $target ne "naviengine")
		{
		$command .= " -D_SERIAL_DOWNLOAD";
		}
	if($smp ne "")
		{
		$command .= " -DSMP";
		}
	if($uni ne "")
		{
		#Use Flexible Memory Model for UniCore
		#Until base have fixed the issue related to this keep commented out.
		#$command .= " -DWITH_FLEXIBLE_MM";
		}
	
	$command .= " -D_NAND2" if ($nand ne "" && $arm ne "");
	$command .= " -DUSE_24BPP_DISPLAY_VARIANT" if ($target eq "h6" || "lab");
	if($wdp ne "")
		{
		$command .= " -DWITH_FLEXIBLE_MM -DUSE_SDIO_SD_MMC -D_INTERNAL_MMC -DUSE_DATA_PAGING"
		}
	$command .= " -DSYMBIAN_BASE_USE_GCE -DSYMBIAN_GRAPHICS_USE_GCE" if ($gce ne "");

	# Needed to prevent Xml.iby errors if these flags are added to techview.oby for example, then remove them here
	$command .= " -DSYMBIAN_EXCLUDE_LIBXML2 -DSYMBIAN_EXCLUDE_LIBXML2_SAX_CPARSER_PLUGIN -DSYMBIAN_EXCLUDE_LIBXML2_DOM_XPATH_API";
	
	# select instruction set type, only applicable for ARM based platforms
	#The below flags are currently not used for NaviEngine
	if($arm ne "" && $target ne "naviengine")
	{
	if ($eabi ne "" || -f $eka2IdentifyFile)
		{
		$command .= " -DRVCT -D_EABI=ARMV5";
		}
	else
		{
		$command .= " -D_THUMB";
		}
	}	

	# Use the default FeatureManager database, not yet supported for x86
	if($arm ne "")
		{
		$command .= " -fm=$ENV{EPOCROOT}epoc32\\rom\\include\\featuredatabase.xml"; 
		}
	else
		{
		$command .= ' -nofm'; #-fm option not supported as of 0708.
		}
	
	# select development board (now we build h4 by default)
	if ($target eq "lbk")
		{
		$command .= " lubbock";
		}
 	elsif ($target eq "h2")
 		{
 		$command .= " h2";
 		}
	 elsif ($target eq "x86")
 		{
 		$command .= " x86pc";
 		}	
 	elsif ($target eq "naviengine")
 		{
 		$command .= " naviengine";
 		}	
 	elsif ($target eq "h6")
 		{
 		$command .= " pagedrom" if ($dp ne "");
 		$command .= " 34xx_sdp";
 		}
	elsif ($target eq "lab")
 		{
 		$command .= " pagedrom" if ($dp ne "");
 		$command .= " lab";
 		}
 	elsif ($target eq "h4")
 		{
 		$command .= " pagedrom" if ($dp ne "");
	 	$command .= " h4hrp";
 		}
	else
		{
		die "target does not exist!!"
		}
		
	# devkit.oby not included for NaviEngine currently	
	if($target ne "naviengine")
		{
		$command .= " devkit";
		}
	$command .= " techview";
	
	$command .= " misc.iby";
	
	# use platform security, if appropriate
	if (-f $PlatSecIdentifyFile)
		{
		$command .= " platsec";
		}

	if ($extras ne "")
		{
		$command .= " mmfOpt.iby" ; # mmfOpt.iby should include optional stuff
		$command .= " iclOpt.iby" ; # iclOpt.iby should include optional stuff
		$command .= " miscOpt.iby" ; # miscOpt.iby should include optional stuff
		
		if ($test ne "")
			{
			$command .= " mmfOptTest.iby" ; # mmfOptTest.iby should include tests of optional stuff
			$command .= " iclOptTest.iby" if $include_icl_tests ne ""; # iclOptTest.iby should include tests of optional stuff
			$command .= " miscOptTests.iby"; # miscOptTests.iby should include tests of optional stuff
			}
			
		# Open source code delivered separately to Symbian OS
		foreach $OptionalFile (@OptionalFiles)
			{
			if (-f $OptionalFile)
				{
				$OptionalFile =~ s/^.*\\//; # get only the filename
				
				if ($OptionalFile eq "oggvorbiscodec.iby" 
					|| $OptionalFile eq "omxoggvorbiscodec.iby")
					{
					$OggVorbisCodecPresent = 1;
					}
				
				$command .= " ";
				$command .= $OptionalFile;
				}
			else
				{
				print "**** WARNING: $OptionalFile not added to ROM ****\n"
				}
			}

		# Add oggvorbis to ROM only if oggvorbiscodec and omxoggvorbiscodec exist to prevent ROM build errors
		if ($OggVorbisCodecPresent)
			{
			$command .= " oggvorbis.iby";
			}
		}
	
	if ($gce ne "")
		{
		$command .= " videorenderer.iby";
		
		if ($test ne "")
			{
			$command .= " tsu_mmf_videorenderer.iby";
			}
		}
	
	if ($test ne "")
		{
		if ($component eq $icl_param)
			{
			$command .= " iclIntTests.iby";
			$command .= " iclUnitTests.iby";
			}
		elsif ($component eq $mmf_param)
			{
			$command .= " mmfUnitTests.iby";
			if ($extras eq "")
				{
				$command .= " mmfNotOptTest.iby";
				}
			}
		else
			{ 
			# -f or default
			$command .= " MiscUnitTests.iby";
			$command .= " MiscIntTests.iby";
			$command .= " MMFUnitTests.iby";
			$command .= " MMFIntTests.iby";
			$command .= " ICLUnitTests.iby" if $include_icl_tests ne "";
			$command .= " ICLIntTests.iby" if $include_icl_tests ne "";
			if ($targetToolsExists)
				{
				$command .= " ExtraMultimediaTests.iby ecam.iby";
				$command .= " timageviewer.iby timagedisplay.iby tphotoeditor.iby timagetran.iby" if $include_icl_tests ne "";
				}
			if ($extras eq "")
				{
				$command .= " mmfNotOptTest.iby";
				}
			}
		}
	if ($targetToolsExists)
		{
		if ($auto ne "") 
			{
			$command .= " MMAutoExec.iby";
			}
		}

	#pvdecoderaac.iby is somehow not getting included into naviengine roms, the way it is in case of H4 and the rest.
	if($target eq "naviengine")
		{
		$command .= " pvdecoderaac.iby";
		}
	
	$command .= $addnl_ibys;
	
	if ($gce eq "" and $dp eq "")
		{
		$command .= " surfacemanager_ref.iby" if ($target eq "h6" || "lab" || "naviengine");
		}
		
	print $command, "\n";

	die "EPOCROOT is not set" unless ($EPOCROOT ne "");
	my $romDir = $EPOCROOT . $KRomDir;
	die "$romDir does not exist" unless -d $romDir;
	chdir ($romDir) or die "Could not cd to $romDir - $!";

	# delete the expected image, so clear whether worked or not
	if (-f $img)
		{
		my $count = unlink $img;
		die "Could not delete $KH2Img" unless $count == 1;
		}

	# delete any existing zip, otherwise we could end up with multiple ROMs in our zip file
	if ($arm ne "" && -f $KZipName)
		{
		my $count = unlink $KZipName;
		die "Could not delete $KZipName" unless $count == 1;
		}
		
	my $result = system($command);
	if ($result != 0)
		{
		print "Command failed ($result)\n";
		exit 1;
		}
	if($arm ne "")
		{
		if ($nand eq "" and $wdp eq "") # for nand etc you need to use the rofs loder, otherwise conver to sys$rom.zip
			{
			my $zipCommand = "zip " . $KZipName . " " . $img;
		
			print $zipCommand, "\n";
			$result = system($zipCommand);
			if ($result != 0)
				{
				print "Command failed ($result)\n";
				exit 1;
				}
			}	
		elsif ($nand ne "" or $wdp ne "")
			{	
			if ($target eq "h6")
				{
				if ($nand ne "")
					{
					copy($KH6NandBootLoader,$KBinName) or die "Copy failed: $!";
					copy($KH6NandImg,$CoreImg) or die "Copy failed: $!";
					copy($KH6NandRofsImg,$Rofs1Img) or die "Copy failed: $!";
					}
				else # if ($wdp ne "") - not supported yet but ready for future releases
					{
					print "\nWDP is not supported yet on H6\n";
					exit 1;
#					copy($KMmcBootLoader,$KBinName) or die "Copy failed: $!";
#					copy($KH6Img,$CoreImg) or die "Copy failed: $!";
#					copy($KH6RofsImg,$Rofs1Img) or die "Copy failed: $!";
					}
				}
			elsif ($target eq "lab")
				{
				if ($nand ne "")
					{
					copy($KLabNandImg,$CoreImg) or die "Copy failed: $!";
					copy($KLabNandRofsImg,$Rofs1Img) or die "Copy failed: $!";
#					copy($KLabNandBootLoader,$KBinName) or die "Copy failed: $!"; Lab Nandloader is still not part of the CBR release hence commenting this command for time being
					}
				else # if ($wdp ne "") - not supported yet but ready for future releases
					{
					print "\nWDP is not supported yet on Labrador\n";
					exit 1;
#					copy($KMmcBootLoader,$KBinName) or die "Copy failed: $!";
#					copy($KLabImg,$CoreImg) or die "Copy failed: $!";
#					copy($KLabRofsImg,$Rofs1Img) or die "Copy failed: $!";
					}		
				}				
			else
				{
				if ($nand ne "")
					{
					copy($KH4NandBootLoader,$KBinName) or die "Copy failed: $!";
					copy($KH4NandImg,$CoreImg) or die "Copy failed: $!";
					copy($KH4NandRofsImg,$Rofs1Img) or die "Copy failed: $!";
					}
				else # if ($wdp ne "")
					{
					copy($KMmcBootLoader,$KBinName) or die "Copy failed: $!";
					copy($KH4Img,$CoreImg) or die "Copy failed: $!";
					copy($KH4RofsImg,$Rofs1Img) or die "Copy failed: $!";
					}
				}
			print "Generated " . $CoreImg . ", " . $Rofs1Img . " and " . $KBinName . "\n";
			$command = "dir " . $CoreImg . " " . $Rofs1Img . " " . $KBinName;
			system($command);
			}
		}
	}	

sub Usage
	{
	# print usage statement and exit
	print <<ENDHERESTRING;
usage:
 	mmrombuild {-i|-m|-f} [+i] [-d] [-x] [-t] [-a] [-eabi] [-h2] [-h4] [-h6] [-lab] [-lbk] [-p <ibyfile>] [-ne <-uni/-smp>]

mmbuildrom will build a lubbock / H2 / H4 / NaviEngine / Labrador ROM for multimedia testing

It will always build a standard rom for the given platform, the various flags
support what test and optional software is installed in addition.
The parameters are as follows:

-d) Build demand-paging NAND rom - instead of standard NOR build. Implies -n
-w) Build writable data paging NAND rom - implies -d but not -n!
-i) Build ICL test (unit and integration) software only, if -t is set
-m) Build MMF unit test software only, if -t is set
+i) ICL tests are now disabled unless this parameter is also given
-n) Build NAND rom - otherwise builds NOR
-f) Add all tests software, it -t is set (default)
-t) Add test software
-x) Add optional Multimedia modules
-g) Add optional Graphics Composition Engine enabled components
-eabi) Builds rom for armv5
-h2) Builds for H2 board
-h4) Builds for H4 board
-x86pc) Builds rom for X86 Platform
-lab) Builds for labrador board
-ne) Builds rom for NaviEngine board(NE (uni/smp) does not support demand paging yet)
-smp) Builds an SMP-enabled Rom Image(Valid only if -ne flag is used)
-uni) Builds UniCore Rom Image(Valid only if -ne flag is used)
-lbk) Builds for lubbock board (default is h2)
-p <ibyfile>) Append .iby file to list

ENDHERESTRING
	if ($targetToolsExists)
		{
		print "-a) Add MMAutoExec iby file (internal Symbian use only)"
		}
	if($wrong_ne_param)
		{
		print "\n \n*Did you forget to specify whether you want a UniCore image or an SMP image?*\n\n";
		}
	exit 1;
	}
	
