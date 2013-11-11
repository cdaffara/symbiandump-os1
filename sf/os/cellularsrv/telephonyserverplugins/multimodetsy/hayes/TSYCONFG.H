// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSY Configuration Class Header
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __TSYCONFG_H
#define __TSYCONFG_H

#include <e32base.h>
#include <d32comm.h>
#include <etelmm.h>

#include <commsdattypesv1_1.h>
using namespace CommsDat;

// 
// CTsyConfig Class
// Creating the class will load the current modem settings.
// They may be refreshed by calling ReloadSettings.
// The ModemName, PortDriverName, CommPortName and String functions 
// will return descriptors to the relevant settings taken from 
// the current modem's settings.
// The CommConfig function will return a reference to the Configuration 
// structure in the current modem's settings.  
// All these retrieval functions should return NULL if there is an error.
// If there is an error ReloadingSettings it should be returned in the TInt.
// All file closing, tidying and memory deallocation should be done 
// in the ~CTsyConfig destructor.
//
/**
@internalComponent
*/
enum TConfigType
	{
	EConfigTypePreInit,
	EConfigTypeInit,
	EConfigTypeConnect,
	EConfigTypeHangUp,
	EConfigTypeFull,
	EConfigTypeQuickInit,
	EConfigTypeDDBugWorkAroundStart,
	EConfigTypeDDBugWorkAroundEnd
	};

/**
@internalComponent
*/
const TInt KInternetAccessPoint=1;


class CTsyConfig : public CBase
/**
@internalComponent
*/
	{
//	friend class CDefaultRecordChangeNotifier;
public:
	static CTsyConfig* NewL(TBool aManual =EFalse);
	~CTsyConfig();

	TInt ConfigModemString(const TDesC& aStringTag,TDes8& aString);
	TInt ConfigModemString(const TDesC& aStringTag,TDes16& aString);
	TInt PortConfig(TCommConfig& aConfigPckg,TConfigType aConfigType);
	void GetIntervalPref(TInt& aInterval);
	void GetSpeakerSettingPref(RCall::TMonitorSpeakerControl& aSpeakerSetting);
	void GetSpeakerVolumePref(RCall::TMonitorSpeakerVolume& aSpeakerVolume);
	void GetWaitForDialTonePref(RCall::TWaitForDialTone& aWaitForDialTonePref);
	void GetCurrentTableViewsL();
	void GetLocationModemSettingsL();
	void ResetCurrentTableViews(TBool aForceErase=EFalse);
	void GetRequestedTableViewsL();
	void SetTableViewsL(RMobilePhone::TMMTableSettings& aMMTableSettings);

protected:
	void CommConfigL(TCommConfigV01& aConfig);
private:
	// Private object construction methods
	CTsyConfig();
	void ConstructL(TBool aExplicit);

	// Private utility methods
//	TBool ModemTableUsesMMTSYL(const TUint32& aModemId);
 	void GetModemBearerIdL(TUint32& aBearerId); 
 	void GetLocationIdL(const TUint32& aBearerId, TUint32& aLocationId); 

	TInt ConfigModemStringL(const TDesC& aStringTag,TDes8& aString);
	TInt ConfigModemStringL(const TDesC& aStringTag,TDes16& aString);

private:
	TCommConfigV01 iConfig;
	CMDBSession* iDbSession;
	TUint32 iModemBearer;
	TUint32 iLocationId;
	TBool iManualConfigMode;
	RMobilePhone::TMMTableSettings iMMTableSettings;

	TInt iInterval;
	RCall::TMonitorSpeakerControl iSpeakerSetting;
	RCall::TMonitorSpeakerVolume iSpeakerVolume;
	RCall::TWaitForDialTone iWaitForDialTonePref;
	};

#endif
