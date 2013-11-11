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
rem ********** Creating DRM archives *********************
rem must be executed prior to any DRM test on a target hardware
rem 
rem

del c:\ICL\TestImages\Image.contentrights
del c:\ICL\TestImages\Image.content
del c:\ICL\TestImages\ImageContentNoRights.content
del c:\ICL\TestImages\SingleImage.content
del c:\ICL\TestImages\SingleImage.contentrights

del c:\AclntITestData\Audio.contentrights
del c:\AclntITestData\Audio.content
del c:\AclntITestData\AudioContentNoRights.content

del c:\AclntITestData\AudioOgg.contentrights
del c:\AclntITestData\AudioOgg.content
del c:\AclntITestData\AudioOggContentNoRights.content

del c:\MidiClntITestData\Midi.contentrights
del c:\MidiClntITestData\Midi.content

del c:\mm\mmf\testfiles\vclnt\VideoContentNoRights.content 
del c:\mm\mmf\testfiles\vclnt\Video.contentrights
del c:\mm\mmf\testfiles\vclnt\Video.content

del c:\tsu_icl_imgdisp_01\ImgDispl.contentrights
del c:\tsu_icl_imgdisp_01\ImgDispl.content

del c:\mm\mmf\testfiles\aclnt\SingleTone.contentrights
del c:\mm\mmf\testfiles\aclnt\SingleTone.content

md c:\rta\
copy e:\rta\*.* c:\rta\

md c:\logs\
md c:\logs\rta\
md c:\logs\testexecute\

testexecute c:\rta\rta.script

del c:\rta\*.*

md e:\logs\
md e:\logs\rta\
del e:\logs\rta\*.*

move c:\logs\rta\*.* e:\logs\rta\

md e:\logs\testexecute\
del e:\logs\testexecute\rta.htm
del e:\logs\testexecute\testresults.htm

move c:\logs\testexecute\rta.htm e:\logs\testexecute\rta.htm
move c:\logs\testexecute\testresults.htm e:\logs\testexecute\testresults.htm
