# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# General Path and File Utility Functions for use with Makmake
# Distinguish paths from filepaths by assuming paths end with "\"
# therefore ensure this is the case for all paths coming into programs using this module
# 
#

package Pathutl;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	Path_SetVerbose Path_Drive Path_WorkPath Path_RltToWork Path_AbsToWork
	Path_DelFiles Path_Split Path_Dirs Path_StepDirs Path_Strip 
	Path_MakePathL Path_UpToRoot Path_MakeRlt Path_MakeAbs Path_Chop
	Path_MakeEAbs Path_Quote Path_MakeRltToBase Path_Norm Path_PrefixWithDrive Path_PrefixWithDriveAndQuote
);

use strict;
use Cwd;
use File::Path;                # for mkpath

my %Mode=(
	Verbose=>0
);
my $Drive;
my $WorkPath;
my @WorkPathList;

sub Path_SetVerbose () {
	$Mode{Verbose}=1;
}

sub Path_Drive () {
# return the current drive - programs shouldn't change directory if using this module
	$Drive;
}

sub Path_WorkPath () {
# return the current working directory - programs shouldn't change directory if using this module
	$WorkPath;
}

sub helper_MakeRlt ($@) {
# helper function for computing relative path(s) given a base path
	my ($BaseRef,@List)=@_;
	foreach my $p (@List) {
		my $filename=&Path_Split('File',$p);
		my @plist=&Path_Dirs($p);
		my $upcount=scalar @{$BaseRef};
		foreach (@{$BaseRef}) {
			if (uc $_ ne uc $plist[0]) {
				last;
			}
			$upcount -= 1;
			shift @plist;
		}
		$p="";
		while ($upcount-->0) {
			$p .= "..\\";
		}
		foreach (@plist) {
			$p .= "$_\\";
		}
		$p=".\\" if ($p eq "");		# ensure a well-formed result if path == work
		$p .= $filename;
	}
	return wantarray ? @List : $List[0];	
}

sub Path_RltToWork (@) {
# make a path or list of paths relative to the current working directory
	my @List=@_;
	@List=&helper_MakeRlt(\@WorkPathList,@List);
	return wantarray ? @List : $List[0];
}

sub Path_MakeRltToBase ($@) {	#args: $_[0] Base $_[1]... list of (Abs FilePath/Path)
# make a path, or list of paths, relative to a particular directory specified by the first
# path passed into the function
	return undef unless $_[0]=~m-(|\\$)-o;	# allow for null value passed in
	my ($Base,@List)=@_;
	my @BasePathList=&Path_Dirs($Base);
	@List=&helper_MakeRlt(\@BasePathList,@List);
	return wantarray ? @List : $List[0];	
}

sub Path_AbsToWork (@) {
# make a path or list of paths relative to the current working directory absolute
	my @List=@_;
	@List=&Path_MakeAbs($WorkPath,@List);
	return wantarray ? @List : $List[0];	
}

sub Path_DelFiles (@) {
# delete a list of files
	my @List=@_;
	my $File;
	foreach $File (@List) {
		if (unlink $File) {
			if ($Mode{Verbose}) {
				print "Deleted File: \"$File\"\n";
			}
			next;
		}
		if ($Mode{Verbose}) {
			print "Not Found: \"$File\"\n";
		}
	}
}

sub Path_Split ($$) {	#args: $_[0] 'Path' or 'Base' or 'Ext' $_[1] Abs/Rel FilePath/Path
# return the section of a file path required - Path, Base, Ext or File
	my ($Sect,$P)=@_;

	return '' if !$P;
	
	$Sect= ucfirst lc $Sect;
	if ($Sect eq 'Path') {
		if ($P=~/^(.*\\)/o) {
			return $1;
		}
		return '';
	}
	if ($Sect eq 'Base') {
		if ($P=~/\\?([^\\]*?)(\.[^\\\.]*)?$/o) {
			return $1;
		}
		return '';
	}
	if ($Sect eq 'Ext') {
		if ($P=~/(\.[^\\\.]*)$/o) {
			return $1;
		}
		return '';
	}
	if ($Sect eq 'File') {
		if ($P=~/([^\\]*)$/o) {
			return $1;
		}
		return '';
	}
	undef;
}

sub Path_Dirs ($) {	#args: $_[0] Abs FilePath/Path
# return an ordered list of individual directories that make up a path
	return undef unless $_[0]=~m-^\\-o;
	my $P=&Path_Split('Path',$_[0]);
	return undef unless $P=~s-^(.*)\\$-$1-o;
	$P=~s-^\\(.*)-$1-o;
	split /\\/,$P;
}

sub Path_StepDirs ($) { #args: $_[0] Abs FilePath/Path
# return an ordered list of paths - starting with the directory in the root directory from the
# path passed into the function, each subsequent path contains the next directory from the path
# passed into the function, and the last path is the same as the path passed into the function
	return undef unless $_[0]=~m-^\\-o;
	my $P=$_[0];
	my @Dirs=&Path_Dirs($P);
	my @StepDirs;
	my $dir;
	my $stepDir="\\";
	foreach $dir (@Dirs) {
		$stepDir.="$dir\\";
		push @StepDirs, $stepDir;
	}
	@StepDirs;
}

sub Path_Strip ($) {	#args: $_[0] Abs FilePath/Path
# Remove excess occurrences of '..' and '.' from a path
	return undef unless $_[0]=~m-^\\-o;
	my $P=$_[0];
	while ($P=~s-\\\.\\-\\-go) { }
	while ($P=~s-\\(?!\.{2}\\)[^\\]*\\\.{2}(?=\\)--go) { }
	$P;
}

sub Path_MakePathL (@) {	#args: @_ list of Abs FilePath/Path
# make a directory or list of directories
	my @Paths=@_;
	my $P;
	foreach $P (@Paths) { 
		return undef unless $P=~m-^\\-o;
		$P=&Path_Split('Path',$P);
		$P=&Path_Strip($P);
		$P=~m-(.*)\\-o;
		if (-d $1) {
			if ($Mode{'Verbose'}) {
				print "Existing Path: \"$P\"\n";
			}
			next;
		}
		mkpath[$P];
		if ($Mode{'Verbose'}) {
			print "Created Path: \"$P\"\n";
		}
	}
	return wantarray ? @Paths : $Paths[0];
}

sub Path_UpToRoot ($) {	#args: $_[0] Abs FilePath/Path
# return the path that will lead from the directory the path passed into the function
# specifies back up to the root directory
	return undef unless $_[0]=~m-^\\-o;
	my $Path=$_[0];
	my $UpP;
	while ($Path=~m-\\-go) {
		$UpP.="..\\";
	}
	undef $Path;
	$UpP=~s-^(.*)\.\.\\-$1-o;
	$UpP=".\\" unless $UpP;
}

sub Path_MakeRlt ($@) {	#args: $_[0] Start UpPath $_[1]... list of (Abs FilePath/Path)
# make a path, or list of paths, relative to a particular directory specified by the first
# path passed into the function which leads upwards from a particular directory
	return undef unless $_[0]=~m-(|\\$)-o;	# allow for null value passed in
	my ($UpPath,@List)=@_;
	my $p;
	foreach $p (@List) {
		return undef unless $p=~m-^\\-o;
		$p=~s-^\\(.*)$-$1-o;
		$p=$UpPath.$p;
	}
	return wantarray ? @List : $List[0];	
}

sub Path_MakeAbs ($@) {	#args: $_[0] Start Abs FilePath/Path $_[1]... list of (Abs/Rel FilePath/Path)
# make a path, or list of paths, absolute given the directory specified by the first path
# passed into the function which the other paths passed into the function are assumed to be
# relative to
	return undef unless $_[0]=~m-^\\-o;
	my ($Path,@List)=@_;
	my $BasePath=&Path_Split("Path",$Path);
	undef $Path;
	my $p;
	foreach $p (@List) {
		if ($p=~m-^\.{2}-o) {
			$p=&Path_Strip($BasePath.$p);
			next;
		}
		if ($p=~m-^[^\.\\]-o) {
			$p=&Path_Strip($BasePath.$p);
			next;
		}
		if ($p=~m-^\\-o) {
			$p=&Path_Strip($p);
			next;
		}
		if ($p=~m-^\.\\(.*)$-o) {
			$p=&Path_Strip($BasePath.$1);
			next;
		}
		return undef;
	}
	return wantarray ? @List : $List[0];
}

sub Path_MakeEAbs ($@) {	#args: $_[0] Start EPOCPath Abs FilePath/Path $_[1]... list of (Abs/Rel FilePath/Path)
# Variant of MakAbs which also maps "+\\" to "${EPOCPath}"
	return undef unless $_[0]=~m-^\\-o;
	my ($EPOCPath,$Path,@List)=@_;
	my $BasePath=&Path_Split("Path",$Path);
	undef $Path;
	my $p;
	foreach $p (@List) {
		if ($p=~m-^\\epoc32\\(.*)$-io) {	# change - special case for existing \\epoc32 references
			$p=$EPOCPath.$1;
			next;
		}
		if ($p=~m-^\s*\+\\(.*)$-o) {
			$p=$EPOCPath.$1;
			next;
		}
		if ($p=~m-^\.{2}-o) {
			$p=&Path_Strip($BasePath.$p);
			next;
		}
		if ($p=~m-^[^\.\\]-o) {
			$p=$BasePath.$p;
			next;
		}
		if ($p=~m-^\\-o) {
			next;
		}
		if ($p=~m-^\.\\(.*)$-o) {
			$p=&Path_Strip($BasePath.$1);
			next;
		}
		return undef;
	}
	return wantarray ? @List : $List[0];
}

sub Path_Chop (@) {
# remove the terminating backslash from a path, or list of paths, if there is one
	my @List=@_;
	my $Path;
	foreach $Path (@List) {
		$Path=~s-^(.*)\\$-$1-o;
	}
	return wantarray ? @List : $List[0];
}

sub Path_Quote ($) {
# Quote name for use in GNU makefiles
	my @List=@_;
	my $Path;
	foreach $Path (@List) {
		$Path=~s- -\\ -go if (defined($Path));
	}
	return wantarray ? @List : $List[0];
}

sub Path_Norm ($) {
# Normalise source specified paths for processing
	my ($Path) = @_;
	$Path =~ s/\//\\/g;
	return $Path;
}

sub Path_PrefixWithDrive ($) {
# Take a path, or list of paths, and prefix with drive based on CWD.
# Relative paths are just returned.
	my @List=@_;
	my $Path;
	my $Drive=$1 if (cwd =~ /^(.:)/); 

	foreach $Path (@List) {
		next if ($Path !~ /^\\/);
		$Path=$Drive.$Path;
	}
	
	return wantarray ? @List : $List[0];
}

sub Path_PrefixWithDriveAndQuote ($) {
# Take a path, or list of paths, and prefix with drive based on CWD.
# Relative paths are just quoted.
	my @List=@_;
	my $Path;
	my $Drive=$1 if (cwd =~ /^(.:)/); 

	foreach $Path (@List) {
		next if ($Path !~ /^\\/);
		$Path=$Drive.$Path;
	}

	foreach $Path (@List) {
		$Path="\"".$Path."\"";
	}
	
	return wantarray ? @List : $List[0];
}



BEGIN {
# get the current working directory
	$WorkPath=cwd;
	$WorkPath=~s-/-\\-go; # separator from Perl 5.005_02+ is forward slash
	$WorkPath=~s/^(.:)//o;    # remove drive letter
	$Drive=$1;
	$WorkPath=~s-^(.*[^\\])$-$1\\-o;        # ensure workpath ends with a backslash
	@WorkPathList=&Path_Dirs($WorkPath);
}

1;
