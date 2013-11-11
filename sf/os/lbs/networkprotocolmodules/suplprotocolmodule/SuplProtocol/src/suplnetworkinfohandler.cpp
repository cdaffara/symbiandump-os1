// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Object to monitor the ETel networki information needed by SUPL
// protocol module
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include <e32debug.h>

#include "suplnetworkinfohandler.h"
#include <e32base.h>
#include <e32property.h>
#include <etelmm.h>
#include <rsshared.h>
#include "supldevloggermacros.h"

using namespace CommsDat;

const TUint KMaxCellIdOrLac = 65535;
const TUint KMaxMccOrMnc = 999;


/**
*/
CSuplNetworkInfoHandler::CSuplNetworkInfoHandler(MSuplNetworkInfoObserver& aObserver) :
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iState(EStateNull),
	iNetworkInfoPckg(iNetworkInfo),
	iOnStoreInfoPckg(iOnStoreInfo),
	iOnStoreEntryPckg(iOnStoreEntry)	
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::CSuplNetworkInfoHandler() Begin\n");
	CActiveScheduler::Add(this);
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::CSuplNetworkInfoHandler() End\n");
	}

/**
*/	
CSuplNetworkInfoHandler::~CSuplNetworkInfoHandler()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::~CSuplNetworkInfoHandler() Begin\n");
	Cancel();
	
	if (iTimingAdvSupport)
		{
		delete iCellInfoHandler;
		iCellPhone.Close();
		}
	
	iOnStore.Close();
	
	iNetPhone.Close();
	iTelServer.Close();
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::~CSuplNetworkInfoHandler() End\n");
	}

/**
*/	
CSuplNetworkInfoHandler* CSuplNetworkInfoHandler::NewL(MSuplNetworkInfoObserver& aObserver)
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::NewL() Begin\n");
	CSuplNetworkInfoHandler* self = new (ELeave) CSuplNetworkInfoHandler(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::NewL() End\n");
	return self;
	}

/**
*/
void CSuplNetworkInfoHandler::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::ConstructL() Begin\n");
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::ConstructL() End\n");
	}

/**
*/
void CSuplNetworkInfoHandler::StartGettingNetworkInfoL()
{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::StartGettingNetworkInfo() Begin\n");

	//Attempt to connect to the ETel Server.  This will start C32Start if
	//it has not already been started by the startup routine.
	User::LeaveIfError(iTelServer.Connect());
		
	// Check if the Comms Process RootServer has started
	iConfigurationProperty.Attach(KUidSystemCategory, RootServer::KUidC32StartPropertyKey.iUid);
	TInt propertyValue = RootServer::EInitialising;
	TInt propertyResult = iConfigurationProperty.Get(propertyValue);
		
	if(propertyValue < RootServer::EConfigurationComplete)
		{
		iState = EStateInitializing;
		iConfigurationProperty.Subscribe(iStatus);
		SetActive();
		}
	else
		{
		// Attempt to get network information
		GetNetworkInfoL();
		}

	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::StartGettingNetworkInfo() End\n");
}

/** 
*/
void CSuplNetworkInfoHandler::GetNetworkInfoL()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetNetworkInfoL() Begin\n");
	InitialisePhonesL();
	
	// Connected to ETel successfully
	if (iTimingAdvSupport)
		{
		// Instantiate the active object that monitors changes to 
		// Timing Advance passing to it the initialized RMobilePhone
		// and the observer interested in Timing Advance.
		iCellInfoHandler = CSuplCellInfoHandler::NewL(iObserver,iCellPhone);
		iCellInfoHandler->StartGettingCellInfo();
		}

	// Get the network info
	iState = EStateGettingNetInfo;
	iNetPhone.GetCurrentNetwork(iStatus, iNetworkInfoPckg, iLocArea);
	SetActive();
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetNetworkInfoL() End\n");
	}

/**
*/
void CSuplNetworkInfoHandler::RunL()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::RunL() Begin\n");
	User::LeaveIfError(iStatus.Int());
	
	switch (iState)
		{
		case EStateInitializing:
			{
			TInt propertyValue = RootServer::EInitialising; // set to safe state
			TInt propertyResult = iConfigurationProperty.Get(propertyValue);
			
			if(propertyValue != RootServer::EConfigurationComplete)
				{
				iConfigurationProperty.Subscribe(iStatus);
				SetActive();
				}
			else
				{
				iConfigurationProperty.Cancel();
				GetNetworkInfoL();
				}
			break;
			}

		case EStateGettingNetInfo:
			{
			if(ValidateNetInfo())
				{
				iObserver.NetInfoResults(iNetworkInfo, iLocArea);
				}
			if(iHomeNetworkInfoSupport)
				{
				GetMccMnc();
				}
			else if(iMsisdnSupport)
				{
				GetMsisdnStoreInfo();
				}
			else
				{
				MonitorNetworkChange();
				}
			break;	
			}
		
		case EStateGettingMccMnc:
			{
			if(iStatus==KErrNone && ValidateNetInfo())
				{
				//All checks are done in ValidateNetInfo
				TUint mcc = 0;
				TLex lex(iNetworkInfo.iCountryCode);
				lex.Val(mcc);
				TUint mnc = 0;
				lex.Assign(iNetworkInfo.iNetworkId);
				lex.Val(mnc);
				iObserver.HomeMccMncResult(mcc, mnc);
				}
			if(iMsisdnSupport)
				{
				GetMsisdnStoreInfo();
				}
			else
				{
				MonitorNetworkChange();
				}
			break;
			}
			
		case EStateGettingMsisdnStoreInfo:
			{
			if(iStatus==KErrNone && iOnStoreInfo.iUsedEntries>0 && 
					(iOnStoreInfo.iCaps & RMobilePhoneStore::KCapsIndividualEntry))
				{
				//Currently we simply use the first record to get the MSISDN 
				iOnStoreEntry.iIndex = 1;
				GetMsisdnStoreEntry();
				}
			else
				{
				iMsisdnSupport = EFalse;
				MonitorNetworkChange();
				}
			break;
			}
		
		case EStateGettingMsisdnStoreEntry:
			{
			if(iStatus==KErrNone)
				{
				iObserver.MsisdnResult(iOnStoreEntry.iNumber.iTelNumber);
				}
			else
				{
				iMsisdnSupport = EFalse;
				}
			MonitorNetworkChange();	
			break;
			}
			
		case EStateNetInfoChangeMon:
			{
			if(ValidateNetInfo())
				{
				iObserver.NetInfoResults(iNetworkInfo, iLocArea);
				}
			MonitorNetworkChange(); // Keep monitoring
			break;				
			}
		
		default:
			break;		
		}
	
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::RunL() End\n");
	}

/**
*/	
void CSuplNetworkInfoHandler::DoCancel()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::DoCancel() Begin\n");
	//Check to see what stage we in are to find out what to cancel
	switch (iState)
		{
		case EStateInitializing:
			{
			iConfigurationProperty.Cancel();
			break;				
			} 
		case EStateGettingNetInfo:
			{
			iNetPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
			break;
			}
		case EStateGettingMccMnc:
			{
			iNetPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetwork);
			break;
			}
		case EStateNetInfoChangeMon:
			{
			iNetPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
			break;
			}
			
		default:
			break;
		}
	
	if(iCellInfoHandler)
		{
		iCellInfoHandler->Cancel();
		}
	
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::DoCancel() End\n");
	}

/**
*/	
TInt CSuplNetworkInfoHandler::RunError(TInt /*aError*/)
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::DoCancel() Begin\n");
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::DoCancel() End\n");
	return KErrNone;
	}

/** Asynchronous request to obtain MCC and MNC
*/
void CSuplNetworkInfoHandler::GetMccMnc()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetMccMnc() Begin\n");
	iState = EStateGettingMccMnc;
	iNetPhone.GetHomeNetwork(iStatus, iNetworkInfoPckg);
	SetActive();
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetMccMnc() End\n");
	}

/** Asynchronous request to obtain Msisdn - getting store info.
*/
void CSuplNetworkInfoHandler::GetMsisdnStoreInfo()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetMsisdnStoreInfo() Begin\n");
	iState = EStateGettingMsisdnStoreInfo;
	iOnStore.GetInfo(iStatus, iOnStoreInfoPckg);
	SetActive();
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetMsisdnStoreInfo() End\n");
	}

/** Asynchronous request to obtain Msisdn - getting store entry.
*/
void CSuplNetworkInfoHandler::GetMsisdnStoreEntry()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetMsisdnStoreEntry() Begin\n");
	iState = EStateGettingMsisdnStoreEntry;
	//please note that the index is set in RunL for iOnStoreEntryPckg
	iOnStore.Read(iStatus, iOnStoreEntryPckg);
	SetActive();
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetMsisdnStoreEntry() End\n");
	}


/** Asychronous request to obtain notifications
of change in the current network data
*/
void CSuplNetworkInfoHandler::MonitorNetworkChange()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::MonitorNetworkChange() Begin\n");
	iState = EStateNetInfoChangeMon;
	iNetPhone.NotifyCurrentNetworkChange(iStatus, iNetworkInfoPckg, iLocArea);
	SetActive();
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::MonitorNetworkChange() End\n");
	}

/** Retrieves the name of the .tsy to use from CommDB/CommsDat

Hard-coded to use the .tsy file specified in the ModemBearerEntry
which is specified (by index) by the KCDTIdModemPhoneServicesSMS
setting in the global settings table.
*/
void CSuplNetworkInfoHandler::GetCommDbTSYnameL(TDes& aTsyName)
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetCommDbTSYnameL() Begin\n");
	CMDBSession* db = CMDBSession::NewLC(KCDLatestVersion);

	CMDBField<TUint32>* globalSettingField = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
	CleanupStack::PushL(globalSettingField);

	globalSettingField->SetRecordId(1);
	globalSettingField->LoadL(*db);
	TUint32 modemId = *globalSettingField;

	CMDBField<TDesC>* tsyField = new(ELeave) CMDBField<TDesC>(KCDTIdTsyName);
	CleanupStack::PushL(tsyField);

	tsyField->SetRecordId(modemId);
	tsyField->LoadL(*db);

	aTsyName = *tsyField;
	CleanupStack::PopAndDestroy(3, db); // db, tsyField & globalSettingField
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::GetCommDbTSYnameL() End\n");
	}

/** Finds and opens the phones to be used to monitor current network
info and cell info (timing advance).

This method also checks if the TSY can provide the different
bits of information needed by SUPL from ETEL: current and home network
information, cell ID information (for Timing Advance).
	
@leave	Leaves if underlying ETEL.DLL returns error,
or if we can't find a suitable Phone or can't open it.
*/
void CSuplNetworkInfoHandler::InitialisePhonesL()
	{
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::InitialisePhonesL() Begin\n");
	TInt err(KErrNone);
		
	// Read the name of the .tsy to load from CommDB
	TBuf<KCommsDbSvrMaxFieldLength> tsyName;
	GetCommDbTSYnameL(tsyName);

	// Load .tsy into ETel
	User::LeaveIfError(iTelServer.LoadPhoneModule(tsyName));

	// Find available phones; check that the one we requested 
	// to be loaded is there.
	TInt phoneIndex(0);
	User::LeaveIfError(iTelServer.EnumeratePhones(phoneIndex));
	while(phoneIndex-->0)
		{
		TName searchTsyName;
		// Check whether this phone belongs to loaded TSY
		if ((iTelServer.GetTsyName(phoneIndex, searchTsyName) == KErrNone) 
			&& (searchTsyName.CompareF(tsyName) == KErrNone)) 
			break;
		}
		
	// Open a phone to be used for monitoring the network.
	RTelServer::TPhoneInfo phoneInfo;
	User::LeaveIfError(iTelServer.GetPhoneInfo(phoneIndex, phoneInfo));	
	User::LeaveIfError(iNetPhone.Open(iTelServer, phoneInfo.iName));
	
	// Check that the phone is ready to be used.	
	RPhone::TStatus status;
	User::LeaveIfError(iNetPhone.GetStatus(status));
	if(status.iModemDetected != RPhone::EDetectedPresent)
		{		
		err = iNetPhone.Initialise();
		if(err != KErrNone)
			{
			iNetPhone.Close();
			User::Leave(err);
			}
		}	

	// Check that network info can be accessed
	TUint32 networkCaps;
	User::LeaveIfError(iNetPhone.GetNetworkCaps(networkCaps));
	if (!(networkCaps & RMobilePhone::KCapsGetCurrentNetwork
		  && networkCaps & RMobilePhone::KCapsNotifyCurrentNetwork))
		{
		// No point in continuing
		err = KErrNotSupported;
		}
	else
		{
		if(networkCaps & RMobilePhone::KCapsGetHomeNetwork)
			{
			iHomeNetworkInfoSupport = ETrue;
			}
		
		// Network info can be accessed.

		// Check if Timing Advance is supported by the TSY
		// (If it isn't, that is NOT a critical error, so donnot
		// change the returned error code)
		iTimingAdvSupport = EFalse;
		iTelServer.IsSupportedByModule(tsyName, KEtelFuncCellInfo, iTimingAdvSupport);
		if (iTimingAdvSupport)
			{
			// Open a new phone for Get/NotifyCellInfo() requests			
			if (KErrNone ==	iCellPhone.Open(iTelServer, phoneInfo.iName))
				{
				// Check that iCellPhone is ready to be used	
				if(KErrNone == iCellPhone.GetStatus(status))
					{
					if(status.iModemDetected != RPhone::EDetectedPresent)
						{
						if(KErrNone != iCellPhone.Initialise())
							{
							// Unable to initialise phone
							iCellPhone.Close();
							iTimingAdvSupport = EFalse;
							}
						}
					}
				else
					{
					// Unable to get phone status
					iCellPhone.Close();
					iTimingAdvSupport = EFalse;					
					}
				}
			else
				{
				// Unable to open phone
				iTimingAdvSupport = EFalse;	
				}
			}
		}
	User::LeaveIfError(err);
	
	iMsisdnSupport = EFalse;
	if(iTelServer.IsSupportedByModule(tsyName, KETelFuncMobileOwnNumberStore, iMsisdnSupport)==KErrNone && 
			iMsisdnSupport)
		{
		if(iOnStore.Open(iNetPhone)!=KErrNone)
			{
			iMsisdnSupport=EFalse;
			}
		}
	
	SUPLLOG(ELogP1, "CSuplNetworkInfoHandler::InitialisePhonesL() End\n");
	}

/**
Checks and fixes if necessary the network info
*/
TBool CSuplNetworkInfoHandler::ValidateNetInfo()
	{
	TUint val = 0;
	TLex lex(iNetworkInfo.iCountryCode);
	TInt err = lex.Val(val);
	if(err!=KErrNone || val > KMaxMccOrMnc)
		{
		return EFalse;
		}
	lex.Assign(iNetworkInfo.iNetworkId);
	err=lex.Val(val);
	if(err!=KErrNone || val > KMaxMccOrMnc)
		{
		return EFalse;
		}
			
	if(iLocArea.iAreaKnown && (iLocArea.iLocationAreaCode > KMaxCellIdOrLac || 
			iLocArea.iCellId > KMaxCellIdOrLac))
		{
		return EFalse;
		}
	return ETrue;
	}
