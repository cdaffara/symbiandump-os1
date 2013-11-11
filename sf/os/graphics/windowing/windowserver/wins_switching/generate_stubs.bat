REM generate_stubs.bat
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
perl generate_stubs.pl ws322u.def ws32_stubs.h Ws32SwitchU.def
perl generate_stubs.pl remotegcu.def remotegc_stubs.h RemoteGcSwitchU.def
perl generate_stubs.pl graphicdraweru.def wsgraphicdrawer_stubs.h WsGraphicDrawerSwitchU.def
