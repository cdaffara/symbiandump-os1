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
# Wrapper to support the EPOC AIF Compiler
# 
#

use strict;

use FindBin;		# for FindBin::Bin
use File::Copy;		# for copy()
use Cwd;		# for cwd
use File::Basename;	# for basename()

my $PerlBinPath;	# fully qualified pathname of the directory containing this script
my $curdrive="x";	    	# will be initialised when first needed

# establish the path to the Perl binaries
BEGIN {
	require 5.005_03;		# check user has a version of perl that will cope
	$PerlBinPath = $FindBin::Bin;	# X:/epoc32/tools
	$PerlBinPath =~ s/\//\\/g;	# X:\epoc32\tools
}
use lib $PerlBinPath;
use E32Variant;         # for variant specific macros
use Pathutl;
use Preprocessor;

sub print_usage
	{
#........1.........2.........3.........4.........5.........6.........7.....
	print <<USAGE_EOF;

Usage:
  epocaif [options] srcfile [-o outputfile] [-t tmpdir] [-b "bmps" | -m mbm] [-l "TargetPath:CWD"]


The available options are

   -Ixxx  -- C++ preprocessor arguments
   -o	  -- output AIF file name including path
   -t	  -- tempory directory for intermediate files
   -b	  -- list of bitmaps Eg., "-b/c8\\location\\bmp1 /c8\\location\\bmp2.."
   -m	  -- compiled MBM file (alternative to -b)
   -l	  -- if specified, captures all source to \\epoc32\\localisation\\...

The aif resource file is then passed through the C++ preprocessor, using any 
specified preprocessor arguments, and then compiled with RCOMP.EXE to 
generate a compiled resource.
The -m or -b option is used to generate a suitable MBM file, if required.
The MBM and the compiled resource are then combined to produce an AIF file.


USAGE_EOF
	}

#-------------------------------------------------------
# Process commandline arguments
#
# Can't use the Getopt package because it doesn't like the -D and -I style options
#
my $sourcefile="";
my $opt_o="";
my $opt_h="";	
my $tmpdir="";
my $opt_v=1;
my $opt_b="";
my $opt_l="";
my $opt_m="";
my $TrgPath;
my $xipaif=1;

my $exe = &PreprocessorToUseExe();
my $cpp_spec= "$exe -undef -C ";	    # preserve comments

my $errors = 0;
while (@ARGV)
	{
	my $arg = shift @ARGV;

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
	if ($arg =~ /^-v$/)
		{
		$opt_v =1;
		next;
		}
	if ($arg =~ /^-o(.*)$/)
		{
		$opt_o =$1;
		$TrgPath = $opt_o;
		next;
		}

	if ($arg =~ /^-t(.*)\\?$/)
		{
		$tmpdir =$1;
		next;
		}
	if ($arg =~ /^-b(.*)$/)
		{
		$opt_b =$1;
		next;
		}	
	if ($arg =~ /^-m(.*)$/)
		{
		$opt_m =$1;
		next;
		}	
	if ($arg =~ /^-l(.*)$/)
		{
		$opt_l =$1;
		next;
		}	

	if ($arg =~ /^-/)
		{
		print "Unknown arg: $arg\n";
		$errors++;
		next;
		}
	$sourcefile=$arg;
	}

if ($opt_m ne "" && $opt_b ne "")
	{
	print "Can't specify both -m and -b\n";
	$errors++;
	}
if ($errors || $sourcefile eq "")
	{
	print_usage();
	exit 1;
	}

my $rss_base = basename($sourcefile);
my ($rssfile) = split(/\./, $rss_base);	    # remove extension
my $rpp_name = "$tmpdir\\$rssfile.rpp";
my $outputfile="$tmpdir\\AIF.RSC";	
my $headerfile=$opt_h;

if ($opt_v)
	{
	print "* Source file:   $sourcefile\n";
	print "* Resource file: $outputfile\n" if ($outputfile ne "");
	}

$opt_o = "-o\"$outputfile\"" if ($outputfile ne "");
$opt_h = "-h\"$headerfile\"" if ($headerfile ne "");


#-------------------------------------------------------
# Run the preprocessor
#

my $variantMacroHRHFile = Variant_GetMacroHRHFile();
if($variantMacroHRHFile){
    my $variantFilePath = Path_Split('Path',$variantMacroHRHFile);
    chop ( $variantFilePath );
    $cpp_spec .= " -I \"" . &Path_RltToWork($variantFilePath) . "\" -include " . &Path_RltToWork($variantMacroHRHFile) . " "; 
}

$cpp_spec .= "-I $PerlBinPath\\..\\include ";	# extra path to support shared tools
$cpp_spec .= "-D_UNICODE ";

$cpp_spec .= quoted_path($sourcefile) ." -o ". quoted_path($rpp_name);

print "* $cpp_spec\n" if ($opt_v);
system($cpp_spec);

my $cpp_status = $?;
die "* cpp failed\n" if ($cpp_status != 0);


#-------------------------------------------------------
# Copy source to epoc32\localisation
#

if ($opt_l ne "")
{
use lockit_info;
	my ($rssfile, $FileType) = split(/\./, basename($TrgPath));
	&Lockit_SrcFile($rssfile, $rpp_name, $opt_l, $FileType, $opt_b);
}

#-------------------------------------------------------
# Merge rls strings to rpp
#
&Merge_rls_string($rpp_name);

#-------------------------------------------------------
# Run the resource compiler
#

my $rcomp_spec = "rcomp -u ";
$rcomp_spec .= "-:$tmpdir\\_dump_of_resource_ "; # causes Rcomp to dump each resource (uncompressed and unpadded) in $tmpdir\\_dump_of_resource_1, $tmpdir\\_dump_of_resource_2, etc
$rcomp_spec .= "$opt_o $opt_h -s\"$rpp_name\" -i\"$sourcefile\"";

print "* $rcomp_spec\n" if ($opt_v);
system($rcomp_spec);
if ($? != 0)
	{
	print "* RCOMP failed - deleting output files\n";
	unlink $outputfile if ($outputfile ne "");
	unlink $headerfile if ($headerfile ne "");
	exit 1;
	}
print "* deleting $rpp_name\n" if ($opt_v);
unlink $rpp_name;

#-------------------------------------------------------
# Run bmconv, if needed
#

if ($opt_b ne "")
	{
	print "* bmconv /q $tmpdir\\AIF.MBM $opt_b\n" if ($opt_v);
	system("bmconv /q $tmpdir\\AIF.MBM $opt_b");
	if ($? != 0)
		{
		print "* BMCONV failed\n";
		exit 1;
		}
	print "* bmconv /q /s $tmpdir\\AIF_XIP.MBM $opt_b\n" if ($opt_v);
	system("bmconv /q /s $tmpdir\\AIF_xip.MBM $opt_b");
	
	if ($? != 0)
		{
		print "* BMCONV failed\n";
		exit 1;
		}
	}
elsif ($opt_m ne "")
	{
	print "* copy $opt_m $tmpdir\\AIF.MBM\n" if ($opt_v); 
	copy($opt_m, "$tmpdir\\AIF.MBM");
	# no xip file genarated 
	$xipaif=0;
	}
else
	{
	# no bitmap specified - this is legitimate
	unlink("$tmpdir\\AIF.MBM");
	unlink("$tmpdir\\AIF_xip.MBM");
	}

#-------------------------------------------------------
# Get the from UID from the first four bytes of "$tmpdir\\_dump_of_resource_1"
#

open(DUMP_OF_RESOURCE_1, "< $tmpdir\\_dump_of_resource_1") or die("* Can't open dump file\n");
binmode(DUMP_OF_RESOURCE_1);
my $data;
my $numberOfBytesRead=read(DUMP_OF_RESOURCE_1, $data, 4);
defined($numberOfBytesRead) or die("* Can't read from dump file\n");
($numberOfBytesRead>=4) or die("* Dump file too short\n");
my $uid=(unpack('V', $data))[0];
undef($data);
undef($numberOfBytesRead);
close(DUMP_OF_RESOURCE_1) or die("* Can't close dump file\n");

#-------------------------------------------------------
# Produce the AIF file from the RSC and MBM files
#

my $uidcrc = "uidcrc.exe 0x101fb032 0 ".sprintf('0x%08x', $uid)." $tmpdir\\out.aif";
my $uidcrc_xip = "uidcrc.exe 0x101fb032 0 ".sprintf('0x%08x', $uid)." $tmpdir\\out_xip.aif";

print "* $uidcrc\n" if ($opt_v);
system($uidcrc);
if ($? != 0)
	{
	print "* UIDCRC failed\n";
	exit 1;
	}
if ($xipaif ne 0)
	{	
	print "* $uidcrc\n" if ($opt_v);
	system($uidcrc_xip);
	if ($? != 0)
		{
		print "* UIDCRC failed\n";
		exit 1;
		}
	}


open(OUT_AIF, ">> $tmpdir\\out.aif") or die("* Can't open temporary file\n");
binmode(OUT_AIF);

if ($xipaif ne 0)
	{
	open(OUTXIP_AIF, ">> $tmpdir\\out_xip.aif") or die("* Can't open temporary file\n");
	binmode(OUTXIP_AIF);
	}

print "* Writing length of the RSC-block\n" if ($opt_v);
my $lengthOfRscBlock=-s("$tmpdir\\aif.rsc");
my $numberOfPaddingBytes=(4-($lengthOfRscBlock%4))%4;
print(OUT_AIF pack('V', $lengthOfRscBlock));
if ($xipaif ne 0)
	{
	print(OUTXIP_AIF pack('V', $lengthOfRscBlock));
	}
print "* Appending the RSC-block\n" if ($opt_v);
&appendFile(\*OUT_AIF, "$tmpdir\\aif.rsc");
if ($xipaif ne 0)
	{
	&appendFile(\*OUTXIP_AIF, "$tmpdir\\aif.rsc");
	}
# append any necessary padding bytes so that the file-offset of the start of the MBM-block is a multiple of 4-bytes
print(OUT_AIF ('_' x $numberOfPaddingBytes));
if ($xipaif ne 0)
	{
	print(OUTXIP_AIF ('_' x $numberOfPaddingBytes));
	}
if (-e("$tmpdir\\aif.mbm"))
	{
	print "* Appending the MBM-block\n" if ($opt_v);
	&appendFile(\*OUT_AIF, "$tmpdir\\aif.mbm");
	}
if (-e("$tmpdir\\aif_xip.mbm"))
	{
	print "* Appending the XIPMBM-block\n" if ($opt_v);
	&appendFile(\*OUTXIP_AIF, "$tmpdir\\aif_xip.mbm");
	}

close(OUT_AIF) or die("* Can't close temporary file\n");
if ($xipaif ne 0)
	{
	close(OUTXIP_AIF) or die("* Can't close temporary file\n");
	}
print "* copy $tmpdir\\out.aif $TrgPath\n" if ($opt_v);
copy("$tmpdir\\out.aif", "$TrgPath");
if ($xipaif ne 0)
	{
	my $basepath = &Path_Split('Path', $TrgPath);
	my $ext=&Path_Split('Ext',  $TrgPath);
	my $basename = basename($TrgPath, $ext);
	my $xip="_xip";
	print "* copy $tmpdir\\out_xip.aif $basepath$basename$xip$ext\n" if ($opt_v);
	copy("$tmpdir\\out_xip.aif", "$basepath$basename$xip$ext");
	}
unlink("$tmpdir\\_dump_of_resource_*");
unlink("$tmpdir\\aif.rsc");
unlink("$tmpdir\\aif.mbm");
unlink("$tmpdir\\out.aif");
unlink("$tmpdir\\aif_xip.mbm");
unlink("$tmpdir\\out_xip.aif");
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

#-------------------------------------------------------
# Subroutine: Append a file into the open (binary) file already opened
#
sub appendFile
	{
	my $fileHandleOfTarget=shift;
	my $fileNameOfSource=shift;
	open(SOURCE, "< $fileNameOfSource") or die("* Can't open $fileNameOfSource\n");
	binmode(SOURCE);
	for (;;)
		{
		my $data;
		my $numberOfBytesRead=read(SOURCE, $data, 1024);
		defined($numberOfBytesRead) or die("* Can't read from $fileNameOfSource\n");
		if ($numberOfBytesRead==0)
			{
			last;
			}
		print($fileHandleOfTarget $data);
		}
	close(SOURCE) or die("* Can't close $fileNameOfSource\n");
	}

