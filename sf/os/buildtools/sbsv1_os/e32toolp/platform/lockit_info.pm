# Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package lockit_info;
use File::Copy;
use File::Path;
use File::Basename;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	Check_Epocroot
	Setup_LockitPath
	Lockit_Releasables
	Copy_Files
	WriteTo_Info
	LineExists
	Lockit_SrcFile
	Open_InfoFile
	Close_InfoFile
);

use strict;
use Pathutl;
my $epocroot;
my $epocPath;

sub Check_Epocroot 
	{
	$epocroot = $ENV{EPOCROOT};
	die "ERROR: Must set the EPOCROOT environment variable\n" if (!defined($epocroot));
	$epocroot =~ s-/-\\-go;	# for those working with UNIX shells
	die "ERROR: EPOCROOT must not include a drive letter\n" if ($epocroot =~ /^.:/);
	die "ERROR: EPOCROOT must be an absolute path without a drive letter\n" if ($epocroot !~ /^\\/);
	die "ERROR: EPOCROOT must not be a UNC path\n" if ($epocroot =~ /^\\\\/);
	die "ERROR: EPOCROOT must end with a backslash\n" if ($epocroot !~ /\\$/);
	die "ERROR: EPOCROOT must specify an existing directory\n" if (!-d $epocroot);

	$epocroot=~ s-\\$--;		# chop trailing \\
	$epocPath = "$epocroot\\epoc32\\localisation";
	}


## main function which handles all lockit-related function
sub Lockit_SrcFile()
{
	my ($ResrcFile, $RppFile,$Resrc_Options, $FileType, $Bitmaps, $RscTarget, $lang);
	
	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
		($ResrcFile, $RppFile,$Resrc_Options, $FileType, $Bitmaps, $RscTarget, $lang)= @_;
	}
	else {

		($ResrcFile, $RppFile,$Resrc_Options, $FileType, $Bitmaps, $RscTarget)= @_;
	}
	$RscTarget=Path_Split('File', $RscTarget);
	my ($Tgtdir, $CWDir) = split(/:/, $Resrc_Options);
	$Tgtdir =~s-^(.*)\\$-$1-o; # remove terminating backslash
	$Tgtdir=~s-^\\--o; # remove leading backslash
	
	Check_Epocroot();
	my $FilePath;

	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
		$FilePath = Setup_LockitPath($CWDir, $ResrcFile, $FileType, $lang);

		# update rppfile in epoc32\localisation if -l option specified and generating resource
			
		copy("$RppFile", "$FilePath\\");
	}
	else {
		$FilePath = Setup_LockitPath($CWDir, $ResrcFile, $FileType);

		# update rppfile in epoc32\localisation if -l option specified and generating resource
		copy("$RppFile", "$FilePath\\$ResrcFile.rpp");
	}

	# update bitmaps in epoc32\localisation if -l option and bitmaps specified
	if ($CWDir && ($Bitmaps ne ""))
	{
		my $BmpRef;
		$Bitmaps =~ s/ +|\t/ /g; # replace tabs and more spaces with single space
		$Bitmaps =~s/^ //g; # remove leading space
		$Bitmaps =~s/ $//g; # remove trailing space

		my (@AifBitmaps) = split(/ /, $Bitmaps);
		foreach $BmpRef (@AifBitmaps) {
			$BmpRef =~ /^([^\\]+)(\\.*)$/;
			my $CDepth = $1;
			my $bmp = $2;
			Copy_Files($bmp, $FilePath);
			my $file_base= basename($bmp);
			chmod (0666,"$FilePath\\$file_base");
		}
	}

	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
		if (LineExists($ResrcFile.$lang, $RscTarget) == 0)
		{
			Open_InfoFile($ResrcFile . $lang);

			WriteTo_Info ( "\n\\$Tgtdir\\$RscTarget :" );

			if($FileType =~ /RSC/i && $RppFile ne "") {
				WriteTo_Info (
					 " $ResrcFile$lang.rpp"
				)
			}

			elsif ($RppFile ne "") {
				WriteTo_Info (
					 " $ResrcFile$lang.text $ResrcFile$lang.struct"
				)
			}

			if($Bitmaps ne "")
			{
				my $BmpRef;
				my (@AifBitmaps) = split(/ /, $Bitmaps);
				foreach $BmpRef (@AifBitmaps) {
					$BmpRef =~ /^([^\\]+)(\\.*)$/;
					my $CDepth = $1;
					my $bmp = $2;
					my $file_base= basename($bmp);
					WriteTo_Info (
						 " $CDepth\\$file_base"
					);
				}
			}

			Close_InfoFile($ResrcFile . $lang);
		}
	}
	else {
		if (LineExists($ResrcFile, $RscTarget) == 0)
		{
			Open_InfoFile($ResrcFile);

			WriteTo_Info ( "\n\\$Tgtdir\\$RscTarget :" );

			if($FileType =~ /RSC/i && $RppFile ne "") {
				WriteTo_Info (
					 " $ResrcFile.rpp"
				)
			}

			elsif ($RppFile ne "") {
				WriteTo_Info (
					 " $ResrcFile.text $ResrcFile.struct"
				)
			}

			if($Bitmaps ne "")
			{
				my $BmpRef;
				my (@AifBitmaps) = split(/ /, $Bitmaps);
				foreach $BmpRef (@AifBitmaps) {
					$BmpRef =~ /^([^\\]+)(\\.*)$/;
					my $CDepth = $1;
					my $bmp = $2;
					my $file_base= basename($bmp);
					WriteTo_Info (
						 " $CDepth\\$file_base"
					);
				}
			}

			Close_InfoFile($ResrcFile);

		}
	}
}


## copy source files
sub Copy_Files ()
	{
	my ($SourceName, $TargetName) = @_;

	copy($SourceName, $TargetName);
	}

## create necessary directories
sub Create_InfoFile ()
	{
	my ($CreateLockitPath, $CreateInfoFile, $Datadir) = @_;
	if ( !-e "$CreateLockitPath") { mkpath($CreateLockitPath); }
	if ( !-e "$epocPath\\group") { mkpath("$epocPath\\group"); }
	if ( !-e "$epocPath\\group\\$CreateInfoFile.info") {	
		open INFO,">$epocPath\\group\\$CreateInfoFile.info" or die "ERROR: Can not create file \"$CreateInfoFile\"\n"; 
		print INFO "DATADIR: $Datadir\n";
		close INFO;
	}
}

## open INFO file to write source information
sub Open_InfoFile ($)
	{
	my $FileToOpen = $_[0];
	open INFO,">>$epocPath\\group\\$FileToOpen.INFO" or die "ERROR: Can not open  \"$epocPath\\group\\$FileToOpen\"\n";
	}


## write source filename to INFO file
sub WriteTo_Info ($) 
	{
	my $Text = $_[0];
	print INFO "$Text";
	}


## check whether source filename is written to INFO file
sub LineExists ()
	{
	my $FileToOpen = $_[0];
	my $LineCheck = $_[1];
	my $exists = 0;

	open INFO,"$epocPath\\group\\$FileToOpen.info" or die "ERROR: Can not open \"$FileToOpen\"\n";
		while(<INFO>) {
			if (/$LineCheck/i) { $exists = 1; return $exists; } 
		}
		close INFO;
	return $exists;
	}
	

#determine Components pathname and create INFO files
sub Setup_LockitPath
	{
	my ($ComponentSrcPath, $Resrc, $FileType, $lang);

	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
		($ComponentSrcPath, $Resrc, $FileType, $lang) = @_;
	}
	else {
		($ComponentSrcPath, $Resrc, $FileType) = @_;
	}
	my ($temp, $CWDir) = split(/\\/, $ComponentSrcPath);
	my $FilePath = $epocPath;

	if($FileType =~ /RSS/i) { $FileType = "rsc"; }
	if($FileType =~ /^acl|abw|aif|a[0-9]/i) {
		$FilePath .= "\\aif";
	}
	
## change added to support cnf file generation
	if ($FileType =~ /CNF/i) { 
		$FilePath .= "\\cnf"; 
	}

	my $WorkPath = ""; 

	if($Resrc !~ /^$CWDir$/i) {
		$WorkPath = "$Resrc"; 
	}

	$FilePath .= "\\$WorkPath\\$FileType";

	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
		&Create_InfoFile("$FilePath", "$Resrc$lang", "\\$WorkPath");
	}
	else {
		&Create_InfoFile("$FilePath", "$Resrc", "\\$WorkPath");
	}

	return $FilePath;
	}


## accumulate list of Lockit releasables
sub Lockit_Releasables
{
	my ($ComponentSrcPath, $ResrcFile, $href, $Bitmaps, $lang);
	my $Resrc;
	my ($BaseResrc,$FileType);
	my $LockitInfoPath;
	my $LockitPath;
	my %Files;

	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
		($ComponentSrcPath, $ResrcFile, $href, $Bitmaps, $lang) = @_;

		$Resrc = basename($ResrcFile);
		($BaseResrc,$FileType) = split(/\./, $Resrc);
		Check_Epocroot();
	
		$LockitInfoPath = "$epocPath\\group\\$BaseResrc$lang.info";

		$LockitPath = Setup_LockitPath($ComponentSrcPath, $BaseResrc, $FileType, "");
		%Files = %$href;

		if($FileType =~ /RSS|ACL|ABW|AIF|A[0-9]/i && $Bitmaps eq ""){
			$LockitPath .= "\\$BaseResrc$lang.rpp";
		}
		else {
			$LockitPath .= "\\$Bitmaps";
		}

	}
	else {
		($ComponentSrcPath, $ResrcFile, $href, $Bitmaps) = @_;

		$Resrc = basename($ResrcFile);
		($BaseResrc,$FileType) = split(/\./, $Resrc);
		Check_Epocroot();
		$LockitInfoPath = "$epocPath\\group\\$BaseResrc.info";

		$LockitPath = Setup_LockitPath($ComponentSrcPath, $BaseResrc, $FileType);
		%Files = %$href;

		if($FileType =~ /RSS|ACL|ABW|AIF|A[0-9]/i && $Bitmaps eq ""){
			$LockitPath .= "\\$BaseResrc.rpp";
		}
		else {
			$LockitPath .= "\\$Bitmaps";
		}
	}

	my %loggedFiles;
	$loggedFiles{lc($_)} = 1 foreach keys (%Files);
	
	$Files{$LockitPath} = 1 if !($loggedFiles{lc($LockitPath)});
	$Files{$LockitInfoPath} = 1 if !($loggedFiles{lc($LockitInfoPath)});
	
	return %Files;
}

sub Close_InfoFile ($)
	{
	my $FileToClose = $_[0];
	close INFO;
	}

1;
