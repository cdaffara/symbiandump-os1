/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/





#ifndef __MMFDEVSOUNDSERVERSTART_H
#define __MMFDEVSOUNDSERVERSTART_H

#include <e32std.h>
#include "mmfaudioserver.h"


_LIT(KDevSoundServerName,"!MMFDevSndSvr");
const TInt KDevSoundServerStackSize=0x2000;			//  8KB
const TInt KDevSoundServerInitHeapSize=0x1000;		//  4KB
const TInt KDevSoundServerMaxHeapSize=0x1000000;		// 16MB

const TUint KMMFDevSoundServerVersion=8;
const TUint KMMFDevSoundServerMinorVersionNumber=0;
const TUint KMMFDevSoundServerBuildVersionNumber=1;


/**
	A utility class used by Audio Server to pass data to DevSound server thread.
	
	@lib MmfDevSoundServer.lib
	@since 
*/
class TDevSoundServerStart
	{
public:
	TDevSoundServerStart(CMMFAudioServer* aAudioServer, const TProcessId& aProcessId, RServer2& aDevSoundServerHandle);
		
public:
	TProcessId iProcessId;
	CMMFAudioServer* iAudioServer;
	RServer2& iDevSoundServerHandle;
	};

#include "mmfdevsoundserverstart.inl"

#endif
