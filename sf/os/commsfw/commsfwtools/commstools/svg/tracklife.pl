# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

use strict;
open LOG, "log.seq" or die "Failed to open log.seq";
my %ob;
my $line = 0;
while(<LOG>)
{
  chomp;
  ++$line;
  if($_ =~ /t .+\(([0-9a-f]+) created\)/i)
  {
    if(defined $ob{$1})
    {
      print "ERROR[#$line]: double create of $1 - was [ $ob{$1}->[1] ] @ #$ob{$1}->[0], now [ $_ ]\n";
    }
    $ob{$1} = [$line, $_];
  }
  elsif($_ =~ /t .+\(([0-9a-f]+) destroyed\)/i)
  {
    if(!defined $ob{$1})
    {
      print "ERROR: destroy of unknown $1 [ $_ ]\n";
    }
    delete $ob{$1};
  }
  elsif($_ =~ /^l \^Booting$/)
  {
  	if(DumpObjs())
  	{
	  print "\nRebooting...\n";
  	}	
  	%ob = ();
  }
}
DumpObjs();

sub DumpObjs()
{
  my @vals = sort { $a->[0] <=> $b->[0] } values %ob;
  if($#vals >= 0)
  {
    print "\nRemaining objs:\n";
    for (@vals)
    {
      print "$_->[1] [#$_->[0]]\n";
    }
    return 1;
  }
  return 0;
}

