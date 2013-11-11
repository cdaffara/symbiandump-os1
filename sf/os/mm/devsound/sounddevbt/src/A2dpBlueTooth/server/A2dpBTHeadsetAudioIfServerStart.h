// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __A2DPBTHEADSETAUDIOIFSERVERSTART_H__
#define __A2DPBTHEADSETAUDIOIFSERVERSTART_H__

#include <e32std.h>

// Server name
_LIT(KA2DPAudioServerName, "!A2DPBTAudioServer");

const TInt KBTAudioServerStackSize=0x2000;			//  8KB
const TInt KBTAudioServerInitHeapSize=0x1000;		//  4KB
const TInt KBTAudioServerMaxHeapSize=0x1000000;		// 16MB

// A version must be specified when creating a session with the server
const TUint KBTAudioServerMajorVersionNumber=0;
const TUint KBTAudioServerMinorVersionNumber=1;
const TUint KBTAudioServerBuildVersionNumber=1;

#endif
