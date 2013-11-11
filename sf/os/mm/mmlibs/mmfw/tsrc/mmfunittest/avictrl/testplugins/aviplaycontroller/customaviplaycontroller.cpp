// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <mmf/server/mmffile.h>
#include "customaviplaycontroller.h"
#include "tsu_mmf_avictrltestids.h"

#define KInitIncompleteCode KMaxTInt

_LIT(KTestAviFile, "c:\\mm\\mmf\\testfiles\\avictrl\\aud_vid.avi");

/**
*
* NewL
*
*/
CMMFController* CCustomAviPlayController::NewL()
	{
    CCustomAviPlayController* self = new(ELeave) CCustomAviPlayController;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	
	return STATIC_CAST( CMMFController*, self );
	}

/**
*
* CCustomAviPlayController
*
*/
CCustomAviPlayController::~CCustomAviPlayController()
	{
	if (iDataSource)
		{
		delete iDataSource;
		}
	if (iDataSink)
		{
		delete iDataSink;
		}
		
    }

/**
*
* CCustomAviPlayController
*
*/
CCustomAviPlayController::CCustomAviPlayController() :	CAviPlayController()
	{
	}

/**
*
* Override base class ResetL
*
*/
void CCustomAviPlayController::ResetL()
	{
	CAviPlayController::ResetL();
	if (iDataSource)
		{
		delete iDataSource;
		iDataSource = NULL;
		}
	if (iDataSink)
		{
		delete iDataSink;
		iDataSink = NULL;
		}
	}
	
/**
*
* Override base class PrimeL
*
*/
void CCustomAviPlayController::PrimeL(TMMFMessage& aMessage)
	{
	if (iMemAllocPrimeTestEnabled)
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,iMemAllocPrimeFailCount);
		TRAPD(err, CAviPlayController::PrimeL(aMessage));
		__UHEAP_SETFAIL(RHeap::ENone ,0);
		User::LeaveIfError(err);
		}
	else
		{
		CAviPlayController::PrimeL(aMessage);
		}
	}

/**
*
* Override base class CustomCommand
*
*/
void CCustomAviPlayController::CustomCommand(TMMFMessage& aMessage)
	{
	//[ check if the command is for the custom plugin
	// otherwise pass it on to the real audio controller ]
	/*if (iPrimingMessage)
		{
		delete iPrimingMessage;
		iPrimingMessage = NULL;
		}
	TRAPD(err, iPrimingMessage = CCustomMMFMessageHolder::NewL(aMessage));
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return;
		}*/
	
	if (aMessage.Destination().InterfaceId() == KMemoryAllocationTestId)
		{
		switch (aMessage.Function())
			{
		case KMemoryAllocTest1:
			DoMemoryAllocTest1(aMessage);
			break;
		case KMemoryAllocTest2:
			DoPrepareMemoryAllocTest2(aMessage);
			break;
		case KMemoryAllocTest3:
			DoMemoryAllocTest3(aMessage);
			break;
		case KOOMPrimeTestEnable:
			DoSetPrimetTestEnable(aMessage);
			break;
		case KOOMPrimeTestFailCount:
			DoSetPrimetTestFailCount(aMessage);
			break;
			}
		}
	else
		{
		//[ let the plugin process the message ]
		CAviPlayController::CustomCommand(aMessage);
		}
	}

static void CleanupController(TAny* ptr)
	{
	CCustomAviPlayController* controller = STATIC_CAST(CCustomAviPlayController*, ptr);
	TRAP_IGNORE(controller->ResetL());
	}
	
/**
*
* ConstructL
*
*/
void CCustomAviPlayController::ConstructL()
	{
	CAviPlayController::ConstructL();
	}

/**
*
* DoMemoryAllocTest1
*
*/
void CCustomAviPlayController::DoMemoryAllocTest1(TMMFMessage& aMessage)
	{
	//[ it is a alloc memory test command ]
	// [ new algorithm adopted from M&G database ]
	//
	TInt errorCode = KErrNone;
	TInt failCount  = 1;
	TBool completed  = EFalse;
	TBool badResult  = EFalse;
	TBool reachedEnd = EFalse; 
	
	for( ; ; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);	// Leavescan will think __UHEAP_SETFAIL is a leaving function due to the macro ending in 'L'. Does not leave
		//NB: Do not use __MM_HEAP_MARK macro's in this test, the cleaning up the CustomMMFAudioController
		//is insufficient to remove all possible allocated memory from the framework.
		//CMMFControllerProxyServer::DoStartThreadL has been updated to do heap checking for 
		//the whole server thread.

		//[ run a scenario of the major api 
		//functions which alloc memory in the controller]
		TRAP( errorCode, 			
			TMMFFileConfig fileConfig; // video file 
			fileConfig().iPath = KTestAviFile;

			//[ lets manufacture a source and sink ]
			//[Create the source]
			MDataSource* source = MDataSource::NewSourceL(KUidMmfFileSource, fileConfig);
			CleanupDeletePushL(source);

			//[ Create the sink ]
			MDataSink* sink = MDataSink::NewSinkL(KUidMmfAudioOutput, KNullDesC8);
			CleanupDeletePushL(sink);

			// Use a cleanup item to stop & reset the controller and so remove the data 
			// sources/sinks automatically when this funtion leaves. This can be done
			// before the sinks/sources are added.
			TCleanupItem cleanupItem(CleanupController, this);
			CleanupStack::PushL(cleanupItem);

			//[ add the source ]
			CAviPlayController::AddDataSourceL(*source);
			 
			//[ add the sink ]
			CAviPlayController::AddDataSinkL(*sink);

			CleanupStack::PopAndDestroy(3);// source, sink, cleanupItem
		);
		if( errorCode == KErrNone )
			{
			// [ check we have passed through all allocs in the test]
			TAny* testAlloc = User::Alloc(1);
			if( testAlloc == NULL )
				{
				reachedEnd = ETrue;
				failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.
				}
			else 
				{
				User::Free( testAlloc );
				}

			completed = reachedEnd || badResult;
			
			}
		else if( errorCode != KErrNoMemory && errorCode != KErrCorrupt) 
			{
			// [ we failed for some reason other than memory
			//     allocation, so fail the test ]
			completed = ETrue;
			badResult = ETrue;
			}
		
		__UHEAP_SETFAIL(RHeap::ENone ,0);
		
		// [ exit the loop ]
		if( completed )
			{
			break;
			}

		failCount +=1;
		}
	if(!badResult)
		{
		aMessage.Complete(KErrNone);
		}
	else
		{
		aMessage.Complete(errorCode);
		}
	}

/**
*
* DoMemoryAllocTest2
*
*/
void CCustomAviPlayController::DoPrepareMemoryAllocTest2(TMMFMessage& aMessage)
	{
	TRAPD(err, 
		TMMFFileConfig fileConfig; // video file 
		fileConfig().iPath = KTestAviFile;

		// Use a cleanup item to stop & reset the controller and so remove the data 
		// sources/sinks automatically when this funtion leaves. This can be done
		// before the sinks/sources are added.
		TCleanupItem cleanupItem(CleanupController, this);
		CleanupStack::PushL(cleanupItem);
		
		//[ lets manufacture a source and sink ]
		//[Create the source]
		if (iDataSource)
			{
			delete iDataSource;
			iDataSource = NULL;
			}
		iDataSource = MDataSource::NewSourceL(KUidMmfFileSource, fileConfig);
		
		//[ Create the sink ]
		if (iDataSink)
			{
			delete iDataSink;
			iDataSink = NULL;
			}
		iDataSink = MDataSink::NewSinkL(KUidMmfAudioOutput, KNullDesC8);
		
		//[ add the source ]
		CAviPlayController::AddDataSourceL(*iDataSource);
		 
		//[ add the sink ]
		CAviPlayController::AddDataSinkL(*iDataSink);

		CleanupStack::Pop(1);	// cleanupItem
		);
	aMessage.Complete(err);
	}

/**
*
* DoMemoryAllocTest3
*
*/
void CCustomAviPlayController::DoMemoryAllocTest3(TMMFMessage& aMessage)
	{
	__UHEAP_SETFAIL(RHeap::EFailNext ,iMemAllocPrimeFailCount);
	TRAPD(err, 
		// Use a cleanup item to stop & reset the controller and so remove the data 
		// sources/sinks automatically when this funtion leaves. This can be done
		// before the sinks/sources are added.
		TCleanupItem cleanupItem(CleanupController, this);
		CleanupStack::PushL(cleanupItem);
		
		//[ play ]
		CAviPlayController::PlayL();
		 
		//[ stop ]
		CAviPlayController::StopL();
		 
		//[ reset ]	
		CAviPlayController::ResetL();

		CleanupStack::PopAndDestroy(1);	// cleanupItem
		);
	__UHEAP_SETFAIL(RHeap::ENone ,0);
	aMessage.Complete(err);
	}
	
/**
*
* DoSetPrimetTestEnable
*
*/
void CCustomAviPlayController::DoSetPrimetTestEnable(TMMFMessage& aMessage)
	{
	TPckgBuf<TBool> primeTestEnable;
	TInt err = aMessage.ReadData1FromClient(primeTestEnable);
	aMessage.Complete(err);
	iMemAllocPrimeTestEnabled = primeTestEnable();
	}

/**
*
* DoSetPrimetTestFailCount
*
*/
void CCustomAviPlayController::DoSetPrimetTestFailCount(TMMFMessage& aMessage)
	{
	TPckgBuf<TInt> primeTestFailCount;
	TInt err = aMessage.ReadData1FromClient(primeTestFailCount);
	aMessage.Complete(err);
	iMemAllocPrimeFailCount = primeTestFailCount();
	}
	
