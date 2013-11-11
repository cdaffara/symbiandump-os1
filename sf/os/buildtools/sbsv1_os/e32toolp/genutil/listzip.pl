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
# e32toolp/genutil/listzip.pl
# Utility for listing the contents of a zip file.
# Syntax:
# perl listzip.pl <prefix> <zipfile>
# This command will print all files in the <zipfile>. Each file name is prefixed by
# <prefix> and is printed on a separate line.
# 
#

sub _print_usage_and_die();
sub _print_err_and_die(@);

sub main(@)
{
    my ($prefix, $zipf, @junk) = @_;

    _print_usage_and_die() if (!$prefix || !$zipf || @junk);

    _print_err_and_die("$prefix is not a directory.") unless -d $prefix;
    _print_err_and_die("$zipf doesn't exist.") unless -f $zipf;

    $prefix =~ s/\/\//\//g;    # SBS provides "Q://", so reduce the doubling
    $prefix =~ s/\/$//;        # remove trailing /, if any
		
    my @raw_data = qx/unzip -l $zipf/;

    for (@raw_data)
    {
        if ($_ =~ /^\s*\d+\s+\d\d[-|\/]\d\d[-|\/]\d\d\s+\d\d:\d\d\s+(.*)/)
        {
            my $line = "${prefix}/$1";
            $line =~ s/\//\\/g;
	    # don't print directories under the <build> tags
	    if (!($line =~ /\\$/)) {
		    print "$line\n";
		    }
        }
    }
}

sub _print_usage_and_die()
{
    print "usage: listzip.pl <prefix> <zipfile>\n";
    exit 2;
}

sub _print_err_and_die(@)
{
    print "listzip.pl: error: @_\n";
    exit 1;
}

main(@ARGV);


