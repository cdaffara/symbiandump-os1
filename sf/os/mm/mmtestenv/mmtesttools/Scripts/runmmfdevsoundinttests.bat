rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

del e:\logs\testresults\tsi_mmf_devsound_debug.htm
del e:\logs\testresults\tsi_mmf_devsound_alloc.htm
md e:\logs\
md e:\logs\testresults\

REM ***********************************************
REM **************** DEVSOUND Tests ***************
REM ***********************************************

md c:\mm\
md c:\tsi_mmf_devsound\
md c:\tsi_mmf_devsound\output\

copy z:\tsi_mmf_devsound.ini c:\mm\tsi_mmf_devsound.ini

copy e:\inttest\mmf\tsi_mmf_devsound\small.ogg c:\tsi_mmf_devsound\small.ogg
copy e:\inttest\mmf\tsi_mmf_devsound\output\small.ogg c:\tsi_mmf_devsound\output\small.ogg
copy e:\inttest\mmf\tsi_mmf_devsound\corrupt.ogg c:\tsi_mmf_devsound\corrupt.ogg
copy e:\inttest\mmf\tsi_mmf_devsound\output\corrupt.ogg c:\tsi_mmf_devsound\output\corrupt.ogg
copy e:\inttest\mmf\tsi_mmf_devsound\big.ogg c:\tsi_mmf_devsound\big.ogg

testframework tsi_mmf_devsound_debug.script
move c:\logs\testresults\*.htm e:\logs\testresults\

testframework tsi_mmf_devsound_alloc.script
move c:\logs\testresults\*.htm e:\logs\testresults\

REM Move the test files and generated record files back to the MMC
move c:\tsi_mmf_devsound\output\corrupt.ogg e:\inttest\mmf\tsi_mmf_devsound\output\corrupt.ogg
move c:\tsi_mmf_devsound\output\small.ogg e:\inttest\mmf\tsi_mmf_devsound\output\small.ogg
move c:\tsi_mmf_devsound\output\output_0008.ogg e:\inttest\mmf\tsi_mmf_devsound\output\output_0008.ogg
move c:\tsi_mmf_devsound\output\output_0101.ogg e:\inttest\mmf\tsi_mmf_devsound\output\output_0101.ogg
move c:\tsi_mmf_devsound\output\output_0102.ogg e:\inttest\mmf\tsi_mmf_devsound\output\output_0102.ogg
move c:\tsi_mmf_devsound\output\output_0103.ogg e:\inttest\mmf\tsi_mmf_devsound\output\output_0103.ogg
move c:\tsi_mmf_devsound\output\output_0104.ogg e:\inttest\mmf\tsi_mmf_devsound\output\output_0104.ogg
move c:\tsi_mmf_devsound\output\output_0105.ogg e:\inttest\mmf\tsi_mmf_devsound\output\output_0105.ogg
move c:\tsi_mmf_devsound\output\output_0604.ogg e:\inttest\mmf\tsi_mmf_devsound\output\output_0604.ogg

del c:\tsi_mmf_devsound\output\*.*
del c:\tsi_mmf_devsound\*.*
del c:\mm\*.*
