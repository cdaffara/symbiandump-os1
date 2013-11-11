# Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# This module provides a mapping from build platforms (e.g. GCCE) to the
# corresponding GCC-E environment settings.
# Currently this module only provides very simple functionality. The intention is
# that if we're going to support different GCC-E versions for different build
# platforms (as we do for RVCT) this module should be extended to provide similar
# functionality to rvct_plat2set.
# 
#

package gcce_plat2set;
use strict;


# Returns the GCCE version corresponding to the given build platform. The first
# function returns the data as a string (e.g. "4.2.3"); the second function
# returns the data as a list (e.g. [4,2,3]).
sub get_version_string($);
sub get_version_list($);

# Returns true if a GCC-E version for the given platform exists.
sub compiler_exists($);


my $g_version;


sub get_version_string($)
{
    return $g_version;
}

sub get_version_list($)
{
    return split(/\./, $g_version);
}

sub compiler_exists($)
{
    if ($g_version)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

BEGIN
{
    my $vers = qx/arm-none-symbianelf-gcc -dumpversion 2>&1/;

    if ($vers =~ /^\s*(\d+\.\d+.\d+)\s*$/)
    {
        $g_version = "$1";
    }
}


1;


