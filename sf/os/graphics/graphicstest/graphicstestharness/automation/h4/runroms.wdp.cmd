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

call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics0a_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics0b_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics0c_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics1_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics1a_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics2_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics3_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics3a_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600 --sendFiles "stress_tests,stress_tests;functional_tests,functional_tests" --retrieveFiles "functional_results_copy/test_results,functional_results/test_results;functional_tests,functional_results/functional_tests;stress_results_copy/test_results,stress_results/test_results"
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics4_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400 --timeout 240
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics4ct_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400 --timeout 240
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics5a_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics5b_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_uibench_s60_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics7_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics8_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400 --timeout 240
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics9_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics12_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics13_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics15_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics16_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics17_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics18_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphicseglhybrid_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphicseglref_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics19_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics20_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics21_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics22_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics23_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics24_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics25_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics29a_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics29b_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 3600
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphics30_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_uiframeworks_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400
call ec-perl w:\scripts\rom\runrom.pl --romDirectory "roms/h4hrp_graphicsresource_armv5_wdp_auto" --nandLoader %1 --resource %2 --jobId %3 --listenTimeout 2400

