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
# copy A.X.aaa to B.X.bbb for all X (32 chars) when given A.aaa B.bbb
# 
#

use strict;

my $source = shift;
my $target = shift;

my $errs = 0;

# copy invariant
use File::Copy;
if (-f $source)
{
	if (copy($source, $target))
	{
		print "copy $source $target\n";
	}
	else
	{
		print "ERROR: failed copy $source $target\n";
		$errs++;
	}
}

# now think about variants

use File::Basename;
my $srcDir = dirname($source);
my $srcRoot = basename($source);
my $srcExt = "";

if ($srcRoot =~ /^(.+)\.([^\.]+)$/)
{
	$srcRoot = $1;
	$srcExt = $2;
}

my $tgtRoot = $target;
my $tgtExt = "";

if ($tgtRoot =~ /^(.+)\.([^\.]+)$/)
{
	$tgtRoot = $1;
	$tgtExt = $2;
}

opendir(DIR, $srcDir) or die("ERROR: cannot read directory $srcDir\n");

# copy all variants
while (my $file = readdir(DIR))
{
	if ($file =~ /^$srcRoot\.(\w{32})\.$srcExt$/i)
	{
		my $from = "$srcDir/$file";
		my $into = "$tgtRoot.$1.$tgtExt";

		if (copy($from, $into))
		{
			print "copy $from $into\n";
		}
		else
		{
			print "ERROR: failed copy $from $into\n";
			$errs++;
		}
	}
}

exit 1 if (!closedir(DIR) || $errs > 0);
exit 0;

