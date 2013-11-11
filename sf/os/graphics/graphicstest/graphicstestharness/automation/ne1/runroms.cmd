rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphicsresource_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 120 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics0a_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 120 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics0c_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 120 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics1_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 90 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics1a_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics2_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 120 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics3_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics4_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 120 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics4ct_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 120 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics5a_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 240 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics5b_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_uibench_s60_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics7_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 90 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics8_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 150 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics9_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 150 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics12_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 120 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics13_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 45 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics15_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics16_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 45 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics17_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 45 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics18_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphicseglhybrid_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphicseglref_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics23_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 3600 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics24_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 120 --listenTimeout 3600 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics25_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 3600 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics29a_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 3600 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics29b_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 3600 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_graphics30_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 60 --listenTimeout 2400 --transferMode serial
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/naviengine_uiframeworks_armv5_auto" --nandLoader %1 --resource %2 --jobId %3 --timeout 80 --listenTimeout 2400 --transferMode serial

