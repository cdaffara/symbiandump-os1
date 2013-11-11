
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

#ifndef TESTGLOBALEFFECTPLUGIN_H
#define TESTGLOBALEFFECTPLUGIN_H

#include <mmf/plugin/mmfglblaudioeffectplugin.h>

#include "../src/TestEffectCustom.h"
#include "PresetList.h"

// Test global audio effect - does nothing but supports setting, getting etc

class CTestEffect : public CBase, 
                    public MMmfGlobalAudioImpl,
                    public MTestEffectCustomIf
	{
public:
	static MMmfGlobalAudioImpl* NewL();
	~CTestEffect();
	
	enum TValueState
		{
		ESetByUnknown,
		ESetByUid,
		ESetByDes,
		ESetByValue
		};
	
protected:
	// from MMmfGlobalAudioImpl
	void Release();
	TUint Capability(TBool aCurrentOnly);
	void RequestNotificationL(TUid aEventUid);
	TBool IsEnabled() const;
	TBool IsActive() const;
	void SetEnabledL(TBool aBool);
	TUid SettingsByUidL() const;
	void SetSettingsByUidL(TUid aPresetUid);
	HBufC8* SettingsByDesL() const;
	void SetSettingsByDesL(const TDesC8& aParam);
	MMmfGlobalAudioPresetList* KnownPresetsL();
	void ExtractValuesL(TDes8& aPackageBuf);
	void SetByValuesL(const TDesC8& aPackageBuf);
	TInt CreateCustomInterface(TUid aInterfaceUid);
	TAny* CustomInterface(TUid aInterfaceUid);
	void PassDestructorKey(TUid aDestructorKey);
	void CompleteConstructL(CMmfGlobalAudioEffect* aParent, MMmfGlobalAudioEffectObserver* aObserver);
	
	// from MTestEffectCustomIf
	void SetActive(TBool aValue);
	void GenCallbackL();
	
private:
	CTestEffect();
private:
	TUid iDestructorKey;
	CMmfGlobalAudioEffect* iParent;
	MMmfGlobalAudioEffectObserver* iObserver;
	TUid iSettingsUid;
	HBufC8* iSettingsDes;
	TInt iSettingsValue;
	TValueState iValueState;
	TBool iEnabled;
	TBool iIsActive;
	CPresetList* iPresetList;
	TUid iNotifyUid;
	};
	
#endif
