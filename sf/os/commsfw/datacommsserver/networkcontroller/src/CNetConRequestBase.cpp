// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file CNetConRequestBase.cpp
*/

#include "CNetworkController.h"
#include "CNetConRequestBase.h"
#include "NetConPanic.h"
#include "NetConLog.h"

#include "CNetConDlgProcessor.h"

CNetConRequestBase::~CNetConRequestBase()
/**
Destructor
*/
	{
	if (iDatabase)
		{
		iDatabase->CancelRequestNotificationOfServiceChange(ipServiceChangeObserver);
		delete iDatabase;
		}
	delete iDialogPrc;
	}

CNetConRequestBase::CNetConRequestBase(MNetConEnv* aController, MNetworkControllerObserver* aObserver, CStoreableOverrideSettings* aOverrides)
: iController(aController), iObserver(aObserver), iOverrides(aOverrides)
/**
Constructor

@param aDatabase, allow the requests and bearers access to the database.
@param aDialogPrc, allow the requests access to the dialog processor.
@param aController, For upcalls from the requests.
@param aObserver, define the reverse API - from a Network Controller towards NIFMAN
@param aOverrides, store and retrieve override sets to and from both streams and buffers.
*/
	{ }

void CNetConRequestBase::ConstructL()
	{
	iDatabase = iController->NewDatabaseL();
	__ASSERT_DEBUG(iDatabase, NetConPanic(NetworkController::ETelBearerBadState));
	}

void CNetConRequestBase::CancelRequest()
/**
Cancel this request
*/
	{
	// cancel any outstanding dialog processor activity
	if (iDialogPrc)
		{
		iDialogPrc->CancelEverything();
		}

	// cancel any outstanding bearer availability check
	iController->CancelBearerAvailabilityCheck();

	// Cancel Service Change Notification
	iDatabase->CancelRequestNotificationOfServiceChange(ipServiceChangeObserver);
	}

/**
accessors
send for customer input

@return iObserver, a reverse API - from a Network Controller towards NIFMAN
*/
MNetworkControllerObserver* CNetConRequestBase::Observer() const
	{
	return iObserver;
	}

/**
accessors
Network controller connection overrides.
send for customer input

@return iOverrides, override sets to and from both streams and buffers.
*/
CStoreableOverrideSettings* CNetConRequestBase::Overrides() const
	{
	return iOverrides;
	}

TBool CNetConRequestBase::SelectedServiceAvailable(TUint32 aAvailableBearerSet) const
/**
The function checks whether the Selected Service is Available

@param aAvailableBearerSet the set of available bearers
@return ETrue if the selected service is available otherwise EFalse
*/
	{

	TBool available(EFalse);

	if (iSettings.iServiceType == TPtrC(DIAL_OUT_ISP) || iSettings.iServiceType == TPtrC(DIAL_IN_ISP))
		{
		// user requested a CSD type connection but is the bearer available?
		available = (aAvailableBearerSet & KCommDbBearerCSD);
		}
	if (iSettings.iServiceType == TPtrC(LAN_SERVICE))
		{
		// user requested a CDMA type connection but is the bearer available?
		available = (aAvailableBearerSet & KCommDbBearerLAN);
		}
	if (iSettings.iServiceType == TPtrC(VPN_SERVICE))
		{
		// user requested a Vpn service...
		available = (aAvailableBearerSet & KCommDbBearerVirtual );
		}

	if (available)
		{
		LOG( NetConLog::Printf(_L("\tSelected service is available")); )
		}
	else
		{
		LOG( NetConLog::Printf(_L("\tSelected service is not available")); )
		}

	return available;
	}

void CNetConRequestBase::SelectConnection(const TConnectionPrefs& aPrefs)
/**
Display a dialog to the user asking them to select an IAP to connect to

MNetConDialogProcAccess Interface

@param aObserver the object to receive notification when the dialog box completes
@param aPrefs the connection preferences used to select this IAP
*/
	{
	const TInt err = ConstructDialogProcessor();
	if( KErrNone != err )
		{
		TConnectionSettings settings;
		MDPOSelectComplete(err, settings);
		return;
		}

	iDialogPrc->SelectConnection(*this, aPrefs);
	}

void CNetConRequestBase::SelectConnection(const TConnectionPrefs& aPrefs, TInt aLastError)
/**
Display a dialog to the user asking them to select an IAP to connect to

@param aObserver the object to receive notification when the dialog box completes
@param aPrefs the connection preferences used to select this IAP
@param aLastError if this is not the 1st connection attempt this is the error of the last attempt
*/
	{
	const TInt err = ConstructDialogProcessor();
	if( KErrNone != err )
		{
		TConnectionSettings settings;
		MDPOSelectComplete(err, settings);
		return;
		}

	iDialogPrc->SelectConnection(*this, aPrefs, aLastError);
	}

void CNetConRequestBase::SelectModemAndLocation()
/**
Display a dialog to the user asking the user to select a modem and location entry from CommDb

@param aObserver the object to receive notification when the dialog box completes
*/
	{
	const TInt err = ConstructDialogProcessor();
	if( KErrNone != err )
		{
		TConnectionSettings settings;
		MDPOSelectModemAndLocationComplete(err, settings);
		return;
		}

	iDialogPrc->SelectModemAndLocation(*this);
	}

void CNetConRequestBase::WarnNewConnection(const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError)
/**
Display a dialog to the user warning that a new connection is about to be attempted to the specified IAP

@param aObserver the object to receive notification when the dialog box completes
@param aPrefs the connection preferences used to select this IAP
@param aNewIapName the name of the IAP used for the connection.
@param aNewConnectionNames this parameter is no-longer used.
@param aLastError if this is not the 1st connection attempt this is the error of the last attempt
*/
	{
	const TInt err = ConstructDialogProcessor();
	if( KErrNone != err )
		{
		MDPOWarnComplete(err, EFalse);
		return;
		}

	iDialogPrc->WarnNewConnection(*this, aPrefs, aNewIapName, aNewConnectionNames, aLastError);
	}

void CNetConRequestBase::Reconnect()
/**
Display a dialog to the user asking whether to reconnect a failed connection

@param aObserver the object to receive notification when the dialog box completes
*/
	{
	const TInt err = ConstructDialogProcessor();
	if( KErrNone != err )
		{
		MDPOReconnectComplete(err);
		return;
		}

	iDialogPrc->Reconnect(*this);
	}


/**
Ensure the request dialog processor is correctly constructed. Construct it if not.

@return KErrNone on success, and error code otherwise
*/
TInt CNetConRequestBase::ConstructDialogProcessor()
	{
	if (!iDialogPrc)
		{
		// construct dialog processor
		TRAPD( err, iDialogPrc = iController->NewDialogProcessorL() );
		return err;
		}
	__ASSERT_DEBUG(iDialogPrc, NetConPanic(NetworkController::ETelBearerBadState));
	return KErrNone;
	}

/**
Return the Request's internal database access pointer
*/
CCommsDbAccess* CNetConRequestBase::DbAccess()
    {
	__ASSERT_DEBUG(iDatabase, NetConPanic(NetworkController::ETelBearerBadState));
    return iDatabase;
    }

