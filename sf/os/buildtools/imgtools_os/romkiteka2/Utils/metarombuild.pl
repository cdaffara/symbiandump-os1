#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#! perl

# This script builds ROMs which are specified in a supplied XML file
# To use this script \epoc32\tools\buildrom must be installed on the drive
# with \epoc32\tools in the path

# Note: The TargetBoard attribute is no longer used but is still needed because of the structure of this script!

use strict;
use XML::Simple;
use Getopt::Long;
use Cwd;
use Cwd 'abs_path';
use File::Copy;
use File::Path;

# Get command line arguments
my ($romspec, $boards, $roms, $logdir, $buildnum, $publish, $help, $version) = ProcessCommandLine();

Usage() if ($help);
Version() if ($version);

die "Romspec xml file must be specified using the -romspec option\n" if (!$romspec);

# Construct arrays of boards and roms if they were specified
my @boards = split /,/, $boards if ($boards);
my @roms = split /,/, $roms if ($roms);

# Use the XML::Simple module to parse the romspec and pass the result
# into the $Roms hash reference
my $xml = new XML::Simple;
my $Roms = $xml->XMLin($romspec);

my $RomList = %$Roms->{'Rom'};

my $RomBuildStage = 1;

foreach my $rom (sort keys %$RomList)
{
    my $board = $RomList->{$rom}->{'TargetBoard'};
    if( (@boards == 0 and @roms == 0) or grep $rom, @roms or grep $board, @boards)
    {
        BuildRom($RomBuildStage, $RomList, $rom, $logdir, $buildnum ,$publish);
        $RomBuildStage++;
    }
}

#####################################################
#### Run buildrom on the specified ROM using ########
#### info from the romspec.xml               ########
#####################################################
sub BuildRom
{
    my $Stage = shift;  # BuildRom stage
    my $RomList = shift;  # Hash of the romspec.xml
    my $rom = shift;   # Rom to be built
    my $logdir = shift;  # logs directory
    my $buildnum = shift;  # build number
    my $publish = shift;   # Publish Location
    my $type = $ENV{Type}; # type of Build Master or Release
    my $builddir = $ENV{BuildDir}; # Build Directory
    my $Epocroot = $ENV{EPOCROOT}; # EpocRoot;
    my $InFileList="";  # i.e. Platsec, Techview, obyfiles
    my $XmlFileList="";
    my $MacroList="";   # for the buildrom -D option
    my $TargetBoard = $RomList->{$rom}->{'TargetBoard'};
    my $ImageFile = " -o".$RomList->{$rom}->{'ImageFile'}->{'name'}; # for the buildrom -o option
 
    # Construct the list of InFiles to pass to buildrom
    my $InFiles = %$RomList->{$rom}->{'InFile'};
    foreach my $infile (sort keys %$InFiles)
    {
        if ($infile eq "name")
        {
            $InFileList = " ".$InFiles->{'name'} unless (lc($InFiles->{'name'}) eq lc($TargetBoard));
        }
        else
        {
            $InFileList .= " ".$infile unless(lc($infile) eq lc($TargetBoard));
        }
    }
    my $RomXmlFlag='-f';
    my $XmlFiles = %$RomList->{$rom}->{'XMLFile'};
    foreach my $XmlFlags (keys %$XmlFiles)
    {
        if ($XmlFlags eq "flag")
        {
          $RomXmlFlag= $XmlFiles->{'flag'};
        }
    }
    foreach my $XmlFile (keys %$XmlFiles)
    {
        if ($XmlFile eq "name")
        {
            $XmlFileList = "$RomXmlFlag"."$Epocroot"."epoc32\\rom\\include\\".$XmlFiles->{'name'};
        }
        else
        {
            $XmlFileList .= "$RomXmlFlag"."$Epocroot"."epoc32\\rom\\include\\".$XmlFile unless(lc($XmlFile) eq lc($TargetBoard));
        }
    }
    
    # Get the ROM macro if one is defined
    if ( defined $RomList->{$rom}->{'Macro'} )
    {
        if (defined $RomList->{$rom}->{'Macro'}->{'name'} )
        {
            if ( $RomList->{$rom}->{'Macro'}->{'value'} ne "" )
            {
                $MacroList = " -D".$RomList->{$rom}->{'Macro'}->{'name'}."=".$RomList->{$rom}->{'Macro'}->{'value'};
            }
            else
            {
                $MacroList = " -D".$RomList->{$rom}->{'Macro'}->{'name'};
            }
        }
        else
        {
            my $Macros = %$RomList->{$rom}->{'Macro'};
            foreach my $macro (keys %$Macros)
            {
                if ( $Macros->{$macro}->{'value'} ne "" )
                {
                    $MacroList .= " -D".$macro."=".$Macros->{$macro}->{'value'};
                }
                else
                {
                    $MacroList .= " -D".$macro;
                }
            }
        }
    }
    
    # Call buildrom
        my $buildrom_command = "buildrom $InFileList $MacroList $XmlFileList $ImageFile 2>&1";
        my $gHiResTimer = 0;
        if (eval "require Time::HiRes;")
        {
            $gHiResTimer = 1;
        }
        else
        {
            print "Cannot load HiResTimer Module\n";
        }
        open TVROMLOG, ">> $logdir\\techviewroms$buildnum.log";
        print TVROMLOG "===------------------------------------------------\n";
        print TVROMLOG "-- Stage=$Stage\n";
        print TVROMLOG "===----------------------------------------------\n";
        print TVROMLOG "-- Stage=$Stage started ".localtime()."\n";
        print TVROMLOG "=== Stage=$Stage == Build $rom\n";
        print TVROMLOG "-- Stage=$Stage == $TargetBoard $InFileList\n";
        print TVROMLOG "-- $buildrom_command\n";
        print TVROMLOG "-- MetaromBuild Executed ID $Stage $buildrom_command \n";
        print TVROMLOG "++ Started at ".localtime()."\n";
        if ($gHiResTimer == 1)
        {
            print TVROMLOG "+++ HiRes Start ".Time::HiRes::time()."\n";
        }
        else
        {
            # Add the HiRes timer unavailable statement
            print TVROMLOG "+++ HiRes Time Unavailable\n";
        }
        my $command_output = `$buildrom_command`;
        print TVROMLOG $command_output;
        if ($?)
        {
            print TVROMLOG "ERROR: $buildrom_command returned an error code $?\n";
        }
        if (($command_output =~m/\d\sFile/g) and ($command_output!~/Failed/ig) and ($command_output!~/Unsucessful/ig))
        {
            print TVROMLOG "Rom Built Sucessfully\n";
        }
        else
        {
            print TVROMLOG "ERROR: $buildrom_command failed .Please check log techviewroms$buildnum.log for details\n";
        }
        if ($gHiResTimer == 1)
        {
            print TVROMLOG "+++ HiRes End ".Time::HiRes::time()."\n";
        }
        else
        {
            # Add the HiRes timer unavailable statement
            print TVROMLOG "+++ HiRes Time Unavailable\n";
        }
        print TVROMLOG "++ Finished at ".localtime()."\n";
        print TVROMLOG "=== Stage=$Stage finished ".localtime()."\n";
        close TVROMLOG;
        
        # Publishing of Logs and Roms#####################
        my $ImageFileXML = $ImageFile ;
        $ImageFileXML =~s/^\s-o//i;
        $ImageFileXML =~/(.*\d.techview)/;
        my $ImageFileXMLresult = $1;
        my $cwdir = abs_path ( $ENV { 'PWD' } );
        $cwdir =~s/\//\\/g;
        $rom =~ /(\w+).*/;
        my $data = $1;
        if(($publish ne ""))
        {
            if($rom =~ /(\w+).*/)
            {   
                my $data = $1;
                if(not -d "$publish\\$1")
                    {
                        mkpath "$publish\\$1" || die "ERROR: Cannot create $publish\\$1"; # If folder doesnt exist create it 
                    }
                    opendir(DIR, $cwdir) || die "can't opendir $cwdir: $!";
                    my @file_array =readdir(DIR);
                foreach ($ImageFileXMLresult)
                {
                    foreach my $ImageFileConcat (@file_array)
                    {
                        $ImageFileConcat =~/(.*\d.techview)/;
                        my  $Image = $1;
                        
                        if ($ImageFileXMLresult eq $Image)
                        {
                            copy ("$cwdir\\$ImageFileConcat" , "$publish\\$data\\");# or die "Cannot copy file:$!";
                        }                   
                    }
                    closedir DIR;
                }   
            }
        }
        else
        {
            print"Publish Option not used \n";
        }
}

########################################
##### Process the command line #########
########################################
sub ProcessCommandLine
{
    my ($romspec, $boards, $roms, $publish, $help, $version);

    GetOptions('romspec=s' => \$romspec,
               'roms=s' => \$roms,
               'boards=s' => \$boards,
               'logdir=s' => \$logdir,
               'buildnum=s' => \$buildnum,
               'publish=s' => \$publish,
               'help' => \$help,
               'version' => \$version)|| die Usage();
    
    return ($romspec, $boards, $roms, $logdir, $buildnum, $publish, $help, $version);
}


sub Usage
{
    print <<USAGE_EOF;

Usage
-----
perl metarombuild.pl -romspec <romspec xml file> [options]

 When no options are specified, all ROMs specified in the romspec wil be built.
 
 Options:
    -logdir <path to logs directory>
    -buildnum <build number>
    -publish <location of path where rom logs will be published >
    -help
    -version

USAGE_EOF
exit 0;
}

sub Version
{
    print <<VERSION_EOF;

metarombuild.pl v1.0
Copyright (c) 2005-2009 Nokia Corporation. All rights reserved.
    
VERSION_EOF
exit 0;
}
