# Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Wrapper to support the EPOC Resource Compiler
# 
#

use warnings;
use strict;
use Cwd;		# for cwd
use FindBin;		# for FindBin::Bin
use File::Copy;		# for copy()

my $curdrive="x";	    	# will be initialised when first needed
my $PerlBinPath;	# fully qualified pathname of the directory containing this script

# establish the path to the Perl binaries
BEGIN {
	require 5.005_03;		# check user has a version of perl that will cope
	$PerlBinPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlBinPath =~ s/\//\\/g;	# X:\epoc32\tools
}
use lib $PerlBinPath;
use lockit_info;
use E32Variant;         # for variant specific macros
use Pathutl;
use Preprocessor;

sub print_usage
	{
#........1.........2.........3.........4.........5.........6.........7.....
	print <<USAGE_EOF;

Usage:
  epocrc [options] srcfile [-ooutputfile] [-hheaderfile] [-l "TargetPath:CWDir"]  

Compile an EPOC resource file, optionally generating a compiled resource 
file and an associated header file.

The available options are

   -Dxxx, -Ixxx      -- C++ preprocessor arguments
   -preincludeFILE   -- optional FILE to be used as a preinclude file in all preprocessing
   -u                -- compile for use with Unicode EPOC
   -ttmpdir          -- specify a directory for temporary files
   -nocpp            -- do not call C++ preprocessor
   -l                -- if specified, capture all source to \\epoc32\\localisation\\... 
   -uid2 N           -- specifies 2nd UID for output file
   -uid3 N           -- specifies 3rd UID for output file 
   -m[nnn](,[nnn])*  -- suppress warning nnn in rcomp, can supply multiple 
                        comma-separated three digit values
   -v                -- verbose output
   -vv               -- more verbose, -v passed to rcomp			

See the file epocrc.config for futher usage options.
   
The resource file is first passed through the C++ preprocessor, using any 
specified preprocessor arguments, and then compiled with RCOMP.EXE to 
generate a compiled resource and the associated header file. 

All intermediate files are generated into a temporary directory.

Specifying either -uid2 or -uid3 overrides all uids specified in the source file.

If -preinclude FILE is not specified, the preinclude file listed in
%EPOCROOT%epoc32\\tools\\variant\\variant.cfg is used in all preprocessing.

USAGE_EOF
	}

#-------------------------------------------------------
# Process commandline arguments
#
# Can't use the Getopt package because it doesn't like the -D and -I style options
#
my $opt_uid2='';
my $opt_uid3='';
my $sourcefile="";
my $opt_o="";
my $opt_m="";
my $opt_h="";	
my $opt_l="";
my $tmpdir="";
my $unicode=0;
my $opt_v=0;
my $opt_vmore=0;
my $opt_cpp = 1;
my $check_rls_items = 0;
my $strict_checking = 0;
my $test_mode = 0;
my $warnings_to_enable = 0;

my $variantMacroHRHFile = "";
my $exe = &PreprocessorToUseExe();
my $cpp_spec= "$exe -nostdinc -undef -C ";	    # preserve comments

my $errors = 0;
while (@ARGV)
	{
	my $arg = shift @ARGV;
	if ($arg =~ /^-D(.*)$/)
		{
		if ($1 eq "")
		    {
		    $arg = shift @ARGV;
		    $cpp_spec .= "-D \"$arg\" ";
		    }
		else
		    {
		    $cpp_spec .= "$arg ";
		    }
		next;
		}
	if ($arg =~ /^-I-$/)
		{
		$cpp_spec .= "-I- ";
		next;
		}
	if ($arg =~ /^-I(.*)$/)
		{
		$cpp_spec .= "-I ";
		if ($1 eq "")
		    {
		    $arg = shift @ARGV;
		    }
		else
		    {
		    $arg = $1;
		    }
		$cpp_spec .= quoted_path($arg)." ";
		next;
		}
	if ($arg =~ /^-preinclude(.*)$/)
		{
		$variantMacroHRHFile = $1;
		next;
		}
	if ($arg =~ /^-v$/)
		{
		$opt_v =1;
		next;
		}
	if ($arg =~ /^-vv$/)
		{
		$opt_vmore =1;
		$opt_v =1;
		next;
		}
	if ($arg =~ /^-nocpp$/)
		{
		$opt_cpp =0;
		next;
		}

	if ($arg =~ /^-uid2$/)
		{
		$opt_uid2 = shift @ARGV;
		next;
		}
	
	if ($arg =~ /^-uid3$/)
		{
		$opt_uid3 = shift @ARGV;
		next;
		}
	
	if ($arg =~ /^-u$/)
		{
		$unicode =1;
		next;
		}
	if ($arg =~ /^-o(.*)$/)
		{
		$opt_o =$1;
		next;
		}
	if ($arg =~ /^-h(.*)$/)
		{
		$opt_h =$1;
		next;
		}
	if ($arg =~ /^-t(.*)\\?$/)
		{
		$tmpdir ="$1\\";
		next;
		}
	if ($arg =~ /^-l(.*)$/)
		{
		$opt_l =$1;
		next;
		}
	if($arg =~ /^(-m.*)$/)
		{
		$opt_m =$1;
		next;
		}
	if ($arg =~ /^-epocrc-test$/)
		{
		$test_mode = 1;
		next;
		}
	if ($arg =~ /^-/)
		{
		print "Unknown arg: $arg\n";
		$errors++;
		next;
		}

	if (($opt_uid3 ne "*") && ($opt_uid2 eq "0"))
		{
    print "\n Error: uid3 specified without uid2 \n";
	  $errors++;
	  exit;
	  }
	     
	$sourcefile=$arg;
	}

if ($errors || $sourcefile eq "")
	{
	print_usage();
	exit 1;
	}

my @enabled_warnings = ();
my $file_line;
my @includes_from_config;
my $epocrc_config = $ENV{EPOCROOT}. "epoc32\\tools\\epocrc.config";

if(-e $epocrc_config)
	{
	print "Opening configuration file " . $ENV{EPOCROOT} . "epoc32\\tools\\epocrc.config.\n" if ($opt_v);
	open EPOCRC_CONFIG, $ENV{EPOCROOT}. "epoc32\\tools\\epocrc.config" or die $ENV{EPOCROOT} . "epoc32\\tools\\epocrc.config";
	
	while ($file_line = <EPOCRC_CONFIG>)
		{
		if($file_line=~/^\s*check_rls_items\s*=\s*(\d+)\s*;?\s*(\#.*)?$/)
			{ # matches check_rls_items = <digits> followed by optional semi-colon followed by optional perl comment
			$check_rls_items = $1;
			}
		elsif($file_line=~/^\s*strict_checking\s*=\s*(\d+)\s*;?\s*(\#.*)?$/)
			{ # matches strict_checking = <digits> followed by optional semi-colon followed by optional perl comment
			$strict_checking = $1;
			}
		elsif($file_line=~/^\s*\#.*$/)
			{ # matches perl comment on a line by itself
			}
		elsif($file_line=~/^\s*$/)
			{ # matches empty lines and lines with only whitespace
			}
		elsif($file_line=~/^\s*Include\:\s*(.*)\s*$/i)
			{ # matches Include: followed by a file location
			push @includes_from_config, $1;
			}
		elsif($file_line=~/^\s*warnings_to_enable\s*=\s*(\S+)\s*;?\s*(\#.*)?$/)
			{ # matches warnings_to_enable = <warnings> where <warnings> is a comma separated list of the warning numbers
			$warnings_to_enable = $1;
			# Append 0's to the warning numbers to make them 3 digit
			@enabled_warnings = map (sprintf("%03d",$_), split(/,/, $warnings_to_enable));
			}
		else
			{
			print "Error: cannot parse line -- $file_line\n";
			exit(1);
			}
		}
	}
else
	{
	print "No configuration file found at " . $ENV{EPOCROOT} . "epoc32\\tools\\epocrc.config, using default values.\n" if ($opt_v);
	}

unless ($check_rls_items==0 || $check_rls_items==1)
	{
	print "* Error: \$check_rls_items must take the value 0 or 1\n";
	exit(1);
	}
unless ($strict_checking==0 || $strict_checking==1)
	{
	print "* Error: \$strict_checking must take the value 0 or 1\n";
	exit(1);
	}

if($check_rls_items==0 && $strict_checking==1)
	{
	print "* Error: \$check_rls_items must be set to 1 to allow strict checking of rls comments\n";
	exit(1);
	}

print "Values: \$check_rls_items=$check_rls_items ,\$strict_checking=$strict_checking and \$warnings_to_enable=$warnings_to_enable\n" if ($opt_v);
 
# Remove the warnings to be enabled from the warnings to be supressed list
if(@enabled_warnings) 
	{
 	foreach my $warnings(@enabled_warnings)
 		{
 		$opt_m =~ s/$warnings,*//g;
 		}
 	}
 
# Remove the last , character from $opt_m, which could have been left by previous processing
$opt_m =~ s/,{1}$//;
	
# If all warnings are removed, then $opt_m needs to be blanked
if ($opt_m =~/^-m$/) 
	{
	$opt_m = "";
	}
use File::Basename;
my $outputfile=$opt_o;
my $rss_base = basename($sourcefile);
my ($rssfile) = split(/\./, $rss_base);	    # remove extension
my $output_base;
my $not_used;
my $lang;
my $rpp_name;

if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {


$output_base = basename($outputfile);
($not_used,$lang) = split(/\.r/, $output_base);	    # get current lang from the filename

$rpp_name = $tmpdir . $rssfile . $lang . ".rpp";

}
else {

$rpp_name = $tmpdir. $rssfile . ".rpp";
	
}
my $headerfile=$opt_h;

if ($opt_v)
	{
	print "* Source file:   $sourcefile\n";
	print "* Resource file: $outputfile\n" if ($outputfile ne "");
	print "* Header file:   $headerfile\n" if ($headerfile ne "");
	}

$opt_o = "-o\"$outputfile\"" if ($outputfile ne "");
$opt_h = "-h\"$headerfile\"" if ($headerfile ne "");


#-------------------------------------------------------
# Run the preprocessor
#

if($opt_cpp) 
	{
	$cpp_spec .= "-D_UNICODE " if ($unicode);
	
	$cpp_spec .= quoted_path($sourcefile) ." -o ". quoted_path($rpp_name);

  # get the HRH file containing all the Variant specific Macros, if not specified on the command line
	$variantMacroHRHFile = Variant_GetMacroHRHFile() if (!$variantMacroHRHFile);
	if($variantMacroHRHFile)
		{
		my $variantFilePath = Path_Split('Path',$variantMacroHRHFile);
		chop( $variantFilePath );
		$cpp_spec .= " -I ".quoted_path($variantFilePath)." -include ".quoted_path($variantMacroHRHFile); 
		}
	
	if($check_rls_items)
		{
		my $includePath;
		foreach my $include (@includes_from_config)
			{
			my $relInclude = $ENV{EPOCROOT} . $include;
			$relInclude=~s/^(.*)\\$/$1/;
			if(-d $relInclude)
				{				
				$cpp_spec .= " -I ".quoted_path($relInclude);
				}
			elsif(-e $relInclude)
				{
				$includePath = Path_Split('Path',$include);
				$includePath=~s/^(.*)\\$/$1/;
				$cpp_spec .= " -I ".quoted_path($ENV{EPOCROOT}.$includePath)." -include ".quoted_path($relInclude);			
				}
			else
				{
				print "Warning; cannot recognise $include as a valid file or directory.\n";
				}
			}
		}
		
	print "* $cpp_spec\n" if ($opt_v);
	system($cpp_spec);

	my $cpp_status = $?;
	die "* cpp failed\n" if ($cpp_status != 0);
	}


#-------------------------------------------------------
# Copy rpp files to epoc32\localisation if not checking
# rls items for localisation tags

if ($opt_l ne "" && $check_rls_items == 0)
	{
if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
	&Lockit_SrcFile($rssfile, $rpp_name, $opt_l, "RSC", "", $outputfile, $lang);
}
else {
	&Lockit_SrcFile($rssfile, $rpp_name, $opt_l, "RSC", "", $outputfile);
}
	}

#-------------------------------------------------------
# Merge rls strings to rpp if not checking rls items
# for localisation tags

if($check_rls_items == 0)
	{
	&Merge_rls_string($rpp_name);	
	}

#-------------------------------------------------------
# Test stage if -test_mode has been used
#

my $rcomp_spec = "";
my $test_number = "";
my $test_dir = "";
my $output_redir = "";
if($test_mode)
	{
	my $file_line;	
	open EPOCRC_TEST, "epocrc.test";
	while ($file_line = <EPOCRC_TEST>)
		{
		if($file_line =~ /^\s*RCOMP:\s*(\S+)\s*$/)
			{
			$rcomp_spec = $1;
			}
		elsif($file_line =~ /^\s*TEST-NUMBER:\s*(\d+)\s*$/)
			{
			$test_number = "$1";
			}
		elsif($file_line =~ /^\s*TEST-DIR:\s*(\S+)\s*$/)
			{
			$test_dir = $1;
			}
		}
	if($rcomp_spec eq "" || $test_number eq "" || $test_dir eq "")
		{
		print "$rcomp_spec\n";
		print "$test_number\n";
		print "$test_dir\n";
		print "Error: could not extract required information from epocrc.test file\n";
		exit(1);
		}
	$output_redir = " 1>" . $test_dir . $rcomp_spec . "\.stdout 2>" . $test_dir . $rcomp_spec . "\.stderr";
	$rcomp_spec .= " ";
	}
		
#-------------------------------------------------------
# Run the resource compiler
#
if($rcomp_spec eq "")
	{
	$rcomp_spec = "rcomp ";
	}
$rcomp_spec .= "-u " if ($unicode);
$rcomp_spec .= "-v " if ($opt_vmore);
$rcomp_spec .= "$opt_m " if ($opt_m ne "");
$rcomp_spec .= "-l " if ($check_rls_items);
$rcomp_spec .= "-force " if($strict_checking); 
if ($opt_uid2 ne '' || $opt_uid3 ne '')
	{
	$opt_uid2 = "0" if ($opt_uid2 eq '');	# defaults to zero
	$opt_uid3 = "*" if ($opt_uid3 eq '');	# defaults to * = derived from NAME
	$rcomp_spec .= "-{$opt_uid2,$opt_uid3} ";
	}
$rcomp_spec .= "$opt_o $opt_h -s\"$rpp_name\" -i\"$sourcefile\"";
$rcomp_spec .= "$output_redir";
print "* $rcomp_spec\n" if ($opt_v);
system($rcomp_spec);
if ($? != 0)
	{
	print "* RCOMP failed - deleting output files\n";
	unlink $outputfile if ($outputfile ne "");
	unlink $headerfile if ($headerfile ne "");
	exit 1;
	}

	if (-e $outputfile)
	{
		use File::stat; 
		if (stat($outputfile)->size > 65535)
		{
			# Resource file bigger than 64kB are not supported.
			print "* Compiled resource file bigger than 64kB\n";
			print "* RCOMP failed - deleting output files\n";
			unlink $outputfile if ($outputfile ne "");
			unlink $headerfile if ($headerfile ne "");
			exit 1;
		}
	}

#-------------------------------------------------------
# Copy rpp files to epoc32\localisation if checked
# file for localisation tags

if ($opt_l ne "" && $check_rls_items == 1)
	{
if (defined $ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} &&  ($ENV{ABLD_TOOLSMOD_COMPATIBILITY_MODE} eq 'alpha')) {
	&Lockit_SrcFile($rssfile, $rpp_name, $opt_l, "RSC", "", $outputfile, $lang);
}
else {
	&Lockit_SrcFile($rssfile, $rpp_name, $opt_l, "RSC", "", $outputfile);
}
}

# exit cleanly

if(!$test_mode)
	{
	print "* deleting $rpp_name\n" if ($opt_v);
	unlink $rpp_name;
	}
exit 0;


#-------------------------------------------------------
# Subroutine: convert path into something acceptable to CPP.EXE
#

sub quoted_path
    {
    my ($arg) = @_;
    return "\"$arg\"" if ($arg !~ /^\\[^\\]/);	# not an absolute path
    if ($curdrive eq "x")
		{
		$curdrive="";
		$curdrive=$1 if (cwd =~ /^(.:)/);	
		}
    return "\"$curdrive$arg\"";
    }

#-------------------------------------------------------
# Subroutine: Merge the rls strings in the rpp file specified
#
sub Merge_rls_string
	{
	my ($rppfile) = @_;

	my $line;
	my $StringId;
	my $key;
	my $value;
	my $StringToSubstitute;
	my %ResourceString;
	
	print "* merging text strings to $rppfile\n" if ($opt_v);
	
	open NEWRPP, ">$rppfile.new" or die "* Can't write to $rppfile.new";
	open RPP, "$rppfile" or die "* Can't open $rppfile";
	
	while ($line = <RPP>) 	{
		while (($StringId, $StringToSubstitute)=each %ResourceString)
		{
			$line=~s/\b$StringId\b/$StringToSubstitute/g if ($line !~ /^rls_string/);
		}
	
		# find quoted "" strings
		if($line =~ /^rls_string\s+(\S+)\s+(.*$)/)
		{
			my $text = $2;
			$key = $1;
			$line=~s/(.*)/\/\/$1/;
			my $substr_count = 0;
			if(!exists $ResourceString{$key})
			{
			SUBSTR:	while (1)
			{
				# find quoted "" strings e.g. "hello"
				if($text =~ /^(\s*\"(.*?\\.)*.*?\")/)		
				{
					$value = $1;
					$text = $';
					++$substr_count;
				}
	
				# find quoted '' strings. e.g. 'world'
				elsif($text =~ /^(\s*\'(.*?\\.)*.*?\')/)			
				{
					$value = $1;
					$text = $';
					++$substr_count;
				}
	
				# find hex strings e.g. <0x34><0x45><0x65>
				elsif($text =~ /^(\s*(<.*?>)+)/)		
				{
					$value = $1;
					$text = $';
					++$substr_count;
				}
	
				# find c comment e.g. /*hello world*/ (may exist between strings)
				elsif($text =~ /^(\s*\/\*.*?\*\/)/)		
				{
					$text = $';
					next SUBSTR; # ignore embedded comment
				}
	
				# find c++ comment e.g. //hello world (may exist after strings)
				elsif($text =~ /^(\s*\/\/.*$)/)		
				{
					$text = $';
					next SUBSTR; # ignore trailing comment
				}
	
				# exit search
				else
				{
					if ($substr_count == 0)
					{
						warn("WARNING: rls_string $key either has incorrect syntax or no value\n");
					}
					last SUBSTR;
				}
			$ResourceString{$key} .= $value;
			}
		  	}
		}
		print NEWRPP $line;
	}

	close RPP;
	close NEWRPP;
	copy ("$rppfile.new", "$rppfile");
	unlink ("$rppfile.new");
	}
