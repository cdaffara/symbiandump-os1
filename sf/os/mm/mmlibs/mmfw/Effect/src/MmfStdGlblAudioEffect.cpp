
// MmfStdGlblAudioEffect.cpp

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

#include <mmfstdglblaudioeffect.h>
#include <mmf/plugin/mmfglblaudioeffect.hrh>

// Define standard UIDs

// const TUid KUidMmfGlblVolumePlugin = {KUidMmfGlblVolumePluginDefine};
const TUid KUidMmfGlblEqPlugin = 	 {KUidMmfGlblEqPluginDefine};
const TUid KUidMmfGlblBoostPlugin =  {KUidMmfGlblBoostPluginDefine};

//
// CMmfGlobalEqEffect
//

EXPORT_C CMmfGlobalEqEffect* CMmfGlobalEqEffect::NewL(MMmfGlobalAudioEffectObserver* aObserver)
	{
	CMmfGlobalEqEffect* self = new (ELeave) CMmfGlobalEqEffect;
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	CleanupStack::Pop(self);
	return self;
	}
	
CMmfGlobalEqEffect::CMmfGlobalEqEffect()
	{
	// empty constructor
	}
	
void CMmfGlobalEqEffect::ConstructL(MMmfGlobalAudioEffectObserver* aObserver)
	{
	BaseConstructL(KUidMmfGlblEqPlugin, aObserver);
	}
	
//
// CMmfGlobalMegaEffect
//

EXPORT_C CMmfGlobalMegaEffect* CMmfGlobalMegaEffect::NewL(MMmfGlobalAudioEffectObserver* aObserver)
	{
	CMmfGlobalMegaEffect* self = new (ELeave) CMmfGlobalMegaEffect;
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	CleanupStack::Pop(self);
	return self;
	}
	
CMmfGlobalMegaEffect::CMmfGlobalMegaEffect()
	{
	// empty constructor
	}
	
void CMmfGlobalMegaEffect::ConstructL(MMmfGlobalAudioEffectObserver* aObserver)
	{
	BaseConstructL(KUidMmfGlblBoostPlugin, aObserver);
	}
