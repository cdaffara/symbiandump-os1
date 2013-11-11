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
# Convert the given header file from Khronos into a stub implementation file
# 
#

use File::Basename;
use File::Path;

my $debug = 0;
my $prog = "conv_khronos_openvg_hdr_to_cpp.pl";
my $source = shift;
my $target = shift;
my $operation_mode = shift;
my @lines;

if ($debug) {
  print "$prog: Args $source $target $operation_mode\n";
}

if ("$operation_mode" eq "delete") {
  &cleanup();
  exit 0;
} elsif ("$operation_mode" eq "create") {
  &setupFiles();
  &generateStubImplementation();
  exit 0;
} else {
  print "Usage error: $prog source target [create|delete]\n";
  exit 1;
}

sub cleanup()
  {
    unlink "$target";
  }

sub setupFiles()
  {
    my $dir;
    $dir = dirname($target);
    mkpath $dir;
	
    open(INFILE,  "$source") or die "Can't open input file $source; $!\n";
    open(OUTFILE, ">$target") or die "Can't open output file $target; $!\n";
    print OUTFILE '/* Auto-generated: ' . "$prog" . ' v1.0 */' . "\n";
    print OUTFILE '/* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).  All rights reserved. */' . "\n";
  }

sub generateStubImplementation()
  {
    @lines = <INFILE>;
    my $s = "";
    foreach (@lines) {
      # Find function prototype lines
      if (/^VG[U]?_API_CALL/ || length($s) != 0) {
	    $s = $s.$_;
		if (/;/) {
		  # Convert the function prototype into a stub function definition
		  $s =~ s/\;$/ { }/;
		  # Record the stub functions.  There will be a stub implementation
		  # file which includes these stub functions.  This ensures we never
		  # accidentally miss a new function added to the header file supplied
		  # as $source.  We expect compiler warnings (missing use of arguments,
		  # absent return value etc.).  The aim is to get something which will
		  # compile so that a DEF file can be generated.
		  print OUTFILE "$s";
		  $s = "";
		}
      }       
    }

  }
