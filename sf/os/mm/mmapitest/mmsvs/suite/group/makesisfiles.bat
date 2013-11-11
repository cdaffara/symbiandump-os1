@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

copy ..\release\armv5\urel\T_Camera.exe .
copy ..\release\armv5\urel\T_ImageDecoder.exe .
copy ..\release\armv5\urel\T_ImageEncoder.exe .
copy ..\release\armv5\urel\T_MdaAudioConvertUtility.exe .
copy ..\release\armv5\urel\T_MdaAudioInputStream.exe .
copy ..\release\armv5\urel\T_MdaAudioOutputStream.exe .
copy ..\release\armv5\urel\T_MdaAudioPlayerUtility.exe .
copy ..\release\armv5\urel\T_MdaAudioRecorderUtility.exe .
copy ..\release\armv5\urel\T_MdaAudioToneUtility.exe .
copy ..\release\armv5\urel\T_MidiClientUtility.exe .
copy ..\release\armv5\urel\T_VideoPlayerUtility.exe .
copy ..\release\armv5\urel\T_VideoRecorderUtility.exe .
md t_imagedecoder
copy ..\data\z\multimedia\t_imagedecoder\dec1bit.mbm .\t_imagedecoder
copy ..\data\z\multimedia\t_imagedecoder\dec2bit.mbm .\t_imagedecoder
copy ..\data\z\multimedia\t_imagedecoder\dec4bit.mbm .\t_imagedecoder
copy ..\data\z\multimedia\t_imagedecoder\dec8bit.mbm .\t_imagedecoder
copy ..\data\z\multimedia\t_imagedecoder\dec16bit.mbm .\t_imagedecoder
copy ..\data\z\multimedia\t_imagedecoder\dec24bit.mbm .\t_imagedecoder
md t_imageencoder
copy ..\data\z\multimedia\t_imageencoder\enc1bit.mbm .\t_imageencoder
copy ..\data\z\multimedia\t_imageencoder\enc2bit.mbm .\t_imageencoder
copy ..\data\z\multimedia\t_imageencoder\enc4bit.mbm .\t_imageencoder
copy ..\data\z\multimedia\t_imageencoder\enc8bit.mbm .\t_imageencoder
copy ..\data\z\multimedia\t_imageencoder\enc16bit.mbm .\t_imageencoder
copy ..\data\z\multimedia\t_imageencoder\enc24bit.mbm .\t_imageencoder
call makesis t_camera.pkg t_camera.sis
call makesis t_imagedecoder.pkg t_imagedecoder.sis
call makesis t_imageencoder.pkg t_imageencoder.sis
call makesis t_mdaaudioconvertutility.pkg t_mdaaudioconvertutility.sis
call makesis t_mdaaudioinputstream.pkg t_mdaaudioinputstream.sis
call makesis t_mdaaudiooutputstream.pkg t_mdaaudiooutputstream.sis
call makesis t_mdaaudioplayerutility.pkg t_mdaaudioplayerutility.sis
call makesis t_mdaaudiorecorderutility.pkg t_mdaaudiorecorderutility.sis
call makesis t_mdaaudiotoneutility.pkg t_mdaaudiotoneutility.sis
call makesis t_midiclientutility.pkg t_midiclientutility.sis
call makesis t_videoplayerutility.pkg t_videoplayerutility.sis
call makesis t_videorecorderutility.pkg t_videorecorderutility.sis
call signsis -s t_camera.sis t_camera.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_imagedecoder.sis t_imagedecoder.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_imageencoder.sis t_imageencoder.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_mdaaudioconvertutility.sis t_mdaaudioconvertutility.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_mdaaudioinputstream.sis t_mdaaudioinputstream.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_mdaaudiooutputstream.sis t_mdaaudiooutputstream.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_mdaaudioplayerutility.sis t_mdaaudioplayerutility.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_mdaaudiorecorderutility.sis t_mdaaudiorecorderutility.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_mdaaudiotoneutility.sis t_mdaaudiotoneutility.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_midiclientutility.sis t_midiclientutility.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_videoplayerutility.sis t_videoplayerutility.sis zeus.cer SymbianACS.key caforstat
call signsis -s t_videorecorderutility.sis t_videorecorderutility.sis zeus.cer SymbianACS.key caforstat
