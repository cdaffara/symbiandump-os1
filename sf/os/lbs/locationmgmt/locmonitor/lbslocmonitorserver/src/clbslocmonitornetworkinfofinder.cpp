/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

	
	
#include <e32debug.h>
#include <e32base.h>
#include <e32property.h>
#include <etelmm.h>
#include <rsshared.h>
#include "clbslocmonitornetworkinfofinder.h"
#include "lbsdevloggermacros.h"


#include <commsdattypesv1_1.h>
using namespace CommsDat;

const TUint KMaxGsmCellIdOrLac = 65535;
const TUint KMaxWcdmaCellId = 0xFFFFFFF;   // maximum of 28 bits, least significant (16 bits short cell id + 12 RNC id)
const TUint KMaxMccOrMnc = 999;

/**
*/
CLbsLocMonitorNetworkInfoFinder::CLbsLocMonitorNetworkInfoFinder() :
	iNetworkInfoPckg(iNetworkInfo),
	iState(EStateNull)
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::CLbsLocMonitorNetworkInfoFinder() Begin\n");
	CActiveScheduler::Add(this);
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::CLbsLocMonitorNetworkInfoFinder() End\n");
	}

/**
*/	
CLbsLocMonitorNetworkInfoFinder::~CLbsLocMonitorNetworkInfoFinder()
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::~CLbsLocMonitorNetworkInfoFinder() Begin\n");
	Cancel();
	
	iNetPhone.Close();
	iTelServer.Close();
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::~CLbsLocMonitorNetworkInfoFinder() End\n");
	}

/**
*/	
CLbsLocMonitorNetworkInfoFinder* CLbsLocMonitorNetworkInfoFinder::NewL()
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::NewL() Begin\n");
	CLbsLocMonitorNetworkInfoFinder* self = new (ELeave) CLbsLocMonitorNetworkInfoFinder();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::NewL() End\n");
	return self;
	}

/**
*/
void CLbsLocMonitorNetworkInfoFinder::ConstructL()
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::ConstructL() Begin\n");
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::ConstructL() End\n");
	}

/**
*/
void CLbsLocMonitorNetworkInfoFinder::StartGettingNetworkInfoL()
{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::StartGettingNetworkInfo() Begin\n");

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

	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::StartGettingNetworkInfo() End\n");
}

/** 
*/
void CLbsLocMonitorNetworkInfoFinder::GetNetworkInfoL()
	{
	LBSLOG(ELogP1, "->CLbsLocMonitorNetworkInfoFinder::GetNetworkInfoL() Begin\n");
	InitialisePhoneL();
	
	// Connected to ETel successfully.
	// Get the network info
	iState = EStateGettingNetInfo;
	iNetPhone.GetCurrentNetwork(iStatus, iNetworkInfoPckg, iLocArea);
	SetActive();
	LBSLOG(ELogP1, "<-CLbsLocMonitorNetworkInfoFinder::GetNetworkInfoL() End\n");
	}

/**
*/
void CLbsLocMonitorNetworkInfoFinder::RunL()
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::RunL() Begin\n");
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
		case EStateNetInfoChangeMon:
			{
			TLbsLocMonitorAreaInfoGci areaInfo;
			ValidateNetInfo(areaInfo);
			NotifyObservers(areaInfo);

			MonitorNetworkChange(); // Keep monitoring
			break;				
			}

		default:
		break;
		
		}
	
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::RunL() End\n");
	}

/**
*/	
void CLbsLocMonitorNetworkInfoFinder::DoCancel()
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::DoCancel() Begin\n");
	// Check what stage we in are to find out what to cancel
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
		case EStateNetInfoChangeMon:
			{
			iNetPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
			break;
			}
			
		default:
			break;
		}

	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::DoCancel() End\n");
	}

/**
*/	
TInt CLbsLocMonitorNetworkInfoFinder::RunError(TInt /*aError*/)
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::RunError() Begin\n");
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::RunError() End\n");
	return KErrNone;
	}



/** Asychronous request to obtain notifications
of change in the current network data
*/
void CLbsLocMonitorNetworkInfoFinder::MonitorNetworkChange()
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::MonitorNetworkChange() Begin\n");
	iState = EStateNetInfoChangeMon;
	iNetPhone.NotifyCurrentNetworkChange(iStatus, iNetworkInfoPckg, iLocArea);
	SetActive();
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::MonitorNetworkChange() End\n");
	}

/** Retrieves the name of the .tsy to use from CommDB/CommsDat

Hard-coded to use the .tsy file specified in the ModemBearerEntry
which is specified (by index) by the KCDTIdModemPhoneServicesSMS
setting in the global settings table.
*/
void CLbsLocMonitorNetworkInfoFinder::GetCommDbTSYnameL(TDes& aTsyName)
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::GetCommDbTSYnameL() Begin\n");
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
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::GetCommDbTSYnameL() End\n");
	}

/** Finds and opens the phones to be used to monitor current network
info.

@leave	Leaves if underlying ETEL.DLL returns error,
or if we can't find a suitable Phone or can't open it.
*/
void CLbsLocMonitorNetworkInfoFinder::InitialisePhoneL()
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::InitialisePhonesL() Begin\n");
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

	// Check that current network info can be accessed
	TUint32 networkCaps;
	User::LeaveIfError(iNetPhone.GetNetworkCaps(networkCaps));
	if (!(networkCaps & RMobilePhone::KCapsGetCurrentNetwork
		  && networkCaps & RMobilePhone::KCapsNotifyCurrentNetwork))
		{
		// No point in continuing
		User::Leave(KErrNotSupported);
		}
	
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::InitialisePhonesL() End\n");
	}

/**
Checks and fixes if necessary the network info
*/
TBool CLbsLocMonitorNetworkInfoFinder::ValidateNetInfo(TLbsLocMonitorAreaInfoGci& aAreaInfo)
	{
	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::ValidateNetInfo()Begin\n");

	aAreaInfo.iValidity = EFalse;

	TLex lex(iNetworkInfo.iCountryCode);
	TInt err = lex.Val(aAreaInfo.iMcc);
	if(err!=KErrNone || aAreaInfo.iMcc > KMaxMccOrMnc)
		{	
		return EFalse;
		}

	lex.Assign(iNetworkInfo.iNetworkId);
	err=lex.Val(aAreaInfo.iMnc);
	if(err!=KErrNone || aAreaInfo.iMnc > KMaxMccOrMnc)
		{	
		return EFalse;
		}

	if (!iLocArea.iAreaKnown)
	    {
	    return (EFalse);
	    }
	else 
	    {
	    // We only know about registration with GSM and WCDMA cells (so far!).
	    __ASSERT_DEBUG(iNetworkInfo.iMode == RMobilePhone::ENetworkModeGsm ||
	                   iNetworkInfo.iMode == RMobilePhone::ENetworkModeWcdma, User::Invariant());
	    
	    // Do some checks on the LAC and cell id (16 bits on GSM, up to 28 bits on WCDMA
	    if(iLocArea.iLocationAreaCode > KMaxGsmCellIdOrLac || 
		  ((iLocArea.iCellId > KMaxGsmCellIdOrLac) && (iNetworkInfo.iMode == RMobilePhone::ENetworkModeGsm)) ||
		  (iLocArea.iCellId > KMaxWcdmaCellId))
	        {	
	        return EFalse;			
	        }
	    else
            {
	        // everything looks fine
            aAreaInfo.iLac = iLocArea.iLocationAreaCode;
            aAreaInfo.iCid = iLocArea.iCellId;
            aAreaInfo.iIs3gNetworkMode = iNetworkInfo.iMode == RMobilePhone::ENetworkModeWcdma ? ETrue : EFalse;
            }
        }   // end else area is known

	// Reaching the end here implies we did't find any problems.
	aAreaInfo.iValidity = ETrue;

	LBSLOG(ELogP1, "CLbsLocMonitorNetworkInfoFinder::ValidateNetInfo()End\n");

	return ETrue;
	}
