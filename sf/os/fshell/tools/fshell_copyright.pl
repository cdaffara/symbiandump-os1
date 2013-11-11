#!/usr/bin/perl
# fshell_copyright.pl
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
use File::Copy;
use File::Path;

my $debug = 0;

my $notice = << 'ENDTEXT';
// FILENAME
// 
// Copyright (c) COPYRIGHTYEAR Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
ENDTEXT

my @errorfiles = ();

sub update_copyright($$)
  {
  my ($file, $name) = @_;
  
  open FILE, "<$file" or print "ERROR: Cannot open $file: $!\n" and return "Cannot open";
  if ($debug) { print "Processing $file\n"; }
  my @lines = <FILE>;
  close FILE;
  
  my $start = 1;
  my $copyrightyear = "2010";
  my $updated = 0;
  my @newlines = ();
  
  my $commentstart = "//";
  if ($name =~ /\.(pl|pm|script|pod|ini|idf|esk|mk|cif|flm)/i || !($name =~ /\./))
	{
	# Need to use # instead of //
	# Second part of conditional is for perl scripts that don't specify any ending
	$commentstart = "#";
	}
  elsif ($name =~ /\.(bat)/)
	{
	$commentstart = '@REM';
	}
  elsif ($name =~ /\.(pkg)/)
	{
	$commentstart = ";";
	}
	
  while (my $line = shift @lines)
    { 
	if ($start == 1)
		{
		if ($line =~ /^$commentstart Copyright ?\(c\) [a-zA-Z .]*(20..|19..)/)
			{
			# Some evil things put Copyright Symbian Software Ltd 2009 rather than putting the date first. The [a-zA-Z] bit is to skip that without causing a longest prefix match to pick up the end date instead of the start
			$copyrightyear = $1;
			
			if ($line =~ /Nokia Corporation/)
				{
				# Leave well alone, not ours to change the headers on
				push @newlines, $line;
				$start = 0;
				}
			}
		elsif ($line =~ /^#!/)
			{
			# Leave shebang lines alone!
			push @newlines, $line;
			}
		elsif ($line =~ /^$commentstart/)
			{
			# Comment we're not interested in
			}
		else
			{
			# Reached the end of initial comment
			my $filecopyrightnotice = $notice;
			if ($copyrightyear ne "2010") { $copyrightyear = "$copyrightyear - 2010"; }
			$filecopyrightnotice =~ s/\/\//$commentstart/g; # Change '//' to '#' if needed
			$filecopyrightnotice =~ s/http:$commentstart/http:\/\//g; # I don't know why putting a ^ before the above pattern doesn't work, my brain is hurting
			$filecopyrightnotice =~ s/COPYRIGHTYEAR/$copyrightyear/;
			$filecopyrightnotice =~ s/FILENAME/$name/;

			push @newlines, $filecopyrightnotice;
			push @newlines, $line;
			$start = 0;
			}
		next;
		}
    push @newlines, $line;
    }

  #return if (!$updated);

 
  
  open NEWFILE, ">$file" or die("Cannot overwrite $file: $!\n");
  print NEWFILE @newlines, @lines;
  close NEWFILE or die("Failed to update $file: $!\n");
  print "* updated $file\n";
  }

# Process tree

sub scan_directory($)
  {
  my ($path) = @_;
  
  return if $path =~ /\.hg/ ; # Don't go inside mercurial dir

  opendir DIR, $path;
  my @files = grep !/^\.\.?$/, readdir DIR;
  closedir DIR;
  
  foreach my $file (@files)
    {
    my $newpath = "$path/$file";
    
    if (-d $newpath)
      {
      scan_directory($newpath);
      next;
      }
    next if (-B $newpath);  # ignore binary files
	next if ($newpath =~ /\.(pod|def|txt|src|index|clw|dsp|dsw|plg|rc|svg|css|esk)$/i); # ignore stuff that we're not copyrighting
    
    update_copyright($newpath, $file);
    }
  }

scan_directory(".");

printf "%d problem files\n", scalar @errorfiles;
print "\t", join("\n\t", @errorfiles), "\n";

