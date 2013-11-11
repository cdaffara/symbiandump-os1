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
# prepocessor.pm
# Used to allow us to access and manipulate other pre processors
# 
#

package Preprocessor;

require Exporter;
@ISA=qw(Exporter);

@EXPORT=qw(
	PreprocessorToUseId PreprocessorToUseExe PreprocessorToUsePath 
);



#
# Returns the preprocessor name.
# (After checking it is valid)
#
sub PreprocessorToUseId()
{
    $env="0";
    if (defined $ENV{ALT_PRE})
    {
	$env = $ENV{ALT_PRE};
    }
    
    if ( $env eq "1" ) # Only a value of 1 will use alternative preprocessor
    {
        return "MINGW_NO_CYGWIN";
    }
    else
    {
        return "DEFAULT";
    }
}



#
# Returns the preprocessor exe name.
# without the exe extension.
#


sub PreprocessorToUseExe()
{
    $env = &PreprocessorToUseId();
    
    if ( ($env eq "DEFAULT") )
    {
        return "cpp";
    }
    elsif ( $env eq "MINGW_NO_CYGWIN" )
    {
        return "scpp";
    }
}




#
# Returns the full path and exe of the preprocessor relative to the 
# gcc\bin path.
#
sub PreprocessorToUsePath()
{
    $env = &PreprocessorToUseId();

    if ( ($env eq "") || ($env eq "DEFAULT") )
    {
        return "";
    }
    elsif ( $env eq "MINGW_NO_CYGWIN" )
    {
        return ""; # Currently same path as default (different name though !)
    }
    else
    { # Nothing really to use.
        die
            "Unable to find the correct pre processor\n",
    }
}


1;