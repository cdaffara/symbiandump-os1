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

call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphicsresource_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics0a_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics0b_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics0c_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics1_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics1a_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics2_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics3_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics3a_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600 --sendFiles "stress_tests,stress_tests;functional_tests,functional_tests" --retrieveFiles "functional_results_copy/test_results,functional_results/test_results;functional_tests,functional_results/functional_tests;stress_results_copy/test_results,stress_results/test_results"
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics4_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics4ct_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics5a_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics5b_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_uibench_s60_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics7_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics8_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics9_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics12_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics13_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics15_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics16_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics17_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics18_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphicseglhybrid_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphicseglref_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics23_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics24_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics25_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics29a_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics29b_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_graphics30_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call  ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/34xx_sdp_uiframeworks_armv5_dpdef_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600

