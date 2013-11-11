#!/bin/perl -w

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
# distillsrc.pm - compiles a list of source used in .mrp files, and deletes
# any unused source
# 
#

package CDistillSrc;

use strict;
use File::Spec;
use File::Path;
use File::Basename;
use FindBin;
use lib $FindBin::Bin;
use ReadMrp;

use lib File::Spec->catdir($FindBin::Bin, '..', 'makecbr');
use CConfig;



# Constructor
#
# Parameters:
#
# $aSrcRoot : The root from which all src statements are based
# $aSrcPath : The path under aSrcRoot to the source tree to be processed
# $aSrcPrefix : An optional prefix which can be stripped from all src statements
# $aPlatform : e.g 'beech' - used to locate the platform specific product directory
#
# Returns: The object (or undef if there was a problem)
#
sub New($$$$)
	{
	my $proto = shift;
	my ($aSrcRoot, $aSrcPath, $aSrcPrefix, $aPlatform, $aCheckCase) = @_;

	my $class = ref($proto) || $proto;

	my $self = {};
	bless($self, $class);

	my $error = 0;

	if (!defined($aSrcRoot))
		{
		print "ERROR: RealTimeBuild: A srcroot must be given, to specify where all 'source' declarations originate from\n";
		$error = 1;
		}
		
	if (!defined($aSrcPath))
		{
		print "ERROR: RealTimeBuild: A srcpath must be given, to specify which source under the srcroot is to be filtered. Use '\\' to filter the entire srcroot\n";
		$error = 1;
		}

	if (!defined($aPlatform))
		{
		print "ERROR: RealTimeBuild: A platform must be given, to locate the product directory\n";
		$error = 1;
		}
		
	if ($error)
		{
		print "\n";
		}
	else
		{
		if ($aSrcPath =~ /\.\./)
			{
			print "ERROR: RealTimeBuild: The source path must be relative to the srcroot, and must not contain '..'\n";
			$error = 1;
			}
	
		$self->iSrcRoot($aSrcRoot);
		$self->iSrcPath($aSrcPath);
		$self->iSrcPrefix($aSrcPrefix);
		$self->iPlatform($aPlatform);
		$self->iSrcItems({});
		$self->iCheckCase(!!$aCheckCase);

		$self->AddSrcItem("os/buildtools/toolsandutils/productionbldtools/unref/orphan/cedprd/SuppKit", "non-shipped");
		$self->AddSrcItem("os/buildtools/toolsandutils/productionbldtools/unref/orphan/cedprd/tools", "non-shipped");
		$self->AddSrcItem("os/buildtools/toolsandutils/productionbldtools/unref/orphan/cedprd/DevKit", "non-shipped");
		$self->AddSrcItem("os/buildtools/toolsandutils/productionbldtools", "non-shipped");
		}

	if ($error)
		{
		$self = undef;
		}

	return $self;
	}

# Object data
#
sub iSrcRoot()
	{
	my $self = shift;
	if (@_) { $self->{iSRCROOT} = shift; }
	return $self->{iSRCROOT};
	}

sub iSrcPath()
	{
	my $self = shift;
	if (@_) { $self->{iSRCPATH} = shift; }
	return $self->{iSRCPATH};
	}

sub iSrcPrefix()
	{
	my $self = shift;
	if (@_) { $self->{iSRCPREFIX} = shift; }
	return $self->{iSRCPREFIX};
	}

sub iPlatform()
	{
	my $self = shift;
	if (@_) { $self->{iPLATFORM} = shift; }
	return $self->{iPLATFORM};
	}
	
sub iSrcItems()
	{
	my $self = shift;
	if (@_) { $self->{iSRCITEMS} = shift; }
	return $self->{iSRCITEMS};
	}

sub iCheckCase()
	{
	my $self = shift;
	if (@_) { $self->{iCHECKCASE} = shift; }
	return $self->{iCHECKCASE};
	}

sub iCorrectedCase()
	{
	my $self = shift;
	if (@_) { $self->{iCORRECTEDCASE} = shift; }
	return $self->{iCORRECTEDCASE};
	}

# LoadMrps - Records the source lines out of all .mrp files
#
# Parameters:
# $aConfig - optional configuration file, as used by makecbr
# $aLists - optional component lists, as used by makecbr
# $aMrps - optional .mrp files
#
# Returns: True, if the load was successful. False otherwise
#
sub LoadMrps($$$)
	{
	my $self = shift;
	my ($aConfig, $aLists, $aMrps) = @_;
	# Load in config file

	my @lists = @$aLists;
	my @mrps;
	foreach my $mrp (@$aMrps){
		{
		push @mrps, [$mrp, ''];
		}
	}
	my @configMrps = ();
    if (defined($aConfig))
		{
		my @configs = $self->_LoadConfig($aConfig);

		# Add mrps and lists (after planting them in srcroot)
		push @lists, map($self->_PlantFile($_), @{$configs[0]});
		@configMrps = map($self->_PlantFile($_), @{$configs[1]});
		foreach my $mrp (@configMrps)
			{
			push @mrps, [$mrp, ''];
			}
		}
	
	# Load in mrp lists
	foreach my $file (@lists)
		{
		if (open (MRPLIST, $file))
			{
			foreach my $line (<MRPLIST>)
				{
				chomp $line;
				$line =~ s/#.*$//; # Remove comments
				$line =~ s/^\s*//; # Remove extraneous spaces
				$line =~ s/\s*$//;
	
				if ($line ne "")
					{
					my @parms = split(/\s+/, $line);
	
					if (scalar(@parms) != 2)
						{
						warn "ERROR: RealTimeBuild: Entries in component list '$file' should be of the form 'name mrp_location'. Problem in line: $line\n";
						next;
						}
					else
						{
						# Ignore *nosource* entries
						next if ($parms[1] eq '*nosource*');
						
						push @mrps, [$self->_PlantFile($parms[1]), $parms[0]];
						}
					}
				}
			close MRPLIST or warn "ERROR: RealTimeBuild: Couldn't close '$file' : $!\n";
			}
		else
			{
			warn "Couldn't open '$file' : $!\n";	
			}
		}

	# Load all .mrp files
	if (scalar(@mrps) == 0)
		{
		die "ERROR: RealTimeBuild: No .mrp files were specified\n";
		}

	my $loaded = 1;
	
	foreach my $mrp (@mrps)
		{
		# Get path of mrp file (from here)
		my ($name, $path) = fileparse($mrp->[0]);
		# Convert to path from source root
		if (!($self->_RemoveBaseFromPath($self->iSrcRoot(), \$path)))
			{
			warn "ERROR: Mrp file $mrp->[0] isn't under the source root (".$self->iSrcRoot().")\n";
			next;
			}
		
		my $mrpobj;
        
        # To indicate the correct case and where the .mrp file comes from if failed to check letter case
        if (!($self->_CheckCase($mrp->[0]))) {
            my $mrp_error_source = "optional component list(by -f) or optional .mrp list(by -m)";
            foreach my $myName (@configMrps) {
                if ($myName eq $mrp->[0]) {
                    $mrp_error_source = "config file '".$aConfig."'";
                    last;
                }
            } 
            print "WARNING: Case of '".$mrp->[0]."' supplied in ".$mrp_error_source." does not match the file system. Should be ".$self->iCorrectedCase()."\n";
        }
        
		if (!eval { $mrpobj = New ReadMrp($mrp->[0]) })
			{
			$loaded = 0;
			my $message = $@;
			$message =~ s/^(ERROR:\s*)?/ERROR: RealTimeBuild: /i;
			print $message;
			}
		else
			{
			my $selfowned = 0;
			my $mrpComponentName = $mrpobj->GetComponent();
			if( ($mrp->[1] ne '') && (lc($mrp->[1]) ne lc($mrpComponentName)))
				{
				print "ERROR: RealTimeBuild: Component name \'$mrp->[1]\' does not match \'$mrpComponentName\' in $mrp->[0]\n";
				}
			foreach my $srcitem (@{$mrpobj->GetSrcItems()})
				{
				if ($srcitem =~ /^[\/\\]/)
					{
					# Remove source prefix
					$srcitem = $self->_StripFile($srcitem);
					}
				else
					{
					# Relative source item
					$srcitem = File::Spec->catdir($path, $srcitem);
					}

				my $rootedmrp = $path.$name;
				if ($self->_RemoveBaseFromPath($srcitem, \$rootedmrp))
					{
					$selfowned = 1;
					}

				$self->AddSrcItem($srcitem, $mrpComponentName);
				}
			if ($self->iCheckCase())
				{
				foreach my $binexpitem (@{$mrpobj->GetBinExpItems()})
					{
					# Check lower case
					if ($binexpitem =~ /[A-Z]/)
						{
						print "REMARK: [$mrpComponentName] Binary/export file $binexpitem should be lower case\n";
						}
					}
				}

			if (!$selfowned)
				{
				print "REMARK: .mrp file '$mrp->[0]' does not include itself as source\n"; 
				}
			}
		}
	return $loaded;
	}
	
# AddSrcItem - Records a source file, usually taken from an .mrp file
#
# Parameters:
# $aItem - the source file name
# $aComponent - the name of the component which claimed the file
#
# Returns: None
# Dies: Not normally; only if the source hash data structure gets corrupted
sub AddSrcItem($$)
	{
	my $self = shift;
	my ($aItem, $aComponent) = @_;

	my $item = $aItem;

	# Worth checking that the file exists
	my $truePath = File::Spec->catdir($self->iSrcRoot(), $item);
	if (($item !~ /^\\component_defs/i) && (!-e $truePath))
		{
		print "ERROR: RealTimeBuild: '$aComponent' owns $item, but that path doesn't exist\n";
		$item = ""; # No point adding this path to the tree	
		}
	else
		{
		# Check case consistency
		$self->_CheckCase($truePath) or print "WARNING: [$aComponent] Case of '".$truePath."' does not match the file system. Should be ".$self->iCorrectedCase()."\n";
		}
	
	$item =~ s/^[\/\\]*//; # Remove preceding slashes

	my @path = split(/[\/\\]+/,$item);

	my $dir = $self->iSrcItems();
	while ((scalar @path) > 0)
		{
		my $subdir = lc(shift @path);
	
		if (scalar(@path) == 0)
			{
			# Just enter the final path segment
			if (exists($dir->{$subdir}))
				{
				# Someone already owns at least part of this path
				if (!ref($dir->{$subdir}))
					{
					# Someone owns the whole of this path
					my $conflict = $dir->{$subdir};

					print "REMARK: $aComponent and $conflict both own $item\n";
					}
				else
					{
					if (ref($dir->{$subdir}) ne "HASH")
						{
						die "ERROR: Source hash is corrupted\n";
						}
					else
						{
						# Someone owns a child of this path
						my $childtree = $dir->{$subdir};

						my @conflicts = $self->_GetTreeComps($childtree);
						print "REMARK: $aComponent owns $item, which is already owned by the following component(s): ".join(", ",@conflicts)."\n";
						}
					}
				}
			$dir->{$subdir} = $aComponent;
			}
		else
			{
			# Need to enter another subdirectory
			
			if (exists($dir->{$subdir}))
				{
				if (ref($dir->{$subdir}))
					{
					# Someone already has - just do a quick integrity check
					
					if (ref($dir->{$subdir}) ne "HASH")
						{
						die "ERROR: Source hash is corrupted\n";
						}
					}
				else
					{
					# The path from this point on is already owned by a component
					my $conflict = $dir->{$subdir};
					
					print "REMARK: $aComponent and $conflict both own $item\n";
					last;
					}
				}
			else
				{
				$dir->{$subdir} = {};
				}
			}

		$dir = $dir->{$subdir};
		}
	}

# DistillSrc - Compare the recorded source lines against the source path. Delete anything which doesn't match.
#
# Parameters:
# $aDummy - A flag - non-zero means don't actually delete
#
# Returns: None
sub DistillSrc($$)
	{
	my $self = shift;
	my ($aDummy) = @_;

	my $tree = $self->iSrcItems();
	my $path = File::Spec->catdir($self->iSrcRoot(), $self->iSrcPath());

	$path=~s/[\/\\]+/\\/; # Remove multiple slashes

	# Pop the srcpath off the front of the tree
	my @path = split(/[\/\\]/,$self->iSrcPath());

	foreach my $dir (@path)
		{
		if ($dir eq ".")
			{
			next;
			}
		elsif (exists($tree->{lc($dir)}))
			{
			$tree = $tree->{lc($dir)};
		
			if (!ref($tree))
				{
				# Some component owns all of the srcpath
				last;
				}
			}
		else
			{
			# No mrp files claimed any of the source
			$tree = undef;
			last;
			}
		}

	# Now recurse into the tree and delete files
	if (defined($tree))
		{
		if (ref($tree))
			{
			$self->_DistillTree($tree, $path, $aDummy);
			}
		else
			{
			print "REMARK: All source owned by component '$tree'; no action\n";
			}
		}
	else
		{
		print "WARNING: No .mrp files claim any source; removing $path\n";
		$self->_DeletePath($path, $aDummy);
		}
	}

# Print - Display the source tree
#
# Parameters:
# $aDepth - The number of levels of the tree to show. 0 = all levels
#
# Returns: None
sub Print($$)
	{
	my $self = shift;

	my ($aDepth) = @_;

	$self->_PrintTree("", $self->iSrcItems(), $aDepth);
	}
	
# *** Private methods ***
# *** 

# _LoadConfig - (private) Reads a configuration file, as used by makecbr
#
# Parameters:
# $aConfig - filename of the configuration file
#
# Returns:
# (files, mrps) - where files and mrps are listrefs containing component lists and
# mrp files respectively
#
sub _LoadConfig($)
	{
	my $self = shift;
	my ($aConfig) = @_;
	
	my @files = ();
	my @mrps = ();
	
	my $config = New CConfig($aConfig);

	if (!defined $config)
		{
		die "Couldn't load config file '$aConfig'\n";
		}
		
	# Extract the interesting items into our lists
	push @mrps, $config->Get("gt+techview baseline mrp location");
	push @mrps, $config->Get("gt only baseline mrp location");
	push @files, $config->Get("techview component list");
	push @files, $config->Get("gt component list");
	
	# Remove any items we couldn't find
	@mrps = grep(defined($_), @mrps);
	@files = grep(defined($_), @files);
	
	return (\@files, \@mrps);
	}

# _StripFile - (private) Remover of src prefix. Also maps product directories
#
# Parameters:
# $aFile - Filename to process
#
# Returns: The processed filename
#
sub _StripFile($)
	{
	my $self = shift;
	my ($aFile) = @_;

	my $file = $aFile;

	# Map the product dirs
	my $platform = $self->iPlatform();
	$file =~ s#^[\/\\]?product[\/\\]#/sf/os/unref/orphan/cedprd/#i;

	# Remove the prefix
	my $prefix = $self->iSrcPrefix();
	
	if (defined $prefix)
		{
		my $mapped = $file; # Keep a copy in case we can't remove the prefix
		
		if (!$self->_RemoveBaseFromPath($prefix, \$file))
			{
			$file = $mapped;
			}
		}
	
	return $file;
	}
	
# _PlantFile - (private) Add src root to file. Also take off src prefix
#
# Parameters:
# $aFile - Filename to process
#
# Returns: The processed filename
#
sub _PlantFile($)
	{
	my $self = shift;
	my ($aFile) = @_;

	my $file = $aFile;

	# Remove the prefix
	$file = $self->_StripFile($file);

	# Plant the file in the src root
	$file = File::Spec->catdir($self->iSrcRoot(), $file);
	
	# Ensure all slashes are normalised to a single backslash
	$file =~ s/[\/\\]+/\\/; 
	
	return $file;
	}

# _RemoveBaseFromPath - (private) Remove a base path from the root of a filename.
#
# Parameters:
# $aBase - The base path to remove
# $$aFile - Filename to process (scalar reference)
#
# Returns: True if the file was under the base path, false otherwise
#   $$aFile may be corrupted if the return is false
sub _RemoveBaseFromPath($)
	{
	my $self = shift;
	my ($aBase, $aFile) = @_;

	my $base = $aBase;
	$base =~ s/^[\/\\]*//; # Remove extra slashes
	$base =~ s/[\/\\]*$//;

	my @base = split(/[\/\\]+/, $base);

	$$aFile =~ s/^[\/\\]*//; # Remove preceding slashes
	
	my $matched = 1;
	my $filedir;
	
	foreach my $dir (@base)
		{
		if ($$aFile =~ /[\/\\]/)
			{
			# Split off the bottom dir
			$$aFile =~ /([^\/\\]*)[\/\\]+(.*)$/;
			($filedir, $$aFile) = ($1, $2, $3);
			}
		else
			{
			# Special case - no more dirs
			$filedir = $$aFile;
			$$aFile = "";
			}
		if (lc($filedir) ne lc($dir))
			{
			# Base doesn't match
			$matched = 0;
			last;
			}
		}
	
	return $matched;
	}

# _CheckCase - (private) Given a literal filename, compares the case of the
#                        file on the filesystem against the filename i.e. it
#                        can be used to enforce case sensitivity
#
# Parameters:
# $aFilename - The literal filename
#
# Returns: True if the file matches the supplied case.
#          True if the file doesn't exist at all (user is expected to check that separately)
#          True if case checking has been disabled.
#          False otherwise (if the file exists but under a differing case).
#
# If false, the correctly cased name is present through $self->iCorrectedCase()
sub _CheckCase($)
{
	my $self = shift;
	my ($aFile) = @_;

	return 1 if !($self->iCheckCase()); # checking disabled
	return 1 if ($^O !~ /win32/i); # only works on Windows anyway
	
	return 1 if (!-e $aFile); # file not found (under case-insensitive checking)
	
	$self->iCorrectedCase(Win32::GetLongPathName($aFile));
	return ($aFile eq $self->iCorrectedCase());
}

# _DistillTree - (private) Given a src tree and a dir, clean out any unowned files
#
# Parameters:
# %$aTree - The source tree (hash ref containing nested hash refs and string leaves)
# $aDir - The directory to compare against
# $aDummy - A flag - non-zero means don't do the actual deletion
#
# Returns: A flag - non-zero if there were any owned files present
sub _DistillTree($$$)
	{
	my $self = shift;
	my ($aTree, $aDir, $aDummy) = @_;


	my $keptsome = 0;

	if (opendir(DIR, $aDir))
	{	
		my $dir = $aDir;
		$dir =~ s/[\/\\]*$//; # Remove trailing / from dir
	
		foreach my $entry (readdir(DIR))
			{
			my $path = $dir."\\".$entry;
	
			if ($entry =~ /^\.\.?$/)
				{
				next;
				}
			elsif (exists $aTree->{lc($entry)})
				{
				my $treeentry = $aTree->{lc($entry)};
				if (ref($treeentry) eq "HASH")
					{
					# Part of this path is owned
					if (-d $path)
						{
						# Recurse into path
						my $keep = $self->_DistillTree($treeentry, $path, $aDummy);
						if ($keep)
							{
							$keptsome = 1;
							}
						else
							{
							# Correction; none of this path was owned
							$self->_DeletePath($path, $aDummy);
							}
						}
					elsif (-f $path)
						{
						my @comps = $self->_GetTreeComps($treeentry);
						print "ERROR: RealTimeBuild: $path is a file, yet is used as a directory in components: ".join(", ",@comps)."\n";
						}
					else
						{
						print "ERROR: $path has disappeared while it was being examined\n";
						}
					}
				elsif (!ref($treeentry))
					{
					# This path is completely owned
					$keptsome = 1;
					next;
					}
				else
					{
					die "ERROR: Source hash is corrupted\n";
					}
				}
			else
				{
				$self->_DeletePath($path, $aDummy);
				}
			}
		
		closedir(DIR);
		}
	else
		{
			warn "ERROR: RealTimeBuild: Couldn't open directory '$aDir' for reading\n";
		}

	return $keptsome;
	}

# _GetTreeComps - (private) Get all the leaves out of a tree (or component
#                           names out of a source tree)
# Parameters:
# %$aTree - The source tree (hash ref containing nested hash refs and string leaves)
# 
# Returns: A list of strings found at the leaves (or component names)
sub _GetTreeComps($)
	{
	my $self = shift;
	my ($aTree) = @_;

	my @comps = ();

	foreach my $entry (keys(%$aTree))
		{
		if (ref($aTree->{$entry}) eq "HASH")
			{
			push @comps, $self->_GetTreeComps($aTree->{$entry});
			}
		elsif (!ref($aTree->{$entry}))
			{
			push @comps, $aTree->{$entry};
			}
		else
			{
			die "ERROR: Source hash is corrupted\n";
			}
		}
		
	return @comps;
	}

# _DeletePath - (private) Safe path deletion (file or dir)
#
# $aPath - The path to delet
# $aDummy  - A flag - non-zero means don't actually delete
#
# Returns: None. Prints warnings if deletion fails. Dies only in exceptional circumstances
sub _DeletePath($$)
	{
	my $self = shift;

	my ($aPath, $aDummy) = @_;

	if (-d $aPath)
		{
		if ($aDummy)
			{
			print "DUMMY: Directory $aPath is not specified in any .mrp file\n";
			}
		else
			{
			print "REMARK: Deleting directory $aPath; ";
			my $files = rmtree($aPath);
			if ($files)
				{
				print "$files items removed\n";
				}
			else
				{
				print "\nWARNING: Problem removing directory $aPath\n";
				}
			}
		}
	elsif (-f $aPath)
		{
		if ($aDummy)
			{
			print "DUMMY: File $aPath is not specified in any .mrp file\n";
			}
		else
			{
				unless($aPath =~ /distribution.policy.s60/i)
				{
					print "REMARK: Deleting file $aPath\n";
					unlink $aPath or print "WARNING: Problem deleting file $aPath\n";
				}
			}
		}
	else
		{
		warn "ERROR: Can't delete path $aPath; not a file or directory\n";
		}
	}

# _PrintTree - Display a subset of the source tree
#
# Parameters:
# $aPrefix - The string to prefix all paths
# $aDepth - The number of levels of the tree to show. 0 = all levels
#
# Returns: None
sub _PrintTree($$$)
        {
	my $self = shift;
	
        my ($aPrefix, $aTree, $aDepth) = @_;

	my $prefix = "";
	
	if ($aPrefix ne "")
		{
		$prefix = $aPrefix."\\";
		}

        foreach my $key (sort(keys(%$aTree)))
                {
                if (ref($aTree->{$key}))
                        {
			if ($aDepth!=1)
				{
				my $newprefix = $prefix.$key;
				
				if ($key eq "")
					{
					$newprefix.="{empty}";
					}

                        	$self->_PrintTree($newprefix, $aTree->{$key}, $aDepth-1);
				}
			else
				{
				print $prefix.$key."\\...\n";
				}
                        }
                else
                        {
                        print $prefix.$key." = ".$aTree->{$key}."\n";
                        }
                }
        }

1;
