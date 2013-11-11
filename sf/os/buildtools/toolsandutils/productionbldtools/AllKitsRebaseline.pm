#!perl

# AllKitsRebaseline.pm

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
# Version Info:
# Run with the substed build drive as the current directory.
# e.g.	subst m: <mapped drive to build machine>/04640/output> 
# m:
# <location of tool>\AllKitsRebaseline.pl <absolute target directory of new packages>  <codeline-tree>
# 
#


use Win32::Registry;


package AllKitsRebaseline;

use Win32::Process;
use Win32::Registry;
use File::Path;
use File::Copy;

use KitStandardLocations;
use ZipLog2Pkg;



# ------------------------------------- Global variables -----------------------------------

# Drive/directory for new packages
my $SaveToDir  = "";
my $iprTool    = "$SourceDir\\os\\buildtools\\bldsystemtools\\commonbldutils\\iprtool.pl";
my $commonToolsDirectory = "";


# --------------------------------- Start of CreatePackages() ----------------------------------
#
#
sub CreateFileLists()
{

	# main array: within each entry is:
	#		file name
	#		IPR category to filter on
	#		directory to filter on
        #		a flag to indicate if warnings should be ignored if the directory does not exist
	#		additional flags for CreateProcess call
	my @RequiredLists =
	(
		# common generic
		["GT-common-A", 	"A", 	"$SourceDir\\common\\generic"],	
		["GT-common-B", 	"B", 	"$SourceDir\\common\\generic"],	
		["GT-common-I", 	"I", 	"$SourceDir\\common\\generic"],
		["GT-common-O", 	"O", 	"$SourceDir\\common\\generic"],
		["GT-common-C", 	"C", 	"$SourceDir\\common\\generic"],	
		["GT-common-DET", 	"DET", 	"$SourceDir\\common\\generic", "", "-e"],	
		["GT-common-FG", 	"FG", 	"$SourceDir\\common\\generic"],	

		# tree generic
		["GT-$Platform-A", 		"A", 	"$SourceDir\\$Platform\\generic"],	
		["GT-$Platform-B", 		"B", 	"$SourceDir\\$Platform\\generic"],
		["GT-$Platform-I", 		"I", 	"$SourceDir\\$Platform\\generic"],
		["GT-$Platform-O", 		"O", 	"$SourceDir\\$Platform\\generic"],
		["GT-$Platform-C", 		"C", 	"$SourceDir\\$Platform\\generic"],	
		["GT-$Platform-DET", 	"DET", 	"$SourceDir\\$Platform\\generic", "", "-e"],	
		["GT-$Platform-FG", 	"FG", 	"$SourceDir\\$Platform\\generic"],	

		# common UI
		["UI-common-A", 	"A", 	"$SourceDir\\common\\techview"],	
		["UI-common-B", 	"B", 	"$SourceDir\\common\\techview"],
		["UI-common-I", 	"I", 	"$SourceDir\\common\\techview"],	
		["UI-common-O", 	"O", 	"$SourceDir\\common\\techview"],	
		["UI-common-C", 	"C", 	"$SourceDir\\common\\techview"],	
		["UI-common-DET", 	"DET", 	"$SourceDir\\common\\techview"],	
		["UI-common-FG", 	"FG", 	"$SourceDir\\common\\techview"],	

		# tree UI
		["UI-$Platform-A", 	"A", 	"$SourceDir\\$Platform\\techview", "nowarn"],	
		["UI-$Platform-B", 	"B", 	"$SourceDir\\$Platform\\techview", "nowarn"],
		["UI-$Platform-I", 	"I", 	"$SourceDir\\$Platform\\techview", "nowarn"],	
		["UI-$Platform-O", 	"O", 	"$SourceDir\\$Platform\\techview", "nowarn"],	
		["UI-$Platform-C", 	"C", 	"$SourceDir\\$Platform\\techview", "nowarn"],	
		["UI-$Platform-DET", 	"DET", 	"$SourceDir\\$Platform\\techview", "nowarn"],	
		["UI-$Platform-FG", 	"FG", 	"$SourceDir\\$Platform\\techview", "nowarn"],
                
 		# testtools
		["TestTools",		"DEFGT",	"$SourceDir\\os\\unref\\orphan\\comtt"],	
 
   		# docs
		["Doc-DET", 		"DET", 	"$OutputDir\\documentation"],	
		["Doc-FGX",		"FGX", 	"$OutputDir\\documentation"],
		["Doc-common-DET", 	"DET", 	"$SourceDir\\common\\developerlibrary"],	
		["Doc-common-FGX",	"FGX", 	"$SourceDir\\common\\developerlibrary"],
		["Doc-common-system-FGX",	"FGX", 	"$SourceDir\\common\\system"],

		["Conn-common-Src",	"DEFGT",	"$SourceDir\\common\\epocconnect", "nowarn"],	
		["Conn-$Platform-Src",	"DEFGT",	"$SourceDir\\$Platform\\epocconnect", "nowarn"],	
		["Conn-Bin",		"DEFGXT",	"$OutputDir\\symbianconnect", "nowarn"],

		["TechViewExampleSDK",	"DEFGXT","$SourceDir\\$Platform\\product\\TechViewExampleSDK"]
	) ;




	sub ErrorReport
	{
		print "Error: ".Win32::FormatMessage( Win32::GetLastError() );
	}


	# get Perl bin directory
	if ( ( $main::HKEY_LOCAL_MACHINE->Open("SOFTWARE\\Perl", $RegObj) ) && ( $RegObj->QueryValueEx("bindir", my $Type, $Value) ) )
	{
		# fix problem where Perl 5.6's bindir registry variable contains the exe, but 5.005's doesn't; remove exe name
		$Value =~ s|\\perl\.exe$||i;
	}
	else
	{
		print "Unable to find Perl key\n";
		exit;
	}
	$RegObj->Close();


	# preserve STDOUT, STDERR
	open(OLDOUT, ">&STDOUT") or die $!; # backup filehandles 
	open(OLDERR, ">&STDERR") or die $!; 
	open(STDERR, ">NUL") or die $!; 


	# start the processes 
	my @Processes ;
	my $ProcessObj;
	foreach $entry (@RequiredLists)
	{
		my $FileName  = $entry->[0];
		my $Category  = $entry->[1];
		my $Directory = $entry->[2];
                my $noWarn    = $entry->[3];
		my $ExtraFlag = $entry->[4];
		
		if (-d "$Directory")
		{
			# direct STDOUT to output file
			open(STDOUT,">$SaveToDir\\$FileName");
			select(STDOUT);  # force a flush on stdout so that any output will be in sync.
		
			# create process and inherit STDOUT
			Win32::Process::Create($ProcessObj, "$Value\\perl.exe", "perl.exe ".$iprTool." -c $Category  -f -1  -s  $ExtraFlag", 1, NORMAL_PRIORITY_CLASS, "$Directory") || die ErrorReport();
			push @Processes, $ProcessObj;
		}
		else
		{
			print OLDOUT "INFO:  Directory not found: $Directory\n" if ($noWarn !~ /nowarn/i);
			open(FILE,">$SaveToDir\\$FileName");
			print FILE "\n";	# force all output files to exist, even if no content found by iprtool
			close FILE;
		}
	};
	

	
	# wait for them to complete
	while (scalar(@Processes) > 0)
	{
		$ProcessObj = pop @Processes;
		$ProcessObj->Wait(INFINITE);
		print ".";
	} print "\n";


	open(STDOUT, ">&OLDOUT") or die $!; # restore filehandle
	open(STDERR, ">&OLDERR") or die $!; 

	my $SourceDirWithoutDrive = $SourceDir;
	$SourceDirWithoutDrive =~ s/^\w://;			# remove initial drive letter
	$SourceDirWithoutDrive =~ s#\\#\\\\#g;		# ensure slash is escaped

	my $OutputDirWithoutDrive = $OutputDir;
	$OutputDirWithoutDrive =~ s/^\w://;			# remove initial drive letter
	$OutputDirWithoutDrive =~ s#\\#\\\\#g;		# ensure slash is escaped

	# remove IPR status info and the initial directory (\src, \bin etc.)

	foreach $entry (@RequiredLists)
	{
		my $FileName  = $entry->[0];

		# read in file
		if (open (FILE, "<$SaveToDir\\$FileName"))
		{
			my @file = <FILE>;
			close(FILE);

			unlink("$SaveToDir\\$FileName");

			if (open (FILE, ">$SaveToDir\\$FileName")) 
			{

				foreach $line (@file)
				{
					chomp($line);
					if ($line =~ m/^\s*([\\\/].+[^\/\\]$)/)
					{
						# remove whitespace at start of line
						$line =~ s/^\s*//i;	
						
						# if file name includes "common", replace preceeding part with ''
						$line =~ s/^$SourceDirWithoutDrive([\\\/]common[\\\/].*$)/$1/i;

						# if file name includes $Platform, replace preceeding part with ''
						$line =~ s/^$SourceDirWithoutDrive([\\\/]$Platform[\\\/].*$)/$1/i;

						# if file name includes bin\$Platform\<build_type>\epoc32, replace with 'epoc32'
						$line =~ s/^$OutputDirWithoutDrive[\\\/][^\\\/]*([\\\/]epoc32[\\\/].*$)/$1/i;
						
						# if file name includes bin\$Platform\, delete (for bin.connect, docs)
						$line =~ s/^$OutputDirWithoutDrive//i;

						# replace & with &amp;
						$line =~ s/&/&amp;/i;	
						
						print FILE "$line\n" ;
					}
				}
				close(FILE);
			}
			else
			{
				print "ERROR:  Couldn't create file $SaveToDir\\$FileName\n";
			}
		}
		else
		{
			print "ERROR:  Couldn't open file $SaveToDir\\$FileName\n";
		}
	}
	return 0;
	
}



# --------------------------------- Start of ProcessPackageDefinitions() ----------------------------------------

sub ProcessPackageDefinitions()
{

	my $PackageName = "com.symbian.src.package-definitions" ;

	my $header1 = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<packagedef version=\"1.0\">\n   <package name=\"";
	my $header2 = "\" major-version=\"0\" minor-version=\"0\">\n     <supplier>Symbian Ltd<\/supplier>\n     <sdk-version>7.0s<\/sdk-version>\n";
	my $depends = "     <dependencies>\n       <dependency name=\"$dependency\" major-version=\"0\" minor-version=\"0\" build-number=\"0\"\/>\n     <\/dependencies>\n";
	my $header3 = "   <\/package>\n\n   <manifest>\n";
	my $footer  = "   <\/manifest>\n<\/packagedef>";

	chdir ($commonToolsDirectory);  # ensure correct drive
	chdir $CustKitPath."\\".$PkgDefDir or die "Failed to change directory to ".$CustKitPath."\\".$PkgDefDir;
	
	# open pkgdef file here so it's included in it's own list!
	open (FILE, ">$PackageName.pkgdef") or die "Couldn't open file $PackageName.pkgdef";
	print FILE $header1.$PackageName.$header2.$header3;

	# CustKit packages first
	my @Packages = <com.symbian.*.pkgdef>; 
	foreach $package (@Packages)
	{
		print FILE "      <item src=\"\\$CustKitDir\\$PkgDefDir\\$package\" dest=\"[sdkroot]\\src\\$Platform\\$ProductDir\\$CustKitDir\\$PkgDefDir\\$package\"/>\n";
	}
	print FILE "      <item src=\"\\$CustKitDir\\$PkgDefDir\\distribution.policy\" dest=\"[sdkroot]\\src\\$Platform\\$ProductDir\\$CustKitDir\\$PkgDefDir\\distribution.policy\"/>\n" if (-e "$CustKitPath\\$PkgDefDir\\distribution.policy");
	# single srcdef file & IPR
	print FILE "      <item src=\"\\$CustKitDir\\$SrcDefDir\\CustKit.srcdef\" dest=\"[sdkroot]\\src\\$Platform\\$ProductDir\\$CustKitDir\\$SrcDefDir\\CustKit.srcdef\"/>\n";
	print FILE "      <item src=\"\\$CustKitDir\\$SrcDefDir\\distribution.policy\" dest=\"[sdkroot]\\src\\$Platform\\$ProductDir\\$CustKitDir\\$SrcDefDir\\distribution.policy\"/>\n" if (-e "$CustKitPath\\$SrcDefDir\\distribution.policy");
	
	
	# DevKit packages
	chdir $DevKitPath."\\".$PkgDefDir or die "Failed to change directory to $DevKitPath\\$PkgDefDir" ;
	@Packages = <com.symbian.*pkgdef>; 
	foreach $package (@Packages)
	{
		print FILE "      <item src=\"\\$DevKitDir\\$PkgDefDir\\$package\" dest=\"[sdkroot]\\src\\$Platform\\$ProductDir\\$DevKitDir\\$PkgDefDir\\$package\"/>\n";
	}
	print FILE "      <item src=\"\\$DevKitDir\\$PkgDefDir\\distribution.policy\" dest=\"[sdkroot]\\src\\$Platform\\$ProductDir\\$DevKitDir\\$PkgDefDir\\distribution.policy\"/>\n" if (-e "$DevKitPath\\$PkgDefDir\\distribution.policy");
	# single srcdef file & IPR
	print FILE "      <item src=\"\\$DevKitDir\\$SrcDefDir\\DevKit.srcdef\" dest=\"[sdkroot]\\src\\$Platform\\$ProductDir\\$DevKitDir\\$SrcDefDir\\DevKit.srcdef\"/>\n";
	print FILE "      <item src=\"\\$DevKitDir\\$SrcDefDir\\distribution.policy\" dest=\"[sdkroot]\\src\\$Platform\\$ProductDir\\$DevKitDir\\$SrcDefDir\\distribution.policy\"/>\n" if (-e "$DevKitPath\\$SrcDefDir\\distribution.policy");
	
	print FILE $footer;
	close(FILE);
}




# --------------------------------- Start of AllKitsRebaseline() ----------------------------------------

sub AllKitsRebaseline($)
{
	$SaveToDir = shift;

	# get initial directory so other tools/scripts in this directory can be found
	$commonToolsDirectory = $FindBin::Bin;
	my $platformToolsDirectory = $commonToolsDirectory;
	$platformToolsDirectory =~ s/common/$Platform/i;


	if ($SaveToDir eq "")
	{
		die "Parameter (SaveToDir) is required - please run with parameter\n";
	}
	
	# Create directory for saving temporary packages
	if (! -e $SaveToDir)
	{
		mkpath $SaveToDir or die "Unable to create directory $SaveToDir - $!";
	}


	# Create the categorised lists of files
	print "** CreateFileLists()\n";
	CreateFileLists();


	# prepare special file lists for static packages - assumes supplementary packages will build correctly
	open (FILE,">$SaveToDir\\static");
	my $OutputDirWithoutDrive = $OutputDir;
	$OutputDirWithoutDrive =~ s/^\w:[\/\\]//;			# remove initial drive letter and slash
	print FILE "$OutputDirWithoutDrive\\techview\\epoc32\\tools\\productinstaller\\License.txt\n" if (-e "$SourceDir\\common\\generic\\tools\\custkits\\ProductInstaller\\pkgdef\\License.txt");
	print FILE "lockit.zip\n" if (-e "$ZipDir\\lockit.zip");
	close(FILE);


	# copy package definition files from source code control system to build directories
	(mkpath $DevKitPath  or print "WARNING: Unable to create directory $DevKitPath - $!\n") if (! -d $DevKitPath);
	(mkpath $SuppKitPath or print "WARNING: Unable to create directory $SuppKitPath - $!\n") if (! -d $SuppKitPath);


	system ("xcopy $SourceDir\\$Platform\\$ProductDir\\$DevKitDir  $DevKitPath /E /Z /I /Q");
	system ("xcopy $SourceDir\\$Platform\\$ProductDir\\$SuppKitDir $SuppKitPath /E /Z /I /Q");


	# generate the package definition files
	chdir ("$SaveToDir") or print "Unable to change to directory $SaveToDir\n";
	print "** Source packages\n";
	ZipLog2Pkg::ZipLog2Pkg ("$platformToolsDirectory\\".$BuildBaseName."_src", "-batch");

	print "** Static packages\n";
	ZipLog2Pkg::ZipLog2Pkg ("$platformToolsDirectory\\".$BuildBaseName."_static", "-batch");

	# change to directory of new files and generate the non-source files
	chdir($FindBin::Bin) or print "Unable to change to directory $FindBin::Bin\n";

	print "** NonSourceRebaseline - generating logs\n";
	$ENV{"ProductDir"}=$ProductDir;		# required by exports.bat
	system ("call $commonToolsDirectory\\exports.bat");

	print "** NonSourceRebaseline - generating non source packages\n";
	ZipLog2Pkg::ZipLog2Pkg ("$platformToolsDirectory\\".$BuildBaseName."_bin", "-batch");


}


return 1;
