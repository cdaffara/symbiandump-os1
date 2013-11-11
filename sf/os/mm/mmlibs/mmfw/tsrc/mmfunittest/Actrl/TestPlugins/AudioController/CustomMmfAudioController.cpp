// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CustomMmfAudioController.h"

#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include "ActrlTestUids.h"

_LIT(KTestWavFile19, "c:\\mm\\mmf\\testfiles\\actrl\\newmail.wav");

/**
*
* NewL
*
*/
CMMFController* CCustomMmfAudioController::NewL()
	{
    CCustomMmfAudioController* self = new(ELeave) CCustomMmfAudioController;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	
	return STATIC_CAST( CMMFController*, self );
	}
/**
*
* ~CCustomMmfAudioController
*
*/
CCustomMmfAudioController::~CCustomMmfAudioController()
	{
    delete iStoppingMessage;
	}

/**
*
* CCustomMmfAudioController
*
*/
CCustomMmfAudioController::CCustomMmfAudioController()
:CMMFAudioController(), iIsTest(EFalse)
	{
	}

/**
 *  PrimeL
 *
 *  If Prime fails the client should reset the controller
 *  becaused as noted below this code is not transactional.
 *
 */
void CCustomMmfAudioController::PrimeL(TMMFMessage& aMessage)
	{
	PrimeL();
	aMessage.Complete(KErrNone);
	}
void CCustomMmfAudioController::PlayL(TMMFMessage& aMessage)
	{
	PlayL();
	aMessage.Complete(KErrNone);
	}
void CCustomMmfAudioController::PauseL(TMMFMessage& aMessage)
	{
	PauseL();
	aMessage.Complete(KErrNone);
	} 
 
void CCustomMmfAudioController::PrimeL()
	{
	// apply heap check's here
	// Note if they fail this is the end of the controller
    
	// call the base class functionality
	TRAPD( errorCode, CMMFAudioController::PrimeL());
	// send an event back to the test framework with 
	// the error code
	TMMFEvent aEvent( KPrimeTestId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}

/**
 *
 *  PlayL
 *
 */
void CCustomMmfAudioController::PlayL()
	{
// call the base class functionality
	TRAPD( errorCode, CMMFAudioController::PlayL());
	// send an event back to the test framework with 
	// the error code
	TMMFEvent aEvent( KPlayTestId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}

/**
*
* MapdSetVolumeL
*
*/
void CCustomMmfAudioController::MapdSetVolumeL(TInt aVolume)
	{
	//Special return code to test fix for PDEF120609, test case id MM-MMF-ACLNT-U-0277-CP
	if (aVolume == -999)
		{
		User::Leave(-12345);
		}
	CMMFAudioController::MapdSetVolumeL( aVolume );
	}

/**
*
* MapdGetMaxVolumeL
*
*/
void CCustomMmfAudioController::MapdGetMaxVolumeL(TInt& aMaxVolume)
	{
	CMMFAudioController::MapdGetMaxVolumeL( aMaxVolume );
	}

/**
* MapdGetVolumeL
*
*/
void CCustomMmfAudioController::MapdGetVolumeL(TInt& aVolume)
	{
	CMMFAudioController::MapdGetVolumeL( aVolume );
	}

/**
* 
*PauseL
*
*/
void CCustomMmfAudioController::PauseL()
	{
	TRAPD( errorCode,CMMFAudioController::PauseL());
	//[ send pause event ]
	TMMFEvent aEvent( KPauseTestId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}

/**
* 
* StopL
*
*/
void CCustomMmfAudioController::StopL()
	{
	TRAPD( errorCode,CMMFAudioController::StopL());
	//[ send stop event ]
	TMMFEvent aEvent( KStopTestId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}

/**
*
* ResetL
*
*/
void CCustomMmfAudioController::ResetL()
	{
	TRAPD( errorCode,CMMFAudioController::ResetL());
	//[ send stop event ]
	TMMFEvent aEvent( KResetTestId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}

/**
*
* RemoveDataSourceL
*
*/
void CCustomMmfAudioController::RemoveDataSourceL(MDataSource& aDataSource)
	{
	TRAPD( errorCode,CMMFAudioController::RemoveDataSourceL( aDataSource));
	//[ send stop event ]
	TMMFEvent aEvent( KRemoveDataSourceTestId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}

/**
*
* RemoveDataSinkL
*
*/
void CCustomMmfAudioController::RemoveDataSinkL(MDataSink& aDataSink)
	{
	TRAPD( errorCode,CMMFAudioController::RemoveDataSinkL( aDataSink));
	//[ send stop event ]
	TMMFEvent aEvent( KRemoveDataSinkTestId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}

/**
*
* MarcSetMaxFileSizeL
*
*/

void CCustomMmfAudioController::MarcSetMaxFileSizeL(TInt aFileSize)
	{
	TRAPD( errorCode,CMMFAudioController::MarcSetMaxFileSizeL( aFileSize));
	//[ send stop event ]
	TMMFEvent aEvent( KMarcSetMaxFileSizeId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}

/**
*
* MarcGetRecordTimeAvailableL
* @param aTime
*
*/
void CCustomMmfAudioController::MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime)
	{
	TRAPD( errorCode,CMMFAudioController::MarcGetRecordTimeAvailableL( aTime));
	//[ send stop event ]
	TMMFEvent aEvent( KMarcGetRecordTimeAvailId, errorCode );
	DoSendEventToClient(aEvent);
	//[ we need to preserve the leave semantics for the caller]
	User::LeaveIfError( errorCode );
	}


/**
*
* CustomCommand
*
*/
void CCustomMmfAudioController::CustomCommand(TMMFMessage& aMessage)
	{
	//[ check if the command is for the custom plugin
	// otherwise pass it on to the real audio controller ]
	TRAPD(err, iStoppingMessage = CCustomMMFMessageHolder::NewL(aMessage));
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		return;
		}
	
	iIsTest = ETrue;
	if( IsMemoryAllocCmd( aMessage ) )
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
			TRAP( errorCode, AllocMemoryTestL());

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
			else if( errorCode != KErrNoMemory ) 
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

		//This flag is used by the audio controller to alter its behaviour
		//slightly to allow these tests to work
		iIsTest=EFalse;
		
		if(!badResult)
			{
			aMessage.Complete(KErrNone);
			}
		else
			{
			aMessage.Complete(errorCode);
			}
		}
	else
		{
		//[ let the plugin process the message ]
		CMMFAudioController::CustomCommand(aMessage);
		}
	}

/**
*
* IsMemoryAllocCmd
*
*/
TBool CCustomMmfAudioController::IsMemoryAllocCmd( TMMFMessage& )
	{
	return ETrue;
	}


static void CleanupController(TAny* ptr)
	{
	CMMFAudioController* controller = STATIC_CAST(CMMFAudioController*, ptr);
	TRAP_IGNORE(controller->CMMFAudioController::ResetL());
	}


/**
*
* AllocMemoryTest
*
*/
TInt CCustomMmfAudioController::AllocMemoryTestL()
	{
	TMMFFileConfig fileConfig; // audio file 

	fileConfig().iPath = KTestWavFile19;

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
	CMMFAudioController::AddDataSourceL(*source);
	 
	//[ add the sink ]
	CMMFAudioController::AddDataSinkL(*sink);

	//[ prime ]
	CMMFAudioController::PrimeL();	 
	 
	//[ play ]
	CMMFAudioController::PlayL();
	 
	//[ pause ]
	CMMFAudioController::PauseL();
	 
	//[ play ]
	CMMFAudioController::PlayL();
	 
	//[ stop ]
	CMMFAudioController::StopL(iStoppingMessage->iMessage);
	 
	// [ prime ]
	CMMFAudioController::PrimeL();
	 
	//[ reset ]	
	CMMFAudioController::ResetL();

	CleanupStack::PopAndDestroy(3);// source, sink, cleanupItem

	return KErrNone;
	}

//Leaves with KErrNoMemory.
CMMFController* CMemoryFailAudioController::NewL()
	{
	if(ETrue) //condition used to avoid any warning
		{
		User::Leave(KErrNoMemory);	
		}
	return NULL;
	}
CMemoryFailAudioController::CMemoryFailAudioController():CMMFAudioController()
	{
	}
CMemoryFailAudioController::~CMemoryFailAudioController()
	{
    if(iSink)
    	{
    	iSink->SinkThreadLogoff();
    	}
    if(iClip)
    	{
    	iClip->SourceThreadLogoff();
    	}
	}

void CMemoryFailAudioController::AddDataSourceL(MDataSource& aSource)
	{
    if (iClip)
    	{
    	User::Leave(KErrAlreadyExists);
    	}
	if ((aSource.DataSourceType()==KUidMmfDescriptorSource)||
			(aSource.DataSourceType()==KUidMmfFileSource))
	    {
	    User::LeaveIfError(aSource.SourceThreadLogon(*this));
	    iClip = static_cast<CMMFClip*>(&aSource);
	    if(iClip->Size())
	    	{
	    	iClip->SourcePrimeL();
	    	}
	    else
	    	{
	    	iClip->SourceStopL();
	    	}
	    }
	else 
		{
		User::Leave(KErrNotSupported);
		}
	}

void CMemoryFailAudioController::AddDataSinkL(MDataSink& aSink)
	{
    if(iMMFDevSound)
    	{
    	User::Leave(KErrAlreadyExists);
    	}
    if (aSink.DataSinkType()!=KUidMmfAudioOutput) 
    	{
    	User::Leave(KErrNotSupported);	
    	}
    User::LeaveIfError(aSink.SinkThreadLogon(*this));
    iSink = &aSink;
    MMMFAudioOutput* audioOutput = static_cast<MMMFAudioOutput*>(&aSink);
    iMMFDevSound = &(audioOutput->SoundDevice());
	iMMFDevSound->SetPrioritySettings(iPrioritySettings);
	return;
	}

void CMemoryFailAudioController::SetPrioritySettings(const TMMFPrioritySettings& aSettings)
	{
    iPrioritySettings = aSettings;
    if (iMMFDevSound)
    	{
    	iMMFDevSound->SetPrioritySettings(aSettings);
    	}
	}

//Does not acces files; returns hard-coded value.
TTimeIntervalMicroSeconds CMemoryFailAudioController::DurationL()const
	{
	return TTimeIntervalMicroSeconds(1000000); //return 1sec
	}

CMMFController* CMemoryPassAudioController::NewL()
	{
	CMemoryPassAudioController* self = new (ELeave) CMemoryPassAudioController();
	return STATIC_CAST( CMMFController*, self );
	}
CMemoryPassAudioController::CMemoryPassAudioController():CMemoryFailAudioController()
	{
	}

//Derived from Audio Controller. Panics the controller thread after the play is started
CMMFController* CPanicAudioController::NewL()
	{
	CPanicAudioController* self = new(ELeave) CPanicAudioController;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	return static_cast<CMMFController*>(self);
	}

CPanicAudioController::CPanicAudioController():CMMFAudioController()
	{
	}

CPanicAudioController::~CPanicAudioController()
	{
	delete iPanicTimer;
	}

void CPanicAudioController::AddDataSourceL(MDataSource& aSource)
	{
    TRAP_IGNORE(CMMFAudioController::AddDataSourceL(aSource));
	}

void CPanicAudioController::AddDataSinkL(MDataSink& aSink)
	{
    TRAP_IGNORE(CMMFAudioController::AddDataSinkL(aSink));
	}
	
void CPanicAudioController::PrimeL()
	{
	TRAP_IGNORE(CMMFAudioController::PrimeL());
	}
		
void CPanicAudioController::PlayL()
	{
	TRAP_IGNORE(CMMFAudioController::PlayL());
	//trigger the panic
	iPanicTimer->Start(1000000, 1000000, TCallBack(PanicTimerComplete, this));
	}
		
void CPanicAudioController::ConstructL()
	{
	CMMFAudioController::ConstructL();
	iPanicTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	}

TTimeIntervalMicroSeconds CPanicAudioController::DurationL()const
	{
	return TTimeIntervalMicroSeconds(0);
	}
	
void CPanicAudioController::SetPositionL(const TTimeIntervalMicroSeconds& aPosition)
	{
	TRAP_IGNORE(CMMFAudioController::SetPositionL(aPosition));
	}
	
TInt CPanicAudioController::PanicTimerComplete(TAny* /*aParent*/)
	{
	User::Panic(_L("CustomMmfAudioController"), 0);
	return KErrNone;
	}

