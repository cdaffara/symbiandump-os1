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
// This file defines the class that handles the interface with the 
// network on behalf of the SUPL Protocol Module.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/
#include "suplconnectionmanager.h"
class CSuplSessionRecord;

CSuplConnectionManager* CSuplConnectionManager::iInstance = NULL;// initialize pointer


//=============================================================================
// CSuplConnectionManager
//=============================================================================

/** Static public constructor.
@return A new instance of the CSuplConnectionManager class.
*/
EXPORT_C CSuplConnectionManager* CSuplConnectionManager::NewL()
	{
	//LBSLOG(ELogP2, "CSuplConnectionManager::NewL");

	if (iInstance == NULL)
		{
		CSuplConnectionManager* self = new (ELeave) CSuplConnectionManager();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		iInstance = self;
		}
	return iInstance;
	}


/** Standard destructor.
*/ 
EXPORT_C CSuplConnectionManager::~CSuplConnectionManager()
	{
	//LBSLOG(ELogP2, "CSuplConnectionManager::~CSuplConnectionManager");
	if(iSessionRecord != NULL)
		{
		delete iSessionRecord;
		iSessionRecord = NULL;
		}
		
	iInstance = NULL;
	}


/** Static public destructor. This is called from the test code.
Production code always calls the non-static destructor first.
When test code calls this method the object may or may not have been
destroyed before. Non-static destructor must not be called after calling
this mehod.
@return A new instance of the CSuplConnectionManager class.
*/
EXPORT_C void CSuplConnectionManager::Destroy()
	{
	//LBSLOG(ELogP2, "CSuplConnectionManager::NewL");

	if (iInstance != NULL)
		{
		delete iInstance;
		}
	}
	
/** Default constructor
*/
CSuplConnectionManager::CSuplConnectionManager()
	{
	//LBSLOG(ELogP2, "CSuplConnectionManager::CSuplConnectionManager");
	}


/** Second stage private constructor.
*/
void CSuplConnectionManager::ConstructL()
	{
	//LBSLOG(ELogP2, "CSuplConnectionManager::ConstructL");
	}


EXPORT_C void CSuplConnectionManager::SetTestObserver(MSuplConnectionManagerTestObserver* aTestObserver)
	{
	iTestObserver = aTestObserver;	
	}

// -------------   METHODS CALLED FROM STATE MACHINES (FORWARD CALLS TO TEST OBSERVER) --------------------------------

/** Handle a request to connect a session to a SUPL server
*/
EXPORT_C void CSuplConnectionManager::Connect(const TSuplLocalSessionId& aSessionId, const TLbsHostSettingsId& aSlpId,
	              const CSuplSessionRecord::TServiceType& aServiceType,
	              MSuplConnectionManagerObserver& aConnMgrObserver)
	{
	
    iSessionRecord = CSuplSessionRecord::NewL(aConnMgrObserver, aSessionId, 0, aSlpId);	

	if (iTestObserver != NULL)
		{
		iTestObserver->ConnectRequestReceived(aSessionId, aServiceType,aSlpId);		
		}	
	}

/** Handle a request to send a message to a SUPL server
*/
EXPORT_C void CSuplConnectionManager::SendMessage(const CSuplMessageBase* aMessage,const TSuplLocalSessionId& aSessionId)
	{
	if (iTestObserver != NULL)
		{
		iTestObserver->SendMessageRequestReceived(aMessage, aSessionId);		
		}
	}


/** Handle a disconnection request
*/
EXPORT_C void CSuplConnectionManager::Disconnect(const TSuplLocalSessionId& aSessionId)
	{
	if (iSessionRecord)
		{

		if (iSessionRecord->SessionId() == aSessionId)
			{
			delete iSessionRecord;
			iSessionRecord = NULL;
			}
		
		if (iTestObserver != NULL)
			{
			iTestObserver->DisconnectRequestReceived(aSessionId);		
			}
		}
	}


// --------------------   METHODS CALLED BY TEST -----------------------------------------------------------------

/** Inform the observer whose session ID is aSessionId that the connection is up
*/
EXPORT_C void CSuplConnectionManager::InjectConnectedInd(const TSuplLocalSessionId& aSessionId)
	{
	 if (iSessionRecord != NULL)
	 	{
		if (aSessionId == iSessionRecord->SessionId())
			{
			TInetAddr localIpAddress;
			localIpAddress.SetAddress(KInetAddr);
			iSessionRecord->Observer().Connected(localIpAddress);			
			}
	 	}
	}

/** Inform the observer whose session ID is aSessionId that the connection is up
*/
EXPORT_C void CSuplConnectionManager::InjectConnectionErrorInd(const TSuplLocalSessionId& aSessionId, const MSuplConnectionManagerObserver::TSuplConnectionError& aError)
	{
	 if (iSessionRecord != NULL)
	 	{
		if (aSessionId == iSessionRecord->SessionId())
			{
			iSessionRecord->Observer().ConnectionError(aError);			
			}
	 	}
	}

/** Inform the observer whose session ID is aSessionId that a SUPL message has been received
from the network
*/
EXPORT_C void CSuplConnectionManager::InjectMessageReceivedInd(const TSuplLocalSessionId& aSessionId, CSuplMessageBase* aSuplMessage)
	{
	if (iSessionRecord != NULL)
		{
		if (aSessionId == iSessionRecord->SessionId())
			{
			iSessionRecord->Observer().MessageReceived(aSuplMessage);			
			}
		}
	}

//-------------------   CSuplSessionRecord Methods ------------------------------------------------
//

CSuplSessionRecord* CSuplSessionRecord::NewL(MSuplConnectionManagerObserver& aObserver,
					   const TSuplLocalSessionId& aSessionId,
					   const TUint& aSocketWriterIndex,
					   const TLbsHostSettingsId& aSlpId)
{
	CSuplSessionRecord* self = new (ELeave) CSuplSessionRecord(aObserver, aSessionId,
															 aSocketWriterIndex, aSlpId);
					   
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
}

CSuplSessionRecord::CSuplSessionRecord(MSuplConnectionManagerObserver& aObserver,
					   const TSuplLocalSessionId& aSessionId,
					   const TUint& aSocketWriterIndex,
					   const TLbsHostSettingsId& aSlpId):
					   iObserver(aObserver)
	{
	iLocalSessionId = aSessionId;
	iSocketWriterIndex = aSocketWriterIndex;
	iSlpId = aSlpId;
	iServiceType = EServiceMolr;// !!!
	}
	
CSuplSessionRecord::~CSuplSessionRecord()
{

}

void CSuplSessionRecord::ConstructL()
{
	
}

MSuplConnectionManagerObserver& CSuplSessionRecord::Observer() const
	{
	return iObserver;	
	}
const TSuplLocalSessionId& CSuplSessionRecord::SessionId() const
	{
	return iLocalSessionId;
	}
const TUint& CSuplSessionRecord::SocketWriterIndex() const
	{
	return iSocketWriterIndex;
	}
const TLbsHostSettingsId CSuplSessionRecord::SlpId() const
	{
	return iSlpId;
	}



