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
# all variables called *Path* are set up to end with a backslash
# all variables called *Path or *File are stored as absolute (file)paths within makmake
# all variables called UpPath* are stored as relative paths within makmake
# 
#

use strict;

use FindBin;		# for FindBin::Bin
use Getopt::Long;

my $PerlLibPath;    # fully qualified pathname of the directory containing our Perl modules

BEGIN {
# check user has a version of perl that will cope
	require 5.005_03;
# establish the path to the Perl libraries: currently the same directory as this script
	$PerlLibPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlLibPath =~ s/\//\\/g;	# X:\epoc32\tools
	$PerlLibPath .= "\\";
}
sub ExportDirs ($);

use lib $PerlLibPath;
use E32env;
use E32Plat;
use Modload;
use Output;
use Pathutl;
use E32Variant;
use RVCT_plat2set;
use BPABIutl;
use wrappermakefile;
use CheckSource;
use File::Path; # for rmtree
use featurevariantparser;

my $BldInfName = 'BLD.INF';
my %Options;
my %KeepGoing;
my @DefaultPlats=('WINSCW', 'GCCXML', 'EDG', 'X86GCC');
my @BaseUserDefaultPlats=('ARM4', 'ARM4T', 'WINSCW', 'GCCXML', 'EDG', 'X86GCC');
my @OptionalPlats=('VS6', 'VS2003');
my @PlatsReq;

my %CheckSourceEXPORTSMetaData;
my %CheckSourceEXPORTSIncludes;
my %CheckSourceMMPFILESMetaData;
my %CheckSourceEXTENSIONSMetaData;
my %CheckSourceBldInfIncludes;

for ('ARMV4', 'ARMV5')
{
	push @BaseUserDefaultPlats, $_ if RVCT_plat2set::compiler_exists($_);
}

# Add ARMV5_ABIV1 platform if ENABLE_ABIV2_MODE is set in variant.cfg
my $variantABIV2Keyword = &Variant_GetMacro();
# Add ARMV5_ABIV1 platform only after determining the presence of RVCT compiler.
if ($variantABIV2Keyword && RVCT_plat2set::compiler_exists('ARMV5_ABIV1') ) {
	push @OptionalPlats, 'ARMV5_ABIV1';
}

# bldmake -k shouldn't die if Extension Makefile is missing
our $IgnoreMissingExtensionMakefile = 0;

# Add the BPABI Platforms to be added 
my @BPABIPlats = &BPABIutl_Plat_List;
foreach my $BPABIPlat (@BPABIPlats) 
{
	# BPABI platform related with ARMV5(eg.ARMV5_ABIV2) is added to the platform list after 
	# determining the presence of RVCT compiler	
	if(($BPABIPlat =~/^ARMV5/i))
		{
			if(!($BPABIPlat =~/^ARMV5$/i) && RVCT_plat2set::compiler_exists('ARMV5'))
			{
			push @OptionalPlats, $BPABIPlat;
			}
		}
	# All other BPABI platforms(eg. gcce) are added to the platform list.
	else
		{
			push @OptionalPlats, $BPABIPlat;
		}
}

if ( RVCT_plat2set::compiler_exists('ARMV5') ) {
	#determine the presence of ARVCT compiler
	push @DefaultPlats, 'ARMV5';
}
	
# Need to add WINS and X86 if MSDEV compiler is present
# Use MSDevDir to determine the presence of the compiler
push @BaseUserDefaultPlats, 'WINS', 'X86' if (exists($ENV{'MSDevDir'}));

my @BaseDefaultPlats = @BaseUserDefaultPlats;
push @BaseDefaultPlats, 'X86SMP' if (grep /^X86$/, @BaseUserDefaultPlats);
push @BaseDefaultPlats, 'ARM4SMP' if (grep /^ARM4$/, @BaseUserDefaultPlats);
push @BaseDefaultPlats, 'ARMV4SMP' if (grep /^ARMV4$/, @BaseUserDefaultPlats);
push @BaseDefaultPlats, 'ARMV5SMP' if (grep /^ARMV5$/, @BaseUserDefaultPlats);
push @BaseDefaultPlats, 'X86GMP' if (grep /^X86GCC$/, @BaseUserDefaultPlats);

my $variantMacroHRHFile = Variant_GetMacroHRHFile();
sub ExportDirs ($);

# THE MAIN PROGRAM SECTION
##########################

# Load default feature variant info - for the hrh file
my %DefaultFeatureVariant = featurevariantparser->GetVariant('DEFAULT') if (featurevariantparser->DefaultExists());
my @FeatureVariants = featurevariantparser->GetBuildableFeatureVariants();
	
{
	Load_SetModulePath($PerlLibPath);
	Plat_Init($PerlLibPath);

	{
		my @PlatList = &Plat_List();
		
		if (RVCT_plat2set::compiler_exists('ARMV6')){
			foreach my $ARMV6Target ("ARMV6", "ARMV6_ABIV1", "ARMV6_ABIV2"){
				if (grep /^$ARMV6Target$/, @PlatList) {
					push @BaseUserDefaultPlats, "$ARMV6Target" if (!grep /^$ARMV6Target$/, @BaseUserDefaultPlats);
					push @BaseDefaultPlats, "$ARMV6Target" if (!grep /^$ARMV6Target$/, @BaseDefaultPlats);
				}
			}
		}
	
		if (RVCT_plat2set::compiler_exists('ARMV7')){
 			my $rvct_ver = RVCT_plat2set::get_version_string('ARMV7');
 			if ((defined $rvct_ver) and ($rvct_ver ge "3.1.674")) {
 				if (grep /^ARMV7$/, @PlatList ) {
					push @DefaultPlats, 'ARMV7' if (!grep /^ARMV7$/, @DefaultPlats);
					push @BaseUserDefaultPlats, "ARMV7" if (!grep /^ARMV7$/, @BaseUserDefaultPlats);
					push @BaseDefaultPlats, "ARMV7" if (!grep /^ARMV7$/, @BaseDefaultPlats);
 				}
 			}
 		}
	}
	
#	process the commmand-line
	unless (GetOptions(\%Options, 'v', "k|keepgoing", "notest", "file|f=s")) {
		exit 1;
	}
	unless (@ARGV>=1) {
		&Usage();
	}
	my $Command=uc shift @ARGV;
	unless ($Command=~/^(BLDFILES|CLEAN|INF|PLAT)$/o) {
		&Usage();
	}
	my $CLPlat=uc shift @ARGV;

	unless ($CLPlat) {
		$CLPlat='ALL';
	}

	if ($Command eq 'INF') {
		&ShowBldInfSyntax();
		exit;
	}

	if ($Command eq 'PLAT') {
		my @PlatList = ($CLPlat);
		my $PlatName;
		# Variable introduced to check if the bldmake plat command is called, To be
		# passed as an argument in Plat_GetL function call.
		my $platcommand=1;
		if ($CLPlat eq "ALL") {
			@PlatList = &Plat_List();
			print(
				"Supported Platforms:\n",
				"  @PlatList\n\n"
			);
		}
		print(
			"Macros defined for BLD.INF preprocessing of MMPFILE sections:\n"
		);
		foreach $PlatName (@PlatList) {
			my %Plat;
			eval { &Plat_GetL($PlatName, \%Plat,{},$platcommand); };
			die $@ if $@;
			print(
				"\nPlatform $PlatName:\n",
				"  @{$Plat{MmpMacros}}\n"
			);
		}
		exit;
	}
	if ($Options{file}) {
		$BldInfName = $Options{file};
	}

#	check that the BLD.INF file exists
#	maybe BLDMAKE should allow a path to be specified leading to the BLD.INF file
	my $BldInfPath=&Path_WorkPath;
	unless (-e "${BldInfPath}$BldInfName") {
		&FatalError("Can't find \"${BldInfPath}$BldInfName\"");
	}

	if (!-d $E32env::Data{EPOCPath}){
		&FatalError("Directory \"$E32env::Data{EPOCPath}\" does not exist");
	}

#	decide the output directory
	my $OutDir=&Path_Chop($E32env::Data{BldPath}).$BldInfPath;

#	Work out the path for the IBY files
	my $RomDir=&Path_Chop($E32env::Data{RomPath}).$BldInfPath;

#	Work out the name for the BLD.INF module
	my @Dirs=&Path_Dirs($BldInfPath);
	my $Module = pop @Dirs;
	if (lc($Module) eq 'group') {
		$Module = pop @Dirs;
	}

	if ($Command eq 'CLEAN') {
		unlink "${BldInfPath}ABLD.BAT";
		$OutDir=~m-(.*)\\-o;
		if (-d $1) { # remove backslash for test because some old versions of perl can't cope
			opendir DIR, $1;
			my @Files=grep s/^([^\.].*)$/$OutDir$1/, readdir DIR;
			closedir DIR;
			unlink @Files;
		}
		rmtree <$OutDir\\wrappermakefiles>;
# modified start: makefile improvement 
		rmtree <$OutDir\\FeatureVariantInfo>;
# modified end: makefile improvement 
		exit;
	}

#	parse BLD.INF - to get the platforms and the export files
	eval { &Load_ModuleL('PREPFILE'); };
	&FatalError($@) if $@;

	my @RealPlats=();
	my @Exports=();
	my @TestExports=();
	if ($Options{v}) {
		print "Reading \"${BldInfPath}$BldInfName\" for platforms and exports\n";
	}
	&ParseBldInf(\@RealPlats, \@Exports, \@TestExports, $BldInfPath, 
		$E32env::Data{EPOCIncPath}, $E32env::Data{EPOCPath}, $E32env::Data{EPOCDataPath});

#       Add Customizations
	my @additions;
	foreach my $plat (@RealPlats) {
	        my @customizations = Plat_Customizations($plat);
	        foreach my $custom (@customizations) {
	                push @additions, $custom 
			        unless grep /$custom/, @additions;
		}
        }
	unless ($CLPlat eq 'ALL') {
		push @RealPlats, @additions;
	}

 #	Force GCCXML support for anything that compiles as ARMV5
	if ( (grep /^ARMV5$/, @RealPlats) and not (grep /^GCCXML$/,@RealPlats) ) 
			{
			push @RealPlats, 'GCCXML';
			}

 #	Force EDG support for anything that compiles as ARMV5
	if ( (grep /^ARMV5$/, @RealPlats) and not (grep /^EDG$/,@RealPlats) ) 
			{
			push @RealPlats, 'EDG';
			}

if (0) {
#	Add ARMV5 to the platforms if ARM4 is defined
	if (grep /^ARM4$/, @RealPlats) {
		unless ( (grep /^ARMV4$/, @RealPlats) or (grep /^ARMV5$/, @RealPlats) ){
		push @RealPlats, 'ARMV5';
		push @RealPlats, 'ARMV4';
		}
	}
}
	
#	get any IDE platforms required into a new platforms list, and
#	Create a hash to contain the 'real' names of the platforms, i.e. WINS rather than VC6
	my @Plats=@RealPlats;
	my %Real;
	foreach (@RealPlats) { # change to get VC6 batch files
		$Real{$_}=$_;
		my $AssocIDE;
		my @AssocIDEs;

#		Get the IDEs associated with a real platform. A real plat like,
#		WINSCW may have multiple associated IDEs like VC6 .NET2003 or CW IDE.
		&Plat_AssocIDE($_, \@AssocIDEs);
		next unless @AssocIDEs;
		
		push @Plats, @AssocIDEs;
		foreach $AssocIDE (@AssocIDEs)
		{
			$Real{$AssocIDE}=$Real{$_};
		}
		
	}
	if ($Options{v}) {
		print "Platforms: \"@Plats\"\n";
	}

#	check that the platform specified on the command-line is acceptable
#	and sort out a list of platforms to process
	my @DoRealPlats=@RealPlats;
	my @DoPlats=@Plats;

	unless (@Plats) { 
#	include the optional platform list if no platform is specified
		my $OptionalPlat;
		foreach $OptionalPlat (@OptionalPlats) {
			unless (grep /^$OptionalPlat$/i, @DoPlats) {
				push @DoPlats, $OptionalPlat;
			}
		}
	}

	unless ($CLPlat eq 'ALL') {
		unless (grep /^$CLPlat$/, @Plats) {
			&FatalError("Platform $CLPlat not supported by \"${BldInfPath}$BldInfName\"\n");
		}
		@DoPlats=($CLPlat);
		@DoRealPlats=$Real{$CLPlat};
	}
			
#	sort out the export directories we might need to make
	my @ExportDirs=ExportDirs(\@Exports);
	my @TestExportDirs=ExportDirs(\@TestExports);

#	parse the BLD.INF file again for each platform supported by the project
#	storing the information in a big data structure
	my %AllPlatData;
	my %AllPlatTestData;
	my $Plat;
	
	if ($Options{v} and $CLPlat ne 'ALL'){
		print "Reading \"${BldInfPath}$BldInfName\" for $CLPlat \n";
	}

	foreach $Plat (@RealPlats) {
		if ($Options{v}) {
			if ($CLPlat eq 'ALL') {
				print "Reading \"${BldInfPath}$BldInfName\" for $Plat\n";
			}
		}
		my (@PlatData, @PlatTestData);
		if ($CLPlat eq 'ALL') {
			&ParseBldInfPlat(\@PlatData, \@PlatTestData, $Plat, $BldInfPath, ($DefaultFeatureVariant{VALID} && &Plat_SupportsFeatureVariants($Plat) ? \%DefaultFeatureVariant : undef));
		}
		else {
			&ParseBldInfPlat(\@PlatData, \@PlatTestData, $CLPlat, $BldInfPath, ($DefaultFeatureVariant{VALID} && &Plat_SupportsFeatureVariants($CLPlat) ? \%DefaultFeatureVariant : undef));
		}
		$AllPlatData{$Plat}=\@PlatData;
		$AllPlatTestData{$Plat}=\@PlatTestData;
	}
	undef $Plat;

	undef $CLPlat;
	if ($Command eq 'BLDFILES') {

#		create the perl file, PLATFORM.PM, listing the platforms
		if ($Options{v}) {
			print "Creating \"${OutDir}PLATFORM.PM\"\n";
		}
		&CreatePlatformPm($OutDir, \@Plats, \@RealPlats, \%Real, \%AllPlatData, \%AllPlatTestData);

#		create the .BAT files required to call ABLD.PL
		if ($Options{v}) {
			print "Creating \"${BldInfPath}ABLD.BAT\"\n";
		}
		&CreatePerlBat($BldInfPath);

#		create the makefile for exporting files
		if ($Options{v}) {
			print "Creating \"${OutDir}EXPORT.MAKE\"\n";
		}
		&CreateExportMak("${OutDir}EXPORT.MAKE", \@Exports, \@ExportDirs);

#		create the makefile for exporting test files
		if ($Options{v}) {
			print "Creating \"${OutDir}EXPORTTEST.MAKE\"\n";
		}
		&CreateExportMak("${OutDir}EXPORTTEST.MAKE", \@TestExports, \@TestExportDirs);

# modified start: makefile improvement 
		#create the feature variant infor file
		foreach my $copyofPlat (@DoPlats)
		{
			my $realplat = $Real{$copyofPlat};
			if(&Plat_SupportsFeatureVariants($copyofPlat))
			{
				my $variant_info = &Path_Chop($E32env::Data{BldPath}).$BldInfPath."\\FeatureVariantInfo\\".$realplat."\\";	
				eval { &Path_MakePathL($variant_info); };
				die $@ if $@;
				if ($Options{v}) {
					print "Creating: \"$variant_info\"\n";
				}
				foreach my $featureVariant (@FeatureVariants)
				{
					my $variant_file = $variant_info."$realplat.$featureVariant.info";
# modified by SV start: makefile improvement 
					my $refdata = $AllPlatData{$realplat};
					my $testrefdata = $AllPlatTestData{$realplat};
					if ( @$refdata ) {
						foreach my $RefPro (@$refdata)
						{
							$variant_file = $variant_info."$realplat.$featureVariant.$$RefPro{Base}.info";
							my $ref_basedir = $variant_file;
							$ref_basedir=~s/(.*[\\\/]).*/$1/;
							if ( ! -d $ref_basedir ){
								eval { &Path_MakePathL($ref_basedir); };
								die $@ if $@;
							}
							open VARIANTINFOR,">$variant_file" or die "ERROR: Can't open or create file \"$variant_file\"\n";
							print VARIANTINFOR "VARIANT_PLAT_NAME_$$RefPro{Base}:=default \n";
							close VARIANTINFOR or die "ERROR: Can't close file \"$variant_file\"\n";
						}
					}
					else {
						open VARIANTINFOR,">$variant_file" or die "ERROR: Can't open or create file \"$variant_file\"\n";
						print VARIANTINFOR "VARIANT_PLAT_NAME:=$featureVariant \n";
						close VARIANTINFOR or die "ERROR: Can't close file \"$variant_file\"\n";
						print "file \"$variant_file\"\n"
					}
					if ($testrefdata){
						foreach my $RefPro (@$testrefdata)
						{
							$variant_file = $variant_info."$realplat.$featureVariant.$$RefPro{Base}.info";
							my $ref_basedir = $variant_file;
							$ref_basedir=~s/(.*[\\\/]).*/$1/;
							if ( ! -d $ref_basedir ){
								eval { &Path_MakePathL($ref_basedir); };
								die $@ if $@;
							}
							open VARIANTINFOR,">$variant_file" or die "ERROR: Can't open or create file \"$variant_file\"\n";
							print VARIANTINFOR "VARIANT_PLAT_NAME_$$RefPro{Base}:=default \n";
							close VARIANTINFOR or die "ERROR: Can't close file \"$variant_file\"\n";
						}
						
					}
# modified by SV end: makefile improvement 
					# Close and cleanup
					if ($Options{v}) {
						print "Variant info file has been successfully created\n";
					}
				}				
			}
		}
# modified end: makefile improvement 
#		create the platform meta-makefiles
		foreach my $copyofPlat (@DoPlats) {  # Do not use $_ here !!
			if ($Options{v}) {
				print "Creating \"$OutDir$copyofPlat.MAKE\"\n";
			}
			my $realplat = $Real{$copyofPlat};
			&CreatePlatMak($OutDir, $E32env::Data{BldPath}, $AllPlatData{$realplat}, $copyofPlat, $realplat, $RomDir, $Module, $BldInfPath, \@Exports, '');

			if (&Plat_SupportsFeatureVariants($copyofPlat))
				{
				foreach my $featureVariant (@FeatureVariants)
					{
					print "Creating \"$OutDir$copyofPlat.$featureVariant.MAKE\"\n" if ($Options{v});
					&CreatePlatMak($OutDir, $E32env::Data{BldPath}, $AllPlatData{$realplat}, $copyofPlat, $realplat, $RomDir, $Module, $BldInfPath, \@Exports, '', ".$featureVariant");
					}
				}
		}
		foreach (@DoPlats) {
			if ($Options{v}) {
				print "Creating \"$OutDir${_}TEST.MAKE\"\n";
			}
			&CreatePlatMak($OutDir, $E32env::Data{BldPath}, $AllPlatTestData{$Real{$_}}, $_, $Real{$_}, $RomDir, $Module, $BldInfPath, \@TestExports, 'TEST');

			if (&Plat_SupportsFeatureVariants($_))
				{
				foreach my $featureVariant (@FeatureVariants)
					{
					print "Creating \"$OutDir${_}.".$featureVariant."TEST.MAKE\"\n" if ($Options{v});
					&CreatePlatMak($OutDir, $E32env::Data{BldPath}, $AllPlatTestData{$Real{$_}}, $_, $Real{$_}, $RomDir, $Module, $BldInfPath, \@TestExports, 'TEST', ".$featureVariant");
					}
				}
		}

#		create the platform test batch files
		foreach (@DoRealPlats) {
			if ($Options{v}) {
				print "Creating test batch files in \"$OutDir\" for $_\n";
			}
			&CreatePlatBatches($OutDir, $AllPlatTestData{$_}, $_);
		}

# modified by SV start: makefile improvement 
# create all sub directories
	foreach my $refplat (@DoRealPlats) {
		my $tmp = $AllPlatData{$refplat};
		foreach my $dref (@$tmp){
			my $builddir = $OutDir . $$dref{Base} ."\\" . $refplat . "\\";
				if (!-d $builddir){
					if ($Options{v}) {
						print "Creating directory \"$builddir\" \n";
					}
					eval { &Path_MakePathL($builddir); };
					&FatalError($@) if $@;
				}
			}
	}
# modified by SV end: makefile improvement 

#		report any near-fatal errors
		if (scalar keys %KeepGoing) {
		    print STDERR
			    "\n${BldInfPath}$BldInfName WARNING(S):\n",
			    sort keys %KeepGoing
			    ;
		}

		exit;
	}
}


################ END OF MAIN PROGRAM SECTION #################
#------------------------------------------------------------#
##############################################################


# SUBROUTINE SECTION
####################

sub Usage () {

	eval { &Load_ModuleL('E32TPVER'); };
	&FatalError($@) if $@;

	print
		"\n",
		"BLDMAKE - Project building Utility (Build ",&E32tpver,")\n",
		"\n",
		"BLDMAKE {options} [<command>] [<platform>]\n",
		"\n",
		"<command>: (case insensitive)\n",
		" BLDFILES - create build batch files\n",
		" CLEAN    - remove all files bldmake creates\n",
		" INF      - display basic BLD.INF syntax\n",
		" PLAT     - display platform macros\n",
		"\n",
		"<platform>: (case insensitive)\n",
		"  if not specified, defaults to \"ALL\"\n",
		"\n",
		"Options: (case insensitive)\n",
		" -v   ->  verbose mode\n",
		" -k   ->  keep going even if files are missing\n"
	;
	exit 1;
}

sub ShowBldInfSyntax () {

	print <<ENDHERE1;

BLD.INF - Syntax

/* Use C++ comments if required */
// (Curly braces denote optional arguments)

PRJ_PLATFORMS
{DEFAULT} {-<platform> ...} {<list of platforms>}
// list platforms your project supports here if not default
ENDHERE1

	print "// default = ".join(" ",@DefaultPlats)."\n";

	print <<ENDHERE;
	
PRJ_EXPORTS
[<source path>\<source file>]	{<destination>}
// list each file exported from source on a separate line
// {<destination>} defaults to \\EPOC32\\Include\\<source file>

PRJ_TESTEXPORTS
[<source path>\<source file>]	{<destination>}
// list each file exported from source on a separate line
// {<destination>} defaults to BLD.INF dir

PRJ_MMPFILES
[<mmp path>\<mmp file>] {<qualifiers>}
{MAKEFILE|NMAKEFILE} [<path>\<makefile>] {build_as_arm}
// <qualifiers> are tidy, ignore, build_as_arm

#if defined(<platform>)
// .MMP statements restricted to <platform>
#endif

PRJ_TESTMMPFILES
[<mmp path>\<mmp file>] {<qualifiers>}
{MAKEFILE|NMAKEFILE} [<path>\<makefile>] {<qualifiers>}
// <qualifiers> are {tidy} {ignore} {manual} {support} {build_as_arm}

#if defined(<platform>)
// .MMP statements restricted to <platform>
#endif

ENDHERE

}

sub WarnOrDie ($$) {
	my ($dieref, $message) = @_;
	if ($Options{k}) {
		$KeepGoing{$message} = 1;
	} else {
		push @{$dieref}, $message;
	}
}

sub ExtensionMakefileMissing($)
{
	$IgnoreMissingExtensionMakefile = @_;
}

sub ParseBldInf ($$$$$) {
	my ($PlatsRef, $ExportsRef, $TestExportsRef, $BldInfPath, $EPOCIncPath, $EPOCPath, $EPOCDataPath)=@_;

	my @Prj2D;
	eval { &Prepfile_ProcessL(\@Prj2D, "${BldInfPath}$BldInfName",$variantMacroHRHFile); };
	&FatalError($@) if $@;
	
	my @SupportedPlats=&Plat_List();

	my @Plats;
	my %RemovePlats;

	my $DefaultPlatsUsed=0;
	my %PlatformCheck;

	my %ExportCheck;
	my $Section=0;
	our @PrjFileDie;
	my $Line;
	my $CurFile="${BldInfPath}$BldInfName";
	LINE: foreach $Line (@Prj2D) {
		my $LineNum=shift @$Line;
		$_=shift @$Line;
		if ($LineNum eq '#') {
			$CurFile=$_;
			next LINE;
		}

		$CurFile = &Path_Norm ($CurFile); 
		
		if (/^PRJ_(\w*)$/io) {
			$Section=uc $1;
			if ($Section=~/^(PLATFORMS|EXPORTS|TESTEXPORTS|MMPFILES|TESTMMPFILES|EXTENSIONS|TESTEXTENSIONS)$/o) {
				if (@$Line) {
					push @PrjFileDie, "$CurFile($LineNum) : Can't specify anything on the same line as a section header\n";
				}
				next LINE;
			}
			push @PrjFileDie, "$CurFile($LineNum) : Unknown section header - $_\n";
			$Section=0;
			next LINE;
		}
		if ($Section eq 'PLATFORMS') {
#			platforms are gathered up into a big list that contains no duplicates.  "DEFAULT" is
#			expanded to the list of default platforms.  Platforms specified with a "-" prefix
#			are scheduled for removal from the list.  After processing platforms specified
#			with the "-" prefix are removed from the list.

			unshift @$Line, $_;
			my $Candidate;
			CANDLOOP: foreach $Candidate (@$Line) {
				$Candidate=uc $Candidate;
#				ignore old WINC target
				if ($Candidate eq 'WINC') {
					next CANDLOOP;
				}
#				expand DEFAULT
				if ($Candidate eq 'DEFAULT') {
					$DefaultPlatsUsed=1;
					my $Default;
					foreach $Default (@DefaultPlats) {
						unless ($PlatformCheck{$Default}) {
							push @Plats, $Default;
							$PlatformCheck{$Default}="$CurFile: $LineNum";
						}
					}
					next CANDLOOP;
				}
#				expand BASEDEFAULT
				if ($Candidate eq 'BASEDEFAULT') {
					$DefaultPlatsUsed=1;
					my $Default;
					foreach $Default (@BaseDefaultPlats) {
						unless ($PlatformCheck{$Default}) {
							push @Plats, $Default;
							$PlatformCheck{$Default}="$CurFile: $LineNum";
						}
					}
					next CANDLOOP;
				}
#				expand BASEUSERDEFAULT
				if ($Candidate eq 'BASEUSERDEFAULT') {
					$DefaultPlatsUsed=1;
					my $Default;
					foreach $Default (@BaseUserDefaultPlats) {
						unless ($PlatformCheck{$Default}) {
							push @Plats, $Default;
							$PlatformCheck{$Default}="$CurFile: $LineNum";
						}
					}
					next CANDLOOP;
				}
#				check for removals
				if ($Candidate=~/^-(.*)$/o) {
					$Candidate=$1;
#					check default is specified
					unless ($DefaultPlatsUsed) {
						push @PrjFileDie, "$CurFile($LineNum) : \"DEFAULT\" must be specified before platform to be removed\n";
						next CANDLOOP;
					}
					$RemovePlats{$Candidate}=1;
					next CANDLOOP;
				}
# 				If tools platform is specified in bld.inf file then component is built for cwtools as well 
				if ($Candidate =~ /^tools/i)
						{
						push @Plats, 'CWTOOLS';
						}
#				check platform is supported
				unless (grep /^$Candidate$/, @SupportedPlats) {
					WarnOrDie(\@PrjFileDie, "$CurFile($LineNum) : Unsupported platform $Candidate specified\n");
					next CANDLOOP;
				}
#				check platform is not an IDE
				if ($Candidate=~/^VC/o) {
					push @PrjFileDie, "$CurFile($LineNum) : No need to specify platform $Candidate here\n";
					next CANDLOOP;
				}
#				add the platform
				unless ($PlatformCheck{$Candidate}) {
					push @Plats, $Candidate;
					my $SubPlat = sprintf("%sEDG", $Candidate);
					push @Plats, $SubPlat 
					    if (grep /^$SubPlat$/, @SupportedPlats);
					$PlatformCheck{$Candidate}="$CurFile: $LineNum";
				}
			}
			next LINE;
		}

		# Skip PRJ_TESTEXPORT section if -notest flag		
		next LINE if ($Options{notest} && ($Section=~/^(TESTEXPORTS)$/o)); 
		
		if ($Section=~/^(EXPORTS|TESTEXPORTS)$/o) {

#			make path absolute - assume relative to group directory
			my $Type = 'file';
			if (/^\:(\w+)/) {
				# Export an archive
				$Type = lc $1;
				unless ($Type eq 'zip') {
					push @PrjFileDie, "$CurFile($LineNum) : Unknown archive type - $Type\n";
					next LINE;
				}
				$_ = shift @$Line;
			}

			my $loggedSourceExport = $_;
			$_ = &Path_Norm ($_);
			
			my $Source=&Path_MakeAbs($CurFile, $_);
			my $Releasable='';
			my $emReleasable='';
			my $unzip_option ='';
			if (@$Line) {
#				get the destination file if it's specified
				$Releasable=shift @$Line;
				CheckSource_MetaData(%CheckSourceEXPORTSMetaData, $CurFile, "PRJ_".$Section, $Releasable, $LineNum);
				$Releasable = &Path_Norm ($Releasable);
				$emReleasable=ucfirst $Releasable;
				if ($emReleasable=~/^([A-Z]):(\\.*)$/)  {
				  	$emReleasable=~s/://;
					$Releasable=$EPOCDataPath.$emReleasable;
				}
			}

			my $sourceExportTypeSuffix = "";
			$sourceExportTypeSuffix .= " (NO DESTINATION)" if (!$Releasable && $Section =~ /^EXPORTS$/);		
			CheckSource_MetaData(%CheckSourceEXPORTSMetaData, $CurFile, "PRJ_".$Section.$sourceExportTypeSuffix, $loggedSourceExport, $LineNum, $CheckSource_PhysicalCheck);
			
			if (@$Line) {
				$unzip_option = shift @$Line;
				unless ($unzip_option=~ /overwrite/i) {
					push @PrjFileDie, "$CurFile($LineNum) : Too many arguments in exports section line\n";
					next LINE;
				}
			}
			unless ($Type eq 'zip' or &Path_Split('File', $Releasable)) {
#				use the source filename if no filename is specified in the destination
#				no filename for archives
				$Releasable.=&Path_Split('File', $Source);
			}
			my $defpath;
			if ($Type eq 'zip') {
#				archives relative to EPOCROOT
				$defpath = $ENV{EPOCROOT};
			}
			elsif (($Section =~ /EXPORTS$/) && ($Releasable =~ s/^\|[\/|\\]?//)) {
#			'|' prefix forces "relative to bld.inf file" in PRJ_[TEST]EXPORTS destinations
				$defpath = $CurFile;
			}
			elsif ($Section eq 'EXPORTS') {
#				assume the destination is relative to $EPOCIncPath
				$defpath = $EPOCIncPath;
			}
			else {
				$defpath = $CurFile;
			}
			$Releasable=&Path_MakeEAbs($EPOCPath, $defpath, $Releasable);

#			sanity checks!
			if ($Type eq 'file' && $ExportCheck{uc $Releasable}) {
				push @PrjFileDie, "$CurFile($LineNum) : Duplicate export $Releasable (from line $ExportCheck{uc $Releasable})\n";
				next LINE;
			}
			$ExportCheck{uc $Releasable}="$CurFile: $LineNum";
			if (! -e $Source) {
				WarnOrDie(\@PrjFileDie, "$CurFile($LineNum) : Exported source file $Source not found\n");
			}
			elsif ($Type ne 'zip' && -d $Releasable) {
				push @PrjFileDie, "$CurFile($LineNum) : Export target $Releasable must be a file.\n";
			}
			else {
				if ($Section eq 'EXPORTS') {
					push @$ExportsRef, {
						'Source'=>$Source,
						'Releasable'=>$Releasable,
						'emReleasable'=>$emReleasable,
						'Type'=>$Type,
						'UnzipOption'=>$unzip_option
					};
				}
				else {
					push @$TestExportsRef, {
						'Source'=>$Source,
						'Releasable'=>$Releasable,
						'emReleasable'=>$emReleasable,
						'Type'=>$Type,
						'UnzipOption'=>$unzip_option
					};
				}
			}
			next LINE;
		}
	}
	if (@PrjFileDie) {
		print STDERR
			"\n${BldInfPath}$BldInfName FATAL ERROR(S):\n",
			@PrjFileDie
		;
		exit 1;
	}

#	set the list of platforms to the default if there aren't any platforms specified,
#	else add platforms to the global list unless they're scheduled for removal,
	unless (@Plats) {
		@$PlatsRef=@DefaultPlats;
		# Include the list of BPABI Platforms in a default build.
		my $OptionalPlat;
		foreach $OptionalPlat (@OptionalPlats) {
			#	VS6 and VS2003 are not real platforms and hence are not included in a default build
			unless ( $OptionalPlat eq 'VS6' || $OptionalPlat eq 'VS2003') {
				if (not grep /^$OptionalPlat$/i, @$PlatsRef) {
					push @$PlatsRef, $OptionalPlat;
				}
			}
		}		
	}
	else {
		my $Plat;
		foreach $Plat (@Plats) {
			unless ($RemovePlats{$Plat}) {
				push @$PlatsRef, $Plat;
			}
		}
		push @PlatsReq , @$PlatsRef;
	}
}

sub ExportDirs ($) {
	my ($ExportsRef)=@_;

	my %ExportDirHash;
	foreach (@$ExportsRef) {
		my $dir = ($$_{Type} eq 'zip') ? $$_{Releasable} : &Path_Split('Path',$$_{Releasable});
		if ($dir) {
			$dir=&Path_Chop($dir);
			$ExportDirHash{uc $dir}=$dir;
		}
	}
	my @ExportDirs;
	foreach (keys %ExportDirHash) {
		push @ExportDirs, $ExportDirHash{$_};
	}
	@ExportDirs;
}


sub ParseBldInfPlat ($$$$) {
	my ($DataRef, $TestDataRef, $Plat, $BldInfPath, $FeatureVar)=@_;
		
#	get the platform .MMP macros
	my %Plat;
	eval { &Plat_GetL($Plat,\%Plat); };
	&FatalError($@) if $@;

#	get the raw data from the BLD.INF file
	my @Prj2D;
	eval { &Prepfile_ProcessL(\@Prj2D, "${BldInfPath}$BldInfName", ($FeatureVar ? $FeatureVar->{VARIANT_HRH} : $variantMacroHRHFile), @{$Plat{MmpMacros}}); };
	&FatalError($@) if $@;

	my %dummy;
	my @userIncludes = ('.');
	my @systemIncludes = ();
	$CheckSourceBldInfIncludes{$Plat} = CheckSource_Includes("${BldInfPath}$BldInfName", %dummy, $variantMacroHRHFile, @{$Plat{MmpMacros}}, @userIncludes, @systemIncludes, $CheckSource_NoUserSystemDistinction);
	
#	process the raw data
	my $IsExtensionBlock =0;
	my (@ExtensionBlockData, $ErrorString);
	my %Check;
	my $Section=0;
	my @PrjFileDie;
	my $Line;
	my $CurFile="${BldInfPath}$BldInfName";
	LINE: foreach $Line (@Prj2D) {

		my %Data;
		my %Temp;

		my $LineNum=shift @$Line;
		if ($LineNum eq '#') {
			$CurFile=shift @$Line;
			next LINE;
		}

		$CurFile = &Path_Norm ($CurFile);
		
#		upper-case all the data here, but record original source case
#		in a hash so that it can be recalled for CheckSource purposes

		my %originalSourceCase;
   		foreach (@$Line) {
 			$originalSourceCase{uc $_} = $_;  # needed for extension template makefile MACROs 
   			$_=uc $_;
   		}

		$_=shift @$Line;

#		check for section headers - don't test for the right ones here
#		because we do that in the first parse function

		if (/^PRJ_(\w*)$/o) {
			$Section=$1;
			next LINE;
		}

#		Skip section if PRJ_TESTMMPFILES and -notest option
		next LINE if ($Options{notest} && ($Section=~/^(TESTMMPFILES)$/o)); 

#		check for EXTENSION sections
		if ($Section=~/^(EXTENSIONS|TESTEXTENSIONS)$/o) {

#			We have an extension block
			if (/^start(\w*)$/io) {
				if ($IsExtensionBlock) {
					&FatalError("$CurFile($LineNum) : Cannot embed Extension Template 'start' sections\n");
				}
				$IsExtensionBlock =1;
				$ErrorString = "$CurFile($LineNum)";
				foreach (@$Line)
				{
				if (/^EXTENSION$/)
					{
					my $extensionTemplate = @$Line[scalar(@$Line)-1];
					CheckSource_MetaData(%CheckSourceEXTENSIONSMetaData, $CurFile, "PRJ_".$Section, $originalSourceCase{$extensionTemplate}.".mk", $LineNum, $CheckSource_PhysicalCheck) if ($extensionTemplate);
					}
				}

				push @ExtensionBlockData, $Line; 			
				next LINE;
			}		
			
			if (($IsExtensionBlock) & (! (/^end(\w*)$/io))) {
				if (($_ ne "TOOL") & ($_ ne "OPTION") & ($_ ne "TARGET") & ($_ ne "SOURCES") & ($_ ne "DEPENDENCIES")) {
							&FatalError("$CurFile($LineNum) : Unrecognised keyword: $_.  Is there an 'end' corresponding to the 'start' for the Extension Template?\n"); 
				}
				if ($_ ne "OPTION") {
					unshift(@$Line, $_);					
				}
#				Need to revert MACROs back to their original case
				foreach (@$Line) {
					$_=$originalSourceCase{$_};
				}
				push @ExtensionBlockData, $Line; 
				next LINE;
			}
			
			if (/^end(\w*)$/io) {
				if (! $IsExtensionBlock) {
					&FatalError("$CurFile($LineNum) : No 'start' corresponding to this 'end' in Extension Template section\n"); 
				}
				$IsExtensionBlock =0;
				my $OutDir=Path_Chop($E32env::Data{BldPath}).$BldInfPath;
#				Generate wrapper makefile for this platform.
				eval { &Load_ModuleL('WrapperMakefile'); };
					&FatalError($@) if $@;
				$OutDir=~ s/\\/\//g;  # convert to unix slashes for wrappermakefile.pm
				%Data = GenerateWrapper($Plat, $OutDir, $ErrorString, \@PrjFileDie, @ExtensionBlockData);
				if (!$IgnoreMissingExtensionMakefile)
				{
					$Data{ExtensionRoot}=&Path_Split('Path', $CurFile);
					$Data{Path}=~ s/\//\\/g;  # convert unix slashes back to win32 
					$Data{Base}=~ s/\//\\/g;
				}
				@ExtensionBlockData = ();  # clear array
			}
		}

#		check for MMP sections and get the .MMP file details
		if ($Section=~/^(MMPFILES|TESTMMPFILES)$/o) {
			$Data{Ext}='.MMP';
#			check for MAKEFILE statements for custom building
			my $SubSection = "MMP";
			if (/^MAKEFILE$/o) {
				$SubSection = $_;
				$Data{Makefile}=2;  # treat MAKEFILE=>NMAKEFILE   =1;
				$_=shift @$Line;
				$Data{Ext}=&Path_Split('Ext', $_);
			}
			if (/^NMAKEFILE$/o) {
				$SubSection = $_;
				$Data{Makefile}=2;
				$_=shift @$Line;
				$Data{Ext}=&Path_Split('Ext', $_);
			}
			if (/^GNUMAKEFILE$/o) {
				$SubSection = $_;
				$Data{Makefile}=1;
				$_=shift @$Line;
				$Data{Ext}=&Path_Split('Ext', $_);
			}
			CheckSource_MetaData(%CheckSourceMMPFILESMetaData, $CurFile, "PRJ_$Section $SubSection", $originalSourceCase{$_}, $LineNum, $CheckSource_PhysicalCheck);
			$_ = &Path_Norm ($_);
			
#			path considered relative to the current file
			$Data{Path}=&Path_Split('Path', &Path_MakeAbs($CurFile, $_));

#			this function doesn't care whether the .MMPs are listed with their extensions or not
			$Data{Base}=&Path_Split('Base', $_);
			my $MmpFile= $Data{Path}.$Data{Base};
   
#			check the file isn't already specified
  			if ($Check{$MmpFile}) {
  				push @PrjFileDie, "$CurFile($LineNum) : duplicate $Data{Base} (from line $Check{$MmpFile})\n";
   				next;
   			}
  			$Check{$MmpFile}="$CurFile: $LineNum";

#			check the file exists
			unless (-e "$Data{Path}$Data{Base}$Data{Ext}") {
				WarnOrDie(\@PrjFileDie, "$CurFile($LineNum) : $Data{Path}$Data{Base}$Data{Ext} does not exist\n");
				next LINE;
			}
			

#			process the file's attributes
			if ($Section eq 'MMPFILES') {
				foreach (@$Line) {
					if (/^TIDY$/o) {
						$Data{Tidy}=1;
						next;
					}
					if (/^IGNORE$/o) {
						next LINE;
					}
					if (/^BUILD_AS_ARM$/o) {
					  $Data{BuildAsARM}="-arm";
					  next;
					}

					push @PrjFileDie, "$CurFile($LineNum) : Don't understand .MMP file argument \"$_\"\n";
				}
			}

#			process the test .MMP file's attributes
			elsif ($Section eq 'TESTMMPFILES') {
				foreach (@$Line) {
					if (/^TIDY$/o) {
						$Data{Tidy}=1;
						next;
					}
					if (/^IGNORE$/o) {
						next LINE;
					}
					if (/^BUILD_AS_ARM$/o) {
					  $Data{BuildAsARM}="-arm";
					  next;
					}
					if (/^MANUAL$/o) {
						$Data{Manual}=1;
						next;
					}
					if (/^SUPPORT$/o) {
						$Data{Support}=1;
						next;
					}
					push @PrjFileDie, "$CurFile($LineNum) : Don't understand test .MMP file argument \"$_\"\n";
				}
			}
		}		

#		store the data
		if (($Section eq 'MMPFILES') or ($Section eq 'EXTENSIONS')) {
			if ($IgnoreMissingExtensionMakefile and $Section eq 'EXTENSIONS')
			{
				# More than more ext makefile can be missing so reset indicator
				$IgnoreMissingExtensionMakefile = 0;
			}
			else
			{
				push @$DataRef, \%Data;
			}
			next LINE;
		}
		if (($Section eq 'TESTMMPFILES') or ($Section eq 'TESTEXTENSIONS')) {
			if ($IgnoreMissingExtensionMakefile and $Section eq 'TESTEXTENSIONS')
			{
				# More than more ext makefile can be missing so reset indicator
				$IgnoreMissingExtensionMakefile = 0;
			}
			else
			{
				push @$TestDataRef, \%Data;
			}
			next LINE;
		}
		
	}
#	line loop end

#	exit if there are errors
	if (@PrjFileDie) {
		print STDERR
			"\n\"${BldInfPath}$BldInfName\" FATAL ERROR(S):\n",
			@PrjFileDie
		;
		exit 1;
	}
}


sub FatalError (@) {

	print STDERR "BLDMAKE ERROR: @_\n";
	exit 1;
}

sub CreatePlatformPm ($$$$$$) {
	my ($BatchPath, $PlatsRef, $RealPlatsRef, $RealHRef, $AllPlatDataHRef, $AllPlatTestDataHRef)=@_;


# 	exclude GCCXML, EDG and CWTOOLS  from list of RealPlats
	my @RealPlats;
	foreach my $Plat (@$RealPlatsRef){
	unless (($Plat =~ /^gccxml/i)  or  ($Plat =~ /^edg/i) or  ($Plat =~ /^cwtools/i) or ($Plat =~ /^x86gcc/i) or ($Plat =~ /^x86gmp/i)) {
# 	exclude BPABI targets from list of RealPlats provided they are not specified in the platform list
				if (grep /^$Plat$/i, @OptionalPlats) {
					if (grep /^$Plat$/, @PlatsReq) {
						push @RealPlats, $Plat;
					}
					next;
				}
				push @RealPlats, $Plat;
			}
	}


	&Output(
		"# Bldmake-generated perl file - PLATFORM.PM\n",
		"\n",
		"# use a perl integrity checker\n",
		"use strict;\n",
		"\n",
		"package Platform;\n",
		"\n",
		"use vars qw(\@Plats \@RealPlats %Programs %TestPrograms %FeatureVariantSupportingPlats);\n",
		"\n",
		"\@Plats=(\'",join('\',\'',@$PlatsRef),"\');\n",
		"\n",
		"\@RealPlats=(\'", join('\',\'',@RealPlats),"\');\n",
		"\n",
		"%Programs=(\n"
	);
	my %All; # all programs for all platforms
	my $TmpStr;
	my $Plat;
	foreach $Plat (@$PlatsRef) {
		$TmpStr="	\'$Plat\'=>[";
		if (@{${$AllPlatDataHRef}{$$RealHRef{$Plat}}}) {
			my $ProgRef;
			foreach $ProgRef (@{${$AllPlatDataHRef}{$$RealHRef{$Plat}}}) {
				$TmpStr.="'$$ProgRef{Base}',";
				$All{$$ProgRef{Base}}=1;
			}
			chop $TmpStr;
			}
		&Output(
			"$TmpStr],\n"
		);
		}
	$TmpStr="	ALL=>[";
	if (keys %All) {
		my $Prog;
		foreach $Prog (keys %All) {
			$TmpStr.="'$Prog',";
		}
		chop $TmpStr;
	}
	&Output(
		"$TmpStr]\n",
		");\n",
		"\n",
		"%TestPrograms=(\n"
	);
	%All=();
	foreach $Plat (@$PlatsRef) {
		$TmpStr="	\'$Plat\'=>[";
		if (@{${$AllPlatTestDataHRef}{$$RealHRef{$Plat}}}) {
			my $ProgRef;
			foreach $ProgRef (@{${$AllPlatTestDataHRef}{$$RealHRef{$Plat}}}) {
				$TmpStr.="'$$ProgRef{Base}',";
				$All{$$ProgRef{Base}}=1;
			}
			chop $TmpStr;
		}
		&Output("$TmpStr],\n");
	}
	$TmpStr="	ALL=>[";
	if (keys %All) {
		my $Prog;
		foreach $Prog (keys %All) {
			$TmpStr.="'$Prog',";
		}
		chop $TmpStr;
	}
	&Output(
		"$TmpStr]\n",
		");\n",
		"\n"
	);

	&Output(
		"\n",
		"%FeatureVariantSupportingPlats=("
	);

	$TmpStr = "";
	foreach $Plat (@$PlatsRef)
		{
		$TmpStr .= "\n\t$Plat=>1," if (&Plat_SupportsFeatureVariants($Plat));
		}

	chop $TmpStr;

	&Output(
		"$TmpStr\n",
		");\n",
		"\n",
		"1;\n"
	);

#	write the PLATFORM.PM file
	&WriteOutFileL($BatchPath."PLATFORM.PM");
}

sub CreatePerlBat ($) {
	my ($BldInfPath)=@_;

#	create ABLD.BAT, which will call ABLD.PL
#   NB. must quote $BldInfPath because it may contain spaces, but we know it definitely
#       ends with \ so we need to generate "\foo\bar\\" to avoid quoting the close double quote
	&Output(
		"\@ECHO OFF\n",
		"\n", 
		"REM Bldmake-generated batch file - ABLD.BAT\n",
		"REM ** DO NOT EDIT **", 
		"\n",
		"\n",
		"perl -w -S ABLD.PL \"${BldInfPath}\\\" %1 %2 %3 %4 %5 %6 %7 %8 %9\n",
		"if errorlevel==1 goto CheckPerl\n",
		"goto End\n",
		"\n",
		":CheckPerl\n",
		"perl -v >NUL\n",
		"if errorlevel==1 echo Is Perl, version 5.003_07 or later, installed?\n",
		"goto End\n",
		"\n",
		":End\n"
	);

#	check that the .BAT file does not already exist and is read-only
	if ((-e "${BldInfPath}ABLD.BAT")  && !(-w "${BldInfPath}ABLD.BAT")) {
		warn "BLDMAKE WARNING: read-only ABLD.BAT will be overwritten\n";
		chmod 0222, "${BldInfPath}ABLD.BAT";
	}

#	create the .BAT file in the group directory
	&WriteOutFileL($BldInfPath."ABLD.BAT",1);

}

sub GetArchiveExportList($) {
	my ($ExportRef) = @_;
	my $Type = $ExportRef->{Type};
	my $Src = $ExportRef->{Source};
	my $Dest = $ExportRef->{Releasable};
	$Dest = '' if (!defined($Dest));
	my @list = ();
	if ($Type eq 'zip') {
		unless (open PIPE, "unzip -l $Src | ") {
			warn "Can't unzip $Src\n";
		}
		while (<PIPE>) {
			if (/^\s*\d+\s+\S+\s+\S+\s+(.*?)\s*$/) {
#				ignore empty lines and anything that finishes with / 
				unless(($1=~/\/\s*$/) || ($1=~/^$/)) {

					my $member = $1;
					$member =~ s/\$/\$\$/g;
					if (!$Dest){
						push @list, "$ENV{EPOCROOT}$member";
					}
					else{
						push @list, "$Dest\\$member";
					}
				}
			}
		}
		close PIPE;
	}
	return @list;
}

sub CreateExportMak ($$$) {
	my ($Makefile, $ExportsRef, $ExpDirsRef)=@_;

#	create EXPORT.MAKE

	my $erasedefn = "\@erase";
	$erasedefn = "\@erase 2>>nul" if ($ENV{OS} eq "Windows_NT");
	&Output(
		"ERASE = $erasedefn\n",
		"\n",
		"\n",
		"EXPORT : EXPORTDIRS"
	);
	my $ref;
	if (@$ExportsRef) {
		foreach $ref (@$ExportsRef) {
			if ($$ref{Type} eq 'zip') {
				my @list = &GetArchiveExportList($ref);
				foreach (@list) {
					my $dst=$_;
					&Output(
						" \\\n",
						"\t$dst"
					);
				}
			} else {
				my $name=&Path_Quote($$ref{Releasable});
				&Output(
					" \\\n",
					"\t$name"
				);
			}
		}
	}
	else {
		&Output(
			" \n",
			"\t\@echo Nothing to do\n"
		);
	}
	&Output(
		"\n",
		"\n",
		"\n",
		"EXPORTDIRS :"
	);
	my $dir;
	foreach $dir (@$ExpDirsRef) {
		$dir=&Path_Quote($dir);
		&Output(
			" $dir"
		);
	}
	&Output(
		"\n",
		"\n"
	);
	foreach $dir (@$ExpDirsRef) {
		&Output(
			"$dir :\n",
			    "\t\@perl -w -S emkdir.pl \"\$\@\"\n",
			"\n"
		);
	}
	&Output(
		"\n",
		"\n"
	);
	foreach $ref (@$ExportsRef) {
		my $unzipoption = $$ref{UnzipOption};
		CheckSource_ExportedIncludes($$ref{Source}, $$ref{Releasable}, %CheckSourceEXPORTSIncludes);
		
		if ($$ref{Type} eq 'zip') {
			my $src = &Path_Quote($$ref{Source});
			my $destdir = &Path_Quote($$ref{Releasable});
			$destdir=$ENV{EPOCROOT} if (!defined($destdir) or ($destdir eq ''));
			my @list = &GetArchiveExportList($ref);
  			foreach (@list) {  				
  				my $dst=$_;
  				&Output(
 					"$dst : $src\n",
  				);
  			}
			if ($unzipoption =~ /overwrite/i){
				&Output(
				"\t- unzip -o $src -d \"$destdir\"\n",
				);
			}
			else{	
				&Output(
				"\t- unzip -u  -o $src -d \"$destdir\"\n",
				);
			}
		} else {
			my $dst=&Path_Quote($$ref{Releasable});
			my $src=&Path_Quote($$ref{Source});
			&Output(
				"$dst : $src\n",
					"\tcopy \"\$?\" \"\$\@\"\n",
				"\n"
			);
		}
	}
	&Output(
		"\n",
		"\n"
	);
	if (@$ExportsRef) {
		&Output(
			"CLEANEXPORT :\n"
		);
		foreach $ref (@$ExportsRef) {
			if ($$ref{Type} eq 'zip') {
				my @list = &GetArchiveExportList($ref);
				foreach (@list) {
					my $dst=$_;
					$dst =~ s/\//\\/go;
					&Output(
						"\t-\$(ERASE) \"$dst\"\n"
					);
				}
			} else {
				my $dst = $$ref{Releasable};
				$dst =~ s/\//\\/go;
				&Output(
					"\t-\$(ERASE) \"$dst\"\n"
				);
			}
		}
		&Output(
			"\n",
			"WHAT :\n"
		);
		foreach $ref (@$ExportsRef) {
			if ($$ref{Type} eq 'zip') {
				my @list = &GetArchiveExportList($ref);
				foreach (@list) {
					my $dst=$_;
					$dst =~ s/\//\\/go;
					&Output(
						"\t\@echo \"$dst\"\n"
					);
				}
			} else {
				my $dst = $$ref{Releasable};
				$dst =~ s/\//\\/go;
				&Output(
					"\t\@echo \"$dst\"\n"
				);
			}
		}
	}
	else {
		&Output(
			"CLEANEXPORT :\n",
			"\t\@echo Nothing to do\n",
			"WHAT :\n",
			"\t\@rem do nothing\n"
		);
	}

	&Output(
		"\nCHECKSOURCE :\n"
	);
	
	&Output (CheckSource_MakefileOutput(%CheckSourceEXPORTSMetaData));
	&Output (CheckSource_MakefileOutput(%CheckSourceEXPORTSIncludes));

	&Output("\n");
	
#	write EXPORT.MAKE
	&WriteOutFileL($Makefile);
}

sub CreatePlatExports ($$) {
	my ($RealPlat,$Exports)=@_;
	my $Ref;
	&Output(
 	"\n# Rules which handle the case when \$(CFG) is not defined\n\n" ,
 	"EXPORT:		\tEXPORTUREL EXPORTUDEB\n", 
 	"EXPORTCLEAN:	\tEXPORTCLEANUREL EXPORTCLEANUDEB\n",
 	"EXPORTWHAT:	\tEXPORTWHATUREL EXPORTWHATUDEB\n",
  
 	"\n# definitions \n",
 	"DATAx = $ENV{EPOCROOT}epoc32\\data\n",
 	"EMULx = $ENV{EPOCROOT}epoc32\\$RealPlat\n",
 	"URELx = $ENV{EPOCROOT}epoc32\\release\\$RealPlat\\urel\n",
 	"UDEBx = $ENV{EPOCROOT}epoc32\\release\\$RealPlat\\udeb\n",
 	"\n"
	);
	
	&Output( 
	"# Exports to emulated drive A: to Y \n\n",
	"EXPORTGENERIC : EXPORTDIRSGENERIC",
	);
	 
	my @dirs;
	my @expgen;
	my %dirsg;
	foreach $Ref (@$Exports) {
	 	if ($$Ref{emReleasable}=~/^([A-Y])(\\.*)$/){
		   my $exp="\\$$Ref{emReleasable}";
	 	   if ($$Ref{Type} eq 'zip') {	
			 my @list = &GetArchiveExportList($Ref);	
			  foreach (@list) {
				my $dst=&Path_Quote($_);
				if ($dst=~/([^\\]*)$/o){
					$dst=$1;
				}
				my $zipdest=$dst;
				$zipdest =~ s/\//\\/g;
			    push  @expgen, "$exp\\$zipdest";
			    my $zippath= &Path_Chop(&Path_Split('Path', $zipdest));
				if(!$zippath){
					$dirsg{$exp}=$exp;
				}
				else{
					 $dirsg{"$exp\\$zippath"}="$exp\\$zippath";
				}
				&Output(" \\\n","\t\$(EMULx)$exp\\$zipdest");
			 }      
		 }
		 else { 
		     my $dir =  &Path_Chop(&Path_Split('Path', $exp));  
			 push @expgen,  $exp;	
			 $dirsg{$dir}=$dir;
			 &Output(" \\\n", "\t\$(EMULx)$exp "); 
		 }
	   }
	}
	&Output("\n\nEXPORTDIRSGENERIC : ");
	foreach (keys %dirsg){
			 push @dirs, "\$(EMULx)$dirsg{$_}";
			 &Output(" \\\n", "\t\$(EMULx)$_"); 
	} 
	&Output("\n\n");
	foreach (@expgen){	 
			&Output( 
			"\$(EMULx)$_ : \t\$(DATAx)$_ \n",
			"\tcopy \"\$?\" \"\$@\" \n"
			);
	}	
	&Output("\nEXPORTCLEANGENERIC :\n");		
	foreach (@expgen){	 
			&Output("\t-@\$(ERASE) \$(EMULx)$_ \n");
	} 			
	&Output("\nEXPORTWHATGENERIC :\n");			
	foreach (@expgen){	   
			&Output("\t\@echo \$(EMULx)$_ \n");
	}
		
	&Output( 
	 		"\n\n# Exports to emulated drive Z: - UREL version \n\n",
	 		"EXPORTUREL : EXPORTDIRSUREL",
	 	   );
	
	my @expurel; 
	my %dirsurel;
	foreach $Ref (@$Exports) {
		if ($$Ref{emReleasable}=~/^(Z)(\\.*)$/){
			my $exp="\\$$Ref{emReleasable}";
	 	    if ($$Ref{Type} eq 'zip') {
			  my @list = &GetArchiveExportList($Ref);
			  foreach (@list) {
				my $dst=&Path_Quote($_);
				if ($dst=~/([^\\]*)$/o){
					$dst=$1;
				}
				my $zipdest=$dst;
				$zipdest=~ s/\//\\/g;
				push  @expurel, "$exp\\$zipdest"; 
				my $zippath= &Path_Chop(&Path_Split('Path', $zipdest)); 
				if(!$zippath){
				   $dirsurel{$exp}=$exp;
				}
				else{
					$dirsurel{"$exp\\$zippath"}="$exp\\$zippath";
				}
				&Output(" \\\n","\t\$(URELx)$exp\\$zipdest");
			}  
		}
		else {
			  my $dir =  &Path_Chop(&Path_Split('Path', $exp));  
			  push @expurel,  $exp; 
			  $dirsurel{$dir}=$dir;
			  &Output(" \\\n", "\t\$(URELx)$exp "); 
		}
	  }
	} 
	&Output("\n\nEXPORTDIRSUREL : ");
	foreach (keys %dirsurel){
			push @dirs, "\$(URELx)$dirsurel{$_}";
   			&Output(" \\\n", "\t\$(URELx)$_ "); 
	}
	&Output("\n\n");	
	foreach (@expurel){
			 &Output( 
					"\$(URELx)$_ : \t\$(DATAx)$_ \n",
					"\tcopy \"\$?\" \"\$@\" \n"
			 );
	}		
	&Output("\nEXPORTCLEANUREL :\n"); 		
	foreach (@expurel){	
			 &Output("\t-@\$(ERASE) \$(URELx)$_ \n"); 
	}  
	&Output("\nEXPORTWHATUREL :\n");	
	foreach (@expurel){	
			 &Output( "\t\@echo \$(URELx)$_ \n"); 	
	}
	   
	&Output( 
	 		"\n\n# Exports to emulated drive Z: - UDEB version \n\n",
	 		"EXPORTUDEB : EXPORTDIRSUDEB",
	);  
	 	   
	my %dirsudeb=%dirsurel;          
	my @expudeb=@expurel;
	foreach (@expudeb){
	         &Output(" \\\n", "\t\$(UDEBx)$_ ");		  
	}
	&Output("\n\nEXPORTDIRSUDEB : ");
	foreach(keys %dirsudeb){
	  		push @dirs, "\$(UDEBx)$dirsudeb{$_}";
	  		&Output(" \\\n", "\t\$(UDEBx)$_ "); 
	  	
	}
	&Output("\n\n");
	foreach (@expudeb){
	 		 &Output( 
					"\$(UDEBx)$_ : \t\$(DATAx)$_ \n",
					"\tcopy \"\$?\" \"\$@\" \n"
			 );
	}			
	&Output("\nEXPORTCLEANUDEB :\n");
	foreach (@expudeb){	
			 &Output("\t-@\$(ERASE) \$(UDEBx)$_ \n"); 
	}  
	&Output("\nEXPORTWHATUDEB :\n");	
	foreach (@expudeb){	
			 &Output("\t\@echo \$(UDEBx)$_ \n"); 	
	}
	
    &Output("\n# Directories \n\n");  
	&Output(join (" \\\n", @dirs)." :")       
	&Output("\n\t\@perl -w -S emkdir.pl \$@\n\n");			
		   		
}

sub CreatePlatMak ($$$$$$$$$;$) {
	my ($BatchPath, $E32MakePath, $DataRef, $Plat, $RealPlat, $RomDir, $Module, $BldInfPath, $Exports, $Test, $FeatureVariant)=@_;
	$FeatureVariant = "" if (!$FeatureVariant);

	unless ($Test) {
		$Test='';
	}
	else {
		$Test='TEST';
	}

	my $Ref;
	my $eDrive=0;
	if ($RealPlat =~ /^WINS/) {
	    foreach $Ref (@$Exports) {
			if ($$Ref{emReleasable}=~/^([A-Z])(\\.*)$/) {
				$eDrive=1;
				last;
			}
		}
	} 


	my $OutRomFile="$RomDir$RealPlat$Test.IBY";
	my $GCCDir="gcc\$(PBUILDPID)\\bin";
	
	my $erasedefn = "\@erase";
	$erasedefn = "\@erase 2>>nul" if ($ENV{OS} eq "Windows_NT");

# Get the root platform name to support hierarchy of customizations	
	my $root = Plat_Root($Plat);

	my $config_file = "";

	if (grep /^$root$/i, @BPABIPlats) {
		$config_file = BPABIutl_Config_Path($root);
	}

	my $rvct_path = "";

	if ( $config_file ) {

		if ($root =~ /^ARMV\d/) {

			unless ( RVCT_plat2set::compiler_exists($Plat) )
			{
				FatalError("Can't find any RVCT installation.");
			}

			my $rvct_ver = RVCT_plat2set::get_version_string($Plat);

			if ($Plat =~ "^ARMV5" && $rvct_ver lt "2.2.559")
			{
				warn "BLDMAKE WARNING: ARMV5 requires at least RVCT 2.2.559.";
				OutText();
				return;
			}

			if ($Plat =~ "^ARMV6" && $rvct_ver lt "2.2.559")
			{
				warn "BLDMAKE WARNING: ARMV6 requires at least RVCT 2.2.559.";
				OutText();
				return;
			}

			if ($Plat =~ "^ARMV7" && $rvct_ver lt "3.1.674")
			{
				warn "BLDMAKE WARNING: ARMV7 requires at least RVCT 3.1.674.";
				OutText();
				return;
			}

			my $rvct_bin_name = RVCT_plat2set::get_bin_name($Plat);
			my $rvct_bin_path = RVCT_plat2set::get_bin_path($Plat);
			my $rvct_inc_name = RVCT_plat2set::get_inc_name($Plat);
			my $rvct_inc_path = RVCT_plat2set::get_inc_path($Plat);
			my $rvct_lib_name = RVCT_plat2set::get_lib_name($Plat);
			my $rvct_lib_path = RVCT_plat2set::get_lib_path($Plat);

			main::Output("export $rvct_bin_name:=$rvct_bin_path\n");
			main::Output("export $rvct_inc_name:=$rvct_inc_path\n");
			main::Output("export $rvct_lib_name:=$rvct_lib_path\n");

			my ($rvct_M, $rvct_m, $rvct_b) = RVCT_plat2set::get_version_list($Plat);

			Output( "\n" );
			Output( "export RVCT_VER_MAJOR:=$rvct_M\n" );
			Output( "export RVCT_VER_MINOR:=$rvct_m\n" );
			Output( "export RVCT_VER_BUILD:=$rvct_b\n" );

			$rvct_path = "\$($rvct_bin_name);"; # Example: '$(RVCT22BIN);'.
		}

		&Output(
			"\n",
			"export PLAT:=${Plat}\n\n",
			"include $config_file\n\n"
		);
	}
# modified start: makefile improvement 
	unless($FeatureVariant eq "")
	{
# modified by SV start: makefile improvement 
		foreach $Ref (@$DataRef) {
			&Output(
			"include $BatchPath"."FeatureVariantInfo\\".uc($Plat)."\\"."$Plat$FeatureVariant.$$Ref{Base}.info\n\n",
			);
		}
# modified by SV end: makefile improvement 
	}
# modified end: makefile improvement 
	# Don't hardcode the rvct path if rvct auto switch feature is not enabled.
	if ($ENV{ABLD_PLAT_INI}) 
	{
		&Output(
		'export Path:=',&main::Path_Drive,$E32env::Data{EPOCPath},$GCCDir,";", $rvct_path,"\$(Path)\n",
		"export PATH:=\$(Path)\n"
		);
	}
	else
	{
		&Output(
		'export Path:=',&main::Path_Drive,$E32env::Data{EPOCPath},$GCCDir,";", "\$(Path)\n",
		"export PATH:=\$(Path)\n"
		);
	}

	&Output(		
		"\n",
		"# prevent MAKEFLAGS variable from upsetting calls to NMAKE\n",
		"unexport MAKEFLAGS\n",
		"\n",
		"ERASE = $erasedefn\n",
		"\n",
		"\n",
		"ifdef EFREEZE_ALLOW_REMOVE\n",
		"REMOVEMACRO := EFREEZE_ALLOW_REMOVE=-remove\n",
		"endif\n",
		"\n",
		"\n"
	);

	if ($eDrive) {
		# Generate exports into emulated drives
		&CreatePlatExports($RealPlat,$Exports);
	}
	my $Command;
	foreach $Command (qw(CLEAN CLEANMAKEFILE CLEANALL FINAL FREEZE LIBRARY MAKEFILE RESOURCE SAVESPACE TARGET LISTING WHATMAKEFILE)) {
		&Output(
			"$Command :"
		);

			 if ($eDrive and $Command eq 'CLEAN'){
				 &Output(" EXPORTCLEANGENERIC EXPORTCLEAN\$(CFG) ");
				 foreach $Ref (@$DataRef) {
					 &Output(" $Command$$Ref{Base}");
				 }
			 }	    	 
			 elsif ($eDrive and $Command eq 'RESOURCE'){
				 &Output(" EXPORTGENERIC EXPORT\$(CFG) ");
			   	 foreach $Ref (@$DataRef) {
					 &Output(" $Command$$Ref{Base}");
				 }
				 
				 foreach $Ref (@$DataRef) {
					 &Output("\n\nRESOURCE$$Ref{Base} : EXPORTGENERIC EXPORT\$(CFG)");
				 }
			  }
			  else {
			        if(@$DataRef){
			        	foreach $Ref (@$DataRef) {
			        		&Output(" $Command$$Ref{Base}");
			           }
			          } 		
			         else {
			         	&Output("\n","\t\@echo Nothing to do\n");
			         }
			  }  
		&Output("\n","\n");
	}
	
	&Output(
		"WHAT :"
	);
	if($eDrive){
	  &Output(" EXPORTWHATGENERIC EXPORTWHAT\$(CFG) ");
	}
	my $whatcount=0;
	foreach $Ref (@$DataRef) {
		unless ($$Ref{Tidy}) {
			$whatcount++;
			&Output(
				" WHAT$$Ref{Base}"
			);
		}
	}
	if ($whatcount==0 and !$eDrive) {
		&Output(
			"\n",
			"\t\@rem do nothing\n" 
		);
	}

	&Output(
		"\n",
		"\n",
		"CHECKSOURCE :"
	);
	my $CheckSource=' CHECKSOURCE_GENERIC';
	foreach $Ref (@$DataRef) {
		$CheckSource.=" CHECKSOURCE$$Ref{Base}" if ($$Ref{Ext} eq ".MMP");
	}
	&Output(
		"$CheckSource\n"
	);

	&Output(
		"\n",
		"CHECKSOURCE_GENERIC :\n"
	);

	if ($CheckSourceBldInfIncludes{$Plat})
		{
		my %dummy;
		$dummy{$CheckSourceBldInfIncludes{$Plat}} = 1;
		&Output (CheckSource_MakefileOutput(%dummy));		
		}

	&Output (CheckSource_MakefileOutput(%CheckSourceMMPFILESMetaData));
	&Output (CheckSource_MakefileOutput(%CheckSourceEXTENSIONSMetaData));
	
	&Output(
		"\n",
		"\n",
		"TIDY :"
	);
	my $Tidy='';
	foreach $Ref (@$DataRef) {
		if ($$Ref{Tidy}) {
			$Tidy.=" TIDY$$Ref{Base}";
		}
	}
	if ($Tidy) {
		&Output(
			"$Tidy\n"
		);
	}
	else {
		&Output(
			"\n",
			"\t\@echo Nothing to do\n"
		);
	}
	&Output(
		"\n",
		"\n"
	);
#	change for non-EPOC platforms
	if ($RealPlat=~/^(WINS|WINSCW|WINC|TOOLS|TOOLS2)$/o) {
		&Output(
			"ROMFILE :\n"
		);
	}
	else {
		&Output(
			'ROMFILE : STARTROMFILE'
		);
		foreach $Ref (@$DataRef) {
			&Output(
				" ROMFILE$$Ref{Base}"
			);
		}
		&Output(
			"\n",
			"\n",
			"STARTROMFILE :\n",
			    "\t\@perl -w -S emkdir.pl \"", &Path_Chop($RomDir), "\"\n",
			    "\t\@echo // $OutRomFile > $OutRomFile\n",
			    "\t\@echo // >> $OutRomFile\n"
		);
		if ($Test) {
			my ($Auto, $Manual);
			foreach $Ref (@$DataRef) {
				++$Auto unless ($$Ref{Manual} or $$Ref{Support});
				++$Manual if ($$Ref{Manual});
			}
			if ($Auto) {
				my $IbyTextFrom="data=$BatchPath$Plat.AUTO.BAT";
				my $IbyTextTo="Test\\$Module.AUTO.BAT";
				my $Spaces= 60>length($IbyTextFrom) ? 60-length($IbyTextFrom) : 1; 
				&Output("\t\@echo ", $IbyTextFrom, ' 'x$Spaces, $IbyTextTo, ">> $OutRomFile\n");
			}
			if ($Manual) {
				my $IbyTextFrom="data=$BatchPath$Plat.MANUAL.BAT";
				my $IbyTextTo="Test\\$Module.MANUAL.BAT";
				my $Spaces= 60>length($IbyTextFrom) ? 60-length($IbyTextFrom) : 1; 
				&Output("\t\@echo ", $IbyTextFrom, ' 'x$Spaces, $IbyTextTo, ">> $OutRomFile\n");
			}
		}
	}
	&Output(
		"\n",
		"\n"
	);
	my $CallNmake='nmake -nologo -x - $(VERBOSE) $(KEEPGOING)';
	my $CallGNUmake='$(MAKE) $(VERBOSE) $(KEEPGOING)';

	my %PlatHash;
	&Plat_GetL($RealPlat, \%PlatHash);
	my $CallMake=$CallNmake;
	if ($PlatHash{MakeCmd} eq "make") {
		$CallMake="$CallGNUmake -r";
	}
	&Plat_GetL($Plat, \%PlatHash);
	
	foreach $Ref (@$DataRef) {

#		standard commands
		unless ($$Ref{Makefile}) {
			my $MakefilePath=join('', &Path_Chop($E32MakePath), $BldInfPath, $$Ref{Base}, "\\", $RealPlat, "\\");
# modified start: makefile improvement 
			my $RealMakefile;
			if($FeatureVariant eq "")
			{
				$RealMakefile="-f \"$MakefilePath$$Ref{Base}.$RealPlat$FeatureVariant\"";
			}
			else{
				$RealMakefile="-f \"$MakefilePath$$Ref{Base}.$RealPlat.\$(VARIANT_PLAT_NAME_$$Ref{Base})\"";
			}
# modified end: makefile improvement 
			my $MakefileBase="$MakefilePath$$Ref{Base}";		

			if($Plat eq 'VS6' || $Plat eq 'VS2003')
			{
				$CallMake .= "-f ";
				$RealMakefile = "$MakefileBase$PlatHash{Ext}";
			}
			&Output(
				"MAKEFILE$$Ref{Base}_FILES= \\\n",
					"\t\"$MakefileBase$PlatHash{Ext}$FeatureVariant\"",
			);
#			changes for WINS/WINSCW/WINC and VC6
			if ($Plat =~ /^VC6/) {
				&Output(
					" \\\n\t\"$MakefileBase.DSW\"",
					" \\\n\t\"$MakefileBase.SUP.MAKE\""
				);
			}
			if ($Plat eq 'CW_IDE') {
				&Output(
					" \\\n\t\"$MakefileBase.pref\""		# Defect: actually uses $BaseTrg, not mmp file name
				);
			}
			if ($RealPlat=~/^(WINS|WINSCW|WINC)$/o) {
				&Output(
					" \\\n\t\"$MakefileBase.UID.CPP\""	# Defect: actually uses $BaseTrg, not mmp file name
				);
			}
			
  			my $bld_flags="";
			$bld_flags="\$(ABLD_FLAGS)" if (($Plat =~ /^ARMV5(_ABIV1)?$/ || grep /$Plat/i, @BPABIPlats) || (Plat_Root($Plat) =~ /^ARMV5(_ABIV1)?$/ || grep /$Plat/i, @BPABIPlats));
			
			
			my $build_as_arm_arg="";
			$build_as_arm_arg = $$Ref{BuildAsARM} if ($$Ref{BuildAsARM});

			# Compiler Wrapper option Support  
			# Generate the flag to keep the Compiler Wrapper option information
			my $cmp_wrap_flag="";
			if (($Plat =~ /^ARMV5(_ABIV1)?$/ || grep /$Plat/i, @BPABIPlats) || ($Plat=~/^WINSCW$/) || (Plat_Root($Plat) =~ /^ARMV5(_ABIV1)?$/ || grep /$Plat/i, @BPABIPlats))
			{
				# for armv5 , armv5_abiv1, winscw and all bpabi plaforms
				$cmp_wrap_flag="\$(ABLD_COMPWRAP_FLAG)" ;
			}

			&Output(
				"\n",
				"\n",
				"MAKEFILE$$Ref{Base} :\n",
				    "\tperl -w -S makmake.pl \$(NO_DEPENDENCIES) -D $$Ref{Path}$$Ref{Base} $Plat$FeatureVariant $build_as_arm_arg $bld_flags $cmp_wrap_flag\n",

				"\n",
				"CLEANMAKEFILE$$Ref{Base} :\n",
				    "\t-\$(ERASE) \$(MAKEFILE$$Ref{Base}_FILES)\n",
				"\n",
				"WHATMAKEFILE$$Ref{Base} :\n",
				    "\t\@echo \$(MAKEFILE$$Ref{Base}_FILES)\n",
				"\n",
				"TARGET$$Ref{Base} :\n",
				    "\t$CallMake $RealMakefile \$(CFG)\n",
				"\n",
				"SAVESPACE$$Ref{Base} :\n",
				    "\t$CallMake $RealMakefile \$(CFG) CLEANBUILD\$(CFG)\n",
				"\n",
				"LISTING$$Ref{Base} :\n",
				    "\t$CallMake $RealMakefile MAKEWORK\$(CFG) LISTING\$(CFG)\$(SOURCE)\n",
				"\n",
				"FINAL$$Ref{Base} :\n",
				    "\t\@rem do nothing\n",
				"\n",
			);
			foreach $Command (qw(CLEANALL)) {
				&Output(
					"CLEANALL$$Ref{Base} :\n",
					"\tperl -w -S ermdir.pl $MakefilePath\n",
					"\n",
				);
			}
			foreach $Command (qw(CLEAN RESOURCE)) {
				&Output(
					"$Command$$Ref{Base} :\n",
					    "\t$CallMake $RealMakefile $Command\$(CFG)\n",
					"\n"
				);
			}
			foreach $Command (qw(LIBRARY)) {
				&Output(
					"$Command$$Ref{Base} :\n",
					    "\t$CallMake $RealMakefile $Command\n",
					"\n"
				);
			}
			foreach $Command (qw(FREEZE)) {
				&Output(
					"$Command$$Ref{Base} :\n",
					    "\t$CallMake $RealMakefile $Command \$(REMOVEMACRO)\n",
					"\n"
				);
			}
			unless ($$Ref{Tidy}) {
				&Output(
					"WHAT$$Ref{Base} :\n",
					    "\t\@$CallMake -s $RealMakefile WHAT\$(CFG)\n",
					"\n"
				);
			}
			else {
				&Output(
					"TIDY$$Ref{Base} :\n",
					    "\t$CallMake $RealMakefile CLEANRELEASE CLEANLIBRARY\n",
					"\n"
				);
			}
			
			&Output(
				"CHECKSOURCE$$Ref{Base} :\n",
					"\t\@$CallMake -s $RealMakefile CHECKSOURCE\n",
					"\t\@$CallMake -s $RealMakefile CHECKSOURCE\$(CFG)\n",
				"\n"
				);
			&Output(
				"ROMFILE$$Ref{Base} :\n",
				    "\t\@$CallMake $RealMakefile ROMFILE >> $OutRomFile\n",
				"\n",
				"\n"
			);
		}

#		calls to custom makefiles
		else {
			my $ChopRefPath=&Path_Chop($$Ref{Path});
			my $ChopBldInfPath=&Path_Chop($BldInfPath);
			my $MakefileCall;
			if ($$Ref{Makefile}==2) {
				$MakefileCall="cd $ChopRefPath;$CallNmake";
			} else {
				$MakefileCall="$CallGNUmake -C $ChopRefPath";
			}
			$MakefileCall.=" -f \"$$Ref{Base}$$Ref{Ext}\" TO_ROOT=";
			$MakefileCall.=&Path_Chop(&Path_UpToRoot($$Ref{Path}));
			$MakefileCall.=" EPOCBLD=";
			$MakefileCall.=join('', &Path_Chop(&Path_UpToRoot($$Ref{Path})), &Path_Chop($E32MakePath), $BldInfPath, $$Ref{Base}, "\\", $RealPlat);
			$MakefileCall.=" TO_BLDINF=";
			$MakefileCall.=join('', &Path_Chop(&Path_UpToRoot($$Ref{Path})), $ChopBldInfPath);
			if ($$Ref{ExtensionRoot}) {
				$MakefileCall.=" EXTENSION_ROOT=".&Path_Chop($$Ref{ExtensionRoot});
			}
			if ($$Ref{BuildAsARM}) {
			  $MakefileCall.=" BUILD_AS_ARM=1";
			}			  
			&Output(
# should change to MAKEFILE
				"MAKEFILE$$Ref{Base} :\n",
				    "\t$MakefileCall PLATFORM=$Plat MAKMAKE\n",
				"\n",
# should call in custom makefiles maybe
				"CLEANMAKEFILE$$Ref{Base} :\n",
				"#	$MakefileCall PLATFORM=$Plat CLEANMAKEFILE\n",
				"\n",
				"WHATMAKEFILE$$Ref{Base} :\n",
				"#	\@$MakefileCall -s PLATFORM=$Plat WHATMAKEFILE\n",
				"\n",
# should change to TARGET
				"TARGET$$Ref{Base} :\n",
				    "\t$MakefileCall PLATFORM=$RealPlat CFG=\$(CFG) BLD\n",
				"\n",
# should ignore this target and just call the TARGET target instead?
				"SAVESPACE$$Ref{Base} :\n",
				    "\t$MakefileCall PLATFORM=$RealPlat CFG=\$(CFG) SAVESPACE\n",
				"\n",
				"LISTING$$Ref{Base} :\n",
				"\n",
				"\n",
# should change to LIBRARY
				"LIBRARY$$Ref{Base} :\n",
				    "\t$MakefileCall PLATFORM=$RealPlat LIB\n",
				"\n",
				"FREEZE$$Ref{Base} :\n",
					"\t$MakefileCall PLATFORM=$RealPlat FREEZE \$(REMOVEMACRO)\n",
				"\n",
			);

			foreach $Command (qw(CLEANALL)) {
				&Output(
					"$Command$$Ref{Base} :\n",
					"\t$MakefileCall PLATFORM=$RealPlat CFG=\$(CFG) CLEAN\n",
					"\n"
				);
			}

			foreach $Command (qw(CLEAN RESOURCE FINAL)) {
				&Output(
					"$Command$$Ref{Base} :\n",
					    "\t$MakefileCall PLATFORM=$RealPlat CFG=\$(CFG) $Command\n",
					"\n"
				);
			}
			unless ($$Ref{Tidy}) {
# should change to WHAT
				&Output(
					"WHAT$$Ref{Base} :\n",
					    "\t\@$MakefileCall -s PLATFORM=$RealPlat CFG=\$(CFG) RELEASABLES\n",
					"\n"
				);
			}
			else {
				&Output(
					"TIDY$$Ref{Base} :\n",
					    "\t$MakefileCall PLATFORM=$RealPlat TIDY\n",
# should change to CLEANLIBRARY
					    "\t$MakefileCall CLEANLIB\n",
					"\n"
				);
			}
			&Output(
				"ROMFILE$$Ref{Base} :\n",
				    "\t\@$MakefileCall PLATFORM=$RealPlat ROMFILE >> $OutRomFile\n",
				"\n",
				"\n"
			);
		}

	}
	
	&WriteOutFileL("$BatchPath$Plat$FeatureVariant$Test.MAKE");
}

sub CreatePlatBatches ($$$) {
	my ($OutDir, $DataRef, $Plat)=@_;

#	create the test batch files
#	this function won't work properly if the target basename is different from the .MMP basename
#	so perhaps it should call makmake on the .mmp file to check

	my $AutoText;
	my $ManualText;

	my $Ref;
	foreach $Ref (@$DataRef) {
		if ($$Ref{Manual}) {
			$ManualText.="$$Ref{Base}\n";
			next;
		}
		if ($$Ref{Ext} eq ".MK") {
			next;
		}
		if ($$Ref{Support}) {
			next;
		}
		else {
			$AutoText.="$$Ref{Base}\n";
		}
	}

	if ($AutoText) {
		&Output($AutoText);
		&WriteOutFileL("$OutDir$Plat.AUTO.BAT");
	}

	if ($ManualText) {
		&Output($ManualText);
		&WriteOutFileL("$OutDir$Plat.MANUAL.BAT");
	}
}

sub WriteOutFileL ($$) { # takes batch file and boolean read-only flag
	my ($BATFILE, $ReadOnly)=@_;

	$BATFILE=~ s/\//\\/g;  # convert unix slashes from wrappermakefile.pm

	eval { &Path_MakePathL($BATFILE); };
	&FatalError($@) if $@;

	open BATFILE,">$BATFILE" or &FatalError("Can't open or create Batch File \"$BATFILE\"");
	print BATFILE &OutText or &FatalError("Can't write output to Batch File \"$BATFILE\"");
	close BATFILE or &FatalError("Can't close Batch File \"$BATFILE\"");
}


