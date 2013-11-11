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
# err_formatter.pm
# This tool is asked to invoke the Lada compiler and reformat the errors/warnings from CW
# style to Visual Studio error/warning reporting style.
# 
#


use FindBin;

my $command = join(' ', @ARGV);
open PIPE, "$command 2>&1 | ";


my $nextLineOfInterest = 0;
my $new_error_line=();
my $comp_msg=();
my $error_token=();

# The error/warning format for CW compiler for the option 
# -msgstyle parseable is as follows:
#
# ToolName|ToolType|MsgType (FileName|Linenumber|digit|digit|digit|digit)
# = AtToken
# > ErrorMsg
#
# In the above format, the symbols '|', '(', ')', '=' and '>' occur always
# and this tool assumes there presence.
# Also, in the above, 
#	'ToolName' here is mwccsym2.exe
#	'ToolType' here is Compiler
#	'MsgType' is either Error or Warning
#	'FileName' is the file that caused the compiler error/warning
#	'Linenumber' is the line at which the error/warning is reported
#	'AtToken' is the token at which the error/warning was reported.
#	'ErrorMsg' is the error message and it amrks the end of this error/warning.
#

my @msgs;
while(<PIPE>)
{
	if( $nextLineOfInterest == 1)
	{
		if($_ =~ /^>(.*)/)
		{
			$comp_msg .= $1;
			$new_error_line .= "$comp_msg ";
			$new_error_line .= ": $error_token" if($error_token);
			push @msgs, $new_error_line;
			
			$nextLineOfInterest = 0;
			$comp_msg = "";
			$error_token = "";
			next;
		}
		if($_ =~ /^=(.*)/)
		{
			$error_token = $1;
			next;
		}
		if($_ =~ /\((.*)\|([0-9]+)\|([0-9]+)\|([0-9]+)\|([0-9]+)\|([0-9]+)\)/)
		{
######### $1 is file name
######### $2 is line number

			$new_error_line = $1."(";
			$new_error_line .= $2;
			$new_error_line .= ")";
			$new_error_line .= ": ";

			next;
		}
	}
	if($_ =~ /Compiler\|Error/)
	{
		$comp_msg = "Error: ";
		$nextLineOfInterest = 1;
		next;
	}
	elsif($_ =~ /Compiler\|Warning/)
	{
		$comp_msg = "Warning: ";
		$nextLineOfInterest = 1;
		next;
	}
	else
	{
		$nextLineOfInterest = 0;
		push @msgs, $_;
	}
}

close PIPE;
my $msg;
foreach $msg (@msgs)
{
	print "$msg\n";
}

