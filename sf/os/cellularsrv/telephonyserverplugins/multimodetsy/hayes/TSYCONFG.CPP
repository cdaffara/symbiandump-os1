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
// TSY Configuation Class
// 
//

#include "ATSTD.H"
#include "TSYCONFG.H"
#include "mSLOGGER.H"
#include <commsdattypesv1_1.h>
#include <commsdat_partner.h>
using namespace CommsDat;

// 
// Macros
//

#ifdef __LOGDEB__
_LIT8(KLogEntry,"CTsyConfig::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif

//
// Define some defaults if the modem settings can't be found
//
const TInt KDefaultLocationInternalPref=0;
const RCall::TMonitorSpeakerControl KDefaultModemSpeakerSetting=RCall::EMonitorSpeakerControlOnUntilCarrier;
const RCall::TMonitorSpeakerVolume KDefaultMonitorSpeakerVolume=RCall::EMonitorSpeakerVolumeOff;
const RCall::TWaitForDialTone KDefaultWaitForDialTone=RCall::EDialToneNoWait;
 
CTsyConfig* CTsyConfig::NewL(TBool aExplicit) 
/**
 * Passed a reference to the phone mode so that the default settings will not be altered whilst
 * a call is in progress
 */
	{
	CTsyConfig* tsyConfig=new(ELeave) CTsyConfig();
	CleanupStack::PushL(tsyConfig);
	tsyConfig->ConstructL(aExplicit); 
	CleanupStack::Pop();
	return (tsyConfig);
	}
	
CTsyConfig::CTsyConfig()
	{}

void CTsyConfig::ConstructL(TBool aExplicit)
	{
	iLocationId = 0;
	TInt r=KErrNone;
	iManualConfigMode=aExplicit;
	
	if(!aExplicit)
		{
		for (TInt i=0;i<10;i++)
			{
			TRAP(r, GetCurrentTableViewsL()); // Place a cursor on the default modem record in comms database server
			if (r==KErrAccessDenied) // if we get access denied from DBMS, which is a timing thing, just re-post
				{
				User::After(1000000);
				continue;
				}
			else
				break;
			}
		if (r)
			{
			LOGTEXT(_L8("CommDB values seem to be corrupt"));
			ResetCurrentTableViews(); // clean up everything
			User::Leave(KErrEtelModemSettingsCorrupt);
			}

		GetLocationModemSettingsL(); // Get other location and modem settings 
		ResetCurrentTableViews(); // Clear location and modem table views and close handle to CCommsDatabase
		}
	}

void CTsyConfig::GetLocationModemSettingsL()
/**
 * Gets some commonly used settings from the Location and Modem Table Views and stores them
 * internally as these are not likely to change. 
 * If some of these settings are not found in the table view then a predefined default values are
 * used.
 */
	{
	if(iLocationId == 0)
		{
		iInterval=KDefaultLocationInternalPref;
		iWaitForDialTonePref=KDefaultWaitForDialTone;
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
			iWaitForDialTonePref = RCall::EDialToneWait;
		else
			iWaitForDialTonePref = RCall::EDialToneNoWait;
		}

	if (iModemBearer != 0)
		{
		CMDBField<TUint32>* speakerPrefField = new(ELeave) CMDBField<TUint32>(KCDTIdSpeakerPref);
		CleanupStack::PushL(speakerPrefField);
		speakerPrefField->SetRecordId(iModemBearer);
		speakerPrefField->LoadL(*iDbSession);
		iSpeakerSetting = static_cast<RCall::TMonitorSpeakerControl>(static_cast<TUint32>(*speakerPrefField));
		CleanupStack::PopAndDestroy(speakerPrefField);
		
		CMDBField<TUint32>* speakerVolPrefField = new(ELeave) CMDBField<TUint32>(KCDTIdSpeakerVolPref);
		CleanupStack::PushL(speakerVolPrefField);
		speakerVolPrefField->SetRecordId(iModemBearer);
		speakerVolPrefField->LoadL(*iDbSession);
		iSpeakerVolume = static_cast<RCall::TMonitorSpeakerVolume>(static_cast<TUint32>(*speakerVolPrefField));			
		CleanupStack::PopAndDestroy(speakerVolPrefField);
		}

	TUint32 rate;
	TUint32 dataBits;
	TUint32 stopBits;
	TUint32 parity;
	TUint32 handshake;
	
	if (iModemBearer == 0)
		{
		iSpeakerSetting = KDefaultModemSpeakerSetting;
		iSpeakerVolume = KDefaultMonitorSpeakerVolume;
		User::Leave(KErrNotFound);
		}

	CMDBField<TUint32>* rateField = new(ELeave) CMDBField<TUint32>(KCDTIdRate);
	CleanupStack::PushL(rateField);
	rateField->SetRecordId(iModemBearer);
	rateField->LoadL(*iDbSession);
	rate = *rateField;
	CleanupStack::PopAndDestroy(rateField);
	
	CMDBField<TUint32>* dataBitsField = new(ELeave) CMDBField<TUint32>(KCDTIdDataBits);
	CleanupStack::PushL(dataBitsField);
	dataBitsField->SetRecordId(iModemBearer);
	dataBitsField->LoadL(*iDbSession);
	dataBits = *dataBitsField;
	CleanupStack::PopAndDestroy(dataBitsField);

	CMDBField<TUint32>* stopBitsField = new(ELeave) CMDBField<TUint32>(KCDTIdStopBits);
	CleanupStack::PushL(stopBitsField);
	stopBitsField->SetRecordId(iModemBearer);
	stopBitsField->LoadL(*iDbSession);
	stopBits = *stopBitsField;
	CleanupStack::PopAndDestroy(stopBitsField);
	
	CMDBField<TUint32>* parityField = new(ELeave) CMDBField<TUint32>(KCDTIdParity);
	CleanupStack::PushL(parityField);
	parityField->SetRecordId(iModemBearer);
	parityField->LoadL(*iDbSession);
	parity = *parityField;
	CleanupStack::PopAndDestroy(parityField);
	
	CMDBField<TUint32>* handshakeField = new(ELeave) CMDBField<TUint32>(KCDTIdHandshaking);
	CleanupStack::PushL(handshakeField);
	handshakeField->SetRecordId(iModemBearer);
	handshakeField->LoadL(*iDbSession);
	handshake = *handshakeField;
	CleanupStack::PopAndDestroy(handshakeField);

	iConfig.iRate = (TBps)rate;
	iConfig.iDataBits = (TDataBits)dataBits;
	iConfig.iStopBits = (TStopBits)stopBits;
	iConfig.iParity = (TParity)parity;
	iConfig.iHandshake = (TUint)handshake;
	}

CTsyConfig::~CTsyConfig()
	{
	// Just in case these have not been cleared
	delete iDbSession;
	}

void CTsyConfig::CommConfigL(TCommConfigV01& aConfig)
	{
	aConfig = iConfig;
	}

TInt CTsyConfig::ConfigModemStringL(const TDesC& aStringTag, TDes8& aString)
	{
	TInt r=KErrNone;
	for (TInt i=0;i<10;i++)
		{
		TRAP(r, GetCurrentTableViewsL()); // Place a cursor on the default modem record in comms database server
		if (r==KErrAccessDenied) // if we get access denied from DBMS, which is a timing thing, just re-post
			{
			User::After(1000000);
			continue;
			}
		else
			break;
		}
	if (r)
		{
		LOGTEXT(_L8("CommDB values seem to be corrupt"));
		ResetCurrentTableViews(); // clean up everything
		return (KErrEtelModemSettingsCorrupt);
		}

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

	ResetCurrentTableViews(); // Clear location and modem table views and close handle to CCommsDatabase
	return ret;
	}

TInt CTsyConfig::ConfigModemStringL(const TDesC& aStringTag, TDes16& aString)
	{
	TInt r=KErrNone;
	for (TInt i=0;i<10;i++)
		{
		TRAP(r, GetCurrentTableViewsL()); // Place a cursor on the default modem record in comms database server
		if (r==KErrAccessDenied) // if we get access denied from DBMS, which is a timing thing, just re-post
			{
			User::After(1000000);
			continue;
			}
		else
			break;
		}
	if (r)
		{
		LOGTEXT(_L8("CommDB values seem to be corrupt"));
		ResetCurrentTableViews(); // clean up everything
		return (KErrEtelModemSettingsCorrupt);
		}

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

	ResetCurrentTableViews(); // Clear location and modem table views and close handle to CCommsDatabase
	return ret;
	}

TInt CTsyConfig::ConfigModemString(const TDesC& aStringTag, TDes8& aString)
	{
	TInt ret = KErrNone;
	TRAPD(error, ret = ConfigModemStringL(aStringTag, aString));
	if(error != KErrNone)
		{
		ret = error;
		}
	return ret;
	}

TInt CTsyConfig::ConfigModemString(const TDesC& aStringTag, TDes16& aString)
	{
	TInt ret = KErrNone;
	TRAPD(error, ret = ConfigModemStringL(aStringTag, aString));
	if(error != KErrNone)
		{
		ret = error;
		}
	return ret;
	}

void CTsyConfig::GetIntervalPref(TInt& aInterval)
/**
 * Stores the Interval Preferences settings internally. These settings are stored on construction
 * of the CTsyConfig object.
 * @param aInterval Interval Preference setting
 */
	{
	aInterval = iInterval;
	}

void CTsyConfig::GetSpeakerSettingPref(RCall::TMonitorSpeakerControl& aSpeakerSetting)
/**
 * Stores the Speaker Preferences settings internally. These settings are stored on construction
 * of the CTsyConfig object.
 * @param aSpeakerSetting Speaker Preference setting
 */
	{
	aSpeakerSetting = iSpeakerSetting;
	}

void CTsyConfig::GetSpeakerVolumePref(RCall::TMonitorSpeakerVolume& aSpeakerVolume)
/**
 * Stores the Speaker Volume Preferences settings internally. These settings are stored on construction
 * of the CTsyConfig object.
 * @param aSpeakerVolume Speaker Volume setting
 */
	{
	aSpeakerVolume = iSpeakerVolume;
	}

void CTsyConfig::GetWaitForDialTonePref(RCall::TWaitForDialTone& aWaitForDialTonePref)
/**
 * Stores the Dial Tone Preferences settings internally. These settings are stored on construction
 * of the CTsyConfig object.
 * @param aWaitForDialTonePref Wait for Dial Tone Preference setting
 */
	{
	aWaitForDialTonePref = iWaitForDialTonePref;
	}

TInt CTsyConfig::PortConfig(TCommConfig& aConfigPckg, TConfigType aConfigType)
/**
 * Gets the port config settings and masks out appropriate handshaking settings
 * according to whether state is about to initialise, just connected or about to hang up
 */ 
	{
	TInt r=KErrNone;
	for (TInt i=0;i<10;i++)
		{
		TRAP(r, GetCurrentTableViewsL()); // Place a cursor on the default modem record in comms database server
		if (r==KErrAccessDenied) // if we get access denied from DBMS, which is a timing thing, just re-post
			{
			User::After(1000000);
			continue;
			}
		else
			break;
		}
	if (r)
		{
		LOGTEXT(_L8("CommDB values seem to be corrupt"));
		ResetCurrentTableViews(); // clean up everything
		return (KErrEtelModemSettingsCorrupt);
		}

	TCommConfig configDummyPckg;
	TCommConfigV01& config=configDummyPckg();
	if (aConfigType==EConfigTypeHangUp ||
		aConfigType==EConfigTypeQuickInit)
		{
		config = iConfig;
		config.iHandshake = 0;//&= (~(KConfigFailDCD | KConfigObeyDCD | KConfigFailDSR));
		}
	else
		{
		TRAPD(ret,CommConfigL(config));
		if (ret)
			return ret;
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
	ResetCurrentTableViews(); // Clear location and modem table views and close handle to CCommsDatabase
	return KErrNone;
	}

void CTsyConfig::GetCurrentTableViewsL() 
/**
 * Opens a handle to CCommsDatabase	and positions the view on the default modem specified 
 * in the current Connected Modem record.
 * Note that the TSY does not keep the handle to the CCommsDatabase opened all the time, 
 * therefore this method is used to open the handle only when needed. The methods using 
 * this method are also responsible for calling ResetCurrentTableViews() to close the
 * handle to the CCommsDatabase and to clean up the table views.
 */
	{
	if(iManualConfigMode)
		{	
		GetRequestedTableViewsL();
		}
	else
		{
		ResetCurrentTableViews();
		
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		iDbSession = CMDBSession::NewL(KCDVersion1_2);
#else
		iDbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
		__ASSERT_DEBUG(iDbSession,Panic(ETsyConfigNullDBPointer));

		TUint32 modemId(0), locationId(0);

		// 
		// Search the bearer tables for records using the MM.TSY
		GetModemBearerIdL(modemId);
	
		//
		// Get the associated locationId
		
		GetLocationIdL(modemId,locationId);
		
		//Check if the selected bearer is an MMTSY bearer with a valid location
		if(!modemId || !locationId) 
			{
			//
			// Selected bearer does not mention the MMTSY
			LOCAL_LOGTEXT("GetCurrentSettingsL","MMTSY not mentioned in the selected bearer");
			__ASSERT_DEBUG(EFalse,Panic(ETsyConfigMMTSYNotInModemTables));
			User::Leave(KErrNotFound);
			}
		else
			{
						
			iModemBearer = modemId;			
			iLocationId = locationId;
				
			}
		}
	}
void CTsyConfig::SetTableViewsL(RMobilePhone::TMMTableSettings& aMMTableSettings)
	{
	if(iManualConfigMode)
		{
		iMMTableSettings=aMMTableSettings;

		ResetCurrentTableViews(ETrue); // Clear location and modem table views and close handle to CCommsDatabase
		GetRequestedTableViewsL();
		GetLocationModemSettingsL(); // Get other location and modem settings 
		}
	}	


void CTsyConfig::GetRequestedTableViewsL()
	{
	
	if (iDbSession == NULL)
		{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		iDbSession = CMDBSession::NewL(KCDVersion1_2);
#else
		iDbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
		__ASSERT_DEBUG(iDbSession,Panic(ETsyConfigNullDBPointer));
				
		iModemBearer = 0;

		CMDBField<TUint32>* bearerField = new(ELeave) CMDBField<TUint32>(KCDTIdIAPBearer);
		CleanupStack::PushL(bearerField);
		bearerField->SetRecordId(iMMTableSettings.iLocId);
		bearerField->LoadL(*iDbSession);
		iModemBearer = *bearerField;
		CleanupStack::PopAndDestroy(bearerField);
		}
		
		TName modem;
		CMDBField<TDesC>* tsyField = new(ELeave) CMDBField<TDesC>(KCDTIdTsyName);
		CleanupStack::PushL(tsyField);
		tsyField->SetRecordId(iModemBearer);
		tsyField->SetMaxLengthL(KMaxTextLength);
		tsyField->LoadL(*iDbSession);
		modem = *tsyField;
		CleanupStack::PopAndDestroy(tsyField);
		

		if(modem.Compare(_L("MM"))!=KErrNone)
			{
			// Selected bearer does not mention the MMTSY
			LOCAL_LOGTEXT("GetCurrentSettingsL","Bearer for selected IAP does not use MMTSY");
			__ASSERT_DEBUG(EFalse,Panic(ETsyConfigMMTSYNotInModemTables));
			User::Leave(KErrNotFound);
			}



	}
	
void CTsyConfig::ResetCurrentTableViews(TBool aForceErase)
/**
 * Closes the handle to CCommsDatabase and clears the view on the default modem specified 
 * in the current Connected Modem record. This method needs to be called after 
 * GetCurrentTableViewsL().
 */
	{
	if( !iManualConfigMode || aForceErase )
		{
		delete iDbSession;
		iDbSession = NULL;		

		}
	}

void CTsyConfig::GetLocationIdL(const TUint32& aBearerId,TUint32& aLocationId) 
/*
 *  Scan through the table for records containing MM.TSY
 *  Stop at the first instance of such a record and return the id
 *
 */
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

void CTsyConfig::GetModemBearerIdL(TUint32& aBearerId) 
/*
 *  Scan through the table for records containing MM.TSY
 *  Stop at the first instance of such a record and return the id
 *
 */
	{
    CCDModemBearerRecord *modemRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));
    CleanupStack::PushL(modemRecord);
    
    _LIT(KTsyName,"MM");
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
