// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "surfaceupdateserver.h"

#include <graphics/compositionsurfaceupdate.h>
#include <graphics/extensioncontainer.h>

#include "surfaceupdate.h"
#include "suerror.h"
#ifdef TEST_SURFACE_UPDATE
#include "surfaceupdatetest.h"
#endif


const TUint KDefaultHeapSize=0x10000;

void *gProvider = NULL;
RFastLock gProviderFastLock;

/**
The server maintains session with the clients. 
It starts during the initialization of the Content update receiver thread.  
*/
CSurfaceUpdateServer* CSurfaceUpdateServer::NewL()
	{
	CSurfaceUpdateServer* server = new (ELeave) CSurfaceUpdateServer(EPriorityStandard);
	CleanupStack::PushL(server);
	server->ConstructL();
	CleanupStack::Pop();
	return server;	
	}
	
CSurfaceUpdateServer::CSurfaceUpdateServer(CActive::TPriority aPriority) :
	CServer2(aPriority)
	{
	}
	
CSurfaceUpdateServer::~CSurfaceUpdateServer()
	{
	iUpdateReceiver.Close();
	iUpdateReceiverPriorityOrder.ResetAndDestroy();
	delete iServerProvider;
	}	
	
void CSurfaceUpdateServer::ConstructL()
	{
	iServerProvider = CSurfaceUpdateServerProvider::NewL(EPriorityStandard, this);
	}
	
/**
Assign Content update receiver instance to particular screen. 
The following calls of this function will override the previous. 

@see MSurfaceUpdateServerProvider::Register
*/	
TInt CSurfaceUpdateServer::Register(TInt aScreen, CBase* aUpdateReceiver, TInt aPriority)
	{
	if(aScreen < 0)
		{
		return KErrArgument;
		}
		
	TInt err = KErrNone;
	while((iUpdateReceiver.Count() <= aScreen) && (err == KErrNone))
		{
		err = iUpdateReceiver.Append(NULL);
		}
	
	if(err == KErrNone)
		{
		TUpdateReceiverPriorityEntry *receiverPriorityEntry = NULL; 
		if(!aUpdateReceiver)
			{//Client wants to unregister the Content update receiver which has been associated with the given screen number.
			MCompositionSurfaceUpdate* receiver = iUpdateReceiver[aScreen];
			if(receiver)
				{
				TInt num = iUpdateReceiverPriorityOrder.Count() - 1;
				for(;;)
					{//Content update receiver must be in priority list, therefore we don't need to check num >= 0
					receiverPriorityEntry = iUpdateReceiverPriorityOrder[num];
					if(receiverPriorityEntry->iUpdateReceiver == receiver)
						{//remove the Content update receiver from the priority list
						iUpdateReceiverPriorityOrder.Remove(num);
						delete receiverPriorityEntry;
						break;
						}
					num--;
					}
				iUpdateReceiver[aScreen] = NULL; //unregister the Content update receiver by setting the entry to NULL
				}
			}
		else
			{
	        CExtensionContainer* extCont=static_cast<CExtensionContainer*>(aUpdateReceiver);
	        MCompositionSurfaceUpdate* updateReceiver=extCont->GetInterface<MCompositionSurfaceUpdate>();
	        if (updateReceiver)
	            {
                receiverPriorityEntry = new TUpdateReceiverPriorityEntry;
                if(!receiverPriorityEntry)
                    return KErrNoMemory;
                receiverPriorityEntry->iPriority = aPriority;
                receiverPriorityEntry->iUpdateReceiver = updateReceiver;
        
                err = iUpdateReceiverPriorityOrder.InsertInOrder(receiverPriorityEntry, CSurfaceUpdateServer::CompareUpdateReceiverPriority);
                if(err == KErrNone)
                    {
                    iUpdateReceiver[aScreen] = updateReceiver;
                    }
                else
                    {
                    delete receiverPriorityEntry;
                    }
	            }
	        else
	            {
	            err=KErrArgument;
	            }
			}	
		}
	return err;
	}

/**
Create a new session with a server. Derived from CSession2.

@param aVersion required version of the server
@param aMessage message from the client.
@return New instance of the CSurfaceUpdateSession.
*/
CSession2* CSurfaceUpdateServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
      // Check that the version is OK
	TVersion v(KSurfaceUpdateServMajorVersionNumber,KSurfaceUpdateServMinorVersionNumber,KSurfaceUpdateServBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);	
	
	// Security is not an issue, any client can have connection to the server	
	// Create the session.
	return new (ELeave) CSurfaceUpdateSession(UpdateReceiverPriority());
	}

/**
Utility function to panic the server

@param aPanic Panic code
*/
void CSurfaceUpdateServer::PanicServer(TSurfaceUpdateServPanic aPanic)
	{
	_LIT(KTxtServerPanic,"Surface Update Server panic");
	User::Panic(KTxtServerPanic, aPanic);
	}

/**
Provide composition receiver.

@param aScreen targeted screen
@return Composition receiver object, associated to particular screen
*/
MCompositionSurfaceUpdate* CSurfaceUpdateServer::UpdateReceiver(TInt aScreen) const
	{
	if(aScreen >= iUpdateReceiver.Count())//negative value won't reach this point 
		return NULL;
	return iUpdateReceiver[aScreen];	
	}

TInt CSurfaceUpdateServer::ThreadFunction(TAny* aAny)
	{
	  // get clean-up stack
	CTrapCleanup* cleanup=CTrapCleanup::New();
	__ASSERT_ALWAYS(cleanup!=NULL, CSurfaceUpdateServer::PanicServer(EUpdateServPanicNoMemory));
	
	  // create an active scheduler and server
	CActiveScheduler *pA = new CActiveScheduler;
	__ASSERT_ALWAYS(pA != NULL, CSurfaceUpdateServer::PanicServer(EUpdateServPanicNoMemory));

	  //Install the active scheduler
	CActiveScheduler::Install(pA);

	CSurfaceUpdateServer *pS = NULL;
	TRAPD(err, pS = CSurfaceUpdateServer::NewL());
	__ASSERT_ALWAYS(err == KErrNone, CSurfaceUpdateServer::PanicServer(EUpdateServPanicNoMemory));
	
	  // Start the server
#ifndef TEST_SURFACE_UPDATE
	err = pS->Start(KSurfaceUpdateServerName);
#else
	err = pS->Start(KTestSurfaceUpdateServerName);
#endif
	
	if (err != KErrNone)
		{
		CSurfaceUpdateServer::PanicServer(EUpdateServPanicStartUp);
		}
	*(static_cast <MSurfaceUpdateServerProvider**> (aAny)) = pS->SurfaceUpdateProvider();
      
      // Let everyone know that we are ready to
      // deal with requests.
	RThread::Rendezvous(KErrNone);
	  // And start fielding requests from client(s).
	CActiveScheduler::Start();

     // Tidy up... 	
	delete pS;
	delete pA;
	delete cleanup; 
	
	return KErrNone;
	}

inline TInt CSurfaceUpdateServer::NumUpdateReceivers() const
	{
	return iUpdateReceiver.Count(); 
	}

/**
Compare two content update receiver entries according to their priority. 
The function is used when receiver priority entry is inserted/retrieved from the array (iCompReceiverPriorityOrder).

@return zero, if the priorities of two receivers are equal; 
a negative value, if the priority of the first receiver is less than the priority of the second one; 
a positive value, if the priority of the first receiver is greater than the priority of the second one.
*/
TInt CSurfaceUpdateServer::CompareUpdateReceiverPriority(const TUpdateReceiverPriorityEntry& aEntry1, const TUpdateReceiverPriorityEntry& aEntry2)
	{
	return aEntry2.iPriority - aEntry1.iPriority;
	}

/**
class CSurfaceUpdateSession

Maintain the channel between clients and the server. 
Functions are provided will respond appropriately to client messages. 
*/

CSurfaceUpdateSession::CSurfaceUpdateSession(const RPointerArray<TUpdateReceiverPriorityEntry>& aEntryList) :
	CSession2(),
	iUpdateReceiverEntryList(aEntryList)
	{
	}

/**
Cancel any outstanding client notification requests. 
All resources owned by the session will either be immediately released or 
scheduled for deferred deletion.
*/
CSurfaceUpdateSession::~CSurfaceUpdateSession()
	{
	CancelAllUpdateNotifications();
	iUpdateReceiverNotificationBatches.ResetAndDestroy();
	}

/**
Main function which deals with requests from clients.
*/	
void CSurfaceUpdateSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case EUpdateServNotifyWhenAvailable:
		NotifyWhenAvailable(aMessage);
		return;
	case EUpdateServNotifyWhenDisplayed:
		NotifyWhenDisplayed(aMessage);
		return;
	case EUpdateServNotifyWhenDisplayedXTimes:
		NotifyWhenDisplayedXTimes(aMessage);
		return;
	case EUpdateServSubmitUpdate:
		SubmitUpdate(aMessage);
		return;
	case EUpdateServCancelAllNotifications:
		CancelAllUpdateNotifications();
		aMessage.Complete(KErrNone);
		return;
#ifdef TEST_SURFACE_UPDATE
	case EUpdateServOOM:
		{
		SetHeapFailure(aMessage);
		return;
		}
#endif
	default:
		PanicClient(aMessage, EUpdateServPanicBadRequest);
		return;
		}	 
	}

#ifdef TEST_SURFACE_UPDATE
/**
  Simulate heap allocation failure for the SUS thread's heap.
  Aim for OOM testing.
 */
void CSurfaceUpdateSession::SetHeapFailure(const RMessage2& aMessage)
	{
	TInt numElement = iUpdateReceiverNotificationBatches.Count();
	TInt index = 0;
	while(numElement)
		{
		CUpdateReceiverNotificationBatch* batch = iUpdateReceiverNotificationBatches[index];
		if(batch->iType == EUpdateSrvReusable)
			{
			iUpdateReceiverNotificationBatches.Remove(index);
			delete batch;
			}
		else
			{
			index++;
			}
		numElement--;            
		}
	if(numElement == 0)
		{
		iUpdateReceiverNotificationBatches.Reset();
		}
	TInt failRate = aMessage.Int0();
	if(!failRate)
		{
		__UHEAP_RESET;
		}
	else
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, failRate);
		}
	aMessage.Complete(KErrNone);
	}
#endif

void CSurfaceUpdateSession::PanicClient(const RMessage2& aMessage, TInt aPanic) const
	{
	_LIT(KTxtServer,"SurfUpServ");
	aMessage.Panic(KTxtServer, aPanic);
	}

/**
Return first inactive spare notification object stored in the notification array, 
create a new one if fail to find it in the array.
*/	
CUpdateReceiverNotificationBatch* CSurfaceUpdateSession::UpdateReceiverNotificationBatchL()
	{
	TInt numElement = iUpdateReceiverNotificationBatches.Count();
	CUpdateReceiverNotificationBatch* notifier = NULL;
	CSurfaceUpdateServer* server = (CSurfaceUpdateServer*) Server();
	for(TInt index = 0; index < numElement; index++)
		{
		notifier = iUpdateReceiverNotificationBatches[index];
		if(notifier->iType == EUpdateSrvReusable)
			{
			__ASSERT_ALWAYS(notifier->iMsg.IsNull(), CSurfaceUpdateServer::PanicServer(EUpdateServPanicDataIntegrity));
            notifier->SetNumUpdateReceivers(server->NumUpdateReceivers());
			if(numElement > index + 1)
				{
			//to improve a search, append the element to the end of the array
				iUpdateReceiverNotificationBatches.Remove(index);
				iUpdateReceiverNotificationBatches.AppendL(notifier);
				}
			return notifier;
			}
		}
	
	notifier = new (ELeave) CUpdateReceiverNotificationBatch(this, server->NumUpdateReceivers());
	CleanupStack::PushL(notifier);
	iUpdateReceiverNotificationBatches.AppendL(notifier);
	CleanupStack::Pop();
	
	return notifier;
	}
	
void CSurfaceUpdateSession::NotifyWhenAvailable(const RMessage2& aMessage)
	{
	StoreNotification(iAvailable, aMessage, EUpdateSrvAvailable);
	}
	
void CSurfaceUpdateSession::NotifyWhenDisplayed(const RMessage2& aMessage)
	{
	StoreNotification(iDisplayed, aMessage, EUpdateSrvDisplayed);
	}
	
void CSurfaceUpdateSession::NotifyWhenDisplayedXTimes(const RMessage2& aMessage)
	{
	StoreNotification(iDisplayedXTimes, aMessage, EUpdateSrvDisplayedXTimes);
	}

void CSurfaceUpdateSession::StoreNotification(CUpdateReceiverNotificationBatch*& aNotifier, const RMessage2& aMessage, TNotificationType aType)
	{
	if(aNotifier)
		{
		if(!aNotifier->iMsg.IsNull())
			{//not expected behaviour
			//would happen if a client sends the same notification request in a row
			IssueRequestComplete(KErrCancel);
			PanicClient(aMessage, EUpdateServPanicDataIntegrity);
			return;
			}
		}
	else
		{
		TRAPD(res, aNotifier = UpdateReceiverNotificationBatchL());
		if(res != KErrNone)
			{
			aMessage.Complete(res);
			return;
			}
		}	
	
	aNotifier->iMsg = aMessage;
	aNotifier->iType=aType;
	}

/*
Complete the outstanding requests and reset internal request variables to zero.
@param aErr Error code for completion.   
*/ 	
void CSurfaceUpdateSession::IssueRequestComplete(TInt aErr)
	{
	if(iAvailable)
		{
		if(!iAvailable->iMsg.IsNull())
			{
			iAvailable-> iMsg.Complete(aErr);
			iAvailable->CheckForReuse();	 
			}
		iAvailable = NULL;
		}

	if(iDisplayed)
		{
		if(!iDisplayed -> iMsg.IsNull())
			{
			iDisplayed -> iMsg.Complete(aErr);
			iDisplayed->CheckForReuse();
			}
		iDisplayed = NULL;
		}

	if(iDisplayedXTimes)
		{
		if(!iDisplayedXTimes->iMsg.IsNull())
			{
			iDisplayedXTimes->iMsg.Complete(aErr);
			iDisplayedXTimes->CheckForReuse();
			}
		iDisplayedXTimes=NULL;
		}
	}

/**
Redirect call to the DoSubmitUpdateL; error handling
*/
void CSurfaceUpdateSession::SubmitUpdate(const RMessage2& aMessage)
	{
	TInt err = KErrNone;
	TRAP(err, DoSubmitUpdateL(aMessage));
	if(err != KErrNone)
		{
		IssueRequestComplete(err);
		}
	aMessage.Complete(err);	
	}

/**
Issue request for the update to the composition receiver; 
ask notification on composition event if required.
*/	
void CSurfaceUpdateSession::DoSubmitUpdateL(const RMessage2& aMessage)
	{
	TInt displayedXTimes = 0;
	TInt screen;
	TInt buffer;
	TSurfaceId surfaceId;
	TPckg<TSurfaceId> surfaceIdPckg(surfaceId);

	RRegion *region = NULL;
	TRect *rectangleList = NULL;

	//extract the composition data
	aMessage.ReadL(1, surfaceIdPckg);
	
	screen = aMessage.Int0();
	buffer = aMessage.Int2();
	
	//validate parameters
	if((screen < 0) || (buffer < 0))
		{
		User::Leave(KErrArgument);
		}
	//Check that we haven't used another update method before
	//The comparison is slightly optimized for performance as oppose to code size
	if(iUpdateMethod == EUpdateMethodGlobal) 
		{
		if(screen != KAllScreens)
			User::Leave(KErrNotSupported);
		}
	else if(iUpdateMethod == EUpdateMethodPerScreen) 
		{
		if(screen == KAllScreens)
			User::Leave(KErrNotSupported);
		}
	
	TInt len = aMessage.GetDesLength(3);
	if(len > 0)
		{
		rectangleList = (TRect*) (User::AllocL(len));
		CleanupStack::PushL(rectangleList);
		TPtr8 ptrRect(reinterpret_cast<TUint8*> (rectangleList), len);
		aMessage.ReadL(3, ptrRect);
		TInt regionCount = len / sizeof(TRect);
		region = new (ELeave) RRegion(regionCount, rectangleList);
		CleanupStack::PushL(region);
		}
	if(iDisplayedXTimes)
		{
		displayedXTimes = iDisplayedXTimes -> iMsg.Int0();
		if(displayedXTimes < 1)
			{
			iDisplayedXTimes->iMsg.Complete(KErrArgument);
			iDisplayedXTimes->CheckForReuse();
			iDisplayedXTimes = NULL;
			}
		}
	
	const CSurfaceUpdateServer* server = static_cast <const CSurfaceUpdateServer*> (Server());	
	if(screen != KAllScreens)
		{
		MCompositionSurfaceUpdate* receiver = server->UpdateReceiver(screen);
		if(!receiver)
			{
			User::Leave(KErrUpdateReceiverNotAvailable);
			}
		DispatchUpdate(surfaceId, buffer, region, &displayedXTimes, receiver);
		iUpdateMethod = EUpdateMethodPerScreen;
		}
	else
		{
		DispatchUpdate(surfaceId, buffer, region, &displayedXTimes);
		iUpdateMethod = EUpdateMethodGlobal;
		}
				
	if(region)
		{
		CleanupStack::PopAndDestroy(2, rectangleList);
		}
	}

/**
 Dispatch update to the composition update receivers. 
 The function could fail to allocate the notification objects 
 due to memory shortage, however, the content update command will still be submitted to the GCE.  
 */
void CSurfaceUpdateSession::DispatchUpdate(const TSurfaceId& aSurfaceId, TInt aBuffer, RRegion* aRegion, TInt* aDisplayedXTimes, MCompositionSurfaceUpdate* aReceiver)
	{
	TInt index = 1;
	TInt numReceivers = 1;
	TInt priority = 0;
	
	if(!aReceiver)
		{
		aReceiver = iUpdateReceiverEntryList[0]->iUpdateReceiver;
		priority = iUpdateReceiverEntryList[0]->iPriority;
		numReceivers = iUpdateReceiverEntryList.Count(); //get the number of all receivers present in the system
		}
	
	for(;;)
		{	
		CUpdateReceiverNotification* receiverNotificationAvailable = NULL;
		CUpdateReceiverNotification* receiverNotificationDisplayed = NULL;
		CUpdateReceiverNotification* receiverNotificationDisplayedXTimes = NULL;
		TUint32* timeStamp = NULL;
	
	//activate objects
		if(iAvailable)
			{
			receiverNotificationAvailable = iAvailable->UpdateReceiverNotification();
			if(!receiverNotificationAvailable)
				{
				iAvailable->iMsg.Complete(KErrNoMemory);
				iAvailable->CheckForReuse();
				iAvailable = NULL;
				}
			}

		if(iDisplayed)
			{
			receiverNotificationDisplayed = iDisplayed->UpdateReceiverNotification(priority);
			if(!receiverNotificationDisplayed)
				{
				iDisplayed->iMsg.Complete(KErrNoMemory);
				iDisplayed->CheckForReuse();
				iDisplayed = NULL;
				}
			}

		if(iDisplayedXTimes)
			{
			receiverNotificationDisplayedXTimes = iDisplayedXTimes->UpdateReceiverNotification();
			if(!receiverNotificationDisplayedXTimes)
				{
				iDisplayedXTimes->iMsg.Complete(KErrNoMemory);
				iDisplayedXTimes->CheckForReuse();
				iDisplayedXTimes = NULL;
				}
			}
	
		TRequestStatus *statusAvailable = NULL;
		TRequestStatus *statusDisplayed = NULL;
		TRequestStatus *statusDisplayedXTimes = NULL;
	//activate all notifications
		if(receiverNotificationAvailable)
			{
			statusAvailable = &(receiverNotificationAvailable->Status());
			receiverNotificationAvailable->Activate();
			}
		if(receiverNotificationDisplayed)
			{
			statusDisplayed = &(receiverNotificationDisplayed->Status());
			timeStamp = & receiverNotificationDisplayed -> iTimeStamp;
			receiverNotificationDisplayed->Activate();
			}
		if(receiverNotificationDisplayedXTimes)
			{
			statusDisplayedXTimes = &(receiverNotificationDisplayedXTimes->Status());
			receiverNotificationDisplayedXTimes->Activate();
			}
	//And finally, send request to the receiver   
		aReceiver->ContentUpdated(aSurfaceId, aBuffer, aRegion,
				statusAvailable, statusDisplayed, timeStamp, 
				statusDisplayedXTimes, statusDisplayedXTimes ? aDisplayedXTimes : NULL);
	
		if(numReceivers > index)
			{
			priority = iUpdateReceiverEntryList[index]->iPriority;
			aReceiver = iUpdateReceiverEntryList[index++]->iUpdateReceiver;
			}
		else
			break;
		}//for(;;)
	
	iAvailable = NULL;
	iDisplayed = NULL;
	iDisplayedXTimes = NULL;
	}

void CSurfaceUpdateSession::CancelAllUpdateNotifications()
	{
	//go through all notification objects and issue request complete for outstanding requests
	TInt count = iUpdateReceiverNotificationBatches.Count();
	
	for(TInt index = 0; index < count; index++)
		{
		CUpdateReceiverNotificationBatch* notifier = iUpdateReceiverNotificationBatches[index];
		if(!notifier->iMsg.IsNull())
			{
			notifier->iMsg.Complete(KErrCancel);
			}
		}

	iAvailable = NULL;
	iDisplayed = NULL;
	iDisplayedXTimes = NULL;
	}
	
//*********************
CUpdateReceiverNotification::CUpdateReceiverNotification(CActive::TPriority aPriority, TInt aReceiverPriority, CUpdateReceiverNotificationBatch *aParentNotificationBatch) :
	CActive(aPriority), iUpdateReceiverPriority(aReceiverPriority), iParentNotificationBatch(aParentNotificationBatch)
	{
	CActiveScheduler::Add(this);
#ifdef TEST_SURFACE_UPDATE
	iServer = iParentNotificationBatch->Server();
#endif
	}
	
CUpdateReceiverNotification::~CUpdateReceiverNotification()
	{
	//Content updates can not be cancelled. The user must ensure that the associated content update 
	//is complete before deleting this object.
	__ASSERT_ALWAYS(!IsActive(), CSurfaceUpdateServer::PanicServer(EUpdateServPanicDataIntegrity));
	}
	
void CUpdateReceiverNotification::DoCancel()
	{
	//Content Updates can not be cancelled. Content Updates must be allowed to complete normally.
	CSurfaceUpdateServer::PanicServer(EUpdateServPanicDataIntegrity);
	}

/**
Is triggered every time the GCE signals on notification objects.
Delegates processing to the parent notification batch.   
*/	
void CUpdateReceiverNotification::RunL()
	{
	// CReceiverNotification object is deleted here once it is set to self destroying when run.
	if (iSelfDestructWhenRun)
		{
#ifdef TEST_SURFACE_UPDATE
		DecNumberPendingNotifications();
#endif
		delete this;
		return;
		}
	iParentNotificationBatch->ProcessNotificationEvent(this);
	}
	
TRequestStatus& CUpdateReceiverNotification::Status()
	{
	return iStatus;	
	}

void CUpdateReceiverNotification::Activate()
	{
	iStatus = KRequestPending;
	SetActive();
	}

#ifdef TEST_SURFACE_UPDATE
void CUpdateReceiverNotification::DecNumberPendingNotifications()
	{
	if (iServer)
		{
		iServer-> iNumberPendingNotification--;
		}
	}
#endif

/**
The class maintains the notification objects of the same type and attributed to a 
particular submit update command.   
 */
CUpdateReceiverNotificationBatch::CUpdateReceiverNotificationBatch(CSurfaceUpdateSession *aSession, TInt aNumReceivers) :
	iSession(aSession), iNumUpdateReceivers(aNumReceivers)
	{
	CheckForReuse();
	}

CUpdateReceiverNotificationBatch::~CUpdateReceiverNotificationBatch()
	{
	TInt count = iUpdateReceiverNotifications.Count();
	for(TInt index = 0; index < count; index++)
		{
		CUpdateReceiverNotification* notifier = iUpdateReceiverNotifications[index];
		// CReceiverNotification active object cannot be destroyed if it is active and waiting for
		// receiver to complete the notification. It deletes itself inside its RunL function when the
		// notification is completed.
		if (notifier->IsActive())
			{
			notifier->iSelfDestructWhenRun = ETrue;
#ifdef TEST_SURFACE_UPDATE
			IncNumberPendingNotifications();
#endif
			}
		else
			{
			delete notifier;
			}
		}
	iUpdateReceiverNotifications.Close();
	}

/**
The function processes signal from composition receiver. 
It completes outstanding messages on the client and set timestamp parameters if appropriate.
Note that this object will not be removed from the 
array (CSurfaceUpdateSession:: iReceiverNotifications) and can be reused later.
*/
void CUpdateReceiverNotificationBatch::ProcessNotificationEvent(CUpdateReceiverNotification* aReceiverNotification)
	{
	TInt index = iUpdateReceiverNotifications.Find(aReceiverNotification);
	__ASSERT_DEBUG(index != KErrNotFound, CSurfaceUpdateServer::PanicServer(EUpdateServPanicDataIntegrity));
	iUpdateReceiverNotifications.Remove(index);
	if(iMsg.IsNull())
		{
		CheckForReuse();
		delete aReceiverNotification;
		return; // the message has already been completed, possibly, because of request cancelation
		}
	TBool oldCompleteWithSuccess = iCompleteWithSuccess;
	TBool newCompleteWithSuccess = EFalse;
	if(aReceiverNotification->iStatus.Int() == KErrNone)
		{
		iCompleteWithSuccess = ETrue;
		newCompleteWithSuccess = ETrue;
		}
	
	switch(iType)
		{
	case EUpdateSrvAvailable:
		if(iUpdateReceiverNotifications.Count() == 0)
			{
			TInt res = iCompleteWithSuccess ? KErrNone : aReceiverNotification->iStatus.Int();
			iMsg.Complete(res);
			}
		break;
	case EUpdateSrvDisplayedXTimes:
		if((newCompleteWithSuccess && (index == 0)) || (iUpdateReceiverNotifications.Count() == 0))
			{
			TInt res = iCompleteWithSuccess ? KErrNone : aReceiverNotification->iStatus.Int();
			iMsg.Complete(res);
			}
		break;
	case EUpdateSrvDisplayed:
		if(newCompleteWithSuccess && 
				((iHighestPriority < aReceiverNotification->iUpdateReceiverPriority) || !oldCompleteWithSuccess))
			{
			iHighestPriority = aReceiverNotification->iUpdateReceiverPriority;
			iTimeStamp = aReceiverNotification->iTimeStamp;
			}
		if((newCompleteWithSuccess && (index == 0)) || (iUpdateReceiverNotifications.Count() == 0))
			{
			TPckgBuf<TUint32> p(iTimeStamp);
			TInt res = iMsg.Write(0, p); //We must not leave here as it will cause panic on Active scheduler. This would be inappropriate for the server side thread.
			if(res == KErrNone)
				{
				res = iCompleteWithSuccess ? KErrNone : aReceiverNotification->iStatus.Int();
				}
			iMsg.Complete(res);
			}
		break;
	default:	
		break;
		}
	
	delete aReceiverNotification;
	CheckForReuse();
	}

/**
 The method allocates a notification object and inserts it into the list
*/ 
CUpdateReceiverNotification* CUpdateReceiverNotificationBatch::UpdateReceiverNotification(TInt aReceiverPriority)
	{
	__ASSERT_DEBUG(iNumUpdateReceivers > iUpdateReceiverNotifications.Count(), CSurfaceUpdateServer::PanicServer(EUpdateServPanicDataIntegrity));
	
	CUpdateReceiverNotification* receiverNotification = new CUpdateReceiverNotification(CActive::EPriorityStandard, aReceiverPriority, this);
	if(receiverNotification)
		{
		TInt res = iUpdateReceiverNotifications.Append(receiverNotification);
		if(res != KErrNone)
			{
			delete receiverNotification;
			receiverNotification = NULL;
			}
		}
	
	return receiverNotification;
	}

/**
If notification list is empty mark the batch for further use
*/
void CUpdateReceiverNotificationBatch::CheckForReuse()
	{
	if(iUpdateReceiverNotifications.Count() == 0)
		{
		iType = EUpdateSrvReusable;
		iCompleteWithSuccess = EFalse;
		iHighestPriority = 0;
		iTimeStamp = 0;
		}
	}

#ifdef TEST_SURFACE_UPDATE
CSurfaceUpdateServer* CUpdateReceiverNotificationBatch::Server()
	{
	return (CSurfaceUpdateServer*)(iSession->Server());
	}

void CUpdateReceiverNotificationBatch::IncNumberPendingNotifications()
	{
	CServer2* server = static_cast<CServer2*> (Server());
	if(server)
		{
		(static_cast<CSurfaceUpdateServer*> (server))-> iNumberPendingNotification++;
		}
	}
#endif


/**
Set number of UpdateReceivers - called when update receivers are added/removed.

@param aNumUpdateReceivers - new number of update receivers for the batch.
 */
void CUpdateReceiverNotificationBatch::SetNumUpdateReceivers(TInt aNumUpdateReceivers)
    {
    __ASSERT_DEBUG(aNumUpdateReceivers >= 0 && aNumUpdateReceivers < 1000 /* arbitrary "large" limit */,
            CSurfaceUpdateServer::PanicServer(EUpdateServPanicDataIntegrity));
    __ASSERT_DEBUG(iType == EUpdateSrvReusable, 
            CSurfaceUpdateServer::PanicServer(EUpdateServPanicDataIntegrity));
    iNumUpdateReceivers = aNumUpdateReceivers;
    }
/**

The class will be used by composition receiver
*/
CSurfaceUpdateServerProvider* CSurfaceUpdateServerProvider::NewL(CActive::TPriority aPriority, CSurfaceUpdateServer* aServer)
	{
	CSurfaceUpdateServerProvider* serverProvider = new (ELeave) CSurfaceUpdateServerProvider(aPriority, aServer);
	CleanupStack::PushL(serverProvider);
	serverProvider->ConstructL();
	CleanupStack::Pop();
	return serverProvider;
	}

CSurfaceUpdateServerProvider::CSurfaceUpdateServerProvider(CActive::TPriority aPriority, CSurfaceUpdateServer* aServer) :
								CActive(aPriority), iServer(aServer)
	{
	RThread thread;
	iThreadId = thread.Id();
	}
	
CSurfaceUpdateServerProvider::~CSurfaceUpdateServerProvider()
	{
	iSemaphore.Close();
	Cancel();
	}
/**
Create provider and add it to the active scheduler
*/
void CSurfaceUpdateServerProvider::ConstructL()
	{
	User::LeaveIfError(iSemaphore.CreateLocal(0));
	CActiveScheduler::Add(this);
	iStatus = KRequestPending;
	SetActive();
	}

/**  
 Assign composition receiver instance to particular screen.
 The following calls of this function for the same screen will override the previous.
 @see MSurfaceUpdateServerProvider::::Register
 */	
EXPORT_C TInt CSurfaceUpdateServerProvider::Register(TInt aScreen, CBase* aReceiver, TInt aPriority)
	{
	RThread thread;
	TInt res = thread.Open(iThreadId);
	if(res == KErrNone)
		{
		iScreen = aScreen;
		iUpdateReceiver = aReceiver;
		iPriority = aPriority;

		TRequestStatus* status = &iStatus;
		thread.RequestComplete(status, EUpdateServEventRegister);
		thread.Close();
		iSemaphore.Wait();
		res = iRegisterErr;
		}
	return res;
	}

/**
Terminate the SUS and release all memory associated with it. 
The function was introduced for debugging purpose only and is not considered 
as part of public API.
*/
EXPORT_C void CSurfaceUpdateServerProvider::Terminate()
	{
#ifdef TEST_SURFACE_UPDATE
	RThread thread;

	if(thread.Open(iThreadId) == KErrNone)
		{
	    TInt err = gProviderFastLock.CreateLocal();
	    __ASSERT_ALWAYS(err == KErrNone || err == KErrAlreadyExists, CSurfaceUpdateServer::PanicServer(EUpdateServPanicGlobalFastLock));
	    
	    gProviderFastLock.Wait();
	    gProvider = NULL;
		if (iServer)
			{
			while((static_cast<CSurfaceUpdateServer*> (iServer))-> iNumberPendingNotification)
				User::After(TTimeIntervalMicroSeconds32(1000));
			}
		TRequestStatus* status = &iStatus;
		thread.RequestComplete(status, EUpdateServEventTerminate);
		
		//wait until the thread dies
		TRequestStatus status1;
		thread.Logon(status1);
		User::WaitForRequest(status1);
		thread.Close();
		
     	gProviderFastLock.Close();
		}
#endif
	}

/**
The function processes signals which come from composition receiver thread.
*/	
void CSurfaceUpdateServerProvider::RunL()
	{
	switch(iStatus.Int())
		{
	case EUpdateServEventTerminate:
		CActiveScheduler::Stop();
		return; 
	case EUpdateServEventRegister:
		iRegisterErr = iServer->Register(iScreen, iUpdateReceiver, iPriority);
		iUpdateReceiver = NULL;
		iStatus = KRequestPending;
		SetActive();
		iSemaphore.Signal();
		break;
	default :
		CSurfaceUpdateServer::PanicServer(EUpdateServPanicBadRequest); 
		iStatus = KRequestPending;
		SetActive();
		break;	
		}
	}
	
void CSurfaceUpdateServerProvider::DoCancel()
	{
	}

/**
Spawn a thread within WSERV process. This will lead to starting the surface update server in it 
@publishedPartner
@prototype Intended to be used by Surface Update control flow
	
@param aUpdateProvider - [out] reference pointer to surface update server provider, 
		which will be set when the server is started. The variable will be used to registry 
		composition receiver instances. The caller doesn't acquire the ownership of this instance,
		thus mustn't delete it. The pointer will be valid until server is operating, 
		i.e. system is up.

@return KErrNone if an operation is successful, any other system error codes otherwise
*/
EXPORT_C TInt StartSurfaceUpdateServer(MSurfaceUpdateServerProvider*& aSurfaceUpdateServerProvider)
	{
#ifndef TEST_SURFACE_UPDATE
	TPtrC serverName(KSurfaceUpdateServerName);
#else
	TPtrC serverName(KTestSurfaceUpdateServerName);
#endif
	//locking
	TInt err = gProviderFastLock.CreateLocal();
	
	if (err != KErrNone && err != KErrAlreadyExists)
	    {
        return err;
	    }
	
	gProviderFastLock.Wait();

	TAny *provider = gProvider;
	if(provider)
		{
		aSurfaceUpdateServerProvider = static_cast <MSurfaceUpdateServerProvider*> (provider);
		gProviderFastLock.Signal();
		return KErrNone;
		}
	TFullName   name;
	RThread serverThread;
	TInt res = KErrAlreadyExists;
	
	TFindServer findSurfaceUpdateServer(serverName);
	  // Need to check that the server exists.
	if (findSurfaceUpdateServer.Next(name) !=KErrNone)
		{
		TTime tm;
		TBuf<32> buf;
		tm.UniversalTime();
		TRAP(res, tm.FormatL(buf, _L("_%H%T%S%C")));
		if(res != KErrNone)	
			{
			gProviderFastLock.Signal();
			return res;
			}
		TBuf<128> threadName(serverName);
		threadName.Append(buf); //guarantee uniqueness  of the thread name
		  // Create the thread for the server.
		res = serverThread.Create(threadName,
			CSurfaceUpdateServer::ThreadFunction,
			KDefaultStackSize,
			KDefaultHeapSize,
			KDefaultHeapSize,
			(TAny*) &aSurfaceUpdateServerProvider
			);
          // The thread has been created OK so get it started - however
          // we need to make sure that it has started before we continue.
		if (res==KErrNone)
			{
			TRequestStatus rendezvousStatus;
			TThreadPriority priority = RThread().Priority();    
			serverThread.SetPriority(priority); // The same as the priority of the creating thread
			serverThread.Rendezvous(rendezvousStatus);
			serverThread.Resume();
			User::WaitForRequest(rendezvousStatus);
			res = rendezvousStatus.Int();
			gProvider = aSurfaceUpdateServerProvider;
			}
    // The thread has not been created - clearly there's been a problem.
		else
			{
			serverThread.Close();
			}
		}
       gProviderFastLock.Signal();
		return res;
	}
