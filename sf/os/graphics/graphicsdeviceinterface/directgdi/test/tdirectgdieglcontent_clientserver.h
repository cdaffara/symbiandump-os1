// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TDIRECTGDIEGLCONTENT_CLIENTSERVER_H__
#define __TDIRECTGDIEGLCONTENT_CLIENTSERVER_H__

#include <e32std.h>

_LIT(KEglContentServerName, "tdirectgdieglcontent_server");
const TUid KServerUid3 = {0x10285A76};

const TInt KEglContentAsyncMaxFrames = 10; // number of unique frames in async mode

/**
Message types of egl content server.
*/
enum TEglContentMessages
	{
	ETerminateServer,	// terminate server process
	EGetSyncImage,		// switch renderer to sync mode and get image
	EGetAsyncImage,		// switch renderer to async mode and get image
	EGetAsyncImageDebug	// switch renderer to async debug mode and get image
	};

#endif


