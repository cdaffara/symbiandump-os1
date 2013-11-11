// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __MMFBTDEVSOUNDSERVERSTART_H
#define __MMFBTDEVSOUNDSERVERSTART_H

#include <e32std.h>
#include "MmfBtAudioServer.h"


_LIT(KDevSoundServerName,"!MMFDevSndSvr");
const TInt KDevSoundServerStackSize=0x2000;			//  8KB
const TInt KDevSoundServerInitHeapSize=0x1000;		//  4KB
const TInt KDevSoundServerMaxHeapSize=0x1000000;		// 16MB

const TUint KMMFDevSoundServerVersion=8;
const TUint KMMFDevSoundServerMinorVersionNumber=0;
const TUint KMMFDevSoundServerBuildVersionNumber=1;


class TDevSoundServerStart
/**
*@internalTechnology
*/
	{
public:
	TDevSoundServerStart(CMMFAudioServer* aAudioServer, const TProcessId& aProcessId);
	TDevSoundServerStart();
	
public:
	TProcessId iProcessId;
	CMMFAudioServer* iAudioServer;
	};

#include "MmfBtDevSoundServerStart.inl"

#endif
