# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Routines involved in checking that source matches various filename policy constraints
# 
#


package CheckSource;

require Exporter;
@ISA=qw(Exporter);

# Exported for clarity in calling scripts and modules
our $CheckSource_PhysicalCheck = 1;
our $CheckSource_NoUserSystemDistinction = 1;

@EXPORT=qw(
	CheckSource_MetaData
	CheckSource_Includes
	CheckSource_ExportedIncludes
	CheckSource_MakefileOutput
	CheckSource_UnixSlash
	CheckSource_Lowercase
	CheckSource_Physical
	$CheckSource_PhysicalCheck
	$CheckSource_NoUserSystemDistinction
);

use strict;
use Cwd;
use Pathutl;
use Win32;

my $exclusionsFile = $ENV{EPOCROOT}."epoc32\\tools\\filenamepolicyexclusions.txt";

my $makefileWarningPrefix = "\@echo ";
my $checksourcePrefix = "\@perl -w -S checksource.pl";
my $releaseLocationRoot = quotemeta ($ENV{EPOCROOT}."epoc32");


sub CheckSource_MetaData (\%$$$$;$;$)
	{
	my ($actionHash, $sourceFile, $item, $reference, $lineNumber, $physical, $offset) = @_;

	return if ($reference =~ /^(\.|\.\.)$/);

	my $checksourceCall = "$checksourcePrefix --metadata \"$sourceFile\" \"$item\" \"$reference\" $lineNumber";
	$checksourceCall .= " $physical" if ($physical);
	$checksourceCall .= " \"$offset\"" if ($offset);
	$$actionHash{$checksourceCall} = 1;
	}


sub CheckSource_UnixSlash (\%$$$$;$)
	{
	my ($actionHash, $sourceFile, $item, $reference, $lineNumber, $verbose) = @_;

	$sourceFile =~ s/^[a-zA-Z]{1}://;
	$item =~ s/PRJ_EXPORTS \(NO DESTINATION\)/PRJ_EXPORTS/;

	print "Checking - Unix slash : $sourceFile ($lineNumber) - $item:$reference\n" if ($verbose);

	if ($reference =~/\\/)
		{	
		$$actionHash{constructWarning ($sourceFile, $lineNumber, $item, "Incorrect slash in", $reference)} = 1;
		}
	}


sub CheckSource_Lowercase (\%$$$;$;$)
	{
	my ($actionHash, $sourceFile, $item, $reference, $lineNumber, $verbose) = @_;

	return if ($reference =~ /^[\/|\\]epoc32[\/|\\]tools[\/|\\].*$/);

	$sourceFile =~ s/^[a-zA-Z]{1}://;

	print "Checking - lowercase  : $sourceFile ($lineNumber) - $item:$reference\n" if ($verbose);

	my $exclusion = lowercaseExclusionCheck ($reference);

	if ($exclusion eq "UNLISTED")
		{
		if ($reference =~ /[A-Z]/)
			{			
			$$actionHash{constructWarning ($sourceFile, $lineNumber, $item, "Incorrect case for epoc32 tree in", $reference)} = 1;
			}
		}
	elsif ($exclusion !~ /(OK|ERROR)/)
		{
		$$actionHash{constructWarning($sourceFile, $lineNumber, $item, "Incorrect case versus exclusion list in", $reference, " vs. $exclusion")} = 1;		
		}
	}


sub CheckSource_Physical (\%$$$$;$;$)
	{
	my ($actionHash, $sourceFile, $item, $reference, $lineNumber, $path, $verbose) = @_;
	
	print "Checking - physical   : $sourceFile ($lineNumber) - $item:$reference:$path\n" if ($verbose);

	my $physicalCheck;
	my $searchText;

	my $examineDefaultExportDestination = ($item =~ s/PRJ_EXPORTS \(NO DESTINATION\)/PRJ_EXPORTS/) ? 1 : 0;

	if ($item eq "#include")
		{
		# In the case of #includes, the path passed in is already the fully pathed physical file
		# that needs to be checked (as obtained from the parsed output of CPP)
		$searchText = $reference;
		$physicalCheck = $path;
		}
	else
		{
		my $physicalReference;
		if (($item =~ /^PRJ_(TEST)?MMPFILES MMP/) && ($reference !~ /\.\w+$/i))
			{
			$physicalReference = $reference."\.mmp";
			$searchText = $reference."\.*";
			}
		elsif ($item =~ /^DEFFILE/)
			{
			# The full path for DEFFILE entries is always passed in, so we just
			# need to concentrate on sorting out the oddities for the search
			# text and then just take the final file bit as the physical
			# reference

			$searchText = $reference;			
			$searchText .= "?\.*" if ($reference !~ /(\.def|[\\|\/])$/i);
			$searchText =~ s/(\.def)/\?$1/i if ($item !~ /NOSTRICTDEF/);
			$searchText =~ s/\~/\*/;			
			$physicalReference = $searchText;
			$physicalReference =~ s/\//\\/g;
			$physicalReference =~ s/\?/u/;
			$physicalReference =~ s/\.\w+$/\.def/;
			$physicalReference = &Path_Split ('File', $physicalReference);			
			}
		else
			{
			$searchText = $reference;
			$physicalReference = $searchText;
			}

		my $physicalLocation;
		if ($path)
			{
			$physicalLocation = $path;
			}
		elsif ($reference =~ /^(\\|\/)/)
			{
			$physicalLocation = $ENV{EPOCROOT};
			}
		elsif ($reference =~ /^\+/)
			{
			$physicalLocation = $ENV{EPOCROOT}."epoc32\\";
			}
		elsif ($item =~ /EXTENSIONS/)
			{
			$physicalLocation = $ENV{EPOCROOT}."epoc32\\tools\\makefile_templates\\";
			}
		else
			{
			$physicalLocation = &Path_Split ('Path', $sourceFile);
			}
		$physicalReference =~ s/^[\\|\/]//;
		$physicalCheck = $physicalLocation.$physicalReference;
		}

	$physicalCheck =~ s/\//\\/g;
	$physicalCheck = &Path_Strip ($physicalCheck);

	# If a file reference is actually under \epoc32, we just need to confirm that it's lowercase
	if ($physicalCheck =~ /^$releaseLocationRoot/i)
		{
		CheckSource_Lowercase (%$actionHash, $sourceFile, $item, $reference, $lineNumber, $verbose);
		return;
		}

	# Massage search text to provide something we can compare with a physical check on the filesystem
	$searchText =~ s/\//\\/g;
	$searchText =~ s/\.\.\\//g;
	$searchText =~ s/\.\\//g;
	$searchText =~ s/\\\\/\\/g;

	my $warningSearchText = $searchText;	# Record a more intelligible version of the search text for warning purposes
	
	$searchText = quotemeta ($searchText);
	$searchText =~ s/\\\*/\\w\+/g;			# * -> \w+
	$searchText =~ s/\\\?/\\w\{1\}/g;		# ? -> \w{1}

	my $physicalReality = getPhysical ($physicalCheck);

	my $warningSuffix = "";

	if (!$physicalReality)
		{
		$$actionHash{constructWarning($sourceFile, $lineNumber, $item, "Can\'t find physical file match for", $reference, " on filesystem")} = 1;
		}
	elsif ($physicalReality !~ /^.*$searchText$/)
		{
		if ($physicalReality !~ /^.*$searchText$/i)
			{
			# Doesn't just differ in case...something's gone wrong
			$$actionHash{constructWarning($sourceFile, $lineNumber, $item, "Can\'t find physical file match for", $reference, " - match was attempted against $physicalReality")} = 1;		
			}
		else
			{
			if (($item =~ /^DEFFILE/ || $item =~ /^PRJ_(TEST)?MMPFILES MMP/) && ($reference !~ /$searchText$/))
				{
				$warningSuffix .= " (actual test \'$warningSearchText\')"
				}

			$warningSuffix .= " vs. $physicalReality";
			$$actionHash{constructWarning($sourceFile, $lineNumber, $item, "Incorrect case versus filesystem in", $reference, $warningSuffix)} = 1;
			}
		}

	# Special case - PRJ_EXPORTS source lines with no destination must be normalised via a new destination compliant
	# with the filename policy.  FIXSOURCE will do this, but it needs a warning to work on

	if ($examineDefaultExportDestination)
		{
		$physicalReality =~ /^.*($searchText)$/i;
		my $defaultExportReference = $1;
		
		my $exclusion = lowercaseExclusionCheck ($defaultExportReference);

		if ($defaultExportReference =~ /[A-Z]/)
			{
			$$actionHash{constructWarning ($sourceFile, $lineNumber, $item, "Incorrect case for epoc32 tree from default export in", $reference, $warningSuffix)} = 1;
			}
		}

	}
	

sub CheckSource_Includes ($\%$\@;\@;\@;$)
	{
	# References are used for array arguments only so that they can be distinguished within the subroutine
	my ($sourceFile, $actionHash, $preInclude, $macros, $userIncludesRef, $systemIncludesRef, $noUserSystemDistinction) = @_;
	my (@userIncludes, @systemIncludes);

	@userIncludes = @$userIncludesRef if ($userIncludesRef);
	@systemIncludes = @$systemIncludesRef if ($systemIncludesRef);

	my $call = "$checksourcePrefix --preprocess -- ";

	if (($sourceFile !~ /\.inf$/i) && ($sourceFile !~ /\.mmp/i))
		{
		push @$macros, "__SUPPORT_CPP_EXCEPTIONS__";
		}

	my $platformPreInclude = "";

	foreach my $macro (@$macros)
		{
		$call .= "-D$macro ";

		if ($macro =~ /__ARMCC_2_2__/)
			{
			$platformPreInclude = $ENV{EPOCROOT}."epoc32\\include\\rvct2_2\\rvct2_2.h";

			if (($sourceFile =~ /BASE\\E32\\compsupp\\/i) && $ENV{RVCT22INC})
				{					
				# Need some way to deal with ARMINC from the front-end...
				my $rvctIncDir = $ENV{RVCT22INC};
				push @systemIncludes, $rvctIncDir;
				}
			}
		elsif ($macro =~ /__GCCE__/)
			{
			$platformPreInclude = $ENV{EPOCROOT}."epoc32\\include\\GCCE\\GCCE.h";

			my $GCCEinstall = Cl_bpabi::getConfigVariable('COMPILER_INSTALL_PATH');
			
			push @systemIncludes, "\"\\\"$GCCEinstall\\..\\lib\\gcc\\arm-none-symbianelf\\3.4.3\\include\\\"\"";
			}
		}

	if ($preInclude ne "")
		{
		$call .= "-include ".getDrive().$preInclude." ";
		push @systemIncludes, &Path_Split ('Path', getDrive().$preInclude);
		}		

	if ($platformPreInclude ne "")
		{
		$call .= "-include ".getDrive().$platformPreInclude." ";
		push @systemIncludes, &Path_Split ('Path', getDrive().$platformPreInclude);
		}	

	# Enforce user and system includes in checksource processing.

	foreach my $include (@userIncludes)
		{
		$include =~ s/\\$//;
		$include = getDrive().$include if (($include !~ /^[a-zA-Z]:/) && ($include !~ /^[\"|\.]/));
		$call .= "-I $include ";
		}

	$call .= "-I- " unless $noUserSystemDistinction;

	foreach my $include (@systemIncludes)
		{
		$include =~ s/\\$//;
		$include = getDrive().$include if (($include !~ /^[a-zA-Z]:/) && ($include !~ /^[\"|\.]/));
		$call .= "-I $include ";
		}

	$sourceFile =~ s/\//\\/g;
	$sourceFile = &Path_Strip ($sourceFile);
	$sourceFile = getDrive().$sourceFile;

	$call .= $sourceFile;

	$$actionHash{$call} = 1;

	return $call;
	}


sub CheckSource_ExportedIncludes ($$\%)
	{
	my ($sourceFile, $destinationFile, $actionHash) = @_;

	# Exclude exported files as appropriate
	if ($destinationFile)
		{
		my $epoc32Include = quotemeta ($ENV{EPOCROOT})."epoc32\\\\include";
		return if ($destinationFile !~ /^$epoc32Include/i);
		return if ($destinationFile =~ /\.def$/i);
		}

	$$actionHash{"$checksourcePrefix --parsefile -- $sourceFile"} = 1;
	}


sub CheckSource_MakefileOutput(%)
	{
	my (%actionHash) = @_;

	return "\t\@rem\n" if !(keys (%actionHash));

	my $output = "";

	foreach (keys (%actionHash))
		{
		$output .= "\t$_\n";
		}

	return $output;
	}
	

sub getDrive
	{
    if(cwd =~ /^([a-zA-Z]:)/)
    	{
		return $1;
    	}

	return "";
	}


sub getPhysical ($)
	{
	my ($physicalReference) = @_;
	
	my $physicalReality = Win32::GetLongPathName($physicalReference);

	if ($physicalReality)
		{			
		$physicalReality =~ s/^.*://;
		$physicalReality = &Path_Strip ($physicalReality);		
		}

	return $physicalReality;
	}


sub constructWarning ($$$$$;$)
	{
	my ($sourceFile, $lineNumber, $item, $warningText, $reference, $suffix) = @_;

	$sourceFile =~ s/\//\\/g;
	$sourceFile = Win32::GetLongPathName($sourceFile);
	$sourceFile =~ s/^[a-zA-Z]{1}://;
	$sourceFile = &Path_Strip ($sourceFile);

	my $warning = "";
	$warning .= $sourceFile.":".$lineNumber.": ".$warningText." $item - \'".$reference."\'";
	$warning .= $suffix if ($suffix);
	$warning .= ".";		

	return $warning;
	}


sub lowercaseExclusionCheck ($)
	{
	my ($reference) = @_;

	# An exclusions file isn't mandatory
	return "UNLISTED" if (! -e $exclusionsFile);

	my $EXCLUSIONS;

	if (!(open EXCLUSIONS, "< $exclusionsFile"))
			{
			print ("ERROR: Can't open $exclusionsFile in checksource processing.\n");
			return "ERROR";
			}

	my $referenceDOSSlash = $reference;
	$referenceDOSSlash =~ s/\//\\/g;

	my $exclusionCheck = "UNLISTED";

	while (my $exclusion = <EXCLUSIONS>)
		{
		next if ($exclusion =~ /^\s*$/);

		$exclusion =~ s/^\s+//;
		$exclusion =~ s/\s+$//;
		$exclusion =~ s/\//\\/g;
		my $quotemetaExclusion = quotemeta ($exclusion);

		if ($referenceDOSSlash =~ /^$quotemetaExclusion$/i)
			{				
			if ($referenceDOSSlash !~ /^$quotemetaExclusion$/)
				{
				$exclusionCheck = $exclusion;
				}
			else
				{
				$exclusionCheck = "OK";
				}
			last;
			}
			elsif($referenceDOSSlash =~ /\\$quotemetaExclusion$/i)
			{				
			if ($referenceDOSSlash !~ /\\$quotemetaExclusion$/)
				{
				$exclusionCheck = $exclusion;
				}
			else
				{
				$exclusionCheck = "OK";
				}
			last;
			}
		}

	close EXCLUSIONS;

	return $exclusionCheck;
	}

1;
