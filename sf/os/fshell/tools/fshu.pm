#!perl
# fshu.pm
# 
# Copyright (c) 2007 - 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#

# Description:
# fshu.pm - A collection of common utility sub-routines used by other fshell scripts.

package fshu;
use strict;
use File::Path;
use File::Copy;
use File::Basename;

#
# Subs.
#

sub RelativePath {
  my $path = TidyPath(shift);
  my $relativeTo = TidyPath(shift);
  die "Error: \"$relativeTo\" is not absolute\n" unless ($relativeTo =~ /^([a-zA-Z]:)?\\/);
  $relativeTo =~ s/^([a-zA-Z]:)?\\//; # Remove drive letter and leading '\'.
  $path =~ s/^([a-zA-Z]:)?\\//; # Remove leading '\' and drive letter if present.
  foreach (split /\\/, $relativeTo) {
    $path = "..\\$path";
  }
  return $path;
}

sub AbsolutePath {
  my $path = TidyPath(shift);
  my $absoluteTo = TidyPath(shift);

  my @workingDir = ($path =~ /^\\/) ? () : split(/\\/, $absoluteTo);
  my @path = split(/\\/, $path);
	
  foreach my $pathBit (@path) {
    next if ($pathBit eq '.');
    if ($pathBit eq '..') {
      pop @workingDir;
      next;
    }
    push @workingDir, $pathBit;
  }

  return join('\\', @workingDir);
}

sub TidyPath {
  my $path = shift;
  $path =~ s/\//\\/g;        # Change forward slashes to back slashes.
  $path =~ s/\\\.\\/\\/g;    # Change "\.\" into "\".
  $path =~ s/\\$//;          # Removing trailing slash.

  if ($path =~ /^\\\\/) {    # Test for UNC paths.
    $path =~ s/\\\\/\\/g;    # Change "\\" into "\".
    $path =~ s/^\\/\\\\/;    # Add back a "\\" at the start so that it remains a UNC path.
  }
  else {
    $path =~ s/\\\\/\\/g;    # Change "\\" into "\".
  }

  # Remove leading ".\" if doing so doesn't empty the string.
  $path =~ s/^\.\\(.+)/$1/;

  # Collapse ".."s in the middle of the path.
  my $foundFirstDirName = 0;
  my @path = split(/\\/, $path);
  my @collapsedPath;
  foreach my $pathBit (@path) {
    if (not $foundFirstDirName) {
      if ($pathBit ne '..') {
	$foundFirstDirName = 1;
      }
      push (@collapsedPath, $pathBit);
    }
    else {
      if ($pathBit eq '..') {
	pop (@collapsedPath);
      }
      else {
	push (@collapsedPath, $pathBit);
      }
    }
  }
  $path = join('\\', @collapsedPath);

  if ($path =~ m|^\\epoc32|) {
    # If it starts with \epoc32, chop the leading backslash and stick on $EPOCROOT
    $path =~ s|^\\|$ENV{EPOCROOT}|;
  }

  return $path;
}

sub MakePath ($) {
  my $dir = shift;
  $dir =~ s/\//\\/g; # Convert all forward slashes to back slashes in path.
  unless (-e $dir) {
    if ($dir =~ /^\\\\/) {
      # This is a UNC path - make path manually because UNC isn't supported by mkpath.
      my $dirToMake = '';
      my @dirs = split /\\/, $dir;
      shift @dirs;  # Get rid of undefined dir.
      shift @dirs;  # Get rid of undefined dir.
      my $server = shift @dirs;
      my $share = shift @dirs;
      $dirToMake .= "\\\\$server\\$share";
      unless (-e $dirToMake) {
	die "Network share \"$dirToMake\" does not exist\n";
      }
      foreach my $thisDir (@dirs) {
	$dirToMake .=  "\\$thisDir";
	unless (-e $dirToMake) {
	  mkdir($dirToMake,0) or die "Couldn't make directory $dirToMake: $!\n";
	}
      }
    }
    else {
      mkpath($dir) or die "Couldn't make path \"$dir\": $!\n";
    }
  }
  if ($dir =~ m|^\\epoc32|) {
    # If it starts with \epoc32, chop the leading backslash and stick on $EPOCROOT
    $dir =~ s|^\\|$ENV{EPOCROOT}|;
  }
}

sub CopyFile {
  my $from = TidyPath(shift);
  my $to = TidyPath(shift);
  my $verbose = shift;

  MakePath(dirname($to));
  print "Copying '$from' to '$to'...\n" if $verbose;
  copy ($from, $to) or die "Error: Couldn't copy '$from' to '$to' - $!\n";
}

sub Version {
  my $version = 'Unknown';
  my $kChangeHistoryFileName = "../../documentation/change_history.pod";
  open (HISTORY, $kChangeHistoryFileName) or die "Error: Couldn't open \"$kChangeHistoryFileName\" for reading: $!\n";
  while (my $line = <HISTORY>) {
    if ($line =~ /(Release \d+.*)/i) {
      $version = $1;
      last;
    }
  }
  close (HISTORY);
  return $version;
}

1;

__END__

=head1 NAME

fshu.pm - A collection of common utility sub-routines used by other fshell scripts.

=head1 COPYRIGHT

Copyright (c) 2007-2010 Accenture. All rights reserved.

=cut
