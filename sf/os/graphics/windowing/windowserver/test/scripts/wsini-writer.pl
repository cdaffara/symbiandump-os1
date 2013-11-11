#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#!perl

use strict;

my $file = $ARGV[0];

die "Usage: $0 filename\n" if (!$file);
die "ERROR: \"$file\" does not exist\n" if (!-f $file);

&AddNoRedrawStoring($file) if($ARGV[3] =~ /^noredrawstoring$/i);
&AddAutoLine($file) if ($ARGV[2] =~ /^auto$/i);
&AddMultiLine($file) if ($ARGV[1] =~ /^multiscreen$/i);


# This function adds a line to a wsini.ini so that it runs the tests in auto mode
# This functions assumes the file is in UTF16
sub AddAutoLine
{
  my $file = shift;

  my $string = &ascii_to_utf16('SHELLCMD AUTO');

  &Write_UTF16_Newline($file);

  open(FILE, ">>$file") or warn "WARNING: Could not open file: $!\n";
  print FILE $string;
  close FILE;

  &Write_UTF16_Newline($file);
}


# This function adds a line to a wsini.ini so that it runs in multiscreen
# This functions assumes the file is in UTF16
sub AddMultiLine
{
  my $file = shift;

  my $string0 = &ascii_to_utf16('[SCREEN0]');
  my $string1 = &ascii_to_utf16('[SCREEN1]');

  &Write_UTF16_Newline($file);

  open(FILE0, ">>$file") or warn "WARNING: Could not open file: $!\n";
  print FILE0 $string0;
  close FILE0;
  &Write_UTF16_Newline($file);

  open(FILE1, ">>$file") or warn "WARNING: Could not open file: $!\n";
  print FILE1 $string1;
  close FILE1;
  &Write_UTF16_Newline($file);
}

# This function adds a line to a wsini.ini so that it runs the tests with NOREDRAWSTORING flag
# This functions assumes the file is in UTF16
sub AddNoRedrawStoring
{
  my $file = shift;

  my $string = &ascii_to_utf16('NOREDRAWSTORING');

  &Write_UTF16_Newline($file);

  open(FILE, ">>$file") or warn "WARNING: Could not open file: $!\n";
  print FILE $string;
  close FILE;

  &Write_UTF16_Newline($file);
}


sub Write_UTF16_Newline
{
  my $file = shift;

  open(BIN, ">>$file") or warn "WARNING: Could not open \"$file\": $!\n";
  binmode BIN;
  sysseek BIN, 0, 'SEEK_END';
  syswrite BIN, "\x0D\x00\x0A\x00" or warn "WARNING: Could not write to file\n";
  close BIN;
}



# Function that accepts an ASCII string and returns the same string in UTF16
sub ascii_to_utf16 {
  my $utf16_string = "";
  my $ascii_string = shift;
  my $lengthofstring = length($ascii_string);

  for (my $count=1; $count<=$lengthofstring; $count++)
  {
    my $char = substr($ascii_string,$count-1,1);
    $utf16_string .= $char;
    $utf16_string .= "\x00";
  }

  return $utf16_string;
}