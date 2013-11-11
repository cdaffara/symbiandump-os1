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
# This module provides a mapping from RVCT versions to the corresponding environment
# settings. The information is obtained from a configuration file pointed to by the
# environment variable ABLD_RVCT_INI. The following example shows the file's
# structure:
# [2.2.616]
# bin=C:\Apps\ARM-RVCT\2.2.616\bin
# inc=C:\Apps\ARM-RVCT\2.2.616\inc
# lib=C:\Apps\ARM-RVCT\2.2.616\lib
# [3.1.700]
# bin=C:\Apps\ARM-RVCT\3.1.700\bin
# inc=C:\Apps\ARM-RVCT\3.1.700\inc
# lib=C:\Apps\ARM-RVCT\3.1.700\lib
# If the file is used, it must contain at least one section.
# 
#


package RVCT_ver2set;
use strict;

#
# PUBLIC FUNCTIONS
#

# Returns a list of available RVCT versions, for example ["2.2.435", "2.2.616"].
sub get_versions();

# Returns true if the given RVCT version exists.
sub compiler_exists($);

# Given an RVCT version, returns what the name and value of the RVCT??BIN
# variable would be. 
sub get_bin_name($);
sub get_bin_path($);

# Given an RVCT version, returns what the name and value of the RVCT??INC
# variable would be. 
sub get_inc_name($);
sub get_inc_path($);

# Given an RVCT version, returns what the name and value of the RVCT??LIB
# variable would be. 
sub get_lib_name($);
sub get_lib_path($);


#
# PRIVATE HELPER FUNCTIONS
#

# Prints an error message and then terminates.
sub _err_and_die(@);


#
# GLOBAL DATA
#

# Contains all the information read from the configuration file.
my %g_data;


sub get_versions()
{
    return sort(keys %g_data);
}

sub compiler_exists($)
{
    my $vers = shift;
    return ( $g_data{$vers} ); 
}

sub get_bin_name($)
{
    my $vers = shift;
    return $g_data{$vers}->{bin_name}; 
}

sub get_bin_path($)
{
    my $vers = shift;
    return $g_data{$vers}->{bin_path}; 
}

sub get_inc_name($)
{
    my $vers = shift;
    return $g_data{$vers}->{inc_name}; 
}

sub get_inc_path($)
{
    my $vers = shift;
    return $g_data{$vers}->{inc_path}; 
}

sub get_lib_name($)
{
    my $vers = shift;
    return $g_data{$vers}->{lib_name}; 
}

sub get_lib_path($)
{
    my $vers = shift;
    return $g_data{$vers}->{lib_path}; 
}

sub _err_and_die(@)
{
    print STDERR "error: @_\n";
    exit 1;
}


# initialize module
{
    my $fname = $ENV{ABLD_RVCT_INI};

    if ($fname) # The environment varaible is set.
    {
        _err_and_die("ABLD_RVCT_INI doesn't point to a file.") unless -f $fname;

        open (my $in,  "<",  $fname) or _err_and_die("couldn't open ABLD_RVCT_INI = $fname.");

        my @lines = ();

        while (<$in>)
        {
            chomp;

            next if /^\s*;/ ;
            next if /^\s*#/ ;
            next if /^\s*$/ ;

            push @lines, $_;
        }

        close $in or _err_and_die("couldn't close ABLD_RVCT_INI = $fname.");

        while (@lines)
        {
            my $ver = shift @lines;
            my $kv1 = shift @lines;
            my $kv2 = shift @lines;
            my $kv3 = shift @lines;

            my $Mm = "";

            if ( $ver =~ /^\s*\[(\d+)\.(\d+)\.(\d+)\]\s*$/ )
            {
                _err_and_die("$fname: duplicate section: $ver.") if $g_data{$1};
                $ver = "$1.$2.$3";
                $Mm  = "$1$2";
            }
            else
            {
                _err_and_die("$fname: invalid section header: $ver.");
            }

            my %kv = ();

            for ($kv1, $kv2, $kv3)
            {
                _err_and_die("$fname: not enough data in section $ver.") unless $_;

                if ( $_ =~ /^\s*(bin|inc|lib)\s*=(.*)$/i )
            {
                    my $key = uc($1);
                    my $val = $2;

                    $val =~ s/^\s+//;
                    $val =~ s/\s+$//;

                    _err_and_die("$fname: in section $ver: \"$key\" doesn't point to a directory.") unless -d $val;
                    _err_and_die("$fname: in section $ver: duplicate key \"$key\".") if $kv{$key};

                    $kv{$key} = $val;
                }
                else
                {
                    _err_and_die("$fname: in section $ver: invalid line \"$_\".");
                }
            }

            $g_data{$ver}->{bin_name} = "RVCT${Mm}BIN";
            $g_data{$ver}->{bin_path} = "$kv{BIN}";
            $g_data{$ver}->{inc_name} = "RVCT${Mm}INC";
            $g_data{$ver}->{inc_path} = "$kv{INC}";
            $g_data{$ver}->{lib_name} = "RVCT${Mm}LIB";
            $g_data{$ver}->{lib_path} = "$kv{LIB}";
        }

    }
    else
    {
        # ABLD_RVCT_INI isn't set. This is not an error. All public functions will
        # return NULL.
    }
}

1;


