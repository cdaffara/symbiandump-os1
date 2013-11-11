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
# Cl_edg;
# 
#

package Cl_edg;

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
);
use cl_generic;
use Genutl;
use File::Path;
use Cwd;

sub PMHelp_Mmp {
	print "// No additional keywords for this platform\n";
}

my $epocroot = $ENV{EPOCROOT};
$epocroot=~ s-\\$--;            # chop trailing \\
my $Makecmd;
sub PMStartBldList($) {
	($Makecmd) = @_;
	my $BasicTrgType=&main::BasicTrgType;
	my @BldList=&main::BldList;
	my @ChopRTWSysIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::SysIncPaths)); # old style
	my @ChopRTWUserIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::UserIncPaths)); # old style
	my $RelPath="$epocroot\\EPOC32\\RELEASE";
	my $RPath = &main::Path_RltToWork($RelPath);
	my @MacroList=&main::MacroList();
	my $VariantFile=&main::VariantFile();
	my $Trg=&main::Trg;
	my $DPath = &main::Path_RltToWork($epocroot);
	my $DefFile = &main::DefFile;
	my $EABIDefFile = &main::EABIDef;
    
    my $EdgSysIncludeList = "Edg_sysinc_list.txt";
    my $EdgUserIncludeList = "Edg_userinc_list.txt";
    my $EdgUdebReportList = "Edg_udeb_rep_list.txt";
    my $EdgUrelReportList = "Edg_urel_rep_list.txt";

	&Generic_Header(0,$Makecmd);	# define standard things using relative paths

	# EDG report file generation
	
	&main::Output(
		"GXPTRGUDEB = $RPath\\EDG\\UDEB\n",
		"GXPTRGUREL = $RPath\\EDG\\UREL\n\n",
		"DEFFILE = $DefFile\n\n",
        "EABIDEFFILE = $EABIDefFile\n\n",
		"EDG_SYSINC_LIST=\${EPOCBLD}\\..\\..\\", $EdgSysIncludeList,
		"\n",
		"EDG_USERINC_LIST=\${EPOCBLD}\\..\\..\\", $EdgUserIncludeList,
		"\n",
		"EDG_UREL_REP_LIST=\${EPOCBLD}\\..\\..\\",$EdgUrelReportList,
		"\n",
		"EDG_UDEB_REP_LIST=\${EPOCBLD}\\..\\..\\", $EdgUdebReportList,
		"\n\n"
	);

	&main::Output(
		"INCDIR ="
	);
	

	foreach (@ChopRTWUserIncPaths) {
		&main::Output(
			" --include_directory \"$_\""
		);
	}

	&main::Output(
		"\n"
	);


	&main::Output(
		"SYSINCDIR ="
	);

	foreach (@ChopRTWSysIncPaths) {
	     &main::Output(
			" --sys_include \"$_\"",
	     );
	}

	&main::Output(
		"\n",
		"\n"
	);
	

	my $preinclude =   "--preinclude \$(EPOCINC)\\edg\\edg3_7_rvct2_2.h";#this is the variant line you must change to "simulate" another compiler.
	&main::Output( 
		"GCCFLAGS = --wchar_t_keyword --no_code_gen --exceptions --no_warnings",
		" ${preinclude}",
		"\n"
	);

	&main::Output(
		"GCCDEFS = -D __MARM_THUMB__ -D __MARM_INTERWORK__ -D __ARMCC__ -D __EPOC32__ -D __MARM__ -D __EABI__ -D __ARMCC_2__ -D __ARMCC_2_2__ -D __SUPPORT_CPP_EXCEPTIONS__  -D IMPORT_C= -D EXPORT_C= -D __softfp="
	);
	foreach(@MacroList) {
		&main::Output(
			" -D $_"
		);
	}
	if($VariantFile){
	    &main::Output(" -D __PRODUCT_INCLUDE__=\\\"${VariantFile}\\\"");
	}
	&main::Output(
		" \$(USERDEFS)\n",
		"\n"
	);

	foreach (@BldList) {
		&main::Output(
			"GCC$_ = cpfe"
		);
		
		&main::Output(
			' $(GCCFLAGS)'
		);
		foreach (&main::MacroList($_)) {
			&main::Output(
				" -D $_"
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

	&main::Output(
		"RESOURCEUREL:\n",
		"\t\@echo Nothing to do\n"
	);

	&main::Output(
		"RESOURCEUDEB:\n",
		"\t\@echo Nothing to do\n\n\n"
	);

	foreach (@BldList) {
		&main::Output(
			"$_ :"
		);

		if ($BasicTrgType !~ /IMPLIB/io) {
			my $TrgBase = &main::Path_Split('base',$Trg);           
			&main::Output (
			" \\\n\t",
			&Generic_Quote("\$(GXPTRG$_)\\$TrgBase.gxp")
			);
		}
		&main::Output(
			"\n",
			"\n"
		);
	}
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
		"CLEANRELEASE$Bld : CLEANGENERIC CLEANEDGTEMPFILES$Bld\n",
		"\n"
	);

	&main::Output("CLEANEDGTEMPFILES$Bld :\n",
			"\t\-\$(ERASE) \$(EDG_SYSINC_LIST) ",
			"\$(EDG_USERINC_LIST) ",
			"\$(EDG_${Bld}_REP_LIST) "
			);

	my @releaseables;
	&Generic_WhatCleanTargets($Bld, "WHAT$Bld", "CLEANRELEASE$Bld", @releaseables);
	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopBldPath);
	&Generic_MakeWorkDir("MAKEWORK$Bld",$ChopRelPath);

	&main::Output(
		"\n",
		"\n"
	);

	# XML Stuff
	&main::Output(
				&Generic_Quote("\$(GXPTRG$Bld)\\$BaseTrg.gxp"), " : ",
				&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg.gxp"),
	);
	my @ChopRTWSysIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::SysIncPaths)); # old style
	foreach my $incPath (@ChopRTWSysIncPaths) {
	&main::Output(
		"\n\t\@echo $incPath >> \$(EDG_SYSINC_LIST)",
		);
	}
	
	my @ChopRTWUserIncPaths=&main::Path_Chop(&main::Path_RltToWork(&main::UserIncPaths)); # old style
	foreach my $incPath (@ChopRTWUserIncPaths) {
	&main::Output(
		"\n\t\@echo $incPath >> \$(EDG_USERINC_LIST)",
		);
	}

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
                        next if (lc($Ext) eq '.cia');
			&main::Output(
				" \\\n\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc$Ext.aut")
			);
			$counter1++;
		}
		
        &main::Output(
			"\n\n"
		);
	} else {
		# shorter lists remain unchanged
		#
		&main::Output(
			"OBJECTS$Bld="
		);
                foreach (@SrcList) {
			my $BaseSource = &main::Path_Split('Base', $_);
			my $Ext = &main::Path_Split('Ext', $_);
                        next if (lc($Ext) eq '.cia');
			&main::Output(
			" \\\n\t", &Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSource$Ext.aut")
			);  
		}       
		
		&main::Output(
			"\n",
			"\n"
		);

		&main::Output("RESOURCEINFO$Bld=");
		&main::Output("\n\n");
		
		&main::Output(
            
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseTrg.gxp"), " : \$(OBJECTS$Bld) \$(EABIDEFFILE) \$(RESOURCEINFO$Bld)\n",
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
			&Generic_Quote("\$(EPOCBLD$_)\\$BaseSrc$cia.aut"), " \\\n",
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
    my $cia = (lc($ExtSrc) eq '.cia') ? "" : "";

	return if (@DepList == 0);

	&main::Output(
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc$cia.lis"), " ",
		&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc$cia.aut"), " :",
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

	my $RTWSrcPath=&main::Path_Chop(&main::Path_RltToWork($SrcPath));

	# add source path, source file and build type to hashmap. 
    $sources{$SrcPath}{$Src}{$Bld}=1;
   
	# Use GCC trick to get assembler source files preprocessed with CPP
	$Src =~ s/\.s$/.S/i;

	@browser = ();
	my $EdgReportFile = "\$(EDG_UREL_REP_LIST)";
	if($Bld =~ /udeb/i){
		$EdgReportFile = "\$(EDG_UDEB_REP_LIST)";
	}

	if ($Cia) {
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc\_.aut"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(GCC$Bld) -D __CIA__ --include_directory \"$RTWSrcPath\" \$(INCDIR) --xref \"\$\@\" \"$RTWSrcPath\\$Src\"\n",
		"\t\@echo \"\$\<\" >> $EdgReportFile\n",
		"\n");
	} else {
		
                my $BldPath=&main::BldPath;
                my $EdgReportFilePath=UpDir(UpDir(UpDir($BldPath)));
		
		&main::Output(
			&Generic_Quote("\$(EPOCBLD$Bld)\\$BaseSrc$Ext.aut"), " : ",
			&Generic_Quote("$SrcPath$Src"), "\n",
			"\t\$(GCC$Bld) --include_directory \"$RTWSrcPath\" \$(INCDIR) \$(SYSINCDIR) --xref \"\$\@\" \"$RTWSrcPath\\$Src\"\n",
			"\t\@echo \$(EPOCBLD$Bld)\\$BaseSrc$Ext.aut >> $EdgReportFile\n",
			"\n"
		);

		foreach $browser (@browser)
		{
		 print "$browser\n";
		}
	}
}

sub PMEndSrc {
	&main::Output(
		"\n",
		"\n"
	);
}

sub PMEndSrcList {
		&Generic_End;
}



sub UpDir($)
	{
	my ($aPath) = @_;

	my $lastChar = substr($aPath,-1);
	my $end = length($aPath)-1;

	if ( ($lastChar eq '\\') || ($lastChar eq '/') )
		{
		$end -= 1;
		}

	my $last = rindex($aPath,'\\',$end);
	if ( (rindex($aPath,'/',$end)>$last) | ($last == -1) )
		{ $last = rindex($aPath,'/',$end); }

	if ($last == -1)
		{ return ""; }
	else
		{ return substr($aPath,0,$last).'/'; }
	}

1;
