// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testgceharness.h"

static TAny* globalGCEHarness = NULL;

CTestGCEHarness* CTestGCEHarness::NewL(TInt aNumBuffers)
	{
	CTestGCEHarness* self = static_cast <CTestGCEHarness*>(globalGCEHarness);
	if(!self)
		{
		self = new (ELeave) CTestGCEHarness;
		CleanupStack::PushL(self);
		self->ConstructL(aNumBuffers);
		globalGCEHarness = self;
		CleanupStack::Pop(self);
		}
	return self;
	}

void CTestGCEHarness::Remove()
	{
	CTestGCEHarness* self = static_cast <CTestGCEHarness*>(globalGCEHarness);
	if(self)
		{
		delete self;
		globalGCEHarness = NULL;
		}
	}

/* Use with care. Will leak memory if use inappropriately. Only use if objected pointed to has been
 * deleted ro otherwise removed. Required for panic testing where pointer is left hanging when thread
 * terminates.
 */
void CTestGCEHarness::ResetGlobalPointer()
	{
	globalGCEHarness = NULL;
	}

void CTestGCEHarness::SetBufferEventProcessingDelay(TRendererEvent aEventType, TTimeIntervalMicroSeconds32 aDelay)
	{
	CTestGCEHarness* self = static_cast <CTestGCEHarness*>(globalGCEHarness);
	if(!self)
		{
		return;
		}
	
	TInt count = self->iUpdateArray.Count();
	if(count == 0)
		{
		return;
		}
	
	// Get current value of buffers.
	// Processing delays are for all buffers. This function will need to change
	// if delays are to be set on a per buffer basis.
	TTimeIntervalMicroSeconds32 displayDelay = self->iUpdateArray[0].iDisplayedProcessingDelay;
	TTimeIntervalMicroSeconds32 availableDelay = self->iUpdateArray[0].iAvailableProcessingDelay;

	// set new values for delay
	if(aEventType == EEventDisplayed || aEventType == EEventAll)
		{
		displayDelay = aDelay;
		}
	
	if(aEventType == EEventAvailable || aEventType == EEventAll)
		{
		availableDelay = aDelay;
		}

	for(TInt i=0; i < count; i++)
		{
		self->iUpdateArray[i].iDisplayedProcessingDelay = displayDelay;
		self->iUpdateArray[i].iAvailableProcessingDelay = availableDelay;
		}
	}

/*
 * Use with care. It results in a reset of the harness on the next
 * call to RetrieveL(). This is necessary when the Video Renderer is in timed mode. i.e.
 * The thread that creates an AO must also delete it. 
 */
void CTestGCEHarness::ResetBuffersL(TInt aNumBuffers)
	{
	CTestGCEHarness* self = static_cast <CTestGCEHarness*>(globalGCEHarness);
	if(!self)
		{
		User::Leave(KErrNotReady);
		}
	
	self->iResetBuffers = aNumBuffers;
	}

CTestGCEHarness* CTestGCEHarness::RetrieveL()
	{
	CTestGCEHarness* self = static_cast <CTestGCEHarness*>(globalGCEHarness);
	if(!self)
		{
		User::Leave(KErrNotReady);
		}
	
	// This code is necessary for the replace tests as the timer active objects must be
	// created and deleted in the same thread, which is not always the test thread. When the
	// Video Renderer is in timed mode the thread the harness is executed in is not
	// the same as the test thread.
	if(self->iResetBuffers)
		{
		TInt buffers = self->iResetBuffers;
		self->Close(); // resets self->iResetBuffers
		self->ConstructL(buffers);
		self->Connect();
		}
	return self;
	}

TInt CTestGCEHarness::Connect()
	{
	CTestGCEHarness* self = static_cast <CTestGCEHarness*>(globalGCEHarness);
	if(!self)
		{
		return KErrNotReady;
		}
	
	TInt count = self->iUpdateArray.Count();
	for(TInt i=0; i < count; i++)
		{
		if(self->iUpdateArray[i].iAvailableTimer == NULL)
			{
			self->iUpdateArray[i].iAvailableTimer = CTestTimer::NewL(*self, i, EEventAvailable);
			}
		if(self->iUpdateArray[i].iDisplayedTimer == NULL)
			{
			self->iUpdateArray[i].iDisplayedTimer = CTestTimer::NewL(*self, i, EEventDisplayed);
			}
		}
	return KErrNone;
	}

void CTestGCEHarness::Close()
	{
	CTestGCEHarness* self = static_cast <CTestGCEHarness*>(globalGCEHarness);
	if(!self)
		{
		return;
		}
	
	TInt count = self->iUpdateArray.Count();
	for(TInt i=0; i < count; i++)
		{
		if(self->iUpdateArray[i].iAvailableTimer != NULL)
			{
			self->iUpdateArray[i].iAvailableTimer->Cancel();
			delete self->iUpdateArray[i].iAvailableTimer;
			self->iUpdateArray[i].iAvailableTimer = NULL;
			}
		if(self->iUpdateArray[i].iDisplayedTimer != NULL)
			{
			self->iUpdateArray[i].iDisplayedTimer->Cancel();
			delete self->iUpdateArray[i].iDisplayedTimer;
			self->iUpdateArray[i].iDisplayedTimer = NULL;
			}
		}
	self->iUpdateArray.Reset();
	self->iLastPosted = -1;
	self->iResetBuffers = 0;
	}

void CTestGCEHarness::CancelAllNotifications()
	{
	CTestGCEHarness* self = static_cast <CTestGCEHarness*>(globalGCEHarness);
	if(!self)
		{
		return;
		}
	
	if(self->iStatusDisplayed)
		{
		User::RequestComplete(self->iStatusDisplayed, KErrCancel);
		self->iStatusDisplayed = NULL;
		}
	
	TInt count = self->iUpdateArray.Count();
	for(TInt i=0; i < count; i++)
		{
		if(self->iUpdateArray[i].iAvailableTimer != NULL)
			{
			self->iUpdateArray[i].iAvailableTimer->Cancel();
			}
		if(self->iUpdateArray[i].iDisplayedTimer != NULL)
			{
			self->iUpdateArray[i].iDisplayedTimer->Cancel();
			}
		if(self->iUpdateArray[i].iStatusAvailable)
			{
			User::RequestComplete(self->iUpdateArray[i].iStatusAvailable, KErrCancel);
			self->iUpdateArray[i].iStatusAvailable = NULL;
			}
		}
	}

CTestGCEHarness::CTestGCEHarness()
: iLastPosted(-1)
	{
	}

CTestGCEHarness::~CTestGCEHarness()
	{
	iUpdateArray.Close();
	}

void CTestGCEHarness::ConstructL(TInt aNumBuffers)
	{
	TBufferUpdateData data;
	for(TInt i=0; i < aNumBuffers; i++)
		{
		iUpdateArray.AppendL(data);
		iUpdateArray[i].iAvailableProcessingDelay = KDefaultAvailableProcessingDelay;
		iUpdateArray[i].iDisplayedProcessingDelay = KDefaultDisplayedProcessingDelay;
		iUpdateArray[i].iDisplayedCompleteReason = KErrNone;
		iUpdateArray[i].iAvailableCompleteReason = KErrNone;
		iUpdateArray[i].iDisplayedInProgress = EFalse;
		iUpdateArray[i].iAvailableInProgress = EFalse;
		iUpdateArray[i].iAvailableTimer = NULL;
		iUpdateArray[i].iDisplayedTimer = NULL;
		iUpdateArray[i].iStatusAvailable = NULL;
		}
	}

TInt CTestGCEHarness::SubmitUpdate(TInt aBuffer)
	{
	if(aBuffer < 0 || aBuffer > iUpdateArray.Count()-1)
		{
		return KErrArgument;
		}
	
	if(iUpdateArray[aBuffer].iDisplayedInProgress || iUpdateArray[aBuffer].iAvailableInProgress)
		{
		return KErrInUse;
		}

	iUpdateArray[aBuffer].iDisplayedTimer->After(iUpdateArray[aBuffer].iDisplayedProcessingDelay);
	iUpdateArray[aBuffer].iDisplayedInProgress = ETrue;
	
	iUpdateArray[aBuffer].iStatusAvailable = iStatusAvailable;
	iStatusAvailable = NULL;
	
	if(iUpdateArray.Count() == 1)
		{
		// set off available timer - this buffer
		iUpdateArray[aBuffer].iAvailableTimer->After(iUpdateArray[aBuffer].iAvailableProcessingDelay);
		iUpdateArray[aBuffer].iAvailableInProgress = ETrue;
		}
	else if(iLastPosted > -1)
		{
		// set off available timer for iLastPosted
		iUpdateArray[iLastPosted].iAvailableTimer->After(iUpdateArray[iLastPosted].iAvailableProcessingDelay);
		iUpdateArray[iLastPosted].iAvailableInProgress = ETrue;
		// set last posted to this buffer so that it is kicked off on next submit
		iLastPosted = aBuffer;
		}
	else
		{
		iLastPosted = aBuffer;
		}

	return KErrNone;
	}

void CTestGCEHarness::NotifyWhenAvailable(TRequestStatus& aStatus)
	{
	iStatusAvailable = &aStatus;
	*iStatusAvailable = KRequestPending;
	}

void CTestGCEHarness::NotifyWhenDisplayed(TRequestStatus& aStatus, TTimeStamp& aTimeStamp)
	{
	iStatusDisplayed = &aStatus;
	iTimeStamp = &aTimeStamp;
	*iStatusDisplayed = KRequestPending;
	}

void CTestGCEHarness::OnTimer(TInt aBufferId, TRendererEvent aEvent)
	{
	if(aEvent == EEventDisplayed)
		{
		iUpdateArray[aBufferId].iDisplayedInProgress = EFalse;
		if(iStatusDisplayed)
			{
			*iTimeStamp = User::FastCounter();
			User::RequestComplete(iStatusDisplayed, iUpdateArray[aBufferId].iDisplayedCompleteReason);
			iTimeStamp = NULL;
			iStatusDisplayed = NULL;
			}
		}
	else if(aEvent == EEventAvailable)
		{
		iUpdateArray[aBufferId].iAvailableInProgress = EFalse;
		if(iUpdateArray[aBufferId].iStatusAvailable)
			{
			User::RequestComplete(iUpdateArray[aBufferId].iStatusAvailable, iUpdateArray[aBufferId].iAvailableCompleteReason);
			iUpdateArray[aBufferId].iStatusAvailable = NULL;
			}
		}
	}

CTestGCEHarness::CTestTimer*
CTestGCEHarness::CTestTimer::NewL(CTestGCEHarness& aContainer, TInt aBufferId, TRendererEvent aEvent)
	{
	CTestTimer* self = new(ELeave)CTestTimer(aContainer, aBufferId, aEvent);
 	CleanupStack::PushL(self);
 	self->ConstructL();
 	CleanupStack::Pop();
 	return self;
 	}


