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

#ifndef TESTGCEHARNESS_H
#define TESTGCEHARNESS_H

#include <e32base.h>
#include <graphics/surfaceupdateclient.h>

// The default processing delay for displaty and available events. Test specific delays can be set using
// SetBufferEventProcessingDelay().
static const TInt KDefaultDisplayedProcessingDelay = 2000;
static const TInt KDefaultAvailableProcessingDelay = 2000;

class CTestGCEHarness : public CBase
	{
public:
	class CTestTimer;
	
	enum TRendererEvent
		{
		EEventDisplayed,
		EEventAvailable,
		EEventAll
		};
	
	// The following functions are for use by the testframework test steps only

	// Create global instance
	static CTestGCEHarness* NewL(TInt aNumBuffers);

	// Remove global instance - object must have ben closed first. Video Renderer destruction calls
	// RSurfaceUpdateSession::Close() whihc in turn calls CTestGCEHarness::Close()
	static void Remove();

    // Resets object to use the supplied number of buffers
	static void ResetBuffersL(TInt aNumBuffers);
	
	// Resets global pointer to NULL - used in test pre amble. Required because Panic tests leave the
    // pointer hanging which if left causes the following test to crash. 
	static void ResetGlobalPointer();
	
	// Allows delay to be set on a per test basis. If used before call to NewL() will fail silently.
	static void SetBufferEventProcessingDelay(TRendererEvent aEventType, TTimeIntervalMicroSeconds32 aDelay);
	
	// The following functions are for use by the test RSurfaceUpdateSession implementation only
	
    // Returns pointer to global pointer - leaves if object has not been created previously 
    static CTestGCEHarness* RetrieveL();
	
	// The following are called directly from same named functions in test RSurfaceUpdateSession implementation
    TInt Connect();
	void Close();
	void CancelAllNotifications();
	TInt SubmitUpdate(TInt aBuffer);
	void NotifyWhenAvailable(TRequestStatus& aStatus);
	void NotifyWhenDisplayed(TRequestStatus& aStatus, TTimeStamp& aTimeStamp);
    
    // destructor
    ~CTestGCEHarness();
    
    // data per buffer 
    struct TBufferUpdateData
    	{
    	TTimeIntervalMicroSeconds32 iDisplayedProcessingDelay;
    	TTimeIntervalMicroSeconds32 iAvailableProcessingDelay;
    	TInt iDisplayedCompleteReason;
       	TInt iAvailableCompleteReason;
       	TBool iDisplayedInProgress;
       	TBool iAvailableInProgress;
       	CTestTimer* iDisplayedTimer;
       	CTestTimer* iAvailableTimer;
       	TRequestStatus *iStatusAvailable;
    	};

private:
    CTestGCEHarness();
	void ConstructL(TInt aNumBuffers);
	
	TRequestStatus *iStatusAvailable;
	TRequestStatus *iStatusDisplayed;
	TTimeStamp* iTimeStamp;
	RArray<TBufferUpdateData> iUpdateArray;
	TInt iLastPosted;
	TInt iResetBuffers;
	
	void OnTimer(TInt aBufferId, TRendererEvent aEvent);

	class CTestTimer : public CTimer
		{
		public:
			static CTestTimer* NewL(CTestGCEHarness& aContainer, TInt aBufferId, TRendererEvent aEvent);
			CTestTimer(CTestGCEHarness& aContainer, TInt aBufferId, TRendererEvent aEvent):CTimer(EPriorityStandard),iContainer(aContainer),iBufferId(aBufferId), iEvent(aEvent)
				{
				CActiveScheduler::Add(this);	
				}
		protected:
			void RunL()
				{
				iContainer.OnTimer(iBufferId, iEvent);
				}
		private:
			CTestGCEHarness& iContainer;
			TInt iBufferId;
			TRendererEvent iEvent;
		};
	};

#endif // TESTGCEHARNESS_H
