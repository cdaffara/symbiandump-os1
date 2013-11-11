#!perl -w
# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This simple script shows how to modify the content of a feature manager
# data file.
# 
#

use strict;
use FMCreate;

#
# Hardwire the datafile - this is only an example.
# 
my $datfile = "/epoc32/release/winscw/udeb/z/private/10205054/features.dat";
my $datfile2 = "/epoc32/release/winscw/udeb/z/private/10205054/features2.dat";
my $datfileback = "/epoc32/release/winscw/udeb/z/private/10205054/features.bak";

#
# Create an object that represents a feature data file.
#
my $fmc = FMCreate->new();

#
# Load the content of the data file into our FMCreate object.
# Note that this will die if the content does not seem to be a feature set
# file. This can happen if the first four bytes aren't 'feat' or if reading
# the file fails at any point. This will also happen if the file is the wrong
# size.
#
$fmc->LoadUp($datfile) or die "Failed to load up data from '$datfile'\n";

# Setup an array of all the features we need to switch on.
my $ffuid;
my $ff;
my @tomtab;

$tomtab[0] = 81; 	# KFeatureIdProtocolGsm
$tomtab[1] = 82; 	# KFeatureIdProtocolWcdma
$tomtab[1] = 1696;	# KFeatureIdOnScreenDialer

foreach $ffuid (@tomtab)
#for ($ffuid = 1696; $ffuid <= 1696; $ffuid++)
{
	$ff = $fmc->GetFeatureFlagByUID($ffuid);
	if(ref($ff) ne "FeatureFlag")
	{
		printf ("Feature flag uid 0x%0x was not already in $datfile, creating it.\n", $ffuid);
		#
		# Now add a new feature. The three arguments are UID, status flags (not defined
		# here) and user data word.
		#
		$ff = FeatureFlag->new($ffuid, undef, 0x00000000);
		die "Couldn't create new feature flag object.\n" unless(ref($ff));
			
		#
		# Add it to our existing feature data.
		#
		$fmc->AddFeatureFlag($ff) or die "Couldn't add new feature flag..\n";

		printf ("Feature flag uid 0x%0x created ok.\n", $ffuid);
	} 

	$ff->Supported(1);
	$ff->Upgradable(0);
	$ff->Modifiable(0);
	$ff->BlackListed(0);
	$ff->Uninitialized(0);
	$ff->Persisted(0);
}
#
# Now write out the file to a new location
#
$fmc->WriteToFile($datfile2) or die "Couldn't write feature data file '$datfile2'\n";

rename($datfile, $datfileback) or die "Couldn't backup feature data file '$datfile'\n";
rename($datfile2, $datfile) or die "Couldn't copy feature data file '$datfile2'\n";

printf ("\tFeature Database setup\n");
#
# Example code to remove a feature flag.
#
#$fmc->RemoveFeatureFlagByUID($ffuid) or die "Couldn't remove feature flag\n";

