
// GlobalEffectTest.cpp

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

#include "TestEffect.h"
#include "TestGlobalEffect.hrh"
#include "TestEffectCustom.h"

const TUid KUidTestGlblPlugin = {KUidTestGlblPluginDefine};
const TUid KUidTestGlbCustomCall = {KUidTestGlbCustomCallDefine};

//
// TestGlobalEffect - note normally this will be in a separate DLL to the calling code,
//                    but seems no point in doing that for this test code
//

CTestGlobalEffect* CTestGlobalEffect::NewL(MMmfGlobalAudioEffectObserver* aObserver)
	{
	CTestGlobalEffect* self = new (ELeave) CTestGlobalEffect;
	CleanupStack::PushL(self);
	self->ConstructL(aObserver);
	CleanupStack::Pop(self);
	return self;
	}
	
CTestGlobalEffect::CTestGlobalEffect()
	{
	// empty constructor
	}
	
void CTestGlobalEffect::ConstructL(MMmfGlobalAudioEffectObserver* aObserver)
	{
	BaseConstructL(KUidTestGlblPlugin, aObserver);
	User::LeaveIfError(CreateCustomInterface(KUidTestGlbCustomCall));
	}
	
void CTestGlobalEffect::SetActive(TBool aValue)
	{
	MTestEffectCustomIf* custIf = 
		static_cast<MTestEffectCustomIf*>(CustomInterface(KUidTestGlbCustomCall));
	custIf->SetActive(aValue);
	}
	
void CTestGlobalEffect::GenCallbackL()
	{
	MTestEffectCustomIf* custIf = 
		static_cast<MTestEffectCustomIf*>(CustomInterface(KUidTestGlbCustomCall));
	custIf->GenCallbackL();
	}
	
