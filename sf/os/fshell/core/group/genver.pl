# genver.pl
# 
# Copyright (c) 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#
use strict;
use lib "../../tools";
use fshu;

my $platform = shift @ARGV;
my $version = fshu::Version();
my $localTime = scalar(localtime);
my $gmTime = scalar(gmtime);
my $builder = $ENV{USERNAME};
my $compilerVersion = CompilerVersion();


print "
#include <fshell/iocli.h>

extern void PrintVersionInfo(RIoWriteHandle& aOut, TBool aVerbose)
	{
	if (aVerbose)
		{
		aOut.Write(_L(\"Version:              $version\\r\\n\"));
		aOut.Write(_L(\"Local build time:     $localTime\\r\\n\"));
		aOut.Write(_L(\"GMT build time:       $gmTime\\r\\n\"));
		aOut.Write(_L(\"User-name of builder: $builder\\r\\n\"));
		aOut.Write(_L(\"Compiler version:     $compilerVersion\\r\\n\"));
#ifdef _DEBUG
		aOut.Write(_L(\"Build configuration:  debug\\r\\n\"));
#else
		aOut.Write(_L(\"Build configuration:  release\\r\\n\"));
#endif
		}
	else
		{
		aOut.Write(_L(\"$version\\r\\n\"));
		}
	}
";

sub CompilerVersion {
  my $version = 'Unknown';
  if ($platform =~ /^armv5$/i) {
    open (COMPILER, "armcc 2>&1 |") or die "Couldn't run \"armcc\": $!\n";
    while (my $line = <COMPILER>) {
      if ($line =~ /\[Build\s+\d+\]/) {
	chomp $line;
	$version = $line;
	last;
      }
    }
    close (COMPILER);
  }
  elsif ($platform =~ /^gcce$/i) {
    open (COMPILER, "arm-none-symbianelf-gcc.exe -v 2>&1 |") or die "Couldn't run \"arm-none-symbianelf-gcc.exe -v\": $!\n";
    while (my $line = <COMPILER>) {
      if ($line =~ /^gcc version/) {
	chomp $line;
	$version = $line;
	last;
      }
    }
    close (COMPILER);
  }
  elsif ($platform =~ /^(arm4|armi|thumb)$/i) {
    open (COMPILER, "gcc.exe -v 2>&1 |") or die "Couldn't run \"gcc.exe -v\": $!\n";
    while (my $line = <COMPILER>) {
      if ($line =~ /^gcc version/) {
	chomp $line;
	$version = $line;
	last;
      }
    }
    close (COMPILER);
  }
  elsif ($platform =~ /^winscw$/i) {
    open (COMPILER, "mwccsym2.exe 2>&1 |") or die "Couldn't run \"mwccsym2.exe\": $!\n";
    while (my $line = <COMPILER>) {
      if ($line =~ /^Version/) {
	chomp $line;
	$version = "mwccsym2 $line";
	last;
      }
    }
    close (COMPILER);
  }
  return $version;
}
