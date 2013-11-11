# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

use strict;

package		wrappermakefile;
require Exporter;
@wrappermakefile::ISA=qw(Exporter);
@wrappermakefile::EXPORT=qw(
	GenerateWrapper
);

use Output;

my (@ExtensionUsedBefore);
my $ExtTemplatesPath="$ENV{EPOCROOT}epoc32/tools/makefile_templates/";
$ExtTemplatesPath=~ s/\\/\//g;  # convert win32 slashes to unix slashes
my $DieRef;
our $ExtMakefileMissing = 0;

# Add prototyes to avoid -w warnings
sub GenerateWrapper($$$$@);  
sub ReadMetaFile($$$$);


sub GenerateWrapper($$$$@) {	
	my ($Plat,$OutDir, $ErrorString, $dieRef, @ExtensionBlockData) = @_;
	$DieRef = $dieRef;
	
#	Generates a makefile which contains the settings for each invocation of an extension and calls
#	through to the extension template makefile in /epoc32/tools/makefile_templates.
#	@ExtensionBlockData holds the settings (makefile MACROs) defined within the start..end block in the bld.inf.

	my $ExtHeader = shift(@ExtensionBlockData);

	if (uc $ExtHeader->[0] ne "EXTENSION") {
		&main::FatalError("$ErrorString : Extension Template blocks must contain the 'extension' keyword.\n");
	}
	if ($ExtHeader->[1] eq "") {
		&main::FatalError("$ErrorString : The 'extension' does not have a name, the correct format is: 'start extension <name>'.\n");
	}
	my $ExtName = $ExtHeader->[1];

#	Ensure that each extension invocation is uniquely identified by means of the global array 
#	ExtensionUsedBefore.
	my $Count=0;
	push @ExtensionUsedBefore, $ExtName.$Plat;
	foreach my $item(@ExtensionUsedBefore) {
		if ($item eq $ExtName.$Plat) {
			$Count = $Count + 1;
		}
	}

#	Process the meta information for this extension template.
	my %Data;
	my %WrapperMakefileData = ReadMetaFile($ExtName, $ErrorString, \$Data{Makefile},$dieRef);
	if ($ExtMakefileMissing)
	{
		main::ExtensionMakefileMissing($ExtMakefileMissing);
		return;
	}
	$Data{Ext}='.MK';
	$Data{Base}=$ExtName."_".$Plat."_".$Count;
	$Data{Path}=$OutDir."wrappermakefiles/";

#	Process ExtensionBlockData
	foreach my $ExtBlockStatement (@ExtensionBlockData) {
		my $Key = shift(@$ExtBlockStatement);
		$WrapperMakefileData{$Key}=$ExtBlockStatement;
	}

#	CreateWrapperMakefile
	&Output("# Bldmake generated wrapper makefile\n\n");
	foreach my $Key (keys (%WrapperMakefileData))
	{
			&Output("$Key = "); 
			foreach my $item(@{$WrapperMakefileData{$Key}}) {
				&Output(" $item");
			}
			&Output("\n");
	}
	my $Filename = lc("$ExtName$Data{Ext}");
	&Output("\ninclude $ExtTemplatesPath$Filename"); 
	my $Path = lc("$Data{Path}");
	&main::WriteOutFileL(lc ($Path.$ExtName."_".$Plat."_".$Count.$Data{Ext}));
	%WrapperMakefileData = ();
	return %Data;  
}

#  Read extension meta file data
sub ReadMetaFile($$$$) {
	my ($ExtName, $ErrorString, $Makefile,$dieRef) = @_;

#	Parse the .meta file (found via $ExtName) for this extension template and return a hash 
#	containing the default values (makefile MACROs) for this template.
#	$Makefile is updated to indicate whether this a gnumake or nmake makefile.

	my %MetaData;
	my $Filename = lc("$ExtName");
	$ExtMakefileMissing=0;
	if (!open(METAFILE,"$ExtTemplatesPath$Filename.meta"))
	{
		&main::WarnOrDie($dieRef, "$ErrorString : Extension: $ExtName - cannot open META file: $ExtTemplatesPath$Filename.meta\n");
		$ExtMakefileMissing = 1;
		return;
	}
	LINE: while (<METAFILE>) {
		chomp;		
		my @Elements = split(/\s*\s\s*/);		
		my $Keyword = uc shift(@Elements);

		if ((! $Keyword) or ($Keyword =~ m/^#/)) {
			next LINE;
		}

		if ($Keyword eq "MAKEFILE") {	
			my $Makefiletype = uc shift(@Elements);		
			if ($Makefiletype eq "GNUMAKE") {
				$$Makefile=1;
			}
			if ($Makefiletype eq "NMAKE") {
				$$Makefile=2;
			}
		}
		elsif ($Keyword eq "OPTION") {
			my $Key = shift(@Elements);
			if ((! $Key) or (! $Elements[0])) {
					&main::FatalError("$ErrorString : There is a syntax error in the META file ($ExtTemplatesPath$Filename.meta) for this extension ($ExtName) - the correct format for default options is: 'option <key> <value>'");
				}
			my @Values = @Elements;
			$MetaData{$Key}=\@Values;
		} 
		elsif (($Keyword eq "TECHSTREAM") or ($Keyword eq "PLATFORM")) {
			# not used
		}
		else {
			&main::FatalError("$ErrorString : There is a syntax error in the META file ($ExtTemplatesPath$Filename.meta) for this extension ($ExtName) - unrecognised keyword:$Keyword");	
		}
	}
	if (! $$Makefile) {
		&main::FatalError("$ErrorString : There is a syntax error in the META file ($ExtTemplatesPath$Filename.meta) for this extension ($ExtName) - makefile type not specified [gnumake|nmake]");
	}
	close(METAFILE) or &main::FatalError("$ErrorString : Extension: $ExtName - cannot close Meta File:$ExtTemplatesPath$ExtName.meta\n");
	return %MetaData;
}

1;
