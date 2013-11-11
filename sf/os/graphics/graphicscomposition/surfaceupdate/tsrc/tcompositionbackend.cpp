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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32std.h>
#include "tcompositionbackend.h"
#include <graphics/suerror.h>

const TInt KNotificationsAtTime = 10; //how many notifications could be processed at a time, varies from 1...

CTContentUpdateReceiver::CTContentUpdateReceiver(TInt aScreen) :
	iScreen(aScreen), iVisible(ETrue)
	{
	RThread thread;
	iReceiverThreadId = thread.Id();
	}

CTContentUpdateReceiver::~CTContentUpdateReceiver()
	{
	if(iPeriodic)
		iPeriodic->Cancel();
	delete iPeriodic;
	iLock.Close();
	iPriorityLock.Close();
	}

void CTContentUpdateReceiver::ConstructL()
	{
	TCallBack callback(CallBack);
	callback.iPtr = this;
	User::LeaveIfError(iLock.CreateLocal());
	User::LeaveIfError(iPriorityLock.CreateLocal(0));
    iPeriodic= CPeriodic::NewL(CPeriodic::EPriorityStandard);
    iPeriodic->Start(TTimeIntervalMicroSeconds32(0),TTimeIntervalMicroSeconds32(KCompositionInterval), callback);
	}

CTContentUpdateReceiver* CTContentUpdateReceiver::NewL(TInt aScreen)
	{
	CTContentUpdateReceiver* receiver = new (ELeave) CTContentUpdateReceiver(aScreen);
	CleanupStack::PushL(receiver);
	receiver->ConstructL();
	CleanupStack::Pop();
	return receiver;  
	}

TInt    CTContentUpdateReceiver::Extension_(TUint aExtensionId, TAny*& aRetIface, TAny* a1)
    {
    switch (aExtensionId)
        {
        case MCompositionSurfaceUpdate::ETypeId:
            aRetIface= static_cast<MCompositionSurfaceUpdate*>(this);
            return KErrNone;

        default:    ;
        }
    return CExtensionContainer::Extension_(aExtensionId,aRetIface,a1);
    }

TInt CTContentUpdateReceiver::CallBack(TAny *aAny)
	{
	return (static_cast <CTContentUpdateReceiver*> (aAny))->CheckNewNotifications();
	}

void CTContentUpdateReceiver::Stop()
	{
    iLock.Wait();
	iStop = ETrue;
    iLock.Signal();
	}

EXPORT_C void CTContentUpdateReceiver::SetVisible(TBool aVisible)
    {
    iLock.Wait();
    iVisible = aVisible;
    iLock.Signal();
    }

TInt CTContentUpdateReceiver::CheckNewNotifications()
	{
	iLock.Wait();
 	if(iStop && (iNumberElements <= 0))
		{
	    iLock.Signal();
		CActiveScheduler::Stop();
		return 0;//the return value is irrelevant for CPeriodic function
		}
	if(iSetInternalPriority)
		{
		TRAPD(res, DoSetInternalPriorityL());
	    iLock.Signal();
		__ASSERT_ALWAYS(res ==KErrNone, User::Panic(_L("CheckNewNotifications"), KErrGeneral));
		return 0;//the return value is irrelevant for CPeriodic function
		}	
	TInt index = 0;	
	RThread thread;
	TInt res = thread.Open(iThreadId);
	__ASSERT_ALWAYS(res ==KErrNone, User::Panic(_L("CheckNewNotifications"), KErrGeneral));

		//we will check only one limited amount of requests at the time
	for(TInt iteration = 0; (iNumberElements > index) && (iteration < KNotificationsAtTime); iteration++)
		{
		if(iArray[index].iType == EReqDisplayed)
			{
			*(iArray[index].iTimeStamp) = User::FastCounter();
			if(iCompositionOrder)
				{
				iCompositionOrder->SetOrder(EOrderComposition);
				}
			}
		else if(iArray[index].iType == EReqDisplayedXTimes)
			{
			iArray[index].iDisplayedXTimes--;
			if(iArray[index].iDisplayedXTimes > 0)
				{
				index++;
				continue;
				}
			}
		TRequestStatus* status = iArray[index].iStatus;
		res = iVisible ? KErrNone : KErrNotVisible;
		Remove(index);
		thread.RequestComplete(status, res);
		}
	thread.Close();	
    iLock.Signal();
	return 0;//the return value is irrelevant for CPeriodic function
	}

void CTContentUpdateReceiver::DoSetInternalPriorityL()
	{
	RThread thread;
	User::LeaveIfError(thread.Open(iReceiverThreadId));
	thread.SetPriority(iInternalPriority);
	thread.Close();
	TInt compositionInterval = KCompositionInterval;
	CPeriodic::TPriority priority = CPeriodic::EPriorityStandard;
	if(iInternalPriority < EPriorityNormal)
		{
		priority = CPeriodic::EPriorityIdle;
		compositionInterval = KCompositionIntervalLong;
		}
	else if (iInternalPriority > EPriorityNormal)
		{
		priority = CPeriodic::EPriorityHigh;
		compositionInterval = KCompositionIntervalShort;
		}

	TCallBack callback(CallBack);
	callback.iPtr = this;
	iPeriodic->Cancel();
	delete iPeriodic;
	iPeriodic= CPeriodic::NewL(priority);
	iPeriodic->Start(TTimeIntervalMicroSeconds32(compositionInterval),TTimeIntervalMicroSeconds32(compositionInterval), callback);
	iSetInternalPriority = EFalse;
	iPriorityLock.Signal();
	}

EXPORT_C TInt CTContentUpdateReceiver::SetInternalPriority(TThreadPriority aInternalPriority)
	{
	iLock.Wait();
	iInternalPriority = aInternalPriority;
	iSetInternalPriority = ETrue;
    iLock.Signal();
	
    //wait for the priority changes takes place
    iPriorityLock.Wait();
	return KErrNone;
	}

void CTContentUpdateReceiver::ContentUpdated(const TSurfaceId& aId, 
				TInt aBuffer, 
				const TRegion* aRegion, 
				TRequestStatus* aStatusAvailable, 
				TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp, 
				TRequestStatus* aStatusDisplayedXTimes, TInt* aDisplayedXTimes)
	{
	(TAny)&aId;
	(TAny)aBuffer;
	(TAny)aRegion;
	
	iLock.Wait();
	if(iStop)
		{
		if(aStatusAvailable)
			{
			User::RequestComplete(aStatusAvailable, KErrDied);
			}
		if(aStatusDisplayed)
			{
			User::RequestComplete(aStatusDisplayed, KErrDied);
			}
		if(aStatusDisplayedXTimes)
			{
			User::RequestComplete(aStatusDisplayedXTimes, KErrDied);
			}
	    iLock.Signal();
		return;
		}
	
	RThread thread;
	iThreadId = thread.Id();
	
	if(aStatusAvailable)
		{
		Add(aStatusAvailable, EReqAvailable);
		}
	if(aStatusDisplayed)
		{
		Add(aStatusDisplayed, EReqDisplayed, 0, aTimeStamp);
		}
	if(aStatusDisplayedXTimes)
		{
		Add(aStatusDisplayedXTimes, EReqDisplayedXTimes, *aDisplayedXTimes);
		}
    iLock.Signal();
	}

/** 
   Add notification to the list. The function is called from the SUS thread.
   The client of this API must use a lock mechanizm to preserve data integrity.
*/
TInt CTContentUpdateReceiver::Add(TRequestStatus *aStatus, RequestType aType, 
			TInt aDisplayedXTimes, TUint32* aTimeStamp)
	{
 	TInt index = iNumberElements;
	TInt max = sizeof(iArray) / sizeof(iArray[0]) - 1;
	if(index >= max)
		return KErrOverflow;
	
	iArray[index].iStatus = aStatus;
	iArray[index].iType = aType;
	iArray[index].iDisplayedXTimes = aDisplayedXTimes;
	iArray[index].iTimeStamp = aTimeStamp;
	
	iNumberElements++;
	return KErrNone;
	}

/** 
   Remove notification from the list.
   The function is called from the backend thread. 
   The client of this API must use a lock mechanizm to preserve data integrity.
*/
void CTContentUpdateReceiver::Remove(TInt aIndex) 
	{
	TInt max = sizeof(iArray) / sizeof(iArray[0]) - 1;
	if((aIndex < 0) || (aIndex >= max))
			return;
	
	iNumberElements--;
	if(aIndex < iNumberElements)
		{
		Mem::Move(&iArray[aIndex], &iArray[aIndex + 1], (iNumberElements - aIndex) * sizeof(RequestObject));
		iArray[iNumberElements].iType= EReqEmpty;
		}
	else
		{
		iArray[aIndex].iType = EReqEmpty;
		}	
	}
	
TInt CTContentUpdateReceiver::ThreadFunction(TAny* aAny)
	{
	  // get clean-up stack
	CTrapCleanup* cleanup=CTrapCleanup::New();
	RThread thread;
	_LIT(KTestReceiver, "TestReceiver");
	__ASSERT_ALWAYS(cleanup!=NULL, thread.Panic( KTestReceiver, KErrNoMemory));
	
	  // create an active scheduler and server
	CActiveScheduler *pA = new CActiveScheduler;
	__ASSERT_ALWAYS(pA != NULL, thread.Panic( KTestReceiver, KErrNoMemory));

	  //Install the active scheduler
	CActiveScheduler::Install(pA);

	CTContentUpdateReceiver *pCB = NULL;
	TInt screen = * (static_cast <TInt*> (aAny));
	TRAPD(err, pCB = CTContentUpdateReceiver::NewL(screen));
	__ASSERT_ALWAYS(err == KErrNone, thread.Panic( KTestReceiver, err));
	
 	*(static_cast <CTContentUpdateReceiver**> (aAny)) = pCB;
    
      // Let everyone know that we are ready to
      // deal with requests.
	RThread::Rendezvous(KErrNone);
	  // And start fielding requests from client(s).
	CActiveScheduler::Start();

     // Tidy up... 	
	delete pCB;
	delete pA;
	delete cleanup; 
	
	return KErrNone;
	}

_LIT(KMaskBackend, "CompositionBackend_%d");
const TUint KDefaultHeapSize=0x10000;

EXPORT_C TInt StartTestUpdateReceiver(CTContentUpdateReceiver*& aReceiver, TInt aScreen)
	{
	RThread compositionThread;
	TInt res = KErrGeneral;
	TBuf<64> contentUpdateReceiverThreadName;
	TBuf<64> contentUpdateReceiverThreadMask;
	
	// Guarantee uniqueness of thread name by using timestamp
	TTime tm;
	TBuf<32> timeStamp;
	tm.UniversalTime();
	TRAP(res, tm.FormatL(timeStamp, _L("_%H%T%S%C")));
	if(res != KErrNone)
		{
		return res;
		}

	contentUpdateReceiverThreadName.Format(KMaskBackend, aScreen);
	contentUpdateReceiverThreadName.Append(timeStamp);
	contentUpdateReceiverThreadMask = contentUpdateReceiverThreadName;
	contentUpdateReceiverThreadMask.Insert(0, _L("*"));
	TFindThread findThread(contentUpdateReceiverThreadMask);
	TFullName name;
	  // Need to check that the thread exists.
	if (findThread.Next(name)!=KErrNone)
		{
		aReceiver = reinterpret_cast <CTContentUpdateReceiver*> (aScreen);
		
		  // Create the thread for the server.
		res = compositionThread.Create(contentUpdateReceiverThreadName,
			CTContentUpdateReceiver::ThreadFunction,
			KDefaultStackSize,
			KDefaultHeapSize,
			KDefaultHeapSize,
			(TAny*) &aReceiver
			);
			
          // The thread has been created OK so get it started - however
          // we need to make sure that it has started before we continue.
		if (res==KErrNone)
			{
			TRequestStatus rendezvousStatus;
			compositionThread.SetPriority(EPriorityNormal);
			compositionThread.Rendezvous(rendezvousStatus);
			compositionThread.Resume();
			User::WaitForRequest(rendezvousStatus);
			res = rendezvousStatus.Int();
			}
		}
		compositionThread.Close();
		return res;
	}

EXPORT_C void CloseTestUpdateReceiver(CTContentUpdateReceiver* aReceiver)
	{
	if(!aReceiver)
		return;

	TBuf<64> contentUpdateReceiverThreadName;
	contentUpdateReceiverThreadName.Format(KMaskBackend, aReceiver->Screen());
	TBuf<64> contentUpdateReceiverThreadMask;
	contentUpdateReceiverThreadMask = contentUpdateReceiverThreadName;
	contentUpdateReceiverThreadMask.Insert(0, _L("*"));
	contentUpdateReceiverThreadMask.Append('*');
	TFindThread findThread(contentUpdateReceiverThreadMask);
	TFullName name;
	RThread thread;
	if((findThread.Next(name)!=KErrNone) ||
		(thread.Open(findThread) != KErrNone))
		{
		thread.Close();
		return;
		}
	TRequestStatus status; 
	thread.Logon(status);
	if(aReceiver)
		aReceiver->Stop();
	User::WaitForRequest(status);
	thread.Close();
	}
