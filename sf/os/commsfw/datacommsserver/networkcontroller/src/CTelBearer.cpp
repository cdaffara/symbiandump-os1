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
 @file CTelBearer.cpp
*/

#include "CTelBearer.h"
#include "NetConLog.h"
#include "NetConError.h"
#include "NetConPanic.h"
#include <comms-infras/dbaccess.h>
#include <agentdialog.h>  // for TConnectionPrefs
#include "CNetworkController.h"

CTelBearer::~CTelBearer()
/**
Destructor
*/
	{
	

	CloseTelephony();

	if(iStartCheckingCb)
		{
		iStartCheckingCb->Cancel();
		}

	// Note:  iBearerCompleteCb is declared in the base class CBearerBase.
	if(iBearerCompleteCb)
		{
		iBearerCompleteCb->Cancel();
		}

	delete iStartCheckingCb;
	iStartCheckingCb = NULL;

	delete iBearerCompleteCb;
	iBearerCompleteCb = NULL;
	
	delete iTsyName;
	iTsyName = NULL;
	}

CTelBearer* CTelBearer::NewLC(MBearerObserver* aObserver)
/**
Factory function - creates a new CTelBearer
On return the new object is left on the cleanup stack

@param aObserver, checks for bearer availability 
@param aDatabase,allow the requests and bearers access to the database..
@return the new CTelBearer object
@exception leaves if construction fails
*/
	{

	CTelBearer* self = new(ELeave) CTelBearer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTelBearer* CTelBearer::NewL(MBearerObserver* aObserver)
/**
Factory function - creates a new CTelBearer

@param aObserver, checks for bearer availability 
@param aDatabase,allow the requests and bearers access to the database..
@return the new CTelBearer object
@exception leaves if construction fails
*/
	{

	CTelBearer* self = CTelBearer::NewLC(aObserver);
	CleanupStack::Pop(); // self
	return self;
	}

CTelBearer::CTelBearer(MBearerObserver* aObserver)
: CBearerBase(aObserver)
/**
Constructor
*/
	{ }

void CTelBearer::ConstructL()
/**
2nd phase of construction

@exception leaves if 2nd phase construction fails 
*/
	{

	// call base class ConstructL()
	CBearerBase::ConstructL();

	// Set up start checking callback
	TCallBack callback(StartCheckingCb, this);
	iStartCheckingCb = new (ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);
	}

void CTelBearer::AssignSupportedBearerSet()
/**
Assign the supported telephony bearer set.
*/
	{
	
	// identify bearer type of this object
	iSupportedBearerSet = (KCommDbBearerCSD | KCommDbBearerPSD);
	}

void CTelBearer::Disconnect()
/**
Trigger all agents on this bearer to disconnect
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tDisconnect()")); )

	}

TBool CTelBearer::StartChecking()
/**
Start checking for availability

@return ETrue if available else EFalse
*/
	{
	return StartChecking(EFalse);
	}

TBool CTelBearer::StartChecking(TBool aIsReconnect)
/**
Start checking for availability

@param aIsReconnect, if this is a reconnection then any asynchronous requests are skipped in order to speed things up.
@return ETrue if available else EFalse
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tStartChecking()")); )

	if (iChecking)
		return EFalse;

	iChecking = ETrue;
	iIsReconnect = aIsReconnect;
	iStartCheckingCb->CallBack();
	return ETrue;
	}

TInt CTelBearer::StartCheckingCb(TAny* aThisPtr)
/**
Callback initiated from StartChecking()
  
@param aThisPtr a pointer to the instance of this class that initiated the callback
@return KErrNone always
*/
	{
	CTelBearer* self = (CTelBearer*) aThisPtr;
	self->CheckBearerSupport();
	return KErrNone;
	}

TBool CTelBearer::StopChecking()
/**
Stop checking for availability

@return ETrue if stopped else EFalse
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tStopChecking()")); )

	if (!iChecking)
		return EFalse;

	iStartCheckingCb->Cancel();
	// Note:  iBearerCompleteCb is declared in the base class CBearerBase.
	iBearerCompleteCb->Cancel();
	iChecking = EFalse;

	// free telephony resources
	CloseTelephony();

	return ETrue;
	}

void CTelBearer::CheckBearerSupport()
/**
Find out which bearers are available
*/
	{

	LOG( NetConLog::Printf(_L("TelBearer:\tStarting to check for bearer availability")); )

	TRAPD(err, GetBearerSupportL());
	if(err!=KErrNone)
		{
		LOG( NetConLog::Printf(_L("TelBearer:\tError %d during bearer availability check"), err); )

		// something has gone wrong when trying to check availability
		// so stop checking availability and tell the observer that
		// no bearers are available
		StopChecking();
		iAvailableBearerSet = KCommDbBearerUnknown;
		UpdateObserver();
		}
	}

void CTelBearer::GetBearerSupportL()
/**
Retrieve required telephony information and work out which bearers are available

@exception leaves with one of the system-wide error codes
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tGetBearerSupportL()")); )

	OpenTelephonyL();
	RetrievePhoneCapsL();
	RetrieveNetworkModeL();
	RetrievePacketSupportL();
	}

void CTelBearer::OpenTelephonyL()
/**
Initialise telephony resources

@exception leaves with one of the system-wide error codes
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tOpenTelephonyL()")); )

	__ASSERT_DEBUG(iTelServerState == ENotConnected, NetConPanic(NetworkController::ETelBearerBadState));

	// Request pointer to CommDB's access class
	CCommsDbAccess* dbAccess = iObserver->DbAccess();

	// retrieve the TSY name from the database
	// and remove the .tsy extension if necessary
	TFileName tsyName;
	dbAccess->GetBearerAvailabilityTsyNameL(tsyName);

	if (tsyName.Right(4).CompareF(KTsyNameExtension())==0)
		{
		tsyName = tsyName.Left(tsyName.Length() - 4);
		}

	// Remember the tsy name so it can be unloaded
	if(iTsyName)
		{
		delete iTsyName;
		iTsyName = NULL;
		}
	iTsyName = tsyName.AllocL();

	// connect to ETEL
	User::LeaveIfError(iTelServer.Connect());
	iTelServerState = EServerConnected;

	// load the TSY
	User::LeaveIfError(iTelServer.LoadPhoneModule(tsyName));
	iTelServerState = EPhoneModuleLoaded;

	// open the phone module
	OpenPhoneL(iTelServer, tsyName, iMmPhone);
	iTelServerState = EPhoneOpen;
	
	// start watching signal strength
	iSigStrengthWatcher = CAgentSignalStrengthWatcher::NewL(iMmPhone);
	}

void CTelBearer::CloseTelephony()
/**
Free any telephony resources used
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tCloseTelephony()")); )

	if(iETelServerRequestsWrapper)
		{
		delete iETelServerRequestsWrapper;
		iETelServerRequestsWrapper = NULL;
		}

	if(iSigStrengthWatcher)
		{
		delete iSigStrengthWatcher;
		iSigStrengthWatcher = NULL;
		}

	switch(iTelServerState)
		{
		case EPacketOpen:
			iPacket.Close();
			// ... fall through ...
		case EPhoneOpen:
			iMmPhone.Close();
			// ... fall through ...
		case EPhoneModuleLoaded:
			__ASSERT_ALWAYS(iTsyName, NetConPanic(NetworkController::ETelBearerTsyNameMissing));
			// can't do anything if UnloadPhoneModule fails so ignore return value
			(void)iTelServer.UnloadPhoneModule(*iTsyName);
			// ... fall through ...
		case EServerConnected:
			iTelServer.Close();
			iTelServerState = ENotConnected;
			break;
		case ENotConnected:
			if (iTsyName)
				{
				delete iTsyName;
				iTsyName = NULL;
				}
			break;
		default:
			break;
		}
	}

void CTelBearer::OpenPhoneL(RTelServer& aTelServer, const TDesC& aTsyName, RMobilePhone& aMmPhone)
/**
Open the phone module (ie the TSY)

@param aTelServer the telephony server session
@param aTsyName the name of the phone module to load
@param aMmPhone the phone session in which to load the TSY
@exception leaves with one of the system-wide error codes
*/
	{
	__ASSERT_DEBUG(iTelServerState == EPhoneModuleLoaded, NetConPanic(NetworkController::ETelBearerBadState));
	
	// find out how many phones are supported (i.e. how many TSYs)
	TInt count = 0;
	User::LeaveIfError(aTelServer.EnumeratePhones(count));
	if (count <= 0)
		User::Leave(KErrNotFound);

	RTelServer::TPhoneInfo info;
	TBool found(EFalse);

	// loop through all the phones and find correct TSY
	for (TInt i = 0; i < count; ++i)
		{
		TBuf<KCommsDbSvrMaxFieldLength> currentTsyName;
		User::LeaveIfError(aTelServer.GetTsyName(i, currentTsyName));

		// remove .TSY extension if necessary
		if (currentTsyName.Right(4).CompareF(KTsyNameExtension())==0)
			{
			currentTsyName = currentTsyName.Left(currentTsyName.Length() - 4);
			}

		// compare current TSY name with the one we're looking for
		if (currentTsyName.CompareF(aTsyName)==0)
			{
			// get phone info from the TSY
			User::LeaveIfError(aTelServer.GetPhoneInfo(i, info));
			found = ETrue;
			break;
			}	
		}

	if (!found)
		User::Leave(KErrNotFound);

	// open multimode phone object
	User::LeaveIfError(aMmPhone.Open(aTelServer, info.iName));
	}

void CTelBearer::RetrievePhoneCapsL()
/**
Query the phone for basic and multimode capabilities

@exception leaves with one of the system-wide error codes
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tRetrievePhoneCapsL()")); )

	__ASSERT_DEBUG(iTelServerState == EPhoneOpen, NetConPanic(NetworkController::ETelBearerBadState));

	// retrieve the basic phone capabilities
	// (ie. whether it supports data calls)
	iPhoneCaps.iFlags = RPhone::KCapsUnknown;
	User::LeaveIfError(iMmPhone.GetCaps(iPhoneCaps));

	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tiPhoneCaps.iFlags = %d"), iPhoneCaps.iFlags); )

	// retrieve multimode phone capabilities
	User::LeaveIfError(iMmPhone.GetMultimodeCaps(iPhoneMultimodeCaps));

	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tiPhoneMultimodeCaps = %d"), iPhoneMultimodeCaps); )
	}

void CTelBearer::RetrieveNetworkModeL()
/**
Query the phone for the network mode (e.g. GSM, WCDMA, cdma95 or cdma2000)
Stores the network mode in the database (since it is needed for Mobile IP)
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tRetrieveNetworkModeL()")); )

	__ASSERT_DEBUG(iTelServerState == EPhoneOpen, NetConPanic(NetworkController::ETelBearerBadState));

	// retrieve the current network mode
	// ignore error because the phone might not be able
	// to report this information (eg. a normal modem)
	iNetworkMode = RMobilePhone::ENetworkModeUnknown;
	(void)iMmPhone.GetCurrentMode(iNetworkMode);

	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tiNetworkMode = %d"), iNetworkMode); )

	// Request pointer to CommDB's access class
	CCommsDbAccess* dbAccess = iObserver->DbAccess();

	// set the network mode in the database
	// this is required for Mobile IP
	dbAccess->SetNetworkMode(iNetworkMode);
	}

void CTelBearer::RetrievePacketSupportL()
/**
Find out if packet switched data is available

@exception leaves with one of the system-wide error codes
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tRetrievePacketSupportL()")); )

	__ASSERT_DEBUG(iTelServerState == EPhoneOpen, NetConPanic(NetworkController::ETelBearerBadState));

	if(iPhoneMultimodeCaps & (RMobilePhone::KCapsGprsSupported))
		{
		// the phone supports packet data 
		// so try and open the packet service
		User::LeaveIfError(iPacket.Open(iMmPhone));
		iTelServerState = EPacketOpen;

		// query the TSY for the attach mode
		iPacketAttachMode = RPacketService::EAttachWhenNeeded;
		TInt err = iPacket.GetAttachMode(iPacketAttachMode);
		
		// query packet network status (but only if the TSY is set to attach when possible)
		iPacketDataNetworkStatus = RPacketService::EStatusUnattached;
		if(err==KErrNone && iPacketAttachMode == RPacketService::EAttachWhenPossible)
			{
			User::LeaveIfError(iPacket.GetStatus(iPacketDataNetworkStatus));
			}
		LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tiPacketDataNetworkStatus = %d"), iPacketDataNetworkStatus); )

		// close packet service as it is no-longer needed
		iPacket.Close();
		iTelServerState = EPhoneOpen;

		if(!iIsReconnect)
			{
			// start asynchronous ETEL requests to retrieve
			// the MS class and the network registration status
			iMsClass = RPacketService::EMSClassUnknown;
			iETelServerRequestsWrapper = CASyncEtelRequestWrapper::NewL(this);
			iETelServerRequestsWrapper->StartAsyncRequests(iTelServer, iMmPhone);
			return;
			}
		}
		LOG_DETAILED( else NetConLog::Printf(_L("TelBearer:\tPacket data is not supported")); )

	CalculateAvailableBearerSet();
	}

void CTelBearer::ETelAsyncRequestsComplete(TETelAsyncRequestData aRequestData)
/**
The MS Class and Network Registration status have been retrieved
Store these values and work out which bearers are available

@param aRequestData contains the information returned from ETEL
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tETelAsyncRequestsComplete()")); )

	__ASSERT_DEBUG(iTelServerState == EPhoneOpen, NetConPanic(NetworkController::ETelBearerBadState));

	if (aRequestData.iETelAsynError.iError==KErrNone)
		{
		LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tiNetworkRegStatus = %d"), iNetworkRegStatus); )
		LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tiMsClass = %d"), iMsClass); )

		// data was retrieved sucessfully
		iNetworkRegStatus = aRequestData.iRegistrationStatus;
		iMsClass = aRequestData.iMSClass;
		}
	else
		{
		LOG( NetConLog::Printf(_L("TelBearer:\tAsync ETEL requests completed with error %d)"), aRequestData.iETelAsynError.iError); )

		iNetworkRegStatus = RPacketService::ENotRegisteredNotSearching;
		iMsClass = RPacketService::EMSClassUnknown;
		}

	// we now have all the information we need
	// so work out which bearers are available
	CalculateAvailableBearerSet();
	}

void CTelBearer::CalculateAvailableBearerSet()
/**
Adds each available bearer to the currently available bearer set
and updates the status of this bearer with the bearer manager
*/
	{

	iAvailableBearerSet = KCommDbBearerUnknown;

	if(CsdAvailable())
		iAvailableBearerSet |= KCommDbBearerCSD;
	LOG( NetConLog::Printf(_L("TelBearer:\tFinished availability check - available bearer set is %d"), iAvailableBearerSet); )

	// update the observer with the available bearers
	UpdateObserver();
	}

TBool CTelBearer::CsdAvailable() const
/**
CSD is available as long as the phone can support data

@return ETrue if CSD is available otherwise EFalse
*/
	{

	// can the phone support CSD?
	TBool supported = (iPhoneCaps.iFlags & (RPhone::KCapsData | RPhone::KCapsUnknown));

	LOG_DETAILED(
				if(supported)
					NetConLog::Printf(_L("TelBearer:\tPhone capabilities allow CSD"));
				else
					NetConLog::Printf(_L("TelBearer:\tPhone capabilities do not allow CSD"));
				)

	return supported;
	}

TBool CTelBearer::PhoneAttachedAndRegistered() const
/**
Check whether the phone is attached to the network and registered

@return ETrue if the phone is attached and registered otherwise EFalse
*/
	{

	TBool attached(EFalse);
	
	// Is the phone attached to the packet network?
	if(iPacketAttachMode == RPacketService::EAttachWhenNeeded)
		{
		// if the TSY is set to attach when needed then ignore the attachment status
		attached = ETrue;
		}
	else if (iPacketDataNetworkStatus != RPacketService::EStatusUnattached)
		{
		attached = ETrue;
		}

	TBool registered(ETrue);

	// Is the phone registered on the packet network?
	if (iNetworkRegStatus == RPacketService::ENotRegisteredNotSearching ||
			iNetworkRegStatus == RPacketService::ERegistrationDenied ||
			iNetworkRegStatus == RPacketService::ENotRegisteredAndNotAvailable)
		{
		registered = EFalse;
		}

	LOG_DETAILED(
				if(attached)
					NetConLog::Printf(_L("TelBearer:\tPhone is attached to the packet network"));
				else
					NetConLog::Printf(_L("TelBearer:\tPhone is not attached to the packet network"));

				if(registered)
					NetConLog::Printf(_L("TelBearer:\tPhone is registered on the network"));
				else
					NetConLog::Printf(_L("TelBearer:\tPhone is not registered on the network"));
				)

	return (attached && registered);
	}

TBool CTelBearer::MsClassSupportsPsd() const
/**
Check whether the MS class supports Packet Switched Data

As long as the MS Class is neither RPacketService::EMSClassAlternateMode or 
RPacketService::EMSClassCircuitSwitchedOnly then PSD is supported

i.e. the MS Class can be any of the following and still support PSD:
RPacketService::EMSClassDualMode, RPacketService::EMSClassSuspensionRequired
RPacketService::EMSClassPacketSwitchedOnly, RPacketService::EMSClassUnknown

@return ETrue if PSD is supported otherwise EFalse
*/
	{

	TBool supported = (iMsClass != RPacketService::EMSClassAlternateMode && iMsClass != RPacketService::EMSClassCircuitSwitchedOnly);

	LOG_DETAILED(
				if(supported)
					NetConLog::Printf(_L("TelBearer:\tMS class supports PSD"));
				else
					NetConLog::Printf(_L("TelBearer:\tMS class does not support PSD"));
				)

	return supported;
	}

TInt CTelBearer::SecondPhaseAvailability()
/**
Get the latest signal strength

@return KErrNone if signal strength is ok, otherwise KErrNetConnInadequateSignalStrengh or one of the system-wide error codes
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tSecondPhaseAvailability()")); )
	
	// if the watcher is missing it may be that this function has been called before
	// StartChecking().  In this case just return KErrNone
	if(!iSigStrengthWatcher)
		{
		LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tSignal strength watcher missing")); )
		return KErrNone;
		}

	const TInt KMinSignalStrengthOffset(10000);
	TInt32 sigStrengthInDbm(0);
	if(iSigStrengthWatcher->CurrentSignalStrength(sigStrengthInDbm)!=KErrNone)
		{
		// a value has not yet been retrieved so best to carry on with
		// the connection and let it fail later if it is going to fail.

		LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tNo signal strength value available")); )

		return KErrNone;
		}

	// Request pointer to CommDB's access class
	CCommsDbAccess* dbAccess = iObserver->DbAccess();

	// retrieve signal strength from commdb
	TUint32 minAcceptSignalStrength=0;
	TRAPD(ret, dbAccess->GetIntL(TPtrC(MODEM_BEARER), TPtrC(MODEM_MIN_SIGNAL_LEVEL), minAcceptSignalStrength));
	if(ret!=KErrNone)
		{
		LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tError %d when reading minimum signal strength from CommDb"), ret); )

		return ret;
		}

	if(minAcceptSignalStrength==0)
		{
		// the functionality to stop a connection if the signal strength is
		// below a certain value has been turned off in commdb so return no error

		LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tSignal strength functionality disabled in CommDb")); )

		return KErrNone;
		}

	// Convert to dBm's
	TInt minSigStrengthInDbm=STATIC_CAST(TInt, minAcceptSignalStrength);
	minSigStrengthInDbm-=KMinSignalStrengthOffset;
	if(sigStrengthInDbm < minSigStrengthInDbm)
		{
		LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tSignal strength lower than minimum acceptable value")); )

		// signal strength too low so fail the connection early
		return KErrNetConInadequateSignalStrengh;
		}

	LOG_DETAILED( NetConLog::Printf(_L("TelBearer:\tSignal strength ok")); )
	return KErrNone;
	}

