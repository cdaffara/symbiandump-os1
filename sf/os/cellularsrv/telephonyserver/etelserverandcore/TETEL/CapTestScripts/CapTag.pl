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
# Script:	Auto Capability Tag Insertion and Verification
# Author:	Mike Mingay
# Updated:	27th September 2004
# Version: 	1.7
# Usage
# This script should primarily be used to automatically insert Capability tags for specific functions, 
# specified in an excel spreadsheet, into source code. It secondarily can be used to confirm that capabilities
# already assigned in source code tags match those in the spreadsheet and if not, updates them appropriately.
# The spreadsheet location and format are easily customisable as long as the filename, class, 
# function and desired capability are listed in rows in the spreadsheet - referenced in the "DEFINITIONS" section below.
# Symbian components should be added as separate sheets and should be listed in the "COMPONENT LIST" below.
# This script should be run from the ROOT:\ of a subst subsystem drive (i.e. with ROOT:\Telephony or
# ROOT:\NbProtocols in our case etc). Ensure that the source has been checked out of 
# any source control application (PerForce for Symbian) or you will probably recieve read-only file warnings.
# Run "perl CapTag.pl" and it will process the specified components Two logfiles will be created:
# CapTagSummary.log details the progress of a run and CapTagError.log indicates any problems enountered. 
# Check the error logs as they will highlight any duplicate, missing or updated Capability tags that may need addressing.
# Comment in/out the components in the "COMPONENT LIST" at circa line 50 if you only need to process one or 
# a few components, If the spreadhseet format changes, simply update the "Definitions" section below. 
# Remember that the first row in the spreadsheet will be treated as a header row.
# If you wish to use the script on a non-Telephony related component, please update the locations specified 
# on +- line 184 within the "find(\&OpenFile" functions to reflect those in your subsystem.
# Version History and ToDo list
# 2004-08-04 Yuting Kuo		Updated to write one capability tag per line
# In the "ProcesComponent" sub we could use $numRows= $main::excelsheet->UsedRange->Rows->{Count}; 
# and $numColumns= $main::excelsheet->UsedRange->Columns->{Count}; to determine the data size instead of 
# fixed range but the script is so fast this wont be highly advantageous.
# We could start using $main::excel->{DisplayAlerts}=0; if there are excell warning messages we wish to squash
# A further check to confirm the the capabilities being read and assigned are valid (i.e (ReadUserData,
# NetworkServices etc.) in spelling and case would be useful.
# Declarations
# 
#

use strict;
use Win32::OLE;
use File::Find;

#=========================================================
# DEFINITIONS
#=========================================================
$main::ExcelFile = 'I:\API Capability Assignment.xls';
$main::FileNameColumn = 1;
$main::ClassNameColumn = 2;
$main::FunctionNameColumn = 3;
$main::CapabilityColumn = 11;

#=========================================================
# Main Program Open
#=========================================================

# Open a blank log and error log file (if they exist, first delete existing ones)
unless (open (LOGFILE, ">CapTagSummary.log"))
	{printf "Couldnt open logfile %s\n",$_;}
unless (open (ERRORFILE, ">CapTagError.log"))
	{printf "Couldnt open logfile %s\n",$_;}

printf (LOGFILE "***********************************************************\n");
printf (LOGFILE "CapTag Process LogFile %s\n",scalar localtime());
printf (LOGFILE "***********************************************************\n");
printf (ERRORFILE "***********************************************************\n");
printf (ERRORFILE "CapTag Error LogFile %s\n",scalar localtime());
printf (ERRORFILE "***********************************************************\n");

# Establish MS Excel Access
eval {$main::excel= Win32::OLE->GetActiveObject('Excel.Application')};
		die "Excel not installed" if $@;
unless (defined $main::excel) 
		{$main::excel= Win32::OLE->new('Excel.Application', sub {$_[0]->Quit;})
		or die "Cant start Excel";}
printf "Excel access established\n";

# Open Our Capability Spreadsheet
$main::excelbook = $main::excel->Workbooks->Open({FileName => $main::ExcelFile, ReadOnly=>1});

#=========================================================
# COMPONENT LIST
#=========================================================
ProcessComponent("ETEL");
ProcessComponent("ETELMM");
ProcessComponent("ETELPCKT");
ProcessComponent("ETELSAT");
ProcessComponent("PHBKSYNC");
#ProcessComponent("FAX");
ProcessComponent("ETELISV");
ProcessComponent("C32");
#ProcessComponent("NBPROTOCOLS");


#=========================================================
# Main Program Close
#=========================================================
# Close Excel
#$main::excelbook-> Close();
#$main::excel-> Quit();
# Close log file
close(LOGFILE);
printf "\nCapability Tag Insertion Completed - Have a great day\n";
printf (LOGFILE "\n***********************************************************\n");
printf (LOGFILE "Capability Tag Insertion Completed %s\n",scalar localtime());
printf (LOGFILE "***********************************************************");
printf (ERRORFILE "\n***********************************************************\n");
printf (ERRORFILE "Capability Tag Insertion Completed %s\n",scalar localtime());
printf (ERRORFILE "***********************************************************");
exit(0);

#=========================================================
# Subroutines
#=========================================================

# Processes specified components by stepping through each row in the spreadseet,
# and insterting the described capability for each function into its source file
sub ProcessComponent($)
{
	my $Component = shift;
	printf "\n\nProcessing component %s\n",$Component;
	printf (LOGFILE "\n***********************************************************\n");
	printf (LOGFILE "Processing component %s\n",$Component);
	printf (LOGFILE "***********************************************************\n");
	printf (ERRORFILE "\n***********************************************************\n");
	printf (ERRORFILE "Processing component %s\n",$Component);
	printf (ERRORFILE "***********************************************************\n");

	$main::excelsheet = $main::excelbook->Worksheets($Component);
	$main::openfilename = "";
	$main::newfunctionname = "";
	$main::functionname = "";
	$main::filename = "";
	
	# Now process each row one by one (temp maximum of 2 rows)
	for (my($row)=2;$row<800;$row++)
	{
		# Only continue if there is an entry in this row and if it isnt the same as the previous
		$main::newfunctionname = $main::excelsheet->Cells($row,$main::ClassNameColumn)->{'Value'}."::".$main::excelsheet->Cells($row,$main::FunctionNameColumn)->{'Value'};
		if (($main::newfunctionname ne "::") && ($main::newfunctionname ne $main::functionname))
		{
			#print ERRORFILE "$main::newfunctionname ----------------------------------------\n";
			# Get this APIs information
			$main::filename = $main::excelsheet->Cells($row,$main::FileNameColumn)->{'Value'};
			$main::functionname = $main::newfunctionname;
			# Re-arrange multi-capability entry in alphabetical order (automatically removes unnecessary spaces too!)
			$main::capabilities = join (' ',sort split(' ', $main::excelsheet->Cells($row,$main::CapabilityColumn)->{'Value'}));

			# Confim that appropriate values have been read in from the spreadsheet
			# FunctionName and ClassName will already have been inherently checked in the if statement above
			if (($main::filename eq "") || ($main::capabilities eq ""))
			{
				printf "WARNING: Missing Information in row %s, Filename = %s, Capability = %s\n",$row,$main::filename,$main::capabilities;
				printf (ERRORFILE "WARNING: Missing Information in row %s, Filename = \"%s\", Capability = \"%s\"\n",$row,$main::filename,$main::capabilities);
			}

			# If the file has not been opened new then find it in the source tree
			# This saves us having to research the directory tree for the file
			# listed in EVERY row in the spreadsheet
			if ($main::filename ne $main::openfilename)
			{
			 	printf(LOGFILE "Opening new file %s as it is unopened or different to curently open source file %s\n",$main::filename,$main::openfilename);
				printf "New file %s is unopened or different to curently open source file %s\n",$main::filename,$main::openfilename;
				
				# Now find the file in the source tree and update global file location variables
				$main::FileFound  = 0;
				# Telephony specific search routines
				if ($Component eq "C32")
					{find(\&OpenFile, "\/Ser-Comms/c32");}
				elsif ($Component eq "NBPROTOCOLS")
					{find(\&OpenFile, "\/NbProtocols/smsstack");}
				else
					{find(\&OpenFile, "\/Telephony");}

				# Check the file was indeed found
				unless ($main::FileFound)
				{
					printf "WARNING: Cant find file \"%s\" listed on line %s in the spreasheet \(%s\)\n",$main::filename,$row,$Component;
					printf (ERRORFILE "WARNING: Cant find file \"%s\" listed on line %s in the spreasheet \(%s\)\n",$main::filename,$row,$Component);
				}
			}
			# Now find the function in the source and add its specific capability(s) tag
			AddCapTags();
		}
    }
}

# Finds specified source file for editing and updates global directory locations. This function
# is called for every file in the subdirectories of the "find" directory
sub OpenFile()
{
	# Check if this is this is the file we want
	if ($_ =~ $main::filename && $main::filename ne "")
	{
		$main::FileFound = 1;
		$main::openfilename = $_;
		$main::openfilenamefull = $File::Find::name;
		printf "Found \"%s\" in source tree matching %s\n",$main::filename, $main::openfilename;
		printf (LOGFILE "Found \"%s\" in source tree\n",$main::openfilenamefull);
	}
}

# Searches through the open file line by line for current function and adds its capability(s) tag
# to a second file which then replaced the origional
sub AddCapTags
{
	my $line = "";
	my $NewFileText = "";
	my $FoundFunction = 0;
	my $ProcessedFunction = 0;
	
	# A list of remaining capabilities to write
	my $CapsToWrite = $main::capabilities;

	# Open the origional source file
	unless (open (SOURCEFILE, $main::openfilenamefull))
	{
		printf "Couldnt open \"%s\" for seaching\n",$_;
		printf (ERRORFILE "WARNING: Couldnt open \"%s\" in source tree for seaching\n",$_);
	}

	# Search through the file line by line for our function
	while(<SOURCEFILE>) 
	{
		$line=$_;
		
		# Check if this is the line with our function
		my $FunctionIndex = index $line,$main::functionname;
		if ($FunctionIndex > -1)
		{
			if ($FoundFunction)	
			{
				# BUG: It's writing the warning text to source code without commentting the text
				# BUG: Duplication functions possible with different arguments, but multiline declaration not possible
				#		with this version, also wrong entries in spreadsheet
				$NewFileText = $NewFileText."WARNING: Similar/Duplicate function declaration %s - Please check capability assignment manualy\n";
				printf (ERRORFILE "WARNING: Already found \"%s\" - Please check assignment manually\n",$main::functionname);
				printf "WARNING: Already found \"%s\" - Please check assignment manually\n",$main::functionname;
			}
			else 
			{
				# Found a line in source that matched the function we are serching for
				$FoundFunction = 1;
			}
		}

		# So if we have found the function but not processed it yet, keep going through the 
		# comment if it exists till we get to the "{" or the "*/"
		if ($FoundFunction == 1 && $ProcessedFunction == 0)
		{
			# Check if this already has a capability tag
			my $FoundCapabilityTag = index $line,"\@capability ";

			# Check if this contans a closing doxygen statement or an opening bracket
			my $FoundClosingDoxygen = index $line,"\*\/";
			my $FoundOpeningBracket = index $line,"\{";
			if ($FoundOpeningBracket > -1)
			{
				#printf "Found opening bracket in at \"%s\"\n",$FoundOpeningBracket;
				#printf "Assigning Capability \"%s\" to \"%s\" in \"%s\"\n",$CapsToWrite,$main::functionname,$main::filename;
				printf (LOGFILE "Assigning in \"%s\" Capability \"%s\" to \"%s\"\n",$main::filename,$CapsToWrite,$main::functionname);
				# No doxygen comment was found so enter it now with the capabilities as its only entry
				if ($CapsToWrite ne '')
				{
					$NewFileText = $NewFileText."/**\n";
					foreach (split ' ', $CapsToWrite)
					{
						$NewFileText = $NewFileText."\@capability ".$_."\n";
					}
					$NewFileText = $NewFileText."*/\n";
				}
				$NewFileText = $NewFileText.$line;
				$ProcessedFunction = 1;
			}
			elsif ($FoundClosingDoxygen > -1)
			{
				#print ERRORFILE "   ==> Comments found, writing remaining caps\n";
				#printf "Found doxygen comment close at \"%s\"\n",$FoundClosingDoxygen;
				#printf "Assigning Capability \"%s\" to \"%s\" in \"%s\"\n",$CapsToWrite,$main::functionname,$main::filename;
				printf (LOGFILE "Assigning in \"%s\" Capability \"%s\" to \"%s\"\n",$main::filename,$CapsToWrite,$main::functionname);
				# Found a closing doxygen comment so:
				if ($CapsToWrite ne '')
				{
					# if there are still any capabilities to write
					# Delete the closing comment
					my $CleanedLine = substr($line, 0, $FoundClosingDoxygen);
					$NewFileText = $NewFileText.$CleanedLine."\n";
					# and add the new Capabilities with its own termination comment to ensure there is no data loss
					foreach (split ' ', $CapsToWrite)
					{
						#print ERRORFILE "       $_\n";

						$NewFileText = $NewFileText."\@capability ".$_."\n";
					}
					$NewFileText = $NewFileText."*/\n";
				}
				else
				{
					# Comment found but no capabilities to write (TODO: should this happen?)
					$NewFileText = $NewFileText.$line;
				}
				
				$ProcessedFunction = 1;
			}
			elsif ($FoundCapabilityTag > -1)
			{
				# This function already has Capability tag(s)
				# Find all capabilities on the line e.g. \@capability Cap1 Cap2 Cap3
				$line =~ /.*\@capability (.*)/;
				#print ERRORFILE "   ==> Found [$1]\n";
				foreach (split ' ', $1)
				{
					# Tick off the found capabilities from our list
					#print ERRORFILE "       Ticking off [$_] from list\n";
					#print ERRORFILE "       [$CapsToWrite] -> ";
					unless ($CapsToWrite =~ s/($_)\s*//)
					{
						# A wrong capability in the line
						printf "WARNING: Found incorrect/duplicate Capability \"%s\" for function \"%s\"\n", $_,$main::functionname;
						printf (LOGFILE "WARNING: Found incorrect/duplicate Capability \"%s\" for function \"%s\"\n", $_,$main::functionname);
						printf (ERRORFILE "WARNING: Found incorrect/duplicate Capability \"%s\" for function \"%s\"\n", $_,$main::functionname);
					}
					else
					{
						#print ERRORFILE "[$CapsToWrite]\n";
						# Correct capability, write it to a new @capability line
						printf "Found correct Capability \"%s\" for function \"%s\"\n", $_,$main::functionname;
						printf (LOGFILE "Found correct Capability \"%s\" for function \"%s\"\n", $_,$main::functionname);
						$NewFileText = $NewFileText."\@capability ".$1."\n";
					}
				}
			}
			else
			{
				# This is NOT a comment line contaning the opening bracket for the function so just add is straight to the new file
				$NewFileText = $NewFileText.$line;
			}
	    }
		else
		{
			# This is NOT the line we want so just add is straight to the new file
			$NewFileText = $NewFileText.$line;
		}		
    }
	close(SOURCEFILE);

	# If we did find the function and insert it capability then overright the origional
	# source file with the new one
	if ($FoundFunction == 1)
	{
		# Now create a new file and populate it with the new source
		open(NEWFILE, ">$main::openfilenamefull.new");
		printf (NEWFILE $NewFileText);
		close(NEWFILE);

		# Now delete the origional file and replace it with the new one
		unless (unlink("$main::openfilenamefull"))
			{printf "WARNING: Couldnt delete \"%s\"\n",$main::openfilenamefull;}
		unless (rename("$main::openfilenamefull.new", "$main::openfilenamefull"))
			{printf "WARNING: Couldnt rename \"%s\"\n",$main::openfilenamefull;}
	}
	else
	{
		printf "WARNING: Did NOT find function \"%s\" in source file \"%s\"\n",$main::functionname,$main::filename;
		printf (ERRORFILE "WARNING: Did NOT find function \"%s\" in source file \"%s\"\n",$main::functionname,$main::filename);
	}
}

