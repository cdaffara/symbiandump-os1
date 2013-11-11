# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package TestScriptResults;
use strict;
use Exporter;
use base 'Exporter'; 

# Crashing enums
my $KTestDidNotCrash = 0;
my $KTestCrashed = 1;

# Test Harness enums
our $KCoreConfTest = "CoreConf";
our $KTEFTest = "TEF";
our $KTEFNoTestcasesTest = "TEFNoTestcases";	# Some of the older MM TEF tests were implemented without using testcases, in these cases we use the test steps in their place
our $KTestFrameworkTest = "TF";

our @EXPORT = qw($KCoreConfTest, $KTEFTest, $KTEFNoTestcasesTest, $KTestFrameworkTest);

# Constructor
sub TestScriptResults
	{
	my $this = {};
	$this->{ScriptName} = undef;
	$this->{FilePath} = undef;
	$this->{DidItCrash} = $KTestDidNotCrash;
	$this->{TotalNumberTestCases} = undef;
	$this->{Inconclusives} = [1];	# First entry gives the next free index
	$this->{Fails} = [1];	# First entry gives the next free index
	$this->{TestHarness} = undef;
	bless($this);
	return $this;
	}

sub SetScriptName
	{
	my $this = shift;
	$this->{ScriptName} = shift;
	}

sub ScriptName
	{
	my $this = shift;
	return $this->{ScriptName};
	}
	
sub SetFilePath
	{
	my $this = shift;
	$this->{FilePath} = shift;
	}

sub FilePath
	{
	my $this = shift;
	return $this->{FilePath};
	}

sub CoreConfTest
	{
	my $this = shift;
	$this->{TestHarness} = $KCoreConfTest;	
	}

sub TEFTest
	{
	my $this = shift;
	$this->{TestHarness} = $KTEFTest;
	}
		
sub TEFNoTestcasesTest
	{
	my $this = shift;
	$this->{TestHarness} = $KTEFNoTestcasesTest;
	}
	
sub TestFrameworkTest
	{
	my $this = shift;
	$this->{TestHarness} = $KTestFrameworkTest;
	}

sub TestHarness
	{
	my $this = shift;
	return $this->{TestHarness};	
	}
	
sub AddInconclusiveResult
	{
	my $this = shift;
	my $currentFreeIndex = $this->{Inconclusives}[0];
	$this->{Inconclusives}[$currentFreeIndex] = shift;
	$currentFreeIndex++;
	$this->{Inconclusives}[0] = $currentFreeIndex;
	}

sub ResetInconclusives
	{
	my $this = shift;
	$this->{Inconclusives}[0] = 1;	
	}
	
sub Inconclusives
	{
	my $this = shift;
	my @inconList = @{$this->{Inconclusives}};	
	my $finalIndex = $inconList[0] - 1;
	return @inconList[1 .. $finalIndex];	
	}
	
sub AddFailureResult
	{
	my $this = shift;
	my $currentFreeIndex = $this->{Fails}[0];
	$this->{Fails}[$currentFreeIndex] = shift;
	$currentFreeIndex++;
	$this->{Fails}[0] = $currentFreeIndex;
	}

sub ResetFailures
	{
	my $this = shift;
	$this->{Fails}[0] = 1;	
	}	
	
sub Failures
	{
	my $this = shift;
	my @failureList = @{$this->{Fails}};	
	my $finalIndex = $failureList[0] - 1;
	return @failureList[1 .. $finalIndex];
	}	
	
sub SetTotalTestCount
	{
	my $this = shift;
	$this->{TotalNumberTestCases} = shift;
	}

sub TotalTestCount
	{
	my $this = shift;
	return $this->{TotalNumberTestCases};
	}

sub TestCrashed
	{
	my $this = shift;
	$this->{DidItCrash} = $KTestCrashed;
	}

sub ResetCrashed
	{
	my $this = shift;
	$this->{DidItCrash} = $KTestDidNotCrash;
	}
	
sub DidItCrash
	{
	my $this = shift;
	return $this->{DidItCrash};
	}
	
sub AnyFailures
	{
	my $this = shift;
	if (($this->{Fails}[0] > 1) || ($this->{Inconclusives}[0] > 1) || ($this->DidItCrash()))
		{
		return 1;
		}
	return 0;	
	}
	
sub DebugPrint
	{
	my $this = shift;
	print "\nTest script: $this->{ScriptName}\n$this->{FilePath}\n";
	if ($this->{DidItCrash} == $KTestCrashed)
		{
		print "It CRASHED\n";
		}
	print "Test cases run: $this->{TotalNumberTestCases}\n";
	
	my $lastIndex = $this->{Inconclusives}[0] - 1;
	if ($lastIndex > 0)
		{
		print "INCONCLUSIVES:\n";
		foreach my $inconclusiveResult (@{$this->{Inconclusives}}[1 .. $lastIndex])
			{
			print "$inconclusiveResult\n";
			}
		}
		
	$lastIndex = $this->{Fails}[0] - 1;
	if ($lastIndex > 0)
		{
		print "FAILS:\n";
		foreach my $failResult (@{$this->{Fails}}[1 .. $lastIndex])
			{
			print "$failResult\n";
			}
		}
	}