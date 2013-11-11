#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#
# CheckDistributionFiles.pl
# Aug 3rd 2009. Written for use during the SOS production build.
# Parameter 1 : Directory to search
# The script will search through the directory provided. (This is \clean-src in the current SOS production build.)
# It will produce an advisory note for any missing distribution.policy.s60 files.
# It will produce a migration note for any unnecessary distribution.policy.s60 files.
#

use File::Find;

my $dirToSearch = shift;

# Iterate through all sub-directories checking the distribution.policy.s60 status.
finddepth(sub{if ($_ !~ /^\.\.?$/ ) {$entry = Cwd::cwd."\\"."$_"; if (-d $entry) {checkDir($entry)}}}, $dirToSearch);

# A directory is considered empty for this purpose if it contains no files (it may contain other directories)
sub dirIsEmpty($)
{
    my $currentDir = shift;
    opendir DIR, $currentDir;
	
    while(my $entry = readdir DIR) {    
        next if(($entry =~ /^\.\.?$/) || (-d "$currentDir"."\\"."$entry")); # ignore all directory entries (including . and ..)
        closedir DIR;
        return 0; # first file has been found
    }
    closedir DIR;
    return 1; # no files found
}

# Checks if a directory contains files other than an S60 distribution policy file.
sub onlyPolicyFileExists($)
{
    my $currentDir = shift;
    my $file = "distribution.policy.s60";
 
    opendir DIR, $currentDir;
    while(my $entry = readdir DIR) {
	 
	    # ignore directories (including . and ..) and S60 distribution file
        next if(($entry =~ /^\.\.?$/) || (-d "$currentDir"."\\"."$entry") || ($entry =~ /^$file$/i));  
        closedir DIR;
        return 0; # first file (except distribution.policy.s60) has been found
    }
    closedir DIR;
    return 1; # no extra files found 
}

# checkDir
# Check the distribution.policy.s60 status of the directory passed as parameter.
# 4 cases are checked for
# Case 1: Empty directory , no policy file - this is fine
# Case 2: The directory is empty except for the distribution.policy.s60 file - produce migration note
# Case 3: Non-empty directory with policy file in place - this is fine
# Case 4: Directory is not empty and does not contain a distribution.policy.s60 file - produce an advisory note
#
sub checkDir($) {

	my $currentDir = shift;
	my $file = "distribution.policy.s60";
 
	# An empty directory should not have a policy file.
	if (dirIsEmpty($currentDir) )
		{ # Case 1: Empty directory , no policy file - this is fine
		  # print "\nok, $currentDir has no files so no distribution.policy.s60 file is necessary \n";
		}

	else # Directory is not empty
		{
		# Check if a policy file exists
		if (-e "$currentDir"."\\"."$file")
			{
			# Check that it is necessary i.e. that there are other files in the directory
			if (onlyPolicyFileExists($currentDir))
				{
				# Case 2: The directory is empty except for the distribution.policy.s60 file - produce migration note
				print "MIGRATION_NOTE: Unnecessary distribution.policy.s60 file in $currentDir\n";
				}
			else
				{
				# Case 3: Non-empty directory with policy file in place.
				# print "\nok, distribution.policy.s60 and other files exist in $currentDir\n";
				}

			}
		else # no policy file exists in a non-empty directory
			{
			#  Case 4: Directory is not empty and does not contain a distribution.policy.s60 file - produce an advisory note
			print "ADVISORY NOTE: Missing distribution.policy.s60 file in $currentDir \n";
			}

		} # end directory is not empty

} # end checkDir;
