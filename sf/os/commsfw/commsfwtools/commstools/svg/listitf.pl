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
my $incPersist = 0;
my $incLastSeen = 1;
GetOptions('persist!' => \$incPersist, 'lastseen!' => \$incLastSeen);

my %regs;
my %persistItf;
my %lastSeen;
while(<>)
{
  if($_ =~ /\s+(W\d+): RegisterItf\(([0-9a-fA-F]+)/)
  {
    my $key = "$1: $2";
    $regs{$key} = [ "(#$.)", $. ];
  }
  elsif($_ =~ /\s+(W\d+): DeregisterItf\(([0-9a-fA-F]+)/)
  {
    my $key = "$1: $2";
    delete $regs{$key};
  }
  elsif($_ =~ /\s+(W\d+): (.+):\s+created \[MCFNode ([0-9a-fA-F]+)/)
  {
    my $key = "$1: $3";
    $regs{$key} = ["$2 (#$.)", $.] if defined($regs{$key});
  }
  elsif($_ =~ /CSockManData::AddPersistentItf\(([0-9a-fA-F]+)\)\s+\[#(\d+)\]/)
  {
    $persistItf{$1} = $2;
  }
  elsif($incLastSeen && $_ =~ /TCFSignatureBase:\tDispatchL\(\): Sender=([0-9a-fA-F]+), Recipient=([0-9a-fA-F]+), Message=(.+), Activity=/)
  {
	$lastSeen{$1} = [$3, $.];
	$lastSeen{$2} = [$3, $.];
  }
}

my @keys = sort { $regs{$a}->[1] <=> $regs{$b}->[1] } keys %regs;
for my $key (@keys)
{
  my $persistInfo;
  if($key =~ /W\d+: ([0-9a-fA-F]+)/)
  {
    $persistInfo = $persistItf{$1} if defined($persistItf{$1});
  }
  if(!defined($persistInfo) || $incPersist)
  {
  	my $lastSeenId;
  	if($incLastSeen && defined ($lastSeenId = $lastSeen{$1}))
  	{
	  $lastSeenId = "#$lastSeenId->[1], $lastSeenId->[0]";
	}
    print "$key $regs{$key}->[0]";
    print " (last seen $lastSeenId)" if defined($lastSeenId);
    print " (persistent #$persistInfo)" if defined($persistInfo);
    print "\n";
  }
}

