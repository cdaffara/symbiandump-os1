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
# this package contains generic routines to handle bits of makefiles which are
# common to all of the platforms. Currently it deals with AIF, MBM amd RSC files.
# 
#

package cl_generic;

require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	Generic_Define
	Generic_Definition
	Generic_MakeWorkDir
	Generic_MakeWorkFile
	Generic_Quote
	Generic_Header
	Generic_Releaseables
	Generic_BitMapBld
	Generic_ResrcBld
	Generic_AifBld
	Generic_End
	Generic_CopyAction
	Generic_WhatCleanTargets
	Generic_WhatTargets
	Generic_CleanTargets
);

use strict;
use File::Basename;
use lockit_info;
use CheckSource;
use E32Variant;

my $SavedBldPath;
my $MakefileType=0;	# 0=NMAKE, 1=MAKE
my %CheckSourceResourceIncludes;

sub Generic_Header ($$;$) {
	my ($UseRltPaths, $makefileType, $supportAbsPathConversion) = @_;
	
	$MakefileType=($makefileType eq "nmake")? 0:1;

	my $LibSubDir="UDEB";
	if (&main::PlatOS eq 'EPOC32') {
		$LibSubDir="UREL";
	}
	elsif (&main::PlatOS eq 'TOOLS2') {
		$LibSubDir="LIB";
	}

	my $BldPath=&main::BldPath;

	if ( LocalBuildPathExists() ) {
		$BldPath = ConvertToLocalBuild($BldPath);
	}

	# Get the information regarding supporting Compiler Wrapper option
	my $IsCompilerWrapperOption=&main::CompilerWrapperOption();
	my $RelPath=&main::RelPath;
	my $LibPath=&main::LibPath;
	my $LinkPath=&main::LinkPath;
	my $StatLinkPath=&main::StatLinkPath;
	my $ASSPLinkPath=&main::ASSPLinkPath;

	$SavedBldPath=&main::Path_Chop($BldPath);

	if ($UseRltPaths) {

		if ( ! LocalBuildPathExists() ) {
			# Local build does not exists so keep relative path.
			$BldPath     =&main::Path_RltToWork($BldPath);
		}

		$RelPath     =&main::Path_RltToWork($RelPath);
		$LibPath     =&main::Path_RltToWork($LibPath);
		$LinkPath    =&main::Path_RltToWork($LinkPath);
		$StatLinkPath=&main::Path_RltToWork($StatLinkPath);
		$ASSPLinkPath=&main::Path_RltToWork($ASSPLinkPath);
	}

	$BldPath=&main::Path_Chop($BldPath);
	$RelPath=&main::Path_Chop($RelPath);
	$LibPath=&main::Path_Chop($LibPath);
	$LinkPath=&main::Path_Chop($LinkPath);
	$StatLinkPath=&main::Path_Chop($StatLinkPath);
	$ASSPLinkPath=&main::Path_Chop($ASSPLinkPath);

	my $EPOCDataPath=&main::Path_Chop(&main::EPOCDataPath);
	my $EPOCIncPath=&main::Path_Chop(&main::EPOCIncPath);
	my $DataPath=&main::Path_Chop(&main::DataPath);
	my $TrgPath=&main::Path_Chop(&main::TrgPath);

	my $erasedefn = "\@erase";
	$erasedefn = "\@erase 2>>nul" if ($ENV{OS} eq "Windows_NT");

	&main::Output(
		"\n",
		'# CWD ',             &main::Path_WorkPath, "\n",
		'# MMPFile ',         &main::MmpFile,       "\n",
		'# Target ',          &main::Trg,           "\n",
		'# TargetType ',      &main::TrgType,       "\n",
		'# BasicTargetType ', &main::BasicTrgType,  "\n",
		'# MakefileType ', ($MakefileType==1)? "GNU":"NMAKE", "\n"
	);

	my @BldList=&main::BldList;
	my %featureVariantInfo = &main::FeatureVariantInfo;
	
	if (%featureVariantInfo)
		{
		&main::Output("# FeatureVariantName ", $featureVariantInfo{NAME}, "\n");

		foreach (@BldList)
			{
			&main::Output("# FeatureVariant".$_."Label ", $featureVariantInfo{$_."_LABEL"}, "\n");
			}
		}
		
	&main::Output(
		"\n",
		"ERASE = $erasedefn\n",
		"\n"
	);

	# Include function to perform optional DOS to Unix slash conversion
	# on absolute paths if requested from backend.
	if ($supportAbsPathConversion) {
		&main::Output(
			"ifeq \"\$(UNIX_SLASH_FOR_CC_ABS_PATHS)\" \"1\"\n",
			"define absolutePaths\n",
			"\$(subst \\,\/,\$1)\n",
			"endef\n",
			"else\n",
			"define absolutePaths\n",
			"\$1\n",
			"endef\n",
			"endif\n",
			"\n"
		);
	}

	&main::Output(
		"# EPOC DEFINITIONS\n",
		"\n"
	);
	
	Generic_Define("EPOCBLD",  $BldPath);
	if (defined $ENV{PBUILDPID}) {
		my $BldPathP = $BldPath . '$(PBUILDPID)';
		Generic_Define("EPOCBLDP", $BldPathP);
	} else {
		Generic_Define("EPOCBLDP", $BldPath);
	}
	Generic_Define("EPOCTRG",  $RelPath);
	Generic_Define("EPOCLIB",  $LibPath);
	Generic_Define("EPOCLINK", $LinkPath);
	Generic_Define("EPOCSTATLINK", $StatLinkPath);
	Generic_Define("EPOCBSFSTATLINK", $RelPath);
	Generic_Define("EPOCASSPLINK", $ASSPLinkPath);

	Generic_Define("EPOCDATA", $EPOCDataPath);
	Generic_Define("EPOCINC",  $EPOCIncPath);
	Generic_Define("TRGDIR",   $TrgPath);
	Generic_Define("DATADIR",  $DataPath);
	

	foreach (@BldList) {
		&main::Output("\n");

		my $bldOffset = "";
		$bldOffset .= $featureVariantInfo{$_."_LABEL"}."\\" if (%featureVariantInfo && !$featureVariantInfo{INVARIANT});
			
		Generic_Define("EPOCBLD$_", "\$(EPOCBLD)\\".$bldOffset.lc($_));
		if (defined $ENV{PBUILDPID}) {
			Generic_Define("EPOCBLDP$_", "\$(EPOCBLDP)\\".$bldOffset.lc($_));
		} else {
			Generic_Define("EPOCBLDP$_", "\$(EPOCBLD)\\".$bldOffset.lc($_));
		}
		Generic_Define("EPOCTRG$_", "\$(EPOCTRG)\\".lc($_));
		Generic_Define("EPOCLIB$_", "\$(EPOCLIB)\\".lc($LibSubDir));
		Generic_Define("EPOCLINK$_", "\$(EPOCLINK)\\".lc($LibSubDir));
		Generic_Define("EPOCSTATLINK$_", "\$(EPOCSTATLINK)\\".lc($_));
		Generic_Define("EPOCBSFSTATLINK$_","\$(EPOCBSFSTATLINK)\\".lc($_));
		Generic_Define("EPOCASSPLINK$_", "\$(EPOCASSPLINK)\\".lc($LibSubDir));
	}
	
	# Compiler Wrapper option support 
	# Generate the information Compiler Wrapper options in second level make file.
	if($IsCompilerWrapperOption)
	{
		&main::Output(
			"\n\n",
			"#COMPILER WRAPPER OPTION DEFINITIONS\n",
			"COMPWRAP = ",
			"$ENV{ABLD_COMPWRAP}", # Extract the tool name from environment variable
			"\n\n"
		);
	}
	# Compiler Wrapper option
	
	my $gccxml;
	if (&main::Plat =~ /gccxml/i){
		$gccxml = &main::Plat;
	}
	
	my $edg;
	if (&main::Plat =~ /edg/i){
		$edg = &main::Plat;
	}
	
	&main::Output(
		"\n",
		"# EPOC PSEUDOTARGETS\n",
		"\n"
	);

	foreach (@BldList) {
		if(!$edg){
			&main::Output(
				"$_ : MAKEWORK$_ RESOURCE$_\n\n"
			);
		}
		else {
			&main::Output(
				"$_ : MAKEWORK$_ \n\n"
			);
		}
	}

	&main::Output(	# ALL comes after DEB: because first target in makefile is the default
		"ALL :"
	);
	foreach (@BldList) {
		&main::Output(
			" $_"
		);
	}
	# # Compiler Wrapper option support 
	if($IsCompilerWrapperOption)
	{
		&main::Output(	# ALL comes after DEB: because first target in makefile is the default
			"\n\nCOMPWRAPALL : COMPWRAPUREL COMPWRAPUDEB"
		);

	}

	&main::Output(
		"\n",
		"\n",
		"CLEAN CLEANALL : CLEANBUILD CLEANRELEASE CLEANLIBRARY\n",
		"\n"
	);
	my $pseudoTarget;
	foreach $pseudoTarget ("WHAT", "RESOURCE", "CLEANBUILD", "CLEANRELEASE", 
				"MAKEWORK", "LISTING")
		{
		if(($gccxml && $pseudoTarget eq "RESOURCE") ||
		!($gccxml || $edg)){

			&main::Output(
				"\n",
				"\n",
				"$pseudoTarget $pseudoTarget","ALL :"
			);
			foreach (@BldList) {
				&main::Output(
					" $pseudoTarget$_"
				);
			}
		}	
	}
	if($gccxml){
		&main::Output(
		"\n",
		"\n",
		);
	}

	if(!($gccxml || $edg)){
		&main::Output(
			"\n",
			"\n",
			"MAKEWORK : MAKEWORKLIBRARY\n",
			"\n"
		);
	}
	if(!$edg){
		foreach (@BldList) {
			&main::Output(
				"RESOURCE$_ "
			);
		}
		&main::Output(
			": GENERIC_RESOURCE\n",
			"\n"
		);
	}
}

# Quote name if necessary, according to the type of Makefile

sub Generic_Quote ($) {
	my ($name)=@_;
	if ($MakefileType==1) {
		# GNU make wants backslash before each space
		$name =~ s/ /\\ /go;
	} else {
		# NMAKE prefers quotes around all filenames
		$name = "\"$name\"";
	}
	return $name;
}

# Generic file copying action, which uses built-in variables quoted appropriately
#
# GNU make variables may contain spaces, but perl.exe will be executed directly
# and so doesn't need any quotes.

sub Generic_CopyAction($) {
	my ($target) = @_;
	my $source = '$?';
	$target = '$@' if (!defined $target);
	return "perl -w -S ecopyfile.pl $source $target\n";
}

# Record necessary directories, for eventual emkdir.pl rules
# Also need to record related defines, to eliminate duplicates

my %DirDefines;
sub expandDefines ($) {
	my ($value)=@_;
	while ($value =~ /^(.*?)\$\((\w+)\)(.*)$/) {
		last if ($2 eq 'PBUILDPID');
		$value="$1$DirDefines{$2}$3";
	}
	return $value;
}
sub Generic_Define ($$;$) {
	my ($name, $value, $trailer)=@_;
	$trailer="" if (! defined $trailer);
	&main::Output(
		"$name = $value$trailer\n"
	);
	$DirDefines{$name}=expandDefines($value);
}
sub Generic_Definition ($) {
	return $DirDefines{$_[0]};
}

my %MkDirs;
sub Generic_MkDir ($) {
	my $dir=&main::Path_Chop(&expandDefines($_[0]));
	$dir = lc $dir;
	$dir =~ s/\$\(pbuildpid\)/\$\(PBUILDPID\)/g;
	$MkDirs{$dir}=1;
	return $dir;
}

# Accumulate MAKEWORK targets and lists of directories,
# automatically filling in the rest of the details.
#
my %MakeWork;
sub Generic_MakeWorkDir($$) {
	my ($work, $dir) = @_;

	if ( LocalBuildPathExists() ) {
		$dir = ConvertToLocalBuild($dir);
	}

	my $workhash;
	if (defined $MakeWork{$work}) {
		$workhash=$MakeWork{$work};
	} else {
		my %newhash;
		$workhash=\%newhash;
		$MakeWork{$work}=$workhash;
	}
	my $normdir=lc &main::Path_Chop(&expandDefines($dir));
	$normdir =~ s/\$\(pbuildpid\)/\$\(PBUILDPID\)/g;
	$$workhash{$normdir} ||= &Generic_MkDir($dir);
}

sub Generic_MakeWorkFile($$) {
	my ($work, $file) = @_;
	my $dir=&main::Path_Split('Path',&expandDefines($file));
	&Generic_MakeWorkDir($work,$dir);
}


sub Generic_WhatTargets($$@)
{
	my ($prefix, $whattarget, @files)=@_;
	
	if ((scalar @files) == 0) {
		&main::Output(
			"\n",
			"\n$whattarget :\n",
			"\t\@rem none\n"
		);
	} else {
# emit list of releasables in batches to avoid overflowing the 2048 character 
# batch file line limit doing echo or erase...

		my $count=1;
		my $length=0;
		&main::Output(
			"\n",
			"${prefix}_RELEASEABLES$count="
		);

		my $File;

		foreach $File (sort @files) {
			my $name = &Generic_Quote($File);
			my $namelen = length($name)+3;	# for space, newline and tab
			if ($length+$namelen > 1700) {	# very conservative limit
				$count++;
				$length=0;
				&main::Output(
					"\n",
					"${prefix}_RELEASEABLES$count="
				);
			}
			&main::Output(
				" \\\n\t", $name
			);
			$length += $namelen;
		}
		&main::Output(
			"\n",
			"\n",
			"$whattarget:\n"
		);
		my $filecount=1;
		while ($filecount<=$count) {
			&main::Output(
				"\t\@echo \$(${prefix}_RELEASEABLES$filecount)\n"
			);
			$filecount++;
		}
	}

	&main::Output(
		"\n",
	);
}
	
sub Generic_CleanTargets($$@) {
	my ($prefix, $cleantarget, @files)=@_;
	
	if ((scalar @files) == 0) {
		&main::Output(
			"\n",
			"\n$cleantarget :\n",
			"\t\@rem none\n"
		);
	} else {
# emit list of releasables in batches to avoid overflowing the 2048 character 
# batch file line limit doing echo or erase...

		my $count=1;
		my $length=0;
		&main::Output(
			"\n",
			"${prefix}_CLEANTARGETS$count="
		);

		my $File;

		foreach $File (sort @files) {
			my $name = &Generic_Quote($File);
			my $namelen = length($name)+3;	# for space, newline and tab
			if ($length+$namelen > 1700) {	# very conservative limit
				$count++;
				$length=0;
				&main::Output(
					"\n",
					"${prefix}_CLEANTARGETS$count="
				);
			}
			if (!main::NoExportLibrary || ($name !~ /.dso$/i && $name !~ /.lib$/i))
			{
				&main::Output(
					" \\\n\t", $name
				);
				$length += $namelen;
			}
		}
		&main::Output(
			"\n",
			"\n",
			"$cleantarget:\n",
		);
		my $filecount=1;
		while ($filecount<=$count) {
			&main::Output(
				"\t-\$(ERASE) \$(${prefix}_CLEANTARGETS$filecount)\n"
			);
			$filecount++;
		}
	}

	&main::Output(
		"\n",
	);
}

sub Generic_WhatCleanTargets($$$@) {
	my ($prefix, $whattarget, $cleantarget, @files)=@_;
	
	if ((scalar @files) == 0) {
		&main::Output(
			"\n",
			"\n$whattarget $cleantarget :\n",
			"\t\@rem none\n"
		);
	} else {
# emit list of releasables in batches to avoid overflowing the 2048 character 
# batch file line limit doing echo or erase...

		my $count=1;
		my $length=0;
		&main::Output(
			"\n",
			"${prefix}_RELEASEABLES$count="
		);

		my $File;

		foreach $File (sort @files) {
			my $name = &Generic_Quote($File);
			my $namelen = length($name)+3;	# for space, newline and tab
			if ($length+$namelen > 1700) {	# very conservative limit
				$count++;
				$length=0;
				&main::Output(
					"\n",
					"${prefix}_RELEASEABLES$count="
				);
			}
			&main::Output(
				" \\\n\t", $name
			);
			$length += $namelen;
		}
		&main::Output(
			"\n",
			"\n",
			"$whattarget:\n"
		);
		my $filecount=1;
		while ($filecount<=$count) {
			&main::Output(
				"\t\@echo \$(${prefix}_RELEASEABLES$filecount)\n"
			);
			$filecount++;
		}
		&main::Output(
			"\n",
			"$cleantarget:\n",
		);
		$filecount=1;
		while ($filecount<=$count) {
			&main::Output(
				"\t-\$(ERASE) \$(${prefix}_RELEASEABLES$filecount)\n"
			);
			$filecount++;
		}
	}

	&main::Output(
		"\n",
	);
}

# accumulated list of generic releasables
my %Files;	
sub Generic_End {
	&Generic_WhatCleanTargets("GENERIC","WHATGENERIC","CLEANGENERIC", keys %Files);
	
	&main::Output(
		"# Rules to create all necessary directories\n",
	);

	foreach (sort keys %MakeWork) {
		my $workhash = $MakeWork{$_};
		&main::Output(
			"\n$_ :",
		);
		foreach (sort keys %$workhash) {
			my $withcase=$$workhash{$_};
			if ($withcase =~ /\$\(PBUILDPID\)\\/) {
				&main::Output(
					" \\\n",
					"\t\$(if \$(PBUILDPID),", &Generic_Quote($withcase), ")"
				);
			} else {
				&main::Output(
					" \\\n",
					"\t", &Generic_Quote($withcase)
				);
			}
		}
		&main::Output(
			"\n"
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	my $dir;
	foreach $dir (sort keys %MkDirs) {
		if ($dir =~ /\$\(PBUILDPID\)\\/) {
			&main::Output(
				"\$(if \$(PBUILDPID),", &Generic_Quote($dir),") \\\n",
			);
		} else {
			&main::Output(
				&Generic_Quote($dir)," \\\n",
			);
		}
	}
	&main::Output(
		":\n",
		"\tperl -w -S emkdir.pl \$\@\n",
		"\n"
	);

	&main::Output(
		"CHECKSOURCE :\n",
	);

	&main::Output (CheckSource_MakefileOutput(&main::CheckSourceMMPIncludes));
	&main::Output (CheckSource_MakefileOutput(%CheckSourceResourceIncludes));
	&main::Output (CheckSource_MakefileOutput(&main::CheckSourceMMPMetaData));

	my $cs_targetprefix = "";
	$cs_targetprefix = "U" unless (&main::Plat =~ /tools/i);

	&main::Output(
		"\nCHECKSOURCE".$cs_targetprefix."REL :\n",
	);

	&main::Output (CheckSource_MakefileOutput(&main::CheckSourceURELIncludes));

	&main::Output(
		"\nCHECKSOURCE".$cs_targetprefix."DEB :\n",
	);

	&main::Output (CheckSource_MakefileOutput(&main::CheckSourceUDEBIncludes));

}

sub Generic_Releaseables {

	my $ResrcPath=&main::TrgPath;
	my $dir;
	my $EPOCIncPath=&main::EPOCIncPath;

	&Generic_MakeWorkDir('GENERIC_MAKEWORK',$SavedBldPath);	    # used for temp directory in epocrc.pl
			
	my $AifStructRef=&main::AifStructRef;
	my $AifRef;
	foreach $AifRef (@$AifStructRef) {
# regression change - workaround lack of AIF directory
		$$AifRef{TrgFile}=&main::Path_Split('File',$$AifRef{Trg});  # record for later
		my $path=&main::Path_Split('Path',"$ResrcPath$$AifRef{Trg}");  
		my $file="\$(EPOCDATA)\\$ResrcPath$$AifRef{Trg}";
		my $xip="_xip";
		my $base=&main::Path_Split('Base',"$file");
		my $root=&main::Path_Split('Path',"$file");
		my $ext=&main::Path_Split('Ext',"$file");
		my $file_xip="$root"."$base$xip$ext";	# since XIP AIF format is generated
		if ($path eq "") {
			# no target path for the AIF file, so not a releasable
			$file="$SavedBldPath\\$$AifRef{Trg}";
		} else {
			$Files{$file}=1;
			$Files{$file_xip}=1;
		}
		$$AifRef{GenericTrg}=$file;	    # record for later
		&Generic_MakeWorkFile('GENERIC_MAKEWORK',$file);
	}

	my $BitMapStructRef=&main::BitMapStructRef;
	my $BitMapRef;
	foreach $BitMapRef (@$BitMapStructRef) {
		my $path=$$BitMapRef{TrgPath};
		my $file="\$(EPOCDATA)\\$path$$BitMapRef{Trg}";
		$$BitMapRef{GenericTrg}=$file;	    # record for later
		$Files{$file}=1;
		&Generic_MakeWorkFile('GENERIC_MAKEWORK',$file);
		if ($$BitMapRef{Hdr}) {
			my $mbg=&main::Path_Split('Base', $$BitMapRef{Trg});
			$mbg="\$(EPOCINC)\\$mbg.mbg";
			&Generic_MakeWorkDir('GENERIC_MAKEWORK',$EPOCIncPath);
			$Files{$mbg}=1;
		}
	}

	my $ResourceStructRef=&main::ResourceStructRef;
	my $ResourceRef;
	foreach $ResourceRef (@$ResourceStructRef) {
		if(defined $$ResourceRef{Hdronly})
			{
				my $rsg="\$(EPOCINC)\\$$ResourceRef{BaseTrg}.rsg";
				$$ResourceRef{GenericTrg}=$rsg;	# record for later

				&Generic_MakeWorkDir('GENERIC_MAKEWORK',$EPOCIncPath);
				$Files{$rsg}=1;			
			}
		else
			{
				my $file="\$(EPOCDATA)\\$$ResourceRef{Trg}";
				$$ResourceRef{GenericTrg}=$file;	# record for later
				$Files{$file}=1;
				&Generic_MakeWorkFile('GENERIC_MAKEWORK',$file);
				if ($$ResourceRef{Hdr}) {
					my $rsg="\$(EPOCINC)\\$$ResourceRef{BaseTrg}.rsg";
					&Generic_MakeWorkDir('GENERIC_MAKEWORK',$EPOCIncPath);
					$Files{$rsg}=1;
				}
			}
	}

# Do StringTable 'export'
	my @stringTables = &main::StringTables();
	foreach my $stringtable (@stringTables)
	{
		if(defined $stringtable->{ExportPath})
		{
			$Files{$stringtable->{ExportPath}."\\".$stringtable->{BaseTrg}.".h"} = 1;
		}
	}


	&main::Output(
		"GENERIC_RESOURCE : GENERIC_MAKEWORK\n",
		"\n"
	);

}

sub Generic_BitMapBld {

	my $BitMapRef=&main::BitMapRef;
	my $quotedTrg=&Generic_Quote($$BitMapRef{GenericTrg});
	my $WorkPath = &main::Path_Chop(&main::Path_WorkPath); ## required for Lockit

	&main::Output(
		"# BitMap $$BitMapRef{Trg}\n",
		"\n",
		"GENERIC_RESOURCE : $quotedTrg\n",
		"\n",
		"$quotedTrg :"
	);

#	must lower-case header here since bmconv generates a header with case-sensitive enums accordingly
	my $BitMapHdr=join('', &main::EPOCIncPath, &main::Path_Split('Base', $$BitMapRef{Trg}), '.mbg');
	my $TmpBitMapHdr=join('', &main::BldPath, &main::Path_Split('Base', $$BitMapRef{Trg}), '.mbg');

	my $SrcRef;
	foreach $SrcRef (@{$$BitMapRef{Source}}) {
	
		if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
			%Files = &Lockit_Releasables($WorkPath, $$BitMapRef{Trg}, \%Files, basename($$SrcRef{Src}),"","");		
		}
		else {
			%Files = &Lockit_Releasables($WorkPath, $$BitMapRef{Trg}, \%Files, basename($$SrcRef{Src}));
		}
		&main::Output(
			" \\\n  ", &Generic_Quote($$SrcRef{Src})
		);
	}
	&main::Output(
		"\n",
		"\tperl -w -S epocmbm.pl -h\"$TmpBitMapHdr\"",
		"\t-o\"$$BitMapRef{GenericTrg}\"",
		"\t-l\"\\$$BitMapRef{TrgPath}:$WorkPath\"",
		"\\\n\t\t"
	);
	&main::Output(
		" -b\""
	);
	foreach $SrcRef (@{$$BitMapRef{Source}}) {
		&main::Output(
			"\\\n\t\t/$$SrcRef{ClDepth}$$SrcRef{Src}"
		);
	}


	&main::Output(
		"\" \\\n\t\t"
	);

	&main::Output(
		" -l\"\\$$BitMapRef{TrgPath}:$WorkPath\"\n"
	);

	if ($$BitMapRef{Hdr}) {
		&main::Output(
			"\tperl -w -S ecopyfile.pl \"$TmpBitMapHdr\" \"$BitMapHdr\"\n",
		);
	}
	&main::Output(
		"\n"
	);
}

sub Generic_ResrcBld {
	my $ResourceRef=&main::ResourceRef;
	my $WorkPath = &main::Path_Chop(&main::Path_WorkPath); # required for Lockit
	my @RcompMacro=&main::MmpMacros;

	&main::Output(
		"# Resource $$ResourceRef{Trg}\n",
		"\n"
	);
	my @DepList=&main::DepList;

	&main::Output(
		"DEPEND="
	);
	foreach (@DepList) {
		&main::Output(
			" \\\n",
			"\t", &Generic_Quote($_)
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	my $BaseResrc= $$ResourceRef{BaseTrg};
	my $SrcPath=&main::Path_Split('Path', $$ResourceRef{Source});

	my $AbsSrc;
	my $AbsSrcPath;
	
	my $ResrcHdr=join '', &main::EPOCIncPath(), $BaseResrc, '.rsg';
	my $AbsResrcHdr;
	my $PlatName=&main::PlatName;

	my @ChopAbsSysIncPaths;
	my @ChopAbsUserIncPaths;

	@ChopAbsSysIncPaths=&main::Path_Chop(&main::Path_AbsToWork(&main::ResourceSysIncPaths));
	@ChopAbsUserIncPaths=&main::Path_Chop(&main::Path_AbsToWork(&main::UserIncPaths));

	$AbsSrc=&main::Path_AbsToWork($$ResourceRef{Source});
	$AbsSrcPath=&main::Path_AbsToWork($SrcPath);
	$AbsResrcHdr=&main::Path_AbsToWork($ResrcHdr);

	my $ChopAbsSrcPath=&main::Path_Chop($AbsSrcPath);

	my $EPOCDataDir = &main::Path_Chop($$ResourceRef{TrgPath});
	
	my $RscOption= "";
	my $HeaderOption = "";
	my $HeaderCopyCmd = "";
    if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha'))  {
	if ($$ResourceRef{Hdr} || $$ResourceRef{Hdronly}) {
		$HeaderOption = " -h\"$SavedBldPath\\$BaseResrc$$ResourceRef{Lang}.rsg\"";
		$HeaderCopyCmd = "\tperl -w -S ecopyfile.pl \"$SavedBldPath\\$BaseResrc$$ResourceRef{Lang}.rsg\" \"$ResrcHdr\"\n";
	}
	
	}
	else {
	if ($$ResourceRef{Hdr} || $$ResourceRef{Hdronly}) {
		$HeaderOption = " -h\"$SavedBldPath\\$BaseResrc.rsg\"";
		$HeaderCopyCmd = "\tperl -w -S ecopyfile.pl \"$SavedBldPath\\$BaseResrc.rsg\" \"$ResrcHdr\"\n";
	}
	}
	if (! $$ResourceRef{Hdronly}) {
		$RscOption = " -o\$\@ ";
	}
	
	my $Uidsref=$$ResourceRef{Uids};
	my @Uids=();
	@Uids = @{$Uidsref} if (defined($Uidsref));
	my $Uidsarg="";
	if ($#Uids>0) {
		$Uidsarg="-uid2 $Uids[0] -uid3 $Uids[1]";
	}
	elsif ($#Uids==0) {
		$Uidsarg="-uid2 $Uids[0] ";
	}
	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha'))  {
		%Files = &Lockit_Releasables($WorkPath, $$ResourceRef{Source}, \%Files, "", $$ResourceRef{Lang}) unless $$ResourceRef{Hdronly};
	}
	else {			
		%Files = &Lockit_Releasables($WorkPath,$$ResourceRef{Source}, \%Files, "") unless $$ResourceRef{Hdronly};
	}
	my $quotedTrg=&Generic_Quote($$ResourceRef{GenericTrg});
	&main::Output(
		"GENERIC_RESOURCE : $quotedTrg\n",
		"\n",
		"$quotedTrg : ", &Generic_Quote($AbsSrc), " \$(DEPEND)\n",
		"\tperl -w -S epocrc.pl -m045,046,047 -I \"$ChopAbsSrcPath\""
	);
	foreach (@ChopAbsUserIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}
	&main::Output(
		" -I-"
	);
	foreach (@ChopAbsSysIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}
	foreach(@RcompMacro) {
		&main::Output(
			" -D$_ "
		);
	}
	&main::Output(
		" -DLANGUAGE_$$ResourceRef{Lang} -u \"$AbsSrc\" ",
		" $Uidsarg ",
		"$RscOption $HeaderOption -t\"$SavedBldPath\""
	);

	if (!$$ResourceRef{Hdronly}) {
		&main::Output(
			" -l\"$EPOCDataDir:$WorkPath\""
		);				
	}

	if (&main::ResourceVariantFile()) {
		&main::Output(
			" -preinclude\"".&main::ResourceVariantFile()."\""
		);
	}
	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
		if ((lc $$ResourceRef{Lang}) eq "sc") {
	
			&main::Output(
				"\n",
				$HeaderCopyCmd,
			);
		}
		&main::Output(		
		"\n"
		);
	
	}
	else {
		&main::Output(
			"\n",
			$HeaderCopyCmd,
			"\n"
		);
	}

	my @macros;
	push @macros, "LANGUAGE_$$ResourceRef{Lang}", "_UNICODE";
	my @userIncludes = &main::Path_Chop(&main::Path_AbsToWork(&main::UserIncPaths));
	unshift (@userIncludes, $AbsSrcPath);	
	my @systemIncludes = &main::Path_Chop(&main::Path_AbsToWork(&main::ResourceSysIncPaths));

	CheckSource_Includes($AbsSrc, %CheckSourceResourceIncludes, &Variant_GetMacroHRHFile(), @macros, @userIncludes, @systemIncludes);
}

sub Generic_AifBld {

	my $AifRef=&main::AifRef;
	my $ResrcPath=&main::TrgPath;

	&main::Output(
		"# Aif $$AifRef{Trg}\n",
		"\n"
	);
	my @DepList=&main::DepList;

	&main::Output(
		"DEPEND="
	);
	foreach (@DepList) {
		&main::Output(
			" \\\n",
			"\t", &Generic_Quote($_)
		);
	}
	&main::Output(
		"\n",
		"\n"
	);

	my @ChopRTWSysIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::ResourceSysIncPaths));
	my @ChopRTWUserIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::UserIncPaths));
	my $BaseResrc=&main::Path_Split('Base',$$AifRef{Source});

	my $WorkPath = &main::Path_Chop(&main::Path_WorkPath); # required for Lockit
	my $quotedTrg=&Generic_Quote($$AifRef{GenericTrg});
	&main::Output(
		"GENERIC_RESOURCE : $quotedTrg\n",
		"\n",
		"$quotedTrg : ", &Generic_Quote("$$AifRef{Source}"), " \$(DEPEND)"
	);
	if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
	%Files = &Lockit_Releasables($WorkPath, $$AifRef{TrgFile}, \%Files, "", "");
	}
	else 
	{
	%Files = &Lockit_Releasables($WorkPath, $$AifRef{TrgFile}, \%Files, "");
	}
	my $bitmapArg = "";
	if ($$AifRef{BitMaps}) { # bitmaps aren't essential
		$bitmapArg = "\\\n\t\t-b\"";
		foreach my $BitMapRef (@{$$AifRef{BitMaps}}) {
			my $BitMap = &main::Path_Split('File', $$BitMapRef{Src});
			if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
			%Files = &Lockit_Releasables($WorkPath, $$AifRef{TrgFile}, \%Files, $BitMap, "");
			}
			else
			{
			%Files = &Lockit_Releasables($WorkPath, $$AifRef{TrgFile}, \%Files, $BitMap);
			}
			&main::Output(
				" ", &Generic_Quote("$$BitMapRef{Src}")
			);
			$bitmapArg .= "\\\n\t\t/$$BitMapRef{ClDepth}$$BitMapRef{Src}";
		}
		$bitmapArg .= "\" ";
	}
	&main::Output(
		"\n",
		"\tperl -w -S epocaif.pl -o\$\@ ",
		"\"$$AifRef{Source}\" ",
		"\\\n\t\t",
		"-t\"$SavedBldPath\" ",
		" -l\"\$(TRGDIR):$WorkPath\"",
		$bitmapArg,
		"\\\n\t\t"
	);
	foreach (@ChopRTWUserIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}
	&main::Output(
		" -I-"
	);
	foreach (@ChopRTWSysIncPaths) {
		&main::Output(
			" -I \"$_\""
		);
	}
	&main::Output(
		"\n",
		"\n"
	);
}



#
# Check to see that LOCAL_BUILD_PATH exists.
#

sub LocalBuildPathExists() 
{
	if ( defined( $ENV{LOCAL_BUILD_PATH} ) ) {
		return 1;
	}
	else {
		return 0;
	}
}


#
# Take the build path and ensure that it is now local. 
#


sub ConvertToLocalBuild($)
{
	my ($BldPath)=@_;

	my $sub_replacement = $ENV{"LOCAL_BUILD_PATH"};
	if ( !ValidBuildPath($sub_replacement) ) {
		&FatalError(" Invalid Local Build Path : LOCAL_BUILD_PATH = \"$BldPath\"  must be in format drive:dir_name1 " );
	}
	else
	{
		# Replace the \epoc32\build with local setting
		my $epocroot=$ENV{"EPOCROOT"};
		
		my $sub_match = "\Q${epocroot}\EEPOC32\\\\BUILD";
		$BldPath =~ s/${sub_match}/${sub_replacement}/;
	}

	return $BldPath
}



#
# Checks that the path is at least of the form drive:path_name
# If no drive: then the makefiles will be produced but not work. 
#


sub ValidBuildPath($)
{
	my ($BldPath)=@_;

	if ( $BldPath =~/^[a-zA-z]{1}:{1}.*$/) {
		# Must have at least d:
		return 1;
	}
	else {
		return 0;
	}

}

1;

