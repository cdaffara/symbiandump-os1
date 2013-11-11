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

my $fails = 0;
my $total = 0;

# Each test has an input OBY file and an output OBY file.
# runTest assumes there is also a <input>.oby.out file which
# contains the expected OBY output.

# Adding a "." to the feature variant name (for example using
# the option "-DFEATUREVARIANT=.phone") means that buildrom will
# look for the variant file in ./phone.var rather than in the 
# default location of EPOCROOT/epoc32/tools/variant/phone.var

# test the minimal OBY file

$fails += runTest("-s", "minimal.oby", "result.oby");
$total++;

# test the include path from a feature-variant OBY file

$fails += runTest("-s -DFEATUREVARIANT=.phone", "var1.oby", "result.oby");
$total++;

# test the file substitutions in a feature-variant OBY file

$fails += runTest("-s -DFEATUREVARIANT=.phone", "var2.oby", "result.oby");
$total++;

# test an OBY file that is on the include path from a feature-variant

$fails += runTest("-s -DFEATUREVARIANT=.phone", "varB", "result.oby");
$total++;

# test that absolute paths work everywhere

createDummyInclude($ENV{'EPOCROOT'} . "epoc32/include/test/abs.hrh");
createDummyInclude($ENV{'EPOCROOT'} . "epoc32/include/test/incA/absA.iby");
createDummyInclude($ENV{'EPOCROOT'} . "epoc32/include/test/incB/absB.iby");

$fails += runTest("-s -DFEATUREVARIANT=.abs", "var3.oby", "result.oby");
$total++;

###########################################################################
# report the results and finish

print "\n\n$fails test";
print "s" unless ($fails == 1);
print " failed (out of $total run).\n";
exit 0;

###########################################################################

sub runTest
{
	my $options = shift;
	my $inFile  = shift;
	my $outFile = shift;
	my $expFile = "$inFile.out";

	my $command = "buildrom -fm=..\\include\\featureUIDs.xml $options $inFile";

	# remove the output if it already exists
	unlink($outFile);

	# run the command (it will error because we do not have everything
	# required by rombuild, but we will get an output oby file which
	# shows whether buildrom did the right thing or not).
	system($command . " >nul 2>&1");

	if (!open(OUTPUT, $outFile))
	{
		print STDERR "FAILED: $command\n\t$outFile was not created\n";
		return 1;
	}

	if (!open(EXPECT, $expFile))
	{
		print STDERR "FAILED: $command\n\t$expFile was not found\n";
		close(OUTPUT);
		return 1;
	}

	my $outLine;
	my $expLine;
	my $lineNumber = 1;
	my $outMissing = 0;
	my $expMissing = 0;
	my $nonMatches = 0;

	while ($expLine = <EXPECT>)
	{
		$expLine =~ s/^\s+//;
		$expLine =~ s/\s+$//;

		if ($outLine = <OUTPUT>)
		{
			$outLine =~ s/^\s+//;
			$outLine =~ s/\s+$//;

			if (!($expLine eq $outLine))
			{
				print STDERR "FAILED: $command\n";
				print STDERR "\texpected output line $lineNumber was:\n";
				print STDERR "\t$expLine\n";
				print STDERR "\tactual output line $lineNumber was:\n";
				print STDERR "\t$outLine\n";
				$nonMatches++;
			}
		}
		else
		{
			$outMissing++;
		}
		$lineNumber++;
	}
	while ($outLine = <OUTPUT>)
	{
		$expMissing++;
	}

	close(EXPECT);
	close(OUTPUT);

	if ($nonMatches)
	{
		print STDERR "FAILED: $command\n";
		print STDERR "\t$nonMatches lines did not match\n";
		return 1;
	}
	if ($outMissing)
	{
		print STDERR "FAILED: $command\n";
		print STDERR "\toutput file $outFile is $outMissing lines too short\n";
		return 1;
	}
	if ($expMissing)
	{
		print STDERR "FAILED: $command\n";
		print STDERR "\toutput file $outFile is $expMissing lines too long\n";
		return 1;
	}

	print STDERR "OK: $command\n";
	return 0;	# OK
}

use File::Basename;

sub createDummyInclude
{
	my $file = shift;
	my $dir = dirname($file);

	mkdir($dir) if (!-d $dir);

	if (open(FILE, ">$file"))
	{
		print FILE "/* test file */\n";
		close(FILE);
	}
	else
	{
		print STDERR "could not write file $file\n";
	}
}
