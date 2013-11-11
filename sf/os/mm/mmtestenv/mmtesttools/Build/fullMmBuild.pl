# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

# Full build

use strict;


my $ARGC = @ARGV;
if  (($ARGV[0] eq "-h") ||
     ($ARGV[0] eq "--h") ||
     ($ARGV[0] eq "-help") ||
     ($ARGV[0] eq "--help") ||
     ($ARGV[0] eq "help")
	 )
	{
	print "fullMmBuild\n\n";
	print "Syntax : fullMmBuild [-x] [-k] [-c] [-f] [-eabi] [-y] [-a] [-g] [-s] [-dir=directory] [-a3flog]\n";
	print "-x       include optional elements\n";
	print "-k       keep going\n";
	print "-c       clean build\n";
	print "-f       build A3F and A3F DevSound instead of Legacy DevSound\n";
	print "-l		build Legacy DevSound instead of A3F (ignored if -f given)\n";
	print "-eabi    build rom for armv5 EABI\n";
	print "-arm4    build the extra components for arm4\n"; 
	print "-y       build for ccover (wins only)\n";
	print "-a       call mmbuildrom with MMAutoExec.iby\n";
	print "-g       include build of gccxml target, to allow check of BC etc\n";
	print "-s       skip setup and makefile stages (useful on rebuild following minor changes)\n";
	print "-dir     location of TestTools\\Build directory\n";
	print "-a3flog		enables A3F logging\n";
	print "-iclTestdata	builds ICL test data\n";
	exit 0;
	}


use Getopt::Long;
my %optctl = ();
keys(%optctl)=8;
unless (GetOptions (\%optctl, "x", "c", "k", "f", "eabi" ,"y", "a", "g", "s", "l", "dir=s","arm4","a3flog","iclTestdata")) 
  {exit 1;}

my $ccover_target = "wins";
my $eka2IdentifyFile  = "$ENV{EPOCROOT}epoc32\\release\\winscw\\udeb\\winsgui.dll"; # if file present, is EKA2 build
my $x86gccIdentifyFile  = "$ENV{EPOCROOT}epoc32\\release\\x86gcc\\udeb\\euser.dll"; # if file present, x86 environment present

# main 
	{
	my $auto = "-a " if ($optctl{"a"});
	my $optional = "-x " if ($optctl{"x"});
	my $eabi = "-eabi " if ($optctl{"eabi"});
	my $keep = "-k " if ($optctl{"k"});
	my $target = ""; # default to all
	my $gccxml = "-g " if ($optctl{"g"});
	my $skipSetup = "-s " if ($optctl{"s"});
	my $iclTest = " --iclTestdata " if ($optctl{"iclTestdata"});
	$target = " " . $ccover_target if ($optctl{"y"});

	##################  A3F Section #######################################
	my $a3f_define = 'SYMBIAN_MULTIMEDIA_A3FDEVSOUND';
	my $search_item = '#define\s+'.$a3f_define;  # looking for this
	my $hrh_filename = "$ENV{EPOCROOT}epoc32\\include\\variant\\Symbian_OS.hrh"; # assign hrh file and location
	my $found_search_item = 0; #false
	# print "HRH filename is : $hrh_filename\n";
	
 	if ($optctl{"f"})
	 	{
	 	# This displays the content of the hash table
	 	# use it to confirm the "f" entry has been added.
	 	#
	 	# my $key;  # holds the command line options
	 	# my $value; # redundant for hash entry as only a key is used.
		# while (($key, $value) = each(%optctl))
		#	{
		# 	print $key.", ".$value."\n";
		#	}

		# opens up the hrh file for reading
		open(FILEHANDLE, "<", $hrh_filename) or die "Unable to open $hrh_filename for reading. $!\n";

		my @file_data=<FILEHANDLE>; # read all file data into array

		#check the contents of the array (i.e. the contents of the file)
		for(@file_data)
			{
			# print "$_\n";  #display the line in the file
			
			# search line for string of interest: =~ is equals, !~ is not equal
			if(($_ =~ m/$search_item/i))
				{
				$found_search_item = 1;  #found the macro 
				# print "Search item found: $_\n";    #display the line containing the search item
				}
			else
				{
				# Search item not found, do nothing
				}
			} #for loop 

		close(FILEHANDLE) or die "Can't close file $hrh_filename.\n";  # readonly file
		
		if(!$found_search_item)  #search item has not been found
			{
			# print warning
			print "Modifying file $hrh_filename to enable A3F as -f flag has been used.\n";
			
			# open file for append
			open(FILEHANDLE, ">>", $hrh_filename) or die "Unable to open $hrh_filename for appending. $!\n";
			print FILEHANDLE "\n#define $a3f_define\n"; # append macro to the file
			close(FILEHANDLE) or die "Can't close appended file $hrh_filename.\n";  # readonly file
			}
			
	 	# Now remove the "f" from the hash table so there is no chance of it being passed
	 	# to mmbuildrom.
	 	######## NOTE removal of flag from Hash table ############
	 	delete($optctl{f});   
	 	
	 	# This displays the content of the hash table
	 	# use it to confirm the "f" entry has been removed.
	 	#
		# while (($key, $value) = each(%optctl))
		#	{
		#	print $key.", ".$value."\n";
		#	}
	 	} # if $optctl{"f"}
 	elsif ($optctl{"l"})
 		{
 		 ########### Remove the macro from the hrh as "-l" but not "-f" is specified  ###############
 		
 		$found_search_item = 0 ; # false
		# opens up the original hrh file for reading
		open(FILEHANDLE, "<", $hrh_filename) or die "Unable to open $hrh_filename for reading. $!\n";

		# temporary file
		my $temp_hrh_filename = "$ENV{EPOCROOT}epoc32\\include\\variant\\temp_Symbian_OS.hrh";
		# open temporary file for writing
		open(TEMPFILEHANDLE, ">", $temp_hrh_filename) or die "can't open tempory file $temp_hrh_filename. $!\n";
		
		my @file_data=<FILEHANDLE>; # read all original hrh file data into array

		#check the contents of the array (i.e. the contents of the file)
		for(@file_data)
			{
			# print "$_\n";  #display the line in the file
			
			# search line for string of interest: =~ is equals, !~ is not equal
			if(($_ =~ m/(.*)$search_item(.*)/i))
				{
				#display a warning message (only do so once)
				if( !$found_search_item )
					{
					print "Modifying file $hrh_filename to disable A3F as -l has been given.\n";
					}
				
				# write line excluding the #define bit to temp file. (to preserve any other bits in this line. e.g.: comment delimiters)
				print TEMPFILEHANDLE $1." ".$2."\n";
				$found_search_item = 1 ; # true
				# Do not write line to the file as we want to remove it.
				}
			else
				{
				print TEMPFILEHANDLE $_;  # copy unchanged line to temporary file
				}
			} #for loop 

		close(FILEHANDLE) or die "Can't close file $hrh_filename.$!\n";  # readonly file
		close(TEMPFILEHANDLE) or die "Can't close file $temp_hrh_filename. $!\n";  # temp file
		if($found_search_item)
			{
			my $orig_hrh_filename = "$ENV{EPOCROOT}epoc32\\include\\variant\\orig.hrh";
			rename($hrh_filename, $orig_hrh_filename) or die "can't rename $hrh_filename to $orig_hrh_filename: $!\n";
			rename($temp_hrh_filename, $hrh_filename)  or die "can't rename $temp_hrh_filename to $hrh_filename: $!\n";
			unlink $orig_hrh_filename;  # delete the original as it is no longer needed
			}
		else
			{
			unlink $temp_hrh_filename;  # no changes to be made so delete the temporary file
			}
		
 		}  # main else for A3F
 	
	##################  End A3F Section ####################################

	##################  A3F Logging Section ####################################

	my $search_item = "SYMBIAN_MULTIMEDIA_A3F_ENABLE_LOGGING";  # looking for this	
	my $hrh_filename = "$ENV{EPOCROOT}epoc32\\include\\variant\\Symbian_OS.hrh"; # assign hrh file and location
	my $found_search_item = 0; #false
	
 	if ($optctl{"a3flog"})
	 	{
		# opens up the hrh file for reading
		open(FILEHANDLE, "<", $hrh_filename) or die "Unable to open $hrh_filename for reading. $!\n";

		my @file_data=<FILEHANDLE>; # read all file data into array

		#check the contents of the array (i.e. the contents of the file)
		for(@file_data)
			{
			# search line for string of interest: =~ is equals, !~ is not equal
			if(($_ =~ m/$search_item/i))
				{
				$found_search_item = 1;  #found the macro 
				}
			else
				{
				# Search item not found, do nothing
				}
			} #for loop 

		close(FILEHANDLE) or die "Can't close file $hrh_filename.\n";  # readonly file
		
		if(!$found_search_item)  #search item has not been found
			{
			# open file for append
			open(FILEHANDLE, ">>", $hrh_filename) or die "Unable to open $hrh_filename for appending. $!\n";
			print FILEHANDLE "\n#define $search_item\n"; # append macro to the file
			close(FILEHANDLE) or die "Can't close appended file $hrh_filename.\n";  # readonly file
			}
			
	 	# Now remove the "log" from the hash table so there is no chance of it being passed
	 	# to anything called in this file.
	 	######## NOTE removal of flag from Hash table ############
	 	delete($optctl{log});   		
	 	} # if $optctl{"log"}
 	else
 		{
 		 ########### Remove the macro from the hrh as "-a3flog" is not specified  ###############
 		
 		$found_search_item = 0 ; # false
		# opens up the original hrh file for reading
		open(FILEHANDLE, "<", $hrh_filename) or die "Unable to open $hrh_filename for reading. $!\n";

		# temporary file
		my $temp_hrh_filename = "$ENV{EPOCROOT}epoc32\\include\\variant\\temp_Symbian_OS.hrh";
		# open temporary file for writing
		open(TEMPFILEHANDLE, ">", $temp_hrh_filename) or die "can't open tempory file $temp_hrh_filename. $!\n";
		
		my @file_data=<FILEHANDLE>; # read all original hrh file data into array

		#check the contents of the array (i.e. the contents of the file)
		for(@file_data)
			{
			# search line for string of interest: =~ is equals, !~ is not equal
			if(($_ =~ m/$search_item/i))
				{
				#display a warning message
				print "Removing $_ from file $hrh_filename as -a3flog flag has not been used to indicate an A3F logging build is required.\n";
				$found_search_item = 1 ; # true
				# Do not write line to the file as we want to remove it.
				}
			else
				{
				# Ensure the last newline after the macro is not written to the file.
				# the last new line was added by the macro append in the previous section.
				# The asssumes the macro is the last entry in the file. If it is not then 
				# the if statement should be removed and the print statement left on its own
				# to copy the last line to file.
				if(!$found_search_item)  
					{
					print TEMPFILEHANDLE $_;  # copy unchanged line to temporary file
					}
				}
			} #for loop 

		close(FILEHANDLE) or die "Can't close file $hrh_filename.$!\n";  # readonly file
		close(TEMPFILEHANDLE) or die "Can't close file $temp_hrh_filename. $!\n";  # temp file
		if($found_search_item)
			{
			my $orig_hrh_filename = "$ENV{EPOCROOT}epoc32\\include\\variant\\orig.hrh";
			rename($hrh_filename, $orig_hrh_filename) or die "can't rename $hrh_filename to $orig_hrh_filename: $!\n";
			rename($temp_hrh_filename, $hrh_filename)  or die "can't rename $temp_hrh_filename to $hrh_filename: $!\n";
			unlink $orig_hrh_filename;  # delete the original as it is no longer needed
			}
		else
			{
			unlink $temp_hrh_filename;  # no changes to be made so delete the temporary file
			}
		
 		}  # main else for A3F
 		
	##################  End A3F Logging Section ####################################
	
	if($optctl{"dir"})
		{
		my $dir = $optctl{"dir"};
		chdir $dir or die "Can't cd $dir - $!";
		}

	my $targetToolsDir = "..\\..\\..\\..\\..\\TargetTools\\Build";
	my $testToolsDir = "..\\..\\..\\..\\..\\os\\mm\\mmtestenv\\mmtesttools\\Build";
	my $targetToolsExists = 0;
	if (-d $targetToolsDir)
		{
		$targetToolsExists = 1;
		print "TargetTools directory exists: $targetToolsDir\n"
		}

	# DEF113945:DRM tests fail due to the change from DEF112928
	my $rtaUtilsDir = "..\\..\\..\\..\\..\\Rtautils\\Build";
	my $rtaUtilsExists = 0;
	if (-d $rtaUtilsDir)
		{
		$rtaUtilsExists = 1;
		print "RtaUtils directory exists: $rtaUtilsDir\n";
		}

	# DEF113945:DRM tests fail due to the change from DEF112928
	my $testExecuteDir = "..\\..\\..\\..\\..\\Testexecute\\Build";
	my $testExecuteExists = 0;
	if (-d $testExecuteDir)
		{
		$testExecuteExists = 1;
		print "TestExecute directory exists: $testExecuteDir\n";
		}

	my $command;

 	# mmbuild [-x] [-k] setup
 	$command = "mmbuild " . $optional . $keep . "setup" . $iclTest;
 	if ($skipSetup ne "")
 		{
 		print "Skipping setup...\n";
 		}
 	else
 		{
 		(system ($command)==0 )or die "Couldn't execute $command";
 		}
 	
 	if ($optctl{"c"})
 		{
 		# mmbuild -t clean (throw away result - so we ignore any no-export errors)
 		$command = "mmbuild -t clean >\$NULL 2>&1";
 		print $command, "\n";
 		(system ($command)==0 )or die "Couldn't execute $command";
 		}
 
 	# mmbuild [-k] [-g] build
 	$command = "mmbuild ". $keep. $gccxml. $skipSetup. "build" . $target . $iclTest;
 	(system ($command)==0 )or die "Couldn't execute $command";
 
 	# mmbuild -t [-k] build
 	$command = "mmbuild -t ". $keep. $skipSetup. "build" . $target . $iclTest;
 	(system ($command)==0 )or die "Couldn't execute $command";
 
 	# mmbuild -t -b [-k] build
 	$command = "mmbuild -t -b ". $keep. $skipSetup. "build" . $target . $iclTest;
 	(system ($command)==0 )or die "Couldn't execute $command";
 
	if (!$optctl{"y"})
 		{
		# DEF113945:DRM tests fail due to the change from DEF112928
		if ($rtaUtilsExists)
			{
			$command = "testExtras_Rtautils";
			chdir $rtaUtilsDir or die "Can't cd $rtaUtilsDir - $!";
			(system ($command)==0 )or die "Couldn't execute $command";
			chdir $testToolsDir or die "Can't cd $testToolsDir - $!";
			}

		# DEF113945:DRM tests fail due to the change from DEF112928
		if($testExecuteExists)
			{
			$command = "testExtras_Testexecute";
			chdir $testExecuteDir or die "Can't cd $testExecuteDir - $!";
			(system ($command)==0 )or die "Couldn't execute $command";
			chdir $testToolsDir or die "Can't cd $testToolsDir - $!";
			}

		# mmBuildRom [-x]
		$command = "mmBuildRom -t -d " .$auto . $optional .$eabi;
		print $command, "\n";
		(system ($command)==0 )or die "Couldn't execute $command";
		}

	}
