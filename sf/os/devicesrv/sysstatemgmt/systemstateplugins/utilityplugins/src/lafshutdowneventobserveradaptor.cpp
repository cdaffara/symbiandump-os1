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

#include <e32debug.h>
#include <savepriv.h>
#include <s32mem.h>
#include <ssm/ssmstatemanager.h>
#include "lafshutdowneventobserveradaptor.h"

_LIT( KLafShutDLL, "loadlafshutdown.dll" );
typedef MSsmLoadLafShut* (*TFuncLoadLafShutL)(void);

const TInt KNumConnectRetries	=10;
//number of times re request the shutdown server for registered client array when
//server side client array count is not equal to client side array count 
const TInt KMaxNumOfRetries	= 2;

const TUid KServerUid3={0x10004902};
_LIT(KShutdownSrvName,"shutdownsrvs");

/**
Used to create an instance of CLafShutdownEventObserverAdaptor.

@return	A pointer to an object of type CLafShutdownEventObserverAdaptor.
*/
EXPORT_C MSsmUtility* CLafShutdownEventObserverAdaptor::NewL()
	{
	CLafShutdownEventObserverAdaptor* self = new (ELeave) CLafShutdownEventObserverAdaptor();
	return static_cast<MSsmUtility*>(self);
	}

/**
Used to Initialize the utlity plugin.
@see MSsmUtility::InitializeL()
*/
void CLafShutdownEventObserverAdaptor::InitializeL()
	{
	User::LeaveIfError(iLafShutLib.Load(KLafShutDLL));
	TFuncLoadLafShutL lafShut = reinterpret_cast<TFuncLoadLafShutL>(iLafShutLib.Lookup(1));
	iLafShutdown = lafShut();
	User::LeaveIfNull(iLafShutdown);
	iLafShutdown->CreateShutdownManager(*this);
	}

/**
Starts the utility plugin.
@see MSsmUtility::StartL()
*/
void CLafShutdownEventObserverAdaptor::StartL()
	{
	User::LeaveIfError(iShutdownSrvSession.ConnectL());
	}

/**
Release memory associated with this SUP.
@see MSsmUtility::Release()
*/
void CLafShutdownEventObserverAdaptor::Release()
	{
	iLafShutdown->DeleteShutdownManager();
	iLafShutdown = NULL;
	iLafShutLib.Close();
	}

/**
This function notifies all client registered for shutdown notification, and issues a state change to shutdown.
@panic EInvalidSession If shutdown server's session is invalid
*/
void CLafShutdownEventObserverAdaptor::HandleShutdownEventL(MSaveObserver::TSaveType aAction, TBool aPowerOff, TPowerState aEvent)
	{
	__ASSERT_ALWAYS(iShutdownSrvSession.Handle(), User::Panic(KPanicShutdownEventObserverAdaptor, EInvalidSession));
	//HandleShutdownEventL() will call NotifySave()
	iShutdownSrvSession.HandleShutdownEventL(aAction, aPowerOff, aEvent);

	//Connect to System State Manager
	//Create a shutdown state for transition
	//Request for shutdown transition passing the shutdown event
	RSsmStateManager stateManager;
	User::LeaveIfError(stateManager.Connect());
	TSsmStateTransition state(ESsmShutdown, KSsmAnySubState, aEvent);
	stateManager.RequestStateTransition(state);
	stateManager.Close();
	}

/**
@panic EInvalidSession If shutdown server's session is invalid
*/
CArrayFix<TThreadId>* CLafShutdownEventObserverAdaptor::ClientArrayLC()
	{
	__ASSERT_ALWAYS(iShutdownSrvSession.Handle(), User::Panic(KPanicShutdownEventObserverAdaptor, EInvalidSession));
	return iShutdownSrvSession.ClientArrayLC();
	}

/**
@panic EInvalidSession If shutdown server's session is invalid
*/
TBool CLafShutdownEventObserverAdaptor::IsClientHung(TThreadId aId) const
	{
	__ASSERT_ALWAYS(iShutdownSrvSession.Handle(), User::Panic(KPanicShutdownEventObserverAdaptor, EInvalidSession));
	return iShutdownSrvSession.IsClientHung(aId);
	}

/**
@panic EInvalidSession If shutdown server's session is invalid
*/
void CLafShutdownEventObserverAdaptor::GetShutdownState(TBool& aPowerOff, TBool& aAllSessionsHavePendingRequest) const
	{
	__ASSERT_ALWAYS(iShutdownSrvSession.Handle(), User::Panic(KPanicShutdownEventObserverAdaptor, EInvalidSession));
	iShutdownSrvSession.GetShutdownState(aPowerOff, aAllSessionsHavePendingRequest);
	}

CLafShutdownEventObserverAdaptor::CLafShutdownEventObserverAdaptor()
	{
	}

CLafShutdownEventObserverAdaptor::~CLafShutdownEventObserverAdaptor()
	{
	}

/**
This method will establish the connection with the shutdown server. If the shutdown server
is not started yet, it will be started.
@return KErrNone The connection was established successfully.
@return Some other system-wide error codes
*/
TInt CLafShutdownEventObserverAdaptor::RShutdownSrvSession::ConnectL()
	{
	TInt err=KErrNone;
	TInt retry=KNumConnectRetries;
	FOREVER
		{
		err = CreateSession(__SHUTDOWN_SERVER_NAME, TVersion(KShutdownMajorVN,KShutdownMinorVN,KShutdownBuildVN), KShutdownMessageSlots);
		if ((--retry>0) && ((err==KErrNotFound) || (err==KErrServerTerminated)))
			{
			err = StartServerL();
			if ((err!=KErrNone) && (err!=KErrAlreadyExists))
				{
				break;
				}
			}
		else
			{
			break;
			}
		}
	return err;
	}

/**
This method starts the shutdown server.
@return KErrNone The server was started successfully.
@return KErrAlreadyExists The server is started already.
@return Some other system-wide error codes
*/
TInt CLafShutdownEventObserverAdaptor::RShutdownSrvSession::StartServerL()
	{
	TInt error=KErrNone;
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);

	RProcess server;
	error = server.Create(KShutdownSrvName,KNullDesC,serverUid);
	if(error!=KErrNone)
		return error;
 	TRequestStatus stat;
	server.Rendezvous(stat);
 	if (stat!=KRequestPending)
 		server.Kill(0);		// abort startup
 	else
 		server.Resume();	// logon OK - start the server
 	User::WaitForRequest(stat);		// wait for start or death
 	// we can't use the 'exit reason' if the server panicked as this
 	// is the panic 'reason' and may be '0' which cannot be distinguished
 	// from KErrNone
 	error=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return error;
	}

/**
This method has to be called, when the registered clients have to be notified that a 
particular action has to be done, such as MSaveObserver::ESaveData, MSaveObserver::ESaveAll, 
MSaveObserver::EReleaseRAM,...
If the requested action is not MSaveObserver::ESaveNone, the method will call 
CServShutdownServer::NotifySave().
@param aAction The type of the requested action
@param aPowerOff If it is non-zero, this is the beginning of a powerdown sequence.
@param aEvent The type of the powerdown event (power off or restart)
@leave KErrNotSupported Leaves if aEvent is invalid  
@see CServShutdownServer::NotifySave()
@see TPowerState
*/
void CLafShutdownEventObserverAdaptor::RShutdownSrvSession::HandleShutdownEventL(MSaveObserver::TSaveType aAction, TBool aPowerOff, TPowerState aEvent)
	{
	SendReceive(EEventObsAdaptHandleShutdown, TIpcArgs(aAction, aPowerOff, aEvent));
	}

/**
This method creates an array of CArrayFix<TThreadId> type and appends to it the
thread id-s of the all registered clients.
The created CArrayFix<TThreadId> instance will be pushed on the cleanup stack.
Logic for this function is as follows
step1: Request the shutdown server to get the count of registered clients.
step2: Create buffer of size arrayCount * sizeof(TThreadId).
step3: Request the shutdown server to get the array of registered clients. If some client 
	   is registered after step 2 then server will return new count and empty buffer.
step4: If buffer is emptey and server side client array is not equal to client side array 
	   count then repeat the step 3 for 2 times.
step5: if client side array count is equal to server side client array count then append thread ids 
	   to array and return the array else return empty array.	
@return A pointer to a CArrayFix<TThreadId> array with the client thread id-s.if client side array count 
		is equal to server side client array count then append thread ids to array and return the array else return empty array.
*/
CArrayFix<TThreadId>* CLafShutdownEventObserverAdaptor::RShutdownSrvSession::ClientArrayLC()
	{
	CArrayFix<TThreadId>* clientArray = new (ELeave) CArrayFixFlat<TThreadId>(2);
	CleanupStack::PushL(clientArray);

	//arrayCount refers to the array count at server side
	TInt arrayCount = 0;
	TPckg<TInt> pckg(arrayCount);
	User::LeaveIfError(SendReceive(EEventObsAdaptClientArrayCount, TIpcArgs(&pckg)));
	//return an empty array because arrayCount is 0
	if (arrayCount > 0)
		{
		CBufFlat* buffer = CBufFlat::NewL(arrayCount * sizeof(TThreadId));
		CleanupStack::PushL(buffer);
		
	    TPtr8 bufPtr = buffer->Ptr(0);
		TInt clientSideArrayCount = 0;			//clientSideArrayCount refers to the array count at client side
		//Request the server to get the registered clients. If server side array count is not equal to client 
		//side array count then retry to request the server for KMaxNumOfRetries times 
		for (TInt numOfRetries = 0; clientSideArrayCount != arrayCount && numOfRetries <= KMaxNumOfRetries; ++numOfRetries )
			{
			clientSideArrayCount = arrayCount;
			buffer->ResizeL(clientSideArrayCount * sizeof(TThreadId));
			bufPtr.Set(buffer->Ptr(0));
			User::LeaveIfError(SendReceive(EEventObsAdaptClientArray, TIpcArgs(&bufPtr,clientSideArrayCount,&pckg)));
			}

		if (clientSideArrayCount != arrayCount)		//retries exhausted and count doesn't match
			{
			CleanupStack::PopAndDestroy(buffer);
			User::Leave(KErrGeneral);
			}
			
		RBufReadStream readStream(*buffer);
		readStream.PushL();
		TKeyArrayFix key(0,ECmpTInt);

		for (TInt i = 0; i < arrayCount; ++i)
			{
			const TInt32 threadHighBit = readStream.ReadUint32L();
			const TInt32 threadLowBit = readStream.ReadUint32L();
			TThreadId threadId(MAKE_TINT64(threadHighBit, threadLowBit)); 
			TInt pos;
			if (0 != clientArray->Find(threadId,key,pos))
				{
			    clientArray->AppendL(threadId);  
				}
			}
		readStream.Pop();
		readStream.Close();
		CleanupStack::PopAndDestroy(buffer);
		}
	return clientArray;
	}
/**
This method checks for pending request of a client's thread, which is passed through aId.
@param aId Client's thread id.
@return ETrue if the client with this thread id has no pending request otherwise EFalse.
*/
TBool CLafShutdownEventObserverAdaptor::RShutdownSrvSession::IsClientHung(TThreadId aId) const
	{
	TBool retVal = EFalse;
	TPckg<TInt32> pkg1(aId);
	TPckg<TInt32> pkg2(retVal);
	TIpcArgs args(&pkg1, &pkg2);

	SendReceive(EEventObsAdaptIsClientHung, args);

	return retVal;
	}

/**
This method returns an information about the shutdown status.
@param aPowerOff An output parameter. It will be non-zero, if a powerdown sequence 
				 has been initiated.
@param aAllSessionsHavePendingRequest It will be non-zero, if all clients has pending requests.
*/
void CLafShutdownEventObserverAdaptor::RShutdownSrvSession::GetShutdownState(TBool& aPowerOff, TBool& aAllSessionsHavePendingRequest) const
	{
	TPckg<TBool> powerOff(aPowerOff);
	TPckg<TBool> allSessionsHavePendingRequest(aAllSessionsHavePendingRequest);
	TIpcArgs args(&powerOff, &allSessionsHavePendingRequest);
	SendReceive(EEventObsAdaptGetShutdownState, args);
	}

