
// MmfGlblAudioEffect.cpp

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

//
// CMmfGlobalAudioEffect
//

EXPORT_C TUint CMmfGlobalAudioEffect::Capability(TBool aCurrentOnly)
	{
	return iBaseImplementation->Capability(aCurrentOnly);
	}
	
EXPORT_C void CMmfGlobalAudioEffect::RequestNotificationL(TUid aEventUid)
	{
	iBaseImplementation->RequestNotificationL(aEventUid);
	}
	
EXPORT_C TBool CMmfGlobalAudioEffect::IsEnabled() const
	{
	return iBaseImplementation->IsEnabled();
	}
	
EXPORT_C TBool CMmfGlobalAudioEffect::IsActive() const
	{
	return iBaseImplementation->IsActive();
	}
	
EXPORT_C void CMmfGlobalAudioEffect::SetEnabledL(TBool aBool)
	{
	iBaseImplementation->SetEnabledL(aBool);
	}
	
EXPORT_C TUid CMmfGlobalAudioEffect::SettingsByUidL() const
	{
	return iBaseImplementation->SettingsByUidL();
	}
	
EXPORT_C void CMmfGlobalAudioEffect::SetSettingsByUidL(TUid aPresetUid)
	{
	iBaseImplementation->SetSettingsByUidL(aPresetUid);
	}
	
EXPORT_C HBufC8* CMmfGlobalAudioEffect::SettingsByDesL() const
	{
	return iBaseImplementation->SettingsByDesL();
	}
	
EXPORT_C void CMmfGlobalAudioEffect::SetSettingsByDesL(const TDesC8& aParam)
	{
	iBaseImplementation->SetSettingsByDesL(aParam);
	}
	
EXPORT_C MMmfGlobalAudioPresetList* CMmfGlobalAudioEffect::KnownPresetsL()
	{
	return iBaseImplementation->KnownPresetsL();
	}
	
EXPORT_C void CMmfGlobalAudioEffect::ExtractValuesL(TDes8& aPackageBuf)
	{
	iBaseImplementation->ExtractValuesL(aPackageBuf);
	}
	
EXPORT_C void CMmfGlobalAudioEffect::SetByValuesL(const TDesC8& aPackageBuf)
	{
	iBaseImplementation->SetByValuesL(aPackageBuf);
	}
	
EXPORT_C CMmfGlobalAudioEffect::CMmfGlobalAudioEffect()
	{
	// nothing to construct
	}
	
EXPORT_C CMmfGlobalAudioEffect::~CMmfGlobalAudioEffect()
	{
	if (iBaseImplementation)
		{
		iBaseImplementation->Release();
		}
	}
	
EXPORT_C void CMmfGlobalAudioEffect::BaseConstructL(TUid aImplementationUid, MMmfGlobalAudioEffectObserver* aObserver)
	{
	iBaseImplementation = MMmfGlobalAudioImpl::NewL(aImplementationUid, this, aObserver);
	}
	
EXPORT_C TInt CMmfGlobalAudioEffect::CreateCustomInterface(TUid aInterfaceUid)
	{
	return iBaseImplementation->CreateCustomInterface(aInterfaceUid);
	}
	
EXPORT_C TAny* CMmfGlobalAudioEffect::CustomInterface(TUid aInterfaceUid)
	{
	return iBaseImplementation->CustomInterface(aInterfaceUid);
	}
