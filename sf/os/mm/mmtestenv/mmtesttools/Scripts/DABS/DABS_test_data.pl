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
#

#!/usr/bin/perl

################################################################################
######### This file is meant to be used only once to create an initial
######### test list and the belonging batch and command files.
################################################################################

######### The input is the old mmbuild batch files under TestTools\Scripts
################################################################################
use strict;

# open file which contains all batch files that contains copy and delete commands.
open FILES, "test_data.txt" or die "couldn't open file: $!";

my $input_file;
my $output_copy_file;
my $output_del_file;

while (<FILES>)
{
    chomp;
    
    $output_copy_file = $_;
    $input_file = $output_copy_file;
    $output_copy_file =~ s/.bat//g;
    $output_copy_file =~ s/\.\.\/run//g;
    $output_copy_file .= "_copy_data.txt";

    open OUT, ">$output_copy_file" or die "couldn't open $output_copy_file: $!";

    open IN, "$input_file" or die "couldn't open $input_file: $!";

    # write all copy commands to OUT file.
    while (<IN>)
    {
	chomp;
	my $line = $_;
	if ($line =~ /^md/i || $line =~ /^copy/i)
	{
	    print OUT $line . "\n";
	}
    }
    close OUT;
    close IN;

    # create the DABS pretest command file. Can only be one row.
    #commented out for the moment as a DABS hack exists.
    #my $output_cmd_file = $output_copy_file;
    #$output_cmd_file =~ s/bat$/txt/;
    #open DABS_cmd_file, ">$output_cmd_file" or die "couldn't open $output_cmd_file: $!";

    #print DABS_cmd_file "call e:\\$output_copy_file\n";

    #close DABS_cmd_file;
    
    $output_del_file = $output_copy_file;
    $output_del_file =~ s/copy/del/g;

    open OUT, ">$output_del_file" or die "couldn't open $output_del_file: $!";

    open IN, "$input_file" or die "couldn't open $input_file: $!";

    # write all del commands to OUT file
    while (<IN>)
    {
	chomp;
	my $line = $_;
	if ($line =~ /^del.*c:/i)
	{
	    print OUT $line . "\n";
	}
    }
    
    close OUT;
    close IN;

    # create the DABS posttest command file. Can only be one row.
    #commented out for the moment as a DABS hack exists.
    #$output_cmd_file = $output_del_file;
    #$output_cmd_file =~ s/bat$/txt/;
    #open DABS_cmd_file, ">$output_cmd_file" or die "couldn't open $output_cmd_file: $!";

    #print DABS_cmd_file "call e:\\$output_del_file\n";

    #close DABS_cmd_file;
    
    
}

close FILES;

open DABS_TESTS, "new_tests_92.txt" or die "couldn't open file: $!";

open FILES, "test_data.txt" or die "couldn't open file: $!";

open OUT_SCRIPT, ">real_tests_92.txt" or die "couldn't open file: $!";

my @test_rows;

# find where in the test list the pretest and posttest commands should be,
# and insert them in the list.
# This while loop actually creates load of duplicates of tests that don't
# need a pre or post test command.
# Those duplicates are removed futher down.
while (<DABS_TESTS>)
{
    my ( $program, $log, $commdb, $script, $timeout, $release, $preCommand, $postCommand );

    chomp;

    ($program, $log, $commdb, $script, $timeout, $release, $preCommand, $postCommand) = split(/,/);

    # Strip any trailing spaces
    $program =~ s!\ $!! if( defined($program));
    $log =~ s!\ $!! if( defined($log));
    $commdb =~ s!\ $!! if( defined($commdb));
    $script =~ s!\ $!! if( defined($script));
    $timeout =~ s!\ $!! if( defined($timeout));
    $release =~ s!\ $!! if( defined($release));
    $preCommand =~ s!\ $!! if( defined($preCommand));
    $postCommand =~ s!\ $!! if( defined($postCommand));
    
    # Strip any leading spaces
    $program =~ s!^\ !! if( defined($program));
    $log =~ s!^\ !! if( defined($log));
    $commdb =~ s!^\ !! if( defined($commdb));
    $script =~ s!^\ !! if( defined($script));
    $timeout =~ s!^\ !! if( defined($timeout));
    $release =~ s!^\ !! if( defined($release));
    $preCommand =~ s!^\ !! if( defined($preCommand));
    $postCommand =~ s!^\ !! if( defined($postCommand));
    
    # Set defaults for undefined values
    $program = "" unless ($program);
    $log = "" unless ($log);
    $commdb = "" unless ($commdb);
    $script = "" unless ($script);
    $timeout = $::TestTimeout unless ($timeout);
    $preCommand = "" unless ($preCommand);
    $postCommand = "" unless ($postCommand);
    
    my 	$found_script = 0;
    open FILES, "test_data.txt" or die "couldn't open file: $!";
    
    while (<FILES>)
    {
	chomp;
	my $input_file = $_;

	open IN, "$input_file" or die "couldn't open $input_file: $!";

	while (<IN>)
	{
	    chomp;
	    my $line = $_;
	    if ($line =~ /$script/i)
	    {
		$preCommand = $input_file;
		$preCommand =~ s/.bat//g;
		$preCommand =~ s/\.\.\/run//g;
		$preCommand .= "_copy_data.txt";
		$postCommand = $preCommand;
		$postCommand =~ s/copy/del/g;
		
		push @test_rows, "$program, $log, $commdb, $script, $timeout, $release, $preCommand, $postCommand\n";
		$found_script = 1;
	    }
	}
    	close IN;
    }
    close FILES;

    if (!$found_script)
    {
	push @test_rows, "$program, $log, $commdb, $script, $timeout, $release, ,\n";
    }
}


# Some perl magic to remove duplicate lines.
# Input: @list
# Output: @uniqed
my %u = ();

my @uniqed = grep {defined} map {
    if (exists $u{$_}) { undef; } else { $u{$_} = undef; $_; }
} @test_rows;

undef %u;

my $row;

# The tests have been uniqed, so write them out to the new test list.
foreach $row (@uniqed)
{
    print OUT_SCRIPT "$row";
}

close OUT_SCRIPT;

open DABS_TESTS, "real_tests_92.txt" or die "couldn't open file: $!";

# Fix the del files so they remove the log file of the test script.
while (<DABS_TESTS>)
{
    my ( $program, $log, $commdb, $script, $timeout, $release, $preCommand, $postCommand );

    chomp;

    ($program, $log, $commdb, $script, $timeout, $release, $preCommand, $postCommand) = split(/,/);

    next if ($postCommand eq "");

    # Strip any trailing spaces
    $log =~ s!\ $!! if( defined($log));
    $postCommand =~ s!\ $!! if( defined($postCommand));
    
    # Strip any leading spaces
    $log =~ s!^\ !! if( defined($log));
    $postCommand =~ s!^\ !! if( defined($postCommand));
    
    # $postCommand =~ s/\.txt/\.bat/i;

    open DEL, ">>$postCommand" || die "couldn't open $postCommand for append: $!";
    print DEL "del c:\\$log\n";
    close DEL;
}
