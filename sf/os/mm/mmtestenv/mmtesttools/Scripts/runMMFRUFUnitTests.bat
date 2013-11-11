rem Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


del E:\Logs\TestResults\tsu_mmf_mmruf_badext.htm
del E:\Logs\TestResults\tsu_mmf_mmruf_corrupt.htm
del E:\Logs\TestResults\tsu_mmf_mmruf.htm
del E:\Logs\TestResults\tsu_mmf_mmruf_alloc.htm
md E:\Logs\
md E:\Logs\TestResults\

REM ***********************************************
REM **************** RUF TESTS ******************
REM ***********************************************

md c:\recog\
md c:\recog\common\

copy e:\recog\*.wxt c:\recog\
copy e:\recog\common\* c:\recog\common\

testframeworkrecognizer tsu_mmf_mmruf_badext.script
move c:\Logs\TestResults\tsu_mmf_mmruf*.htm e:\Logs\TestResults\

del c:\recog\*

copy e:\recog\corrupt*.* c:\recog\

testframeworkrecognizer tsu_mmf_mmruf_corrupt.script
move c:\Logs\TestResults\tsu_mmf_mmruf*.htm e:\Logs\TestResults\

del c:\recog\*

copy e:\recog\valid_* c:\recog\

testframeworkrecognizer tsu_mmf_mmruf.script
move c:\Logs\TestResults\tsu_mmf_mmruf*.htm e:\Logs\TestResults\

del c:\recog\*

copy e:\recog\valid_angry_bone.3gp  c:\recog\
copy e:\recog\3gpWidWrngExt.wxt  c:\recog\
copy e:\recog\corrupt_vid.3gp  c:\recog\

testframeworkrecognizer tsu_mmf_mmruf_alloc.script
move c:\Logs\TestResults\tsu_mmf_mmruf*.htm e:\Logs\TestResults\

del c:\recog\common\*
del c:\recog\*

