#include <e32debug.h>
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
//

/**
 @file
 @internalComponent
 @deprecated
*/
#include "suplconnectionmanager.h"
#include "socketwriter.h"
#include <lbs/lbshostsettingsclasstypes.h>
#include "supldevloggermacros.h"

EXPORT_C CSuplConnectionManager* CSuplConnectionManager::NewL()
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::NewL() Begin\n");
	CSuplConnectionManager* self = new (ELeave)CSuplConnectionManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplConnectionManager::NewL() End\n");
	CleanupStack::Pop(self);
	
	return self;
	}

CSuplConnectionManager::CSuplConnectionManager() :
	iSessionRecords(2), iSocketWriters(2), iSocketWriterNextIndex(0)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::CSuplConnectionManager() Begin\n");
	SUPLLOG(ELogP1, "CSuplConnectionManager::CSuplConnectionManager() End\n");
	}

void CSuplConnectionManager::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::ConstructL() Begin\n");
	// Open socket server
	User::LeaveIfError(iSocketServ.Connect());
	SUPLLOG(ELogP1, "CSuplConnectionManager::ConstructL() End\n");
	}

CSuplConnectionManager::~CSuplConnectionManager()
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::~CSuplConnectionManager() Begin\n");
	// Delete array allocations
	for (TInt x = 0; x < iSessionRecords.Count(); ++x)
		{
		delete iSessionRecords[x];
		}
	for (TInt x = 0; x < iSocketWriters.Count(); ++x)
		{
		delete iSocketWriters[x];
		}
	
	iSessionRecords.Close();
	iSocketWriters.Close();

	iSocketServ.Close();
	SUPLLOG(ELogP1, "CSuplConnectionManager::~CSuplConnectionManager() End\n");
	}

EXPORT_C void CSuplConnectionManager::Connect(const TSuplLocalSessionId& aSessionId, 
											  const TLbsHostSettingsId& aSlpId,
											  const CSuplSessionRecord::TServiceType& aServiceType,
											  MSuplConnectionManagerObserver& aConnMgrObserver)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::Connect() Begin\n");
	Connect(aSessionId, aSlpId, aServiceType, aConnMgrObserver, EFalse);
	SUPLLOG(ELogP1, "CSuplConnectionManager::Connect() End\n");
	}

void CSuplConnectionManager::Connect(const TSuplLocalSessionId& aSessionId, 
		  							 const TLbsHostSettingsId& aSlpId,
		  							 const CSuplSessionRecord::TServiceType& aServiceType,
		  							 MSuplConnectionManagerObserver& aConnMgrObserver,
		  							 TBool aForceNonSecure)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::Connect() Begin\n");
	TInt ignore;
	CSocketWriterBase* socketWriter = FindWriter(aSlpId, ignore);
	if (socketWriter == NULL)
		{
		// Get host settings for auth mode
		TLbsHostSettingsSupl settings;
		TRAPD(err, GetHostSettingsL(aSlpId, settings))
		if (err == KErrNone)
			{
			// Check that port value has been set
			TInt portValueInSettings;
			settings.GetPortId(portValueInSettings);
			if(portValueInSettings <= 0) // If SetPortId() was never called...
				{
				settings.SetPortId(7275); // set it to be the OMA specified port
				}
			
			// Get the security settings
			TLbsHostSettingsSupl::TAuthModes mode;
			if (aServiceType == CSuplSessionRecord::EServiceMolr)
				{
				settings.GetAuthModesMOLR(mode);
				}
			else
				{
				settings.GetAuthModesMTLR(mode);
				}
			
			// If forced into non-secure mode then check that the server supports this mode
			if (!(aForceNonSecure && (mode & TLbsHostSettingsSupl::EAuthNone) == 0))
				{
				// Which socket to use?
				TInt err = KErrNone;
				// If forced into non-secure then use a normal socket, otherwise check to see if the 
				// server supports any secure mode as these take precedence
				if (aForceNonSecure || ((mode | TLbsHostSettingsSupl::EAuthNone) == TLbsHostSettingsSupl::EAuthNone))
					{
					TRAP(err, socketWriter = CSocketWriter::NewL(aSlpId, ++iSocketWriterNextIndex, *this, iSocketServ, settings));
					}
				else
					{
					TRAP(err, socketWriter = CSecureSocketWriter::NewL(aSlpId, ++iSocketWriterNextIndex, *this, iSocketServ, settings, aServiceType));
					}
				
				if (err == KErrNone)
					{
					// Add to array of socket writers
					if (iSocketWriters.Append(socketWriter) != KErrNone)
						{
						delete socketWriter;
						socketWriter = NULL;
						}
					}
				}
			}
		}
	
	CSuplSessionRecord* suplRecord = NULL;
	if (socketWriter != NULL)
		{
		TRAP_IGNORE(suplRecord = CSuplSessionRecord::NewL(aConnMgrObserver, aSessionId, aSlpId, aServiceType, socketWriter->CallbackId()));
		}
	
	if (suplRecord == NULL)
		{
		SUPLLOG(ELogP1, "CSuplConnectionManager::Connect() Error(EUndefinedConnectionError, 1)\n");
		aConnMgrObserver.ConnectionError(MSuplConnectionManagerObserver::EUndefinedConnectionError);
		}
	else
		{
		if (iSessionRecords.Append(suplRecord) == KErrNone)
			{
			socketWriter->Connect(aSessionId);
			}
		else
			{
			delete suplRecord;
			SUPLLOG(ELogP1, "CSuplConnectionManager::Connect() Error(EUndefinedConnectionError, 2)\n");
			aConnMgrObserver.ConnectionError(MSuplConnectionManagerObserver::EUndefinedConnectionError);
			}
		}
	SUPLLOG(ELogP1, "CSuplConnectionManager::Connect() End\n");
	}

EXPORT_C void CSuplConnectionManager::Disconnect(const TSuplLocalSessionId& aSessionId)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::Disconnect() Begin\n");
	TInt at = 0;
	CSuplSessionRecord* sessionRecord = FindSession(aSessionId, at);
	if (sessionRecord != NULL) // Cant really do anything if it dont exist!
		{
		// Disconnect the socket writer
		TInt ignore;
		CSocketWriterBase* writer = FindWriter(sessionRecord->SocketWriter(), ignore);
		if (writer != NULL)
			{
			writer->Disconnect();
			}
		
		// Remove from array
		delete iSessionRecords[at];
		iSessionRecords.Remove(at);
		}
	SUPLLOG(ELogP1, "CSuplConnectionManager::Disconnect() End\n");
	}

EXPORT_C void CSuplConnectionManager::SendMessage(const CSuplMessageBase* aMessage,
												  const TSuplLocalSessionId& aSessionId)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::SendMessage() Begin\n");
	TInt ignore;
	CSuplSessionRecord* sessionRecord = FindSession(aSessionId, ignore);
	if (sessionRecord != NULL)
		{
		CSocketWriterBase* socketWriter = FindWriter(sessionRecord->SocketWriter(), ignore);
		if (socketWriter != NULL)
			{
			CSuplMessageBase* message = const_cast<CSuplMessageBase*>(aMessage);
			TRAPD(err, socketWriter->SendMessageL(message, aSessionId));
			if (err != KErrNone)
				{
				SUPLLOG(ELogP1, "CSuplConnectionManager::SendMessage Error(EFailedToSend, 1)\n");
				sessionRecord->Observer().ConnectionError(MSuplConnectionManagerObserver::EFailedToSend);
				}
			}
		else
			{
			SUPLLOG(ELogP1, "CSuplConnectionManager::SendMessage Error(EFailedToSend, 2)\n");
			sessionRecord->Observer().ConnectionError(MSuplConnectionManagerObserver::EFailedToSend);
			}
		}
	SUPLLOG(ELogP1, "CSuplConnectionManager::SendMessage() End\n");
	}
	
void CSuplConnectionManager::Connected(const TSuplLocalSessionId& aSessionId, TInetAddr& aAddr)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::Connected() Begin\n");
	TInt ignore;
	CSuplSessionRecord* sessionRecord = FindSession(aSessionId, ignore);
	if (sessionRecord != NULL)
		{
		sessionRecord->Observer().Connected(aAddr);
		}
	SUPLLOG(ELogP1, "CSuplConnectionManager::Connected() End\n");
	}

/**
 * If final parameter(aDelete) is set to True on return then the calling object should delete itself, as
 * it is no longer stored in the list of available connections.
 */

void CSuplConnectionManager::ConnectionError(const TInt& aError, const TInt aSocketWriterId, 
											 const TSuplLocalSessionId& aSessionId, TBool& aDelete)
	{
	aDelete = EFalse;
	SUPLLOG2(ELogP1, "CSuplConnectionManager::ConnectionError() Begin, aError: %d\n", aError);
	switch (aError)
		{
		case MSuplSocketObserver::EDisconnected:
			{
			// Ignore this. The socket writer will have disconnected from the server, but we wish to keep the socket
			// writer around as it stores connection information. 
			break;
			}
		case MSuplSocketObserver::EFailedToSend:
			{
			TInt ignore;
			CSuplSessionRecord* session = FindSession(aSessionId, ignore);
			if (session != NULL)
				{
				session->Observer().ConnectionError(MSuplConnectionManagerObserver::EFailedToSend);
				} // if
			else
				{
				__ASSERT_DEBUG(EFalse, User::Invariant());
				}
			break;
			}
		case MSuplSocketObserver::EFailedToHandshake:
		case MSuplSocketObserver::EFailedCertCheck:
			{
			// Attemp a non-secure connection
			TInt sessionAt = 0;
			CSuplSessionRecord* session = FindSession(aSessionId, sessionAt);
			if (session != NULL)
				{
				// Delete Originals
				TInt writerAt = 0;
				CSocketWriterBase* socketWriter = FindWriter(aSocketWriterId, writerAt);
				if (socketWriter != NULL)
					{
					aDelete = ETrue;
					iSocketWriters.Remove(writerAt);
					}
				
				iSessionRecords.Remove(sessionAt);
				
				Connect(aSessionId, session->SlpId(), session->ServiceMode(), session->Observer(), ETrue);
				
				delete session;
				}
			break;
			}
		case MSuplSocketObserver::ETimeOut:
			{
			TInt ignore;
			CSuplSessionRecord* session = FindSession(aSessionId, ignore);
			if (session != NULL)
				{
				session->Observer().ConnectionError(MSuplConnectionManagerObserver::EConnectionLost);
				}
			else
				{
				__ASSERT_DEBUG(EFalse, User::Invariant());
				}
			break;
			}
		case MSuplSocketObserver::EFailedToRead:
			{
			// Loop through the sessions and inform them of the error
			for (TInt x = 0; x < iSessionRecords.Count(); ++x)
				{
				if (iSessionRecords[x]->SocketWriter() == aSocketWriterId)
					{
					iSessionRecords[x]->Observer().ConnectionError(MSuplConnectionManagerObserver::EUndefinedConnectionError);
					} // if
				} // for			
			break;
			}
		case MSuplSocketObserver::EFailedToDecode:
		case MSuplSocketObserver::ETooMuchDataRead:
			{
			TInt ignore;
			CSuplSessionRecord* session = FindSession(aSessionId, ignore);
			if (session != NULL)
				{
				session->Observer().ConnectionError(MSuplConnectionManagerObserver::EDecodeMessageFailed);
				}
			else
				{
				__ASSERT_DEBUG(EFalse, User::Invariant());
				}
			break;
			}
		case MSuplSocketObserver::ESocketError:
		case KErrCouldNotConnect:
		case KErrNotFound:
		default:
			{
			TInt ignore;
			CSuplSessionRecord* session = FindSession(aSessionId, ignore);
			if (session != NULL)
				{
				session->Observer().ConnectionError(MSuplConnectionManagerObserver::EUndefinedConnectionError);
				}
			else
				{
				__ASSERT_DEBUG(EFalse, User::Invariant());
				}			
			break;
			}
		}
	SUPLLOG(ELogP1, "CSuplConnectionManager::ConnectionError() End\n");
	}

void CSuplConnectionManager::MessageReceived(CSuplMessageBase* aSuplMessage)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::MessageReceived() Begin\n");
	// Which session?
	CSuplSessionId* sessionId = NULL;	
	TRAPD(err, sessionId = CSuplSessionId::NewL());
	if (err == KErrNone)
		{
		aSuplMessage->GetSessionId(*sessionId);
		
		TInt ignore;
		CSuplSessionRecord* session = FindSession(sessionId->iSetSessionId->iSessionId, ignore);
		if (session != NULL)
			{
			session->Observer().MessageReceived(aSuplMessage);
			}
		delete sessionId;
		}
	SUPLLOG(ELogP1, "CSuplConnectionManager::MessageReceived() End\n");
	}

CSuplSessionRecord* CSuplConnectionManager::FindSession(TLbsHostSettingsId aId, TInt& aIndex)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::FindSession() Begin\n");
	for (TInt x = 0; x < iSessionRecords.Count(); ++x)
		{
		if (iSessionRecords[x]->SlpId() == aId)
			{
			aIndex = x;
			return iSessionRecords[x]; 
			}
		}

	SUPLLOG(ELogP1, "CSuplConnectionManager::FindSession() End\n");

	return NULL;
	}

CSuplSessionRecord* CSuplConnectionManager::FindSession(TSuplLocalSessionId aSessionId, TInt& aIndex)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::FindSession() Begin\n");
	for (TInt x = 0; x < iSessionRecords.Count(); ++x)
		{
		if (iSessionRecords[x]->SessionId() == aSessionId)
			{
			aIndex = x;
			return iSessionRecords[x];
			}
		}


    if (1 == iSessionRecords.Count())
	    {
	    // If only one session record exists, select it even
	    // if the session Id doesn't match (possible corruption
	    // of session ID. The observer will deal with the 
	    // session mismatch).
		aIndex = 0;
		return iSessionRecords[0];	    
	    }

	SUPLLOG(ELogP1, "CSuplConnectionManager::FindSession() End\n");

	return NULL;
	}

CSocketWriterBase* CSuplConnectionManager::FindWriter(TLbsHostSettingsId aId, TInt& aIndex)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::FindWriter() Begin\n");
	for (TInt x = 0; x < iSocketWriters.Count(); ++x)
		{
		if (iSocketWriters[x]->HostSettingsId() == aId)
			{
			aIndex = x;
			return iSocketWriters[x];
			}
		}

	SUPLLOG(ELogP1, "CSuplConnectionManager::FindWriter() End\n");	
	return NULL;
	}

CSocketWriterBase* CSuplConnectionManager::FindWriter(TInt aId, TInt& aIndex)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::FindWriter() Begin\n");
	for (TInt x = 0; x < iSocketWriters.Count(); ++x)
		{
		if (iSocketWriters[x]->CallbackId() == aId)
			{
			aIndex = x;
			return iSocketWriters[x];
			}
		}
	
	SUPLLOG(ELogP1, "CSuplConnectionManager::FindWriter() End\n");

	return NULL;

	}


void CSuplConnectionManager::GetHostSettingsL(TLbsHostSettingsId aId, TLbsHostSettingsSupl& aSettings)
	{
	SUPLLOG(ELogP1, "CSuplConnectionManager::GetHostSettingsL() Begin\n");
	// Open the host store
	CLbsHostSettingsStore* pHostStore = CLbsHostSettingsStore::NewL(KLbsHostSettingsSuplStoreId);
	CleanupStack::PushL(pHostStore);
	
	// Get the host settings
	User::LeaveIfError(pHostStore->GetHostSettings(aId, aSettings));
	
	CleanupStack::PopAndDestroy(pHostStore);
	SUPLLOG(ELogP1, "CSuplConnectionManager::GetHostSettingsL() End\n");
	}




/*
 * CSuplSessionRecord 
 */
CSuplSessionRecord* CSuplSessionRecord::NewL(MSuplConnectionManagerObserver& aObserver,
					   						 const TSuplLocalSessionId& aSessionId,
					   						 const TLbsHostSettingsId& aSlpId,
					   						 const TServiceType aServiceType,
					   						 TUint  aSocketWriterIndex)
	{
	SUPLLOG(ELogP1, "CSuplSessionRecord::NewL()\n");
	CSuplSessionRecord* self = new(ELeave) CSuplSessionRecord(aObserver, aSessionId, 
															  aSlpId, aServiceType, aSocketWriterIndex);
	return self;
	}

CSuplSessionRecord::~CSuplSessionRecord()
	{
	SUPLLOG(ELogP1, "CSuplSessionRecord::~CSuplSessionRecord() Begin\n");
	SUPLLOG(ELogP1, "CSuplSessionRecord::~CSuplSessionRecord() End\n");
	}

CSuplSessionRecord::CSuplSessionRecord(MSuplConnectionManagerObserver& aObserver,
					   				   const TSuplLocalSessionId& aSessionId,
					   				   const TLbsHostSettingsId& aSlpId,
					   				   const TServiceType aServiceType,
					   				   TUint aSocketWriterIndex) :
	iObserver(aObserver), iSessionId(aSessionId),
	iSlpId(aSlpId), iServiceType(aServiceType), iSocketWriterIndex(aSocketWriterIndex)	
	{
	SUPLLOG(ELogP1, "CSuplSessionRecord::CSuplSessionRecord() Begin\n");
	SUPLLOG(ELogP1, "CSuplSessionRecord::CSuplSessionRecord() End\n");
	}

MSuplConnectionManagerObserver& CSuplSessionRecord::Observer() const
	{
	SUPLLOG(ELogP1, "CSuplSessionRecord::Observer() Begin\n");
	SUPLLOG(ELogP1, "CSuplSessionRecord::Observer() End\n");
	return iObserver;
	}

const TSuplLocalSessionId& CSuplSessionRecord::SessionId() const
	{
	SUPLLOG(ELogP1, "CSuplSessionRecord::SessionId() Begin\n");
	SUPLLOG(ELogP1, "CSuplSessionRecord::SessionId() End\n");
	return iSessionId;
	}

const TLbsHostSettingsId CSuplSessionRecord::SlpId() const
	{
	SUPLLOG(ELogP1, "CSuplSessionRecord::SlpId() Begin\n");
	SUPLLOG(ELogP1, "CSuplSessionRecord::SlpId() End\n");
	return iSlpId;
	}

TUint CSuplSessionRecord::SocketWriter() const
	{
	SUPLLOG(ELogP1, "CSuplSessionRecord::SocketWriter() Begin\n");
	SUPLLOG(ELogP1, "CSuplSessionRecord::SocketWriter() End\n");
	return iSocketWriterIndex;
	}

CSuplSessionRecord::TServiceType CSuplSessionRecord::ServiceMode() const
	{
	SUPLLOG(ELogP1, "CSuplSessionRecord::ServiceMode() Begin\n");
	SUPLLOG(ELogP1, "CSuplSessionRecord::ServiceMode() End\n");
	return iServiceType;
	}
