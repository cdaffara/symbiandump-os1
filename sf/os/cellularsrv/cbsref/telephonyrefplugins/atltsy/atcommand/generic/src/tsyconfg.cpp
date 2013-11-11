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
// @file tsyconfig.cpp
// This contains CTsyConfig which manage access to CommDB configuration.
// 

// system include
#include <commsdattypesv1_1.h>
#include <commsdat_partner.h>
using namespace CommsDat;

// user include
#include "tsyconfg.h"
#include "mslogger.h"

#ifdef __LOGDEB__
_LIT8(KLogEntry,"CTsyConfig::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif

// const defination
const TInt KDefaultLocationInternalPref = 0;
const RCall::TMonitorSpeakerControl KDefaultModemSpeakerSetting = RCall::EMonitorSpeakerControlOnUntilCarrier;
const RCall::TMonitorSpeakerVolume KDefaultMonitorSpeakerVolume = RCall::EMonitorSpeakerVolumeOff;
const RCall::TWaitForDialTone KDefaultWaitForDialTone = RCall::EDialToneNoWait;

// ---------------------------------------------------------------------------
// CTsyConfig::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CTsyConfig* CTsyConfig::NewL() 
	{
	CTsyConfig* self = new(ELeave) CTsyConfig();
	CleanupStack::PushL(self);
	self->ConstructL(); 
	CleanupStack::Pop();
	return (self);
	}

// ---------------------------------------------------------------------------
// CTsyConfig::CTsyConfig
// other items were commented in a header
// ---------------------------------------------------------------------------
CTsyConfig::CTsyConfig()
	{	
	}

// ---------------------------------------------------------------------------
// CTsyConfig::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::ConstructL()
	{
	iLocationId = 0;
	iModemBearer = 0;
	SetCurrentTableViewL();
	GetLocationModemSettingsL();
	}

// ---------------------------------------------------------------------------
// CTsyConfig::SetCurrentTableViewL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::SetCurrentTableViewL()
	{
	TInt r = KErrNone;

	for (TInt i = 0; i < 10; i++)
		{
		TRAP(r, GetCurrentTableViewsL()); // Place a cursor on the default modem record in comms database server
		if (r == KErrAccessDenied)        // if we get access denied from DBMS, which is a timing thing, just re-post
			{
			User::After(1000000);
			continue;
			}
		else
			{
			break;
			}
		}
	
	if(r)
		{
		LOGTEXT(_L8("CommDB values seem to be corrupt"));
		User::Leave(KErrEtelModemSettingsCorrupt);
		}
	}

// ---------------------------------------------------------------------------
// CTsyConfig::GetLocationModemSettingsL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::GetLocationModemSettingsL()
	{
	ASSERT(iModemBearer != 0);
	
	iSpeakerSetting = KDefaultModemSpeakerSetting;
	iSpeakerVolume = KDefaultMonitorSpeakerVolume;
	
	if(iLocationId == 0)
		{
		iInterval = KDefaultLocationInternalPref;
		iWaitForDialTonePref = KDefaultWaitForDialTone;
		}
	else
		{
		TBool value(EFalse);
		CMDBField<TUint32>* intervalField = new(ELeave) CMDBField<TUint32>(KCDTIdPauseAfterDialOut);
		CleanupStack::PushL(intervalField);
		intervalField->SetRecordId(iLocationId);
		intervalField->LoadL(*iDbSession);
		iInterval = *intervalField;
		CleanupStack::PopAndDestroy(intervalField);
		
		CMDBField<TUint32>* valueField = new(ELeave) CMDBField<TUint32>(KCDTIdWaitForDialTone);		
		CleanupStack::PushL(valueField);
		valueField->SetRecordId(iLocationId);
		valueField->LoadL(*iDbSession);
		value = *valueField;
		CleanupStack::PopAndDestroy(valueField);
		
		if (value)
			{
			iWaitForDialTonePref = RCall::EDialToneWait;
			}
		else
			{
			iWaitForDialTonePref = RCall::EDialToneNoWait;
			}
		}

    // read speaker preferences
	CMDBField<TUint32>* speakerPrefField = new(ELeave) CMDBField<TUint32>(KCDTIdSpeakerPref);
	CleanupStack::PushL(speakerPrefField);
	speakerPrefField->SetRecordId(iModemBearer);
	speakerPrefField->LoadL(*iDbSession);
	iSpeakerSetting = static_cast<RCall::TMonitorSpeakerControl>(static_cast<TUint32>(*speakerPrefField));
	CleanupStack::PopAndDestroy(speakerPrefField);
	
	// read speaker volume preferences
	CMDBField<TUint32>* speakerVolPrefField = new(ELeave) CMDBField<TUint32>(KCDTIdSpeakerVolPref);
	CleanupStack::PushL(speakerVolPrefField);
	speakerVolPrefField->SetRecordId(iModemBearer);
	speakerVolPrefField->LoadL(*iDbSession);
	iSpeakerVolume = static_cast<RCall::TMonitorSpeakerVolume>(static_cast<TUint32>(*speakerVolPrefField));			
	CleanupStack::PopAndDestroy(speakerVolPrefField);

    // read modem rate
	CMDBField<TUint32>* rateField = new(ELeave) CMDBField<TUint32>(KCDTIdRate);
	CleanupStack::PushL(rateField);
	rateField->SetRecordId(iModemBearer);
	rateField->LoadL(*iDbSession);
	TUint32 rate;
	rate = *rateField;
	CleanupStack::PopAndDestroy(rateField);
	
	// read data bits for modem
	CMDBField<TUint32>* dataBitsField = new(ELeave) CMDBField<TUint32>(KCDTIdDataBits);
	CleanupStack::PushL(dataBitsField);
	dataBitsField->SetRecordId(iModemBearer);
	dataBitsField->LoadL(*iDbSession);
	TUint32 dataBits;
	dataBits = *dataBitsField;
	CleanupStack::PopAndDestroy(dataBitsField);
    
	// read stop bits for modem
	CMDBField<TUint32>* stopBitsField = new(ELeave) CMDBField<TUint32>(KCDTIdStopBits);
	CleanupStack::PushL(stopBitsField);
	stopBitsField->SetRecordId(iModemBearer);
	stopBitsField->LoadL(*iDbSession);
	TUint32 stopBits;
	stopBits = *stopBitsField;
	CleanupStack::PopAndDestroy(stopBitsField);
	
	// read parity for modem
	CMDBField<TUint32>* parityField = new(ELeave) CMDBField<TUint32>(KCDTIdParity);
	CleanupStack::PushL(parityField);
	parityField->SetRecordId(iModemBearer);
	parityField->LoadL(*iDbSession);
	TUint32 parity;
	parity = *parityField;
	CleanupStack::PopAndDestroy(parityField);
	
	// read handshake for modem
	CMDBField<TUint32>* handshakeField = new(ELeave) CMDBField<TUint32>(KCDTIdHandshaking);
	CleanupStack::PushL(handshakeField);
	handshakeField->SetRecordId(iModemBearer);
	handshakeField->LoadL(*iDbSession);
	TUint32 handshake;
	handshake = *handshakeField;
	CleanupStack::PopAndDestroy(handshakeField);
    
	// set the modem configuration
	iConfig.iRate = (TBps)rate;
	iConfig.iDataBits = (TDataBits)dataBits;
	iConfig.iStopBits = (TStopBits)stopBits;
	iConfig.iParity = (TParity)parity;
	iConfig.iHandshake = handshake;
	}

// ---------------------------------------------------------------------------
// CTsyConfig::~CTsyConfig
// other items were commented in a header
// ---------------------------------------------------------------------------
CTsyConfig::~CTsyConfig()
	{
	delete iDbSession;
	}

// ---------------------------------------------------------------------------
// CTsyConfig::ConfigModemStringL
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CTsyConfig::ConfigModemStringL(const TDesC& aStringTag, TDes8& aString)
	{
	ASSERT(iDbSession != 0);
	CCDModemBearerRecord* modemRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
	CleanupStack::PushL(modemRecord);
	modemRecord->SetRecordId(iModemBearer);
	modemRecord->LoadL(*iDbSession);
	
	TInt ret;
	TInt type(0);
	CMDBElement* baseField = NULL;
	TRAP(ret, baseField = modemRecord->GetFieldByNameL(aStringTag, type));
	if (ret == KErrNone)
		{
		// check for type
		switch(type)
			{
			case EMedText:
			case EText:
				{
				CMDBField<TDesC>* field16 = static_cast<CMDBField<TDesC>*>(baseField);
				const TDesC& refField = *field16;
				aString.Copy(refField);
				ret = KErrNone;
				}
				break;
			case EDesC8:
				{
				CMDBField<TDesC8>* field = static_cast<CMDBField<TDesC8>*>(baseField);
				aString = *field;
				ret = KErrNone;
				}
				break;
			default:
				ret = KErrNotFound;
			}
    	}
	CleanupStack::PopAndDestroy(modemRecord);

	return ret;
	}

// ---------------------------------------------------------------------------
// CTsyConfig::ConfigModemStringL
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CTsyConfig::ConfigModemStringL(const TDesC& aStringTag, TDes16& aString)
	{
	ASSERT(iDbSession != 0);
	// get the table record
	CCDModemBearerRecord* modemRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
	CleanupStack::PushL(modemRecord);
	modemRecord->SetRecordId(iModemBearer);
	modemRecord->LoadL(*iDbSession);
	
	TInt ret;
	TInt type(0);
	CMDBElement* baseField = NULL;
	TRAP(ret, baseField = modemRecord->GetFieldByNameL(aStringTag, type));
	if (ret == KErrNone)
		{
		// check for type
		switch(type)
			{
			case EMedText:
			case EText:
				{
				CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(baseField);
				aString = *field;
				ret = KErrNone;
				}
				break;
			case EDesC8:
				{
				// des16 needs to be cast to des8
				CMDBField<TDesC8>* field8 = static_cast<CMDBField<TDesC8>*>(baseField);
				const TDesC8& refField = *field8;
				aString.Copy(refField);
				ret = KErrNone;
				}
				break;
			default:
				ret = KErrNotFound;
			}
		}
	
	CleanupStack::PopAndDestroy(modemRecord);

	return ret;
	}

// ---------------------------------------------------------------------------
// CTsyConfig::GetSpeakerSettingPref
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::GetSpeakerSettingPref(RCall::TMonitorSpeakerControl& aSpeakerSetting)
	{
	aSpeakerSetting = iSpeakerSetting;
	}

// ---------------------------------------------------------------------------
// CTsyConfig::GetSpeakerVolumePref
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::GetSpeakerVolumePref(RCall::TMonitorSpeakerVolume& aSpeakerVolume)
	{
	aSpeakerVolume = iSpeakerVolume;
	}

// ---------------------------------------------------------------------------
// CTsyConfig::GetWaitForDialTonePref
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::GetWaitForDialTonePref(RCall::TWaitForDialTone& aWaitForDialTonePref)
	{
	aWaitForDialTonePref = iWaitForDialTonePref;
	}

// ---------------------------------------------------------------------------
// CTsyConfig::PortConfig
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CTsyConfig::PortConfig(TCommConfig& aConfigPckg, TConfigType aConfigType)
	{
	TCommConfig configDummyPckg;
	TCommConfigV01& config = configDummyPckg();
	config = iConfig;
	
	if (aConfigType==EConfigTypeHangUp ||
		aConfigType==EConfigTypeQuickInit)
		{
		config.iHandshake = 0;//&= (~(KConfigFailDCD | KConfigObeyDCD | KConfigFailDSR));
		}
	else
		{
		switch (aConfigType)
			{
		case EConfigTypePreInit:
			config.iHandshake &= (~(KConfigObeyCTS | KConfigFailCTS | KConfigObeyDCD | KConfigFailDCD | KConfigFailDSR));
			break;
		case EConfigTypeInit:
			config.iHandshake &= (~(KConfigObeyCTS | KConfigFailCTS | KConfigObeyDCD | KConfigFailDCD));
			break;
		case EConfigTypeConnect:
			config.iHandshake &= (~(KConfigFailCTS | KConfigFailDCD));	// fail DCD masked out, as should get NO CARRIER anyway
			break;
		case EConfigTypeFull:
			break;
		case EConfigTypeDDBugWorkAroundStart:
			if (config.iRate!=EBps300)	// ensure that something other than handshaking has changed
				config.iRate=EBps300;	// to work around the bug in the ARM device driver
			else
				config.iRate=EBps2400;
			config.iHandshake=0;
			break;
		case EConfigTypeDDBugWorkAroundEnd:
			config.iHandshake=0;
			break;
		default:
			break;
			}
		}
	
	aConfigPckg=configDummyPckg;
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CTsyConfig::GetCurrentTableViewsL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::GetCurrentTableViewsL() 
	{
	ResetCurrentTableViews();
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	iDbSession = CMDBSession::NewL(KCDVersion1_2);
#else
	iDbSession = CMDBSession::NewL(KCDVersion1_1);
#endif

	iModemBearer = 0;
	iLocationId = 0;
	// Search the bearer tables for records using the Phonetsy.TSY
	GetModemBearerIdL(iModemBearer);

	// Get the associated locationId
	GetLocationIdL(iModemBearer,iLocationId);
	
	//Check if the selected bearer is an MMTSY bearer with a valid location
	if(!iModemBearer || !iLocationId) 
		{
		//
		// Selected bearer does not mention the MMTSY
		LOCAL_LOGTEXT("GetCurrentSettingsL","PhoneTSY not mentioned in the selected bearer");
		User::Leave(KErrNotFound);
		}
	}

// ---------------------------------------------------------------------------
// CTsyConfig::ResetCurrentTableViews
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::ResetCurrentTableViews()
	{
	delete iDbSession;
	iDbSession = NULL;		
	}

// ---------------------------------------------------------------------------
// CTsyConfig::GetLocationIdL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::GetLocationIdL(const TUint32& aBearerId, TUint32& aLocationId) 
 	{
    CCDIAPRecord *iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
  	CleanupStack::PushL(iapRecord);
  	
  	iapRecord->iBearer = aBearerId;
  	TBool err = iapRecord->FindL(*iDbSession);
  	if (err)
  		{
  		aLocationId = iapRecord->iLocation;
  		}
  	else
  		{
  		aLocationId = static_cast<TUint32>(KErrNotFound);
  		}
  	
    CleanupStack::PopAndDestroy(iapRecord);
    
	}

// ---------------------------------------------------------------------------
// CTsyConfig::GetModemBearerIdL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CTsyConfig::GetModemBearerIdL(TUint32& aBearerId)
	{
    CCDModemBearerRecord *modemRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
    CleanupStack::PushL(modemRecord);
    
    _LIT(KTsyName,"phonetsy"); // Note this is case sensitive and must match the name in commdb
    modemRecord->iTsyName.SetMaxLengthL(KMaxTextLength);
    modemRecord->iTsyName = KTsyName;
    
    TBool searchResult = modemRecord->FindL(*iDbSession);
    
    if (searchResult)
	    {
		aBearerId = modemRecord->RecordId();	
	    }
	else
		{
		aBearerId = static_cast<TUint32>(KErrNotFound);
		}
    
    CleanupStack::PopAndDestroy(modemRecord);
	}

// End of file
