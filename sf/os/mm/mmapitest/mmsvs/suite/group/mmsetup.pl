#
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
#

use Cwd;

my	$directory=cwd;
my	$platform="$ARGV[0]";
my	$build="$ARGV[1]";

chdir "\\src\\common\\generic\\multimedia\\ecam\\plugins\\group";
system("bldmake bldfiles");
system("abld test build $platform $build testcamera");

chdir "\\src\\common\\generic\\multimedia\\testframework\\group";
system("bldmake bldfiles");
system("abld build $platform $build TestFrameworkServer");
system("abld build $platform $build TestFrameworkClient");

chdir "\\src\\common\\generic\\multimedia\\unittest\\mmf\\MidiClnt\\MidiTestCntrl\\group";
system("bldmake bldfiles");
system("abld build $platform $build miditstcntrl");

chdir "$directory";
