@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S %0 %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
if errorlevel 1 goto script_failed_so_exit_with_non_zero_val 2>nul
goto endofperl
@rem ';
#!perl
#line 15
use strict;
use FindBin;

my $SELF_NAME = "set-rvct";

my $self_path;

BEGIN
{
    $self_path = $FindBin::Bin;
}

use lib $self_path;

use RVCT_ver2set;


# Some internal helper functions.
sub _usage_and_die(@);
sub _err_and_die(@);
sub _warning(@);
sub _fixup_path($$);


sub main(@)
{
    my @available = RVCT_ver2set::get_versions();

    _err_and_die("no RVCT versions found; check that ABLD_RVCT_INI is set.")
        unless(@available);

    my ($vers, @junk) = @_;

    _usage_and_die(@available)
        if ( !$vers || @junk || !RVCT_ver2set::compiler_exists($vers) );

    if ($ENV{ABLD_PLAT_INI})
    {
        _warning("ABLD_PLAT_INI is set; the build system might clobber your settings.");
    }

    my $path = _fixup_path( $ENV{PATH}, RVCT_ver2set::get_bin_path($vers) );

    # Create the batch file.
    {
        my $fname = "..__.bat";

        open (my $file,  ">",  $fname)
            or _err_and_die("couldn't create $fname.");

        print $file "set PATH=$path\n";

        my ($n, $p);

        $n = RVCT_ver2set::get_bin_name($vers);
        $p = RVCT_ver2set::get_bin_path($vers);
        print $file "set $n=$p\n";

        $n = RVCT_ver2set::get_inc_name($vers);
        $p = RVCT_ver2set::get_inc_path($vers);
        print $file "set $n=$p\n";

        $n = RVCT_ver2set::get_lib_name($vers);
        $p = RVCT_ver2set::get_lib_path($vers);
        print $file "set $n=$p\n";

        print $file "echo.\n";
        print $file "armcc --vsn\n";

        close $file or _err_and_die("couldn't close $fname.");
    }
}

sub _usage_and_die(@)
{
    for (@_)
    {
        print STDERR "    $SELF_NAME $_\n";
    }

    exit 1;
}

sub _err_and_die(@)
{
    print STDERR "error: @_\n";
    exit 1;
}

sub _warning(@)
{
    print STDERR "warning: @_\n";
}

sub _fixup_path($$)
{
    my @path = split(/;/, shift);
    my $bin  = shift;

    my @result = ();

    foreach (@path)
    {
        push @result, ($_) unless ($_ eq $bin);
    }

    return join(";", $bin, @result);
}


main(@ARGV);



__END__
:endofperl

if "%errorlevel%" == "0" (if exist ..__.bat call ..__.bat & del ..__.bat)

:: vim:ft=perl

