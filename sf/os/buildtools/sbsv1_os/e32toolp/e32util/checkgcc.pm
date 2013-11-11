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
# module for checking gcc is set up correctly
# 
#

package CheckGcc;

use strict;
use Preprocessor;


sub CheckGcc_Default()
{
 	# die if CPP.EXE (or whatever) in a dodgy place in the path
	my $pbp = $ENV{PBUILDPID};
	my @Paths=split ';', $ENV{Path};
	unshift @Paths,'.';	# add the current directory
	foreach (@Paths) {
		s-/-\\-go;	# for those working with UNIX shells
		s-^(.*[^\\])$-$1\\-o;   # ensure path ends with a backslash
		if ((-e $_.'CPP.EXE') or (-e $_.'CPP.BAT') or (-e $_.'CPP.CMD')) {
			unless (/\\GCC(\w\w)?\\BIN\\$/i) {
				unless (lc($1) eq lc($pbp)) {
					die
						"ERROR: First CPP executable found in path is in $_,\n",
						"but the required CPP.EXE was expected to be found in a directory\n",
						"with a name ending in \\GCC$pbp\\BIN\\, where the Cygnus tools\n",
						"this program depends upon are stored.\n",
						"Is your path set up correctly?\n"
					;
				}
			}
			return;
		}
	}
	die "ERROR: CPP executable not found in path\n";
}

sub CheckGcc_Generic()
{
	# die if CPP.EXE (or whatever pre processor) in a dodgy place in the path

	my @Paths=split ';', $ENV{Path};
	unshift @Paths,'.';	# add the current directory

	my $exe = &PreprocessorToUseExe();
	my $path = &PreprocessorToUsePath();
	
	foreach (@Paths) {
		s-/-\\-go;	# for those working with UNIX shells
		s-^(.*[^\\])$-$1\\-o;   # ensure path ends with a backslash
		s-$-$path-;	# add in the path relative to gcc\bin.
		if ((-e $_.$exe.'.EXE') or (-e $_.$exe.'.BAT') or (-e $_.$exe.'.CMD')) {
			unless (/\\EPOC32\\TOOLS\\\Q$path\E$/i) {
				die
					"ERROR: First $exe executable found in path is in $_,\n",
					"but the required $exe.EXE was expected to be found in a directory\n",
					"with a name ending in \\EPOC32\\TOOLS\\$path, where the tools\n",
					"this program depends upon are stored.\n",
					"Is your path set up correctly?\n"
				;
			}
			return;
		}
	}
	die "ERROR: $exe executable not found in path\n";

}
BEGIN {
	my $preprocessor_to_use = &PreprocessorToUseId();
	
	if ( $preprocessor_to_use eq "DEFAULT" )
	{
		&CheckGcc_Default();	# pre processor & cpp same. 
	}
	elsif ( $preprocessor_to_use eq "MINGW_NO_CYGWIN" )
	{
		&CheckGcc_Generic();	# check for preprocessor.
	}
	else
	{
		die("CHECKGCC.PM :  error should never happen\n");
	}
}


1;

