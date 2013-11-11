// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Object to monitor the ETel network registration status and 
// broadcast it to the other components of the LBS sub-system.
// 
//

#include <e32base.h>
#include <e32property.h>
#include <etelmm.h>
#include <rsshared.h>
#include "lbsdevloggermacros.h"
#include "netregstatusmonitor.h"

using namespace CommsDat;


/**
*/
CNetworkRegistrationStatusMonitor::CNetworkRegistrationStatusMonitor(
		MNetworkRegistrationStatusObserver& aObserver) :
	CActive(EPriorityStandard),
	iPhoneNetRegStatus(RMobilePhone::ENotRegisteredNoService),
	iCurrentNetRegStatus(RLbsNetworkRegistrationStatus::ENotRegistered),
	iLastGoodNetRegStatus(RLbsNetworkRegistrationStatus::ENotRegistered),
	iInitialised(EFalse),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/**
*/	
CNetworkRegistrationStatusMonitor::~CNetworkRegistrationStatusMonitor()
	{
	Cancel();
	//Make sure we have finished the initialisation state, before attempting to close these
	if(iInitialised)
		{
		iPhone.Close();
		iTelServer.Close();
		iLbsNetRegStatus.Close();
		}
	}

/**
*/	
CNetworkRegistrationStatusMonitor* CNetworkRegistrationStatusMonitor::NewL(MNetworkRegistrationStatusObserver& aObserver)
	{
	CNetworkRegistrationStatusMonitor* self = new (ELeave) CNetworkRegistrationStatusMonitor(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
*/
void CNetworkRegistrationStatusMonitor::ConstructL()
	{	
    LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor::ConstructL++");
	iConfigurationProperty.Attach(KUidSystemCategory, RootServer::KUidC32StartPropertyKey.iUid); // needs the KEY
	TInt propertyValue = RootServer::EInitialising; // set to safe state
	TInt propertyResult = iConfigurationProperty.Get(propertyValue);
	
	if(propertyValue < RootServer::EConfigurationComplete)
		{
		//Attempt to connect to the ETel Server.  This will start C32Start if it has not already been started by the startup routine.
		User::LeaveIfError(iTelServer.Connect());
		LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor Connected to ETel");		
		iConfigurationProperty.Subscribe(iStatus);
		SetActive();
		}
	else
		{
		//Property already present, continue usual initialisation
		SetupPhoneL();
		iInitialised = ETrue;
		}
	LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor::ConstructL++");
	}

void CNetworkRegistrationStatusMonitor::SetupPhoneL()
	{
    LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor::SetupPhoneL++");
	// Connect to the Network Registration Status internal LBS bus
	iLbsNetRegStatus.OpenL();
	LBSLOG(ELogP1, "attached to NetworkRegistrationStatus property");	
	TRAPD(err, InitialisePhoneL());
	if (err == KErrNone)
		{
		// Connected to ETel successfully, now get the network 
		// registration status.
		iPhone.GetNetworkRegistrationStatus(iStatus, iPhoneNetRegStatus);
		LBSLOG2(ELogP2, "Current network registration status: %S", &NetRegStatusToDesC(iPhoneNetRegStatus)); 
		SetActive();
		}
	else
		{		
	    LBSLOG(ELogP2, "couldn't connect to ETEL, setting network reg status to ENotRegistered");    
		// Couldn't connect to ETel; publish a default value.
		iLbsNetRegStatus.SetNetworkRegistrationStatus(RLbsNetworkRegistrationStatus::ENotRegistered);
		}
	LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor::SetupPhoneL--");
	}

/**
*/
void CNetworkRegistrationStatusMonitor::RunL()
	{
    LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor::RunL++");
	User::LeaveIfError(iStatus.Int());
	
	//Check to see whether the initialisation has finished
	if(iInitialised == EFalse)
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
			iInitialised = ETrue;
			
			SetupPhoneL();
			}
		
		}
	else
		{
		LBSLOG2(ELogP5, "Current network registration status: %S", &NetRegStatusToDesC(iPhoneNetRegStatus));
	
		// Re-subscribe to monitor any further changes.
		MonitorNetworkRegistrationStatus();
	
		// Publish the new network registration status to LBS components.
		switch (iPhoneNetRegStatus)
			{
			case RMobilePhone::ERegisteredOnHomeNetwork:
				{
				iCurrentNetRegStatus = RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork;
				break;
				}
			case RMobilePhone::ERegisteredRoaming:
				{
				iCurrentNetRegStatus = RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork;
				break;
				}
			// All of the following should result in 'not registered' status
			case RMobilePhone::ENotRegisteredNoService:
			case RMobilePhone::ENotRegisteredEmergencyOnly:
			case RMobilePhone::ENotRegisteredSearching:
			case RMobilePhone::ERegisteredBusy:
			case RMobilePhone::ERegistrationDenied:
			case RMobilePhone::ERegistrationUnknown:
			default:
				{
				iCurrentNetRegStatus = RLbsNetworkRegistrationStatus::ENotRegistered;
				break;
				}
			}
		iLbsNetRegStatus.SetNetworkRegistrationStatus(iCurrentNetRegStatus);
		
		if (iCurrentNetRegStatus != RLbsNetworkRegistrationStatus::ENotRegistered)
			{
			iLastGoodNetRegStatus = iCurrentNetRegStatus;
			}
		
		LBSLOG2(ELogP5, "Network registration status broadcast to LBS: %S", &LbsNetRegStatusToDesC(iCurrentNetRegStatus));
		LBSLOG2(ELogP5, "Last Good Network registration status: %S", &LbsNetRegStatusToDesC(iLastGoodNetRegStatus));
		
		iObserver.OnNetworkRegistrationStatusChange(iCurrentNetRegStatus);
		}
	LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor::RunL--");
	}

/**
*/	
void CNetworkRegistrationStatusMonitor::DoCancel()
	{
	//Check to see what stage we in are to find out what AO to cancel
	if(iInitialised)
		{
		//We have initialised, so cancel RPhone request
		iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);
		}
	else
		{
		//Still attempting to initialise, so cancel RootServer RProperty request
		iConfigurationProperty.Cancel();
		}
	}

/**
*/	
TInt CNetworkRegistrationStatusMonitor::RunError(TInt /*aError*/)
	{
	//TODO: add in a panic here?
	return KErrNone;
	}

/**
*/
void CNetworkRegistrationStatusMonitor::MonitorNetworkRegistrationStatus()
	{
	iPhone.NotifyNetworkRegistrationStatusChange(iStatus, iPhoneNetRegStatus);
	SetActive();	
	}

/**
*/
RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus CNetworkRegistrationStatusMonitor::GetCurrentNetworkRegistrationStatus()
	{
	return iCurrentNetRegStatus;
	}

/**
*/
RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus CNetworkRegistrationStatusMonitor::GetLastGoodNetworkRegistrationStatus()
	{
	return iLastGoodNetRegStatus;
	}

/** Retrieves the name of the .tsy to use from CommDB/CommsDat

The network gateway is hard-coded to use the .tsy file specified in
the ModemBearerEntry which is specified (by index) by the
KCDTIdModemPhoneServicesSMS setting in the global settings table.
*/	
void CNetworkRegistrationStatusMonitor::GetCommDbTSYnameL(TDes& aTsyName)
	{
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
	}

/** Finds and opens the phone to be used to monitor the network registration status.

@leave	Leaves if underlying ETEL.DLL returns error,
or if we can't find a suitable Phone or can't open it.
*/
void CNetworkRegistrationStatusMonitor::InitialisePhoneL()
	{	
    LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor::InitialisePhoneL++");
	TInt err(KErrNone);
	
	User::LeaveIfError(iTelServer.Connect());
	LBSLOG(ELogP1, "connected to RTelServer");
	// Read the name of the .tsy to load from CommDB
	TBuf<KCommsDbSvrMaxFieldLength> tsyName;
	GetCommDbTSYnameL(tsyName);
	
	// Load .tsy into ETel
	err = iTelServer.LoadPhoneModule(tsyName);
	
	LBSLOG2(ELogP2, "TSY Name: %S", &tsyName);
	User::LeaveIfError(err);
	LBSLOG(ELogP2, "TSY loaded");
	// Find available phones; check that the one we requested 
	// to be loaded is there.
	TInt phoneIndex(0);
	User::LeaveIfError(iTelServer.EnumeratePhones(phoneIndex));
	LBSLOG2(ELogP2, "phones enumerated. There are %d", phoneIndex);
	while(phoneIndex-->0)
		{
		TName searchTsyName;
		// Check whether this phone belongs to loaded TSY
		if ((iTelServer.GetTsyName(phoneIndex, searchTsyName) == KErrNone) 
			&& (searchTsyName.CompareF(tsyName) == KErrNone)) 
		    {
		    LBSLOG(ELogP2, "Found phone belonging to the loaded TSY");
			break;
		    }
		}
		
	// Open a phone to be used for monitoring the network registration status.
	RTelServer::TPhoneInfo phoneInfo;
	User::LeaveIfError(iTelServer.GetPhoneInfo(phoneIndex, phoneInfo));	
	LBSLOG(ELogP2, "Got phone info from RTelServer");
	err = iPhone.Open(iTelServer, phoneInfo.iName);
	if(KErrNone != err)
	    {
	    LBSLOG2(ELogP2, "error %d returned from RMobilePhone::Open()", err);
	    }
	User::LeaveIfError(err);
	LBSLOG2(ELogP2, "%S phone opened", &phoneInfo.iName);
	// Check that the phone is ready to be used.	
	RPhone::TStatus status;
	User::LeaveIfError(iPhone.GetStatus(status));
	LBSLOG3(ELogP2, "Phone status: modem detected = %d, mode = %d", status.iModemDetected, status.iMode);
	if(status.iModemDetected != RPhone::EDetectedPresent)
		{		
	    
		err = iPhone.Initialise();
		if(err != KErrNone)
			{
		    LBSLOG2(ELogP2, "Error %d on initialising phone", err);
			iPhone.Close();
			User::Leave(err);
			}
		}	

	// Check that we can access the info we want
	TUint32 networkCaps;
	err = iPhone.GetNetworkCaps(networkCaps);
	if(KErrNone != err)
	    {
	    LBSLOG2(ELogP2, "Error %d on getting network caps", err);
	    }
	User::LeaveIfError(err);
	LBSLOG2(ELogP2, "Network caps = 0x%x", networkCaps);
	if (!(networkCaps & RMobilePhone::KCapsGetRegistrationStatus
		  && networkCaps & RMobilePhone::KCapsNotifyRegistrationStatus))
		{
	    LBSLOG(ELogP2, "Leaving with KErrNotSupported (registration status not supported by phone)");
		err = KErrNotSupported;
		}
	
	User::LeaveIfError(err);
	LBSLOG(ELogP1, "CNetworkRegistrationStatusMonitor::InitialisePhoneL--");
	}

#ifdef ENABLE_LBS_DEV_LOGGER
/** Logging-only function to return a meaningful description of an enum value.
*/
const TDesC& CNetworkRegistrationStatusMonitor::NetRegStatusToDesC(RMobilePhone::TMobilePhoneRegistrationStatus iRegStatus)
	{	
	switch (iRegStatus)
		{
		case RMobilePhone::ERegistrationUnknown:
			{
			_LIT(KRegistrationUnknown, "ERegistrationUnknown");
			return KRegistrationUnknown();
			}
		case RMobilePhone::ENotRegisteredNoService:
			{
			_LIT(KNotRegisteredNoService, "ENotRegisteredNoService");
			return KNotRegisteredNoService();
			}
		case RMobilePhone::ENotRegisteredEmergencyOnly:
			{
			_LIT(KNotRegisteredEmergencyOnly, "ENotRegisteredEmergencyOnly");
			return KNotRegisteredEmergencyOnly();
			}
		case RMobilePhone::ENotRegisteredSearching:
			{
			_LIT(KNotRegisteredSearching, "ENotRegisteredSearching");
			return KNotRegisteredSearching();
			}
		case RMobilePhone::ERegisteredBusy:
			{
			_LIT(KRegisteredBusy, "ERegisteredBusy");
			return KRegisteredBusy();
			}
		case RMobilePhone::ERegisteredOnHomeNetwork:
			{
			_LIT(KRegisteredOnHomeNetwork, "ERegisteredOnHomeNetwork");
			return KRegisteredOnHomeNetwork();
			}
		case RMobilePhone::ERegistrationDenied:
			{
			_LIT(KRegistrationDenied, "ERegistrationDenied");
			return KRegistrationDenied();
			}
		case RMobilePhone::ERegisteredRoaming:
			{
			_LIT(KRegisteredRoaming, "ERegisteredRoaming");
			return KRegisteredRoaming();
			}
		}
	
	_LIT(KUnrecognisedRegistrationStatus, "UnrecognisedRegistrationStatus");
	return KUnrecognisedRegistrationStatus();
	}

/** Logging-only function to return a meaningful description of an enum value.
*/
const TDesC& CNetworkRegistrationStatusMonitor::LbsNetRegStatusToDesC(RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus iRegStatus)
	{
	switch (iRegStatus)
		{
		case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
			{
			_LIT(KNetworkRegistrationUnknown, "ENetworkRegistrationUnknown");
			return KNetworkRegistrationUnknown();
			}
		case RLbsNetworkRegistrationStatus::ENotRegistered:
			{
			_LIT(KNotRegistered, "ENotRegistered");
			return KNotRegistered();
			}
		case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
			{
			_LIT(KRegisteredHomeNetwork, "ERegisteredHomeNetwork");
			return KRegisteredHomeNetwork();
			}
		case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
			{
			_LIT(KRegisteredRoamingNetwork, "ERegisteredRoamingNetwork");
			return KRegisteredRoamingNetwork();
			}
		}
	
	_LIT(KUnrecognisedRegistrationStatus, "UnrecognisedRegistrationStatus");
	return KUnrecognisedRegistrationStatus();	
	}
#endif // ENABLE_LBS_DEV_LOGGER

