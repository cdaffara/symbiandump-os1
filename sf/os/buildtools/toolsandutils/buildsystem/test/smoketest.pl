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
# run smoke tests for the buildsystem
#
# each test should use the Test module
#
# Failing tests can be re-run individually in their own folder
# if you need more detailed output to determine the fault.

use strict;
use Test::Harness;
$Test::Harness::Verbose = 1;

# a list of all the tests.
my @all = ( 
'helloworld/test.pl' ,
'binaryvariation/test.pl' ,
'extensions/test.pl'
);

runtests(@all);
exit 0;

