// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file
 @test
 @internalComponent
*/

#ifndef __T_WSERV_GENERICPLUGIN_CONSTS_H__
#define __T_WSERV_GENERICPLUGIN_CONSTS_H__

struct TFadingParams
    {
    TUint8 blackMap;
    TUint8 whiteMap;
    };
    
//Return value of the CheckLogL.
enum TCheckLogStatus
		{
		EFileNotExist = -1,
		EMsgNotFound = -2,
		EMsgFound = 1
		};		

static const TUint KDelay=1000000;	//1 sec

//Default value for the width and height of the window created.
const TInt KTempWindowHeight = 200;
const TInt KTempWindowWidth = 200;

const TUint32 KNullWsHandle = 0xFFFFFFFF;
const TUint KBadPluginUid = 0x10285fff;
#ifdef GRAPHICS_TEST_PLUGIN_NGA
const TUint KServicePluginUid = 0x10285C64; //service plugin UID
#else //GRAPHICS_TEST_PLUGIN_NGA
const TUint KServicePluginUid = 0x10285D5D;
#endif //GRAPHICS_TEST_PLUGIN_NGA

//Name of the log file to which test plugins write message.
_LIT(KLogFileName, "c:\\graphics\\testplugin.txt");

//Name of the test step
_LIT(KT_WServGenericpluginStepLoad,"T_WServGenericpluginStepLoad");

#endif // __T_WSERV_GENERICPLUGIN_CONSTS_H__
