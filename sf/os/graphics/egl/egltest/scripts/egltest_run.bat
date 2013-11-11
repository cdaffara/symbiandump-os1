@rem
@rem Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

testexecute.exe z:\egltest\egltest_t_geterroranddisplay.script
testexecute.exe z:\egltest\egltest_t_general.script
testexecute.exe z:\egltest\egltest_t_syncobject.script
testexecute.exe z:\egltest\egltest_t_syncobject_flush.script
testexecute.exe z:\egltest\egltest_t_nativepixmaptype.script
testexecute.exe z:\egltest\egltest_t_createpixmapsurface.script
testexecute.exe z:\egltest\egltest_t_image.script
testexecute.exe z:\egltest\egltest_t_sibling.script
testexecute.exe z:\egltest\egltest_t_image_multithread.script
testexecute.exe z:\egltest\egltest_t_image_multiprocess.script
testexecute.exe z:\egltest\egltest_t_vgimagetosurfaces.script
testexecute.exe z:\egltest\egltest_t_image_negative.script
testexecute.exe z:\egltest\egltest_t_benchmark_sgimage.script
testexecute.exe z:\egltest\egltest_t_oom_sgimage.script
testexecute.exe z:\egltest\egltest_t_stress_sgimage.script
testexecute.exe z:\egltest\egltest_t_benchmark_swapbuffers.script
testexecute.exe z:\egltest\egltest_t_surfacescaling.script

:: Ignore the following if using this script for emulator testing.
:: Logs are automatically copied on hardware to MMC
:: which is assumed to be drive E:
@echo off
if not exist c:\logs\testexecute\ goto :EOF
md e:\logs
md e:\logs\testexecute
copy c:\logs\testexecute\egltest*.htm e:\logs\testexecute\
