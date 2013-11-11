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
use Getopt::Long;
my $trimmed = 1;
my $inPlace = 0;
my $zapHex = 0;
my $zapTime = 0;
my $forceCDU = 0;
my $forceULOG = 0;
GetOptions('trim|t!' => \$trimmed, 'inplace|i!' => \$inPlace, 'zaphex!' => \$zapHex, 'zaptime!' => \$zapTime, 'cdu!' => \$forceCDU, 'ulogger!' => \$forceULOG);

my $log = "log.txt";
if(defined($ARGV[0]))
{
  $log = $ARGV[0];
  die "$log not found" if !-e $log;
}
elsif(!-e $log)
{
  $log = "/epoc32/winscw/c/logs/log.txt";
}
my %ids;
my $uLogger = $forceULOG;
if($inPlace)
{
  @ARGV = $log;
  $^I = ".bak";
  if(!$forceCDU && !$forceULOG && defined($_ = <>))
  {
	$uLogger = $_ =~ /^Sequence,/;
	DoLine($_);
  }
  while(<>)
  {
    DoLine($_);
  }
}
else
{
  open LOG, $log or die "Failed opening '$log'";
  if(!$forceCDU && !$forceULOG && defined($_ = <LOG>))
  {
	$uLogger = $_ =~ /^Sequence,/;
	DoLine($_);
  }
  while(<LOG>)
  {
    DoLine($_);
  }
}

my $CDULoggerTid;

sub DoCDULine($$)
{
  my ($pref, $line) = ($_[0], $_[1]);
  if($line !~ /^#/ && $line =~ /(^[^\s]+)\s+([^\s]+)\s+([^\s]+)\s+([^\s]+)(\s+)(.*)/)
  {
    my @bits = ( $1, $2, $3, $4, $5, $6 );
    my $tid = hex($bits[3]);
    if($bits[5] =~ /^W(\d+): /)
    {
      $ids{$tid} = $1;
    }
    elsif(defined $ids{$tid})
    {
      $bits[5] = sprintf "W%d: %s", $ids{$tid}, $bits[5];
    }
    $bits[5] =~ s/[0-9a-f]{8}/--------/gi if($zapHex);
    if($trimmed)
    {
	  print "$pref$bits[0]\t$bits[1]$bits[4]$bits[5]\n";
	}
	else
	{
	  print "$pref$bits[0]\t$bits[1]\t$bits[2]\t$bits[3]$bits[4]$bits[5]\n";
	}
    }
    else
    {
	print "$pref$line\n" unless $zapTime && $line =~ /^#Time/;
  }
}

sub DoULoggerLine($)
{
  if($_[0] =~ /^(\d+),(\d+),(\d+),(\d+),(\d+),(0x[0-9a-fA-F]+),(.+)/)
  {
    my ($seq, $pri, $sub, $time1, $time2, $context, $desc)  = ($1, $2, $3, $4, $5, $6, $7);
    $time1 = $time2 = "" if $zapTime;
   	$seq = '---' if($zapHex);
    if($desc =~ /^W(\d+):\s+SocketServer::InitL\(\) Done!/)
    {
      print stderr "WARNING: $context was defined as $ids{$context}\n" if(defined($ids{$context}) && $ids{$context} ne $1);
      $ids{$context} = $1;
    }
    elsif($desc =~ /^EThreadIdentification: Thread Create: \[NThread=(0x[0-9a-fA-F]+)\] \[DProcess=0x[0-9a-fA-F]+\] \[Name=Comsdbg2\]/)
    {
      $CDULoggerTid = $1;
    }
    elsif($desc =~ /^(ENodeMessages: )(.*)/ || $desc =~ /^(ERDebugPrintf: )(.*)/ || $desc !~ /W\d+:/)
    {
      if($1 eq 'ERDebugPrintf: ' && $context eq $CDULoggerTid)
      {
	   	$context = '0x--------' if($zapHex);
        my $pref = "$seq,$pri,$sub,$time1,$time2,$context,";
        DoCDULine($pref, $2);
        return;
      }
      elsif($1 eq 'ENodeMessages: ' && $2 !~ /W\d+:/)
      {
	   	$context = '0x--------' if($zapHex);
        my $pref = "$seq,$pri,$sub,$time1,$time2,$context,";
        DoCDULine($pref, $2);
        return;
      }
      my $tid = $ids{$context};
      $desc = "W$tid: $desc" if defined $tid;
    }
   	$context =~ s/[0-9a-f]{8}/--------/gi if($zapHex);
   	$desc =~ s/[0-9a-f]{8}/--------/gi if($zapHex);
    print "$seq,$pri,$sub,$time1,$time2,$context,$desc\n";
    }
  elsif($_[0] ne '')
  {
    print "$_[0]\n";
  }
}

sub DoLine($)
{
  chomp $_[0];
  if(!$uLogger)
  {
    DoCDULine('', $_[0]);
  }
  else
  {
    DoULoggerLine($_[0]);
  }
}

