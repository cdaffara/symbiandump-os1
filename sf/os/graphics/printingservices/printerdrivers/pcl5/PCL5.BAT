REM PCL5.BAT
REM Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
REM All rights reserved.
REM This component and the accompanying materials are made available
REM under the terms of "Eclipse Public License v1.0"
REM which accompanies this distribution, and is available
REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
REM 
REM Initial Contributors:
REM Nokia Corporation - initial contribution.
REM 
REM Contributors:
REM 
REM Description:
REM 
rem pdrtran lane.pd
pdrtran pcl5.pd pcl5.pdr
copy pcl5.pdr \epoc32\release\wins\urel\z\system\printers\pcl5.pdr
copy pcl5.pdr \epoc32\release\wins\udeb\z\system\printers\pcl5.pdr
copy pcl5.pdr \epoc32\release\marm\urel\pcl5.pdr
copy pcl5.pdr \epoc32\release\marm\udeb\pcl5.pdr
