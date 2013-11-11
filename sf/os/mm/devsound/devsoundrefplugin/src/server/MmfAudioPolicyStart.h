// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __MMFAUDIOPOLICYSTART_H
#define __MMFAUDIOPOLICYSTART_H

#include <e32std.h>

_LIT(KAudioPolicyServerName,"!MMFAudioPolicy");
const TInt KAudioPolicyServerStackSize=0x2000;			//  8KB
const TInt KAudioPolicyServerInitHeapSize=0x1000;		//  4KB
const TInt KAudioPolicyServerMaxHeapSize=0x1000000;		// 16MB


const TUint KMMFAudioPolicyVersion=8;
const TUint KMMFAudioPolicyMinorVersionNumber=0;
const TUint KMMFAudioPolicyBuildVersionNumber=1;


class TServerStart
/**
*@internalTechnology
*/
	{
public:
	TServerStart(RServer2& aPolicyServerHandle);
public:
	RServer2& iPolicyServerHandle;
	};

#include "MmfAudioPolicyStart.inl"
#endif

