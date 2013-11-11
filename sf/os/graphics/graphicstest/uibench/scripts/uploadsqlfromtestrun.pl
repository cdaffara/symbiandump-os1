#!perl
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Script to upload performance or other data embedded as SQL statements
# in test execute output logs.
#
# This script keys off "SQL_UPLOAD_VERSION_0:" and "SQL_SESSION_ID=" tags for
# data extraction.

use Getopt::Long;
use File::Basename;
use File::Find;
use Cwd;

# The default perl install does not provide the mysql database driver, needed
# to connect to a MySQL database.  For convenience we deliver a pure perl
# implementation locally.  This application can use such GPL modules under the
# System Library exception of GPL.  If this script needs to be called from
# another directory, then the 'use lib ".";' directive won't work.  To resolve
# this problem, run the script "perl installmysqlperlmodule.pl" first.


use lib ".";

use MySQL;

use strict;
use warnings;

our ($searchRoot, $helpOnUsage, $jobId);
our @globalFileList;
our @globalSessionTransaction;
our @bulkSqlTransaction;

sub Usage
	{
	my ($aRequireParameter,$aMissingParameter) = @_;
	$aRequireParameter = 0 if not defined $aRequireParameter;

  	print <<END_OF_USAGE_TEXT;
Usage: perl uploadsqlfromtestrun.pl --dir=searchRootDir --job=jobID [--help]

Note this script requires the Net::MySQL package to either be installed or
to be supplied in the search path.  An example such commandline is:

   Q:\\>perl -IQ:\\epoc32\\release\\winscw\\udeb\\z\\uibench
   \\epoc32\\release\\winscw\\udeb\\z\\uibench\\uploadsqlfromtestrun.pl
   --dir=\\epoc32\\winscw\\c\\logs\\testexecute
   --job=655433

This script recurses through searchRootDir looking for htm files containing
<prefix>SQL_UPLOAD_VERSION_0:<sql commands>
upon which it invokes those <sql commands>

It also looks for 
<prefix>SQL_SESSION_ID=<session value>
upon which it associates the supplied integer jobID with the <session value>
in the database.  In the database these identifiers are unsigned integers:
jobid         int(10)    unsigned
sessionid     bigint(20) unsigned

The jobID would normally come from the overnight build system.  Low numbered
jobIDs, i.e. those <10000, would not collide with the build system and so can
be used when running this script interactively outside the context of a build
system.

The help option (--help, -h or -?) prints this message

END_OF_USAGE_TEXT
	
	if (defined $aMissingParameter)
		{	
  		print "Error: Parameter \"--$aMissingParameter\" missing\n"
  		}
 	exit $aRequireParameter;
	}
	
sub RemoveBackSlashes
	{
	my ($aPath) = @_;
	$aPath =~ s/\\/\//g;
	return $aPath;
	}

sub AddToGlobalFileList
	{
	my $aFile = $_;
	
	if (-f $aFile && $aFile =~ /.*.htm$/i)
		{
		push @main::globalFileList, $File::Find::name;
		}
	}
	
sub ParseFiles()
	{
	foreach my $file (@main::globalFileList)
		{
		open (FILE, "$file");		
		foreach my $line (<FILE>)
			{
			if ($line =~ /.*SQL_UPLOAD_VERSION_0:*/i)
				{
				$line =~ s/.*SQL_UPLOAD_VERSION_0://g;
				push @main::bulkSqlTransaction, $line;
				}
			if ($line =~ /.*SQL_SESSION_ID=/i)
				{
				$line =~ s/.*SQL_SESSION_ID=//g;
				chomp $line;
				$line = "INSERT INTO performance.jobsessionmap (jobid, sessionid) VALUES ('"
						. $main::jobId . "', '"
						. $line . "');\n"
						;
				push @main::globalSessionTransaction, $line;
				}
			}
		close FILE;
		}
	}

sub connectToSqlDatabase
	{
	return
		Net::MySQL->new(
		hostname => '4GBD02346',
		database => 'performance',
		user     => 'uibench',
		password => 'grui'
		);
	}
	
sub UploadSqlData()
	{
	my $dbHandle;
	$dbHandle = connectToSqlDatabase();
	$dbHandle->query(@bulkSqlTransaction);
	die if ($dbHandle->is_error);
	$dbHandle->close;
	
	# We are re-creating the connection to the database because this forces
	# the underlying client-server transaction to flush its socket.  There
	# is no flush API that the MySQL perl module gives us.  If we don't do
	# this, the transaction completes without errors, but does not actually
	# put the session rows into the database!
	$dbHandle = connectToSqlDatabase();
	$dbHandle->query(@globalSessionTransaction);
	$dbHandle->close;
	}
	
GetOptions ('dir=s' => \$searchRoot,
            'job=s' => \$jobId,
            'help|h|?' =>\$helpOnUsage) || Usage();

Usage(0) if $helpOnUsage;
Usage(1,'dir') if not defined $searchRoot;
Usage(1,'job') if not defined $jobId;

$searchRoot = RemoveBackSlashes($searchRoot);

@globalFileList = ();
find(\&AddToGlobalFileList, ($searchRoot));

@bulkSqlTransaction = ();
@globalSessionTransaction  = ();
ParseFiles();
UploadSqlData();
