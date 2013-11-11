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

#include "TestGlobalEffectPlugin.h"

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "../src/TestGlobalEffect.hrh"

const TUid KUidTestGlbCustomCall = {KUidTestGlbCustomCallDefine};
const TInt KPresetsCount = 3;

//
// CTestEffect
//

CTestEffect::CTestEffect()
	{
	}
	
CTestEffect::~CTestEffect()
	{
	delete iSettingsDes;
	delete iPresetList;
	REComSession::DestroyedImplementation(iDestructorKey);
	}
	
MMmfGlobalAudioImpl* CTestEffect::NewL()
	{
	return new (ELeave) CTestEffect;
	}

void CTestEffect::Release()
	{
	// this is effectively delete
	delete this;
	}
	
TUint CTestEffect::Capability(TBool aCurrentOnly)
	{
	// not totally clear what to do for testing purposes, so just return 0 or -1 depending on param
	return aCurrentOnly ? 0xffff : 0;
	}
	
void CTestEffect::RequestNotificationL(TUid aEventUid)
	{
	// for test, just remember the pass event uid and use in test GenCallbackL() call
	iNotifyUid = aEventUid;
	}
	
void CTestEffect::GenCallbackL()
	{
	iObserver->GAEEventNotificationL(iParent, iNotifyUid, KNullDesC8);
	}
	
TBool CTestEffect::IsEnabled() const
	{
	return iEnabled;
	}
	
void CTestEffect::SetEnabledL(TBool aBool)
	{
	iEnabled = aBool;
	}
	
TBool CTestEffect::IsActive() const
	{
	return iIsActive;
	}
	
void CTestEffect::SetActive(TBool aValue)
	{
	iIsActive = aValue;
	}
	
TUid CTestEffect::SettingsByUidL() const
	{
	if (iValueState!=ESetByUid)
		{
		User::Leave(KErrNotSupported);
		}
	return iSettingsUid;
	}
	
void CTestEffect::SetSettingsByUidL(TUid aPresetUid)
	{
	iSettingsUid = aPresetUid;
	iValueState = ESetByUid;
	}
	
HBufC8* CTestEffect::SettingsByDesL() const
	{
	// for test, if we've not set via Des then return default value - except for SetByValue
	_LIT8(KDefaultDes, "1234");
	if (iValueState==ESetByDes)
		{
		return iSettingsDes->AllocL();
		}
	else if (iValueState!=ESetByValue)
		{
		return KDefaultDes().AllocL();
		}
	else
		{
		User::Leave(KErrNotSupported);
		return NULL;
		}
	}
	
void CTestEffect::SetSettingsByDesL(const TDesC8& aParam)
	{
	delete iSettingsDes;
	iSettingsDes = NULL;
	iSettingsDes = aParam.Alloc();
	iValueState = ESetByDes;
	}
	
MMmfGlobalAudioPresetList* CTestEffect::KnownPresetsL()
	{
	delete iPresetList;
	iPresetList = NULL;
	iPresetList = CPresetList::NewL();
	
	// produce list of num in numeric and readable form, upto count
	for (TInt index=0; index<KPresetsCount; index++)
		{
		TBuf<10> name;
		name.Num(index);
		TUid tempUid={index};
		iPresetList->AppendL(tempUid, name);
		}
	return iPresetList;
	}
	
void CTestEffect::ExtractValuesL(TDes8& aPackageBuf)
	{
	if (iValueState!=ESetByValue)
		{
		User::LeaveIfError(KErrNotSupported);
		}
	TPckgC<TInt> packageBuf(iSettingsValue);
	aPackageBuf = packageBuf;
	}
	
void CTestEffect::SetByValuesL(const TDesC8& aPackageBuf)
	{
	TPckg<TInt> packageBuf(iSettingsValue);
	packageBuf.Copy(aPackageBuf);
	iValueState = ESetByValue;
	}
	
TInt CTestEffect::CreateCustomInterface(TUid aInterfaceUid)
	{
	if (aInterfaceUid==KUidTestGlbCustomCall)
		{
		// we derive from this anyway, so just return KErrNone to say supported
		return KErrNone;
		}
	return KErrNotSupported;
	}


TAny* CTestEffect::CustomInterface(TUid aInterfaceUid)
	{
	if (aInterfaceUid==KUidTestGlbCustomCall)
		{
		MTestEffectCustomIf* castedThis = this;
		return castedThis;
		}
	return NULL;
	}
		
void CTestEffect::PassDestructorKey(TUid aDestructorKey)
	{
	iDestructorKey = aDestructorKey;
	}
	
void CTestEffect::CompleteConstructL(CMmfGlobalAudioEffect* aParent, MMmfGlobalAudioEffectObserver* aObserver)
	{
	iParent = aParent;
	iObserver = aObserver;
	}




// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KUidTestGlblPluginImpDefine,	CTestEffect::NewL),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

