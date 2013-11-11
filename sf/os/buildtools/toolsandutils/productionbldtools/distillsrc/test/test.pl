#!\bin\perl -w
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
# distillsrc.pl test script
# 
#

use FindBin;
use Cwd ('chdir','abs_path');

my $cwd = abs_path($FindBin::Bin."\\..");
chdir($cwd) or die "Couldn't change directory to $cwd\n";

# Test cases
#
# Entry format: [parameters, [[pass regexps],[fail regexps]]]
# 
# [pass regexps] and [fail regexps] are lists of regular expressions, each
# as a string. All pass regexps must match. No fail regexps may match.
# If a line matches both, it is said to have passed (hence a generic failure
# rule can be written to match any output which wasn't expected)
my @testcases =
	(
	['-r fred -l beech' ,
		[
			['^ERROR: RealTimeBuild:.*srcpath must be given'],
			['^ERROR: RealTimeBuild:']
		]
	],
	['-s fred -l beech' ,
		[
			['^ERROR: RealTimeBuild:.*srcroot must be given'],
			['^ERROR: RealTimeBuild:']
		]
	],
	['-r fred -s src' ,
		[
			['^ERROR: RealTimeBuild:.*platform'],
			['^ERROR: RealTimeBuild:']
		]
	],
	['-d -r test -s source -p prefix -l fred -c test\options.txt -N' ,
		[
			[
			'= spacey$',
			'= whole$',
			'^DUMMY: File test\\\\source\\\\fixer.mrp',
			'^DUMMY: File test\\\\source\\\\DISTRIBUTION\\.POLICY',
			'^DUMMY: File test\\\\source\\\\missing\\\\unowned'
			],
			[
			'^DUMMY:.*is not specified in any \\.mrp file',
			'does not include itself'
			]
		]
	],
	['-d -r test -s source -p prefix -l fred -c test\options.txt -m test\source\fixer.mrp' ,
		[
			[
			'= spacey$',
			'= whole$',
			'= fixer$',
			'WARNING: \[fixer\] Case of .test\\\\soUrce\\\\DIStribuTION.poliCY. does not match.* Should be test\\\\source\\\\DISTRIBUTION.POLICY$',
			'WARNING: \[fixer\] Case of .test\\\\soUrce\\\\missing.* does not match.* Should be test\\\\source\\\\missing',
			'source\\\\spacey\\\\spa cey.txt = spacey$',
			],
			[
			'^DUMMY:.*is not specified in any \\.mrp file',
			'does not include itself'
			]
		]
	],
	['-d -r test -s source -p prefix -l fred -f test\base.txt' ,
		[
			[
			'= whole$',
			'^DUMMY: File test\\\\source\\\\fixer\\.mrp',
			'^DUMMY: File test\\\\source\\\\DISTRIBUTION\\.POLICY',
			'^DUMMY: Directory test\\\\source\\\\missing',
			'^DUMMY: File test\\\\source\\\\shared\\\\missing',
			'^DUMMY: Directory test\\\\source\\\\spacey'
			],
			[
			'^DUMMY:.*is not specified in any \\.mrp file',
			'does not include itself'
			]
		]
	],
	['-d -r test -s nowt -l fred -m test\naughty.mrp' ,
		[
			[
			'nothing_important.* doesn\'t exist',
			'No \.mrp files claim any source',
			'naughty\.mrp.*does not include itself'
			],
			['^DUMMY:.*is not specified in any \\.mrp file']
		]
	],
	['-r test -s source -l fred -m test\not_present.mrp' ,
		[
			['^ERROR: RealTimeBuild:.*not_present\\.mrp.*does not exist'],
			[]
		]
	],
	['-r test -s source -l fred -m test\duffer.mrp',
		[
			['^ERROR: RealTimeBuild:.*not a valid command.*test\\\\duffer\\.mrp'],
			['= duffer$']
		]
	],
	['-d -r test -s source -p prefix -l fred -f test\base.txt',
		[
			[
			'^WARNING:.*Case of \'.*complete.mrp\' does not match.* Should be.*source.*$',
			'^WARNING:.*Case of \'.*whole.mrp\' does not match.* Should be.*whole.whole.*$',
			],
			['^WARNING:.*Case of .* does not match.* Should be.*$']
		]
	],
	['-d -r test -s source -p prefix -l fred -m test\bincase.mrp',
		[
			[
			'^REMARK:.*\\\\epoc32\\\\thisOneIsnt should be lower case',
			'^REMARK:.*\\\\epoc32\\\\andNorIs\/thisone should be lower case',
			'^REMARK:.*\\\\epoc32\\\\thisonesNot should be lower case',
			'^REMARK:.*\\\\epoc32\\\\norThis\\\\one should be lower case'
			],
			['^REMARK:.*should be lower case.*$']
		]
	]
	);

my $passes = 0;
my $tests = 0;

foreach my $testcase (@testcases)
	{
	my $cmd = "distillsrc.pl ".$testcase->[0];
	print "\nRunning $cmd\n";
	my @output = `perl $cmd 2>&1`;
	my @passregexps = @{$testcase->[1]->[0]};
	my @failregexps = @{$testcase->[1]->[1]};

	my $testpass = -1; # (unset)

	foreach my $line (@output)
		{
		chomp ($line);
		my $pass = 0;
		my $fail = 0;
		
		foreach my $passregexp (@passregexps)
			{
			if ($line =~ /$passregexp/)
				{
				$pass = 1;

				# Remove rule from list
				@passregexps = grep($_ ne $passregexp, @passregexps);
				}
			}

		if (!$pass)
			{
			foreach my $failregexp (@failregexps)
				{
				if ($line =~ /$failregexp/)
					{
					$fail = 1;
					}
				}
			}

		if ($pass)
			{
			print "GOOD:$line\n";
			if ($testpass == -1)
				{
				$testpass = 1; # Passed (so far)
				}
			}
		elsif ($fail)
			{
			print "BAD:$line\n";
			$testpass = 0; # Failed
			}
		else { print "OKEH:$line\n"; }
		}
	
	if ($testpass == 1)
		{
		if ((scalar @passregexps) == 0)
			{
			print "*** PASS\n";
			$passes ++;
			}
		else
			{
			print "*** FAIL - tests not matched:\n> ";
			print join("\n> ", @passregexps)."\n";
			}
		}
	elsif ($testpass == 0)
		{
		print "*** FAIL - hard fail\n";
		}
	else
		{
		print "*** FAIL - no tests matched\n";
		}

	$tests ++;
	}

print "*** Out of $tests tests, $passes passed\n";
