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
# Cl_gccxml;
# 
#

package Cl_gccxml;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	PMHelp_Mmp
	PMStartBldList
	PMBld
	PMStartSrcList
	PMStartSrc
	PMSrcDepend
	PMSrcBldDepend
	PMEndSrcBld
	PMEndSrc
	PMEndSrcList
	PMBitMapBld
	PMResrcBld

);
use strict;
use cl_generic;
use Genutl;
use File::Path;
use Cwd;
use Pathutl;

sub PMHelp_Mmp {
	print "// No additional keywords for this platform\n";
}

	# takes an 'expression'  to evaluate with $_ bound to each of the
	# remaining args
	sub PrintList
	{
		my $expr = shift @_;
		foreach (@_) {
		my $str = eval($expr);
		&main::Output($str);
		}
	}

my $epocroot = $ENV{EPOCROOT};
$epocroot=~ s-\\$--;            # chop trailing \\
my $Makecmd;
sub PMStartBldList($) {
	($Makecmd) = @_;
	my $BasicTrgType=&main::BasicTrgType;
	my @BldList=&main::BldList;
	my @ChopSysIncPaths=&main::Path_Chop(&main::SysIncPaths);
	my @ChopUserIncPaths=&main::Path_Chop(&main::UserIncPaths);
	my $RelPath="$epocroot\\EPOC32\\RELEASE";
	my @MacroList=&main::MacroList();
	my $VariantFile=&main::VariantFile();
	my $Trg=&main::Trg;
	my $DPath = &main::Path_RltToWork($epocroot);
	my $DefFile = &main::DefFile;
	my $EABIDefFile = &main::EABIDef;
	my $ExtCompileOption = &main::CompilerOption("GCCXML");
	
	&Generic_Header(0,$Makecmd);	# define standard things using absolute paths

	$DefFile="" if(!(-e $DefFile));
	&main::Output(
		"GXPTRGUDEB = $RelPath\\GCCXML\\UDEB\n",
		"GXPTRGUREL = $RelPath\\GCCXML\\UREL\n\n",
		"DEFFILE = $DefFile\n\n",
        "EABIDEFFILE = $EABIDefFile\n\n"
	);

	&main::Output(
		"INCDIR  ="
	);

	foreach (@ChopUserIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}

	&main::Output(
		" -I- "
	);

	foreach (@ChopSysIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}
	if($VariantFile){
	    &main::Output("\\\n  -include \"$VariantFile\"");
	}
	&main::Output(
		"\n",
		"\n"
	);

	&main::Output( 		
		"GCCFLAGS= -nostdinc -Wall -Wno-ctor-dtor-privacy -Wno-unknown-pragmas -UWIN32 -fshort-wchar -quiet -w $ExtCompileOption\n",  # -fsyntax-only
		"\n"
	);

	&main::Output(
		"GCCDEFS = -D __EABI__ -D __SUPPORT_CPP_EXCEPTIONS__"
	);
	foreach(@MacroList) {
		&main::Output(
			" -D$_"
		);
	}
	&main::Output(
		" \$(USERDEFS)\n",
		"\n"
	);

	foreach (@BldList) {
		&main::Output(
			"GCC$_ = gccxml_cc1plus -bi"
		);
		if (/REL$/o) {
			&main::Output(
				      ' -fomit-frame-pointer '
			);
		}
		elsif (/DEB$/o) {
			&main::Output(
				' -g'
			);
		}
		&main::Output(
			' $(GCCFLAGS)'
		);
		foreach (&main::MacroList($_)) {
			&main::Output(
				" -D$_"
			);
		}
                
		&main::Output(
			" \$(GCCDEFS)\n"
		);
	}
	&main::Output(
		"\n",
		"\n"
	);
	
	&main::Output(
			"LIBRARY:\n",
			"\t\@echo Nothing to do\n"
	);

	# Resource building is done entirely via cl_generic.pm
	PrintList("\"\nRESOURCE\$_ : MAKEWORK\$_\"", @BldList)
	&main::Output(
		"\n",
		"\n",
	);

	foreach (@BldList) {
		&main::Output(
			"$_ :"
		);

		if ($BasicTrgType !~ /IMPLIB/io) {
			my $TrgBase = &main::Path_Split('base',$Trg);           
			&main::Output (
			" \\\n\t",
			&Generic_Quote("\$(GXPTRG$_)\\$TrgBase$BasicTrgType.gxp")
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
	}
}

# DumpToFile
sub DumpMMPToFile
{
	my $rfiFileList;
	my @rfiFiles;
	if(scalar @_) # if an argument has been passed then it is a reference to the list of .RFI files.
	{
		($rfiFileList) = @_;
		@rfiFiles = @$rfiFileList;
	}

	my $BldPath=&main::BldPath;
	my $Target = &main::Trg; 
	my $TargetBase = &main::Path_Split('Base', $Target); 
	my $Gxpurel="$epocroot\\epoc32\\release\\gccxml\\urel";
	my $Gxpudeb="$epocroot\\epoc32\\release\\gccxml\\udeb";
	unless (-d $BldPath) {
		mkpath([$BldPath]);
	}
	unless (-d $Gxpurel) {
		mkpath([$Gxpurel]);
	}
	unless (-d $Gxpudeb) {
		mkpath([$Gxpudeb]);
	}
    $BldPath = (&main::Path_StepDirs($BldPath))[-2];
	use File::Basename;
    my $FileName = basename(&main::MmpFile());
    $FileName = $BldPath.$FileName.".xml";
    open FILE,"> $FileName" or die "Cannot open mmp info dump file $FileName: $!";
    print FILE "<?xml version=\"1.0\"?>\n<mmpInfo>\n"; 
   
    my $Path = &main::RelPath();
    my $MmpFile = &main::MmpFile();  
    my $TrgType = &main::TrgType(); 
    my $TrgPath = &main::TrgPath(); 
    print FILE "\t<mmp path=\"$MmpFile\"/>\n"; 
	print FILE "\t<target name=\"$Target\" type=\"$TrgType\" ";
    
	if(not $TrgPath eq '') 
	{
		print FILE "path=\"$TrgPath\""; 
	}
	print FILE "/>\n";
	# insert current working directory
	my $WorkPath = &main::Path_WorkPath();
	print FILE "\t<cwd path=\"$WorkPath\"/>\n";
    
    my $ABI = &main::ABI; 
    print FILE "\t<abi type=\"$ABI\"/>\n"; 
    my $LinkAs = &main::LinkAs;
    print FILE "\t<linkAs name=\"$LinkAs\"/>\n"; 
    my $LinkAsBase = &main::LinkAsBase;
    print FILE "\t<linkAsBase name=\"$LinkAsBase\"/>\n"; 
    
    # Uids
    my @UidList=&main::UidList;
    my $count = 0;
    print FILE "\t<uids";
    foreach my $uid(@UidList)
    {
		print FILE " u$count=\"$uid\"" ;
		$count++; 
    }
    print FILE "/>\n";
    
    # Versioning 
    my %Version = &main::Version; 
    print FILE "\t<version";
    foreach my $var (sort keys %Version)
    {
        print FILE " $var=\"$Version{$var}\"";
    }
    print FILE "/>\n";
    
    # Capabilities
    my $Capability = &main::Capability;
    print FILE "\t<capability id=\"$Capability\"/>\n"; 
    
	# DefFile
	my $DefFile = &main::DefFile;
	$DefFile="" if(!(-e $DefFile));
	print FILE "\t<defFile path=\"$DefFile\" type=\"GCC\"/>\n";

	# EABIDefFile
	my $EABIDefFile = &main::EABIDef;
	print FILE "\t<defFile path=\"$EABIDefFile\" type=\"EABI\"/>\n";
        
    # Handle All types of libraries
    my $FirstLib = &main::FirstLib; 
    my @LibList = &main::LibList;
    my @ASSPLibList = &main::ASSPLibList;
    my @StatLibList = &main::StatLibList;;    
    print FILE "\t<libs>\n"; 
    
	#first
    print FILE "\t\t<lib name=\"$FirstLib\" type=\"First\"/>\n"; 
    
    #  normal
    foreach my $lib (@LibList)
    {
        print FILE "\t\t<lib name=\"$lib\"/>\n"; 
    }
    # ASSP Specific
    foreach my $lib (@ASSPLibList)
    {
        print FILE "\t\t<lib name=\"$lib\" type=\"ASSP\"/>\n";
    }
    
    # Static Libraries
    foreach my $lib (@StatLibList)
    {
        print FILE "\t\t<lib name=\"$lib\" type=\"Static\"/>\n";
    }
    print FILE "\t</libs>\n";

    # Resources
    print FILE "\t<resources>\n";
    my $rfiFile;
    foreach $rfiFile (@rfiFiles)
    {
	    print FILE "\t\t<resource name=\"$rfiFile\"/>\n";
    }
    print FILE "\t</resources>\n";

    close FILE;
}
	
sub PMBld {
	my @ASSPLibList=&main::ASSPLibList;
	my @SrcList=&main::SrcList;
	my $BaseTrg=&main::BaseTrg;
	my $Bld=&main::Bld;
	my $ChopBldPath=&main::Path_Chop(&main::BldPath);
	my $EPOCIncPath=&main::EPOCIncPath;
	my $BasicTrgType=&main::BasicTrgType;
	my $ChopRelPath=&main::Path_Chop(&main::RelPath);
	my $RelPath=&main::RelPath;
	my $Trg=&main::Trg;
	my $TrgType=&main::TrgType;
	my @UidList=&main::UidList;
	# DefFile
	my $DefFile = &main::DefFile;
	# EABIDefFile
	my $EABIDefFile = &main::EABIDef;

	my $rfiList = GatherResourceInformation();
	my @RfiFiles = @$rfiList;



	DumpMMPToFile(\@RfiFiles);

	# REAL TARGETS
	#-------------
	&main::Output(
		"# REAL TARGET - BUILD VARIANT $Bld\n",
		"\n"
	);

	&main::Output(
		"WHAT$Bld : WHATGENERIC\n",
		"\n",
		"CLEAN$Bld : CLEANBUILD$Bld CLEANRELEASE$Bld\n",
		"\n",
		"CLEANBUILD$Bld : \n",
		"\t\@perl -S ermdir.pl \"\$(EPOCBLD$Bld)\"\n",
		"\n",
		"CLEANRELEASE$Bld : CLEANGENERIC\n",
		"\n"
	);
	my @releaseables;
	&Generic_WhatCleanTargets($Bld, "WHAT$Bld", "CLEANRELEASE$Bld", @releaseables);
	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopBldPath);
	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopRelPath);
	&Generic_Releaseables;

	&main::Output(
		"\n",
		"\n"
	);

	# XML Stuff
	&main::Output(
				&Generic_Quote("\$(GXPTRG$Bld)\\$BaseTrg$BasicTrgType.gxp"), " : ",
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg$BasicTrgType.gxp"),
				"\n\tcopy \"\$(EPOCBLD$Bld)\\$BaseTrg$BasicTrgType.gxp\" \"\$@\""
	); 
	
	&main::Output(
		"\n",
		"\n"
	);
        
	# TARGET *.GXP
	#------------
	if (scalar @SrcList >150) {
		# deal with very long lists by splitting them into 150 file pieces, which allows
		# about 200 bytes per filename if the underlying max size is 32K
		#

		my $counter1=150;	# i.e. trigger new variable immediately
		my $counter2=0;
		my @objvarlist=();
		foreach (@SrcList) {
			if ($counter1==150) {
				$counter1=0;
				$counter2++;
				my $objvar = "OBJECTS$Bld$counter2";
				push @objvarlist, " \$($objvar)";
				&main::Output(
					"\n",
					"$objvar="
				);
			}
			my $BaseSrc = &main::Path_Split('Base', $_);
			my $Ext = &main::Path_Split('Ext', $_);
			$BaseSrc.='_' if (lc($Ext) eq '.cia');
			&main::Output(
				" \\\n\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.xml")
			);
			$counter1++;
		}
		use File::Basename;
	    my $FileName = basename(&main::MmpFile()).".xml";
		&main::Output(
			" \\\n\t\$(EPOCBLD)\\$FileName",
			"\n",
			"\n",
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg$BasicTrgType.gxp"), " : ", @objvarlist,"\n",
			"\tif exist \"\$\@\" del \"\$\@\"\n"
		);
		foreach (@objvarlist) {
			&main::Output(
				"\tzip -j \$\@$_\n"
			);
		}
		&main::Output(
			"\tif exist \$(EABIDEFFILE) zip -j \$\@ \$(EABIDEFFILE)\n",
			"\n\n"
		);
	} else {
		# shorter lists remain unchanged
		#
		&main::Output(
			"OBJECTS$Bld="
		);
		use File::Basename;
	    my $FileName = basename(&main::MmpFile()).".xml";

		&main::Output("\$(EPOCBLD)\\$FileName");
        foreach (@SrcList) {
			my $BaseSource = &main::Path_Split('Base', $_);
			my $Ext = &main::Path_Split('Ext', $_);
			$BaseSource.='_' if (lc($Ext) eq '.cia');
			&main::Output(
			" \\\n\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSource.xml")
			);
		}       
		
		&main::Output(
			"\n",
			"\n"
		);

		&main::Output("RESOURCEINFO$Bld=");
		foreach (@RfiFiles)
		{
			&main::Output(" \\\n\t\$(EPOCBLD)\\$_");
		}
		&main::Output("\n\n");

		&main::Output(

			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg$BasicTrgType.gxp"), " : \$(OBJECTS$Bld) \$(EABIDEFFILE) \$(RESOURCEINFO$Bld)\n",

			"\tif exist \"\$\@\" del \"\$\@\"\n",
			"\tzip -j \$\@ \$^ \$(EABIDEFFILE) \$(RESOURCEINFO$Bld)\n",
			"\n\n"
		);
	}
}


sub PMStartSrcList {

	&main::Output(
		"# SOURCES\n",
		"\n"
	);
}

sub PMBitMapBld {
	&Generic_BitMapBld;
}

sub PMResrcBld {
	&Generic_ResrcBld;
}

sub PMStartSrc {
	my $Src=&main::Src;

	&main::Output(
		"# Source $Src\n",
		"\n"
	);
}

sub PMSrcDepend {
	my @BldList=&main::BldList;	
	my @DepList=&main::DepList;
	my $BaseSrc=&main::BaseSrc;
	my $ExtSrc=&main::ExtSrc;
	my $cia = (lc($ExtSrc) eq '.cia') ? "_" : "";

	return if (@DepList == 0);

	foreach (@BldList) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc$cia.lis"), " ",
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc$cia.xml"), " \\\n",
		);
	}
	&main::Output(
		":"
	);
	foreach (@DepList) {
		&main::Output(
		" \\\n\t", &Generic_Quote($_)
		);
	}
	&main::Output(
		"\n",
		"\n"
	);
}

sub PMSrcBldDepend {
	my $Bld=&main::Bld;
	my @DepList=&main::DepList;
	my $BaseSrc=&main::BaseSrc;
	my $ExtSrc=&main::ExtSrc;
	my $cia = (lc($ExtSrc) eq '.cia') ? "_" : "";

	return if (@DepList == 0);

	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc$cia.lis"), " ",
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc$cia.xml"), " :",
	);
	foreach (@DepList) {
		&main::Output(
			" \\\n\t", &Generic_Quote($_)
		);
	}
	&main::Output( 
		"\n",
		"\n"
	);
}

my %sources;

sub PMEndSrcBld {
	my $ABI=&main::ABI;
	my $BaseSrc=&main::BaseSrc;
	my $Bld=&main::Bld;
	my $Src=ucfirst lc &main::Src;
	my $SrcPath=&main::SrcPath;
	my $Ext = &main::Path_Split('Ext', $Src);
	my $Cia = (lc($Ext) eq '.cia') ? 1 : 0;

	my $ChopSrcPath=&main::Path_Chop($SrcPath);

	# add source path, source file and build type to hashmap. 
    $sources{$SrcPath}{$Src}{$Bld}=1;
   
	# Use GCC trick to get assembler source files preprocessed with CPP
	$Src =~ s/\.s$/.S/i;

	if ($Cia) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.xml"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(GCC$Bld) -D__CIA__ -I \"$ChopSrcPath\" \$(INCDIR) -fxml=\$\@ -o nul \"$ChopSrcPath\\$Src\"\n",
			"\n",
		);
	} else {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc.xml"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(GCC$Bld) -I \"$ChopSrcPath\" \$(INCDIR) -fxml=\$\@ -o nul \"$ChopSrcPath\\$Src\"\n",
			"\n",
		);
	}

	#Keep the staus of the dependency-generation-flag was enabled or disabled.
	my $Deps_Stat = &main::Deps_GetNoDependencies();
	if($Deps_Stat){
		#If the no-dependency-generation-flag turned on, turn it off temporarily, so that we can get 
		#the dependencies.
		&main::Deps_SetNoDependenciesStatus(0);
	}
	# Code to print the log file includeheaders.txt
	my $IncFileName = "$epocroot\\epoc32\\release\\gccxml\\includeheaders.txt";
	open INC_FILE,">> $IncFileName" or die "Cannot open file $IncFileName";

	#Get the dependencies for the current source file.
	my @DepList=&main::Deps_GenDependsL("$SrcPath$Src");
	foreach(@DepList) {
		print INC_FILE "$_\n";
	}
	close INC_FILE;
	
	#Set the dependency-generation-flag to its original state so that, it doesn't affect the other target 
	#builds.
	&main::Deps_SetNoDependenciesStatus($Deps_Stat);

	# Code to print the existance of the file in logs
	my $neededfile="$epocroot\\epoc32\\release\\gccxml\\includeheaders.txt";
	if (!-e $neededfile)
	{ 
		print "!!! File $epocroot\\epoc32\\release\\gccxml\\includeheaders.txt does not exist\n";
	}
}

sub PMEndSrc {
	&main::Output(
		"\n",
		"\n"
	);
}

sub PMEndSrcList {
        my $BldPath=&main::BldPath;
        my $Target = &main::Trg; 
        my $TargetBase = &main::Path_Split('Base', $Target);
		use File::Basename;
	    my $FileName = basename(&main::MmpFile());
	    $FileName = $BldPath.$FileName.".xml";
        open FILE,">> $FileName" or die "Cannot open mmp info dump file $FileName: $!";
		my $path;
        foreach $path (keys %sources)
        {
            my %thing =  %{$sources{$path}}; # {"UREL"}; 
            my $file;
            foreach $file (keys %thing)
            {
                if( defined $thing{$file}{"UREL"})
                {
                    if( defined  $thing{$file}{"UDEB"} )
                    {
                        print FILE "\t<sourceFile name=\"$file\" path=\"$path\"/>\n";
                    }
                    else
                    {
                        print FILE "\t<sourceFile name=\"$file\" path=\"$path\" type=\"UREL\"/>\n";
                    }
                }
                else
                {
                    if(defined $thing{$file}{"UDEB"})
                    {
                        print FILE "\t<sourceFile name=\"$file\" path=\"$path\" type=\"UDEB\"/>\n";
                    }
                    else
                    {
                        die "Should Never Get HERE!";
                    }
                }
                
            }
        }
        


        my $BldInfDir=&Path_WorkPath;
        my $PrjBldDir=$E32env::Data{BldPath};
        $PrjBldDir=~s-^(.*)\\-$1-o;
        $PrjBldDir.=$BldInfDir;
        $PrjBldDir=~m-(.*)\\-o; # remove backslash because some old versions of perl can't cope


        ### the export call is like this
        my $lCmd = "make -r -f \"${PrjBldDir}EXPORT.make\" WHAT";
        my @lExportDump = split(/\n/,`$lCmd`);
        foreach my $lLine (@lExportDump) {
            $lLine =~ s/"//g;
            chomp($lLine);
            if($lLine =~ /.*\.h/i) {
                print FILE "\t<export name=\"$lLine\"/>\n";
            }
        }

		print FILE "</mmpInfo>\n\n";
        close FILE;



		&Generic_End;
}


sub GetRelDir
{
	my ($fileName) = @_;
	$fileName = CleanPath($fileName); # make sure that /'s are use rather than \ or \\
	if($fileName!~m-\/-) # no directory, just a file name, so return ./
	{
		return "./";
	}
	elsif($fileName=~m-^(.*\/)[^\/]+$-) # return directory
	{
		return $1;
	}
	else # don't know how could get here
	{
		return "./";
	}
}


sub SearchMmpFile
{
	my @rfiFiles;
	my @SystemIncludes = &main::SysIncPaths;
	my @UserIncludes = &main::UserIncPaths;
	my $ResourceStruct = &main::ResourceStructRef;
	
	foreach my $ResourceStructure (@$ResourceStruct)
	{
		push @rfiFiles, AnalyseResourceFile($$ResourceStructure{Source}, \@SystemIncludes, \@UserIncludes);
	}
	return \@rfiFiles;
}

sub AnalyseResourceFile
{ # when get to here $rssFile should be fully qualified relative to mmp file location (i.e. base: .../name.rss)
	my ($rssFile, $systemIncludeList, $userIncludeList) = @_;
	my @resourceRelatedFiles = ($rssFile);
	my $baseFile = 0; #flag to indicate whether this is the base file which the rfi file is named after
	my $rssItem;
	my $containerFile;
	foreach $rssItem (@resourceRelatedFiles) #@resourceRelatedFiles is added to during this foreach loop as included files are found
	{
		$rssItem = CleanPath($rssItem);
		if( ! ($baseFile) )
		{
			$baseFile = 1; # set to non-zero so that setting up the rfi file is only done once
			if($rssItem =~m-\/-)
			{
				if($rssItem =~m-^\S*\/([^\/]+)$-) # just extracts file name
				{
					$containerFile = $1 . ".rfi";
				}
			}
			else
			{
				$containerFile = $rssItem . ".rfi";
			}
			open CONTAINER, ">$containerFile";
		}
		OutputHeader($rssItem);
		my $resourceFiles = ReadFile($rssItem, \@resourceRelatedFiles, $systemIncludeList, $userIncludeList);
		@resourceRelatedFiles = @$resourceFiles;
	}
	close CONTAINER;
	return $containerFile;
}

sub CheckForInclude
{ # check whether the passed line from the resource type file is a #include line, if it is then store it to be added to the rfi file
	my ($line, $dir, $resourceFiles, $systemIncludeList, $userIncludeList) = @_;
	my @resourceFiles = @$resourceFiles;
	if($line =~ /^\s*\#include\s+([\<\"])(\S+)([\>\"])(\s+\/\/.*)?/)
	{
		if( ($1 eq "\"") and ($3 eq "\"") )
		{
			my $possibleAddition = SearchDirectories($2, "quoted", $dir, $systemIncludeList, $userIncludeList);
			if($possibleAddition ne "")
			{
				push @resourceFiles, $possibleAddition;
			}
		}
		elsif( ($1 eq "\<") and ($3 eq "\>") )
		{
			my $possibleAddition = SearchDirectories($2, "angle", $dir, $systemIncludeList, $userIncludeList);
			if($possibleAddition ne "")
			{
				push @resourceFiles, $possibleAddition;
			}
		}
	}
	return \@resourceFiles;
}

sub ReadFile
{ # copy the passed file into the rfi file and check it for included files
	my ($fileName, $resourceFiles, $systemIncludeList, $userIncludeList) = @_;
	my $dir = GetRelDir($fileName);

	open RESOURCE, $fileName or die "Can't open file $fileName from " . cwd() . "\n";
	my $line;
	foreach $line (<RESOURCE>)
	{
		print CONTAINER $line;
		$resourceFiles = CheckForInclude($line, $dir, $resourceFiles, $systemIncludeList, $userIncludeList);
	}
	close RESOURCE;
	return $resourceFiles;
}

sub OutputHeader
{
	my ($fileName) = @_;
	print CONTAINER "\n\n/* GXP ***********************\n";
	if($fileName =~m-\/-)
	{ # remove path as only want to store file name
		if($fileName =~m-^\S*\/([^\/]+)$-)
		{
			print CONTAINER " * $1\n";
		}
	}
	else
	{
		print CONTAINER " * $fileName\n";
	}
	print CONTAINER " ****************************/\n\n";
}


sub RecordSystemIncludes
{
	my ($line) = @_;
	my @terms = split(/ /, $line);
	my $term;
	my @systemIncludes = ();
	foreach $term (@terms)
	{
		if($term!~m/\/\//) # if term is not the start of a c++ style comment
		{
			push @systemIncludes, $term;
		}
		else
		{
			last;
		}
	}
	return \@systemIncludes;
}

sub RecordUserIncludes
{
	my ($line) = @_;
	my @terms = split(/ /, $line);
	my $term;
	my @userIncludes = ();
	foreach $term (@terms)
	{
		if($term!~m/\/\//) # if term is not the start of a c++ style comment
		{
			push @userIncludes, $term;
		}
		else
		{
			last;
		}
	}
	return \@userIncludes;
}

sub CleanPath # change \ and \\ in path to /
{
	my ($fileName) = @_;
	$fileName =~ s-\\\\-\/-og;
	$fileName =~ s-\\-\/-og;
	return $fileName;
}

sub RecordSourcePath
{
	my ($line) = @_;
	my $sourcePath;
	if($line=~/^(\S+)/) # in case of comments at end of line
	{
		$sourcePath = $1;
		$sourcePath = CleanPath($sourcePath);
		if($sourcePath !~ m/\/$/)
		{
			$sourcePath .= "\/";
		}
	}
	return $sourcePath;
}

sub SearchDirectories
{
	my ($fileName, $includeType, $base, $systemIncludeList, $userIncludeList) = @_;
	my @systemIncludes = @$systemIncludeList;
	my @userIncludes = @$userIncludeList;

	$fileName = CleanPath($fileName);

	if(-e $base.$fileName)
	{
		return $base.$fileName;
	}
	if($includeType eq "quoted")
	{
		# search through the user includes and return dir + file name if found
		my $directory;
		foreach $directory (@userIncludes)
		{
			my $qualifiedFileName = $directory . "/" . $fileName;
			if(-e $qualifiedFileName)
			{
				return $qualifiedFileName;
			}
		}
	}

	# search through the system includes
	my $directory;
	foreach $directory (@systemIncludes)
	{
		my $qualifiedFileName = $directory . "/" . $fileName;
		if(-e $qualifiedFileName)
		{
			return $qualifiedFileName;
		}
	}
	return "";
}


sub GatherResourceInformation
{
	my $BldPath=&main::BldPath;
	unless (-d $BldPath) {
		mkpath([$BldPath]);
	}
	$BldPath = (&main::Path_StepDirs($BldPath))[-2];
	my $bldInfDirectory = cwd(); # store current directory so can return to it later
	my $MmpFile = &main::MmpFile();
	$MmpFile = CleanPath($MmpFile);
	my $MmpFileDir = "./";
	my $MmpFileName = $MmpFile;
	if($MmpFile=~m-\/-)
	{
		if($MmpFile=~m-^(.*)\/([^\/]*)$-)
		{
			$MmpFileDir = $1;
			$MmpFileName = $2;
		}
	}
	chdir($MmpFileDir) or die "Error: Could not change to MMP file directory: $MmpFileDir\n";
	
	my $rfiFileList = &SearchMmpFile; # analyse the mmp file for resource files

	my @RfiFiles = @$rfiFileList;
	foreach (@RfiFiles) # copy the rfi files to the BldPath and delete them from temporary locations
	{
		system("copy \"$_\" \"$BldPath$_\"");
		unlink $_;
	}
	chdir($bldInfDirectory); # return to the orignial directory

	return \@RfiFiles; # return list of rfi files to be added to .mmp.xml file
}
1;
