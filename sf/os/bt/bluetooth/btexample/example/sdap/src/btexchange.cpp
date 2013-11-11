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

#include <e32base.h>
#include <e32std.h>
#include "exchange.h"
#include "btexchange.h"


//Dll entry
EXPORT_C TInt _E32Dll()
	{
	return KErrNone;
	}

//Construction
/**
@internalComponent
Factory function that constructs a CBluetoothExchanger object.
@param aUUID
The Universally Unique IDentifier of the service that is provided
and should be searched for.
@return The constructed object
*/
EXPORT_C CBluetoothExchanger* CBluetoothExchanger::NewL(const TUUID &aUUID)
	{
	CBluetoothExchanger* self = CBluetoothExchanger::NewLC(aUUID);
	CleanupStack::Pop(self);
	return self;
	}

/**
@internalComponent
Factory function that constructs a CBluetoothExchanger object.
@param aUUID
The Universally Unique IDentifier of the service that is provided
and should be searched for.
@return The constructed object
*/
EXPORT_C CBluetoothExchanger* CBluetoothExchanger::NewLC(const TUUID &aUUID)
	{
	CBluetoothExchanger* self = new(ELeave) CBluetoothExchanger;
	CleanupStack::PushL(self);
	self->ConstructL(aUUID);
	return self;
	}

/**
@internalComponent
Initialises the class, and registers the service with the SDP database service.
@param aUUID
The Universally Unique IDentifier of the service that is provided
and should be searched for.
@leave If SDP registration fails, the function leaves.
*/
void CBluetoothExchanger::ConstructL(const TUUID &aUUID)
	{
	TSdpServRecordHandle recordHandle = 0;
	User::LeaveIfError(iSdpSession.Connect());
	User::LeaveIfError(iSdpDb.Open(iSdpSession));
	iSdpDb.CreateServiceRecordL(aUUID, recordHandle);
	//Service record with mandatory attributes now exists - maybe add others to it

	iSockSession.Connect();
	iSeeker = CBluetoothSeeker::NewL (iSockSession, aUUID);
	}

//Destruction
CBluetoothExchanger::~CBluetoothExchanger()
	{
	iSdpDb.Close();
	iSdpSession.Close();
	}

//Overrides of pure virtuals from CExchanger

//Inquiry & SDP implementation
/**
@internalComponent
Uses Inquiry and SDP to retrieve a list of peer devices which support
the UUID that was registered during construction.  
The functionality is implemented by the CBluetoothSeeker active object.
@param aStatus
A TRequestStatus object that is signalled when the search is complete.
@leave Leaves if the bluetooth protocols required can't be loaded.
*/
EXPORT_C void CBluetoothExchanger::LookForPeersL(TRequestStatus &aStatus)
	{
	//This is handled by the seeker class that we own.
	iSeeker->BeginUpdateL(aStatus);
	}

/**
@internalComponent
@return 
The count of devices that were found by the search.  
@pre 
A LookForPeersL search must have sucessfully completed
*/
EXPORT_C TInt CBluetoothExchanger::Count()
	{
	return iSeeker->Names().Count();
	}

/**
@internalComponent
Selects the first device that was found
*/
EXPORT_C void CBluetoothExchanger::First()
	{
	iDeviceIndex = 0;
	}

/**
@internalComponent
Retrieves the selected device, and selects the next device
that was found.
@param aPtr
A reference to a TNameEntry* pointer.  On success, it is filled
in with a pointer to a device that was found.  
@return KErrNotFound on failure, KErrNone on success.
*/
EXPORT_C TInt CBluetoothExchanger::Next(TNameEntry*& aPtr)
	{
	if(iDeviceIndex >= Count())
		return (KErrNotFound);
	aPtr = &(iSeeker->Names()[iDeviceIndex++]);
	return KErrNone;
	}

// CBluetoothSeeker 
_LIT(KBluetoothSeekerClassName, "CBluetoothSeeker");

// construction
CBluetoothSeeker::CBluetoothSeeker(RSocketServ &aSession) : CActive(CActive::EPriorityStandard), iSession(aSession), iNames(4)
	{
	iState = EIdle;
	}

CBluetoothSeeker::~CBluetoothSeeker()
	{
	iResolver.Close();
	delete iSdpAgent;
	delete iSdpSearchPattern;
	}

CBluetoothSeeker* CBluetoothSeeker::NewLC(RSocketServ &aSession, const TUUID &aUUID)
	{
	CBluetoothSeeker* self = new(ELeave) CBluetoothSeeker(aSession);
	CleanupStack::PushL(self);
	self->ConstructL(aUUID);
	return self;
	}

CBluetoothSeeker* CBluetoothSeeker::NewL(RSocketServ &aSession, const TUUID &aUUID)
	{
	CBluetoothSeeker* self = CBluetoothSeeker::NewLC(aSession, aUUID);
	CleanupStack::Pop(self);
	return self;
	}

void CBluetoothSeeker::ConstructL(const TUUID &aUUID)
	{
	iSdpSearchPattern = CSdpSearchPattern::NewL();
	iSdpSearchPattern->AddL(aUUID);
	CActiveScheduler::Add(this);
	}

void CBluetoothSeeker::RunL()
	{
	switch(iState)
		{
		case EInquiring:
			HandleInquiryResultL();
			break;
		case EServiceRequest:
			SDPQueryL();
			break;
		default:
			//RunL in bad state
			User::Panic(KBluetoothSeekerClassName, KErrGeneral);
			break;
		}
	}

TInt CBluetoothSeeker::RunError(TInt aErr)
	{
	//Complete any outstanding request with an error.
	if(iUpdateStatus)
		{
		User::RequestComplete(iUpdateStatus,aErr);
		}
	return KErrNone;
	}

void CBluetoothSeeker::DoCancel()
	{
	switch(iState)
		{
		case EInquiring:
			iResolver.Cancel();
			break;
		case EServiceRequest:
			delete iSdpAgent;
			iSdpAgent = NULL;
			break;
		default:
			break;
		}
	User::RequestComplete(iUpdateStatus, KErrCancel);
	}

void CBluetoothSeeker::BeginUpdateL(TRequestStatus &aStatus)
	{
	TProtocolDesc pInfo;
	_LIT(KLinkMan, "BTLinkManager");
	TProtocolName name(KLinkMan);
	User::LeaveIfError(iSession.FindProtocol(name,pInfo));
	if(IsActive())
		{
		User::Leave(KErrInUse);
		}
	//First, start an inquiry
	User::LeaveIfError(iResolver.Open(iSession,pInfo.iAddrFamily,pInfo.iProtocol));

	iInquiryAddress.SetIAC(KGIAC);
	iInquiryAddress.SetAction(KHostResInquiry);
	iResolver.GetByAddress(iInquiryAddress, iNameEntry, iStatus);
	SetActive();
	iState = EInquiring;
	iUpdateStatus = &aStatus;
	*iUpdateStatus = KRequestPending;
	}

CArrayFixFlat<TNameEntry>& CBluetoothSeeker::Names()
	{
	return iNames;
	}

void CBluetoothSeeker::HandleInquiryResultL()
	{
	if(iStatus.Int() == KErrNone)
		{
		//We found another device, add it to the list
		iNames.AppendL(iNameEntry);
		iResolver.Next(iNameEntry, iStatus);
		SetActive();
		}
	else if(iStatus.Int() == KErrHostResNoMoreResults)
		{
		//No more results, so now start SDP checks
		if(iNames.Count() == 0)
			{
			//No devices found by inquiry, so complete request now with a failure
			User::RequestComplete(iUpdateStatus, KErrNotFound);
			iState = EIdle;
			}
		else
			{
			//Some devices found, filter them by whether they support this service
			iDeviceIndex = 0;
			iState = EServiceRequest;
			SDPQueryL();
			}
		}
	else
		{
		//Some other error  - fail the request
		User::RequestComplete(iUpdateStatus, iStatus.Int());
		iState = EIdle;
		}
	}

void CBluetoothSeeker::SDPQueryL()
	{
	delete iSdpAgent;
	iSdpAgent = NULL;
	if(iDeviceIndex < iNames.Count())
		{
		TBTSockAddr* addr = (TBTSockAddr *)&((iNames[iDeviceIndex])().iAddr);
		iSdpAgent = CSdpAgent::NewL(*this, addr->BTAddr());
		iSdpAgent -> SetRecordFilterL(*iSdpSearchPattern);
		iSdpAgent -> NextRecordRequestL();
		// We signal ourselves when the callback completes
		iStatus = KRequestPending;
		SetActive();
		}
	else
		{
		//No more devices, set final status
		User::RequestComplete(iUpdateStatus, KErrNone);
		}
	}

//virtual overrides from MSdpAgentNotifier
void CBluetoothSeeker::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle /*aHandle*/, TInt aTotalRecordsCount)
	{
	if(aError != KErrNone || aTotalRecordsCount == 0)
		{
		//This device doesnt support the service we are looking for
		//or SDP failed and we can't tell that it does.
		//Either way, remove it from our list of supported peers.
		iNames.Delete(iDeviceIndex);
		}
	else
		{
		//This device does support the service we are looking for
		//(Attribute requests would be done here if implemented)

		//While we are connected anyway (due to SDP), do a name request
		//- its almost free at this point, so use the synchronous version
		TInquirySockAddr &addr = TInquirySockAddr::Cast(iNames[iDeviceIndex]().iAddr);
		addr.SetAction(KHostResName);
		iResolver.GetByAddress(addr, iNames[iDeviceIndex]);
		//move on and check the next device.
		iDeviceIndex++;
		}
	//Signal ourself to process the next one
	TRequestStatus *status = &iStatus;
	User::RequestComplete(status,KErrNone);
	}

void CBluetoothSeeker::AttributeRequestResult(TSdpServRecordHandle , TSdpAttributeID , CSdpAttrValue* )
	{
	//should never be called (not currently used)
	User::Panic(KBluetoothSeekerClassName, KErrNotSupported);
	}
void CBluetoothSeeker::AttributeRequestComplete(TSdpServRecordHandle, TInt )
	{
	//should never be called (not currently used)
	User::Panic(KBluetoothSeekerClassName, KErrNotSupported);
	}
