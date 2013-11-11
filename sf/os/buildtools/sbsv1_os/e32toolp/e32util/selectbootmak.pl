# Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Used to select a makefile which matches the installed version of armasm
# check to see which version of armasm we've got (first on PATH).
# if its the old one then copy the file specified by $old to $dest
# else copy $src to $dest
# 
#

use strict;
use File::Copy;

my ($dest, $old, $src) = @ARGV;

my $oldVersionId = "ARM AOF Macro Assembler vsn 2.37 (Advanced RISC Machines SDT 2.11) [Sep  9 1997]";

open ARMASM, "armasm -help|";

my $id = <ARMASM>;

chop $id;

close ARMASM;

$src = $old if ($id eq $oldVersionId);

unlink $dest;

copy("$src", "$dest");

