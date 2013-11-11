#! /usr/bin/perl
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
# This script scans a directory and determines the category of all exported
# files which would be produced if all bld.inf files present were built.
# Note that EPOCROOT needs to be set for this to resolve the destination of
# the exports correctly in most cases.
# 
#

use KitStandardLocations;
my $localEpocRoot = "$OutputDir\\generic\\";
$pathToE32Variant = $localEpocRoot."epoc32\\tools";
unshift (@INC, $pathToE32Variant);            # add this to the @INC list of library paths

# set up environment for E32Variant initialisation & preserve old value
my $oldEpocRoot = $ENV{EPOCROOT};
$ENV{EPOCROOT} = $localEpocRoot;
require E32Variant;
# restore environment
$ENV{EPOCROOT} =  $oldEpocRoot;


$noSource = 0; # Set to 1 to run with only .inf files and DISTRIBUTION.POLICY files present
$noMissing = 0; # Set to 1 to ignore errors about missing source files (only valid if $noSource is 0, obviously) - to ignore broken bld.inf files

# Discriminate between '-options' and 'parameters'
my @parms = grep(!/^-/,@ARGV);
my @options = grep(/^-/,@ARGV);

$bldinf = 0;
$restrict = 0;
$cats = {};
$export = 0;
$zorcdrive = 0;
my $sourceRoot;
my $option;
my $path;
my $script;

# Parse options first
my $index=0;
while ($index < scalar(@ARGV))
	{
	$option = $ARGV[$index];
	
	if (lc($option) =~ /^-c/)
		{
		# Categories - record each category entered
		if (lc($option) eq "-c")
			{
			$index++;
			if ($index < scalar(@ARGV))
				{
				$option = $ARGV[$index];
				}
			else
				{
				die "Option '-c' has missing parameters\n";
				}

			if ($option =~ /^-/)
				{
				die "Option '-c' has missing parameters\n";
				}
			}
		else
			{
			$option =~ s/^-c//i;
			}

		foreach my $char (split(//,$option))
			{
			if ($char =~ /[ABCDEFGXIOT]/)
				{
				$cats->{lc($char)} = 1;
				}
			else
				{
				die "Category $char from '$option' is not a valid IPR category\n";
				}
			}
		}
	elsif (lc($option) eq "-bldinf")
		{
		# Report bldinfs
		$bldinf = 1;
		}
	elsif (lc($option) eq "-export")
		{
		# Include export restricted files
		$export = 1;
		}
	elsif (lc($option) eq "-restricted")
		{
		# Include only export restricted files
		$restrict = 1;
		}
	elsif (lc($option) =~ /^-src/)
		{
		# Source path
		if (lc($option) eq "-src")
			{
			$index++;
			if ($index < scalar(@ARGV))
				{
				$option = $ARGV[$index];
				}
			else
				{
				die "Option '-src' is missing its parameter\n";
				}

			if ($option =~ /^-/)
				{
				die "Option '-src' is missing its parameter\n";
				}
			}
		else
			{
			$option =~ s/^-src//i;
			}

		$sourceRoot = $option;
		}
	elsif (lc($option) =~ /^-genbuild/)
		{
		# Genbuild compatible script
		if (lc($option) eq "-genbuild")
			{
			$index++;
			if ($index < scalar(@ARGV))
				{
				$option = $ARGV[$index];
				}
			else
				{
				die "Option '-genbuild' is missing its parameter\n";
				}

			if ($option =~ /^-/)
				{
				die "Option '-genbuild' is missing its parameter\n";
				}
			}
		else
			{
			$option =~ s/^-genbuild//i;
			}

		$script = $option;
		}
	elsif (lc($option) =~ /^-/)
		{
		help();
		print "\nOption \"$option\" not recognised.\n";
		exit(0);
		}
	else
		{
		if (defined($path))
			{
			die "Only one path can be scanned at a time (not '$path' and '$option')\n";
			}
		else
			{
			$path = $option;
			}
		}
	
	$index++;
	}

if ($export && $restrict)
	{
	# -restricted overrides -export
	$export = 0;
	}

if (!defined ($sourceRoot))
	{
	help();
	print "\nexportipr.pl expects a source path to the source tree as a parameter\n";
	exit(0);
	}

if (defined($path) && defined($script))
	{
	help();
	print "\nexportipr.pl can only scan a path or parse a script, not both\n";
	exit(0);
	}

# read in macros defined in this build
my $variantMacros = "";
foreach my $macro (E32Variant::Variant_GetMacroList())
	{
	$macro =~ s/\s+$//;	# strip trailing spaces
	$variantMacros = $variantMacros . " -D" . $macro ;
	}

if (defined($script))
	{
	parse($script, $sourceRoot, $variantMacros);
	}
else
	{
	$path =~ s/[\/\\]+$//;

	scan($path, $sourceRoot, $variantMacros);
	}

exit(1);

# Recursively check a path for bld.inf files and process them.
sub scan($$$)
	{
	my ($aPath, $aRoot, $variantMacros) = @_;

	opendir(DIR, $aRoot."/".$aPath) or die "Path $aRoot/$aPath not found!\n";

	foreach my $entry (readdir(DIR))
		{
		if (($entry ne ".") and ($entry ne ".."))
			{
			if (-d $aRoot."/".$aPath."/".$entry)
				{ scan($aPath."/".$entry, $aRoot, $variantMacros); }
			elsif (lc($entry) eq "bld.inf")
				{
				getExports($aPath."/".$entry, $aRoot, $variantMacros);
				}
			}
		}

	closedir(DIR);
	}

# Scan a genbuild script file for bld.inf files and process them.
sub parse($$$)
	{
	my ($aScript, $aRoot, $variantMacros) = @_;

	open(SCRIPT, $aScript) or die "Genbuild script '$aScript' not found\n";

	foreach my $line (<SCRIPT>)
		{
		chomp($line);

		$line =~ s/#.*$//; # Remove comments
		$line =~ s/^\s+//; # Remove preceding space
		
		if ($line ne "")
			{
			if ($line =~ /^</)
				{
				# Ignore <option> or <special>
				}
			elsif ($line =~ /^\s*([\S]+)\s+([\S]+)\s*$/)
				{
				my ($component,$path) = ($1,$2);
				
				getExports($path."\\bld.inf", $aRoot, $variantMacros);
				}
			else
				{
				die "Line '$line' not recognised in script.\n";
				}
			}
		}

	close(SCRIPT);
	}

# Process a bld.inf file to get all exported files and their category
sub getExports($)
	{
	my ($aFile, $aSourceRoot, $variantMacros) = @_;

	my $sourceRoot = $aSourceRoot;
	$sourceRoot =~ s/[\/\\]+$//;

	# There are lots of references to this so put it in a variable.
	my $bifile = $ENV{TEMP}."/bld.inf";

	# Remove cpp output file in case cpp fails and an old one is left
	unlink $bifile if -e $bifile ;

	# Preprocess (via cpp), die if there are problems. System has negative
	# logic so 'and die' is correct.
	system("cpp ".$variantMacros." -undef $sourceRoot/$aFile $bifile") and die "Error: 'cpp ".$variantMacros." $sourceRoot/$aFile $bifile' failed.\n";
	# The macro 'arm' is automatically defined by gcc.  This replaces 'arm' in the bld.inf with ' 1 ', hence use of -undef when calling cpp.

	open(FILE, $bifile ) or die "Error: CPP output not accessible\n";
	
	my $exports = 0;

	# Pretend the cwd (working dir) is the location of the original bld.inf
	my $cwd = $aFile;
	$cwd =~ s/[^\/\\]+$//; # Remove any preceding slashes

	foreach my $line (<FILE>)
		{
		chomp $line;
		$line =~ s/^\s+//; # Strip spaces, before,
		$line =~ s/\s+$//; # and after.
		
		if (lc($line) eq "prj_exports")
			{
			# Start of exports declaration
			$exports = 1;
			}
		elsif (lc($line) =~ /^prj/)
			{
			# Start of any other section (end of exports decl)
			$exports = 0;
			}
		elsif ($line=~/^# (\d+) "(.*)"( \d+)?/o)
			{
			# Included bld.inf file; paths now relative to this
				{
				$cwd=$2;
				$cwd=~s-\\\\-\\-go; # Reduce escaped backslashes
				$cwd=~s/[^\/\\]+$//; # Strip preceding slashes
				}
			}
		elsif ($exports)
			{
			# Line is in PRJ_EXPORTS declaration
			if ($line ne "")
				{
				my ($source, $dest) = split_parms($line);

				# Parse destination path
				if (($dest =~ /[\/\\]$/) || (!defined($dest)))
					{
					# Filename omitted
					my $file = $source; # Take source filename
					$file =~ s/^.*[\/\\]//; # Strip path
					$dest = $dest.$file; # Put in epoc32\include
					}
				
				if ($dest =~ /^[ZC]:[\/\\]/i)
					{
					# This export is using the emulated C: or Z: drive,
					# replace [C/Z]: by \epoc32\data\[C/Z]
					$dest =~ s/^([ZC]):/\\epoc32\\data\\$1/i;
					$zorcdrive = 1;
					}
				elsif ($dest =~ /^[^\/\\]/)
					{
					# if target starts by +\ or +/ replace + by \epoc32
					if ($dest =~ /^\+[\/\\]/)
					{
						$dest =~ s/^\+/\\epoc32/;
					}
					else 
					{
					# Path relative (to epoc32\include)
					$dest = "\\epoc32\\include\\$dest";
					}
					}
				if ($dest =~ /^[\/\\]epoc32/)
					{
					# Destination under epoc32
					$dest =~ s/^[\/\\]//;
					$dest = $ENV{EPOCROOT}.$dest; # Put under EPOCROOT
					}

				# Parse source path
				if ($source =~ /^[\/\\]/)
					{
					# Source relative to source root
					$source =~ s/^[\/\\]+//;
					$source = $sourceRoot."\\".$source;
					}
				else
					{
					# Truly relative source path
					$source = $cwd.$source;
					}
				
				if (-e $source || $noSource)
					{
					# Get source category
					my ($cat, $restricted) = getExportCategory($source);
					
					if ( $export # Include irrespective of export restrictions
					  || ($restrict && $restricted) # Include only if restricted
					  || ((!$restrict) && (!$restricted)) # Include only if not restricted
					  )
						{
						if ((scalar(keys(%$cats)) == 0) || ($cats->{lc($cat)}))
							{
							if( $export && $zorcdrive && ($cat =~ /^[ABCI]$/))
								{
								print STDERR "WARNING: cat $cat resource file not categorised : $dest \n";
								}
							if (scalar(keys(%$cats)) == 0)
								{
								# No category specified - display category
								print STDOUT $cat." ";
								if ($export && $restricted)
									{
									print STDOUT "(export restricted) ";
									}
								}

							print STDOUT "$dest";

							if ($bldinf)
								{
								# Display bld.inf file
								print STDOUT " ".$aFile;
								}

							print STDOUT "\n";
							}
						}
					}
				elsif (!$noMissing)
					{
					print "Error: $aFile: Export file '$source' not found\n";
					}
				}
			}
			$zorcdrive = 0;
		}
	close(FILE);
	# Remove cpp output file so nothing else can try to use it.
	unlink $bifile or die "Error: Could not delete $bifile.\n";
	}

# Identify IPR category for a given file
sub getExportCategory($)
	{
	my ($aSource) = @_;

	my $sdir = $aSource;
	$sdir =~ s/[\/\\][^\/\\]+$//;
	my $cat = "X"; # Default if no DISTRIBUTION.POLICY
	my $restricted = 0;
	
	if (open(DISTPOL,"$sdir/DISTRIBUTION.POLICY"))
		{
		foreach my $line (<DISTPOL>)
			{
			chomp($line);
			if ($line =~ /^\s*CATEGORY\s+(\w)\s*$/i)
				{
				$cat = $1;
				}
			elsif ($line =~ /^\s*export\s+restricted\s*$/i)
				{
				$restricted = 1;
				}
			}
		close(DISTPOL);
		}
	return ($cat, $restricted);
	}

# Takes a space separated list of parameters (spaces may be double quoted) and
# returns a perl list.
sub split_parms($)
	{
	my ($aLine) = @_;
	
	my $line = $aLine;
	my @parms;
	my $parm;

	while ($line !~ /^\s*$/)
		{
		#Remove preceding spaces
		$line =~ s/^\s+//;

		if ($line =~ /^"/)
			{
			# Split on quote
			if ($line =~ m/^"([^"]*)"(.*)$/)
				{
				push @parms, $1;
				$line = $2;
				}
			}
		else
			{
			# Split on space
			if ($line =~ m/^([^\s]*)\s+(.*)$/)
				{
				push @parms, $1;
				$line = $2;
				}
			else
				{
				# No spaces present
				push @parms, $line;
				$line = "";
				}
			}
		}

	return @parms;
	}

sub help()
	{
	print "Syntax: [perl ]exportipr.pl -src sourcepath [-bldinf] [-c...] [path | -genbuild filename]\n";
	print "\n  sourcepath is a path to a source tree\n";
	print "\n  EPOCROOT must be set for the destination of the exports to be\n";
	print "  reported accurately.\n";
	print "\n  Options:\n";
	print "   path : is a path (within the source tree) used if the whole source tree\n";
	print "   is not to be scanned\n";
	print "  -genbuild filename : points to a genbuild compatible file, used if the whole";
	print "   source tree is not to be scanned\n";
	print "  -bldinf : reports the bld.inf file which exports each file\n";
	print "  -c... : restricts the output to specific categories (e.g. -cFG)\n";
	print "  -export : includes files irrespective of export restrictions\n";
	print "  -restricted : only includes files with export restrictions\n";
	print "By default export restricted files are not reported\n";
	print "\nWith no -c option the category of each file is reported\n";
	print "With the -export option additionally applied, export restricted files are indicated\n";
	print "If a -c option is applied and -bldinf is not, a plain list of\n";
	print "exports will be produced.\n";
	}
