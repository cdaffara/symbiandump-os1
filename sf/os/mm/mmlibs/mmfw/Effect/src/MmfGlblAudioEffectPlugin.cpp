
// MmfGlblAudioEffectPlugin.cpp

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

#include <mmf/common/mmfglblaudioeffect.h>
#include <mmf/plugin/mmfglblaudioeffectplugin.h>
#include <mmf/plugin/mmfglblaudioeffect.hrh>

#include <ecom/ecom.h>
#include <mm/mmpluginutils.h>

#include <mmf/common/mmfcontroller.h> //needed for CleanupResetAndDestroyPushL()

// construct implementation of global plugin

MMmfGlobalAudioImpl* MMmfGlobalAudioImpl::NewL(TUid aImplementationUid, 
							   CMmfGlobalAudioEffect* aParent,
	                           MMmfGlobalAudioEffectObserver* aObserver)
	{
	// The Uid of the plugin will be the match string
	TInt uidAsInteger = aImplementationUid.iUid;
	TBuf8<20> tempBuffer;
	tempBuffer.Num(uidAsInteger, EHex); // has value
	TUid interfaceUid = {KUidGlobalAudioEffectInterface};

	TUid destructorKey;
	MMmfGlobalAudioImpl* self = 
		static_cast<MMmfGlobalAudioImpl*>
			(MmPluginUtils::CreateImplementationL(interfaceUid, destructorKey, tempBuffer, KRomOnlyResolverUid));

	self->PassDestructorKey(destructorKey);

	CleanupReleasePushL(*self);
	self->CompleteConstructL(aParent, aObserver);
	CleanupStack::Pop(self);
	
	return self;
	}
