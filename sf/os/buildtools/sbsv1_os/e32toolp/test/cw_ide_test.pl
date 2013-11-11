#!perl
# Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Script to build specified MMP files with both command line and IDE, then
# use EVALID to compare the results.
# Inspired by "buildall.pl", written by Cuong Phan
# 
#

use strict;
use File::Basename;		# for fileparse()
use File::Path;			# for mkpath
use Cwd;				# for cwd
use OLE;
use Getopt::Long;

sub Usage ($) 
	{
	my ($message) = @_;
	print <<ENDHERESTRING;
$message

Usage : perl cw_ide_test.pl [-v] -f mmplist platform1 [platform2 ...]

Builds specified MMP files with both the command line and the CodeWarrior IDE,
storing the build products in zip files and comparing them with EVALID.
The output can be summarised using scanlog.pl.

All of the specified MMP files are build for all of the specified platforms.
If -v is specified, the detailed build commands are included in the output.

ENDHERESTRING

	exit 1;
	}

my %Options=();
GetOptions(\%Options, "v", "f=s");

&Usage("No platforms specified") if (@ARGV < 1);
&Usage("Must specify list of mmp files using -f") if (!defined($Options{"f"}));

my @mmplist=();
my $specifiedCWD;
open FILELIST, "<$Options{f}" or &Usage("Cannot open $Options{f}");
while (<FILELIST>)
	{
	$specifiedCWD = "";
		
	if (/#.*cwd:/i)
		{
		$specifiedCWD = $_;
		$specifiedCWD =~ s/^.*cwd:/cwd:/i;
		$specifiedCWD =~ s/\).*$//;
		$specifiedCWD =~ s/\s*//g;
		$specifiedCWD =~ s/\//\\/g;
		$specifiedCWD .="?";
		}
		
	s/#.*$//;		# remove comments
	s/\s*$//;		# remove trailing ws
	s/^\s*//;		# remove leading ws
	s/\//\\/g;		# convert / to \
	next if ($_ eq "");
	push @mmplist, $specifiedCWD.$_;
	}
close FILELIST;

&Usage("No MMP files?") if (@mmplist == 0);

# create an instance of CodeWarrior
my $CW = CreateObject OLE  "CodeWarrior.CodeWarriorApp";
if (!defined($CW))
	{
	print "Failed to start CodeWarrior\n";
	exit(1);
	}

# turn on autoflush, to get stdout in the right place...
# These runes come from perlfaq5

my $old_fh = select(STDOUT);
$| = 1;
select($old_fh);

foreach my $mmpfile (@mmplist)
	{
	$specifiedCWD = "";

	if ($mmpfile =~ /^cwd:/)
		{
		$specifiedCWD = $mmpfile;
		$specifiedCWD =~ s/^cwd://;
		$specifiedCWD =~ s/\?.*$//;
		$mmpfile =~ s/^cwd:.*\?//;
		}

	if (!-f $mmpfile)
		{
		print "MISSING: $mmpfile\n";
		next;
		}
	
	foreach my $platform (@ARGV)
		{
		$platform = uc $platform;
		my ($mmpname, $mmpdir, $mmpext) = fileparse($mmpfile,'\..*');
		my $phasename = $mmpname."_".$platform;
		
		my $origdir = cwd;

		if ($specifiedCWD)
			{
			print ("chdir $specifiedCWD\n");			
			chdir ($specifiedCWD);

			# Workaround for Base components, where GENEXEC.MKE needs to be executed to export the required headers for a build
			do_system ("bldmake bldfiles");
			do_system ("abld makefile $platform");
			}
		else
			{
			print ("chdir $mmpdir\n");
			chdir ($mmpdir);
			}

		my $time = localtime;
		print "===-------------------------------------------------\n";
		print "=== $phasename\n";
		print "===-------------------------------------------------\n";
		print "=== $phasename started $time\n";

		if ($specifiedCWD)
			{
			$mmpdir = "\\".$mmpdir;
			}
		else
			{
			$mmpdir = "";
			}

		&do_one_mmp_file($phasename, $mmpname, $platform, $mmpdir);

		$time = localtime;
		print "=== $phasename finished $time\n";

		chdir($origdir);
		print ("chdir $origdir\n");
		}
	}

$CW->Quit(0);
exit(0);

sub do_system($)
	{
	my ($cmd) = @_;
	print "    $cmd\n";
	return system($cmd);
	}

sub zip_and_check($$$)
	{
	my ($zipname, $releaseref, $complain) = @_;
	
	unlink($zipname) if (-f $zipname);
	
	my @ziplist;
	foreach (sort keys %$releaseref)
		{
		if (-f $_)
			{
			push @ziplist,$_;	# add to zip archive
			}
		else
			{
			print "MISSING: $_\n" if ($complain);
			}
		}
	
	if (scalar @ziplist ==0 && $complain)
		{
		print "Can't create empty archive $zipname\n";
		return;
		}
	
	# Make the non-empty archive
	
	open ZIP, "| zip -q $zipname -@";
	print ZIP
	 join("\n",@ziplist,"");
	close ZIP;
	print "Created $zipname\n";
	}
	
sub do_one_mmp_file($$$)
	{
	my ($phasename, $mmpname, $platform, $mmpdir) = @_;

	print "=== $phasename == $mmpname.mmp\n";

	# use MAKMAKE to generate the makefile
	# make CLEAN to remove any existing build results
	# make ALL to build everything
	# make WHAT to get the releaseables, as per abld.pl
	# check the releaseables and zip up the ones which do exist
	# make CLEAN again to get ready for the IDE build...
		
	my $makefile = "$mmpname.$platform";
	my %allreleaseables=();		# complete list, for putting back afterwards
	my %releaseables=();		# just things we expect the IDE to build
	my %uncheckedreleaseables=();
	my $makecmd = "make -s";
	$makecmd = "make" if ($Options{"v"});
	
	unlink $makefile if (-f $makefile);
	&do_system("perl -S makmake.pl $mmpdir$mmpname $platform");
	return if (!-f $makefile);
	
	open PIPE,"make -s -r -f $makefile WHAT |";
	while (<PIPE>) 
		{
		next if (/Nothing to be done for \S+\.$/o);
#		releasables split on whitespace - quotes possible -stripped out
		while (/("([^"\t\n\r\f]+)"|([^ "\t\n\r\f]+))/go) 
			{
			my $file = ($2 ? $2 : $3);
			$allreleaseables{$file}=1;
			next if ($file =~ /epoc32\\localisation\\/i);
			next if ($file =~ /epoc32\\data/i && $platform =~ /winscw/i);
			$releaseables{$file}=1;
			}
		}
	close PIPE;
	&zip_and_check("$mmpname.orig.$platform.zip", \%allreleaseables, 0);
	
	&do_system("$makecmd -r -f $makefile CLEAN");
	&do_system("$makecmd -r -f $makefile ALL");
	&zip_and_check("$mmpname.cmd.$platform.zip", \%releaseables, 1);

	&do_system("$makecmd -r -f $makefile CLEAN");
	
	print "=== $phasename == $mmpname.mcp\n";

	# Remove the remnants of previous projects
	# use MAKMAKE to generate the importable .xml
	# import the xml to create the .mcp
	# build the relevant targets
	# extract the contents of the "Errors & Warnings" window
	# check against the commandline list of releasables, zip up the ones which exist
	
	my $currentdir = cwd;
	$currentdir =~ s/\//\\/g;
	my $xmlfile = "$currentdir\\$mmpname.xml";
	my $mcpfile = "$currentdir\\$mmpname.mcp";
	my $mcpdata = "$currentdir\\${mmpname}_Data";
	
	&do_system("rmdir /s/q $mcpdata") if (-d $mcpdata);
	unlink $mcpfile if (-f $mcpfile);
	unlink $xmlfile if (-f $xmlfile);

	&do_system("perl -S makmake.pl $mmpdir$mmpname CW_IDE:$platform");
	if (-f $xmlfile)
		{
		&fixup_XML($xmlfile) if ($Options{"v"});
		my $project = $CW->ImportProject($xmlfile, $mcpfile, 'true');
		if (defined($project))
			{
			my $success = &BuildTargets($project->Targets,$platform);
			$project->Close();
			return if (!$success);
			&zip_and_check("$mmpname.ide.$platform.zip", \%releaseables, 1);
			}
		else
			{
			print "ERROR: failed to import $xmlfile\n";
			return;
			}
		}
	else
		{
		print "ERROR: failed to create $xmlfile\n";
		return;
		}

	print "=== $phasename == $mmpname.evalid\n";

	# remove & recreate a clean temporary directory for unzipping
	# unzip the saved results of the cmd and ide builds
	# use EVALID to compare the releaseables
		
	my $evaliddir = "c:\\temp\\evalid";
	&do_system("rmdir /s/q $evaliddir") if (-d $evaliddir);
	mkpath([$evaliddir]);
	
	if (   &do_system("unzip -q $mmpname.cmd.$platform.zip -d $evaliddir\\cmd")==0
		&& &do_system("unzip -q $mmpname.ide.$platform.zip -d $evaliddir\\ide")==0)
		{
		open EVALID,"perl -S evalid.pl $evaliddir\\ide $evaliddir\\cmd -c |";
		while (<EVALID>)
			{
			print $_ if (/^(PROBLEM|Failed)/);
			print $_ if (/^OK/ && $Options{"v"});
			if (/^FAILED/)
				{
				if (/\.map\t/i)
					{
					print "WARNING(S): $_";
					}
				else
					{
					print "FATAL ERROR(S): $_";
					}
				}
			}
		close EVALID;
		}
	else
		{
		print "FATAL ERROR(S): problems unpacking zip files\n";
		}
	&do_system("rmdir /s/q $evaliddir") if (-d $evaliddir);
	
	# Restore original files, if any
	if (-e "$mmpname.orig.$platform.zip")
		{
		&do_system("unzip -q -o $mmpname.orig.$platform.zip -d \\");
		}
	}

sub fixup_XML($)
	{
	my ($xmlfile) = @_;
	open XML,"<$xmlfile" or return;
	my @lines = <XML>;
	close XML;
	
	foreach (@lines)
		{
		# Insist that build commands are logged to the output window, irrespective of CW version
		s-<SETTING><NAME>LogMessages</NAME><VALUE>false</VALUE></SETTING>-<SETTING><NAME>LogMessages</NAME><VALUE>true</VALUE></SETTING>-;
		s-<SETTING><NAME>ShowCommandLine</NAME><VALUE>false</VALUE></SETTING>-<SETTING><NAME>ShowCommandLine</NAME><VALUE>true</VALUE></SETTING>-;

		# Remove generation of Browse info by Language Parser (temporary workaround for crashes in automated IDE builds)
		s-<SETTING><NAME>BrowserGenerator</NAME><VALUE>2</VALUE></SETTING>-<SETTING><NAME>BrowserGenerator</NAME><VALUE>0</VALUE></SETTING>-;		
		}
		
	open XML,">$xmlfile" or return;
	print XML @lines;
	close XML;
	}
	
sub BuildTargets($$)
	{
	my ($targets,$platform) = @_;

	for (my $item=0; $item<$targets->Count; $item++ ) 
		{
		my $target = $targets->Item($item);
	    my $targetName = $target->name;
	    # Skip platforms we aren't interested in...
	    next if ($targetName !~ /$platform /i);

		print "Building $targetName...\n";
		     
	    $target->RemoveObjectCode( 'true' );

		my $buildMessages = $target->BuildAndWaitToComplete();
		if (!defined($buildMessages))
			{
			printf "FATAL ERROR(S): build aborted? (%s)\n", $target->LastError();
			return 0;
			}
			
		my $messageList = $buildMessages->Informations;
		&printMessages("",$messageList) if defined($messageList);
		
		my $warningCount = $buildMessages->WarningCount;
		my $errorCount = $buildMessages->ErrorCount;    
		print "Completed $targetName with $errorCount errors and $warningCount warnings\n";

		if ($errorCount > 0)
			{
			$messageList = $buildMessages->Errors;
			&printMessages("FATAL ERROR(S): ", $messageList);
			}
		if ($warningCount > 0)
			{
			$messageList = $buildMessages->Warnings;
			&printMessages("WARNING: ", $messageList);
			}
		} 
	return 1;
	}

sub	printMessages ($$)
	{
    my ($prefix,$messageList) = @_;

	# traverse through the list of messages
	for (my $item = 0; $item < ($messageList->Count); $item++)
		{
        my $message = $messageList->Item($item);
		print $prefix,$message->MessageText,"\n";
		}
	}

