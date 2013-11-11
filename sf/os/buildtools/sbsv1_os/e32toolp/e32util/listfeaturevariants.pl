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
# list A.X.aaa for all X (32 chars) when given A.aaa
# 
#

use strict;

my $source = shift;

# list invariant
print "$source\n" if (-f $source);

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

opendir(DIR, $srcDir) or die("ERROR: cannot read directory $srcDir\n");

# list all variants
while (my $file = readdir(DIR))
{
	print "$srcDir\\$file\n" if ($file =~ /^$srcRoot\.(\w{32})\.$srcExt$/i);
}

exit 1 if (!closedir(DIR));
exit 0;

