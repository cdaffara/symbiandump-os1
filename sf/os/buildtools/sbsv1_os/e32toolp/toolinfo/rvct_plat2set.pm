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
# This module provides a mapping from build platforms (e.g. ARMV5) to the
# corresponding RVCT environment settings. The information is obtained from an INI
# file pointed to by the environment variable ABLD_PLAT_INI. The file can have two
# different formats. The first format (Format 1) is:
# [Symbian_OS_v9.5]
# ARMV5 = RVCT 2.2.616
# ARMV6 = RVCT 2.2.616
# ARMV7 = RVCT 3.1.700
# [Symbian_OS_v9.6]
# ARMV5 = RVCT 3.1.862
# ARMV7 = RVCT 4.0.100
# [Symbian_OS_vFuture]
# ARMV5 = RVCT 4.1.812
# ARMV7 = RVCT 4.0.100
# This format matches the tuple <OS version, build platform> onto a RVCT version.
# The build system will look up the OS version in "\epoc32\data\buildinfo.txt".
# The second format (Format 2) is:
# ARMV5 = RVCT 2.2.616
# ARMV6 = RVCT 2.2.616
# ARMV7 = RVCT 3.1.700
# This format doesn't take the OS version into account and is mostly intended for
# the Integration team.
# If ABLD_PLAT_INI is not set, or if the INI file doesn't contain any data for the
# given platform, whatever RVCT version is found in the path will be used.
# 
#

package RVCT_plat2set;
use strict;

use RVCT_ver2set;

#
# PUBLIC FUNCTIONS
#

# Returns the RVCT version corresponding to the given build platform. The first
# function returns the data as a string (e.g. "2.2.616"); the second function
# returns the data as a list (e.g. [2,2,616]).
sub get_version_string($);
sub get_version_list($);

# Returns true if an RVCT version corresponding to the given platform exists.
sub compiler_exists($);

# Given a build platform, returns what the name and value of the RVCT??BIN variable
# would be. 
sub get_bin_name($);
sub get_bin_path($);

# Given a build platform, returns what the name and value of the RVCT??INC variable
# would be. 
sub get_inc_name($);
sub get_inc_path($);

# Given a build platform, returns what the name and value of the RVCT??LIB variable
# variable would be. 
sub get_lib_name($);
sub get_lib_path($);

# Given a build platform and the name of an RVCT library, returns the full path to
# that library.
sub find_lib($$);

#
# PRIVATE HELPER FUNCTIONS
#

sub _get_something(@);
sub _err_and_die(@);
sub _get_os_version();
sub _parse_section_data($@);


#
# GLOBAL DATA
#

my %g_data;


sub get_version_string($)
{
    return _get_something(@_, 'version');
}

sub get_version_list($)
{
    my $tmp = _get_something(@_, 'version');

    return split(/\./, $tmp);
}

sub compiler_exists($)
{
    my $plat = shift;

    if ( $g_data{$plat}->{version} || $g_data{0}->{version} )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

sub get_bin_name($)
{
    return _get_something(@_, 'bin_name');
}

sub get_bin_path($)
{
    return _get_something(@_, 'bin_path');
}

sub get_inc_name($)
{
    return _get_something(@_, 'inc_name');
}

sub get_inc_path($)
{
    return _get_something(@_, 'inc_path');
}

sub get_lib_name($)
{
    return _get_something(@_, 'lib_name');
}

sub get_lib_path($)
{
    return _get_something(@_, 'lib_path');
}

sub find_lib($$)
{
    my $plat = shift;
    my $lib  = shift;

    my $p = get_lib_path($plat);

    my $f1 = "$p\\armlib\\$lib";
    my $f2 = "$p\\cpplib\\$lib";

    return "$p\\armlib\\$lib" if -f $f1;
    return "$p\\cpplib\\$lib" if -f $f2;

    _err_and_die("could not find $lib for platform $plat.");
}

sub _get_something(@)
{
    my $plat = shift;
    my $what = shift;
    
    my $data = $g_data{$plat}->{$what};

    if (!$data)
    {
        $data = $g_data{0}->{$what};
    }

    return $data;
}

sub _err_and_die(@)
{
    if ( defined(&main::FatalError) )
    {
        main::FatalError(@_);
    }
    else
    {
        print STDERR "error: @_\n";
    }

    exit 1;
}

sub _get_os_version()
{
    my $fname = "/epoc32/data/buildinfo.txt";
    
    my $os_ver = "";

    open (my $in,  "<",  $fname) or _err_and_die("couldn't open $fname.");

    while (<$in>)
    {
        chomp;

        if ( $_ =~ /^\s*ManufacturerSoftwareBuild\s+M\d\d\d\d\d_(\S+)\s*$/i )
        {
            $os_ver = $1;
            last
        }
    }

    close $in or _err_and_die("couldn't close $fname.");

    _err_and_die("couldn't read OS version in $fname") unless $os_ver;

    return $os_ver;
}

sub _parse_section_data($@)
{
    my ($fname, @lines) = @_;

    for (@lines)
    {
        if ( $_ =~ /^\s*(\w+)\s*=\s*RVCT\s+(\d+\.\d+\.\d+)\s*$/i )
        {
            my $plat = uc($1);
            my $vers = $2;

            _err_and_die("$fname: platform $plat already defined.") if $g_data{$plat};
            _err_and_die("$fname: RVCT $vers doesn't exist.") unless RVCT_ver2set::compiler_exists($vers);

            $g_data{$plat}->{version} = $vers;

            $g_data{$plat}->{bin_name} = RVCT_ver2set::get_bin_name($vers);
            $g_data{$plat}->{bin_path} = RVCT_ver2set::get_bin_path($vers);

            $g_data{$plat}->{inc_name} = RVCT_ver2set::get_inc_name($vers);
            $g_data{$plat}->{inc_path} = RVCT_ver2set::get_inc_path($vers);

            $g_data{$plat}->{lib_name} = RVCT_ver2set::get_lib_name($vers);
            $g_data{$plat}->{lib_path} = RVCT_ver2set::get_lib_path($vers);
        }
        else
        {
            _err_and_die("$fname: invalid field: $_.");
        }
    }
}


# initialize module
{
    # Initialise platform "0", wich represents the RVCT version found in the path.
    my $vsnworked = 0; # Flag to check if --vsn command worked
    # clear --licretry from RVCT22_CCOPT
    my $key;
    my %RVCT_CCOPT;
    foreach $key (keys %ENV) {
        if($key =~ /RVCT\d+_CCOPT/i ) {
            $RVCT_CCOPT{$key} = $ENV{$key};
            $ENV{$key} =~ s/--licretry//i;
        }
    }
    my @lines = qx/armcc --vsn 2>&1/;

    foreach (@lines)
    {
        if ($_ =~ /\bRVCT(\d)\.(\d)\s+\[Build (\d+)\]/ )
        {
	    $vsnworked = 1;
            my ($bin, $inc, $lib) = ("RVCT$1$2BIN", "RVCT$1$2INC", "RVCT$1$2LIB");

            $g_data{0}->{version} = "$1.$2.$3";

            $g_data{0}->{bin_name} = $bin;
            $g_data{0}->{bin_path} = $ENV{$bin};

            $g_data{0}->{inc_name} = $inc;
            $g_data{0}->{inc_path} = $ENV{$inc};

            $g_data{0}->{lib_name} = $lib;
            $g_data{0}->{lib_path} = $ENV{$lib};

            last;
        }
    }
    # In case --vsn doesn't work, call --version_number and setup environment variables
    if ($vsnworked == 0)
    {
	    my @lines = qx/armcc --version_number 2>&1/;
	    foreach (@lines) 
	    {
		    if ($_ =~ /^(\d{1})(\d{1})\d{1}(\d{3})$/) 
		    {
			    my ($bin, $inc, $lib) = ("RVCT$1$2BIN", "RVCT$1$2INC", "RVCT$1$2LIB");
			    $g_data{0}->{version} = "$1.$2.$3";
			    
			    $g_data{0}->{bin_name} = $bin;
			    $g_data{0}->{bin_path} = $ENV{$bin};

			    $g_data{0}->{inc_name} = $inc;
		            $g_data{0}->{inc_path} = $ENV{$inc};

		            $g_data{0}->{lib_name} = $lib;
		            $g_data{0}->{lib_path} = $ENV{$lib};

            		    last;
	   		 }
    	    }
    }

    # restore RVCT22_CCOPT
    foreach $key (keys %RVCT_CCOPT) {
        $ENV{$key} = $RVCT_CCOPT{$key};
    }
    # Initialise all platforms defined in the INI file, if that file exists.

    my $fname = $ENV{ABLD_PLAT_INI};

    if ($fname) # The environment variable is set.
    {
        _err_and_die("ABLD_PLAT_INI doesn't point to a file.") unless -f $fname;

        open (my $in,  "<",  $fname) or _err_and_die("couldn't open ABLD_PLAT_INI = $fname.");

        my @lines = ();
        my $format1 = 0;

        while (<$in>)
        {
            chomp;

            next if /^\s*;/ ;
            next if /^\s*#/ ;
            next if /^\s*$/ ;

            if ($_ =~ /^\s*\[/i)
            {
                # This must be the start of an INI section so We treat the file as
                # format 1.
                $format1 = 1;
            }

            push @lines, $_;
        }


        close $in or _err_and_die("couldn't close ABLD_PLAT_INI = $fname.");
        _err_and_die("$fname contains no data") unless @lines;

        if ($format1)
        {
            my $os_ver = _get_os_version();
            my @lines2 = ();

            while (@lines)
            {
                my $line = shift @lines;
                
                if ( $line =~ /^\s*\[$os_ver\]/i)
                {
                    last;
                }
            }

            while (@lines)
            {
                my $line = shift @lines;

                if ( $line =~ /^\s*\[/i)
                {
                    last;
                }
            
                push @lines2, $line;
            }

            _parse_section_data($fname, @lines2);
        }
        else # Format 2.
        {
            # The file is one big section without header.
            _parse_section_data($fname, @lines);
        }

    } # if ($fname)
}


1;


