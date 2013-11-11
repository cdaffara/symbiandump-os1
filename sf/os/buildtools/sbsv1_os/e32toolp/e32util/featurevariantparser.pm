#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

# Module FEATUREVARIANTPARSER. Parses .VAR files and returns key variables.

# The following hashes can be used with this module:

# NAME 				-> Returns the name of the variant file (without the extension)

# FULLPATH 			-> Returns the full path of the variant file (including the extension)

# VALID 			-> Set to 1 if the variant file is valid, otherwise set to 0

# VIRTUAL 			-> Set to 1 if the variant is a grouping node, otherwise set to 0

# ROM_INCLUDES 		-> Returns a pointer to the list of ROM_INCLUDES (including Parent nodes).

# BUILD_INCLUDES 	-> Returns a pointer to the list of BUILD_INCLUDES (including Parent nodes).

# VARIANT_HRH 		-> Returns the full VARIANT_HRH file path used by the VAR file.

# PARENTS			-> Returns a pointer to the list of all the parent nodes, starting with immediate parent

# CHILDREN			-> Returns a pointer to the list of all the children nodes.

# USAGE : The GetVariant method should only be called using featurevariantparser->GetVariant(var_name, directory(optional) );
# If the directory for the VAR file is not supplied,the default directory will be searched for var_name.var

use strict;

package featurevariantparser;
use File::Spec;

my @buildinclude;
my @rominclude;
my @parents;
my @childNodes;
my $virtual;
my $childNodeStatus;
my $varianthrh;

my $defaultDir = "$ENV{EPOCROOT}epoc32\\tools\\variant";

my $dir;
my $fullpath;
my $fulldir;

my $pathregex = '.+[^\s]'  ;   # Regex to match all characters (including \ or /), excluding whitespaces.

our $verbose = 0;

# Wrapper function to return all the correct variables
# Arguments : (Variant Name, Variant Directory(optional))
# Returns a Hash.
#
# Note: This has to return a copy of all the data - no references!
#  There are package globals that are reused on a call to this function
#  so references would go out of date after repeated calls to GetVariant
#  This package should have been written using objects - too late now

sub GetVariant
{

    @buildinclude    = ();
    @rominclude      = ();
    @parents         = ();
    @childNodes      = ();
    $dir             = "";
    $fullpath        = "";
    $varianthrh      = "";
    $virtual         = 0;
    $childNodeStatus = 0;
    
    my $parnodes = "";
    my %data;
    my $children  = "";
    my $romincs   = "";
    my $buildincs = "";

    $data{'VALID'} = 0;

    my ( $empty, $varname, $dirname ) = @_;

    my $fullvarpath = ReturnFullVariantPath( $varname, $dirname );

    if ( $dirname )
    {
        $fulldir = $dirname;
    }
    else
    {
        $fulldir = $defaultDir;
    }

    $data{'FULLPATH'} = "$fullvarpath";
    $data{'NAME'}     = "$varname";

    # If the variant file exists, check the syntax and setup variables.
    if ( FileExists($fullvarpath) )
    {

        if ( CheckVarFileSyntax( $fullvarpath, $varname ) )
        {
            $data{'VALID'} = 1;
        }
    }
    else
    {
        print "ERROR: $fullpath" . " does not exist\n";
    }

    my $count = 0;

    # If VAR file is valid, setup all other variables.
    if ( $data{'VALID'} )
    {

        $romincs   = FindRomInclude($fullvarpath);
        $buildincs = FindBuildInclude($fullvarpath);
        $children  = FindChildNodes($fullvarpath);
        $parnodes  = FindParentNodes($fullvarpath);

        # Remove empty elements from the BUILD_INCLUDE list     
        @$buildincs = grep /\S/, @$buildincs;

        # Fix paths for all BUILD_INCLUDES
        for ( my $i = 0 ; $i < scalar(@$buildincs) ; $i++ )
        {
            @$buildincs[$i] = FixPaths( @$buildincs[$i] );
        }

        # Remove empty elements from the ROM_INCLUDE list
		@$romincs = grep /\S/, @$romincs;

        # Fix paths for all ROM_INCLUDES
        for ( my $i = 0 ; $i < scalar(@$romincs) ; $i++ )
        {
            @$romincs[$i] = FixPaths( @$romincs[$i] );
        }

        # Remove empty elements from the CHILDREN list
		@$children = grep /\S/, @$children;
		
        # Remove empty elements from the PARENT list
		@$parnodes = grep /\S/, @$parnodes;

        $data{'BUILD_INCLUDES'} = CloneList($buildincs);        
        $data{'ROM_INCLUDES'}   = CloneList($romincs);
        $data{'PARENTS'}        = CloneList($parnodes);
        $data{'CHILDREN'}       = CloneList($children);
        $data{'VARIANT_HRH'}    = $varianthrh;
        $data{'VIRTUAL'}        = $virtual;
    }

    # If variant file is not valid, return reference to a blank array
    else
    {
        $data{'BUILD_INCLUDES'} = [];
        $data{'ROM_INCLUDES'}   = [];
        $data{'VARIANT_HRH'}    = "";
        $data{'PARENTS'}        = [];
        $data{'CHILDREN'}       = [];
    }

    return %data;
}

# Helper method that clones a reference to a simple list
sub CloneList
    {
    my $ref = shift;
    
    # Check the reference is a list
    die "Not a list ref" if ref($ref) ne 'ARRAY';
    
    # Create a new list object
    my @list;
    foreach my $entry ( @$ref )
        {
        # Only clone lists of scalars
        die "Not a scalar" if ref($entry);
        
        # Add the entry to the new list
        push @list, $entry;
        }
    
    # return a reference to the copy    
    return \@list;
    }
    
# Method to correct all the slashes, and also append EPOCROOT if the path begins with a \ or /
# If path doesn't start with \ or /, returns an abosulte canonical path
sub FixPaths
{

    my $arr = $_[0];

    if ( $arr =~ m/^\// )
    {
       $arr =~ s/^\/?//;
        return File::Spec->canonpath( "$ENV{EPOCROOT}" . "$arr" );
    }

    elsif ( $arr =~ m/^\\/ )
    {
        $arr =~ s/^\\?//;
        return File::Spec->canonpath( "$ENV{EPOCROOT}" . "$arr" );
    }

    else
    {
        return File::Spec->rel2abs( File::Spec->canonpath("$arr") );
    }

}

# Method to construct a full variant path from the variant file and directory
sub ReturnFullVariantPath
{

    my $vardirectory = $_[1];
    my $varname      = $_[0];

    # Check if a directory is supplied
    if ($vardirectory)
    {
        $dir = "$vardirectory";
    }

    else
    {
        $dir = $defaultDir;
    }
    my $filename = "$varname" . "\.var";
    $fullpath = File::Spec->catfile( File::Spec->rel2abs($dir), $filename );

    if ( !File::Spec->file_name_is_absolute($fullpath) )
    {
        $fullpath = File::Spec->rel2abs($fullpath);
    }

    return $fullpath;
}

# Method to find the BUILDINCLUDE values of the VAR file.
sub FindBuildInclude
{

    my $filename = $_[0];

    my $parentNodes;

    # Construct a list of parent nodes if node is a child
    if ($childNodeStatus)
    {
        $parentNodes = FindParentNodes("$filename");
    }

    if ($parentNodes)
    {

        # Go through and build the list of all parent BUILD_INCLUDES
        for ( my $i = scalar(@$parentNodes) - 1 ; $i >= 0 ; $i-- )
        {

            my $tmp = ReturnFullVariantPath( @$parentNodes[$i], $fulldir );
            open( NEWHANDLE, "<$tmp" );
            while (<NEWHANDLE>)
            {
                if (/BUILD_INCLUDE/)
                {
                    ExtractBuildIncludeValue($_);
                }
            }
            close(NEWHANDLE);
        }
    }

    # Append the BUILD_INCLUDES of the VAR file in the end
    open( NEWHANDLE, "<$filename" );

    while (<NEWHANDLE>)
    {
        if (/BUILD_INCLUDE/)
        {
            ExtractBuildIncludeValue($_);
        }
    }
    close(NEWHANDLE);

    undef(@parents);    # Flush out parent array

    return \@buildinclude;

}

# Method to extract the BUILD_INCLUDE value of a node.
sub ExtractBuildIncludeValue
{

# If modifier append is found, push the buildinclude to the end of the array list.
    if (/^BUILD_INCLUDE\s+append\s+($pathregex)/)
    {
        push( @buildinclude, ($1) );

    }

# If modifier prepend is found, push the buildinclude to the beginning of the array list.
    if (/^BUILD_INCLUDE\s+prepend\s+($pathregex)/)
    {
        unshift( @buildinclude, ($1) );
    }

#If keyword set is found, then empty the buildinclude variable and push the new value
    if (/^BUILD_INCLUDE\s+set\s+($pathregex)/)
    {
        undef(@buildinclude);
        push( @buildinclude, ($1) );
    }

}

# Method to find the ROMINCLUDE values of the VAR file.
sub FindRomInclude
{

    my $filename = $_[0];

    my $parentNodes;

    # Construct a list of parent nodes if node is a child
    if ($childNodeStatus)
    {
        $parentNodes = FindParentNodes("$filename");
    }

    if ($parentNodes)
    {

        # Go through and build the list of all parent ROM_INCLUDES
        for ( my $i = scalar(@$parentNodes) - 1 ; $i >= 0 ; $i-- )
        {
            my $t = ReturnFullVariantPath( @$parentNodes[$i], $fulldir );
            open( NEWHANDLE, "<$t" );

            while (<NEWHANDLE>)
            {
                if (/ROM_INCLUDE/)
                {
                    ExtractRomIncludeValue($_);
                }
            }
            close(NEWHANDLE);
        }
    }

    # Append the ROM_INCLUDES of the VAR file in the end
    open( NEWHANDLE, "<$filename" );

    while (<NEWHANDLE>)
    {
        if (/ROM_INCLUDE/)
        {
            ExtractRomIncludeValue($_);
        }
    }

    undef(@parents);    # Flush out parent array;
    return \@rominclude;

}

# Method to extract the ROM_INCLUDE value of a node.
sub ExtractRomIncludeValue
{

# If modifier append is found, push the rominclude to the end of the array list.
    if (/^ROM_INCLUDE\s+append\s+($pathregex)/)
    {
        push( @rominclude, ($1) );
    }

# If modifier prepend is found, push the rominclude to the beginning of the array list.
    if (/^ROM_INCLUDE\s+prepend\s+($pathregex)/)
    {
        unshift( @rominclude, ($1) );
    }

# If keyword set is found, then empty the rominclude variable and push the new value
    if (/^ROM_INCLUDE\s+set\s+($pathregex)/)
    {
        undef(@rominclude);
        push( @rominclude, ($1) );
    }

}

# Method to find the immediate parent node of a child node
sub ExtractExtendsValue
{

    $_[0] =~ m/^EXTENDS\s+(\w+)/;
    return $1;
}

# Extract the value of the VARIANT keyword
sub ExtractVariantValue
{

    $_[0] =~ m/^VARIANT\s+(\w+)/;
    return $1;
}

# Extracts the value of the HRH file from the VARIANT_HRH line supplied
sub ExtractHrhValue
{

    $_[0] =~ m/^VARIANT_HRH\s+($pathregex)/;
    return $1;

}

# Finds if the variant file is a group node
# Note: This method is only a supplementary method, not actually used during this module
#       Provides a quick way to check is any VAR file is grouping node or not without loading the entire file.
sub IsVirtual
{

    my $filename = $_[0];

    open( READHANDLE, "<$filename" );
    while (<READHANDLE>)
    {
        if (/^VIRTUAL\s*$/)
        {
            close(READHANDLE);
            return 1;
        }
    }
    close(READHANDLE);
    return 0;
}

# Constructs a list of Parent nodes for a given Child node.
sub FindParentNodes
{

    my $filename   = $_[0];
    my $hasparents = 0;

    open( READHANDLE, "<$filename" );
    while (<READHANDLE>)
    {
        if (/EXTENDS/)
        {
            $hasparents = 1;
            push( @parents, ExtractExtendsValue($_) );

        }
    }

    close(READHANDLE);

    if ( $hasparents == 1 )
    {
        FindParentNodes(
            ReturnFullVariantPath( @parents[ scalar(@parents) - 1 ], $fulldir )
        );
    }
    else
    {
        return \@parents;
    }

}

# Constructs a list of Child nodes for a given Parent node (full path or .var path required)
sub FindChildNodes
{

    my $var = ReturnNativeVarName("$_[0]");

    my $tmpname    = "";
    my @childarray = ();

    opendir( DIR, $fulldir );

    while ( defined( my $file = readdir(DIR) ) )
    {

        if ( $file =~ m/\.var$/ )
        {
            $tmpname = $file;
            $tmpname =~ s/\.var$//;

            open( FILEHANDLE, ReturnFullVariantPath( $tmpname, $fulldir ) );

            while (<FILEHANDLE>)
            {

                if (/^EXTENDS/)
                {
                    if ( lc $var eq lc ExtractExtendsValue($_) )
                    {
                        push( @childarray, $tmpname );
                    }
                }

            }
            close(FILEHANDLE);
        }
    }

    close(DIR);

    foreach my $child (@childarray)
    {
        push( @childNodes, $child );
    }

    foreach my $child (@childarray)
    {
        FindChildNodes( ReturnFullVariantPath( $child, $fulldir ) );
    }

    return \@childNodes;
}

# Method to return all the buildable (i.e. No syntax erros and non-virtual) list of
# variants that can be built in the specified directory. If no directory is specified,
# the default location is searched.

# Usage: GetBuildableFeatureVariants(<Directory>)

sub GetBuildableFeatureVariants
{

    my $empty = shift;
    my $dir   = shift;
    my @list;
    my $fulldir;

    if ( $dir )
    {
        $fulldir = $dir;
    }
    else
    {
        $fulldir = $defaultDir;
    }

    opendir( DIR, $fulldir );

    while ( defined( my $file = readdir(DIR) ) )
    {

        if ( $file =~ m/\.var$/ )
        {

            $file =~ s/\.var$//;

            my $fullpath = ReturnFullVariantPath( $file, $fulldir );

            if ( CheckVarFileSyntax( $fullpath, $file )
                && !IsVirtual($fullpath) )
            {
                push( @list, $file );

            }
        }
    }

    return sort @list;

}

# Method to return a list of the valid and non-virtual children of a given VAR node, in a given location.
# If the calling var file is non-virtual, it is returned as the only element of the list If not, then
# the method parses though and finds all buildable children

# USAGE: ResolveFeatureVariant(<varname>,<dirame>);

sub ResolveFeatureVariant
{

    my $empty   = shift;
    my $varfile = shift;
    my $dir     = shift;
    my $fulldir;
    my @list;

    if ( !$dir eq "" )
    {
        $fulldir = $dir;
    }
    else
    {
        $fulldir = $defaultDir;
    }
    my $fullpath = ReturnFullVariantPath( $varfile, $fulldir );

    if ( CheckVarFileSyntax( $fullpath, $varfile ) && !IsVirtual($fullpath) )
    {
        push( @list, $varfile );
        return @list;
    }

    my %variant = GetVariant( 0, $varfile, $fulldir );
    my $child = $variant{'CHILDREN'};

    foreach my $item (@$child)
    {

        my $fullpath = ReturnFullVariantPath( $item, $fulldir );

        if ( CheckVarFileSyntax( $fullpath, $item ) && !IsVirtual($fullpath) )
        {
            push( @list, $item );
        }
    }

    return @list;

}

# Method to return all valid (no syntax errors only) list of variants
# in the specified directory. If no directory is specified,the default location is searched.

# Usage: GetValidVariants(<Directory>)

sub GetValidVariants
{

    my $empty = shift;
    my $dir   = shift;
    my @list;
    my $fulldir;

    if ( !$dir eq "" )
    {
        $fulldir = $dir;
    }
    else
    {
        $fulldir = $defaultDir;
    }

    opendir( DIR, $fulldir );

    while ( defined( my $file = readdir(DIR) ) )
    {

        if ( $file =~ m/\.var$/ )
        {

            $file =~ s/\.var$//;

            my $fullpath = ReturnFullVariantPath( $file, $fulldir );

            if ( CheckVarFileSyntax( $fullpath, $file ) )
            {

                push( @list, $file );

            }
        }
    }

    return sort @list;
}

# Returns the Variant name from a complete path, without the .var extension
sub ReturnNativeVarName
{

    my $tmp = "$_[0]";
    $tmp =~ /^.*[\\|\/](.*)\.var$/i;  
    return $1;

}

# Checks if a file passed to this function exists.
sub FileExists
{
	return -e $_[0];

}

# Checks if the default variant file (default.var) exists in the directory supplied.
# If no directory is supplied, looks under the default location.
# Should only be called directly from the featurevariantparser module reference, not from any methods within the module.

# USAGE: featurevariantparser->DefaultExists(<DirName>);
# <DirName> : Optional -- Specifies which directory to look under to find the default.var file. Can be relative or absolute.

sub DefaultExists
{

    my $dirToSearch = "";
    if ( $_[1] )
    {
        $dirToSearch = $_[1];
    }
    else
    {
        $dirToSearch = $defaultDir;
    }

    $dirToSearch =
      File::Spec->canonpath(
        File::Spec->rel2abs( File::Spec->canonpath("$dirToSearch") ) );

    return ( -e "$dirToSearch/default.var" );

}

# Checks the variant file for the correct syntax and reports any errors
# Also sets up some variables(VIRTUAL ,VARIANT_HRH and VARIANT) whilst file is being parsed.

# Usage: CheckVarFileSyntaxt(<fullpath>,<varfile>) . Note: <varfile> without .var
sub CheckVarFileSyntax
{

    my $fullpath          = $_[0];
    my $varname           = $_[1];
    my $varianthrhpresent = 0;

    open( READVAR, "<$fullpath" );
    my $exp  = "#";
    my $line = "";

    while (<READVAR>)
    {
        $line = $.;

	# Checks for a valid argument supplied to EXTENDS keyword. Checks for one and only one argument supplied.
        if (/^EXTENDS/)
        {
            if ( !m/^EXTENDS\s+./ )
            {
                print "\nERROR: Invalid format supplied to argument EXTENDS on line "
                  . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }
            my $str = ExtractExtendsValue($_);
            if ( $str =~ /\s+/ )
            {
                print "\nERROR: Cannot extend from two nodes. Error in line "
                  . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }

            $childNodeStatus = 1;

        }

        # Checks for the grammar of BUILD_INCLUDE, i.e. KEYWORD MODIFIER VALUE
        elsif (/^BUILD_INCLUDE/)
        {

          if (!m/^BUILD_INCLUDE\s+(append|prepend|set)\s+$pathregex/)
            {
                print "\nERROR: Invalid syntax supplied to keyword BUILD_INCLUDE on line "
                  . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }
            
		 if (m/^BUILD_INCLUDE\s+(append|prepend|set)\s+$pathregex\s+$pathregex/)
            {
                print "\nERROR: Too many arguments supplied to keyword BUILD_INCLUDE on line "
                  . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }
        }

        # Checks for the grammar of ROM_INCLUDE, i.e. KEYWORD MODIFIER VALUE
        elsif (/^ROM_INCLUDE/)
        {

		if (!m/^ROM_INCLUDE\s+(append|prepend|set)\s+$pathregex/)
            {
                print "\nERROR: Invalid syntax supplied to keyword ROM_INCLUDE on line "
                  . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }

       if (m/^ROM_INCLUDE\s+(append|prepend|set)\s+$pathregex\s+$pathregex/)
            {
                print "\nERROR: Too many arguments supplied to keyword ROM_INCLUDE on line "
                  . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }
        }

        # Checks for a valid VARIANT name
        elsif (/^VARIANT[^_HRH]/)
        {
            if ( !m/^VARIANT\s+\w+/ )
            {
                print "\nERROR: VARIANT name not specified on line " . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }
            if ( uc("$varname") ne uc( ExtractVariantValue($_) ) )
            {
                print "\nERROR: VARIANT filename does not match variant name specified on line "
                  . "$line"
                  . " in file "
                  . "$fullpath"
                  . "\nVariant value extracted from the VAR file is " . "$_";
            }

        }

        # Checks that keyword VIRTUAL is declared correctly
        elsif (/^VIRTUAL/)
        {
            if (m/^VIRTUAL\s+\w+/)
            {
                print "\nERROR: Invalid declaration of VIRTUAL on line " . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }

            $virtual = 1;
        }

        # Checks if VARIANT_HRH is declared correctly.
        elsif (/^VARIANT_HRH/)
        {
            $varianthrhpresent = 1;
            my $lineno = $.;
            if ( !m/^VARIANT_HRH\s+./ )
            {
                print "\nERROR: Invalid format supplied to argument VARIANT_HRH on line "
                  . "$lineno"
                  . " in file "
                  . "$fullpath";
                return 0;
            }

            my $str = ExtractHrhValue($_);
            if ( $str =~ /\s+/ )
            {
                print "\nERROR: Cannot have 2 or more hrh files. Error in line "
                  . "$lineno"
                  . " in file "
                  . "$fullpath";
                return 0;
            }

            if ( !FileExists( FixPaths($str) ) )
            {
                print "\nERROR: VARIANT HRH file : "
                  . FixPaths($str)
                  . " specified on line "
                  . "$lineno"
                  . " does not exist";
                return 0;
            }

            $varianthrh = FixPaths( ExtractHrhValue($_) );

        }
        
        # If none of the valid keywords are found
        else
        {

            # Do nothing if a comment or blank line is found
            if ( (m/$exp\s+\S/) || (m/$exp\S/) || ( !m/./ ) || (m/^\n/) )
            {
            }

            # Unsupported keyword
            else
            {

                print "\nERROR: Invalid keyword " . '"' . "$_" . '"'
                  . " found on line " . "$."
                  . " in file "
                  . "$fullpath";
                return 0;
            }
        }
    }

    close(READVAR);

    # If no HRH file defined, check if the default one exists
    if ( !$varianthrhpresent )
    {
        print "\nINFO: No VARIANT_HRH defined in VAR file, using $ENV{EPOCROOT}epoc32\\include\\variant\\$varname\.hrh" if ($verbose);
        my $str =
          ExtractHrhValue(
            "VARIANT_HRH $ENV{EPOCROOT}epoc32\\include\\variant\\$varname\.hrh"
          );

        if ( !FileExists($str) )
        {
            print "\nERROR: VARIANT HRH file : " . "$str " . "does not exist\n";
            return 0;
        }
        else
        {
            $varianthrh = $str;
        }
    }
    return 1;
}

1;

