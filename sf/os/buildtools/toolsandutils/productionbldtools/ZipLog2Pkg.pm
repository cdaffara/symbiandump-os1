#!perl

# ziplog2pkg.pm

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
#

use strict;


package ZipLog2Pkg;

use File::Path;
use CProgressLog;
use KitStandardLocations;

my $log;
my %stdVars = ();


# --------------------------------- Start of ZipLog2Pkg() ----------------------------
sub ZipLog2Pkg($$)
{
	my $cmdfile = shift;

	my $parameters = shift;
	my @options = grep(/^-/,$parameters);

	my $verbose = 1;
	foreach my $option (@options)
		{
		if ($option eq "-v")
			{
			$verbose = 2;
			}
		elsif (($option eq "-batch") || ($option eq "-b"))
			{
			$verbose = 0;
			}
		else
			{
			print "WARNING: Option '$option' not understood; ignoring\n";
			}
		}

	# set up hash containing variables defined in KitStandardLocations.pm
	foreach my $var (@KitStandardLocations::EXPORT)
		{
		$var =~ s/\$//;
		eval{
			no strict 'refs';
			$stdVars{"%".$var."%"} = "$$var";
			};
		} 

	my $PlatformProductToolsDir = $FindBin::Bin;
	$PlatformProductToolsDir =~ s/common/$KitStandardLocations::Platform/i;    # change "common" to platform name in path to tools directory
	$stdVars{"%baseline%"} = "$PlatformProductToolsDir\\baseline";

	$log = New CProgressLog($verbose);
	open (CMDFILE, "<$cmdfile") or print "Unable to open $cmdfile - $!\n" ;

	my $packages = {};
	my @files;
	my @usedFiles;
	my $version;
	my @global_map;

	$global_map[0] = "";				# global map from
	$global_map[1] = "[emul]\\";		# global map to


	foreach my $line (<CMDFILE>)
		{
		chomp($line);
		
		if ($line =~ /^#/)
			{
			print "\n$line\n" # Printable comment
			}
		elsif (($line =~ /^\/\//) || ($line =~ /^\s*$/))
			{
			# Nonprintable comment or blank line
			}
		else
			{
			# replace standard variables in @params
			if ($line =~ /%/)
			{
				$line = replaceStdVars ($line)
			}		
			
			my @parms = split(" ", $line);
			# Recombine quoted entries
			for (my $index=0; $index < scalar(@parms); $index++)
				{
				if ($parms[$index] =~ /^\"/)
					{
					# If double quoted
					my $entry = $parms[$index];

					while ( ($entry !~ /\"$/) && ( ($index+1) < scalar(@parms) ) )
						{
						# If end quote not found, merge following entry 
						$entry = $entry." ".$parms[$index+1];
						
						splice @parms, $index+1, 1;
						}

					if ($entry !~ /\"$/)
						{
						if ($entry =~ /^\"[^\"]*$/)
							{
							die "'$line' invalid: Opening quote must have corresponding close quote\n";
							}
						else
							{
							die "'$line' invalid: Close quote must come at end of parameter\n";
							}
						}
					
					# Store combined quoted entries
					$entry =~ s/^\"//;
					$entry =~ s/\"$//;
					splice @parms, $index, 1, $entry;
					}
				elsif ($parms[$index] =~ /^'/)
					{
					# If single quoted
					my $entry = $parms[$index];

					while ( ($entry !~ /'$/) && ( ($index+1) < scalar(@parms) ) )
						{
						# If end quote not found, merge following entry 
						$entry = $entry." ".$parms[$index+1];
						
						splice @parms, $index+1, 1;
						}

					if ($entry !~ /'$/)
						{
						if ($entry =~ /^'[^']*$/)
							{
							die "'$line' invalid: Opening quote must have corresponding close quote\n";
							}
						else
							{
							die "'$line' invalid: Close quote must come at end of parameter\n";
							}
						}
					
					# Store combined quoted entries
					$entry =~ s/^'//;
					$entry =~ s/'$//;
					splice @parms, $index, 1, $entry;
					}
				}
				
				
			my $command = shift @parms;
			$command = lc($command);

			if (($command eq "load") or ($command eq "add"))
				{
				if ($command eq "load")
					{
					# Don't append this; clear first
					if (scalar(@files)>0)
						{
						$log->Warn("WARNING: ".scalar(@files)." files discarded.");
						}
					@files = ();
					}
					
				if (scalar(@parms) != 1)
					{
					die "'$line' invalid:\nExpected only one parameter (ziplog filename)\n";
					}
				
				my @new;
				
				if ($parms[0] =~ /^#/)
					{
					my $file = $parms[0];
					$file =~ s/^#//;
					@new = @{readFileList($file)};
					}
				else
					{
					@new = @{readZiplog($parms[0])};
					}

				$log->Progress("");
					
				my $orig = scalar(@files);
				@files = (@files, @new);

				# Clear any additional duplicated items
				@files = @{removeDuplicates(\@files, 1)};

				# Don't load in any lines that were previously loaded
				# (add in @usedFiles twice, remove all duplicates)
				@files = (@usedFiles, @usedFiles, @files);
				@files = @{removeDuplicates(\@files, 0)}; # Removes all
				        # of the @used files added, plus any of those which
						# matched the original @files too
						
				$log->Progress("Loaded $parms[0]. ".scalar(@new)." files (".(scalar(@new)+$orig-scalar(@files))." duplicates)");
				}
			elsif ($command eq "patch")
				{
				if (scalar(@parms) != 1)
					{
					die "'$line' invalid:\nExpected only one parameter (file list filename)\n";
					}

				my $file = $parms[0];
				if ($file =~ /^#/)
					{
					$file=~s/^#//;
					}
				else
					{
					$log->Error("ERROR: Patch file should be prefixed with a '#' to specify filelist.\nAttempting default of filelist.");
					}

				# Read file list in
				my @filelist = @{readFileList($file)};

				# Add them back in to files to match
				my $orig = scalar(@files);
				@files = (@files, @filelist);
				@files = @{removeDuplicates(\@files, 1)};

				# Take them out of files already matched
				# (so a load containing one of them won't eliminate it)
				@usedFiles = (@usedFiles, @filelist, @filelist);
				@usedFiles = @{removeDuplicates(\@usedFiles, 0)};

				$log->Progress("Loaded $parms[0]. ".scalar(@filelist)." files (".(scalar(@filelist)+$orig-scalar(@files))." duplicates)");
				}
			elsif ($command eq "create")
				{
				# Create package list
				if (scalar(@parms) < 2)
					{
					die "'$line' invalid:\nExpected at least 2 parameters (package name, device name, [dependencies ...])\n";
					}

				# Parms: packagePath, device, dependencies...
				my $packagePath = shift @parms;
				my $device = shift @parms;

				my $packageName = $packagePath;
				$packageName =~ s/^.*[\/\\]//;
				$packagePath =~ s/[\/\\][^\/\\]+$/\\/ or $packagePath = "";

				if ($packageName eq "null")
					{
					die "'$line' invalid:\nCannot create package named 'null'. This name is reserved for files not to be written\n";
					}
				
				$packages->{$packageName} = [$packagePath, $device, [], \@parms, [], [], \@global_map, "", "", "", []];
				}
			elsif ($command eq "path")
				{
				# Parms: packageName, path to add
				if (scalar(@parms) != 2)
					{
					die "'$line' invalid:\nExpected 2 parameters (package name, path to add)\n";
					}
				my $packageName = shift @parms;
				my $path = shift @parms;

				my ($packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$packageName}};
				push @$paths, $path;

				$packages->{$packageName} = [$packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags];
				}
			elsif ($command eq "menu")
				{
				# Parms: package name, menu entry, description, working path, file path [arguments, icon path]
				if (scalar(@parms) < 5)
					{
					die "'$line' invalid:\nExpected at least 5 parameters (package name, menu entry, description, working path, file path, arguments, icon path)\n";
					}
				my ($packageName,$entry,$desc,$path,$file,$args,$icon) = @parms;

				my ($packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$packageName}};
				push @$menu, [$entry, $desc, $path, $file, $args, $icon];

				$packages->{$packageName} = [$packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags];
				}
			elsif ($command eq "map_global")
				{
				# Parms: global map from, global map to
				if (scalar(@parms) != 2)
					{
					die "'$line' invalid:\nExpected 2 parameters (global map from, global map to)\n";
					}
				@global_map = @parms;
				}
			elsif ($command eq "map")
				{
				# Parms: packageName, map from, map to
				if (scalar(@parms) != 3)
					{
					die "'$line' invalid:\nExpected 3 parameters (package name, map from, map to)\n";
					}
				my ($packageName, $from, $to) = @parms;

				my ($packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$packageName}};
				$packages->{$packageName} = [$packagePath, $device, $files, $dependencies, $paths, $menu, [$from, $to], $category, $description, $licence, $tags];
				}
			elsif ($command eq "category")
				{	
				# Parms: packageName, category
				if (scalar(@parms) < 2)
					{
					die "'$line' invalid:\nExpected 2 parameters (package name, category)\n";
					}
				my $packageName = shift @parms;
				my ($packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$packageName}};
				my $category = shift @parms;
				$packages->{$packageName} = [$packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags];
				}
			elsif ($command eq "description")
				{
				# Parms: packageName, description
				if (scalar(@parms) < 2)
					{
					die "'$line' invalid:\nExpected 2 parameters (package name, description)\n";
					}
				my $packageName = shift @parms;
				my ($packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$packageName}};
				my $description = shift @parms;
				$packages->{$packageName} = [$packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags];
				}
			elsif ($command eq "licence")
				{
				# Parms: packageName, licence file
				if (scalar(@parms) != 2)
					{
					die "'$line' invalid:\nExpected 2 parameters (package name, licence file)\n";
					}
				my $packageName = shift @parms;
				my ($packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$packageName}};
				my $licence = shift @parms;
				$packages->{$packageName} = [$packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags];
				}
			elsif ($command eq "tag")
				{
				# Parms: packageName, tag name, tag value
				if (scalar(@parms) != 3)
					{
					die "'$line' invalid:\nExpected 3 parameters (package name, tag name, tag value)\n";
					}
				my ($packageName, $tagName, $tagValue) = @parms;

				my ($packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$packageName}};
				$packages->{$packageName} = [$packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, [$tagName, $tagValue]];
				}
			elsif ($command eq "test")
				{
				# Warn if any files match
				if (scalar(@parms) < 1)
					{
					die "'$line' invalid:\nExpected >1 parameter (expressions to match)\n";
					}

				my @files = @{ match(\@files, \@parms, [0], $cmdfile) };

				foreach my $file (@files)
					{
					$log->Warn("WARNING: '$file' matched tests '".join(", ",@parms)."'");
					}
				}
			elsif ($command eq "write")
				{
				# Write out to a package list
				my @options = grep(/^-/,@parms);
				my @parms = grep(!/^-/,@parms);

				if (scalar(@parms) < 2)
					{
					die "'$line' invalid:\nExpected >2 parameters (expressions..., package name)\n";
					}

				my $warn = 1;
				foreach my $option (@options)
					{
					if ($option eq "-nowarn")
						{
						$warn = 0;
						}
					else
						{
						die "'$line' invalid:\nOption $option not understood\n";
						}
					}
		
				my $packageName = pop @parms;

				# Perform the matching
				my @package = @{ match(\@files,\@parms, [$warn], $cmdfile) };

				# Get list of the files that are still to be matched
				@files = (@files, @package, @package);
				@files = @{removeDuplicates(\@files,0)};
				
				# Record the matched files as used
				@usedFiles = (@usedFiles, @package);

				$log->Progress("Matched ".scalar(@package)." files for $packageName");

				if (defined($packages->{$packageName}))
					{	
					my ($packagePath, $device, $merge, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$packageName}};

					$merge = [@package, @$merge];
					$packages->{$packageName} = [$packagePath, $device, $merge, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags];
					}
				else
					{
					if ($packageName ne "null")
						{
						die "\nIn line '$line':\nAttempt to add to non-existent package '$packageName'.\nUse 'create $packageName [device name]' to create the package definition first\n";
						}
					}
				}
			elsif ($command eq "version")
				{
				if (defined($version))
					{
					die "\nIn line '$line':\nVersion has already been defined. One script can only have one version\n";
					}
				else
					{
					if (scalar(@parms) > 1)
						{
						die "\nIn line '$line':\nExpected only one parameter (spaces must be quoted)\n";
						}
					else
						{
						$version = shift @parms;
						}
					}
				}
			else
				{
				$log->Error("ERROR: Command not understood; ignoring: $line");
				}
			}
		}
	close(CMDFILE);

	# Write out $packages
	my $device;
	my $files;
	my $dependencies;
	my $paths;
	my $menu;
	my $map;
	my $category;
	my $description;
	my $licence;
	my $tags;
	my $packagePath;
	foreach my $pkgName (keys(%$packages))
		{
		($packagePath, $device, $files, $dependencies, $paths, $menu, $map, $category, $description, $licence, $tags) = @{$packages->{$pkgName}};
		writePkg($packagePath, $pkgName, $files, $device, $dependencies, $paths, $menu, $map, $version, $category, $description, $licence, $tags);
		}
}

sub readZiplog($)
	{
	my ($zipLog)=@_;
	my @package;

	# read in file
	open (FILE, "<$zipLog") or die "Couldn't open file $zipLog";

	# parse for elements we want
	foreach my $line (<FILE>)
		{
		$line =~ s/^\s+// ;

		if ($line =~ m/^adding: (.*)$/ )
			{
			# found line with file on - need to parse
			my $fileline = $1;

			# remove (deflated... or (stored... or ( anything   bit at end

			$fileline=~s/ \(.*\)$//;

			$fileline =~ s/\s+$// ;

			# ignore lines ending in slash - probably a directory

			if ($fileline =~ /\/$/ )
				{
				print "Directory?? - $fileline\n" ;
				}
			else
				{
				$fileline =~ s,/,\\,g;
				$fileline =~ s/^[\/\\]//;
				push (@package, $fileline);
				}
			}
		}

	close(FILE);

	return \@package;
	}

sub readFileList($)
	{
	my ($file) = @_;

	my @filelist;
		
	# Read file list in
	open (FILELIST, $file) or die "File '$file' not found\n";
	while (my $filename=<FILELIST>)
		{
		chomp($filename);
		$filename =~ s,/,\\,g ;
		$filename =~ s/^[\/\\]//;  # remove initial slash
		push @filelist, $filename;
		}
	
	close(FILELIST);
	
	return \@filelist;
	}

sub writePkg($$$$$$$$$$$$)
	{
	my ($pkgPath, $pkgName, $fileList, $device, $dependencies, $paths, $menu, $map, $version, $category, $description, $licence, $tags) = @_;
	my $from;
	my $to;

	if ($map != 0)
		{
		($from, $to) = @$map;
		}
	if ($pkgPath ne "")
		{
		mkpath($pkgPath);

		if (-e $pkgPath)
			{
			if (!(-d $pkgPath))
				{
				die "Couldn't open output dir $pkgPath (file of the same name already exists)\n";
				}
			}
		else
			{
			die "Couldn't open output dir $pkgPath\n";
			}
		}
	
	open (FILE, ">$pkgPath$pkgName.pkgdef") or die "Couldn't open output file $pkgPath$pkgName.pkgdef";
	print (FILE "<?xml version=\"1.0\" encoding=\"ISO8859-1\"?>\n");
	print (FILE "\n");

	print (FILE "<packagedef version=\"1.0\">\n");
	print (FILE "  <package name=\"$pkgName\" major-version=\"0\" minor-version=\"0\">\n");
	print (FILE "    <supplier>Symbian Ltd</supplier>\n");
	print (FILE "    <sdk-version>$version</sdk-version>\n");
	print (FILE "    <category>$category</category>\n") if ($category ne "");
	print (FILE "    <description>$description</description>\n\n") if ($description ne "");
	
	print (FILE "    <licensing-agreement xml:lang=\"en_US\" document-root=\"".$licence."\"  mime-type=\"text\/plain\"/>\n") if ($licence ne "");
	print (FILE "    <attributes>\n      <device name=\"".$device."\"/>\n    </attributes>\n") if ($device ne "device");
		
	if (scalar(@$tags) > 0)
	{
		print (FILE "    <install-path-tags>\n");
		(my $tagName, my $tagValue) = @$tags;
		print (FILE "      <tag tag=\"$tagName\">$tagValue</tag>\n");
		print (FILE "    </install-path-tags>\n");
	}
	if (scalar(@$dependencies) > 0)
		{
		print (FILE "    <dependencies>\n");
		foreach my $dependency (@$dependencies)
			{
			print (FILE "      <dependency name=\"$dependency\" major-version=\"0\" minor-version=\"0\" build-number=\"0\"/>\n");
			}
		print (FILE "    </dependencies>\n");
		}
	if (scalar(@$paths) > 0)
		{
		print (FILE "    <environment-changes>\n");
		foreach my $path (@$paths)
			{
			print (FILE "      <add-to-variable name=\"Path\" value=\"$path\" how=\"append\" separator=\";\"/>\n");
			}
		print (FILE "    </environment-changes>\n");
		}
	if (scalar(@$menu) > 0)
		{
		print (FILE "    <shortcuts>\n");
		foreach my $entry (@$menu)
			{
			my ($name, $description, $workingDirectory, $filePath, $arguments, $iconPath) = @$entry;
			
			print (FILE "      <shortcut shortcut-path=\"Symbian\" file-path=\"".$filePath."\"");
			print (FILE " description=\"".$description."\"") unless ($description eq "");
			print (FILE " arguments=\'".$arguments."\'") unless ($arguments eq "");
			print (FILE " working-directory=\"".$workingDirectory."\"") unless ($workingDirectory eq "");
			print (FILE ">\n");
			print (FILE "        <shortcut-name xml:lang=\"en-US\" name=\"".$name."\"/>\n");
			print (FILE "        <shortcut-icon path=\"".$iconPath."\"/>\n") unless ($iconPath eq "");
			print (FILE "      </shortcut>\n");
			}
		print (FILE "    </shortcuts>\n");
		}
	print (FILE "  </package>\n");
	print (FILE "\n");
	print (FILE "  <manifest>\n");

	$fileList=[sort(@$fileList)];
	$fileList=removeDuplicates($fileList,1); # Remove additional duplicates (may happen with 'patch'ed files)

	foreach my $line (@$fileList)
		{
		print FILE "    <item src=\"\\$line\"";
		$line =~ s/^$from(.*)$/$1/i;
		print FILE " dest=\"$to$line\"/>\n";
		} 
	print FILE "  <\/manifest>\n</packagedef>\n";
	close(FILE);
	}

sub removeDuplicates($$)
	{
	# NB: Case insensitive matching
	my ($aListRef, $aLeave) = @_;
	my @list = @$aListRef;

	if ($aLeave>1)
		{
		die "removeDuplicates(\$\$): Cannot leave more than 1 duplicate\n";
		}

	my $prevEntry = undef;
	my $entry;

	my $index=0;

	@list=sort(@list);

	while ($index < scalar(@list))
		{
		$entry = lc($list[$index]);

		if ($entry eq $prevEntry)
			{
			if ($aLeave == 0)
				{
				# Get rid of the prevEntry as well if we need
				splice(@list, $index-1, 1);
				$index--;
				}

			while ($entry eq $prevEntry)
				{
				# If there's a duplicate, remove the duplicate
				splice(@list, $index, 1);
				# then get the next entry to check	
				$entry = lc($list[$index]);
				}
			}

		$prevEntry = $entry;

		$index++;
		}
	
	return \@list;
	}

# Keep only duplicated entries - and then only keep one of them
sub keepDuplicates($)
	{
	my ($listRef) = @_;
	
	my @list = sort(@$listRef);
	
	my $index=0;

	while ($index < scalar(@list))
		{
		if (($index<(scalar(@list)-1)) && (lc($list[$index]) eq lc($list[$index+1])))
			{
			# Remove all other duplicates except the last one
			while ((lc($list[$index]) eq lc($list[$index+1])) && $index < (scalar(@list)-1))
				{
				splice(@list, $index, 1);
				}
			$index++;
			}
		else
			{
			# Not a duplicate; remove it
			splice(@list, $index, 1);
			}
		}
	
	return \@list;
	}

sub match($$$$)
	{
	my @package = @{(shift)}; # Files available for matching
	my @patterns = @{(shift)}; # List of patterns to match
	my @options = @{(shift)}; # Options ($warn)
	my $filename = shift; # This is only used for error messages

	my ($warn) = @options;

	my @regexp = grep(/^\/.*\/$/, @patterns);
	my @leftpatterns = grep(!/^\/.*\/$/, @patterns);
	my @filelist = grep(/^#/, @leftpatterns);
	@leftpatterns = grep(!/^#/, @leftpatterns);

	if (scalar(@leftpatterns) > 0)
		{
		foreach my $expression (@leftpatterns)
			{
			$log->Error("ERROR: Expression '$expression' not understood (must be /regexp/ or #filename)");
			}
		exit($log->getErrorCode());
		}

	if (scalar(@filelist)>0)
		{
		# Prepare list of files to match
		my $file = shift @filelist;
		$file =~ s/^#//;
		my @tomatch = @{readFileList($file) };

		@tomatch = @{removeDuplicates(\@tomatch, 1)}; # Remove extra duplicates

		foreach my $file (@filelist)
			{
			$file =~ s/^#//;
			my @list = @{readFileList($file)};

			@list = @{removeDuplicates(\@list, 1)}; # Remove extra duplicates
			
			@tomatch = @tomatch, @list;
			@tomatch = @{keepDuplicates(\@tomatch)}; # Keep only matching files
			}

		foreach my $regexp (@regexp)
			{
			$regexp =~ s/^\/(.*)\/$/\1/;
			@tomatch = grep(/$regexp/i, @tomatch); # Filter list of files to match
			}

		# Match files
		my @match = ();
		my @nomatch = ();
		my $lcentry;
		
		foreach my $entry (@tomatch)
			{
			$lcentry = lc($entry);
			my @matches = grep((lc($_) eq $lcentry), @package);

			if (scalar(@matches) == 0)
				{
				push @nomatch, $entry;
				}
			elsif (scalar(@matches) == 1)
				{
				push @match, $matches[0];
				}
			else
				{
				die "Fatal error: Failure to clear duplicates\n";
				}
			}
		@package = @match;
				
		if ($warn)
			{
			if (scalar(@nomatch)>0)
				{
				my $object;

				if (scalar(@patterns) == 1)
					{
					$object="file";
					}
				else
					{
					$object="rule";
					}
				
				$log->Warn("WARNING: ".scalar(@nomatch)." files not matched from $object '".join(" ",@patterns)."'.");
				
				$log->ListMissing(@nomatch);
				}
			}
			
		if ((scalar(@package) == 0) && (scalar(@tomatch) > 0))
			{
			if ($warn)
				{
				$log->Warn("WARNING: 0 files matched against rule '".join(" ",@patterns)."' [$filename:$.]");
				}
			}
		}
	else
		{
		# Regexps only
		foreach my $regexp (@regexp)
			{
			$regexp =~ s/^\/(.*)\/$/\1/;
			@package = grep(/$regexp/i, @package );
	
			if (scalar(@package) == 0)
				{
				last;
				}
			}
		if (scalar(@package) == 0)
			{
			if ($warn)
				{
				$log->Warn("WARNING: 0 files matched against rule '".join(" ",@patterns)."' [$filename:$.]");
				}
			}
		}

	return \@package;
	}				


sub replaceStdVars($)
	{
	my $line = $_[0];
	$line =~ s/(%[^%]*?%)/$stdVars{$1}/gi;
	return $line;
	}
		


1;
