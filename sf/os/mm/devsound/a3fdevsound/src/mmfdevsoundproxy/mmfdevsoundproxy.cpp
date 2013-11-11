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



// INCLUDE FILES
#include "mmfdevsoundproxy.h"
#ifdef _DEBUG
#include <e32debug.h>

#define SYMBIAN_DEBPRN0(str)                RDebug::Print(str, this)
#define SYMBIAN_DEBPRN1(str, val1)          RDebug::Print(str, this, val1)
#define SYMBIAN_DEBPRN2(str, val1, val2)    RDebug::Print(str, this, val1, val2)
#else
#define SYMBIAN_DEBPRN0(str)
#define SYMBIAN_DEBPRN1(str, val1)
#define SYMBIAN_DEBPRN2(str, val1, val2)
#endif //_DEBUG

// SYMBIAN_CHECK used to add extra asserts when MACRO is added - helps debugging overall A3F

#ifdef SYMBIAN_FULL_STATE_CHECK
   #define SYMBIAN_CHECK(c,p) __ASSERT_ALWAYS(c,p)
#else
   #define SYMBIAN_CHECK(c,p)
#endif 

const TInt KMaxMessageQueueItems = 8;

const TMMFCapabilities KZeroCapabilities = 
	{
	0,0,0,0 // all zero's
	};

// ============================ LOCAL FUNCTIONS ================================

// -----------------------------------------------------------------------------
// This function raises a panic
//
// @param	aError
// 		one of the several panic codes that may be raised by this dll
//
// @panic	EMMFDevSoundProxyPlayDataWithoutInitialize is raised when playdata
//          is called without initialization
// @panic	EMMFDevSoundProxyRecordDataWithoutInitialize is raised when
//          recorddata is called without initialization
//
GLDEF_C void Panic(TMMFDevSoundProxyPanicCodes aPanicCode)
	{
	User::Panic(KMMFDevSoundProxyPanicCategory, aPanicCode);
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RMMFDevsoundProxy::RMMFDevsoundProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RMMFDevSoundProxy::RMMFDevSoundProxy() :
	iBuffer(NULL),
	iSeqName(NULL),
	iDestinationPckg(TMMFMessageDestination(KUidInterfaceMMFDevSound,
											KMMFObjectHandleDevSound)),
	iState(EIdle),
	iAudioServerProxy(NULL),
	iDevSoundObserver(NULL),
	iMsgQueueHandler(NULL),
	iCustIntPckg()
	{
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::Close
// Close the server session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::Close()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Close - Enter"));
	if (iAudioServerProxy)
		{
		if (iAudioServerProxy->Handle() != NULL)
			{
			TMMFDevSoundProxySettings set;
			TMMFDevSoundProxySettingsPckg pckg(set);
			TInt err = SendReceive(EMMFDevSoundProxyClose,
						iDestinationPckg,
						pckg);
			}
		iAudioServerProxy->Close();
		delete iAudioServerProxy;
		iAudioServerProxy = NULL;
		}
	RMmfSessionBase::Close();
	iState = EIdle;
	if (iMsgQueueHandler)
		{
		iMsgQueueHandler->Cancel();
		delete iMsgQueueHandler;
		iMsgQueueHandler = NULL;
		}
	iMsgQueue.Close();
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Close - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::Open
// Open a DevSound server session
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::Open()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Open - Enter"));
	TInt err = iMsgQueue.CreateGlobal(KNullDesC, KMaxMessageQueueItems, EOwnerThread);
	// global, accessible to all that have its handle
	
	if (err == KErrNone)
		{
		iAudioServerProxy = NULL;
		iMsgQueueHandler = NULL;
		iBuffer = NULL;
		iAudioServerProxy = new RMMFAudioServerProxy();
		if (iAudioServerProxy == NULL)
			{
			err = KErrNoMemory;
			}
		}
	if (err == KErrNone)	
		{
		err = iAudioServerProxy->Open();
		}
	if (err == KErrNone)
		{
		err = SetReturnedHandle(iAudioServerProxy->GetDevSoundSessionHandle());
		}
	if (err)
		{	
		Close();
		}
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::Open - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::PostOpen
// Finish opening process
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::PostOpen()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PostOpen - Enter"));
	TInt err = SendReceive(EMMFDevSoundProxyPostOpen, iDestinationPckg);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::PostOpen - Exit [%d]"), err);
	return err;
	}


// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetDevSoundInfo
// Launch DevSound that might have been waiting for audio policy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::SetDevSoundInfo()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetDevSoundInfo - Enter"));
	TInt err = SendReceive(EMMFAudioLaunchRequests);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::SetDevSoundInfo - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::InitializeL
// Initialize DevSound for a specific mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::InitializeL(
							MDevSoundObserver& aDevSoundObserver,
							TMMFState aMode,
							MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::InitializeL - Enter"));
	TInt err = KErrNone;
	iDevSoundObserver = &aDevSoundObserver;
	
	if (!(iState==EIdle || iState==EInitialized))
		{
		err = KErrNotReady;
		}
	else
		{
		TMMFDevSoundProxySettings set;
		set.iMode = aMode;
		TMMFDevSoundProxySettingsPckg pckg(set);
		TIpcArgs args(&iDestinationPckg, &pckg, iMsgQueue);
		err = RSessionBase::SendReceive(EMMFDevSoundProxyInitialize1, args);		
		if (err == KErrNone)
			{
			StartReceivingMsgQueueHandlerEventsL(aDevSoundCIObserver);
			iState = EInitializing;
			}	
		}
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::InitializeL - Exit [%d]"), err);
	User::LeaveIfError(err);
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::InitializeL
// Initialize DevSound with specific HwDevice id and mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::InitializeL(
								MDevSoundObserver& /*aDevSoundObserver*/,
								TUid /*aHWDev*/,
								TMMFState /*aMode*/,
								MMMFDevSoundCustomInterfaceObserver& /*aDevSoundCIObserver*/)
	{
	TInt err = KErrNotSupported;
	User::LeaveIfError(err);
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::InitializeL
// Initialize DevSound for the specific FourCC and mode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::InitializeL(
								MDevSoundObserver& aDevSoundObserver,
								TFourCC aDesiredFourCC,
								TMMFState aMode,
								MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::InitializeL - Enter"));
	TInt err = KErrNone;
	if(aMode == EMMFStateTonePlaying)
		{
		User::Leave(KErrNotSupported);
		}
	iDevSoundObserver = &aDevSoundObserver;
	if (!(iState==EIdle || iState==EInitialized))
		{
		err = KErrNotReady;
		}
	else
		{
		TMMFDevSoundProxySettings set;
		set.iDesiredFourCC = aDesiredFourCC;
		set.iMode = aMode;
		TMMFDevSoundProxySettingsPckg pckg(set);
		TIpcArgs args(&iDestinationPckg, &pckg, iMsgQueue);
		err = RSessionBase::SendReceive(EMMFDevSoundProxyInitialize4, args);
		if (err == KErrNone)
			{
			StartReceivingMsgQueueHandlerEventsL(aDevSoundCIObserver);
			iState = EInitializing;
			}	
		}
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::InitializeL - Exit [%d]"), err);
	User::LeaveIfError(err);
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::Capabilities
// Returns the capabilities of the DevSound server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TMMFCapabilities RMMFDevSoundProxy::Capabilities()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Capabilities - Enter"));
	// TODO should we use the following ? SYMBIAN_CHECK(iState>=EInitialized, Panic(EMMFDevSoundProxyCapabilitiesInWrongState));
	if (iState < EInitialized)
		{
		// call has been made before we are initialized. Not much we can do, so return
		// dummy values but hit debugger on the emulator
		__DEBUGGER()
		RDebug::Print(_L("BRDBG:CapabilitiesCalledWhenNotInitialised")); // TODO Remove or redo as trace
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Capabilities - Exit"));
		return KZeroCapabilities;
		}
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	TInt err = SendReceiveResult(EMMFDevSoundProxyCapabilities,
					iDestinationPckg,
					KNullDesC8,
					pckg);
	if (err == KErrNone)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Capabilities - Exit"));
		return pckg().iCaps;
		}
	else
		{
		SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::Capabilities - Exit [%d]"), err);
		return KZeroCapabilities;
		}
	}

// -----------------------------------------------------------------------------
// TMMFCapabilities RMMFDevSoundProxy::Config
// Returns the current configuration of the DevSound.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TMMFCapabilities RMMFDevSoundProxy::Config()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Config - Enter"));
	// TODO should we use the following ? SYMBIAN_CHECK(iState>=EInitialized, Panic(EMMFDevSoundProxyConfigInWrongState));
	if (iState < EInitialized)
		{
		// call has been made before we are initialized. Not much we can do, so return
		// dummy values but hit debugger on the emulator
		__DEBUGGER()
		RDebug::Print(_L("BRDBG:ConfigCalledWhenNotInitialised")); // TODO Remove or redo as trace
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Config - Exit"));
		return KZeroCapabilities;
		}
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyConfig,
					iDestinationPckg,
					KNullDesC8,
					pckg);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Config - Exit"));
	return pckg().iConfig;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetConfigL
// Configures the DevSound server.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::SetConfigL(
	const TMMFCapabilities& aConfig )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetConfigL - Enter"));
	TInt err = KErrNone;
	
	if (iState==EInitialized)
		{
		TMMFDevSoundProxySettings set;
		set.iConfig = aConfig;
		TMMFDevSoundProxySettingsPckg pckg(set);
		err = SendReceive(EMMFDevSoundProxySetConfig,
							iDestinationPckg,
							pckg);
		}
	else
		{
		RDebug::Print(_L("BRDBG:SetConfigCalledWhenNotInitialised")); // TODO Remove or redo as trace				
		err = KErrNotReady;
		}
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::SetConfigL - Exit [%d]"), err);
	User::LeaveIfError(err);
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::MaxVolume
// Returns the maximum volume supported by DevSound server for playing back..
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::MaxVolume()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::MaxVolume - Enter"));
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyMaxVolume,
					iDestinationPckg,
					KNullDesC8,
					pckg);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::MaxVolume - Exit"));
	return pckg().iMaxVolume;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::Volume
// Returns the current volume.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::Volume()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Volume - Enter"));
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyVolume,
					iDestinationPckg,
					KNullDesC8,
					pckg);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Volume - Exit"));
	return pckg().iVolume;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetVolume
// Sets the current volume.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::SetVolume(TInt aVolume )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetVolume - Enter"));
	TMMFDevSoundProxySettings set;
	set.iVolume = aVolume;
	TMMFDevSoundProxySettingsPckg pckg(set);
	TInt err = SendReceive(EMMFDevSoundProxySetVolume,
					iDestinationPckg,
					pckg);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::SetVolume - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::MaxGain
// Returns maximum gain supported by DevSound server for recording.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::MaxGain()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::MaxGain - Enter"));
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyMaxGain,
					iDestinationPckg,
					KNullDesC8,
					pckg);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::MaxGain - Exit"));
	return pckg().iMaxGain;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::Gain
// Returns the current gain.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::Gain()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Gain - Enter"));
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyGain,
				iDestinationPckg,
				KNullDesC8,
				pckg);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Gain - Exit"));
	return pckg().iGain;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetGain
// Sets the current gain.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::SetGain(
	TInt aGain )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetGain - Enter"));
	TMMFDevSoundProxySettings set;
	set.iGain = aGain;
	TMMFDevSoundProxySettingsPckg pckg(set);

	TInt err = SendReceive(EMMFDevSoundProxySetGain,
					iDestinationPckg,
					pckg);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::SetGain - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::GetPlayBalanceL
// Returns play balance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::GetPlayBalanceL(
		TInt& aLeftPercentage,
		TInt& aRightPercentage )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetPlayBalanceL - Enter"));
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyPlayBalance,
									iDestinationPckg,
									KNullDesC8,
									pckg));
	aLeftPercentage = pckg().iLeftPercentage;
	aRightPercentage = pckg().iRightPercentage;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetPlayBalanceL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetPlayBalanceL
// Sets playbalance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::SetPlayBalanceL(
					TInt aLeftPercentage,
					TInt aRightPercentage )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetPlayBalanceL - Enter"));
	TMMFDevSoundProxySettings set;
	set.iLeftPercentage = aLeftPercentage;
	set.iRightPercentage = aRightPercentage;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceive(EMMFDevSoundProxySetPlayBalance,
								iDestinationPckg,
								pckg));
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetPlayBalanceL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::GetRecordBalanceL
// Returns record balance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::GetRecordBalanceL(
					TInt& aLeftPercentage,
					TInt& aRightPercentage )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetRecordBalanceL - Enter"));
	TMMFDevSoundProxySettings set;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyRecordBalance,
										iDestinationPckg,
										KNullDesC8,
										pckg));
	aLeftPercentage = pckg().iLeftPercentage;
	aRightPercentage = pckg().iRightPercentage;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetRecordBalanceL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetRecordBalanceL
// Sets record balance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::SetRecordBalanceL(
				TInt aLeftPercentage,
				TInt aRightPercentage )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetRecordBalanceL - Enter"));
	TMMFDevSoundProxySettings set;
	set.iLeftPercentage = aLeftPercentage;
	set.iRightPercentage = aRightPercentage;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceive(EMMFDevSoundProxySetRecordBalance,
									iDestinationPckg,
									pckg));
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetRecordBalanceL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::PlayInitL
// Initilaizes DevSound to play digital audio and starts the play process.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::PlayInitL()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayInitL - Enter"));
	if (!iDevSoundObserver || iState!=EInitialized)
		{
		if (iState == EPlaying || iState == EPlayingBufferWait)
		    {
		    // treat PlayInitL() during play as Resume()
		    User::LeaveIfError(Resume());
		    SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayInitL - Exit"));
		    return;
		    }
		User::Leave(KErrNotReady);
		}

	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayInit,
							iDestinationPckg));
	iState = EPlaying;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayInitL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::RecordInitL
// Initilaizes DevSound to record digital audio and starts the record process.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::RecordInitL()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::RecordInitL - Enter"));
	if (!iDevSoundObserver || iState!=EInitialized)
		{
		if(iState == ERecording || iState == ERecordingBufferWait || iState == ERecordingInLastBufferCycle 
                                                                  || iState == ERecordingResumingInLastBufferCycle)
		    { 
		    // treat RecordInitL() during record as Resume()
            User::LeaveIfError(Resume());
            SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::RecordInitL - Exit"));
            return;
		    }
        User::Leave(KErrNotReady);
        }
	
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyRecordInit,
								iDestinationPckg));
	iState = ERecording;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::RecordInitL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::PlayData
// Plays the data in the buffer at the current volume.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::PlayData()
	{
	//SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayData - Enter")); //Uncommenting this will produce a lot of logging!
	__ASSERT_ALWAYS(iState == EPlaying || iState == EPlayingBufferWait,
				Panic(EMMFDevSoundProxyPlayDataWithoutInitialize));
	ASSERT(iDevSoundObserver);
	SYMBIAN_CHECK( iState == EPlayingBufferWait,
				Panic(EMMFDevSoundProxyPlayDataInWrongState));
	TMMFDevSoundProxyHwBuf set;
	set.iLastBuffer = iBuffer->LastBuffer();
	set.iBufferSize = iBuffer->Data().Size();
	TMMFDevSoundProxyHwBufPckg pckg(set);

	SendReceive(EMMFDevSoundProxyPlayData, iDestinationPckg, pckg);
	iState = EPlaying;
	//SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayData - Exit")); //Uncommenting this will produce a lot of logging!
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::RecordData
// Signals the device to continue recording.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::RecordData()
	{
	//SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::RecordData - Enter")); //Uncommenting this will produce a lot of logging!
	__ASSERT_ALWAYS(iState == ERecording || iState == ERecordingBufferWait ||
                    iState == ERecordingInLastBufferCycle || iState == ERecordingResumingInLastBufferCycle,
					Panic(EMMFDevSoundProxyRecordDataWithoutInitialize));
	ASSERT(iDevSoundObserver);
	SYMBIAN_CHECK(iState == ERecordingBufferWait || iState == ERecordingInLastBufferCycle ||
                iState == ERecordingResumingInLastBufferCycle,
                    Panic(EMMFDevSoundProxyPlayDataInWrongState));
	switch (iState)
	    {
	    case ERecordingBufferWait:
	        // standard case
	        SendReceive(EMMFDevSoundProxyRecordData, iDestinationPckg);
	        iState = ERecording;
	        break;
	    case ERecordingInLastBufferCycle:
	        // ack of the last buffer. Just swallow - the server should not be sent an ack
	        iState = ERecording; 
	        break;
	    case ERecordingResumingInLastBufferCycle:
	        // this is a RecordData() following a Resume() in the last cycle. This is where we do the resume!
	        SendReceive(EMMFDevSoundProxyResume, iDestinationPckg); // note ignore any error
	        iState = ERecording;
	        break;
	    }
	//SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::RecordData - Exit")); //Uncommenting this will produce a lot of logging!
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::Stop
// Stops the ongoing opeartion.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::Stop()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Stop - Enter"));
	if (iState > EInitialized)
		{
		SendReceive(EMMFDevSoundProxyStop, iDestinationPckg);
		iState = EInitialized;	
		iMsgQueueHandler->Finish(); // will delete the buffer
		}
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Stop - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::Pause
// Temporarily stops the ongoing operation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::Pause()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Pause - Enter"));
	if(iState > EInitialized)
	    {
	    SendReceive(EMMFDevSoundProxyPause, iDestinationPckg);
	    }
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Pause - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::PlayToneL
// Plays the simple tone.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::PlayToneL(
								TInt aFrequency,
								const TTimeIntervalMicroSeconds& aDuration)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayToneL - Enter"));
	if(iState==ETonePlaying)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayToneL - Exit"));
		return;
		}
	
	if (!iDevSoundObserver || iState!=EInitialized)
		{
		User::Leave(KErrNotReady);
		}

	TMMFDevSoundProxySettings set;
	set.iFrequencyOne = aFrequency;
	set.iDuration = aDuration;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayTone,
								iDestinationPckg,
								pckg));
	iState = ETonePlaying;
	iToneMode = ESimple;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayToneL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::PlayDualToneL
// Plays the dual tone.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::PlayDualToneL(
									TInt aFrequencyOne,
									TInt aFrequencyTwo,
									const TTimeIntervalMicroSeconds& aDuration)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayDualToneL - Enter"));
	if(iState==ETonePlaying)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayDualToneL - Exit"));
		return;
		}
			
	if (!iDevSoundObserver || iState!=EInitialized)
		{
		User::Leave(KErrNotReady);
		}

	TMMFDevSoundProxySettings set;
	set.iFrequencyOne = aFrequencyOne;
	set.iFrequencyTwo = aFrequencyTwo;
	set.iDuration = aDuration;
	TMMFDevSoundProxySettingsPckg pckg(set);
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayDualTone, iDestinationPckg, pckg));
	iState = ETonePlaying;
	iToneMode = EDual;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayDualToneL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::PlayDTMFStringL
// Plays the DTMF string.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::PlayDTMFStringL(const TDesC& aDTMFString)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayDTMFStringL - Enter"));
	if(iState==ETonePlaying)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayDTMFStringL - Exit"));
		return;
		}
			
	if (!iDevSoundObserver || iState!=EInitialized)
		{
		User::Leave(KErrNotReady);
		}

	TPtr tempPtr(0,0);
	tempPtr.Set(CONST_CAST(TUint16*, aDTMFString.Ptr()),
					aDTMFString.Length(),
					aDTMFString.Length());

	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyPlayDTMFString,
						iDestinationPckg,
						KNullDesC8,
						tempPtr));
	iState = ETonePlaying;
	iToneMode = EDTMFString;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayDTMFStringL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::PlayToneSequenceL
// Plays the tone sequence. (NRT/RNG)
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::PlayToneSequenceL(const TDesC8& aData )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayToneSequenceL - Enter"));
	if(iState==ETonePlaying)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayToneSequenceL - Exit"));
		return;
		}
			
	if (!iDevSoundObserver || iState!=EInitialized)
		{
		User::Leave(KErrNotReady);
		}

	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayToneSequence,
								iDestinationPckg,
								aData));
	iState = ETonePlaying;
	iToneMode = ESequence;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayToneSequenceL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::PlayFixedSequenceL
// Plays the fixed sequence.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::PlayFixedSequenceL(TInt aSequenceNumber)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayFixedSequenceL - Enter"));
	if(iState==ETonePlaying)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayFixedSequenceL - Exit"));
		return;
		}
			
	if (!iDevSoundObserver || iState!=EInitialized)
		{
		User::Leave(KErrNotReady);
		}

	TPckgBuf<TInt> seqNum(aSequenceNumber);
	User::LeaveIfError(SendReceive(EMMFDevSoundProxyPlayFixedSequence, iDestinationPckg, seqNum));
	iState = ETonePlaying;
	iToneMode = EFixedSequence;
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::PlayFixedSequenceL - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetDTMFLengths
// Set attributes for playing DTMF String.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::SetDTMFLengths(
									TTimeIntervalMicroSeconds32& aToneOnLength,
									TTimeIntervalMicroSeconds32& aToneOffLength,
									TTimeIntervalMicroSeconds32& aPauseLength )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetDTMFLengths - Enter"));
	TMMFDevSoundProxySettings set;
	set.iToneOnLength = aToneOnLength;
	set.iToneOffLength = aToneOffLength;
	set.iPauseLength = aPauseLength;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceive(EMMFDevSoundProxySetDTMFLengths, iDestinationPckg, pckg);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetDTMFLengths - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetVolumeRamp
// Sets the volume ramp duration.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::SetVolumeRamp(
								const TTimeIntervalMicroSeconds& aRampDuration)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetVolumeRamp - Enter"));
	TMMFDevSoundProxySettings set;
	set.iDuration = aRampDuration;
	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceive(EMMFDevSoundProxySetVolumeRamp, iDestinationPckg, pckg);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetVolumeRamp - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::GetSupportedInputDataTypesL
// Returns supported datatypes for playing audio.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::GetSupportedInputDataTypesL(
								RArray<TFourCC>& aSupportedDataTypes,
								const TMMFPrioritySettings& aPrioritySettings)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetSupportedInputDataTypesL - Enter"));
	aSupportedDataTypes.Reset();

	TMMFPrioritySettings prioritySet = aPrioritySettings;
	TMMFPrioritySettingsPckg pckg(prioritySet);

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(SendReceiveResult(
							EMMFDevSoundProxyGetSupportedInputDataTypes,
							iDestinationPckg,
							pckg,
							numberOfElementsPckg));
							
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetSupportedInputDataTypesL - Exit 1"));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TFourCC));
	TPtr8 ptr = buf->Des();


	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyCopyFourCCArrayData,
										iDestinationPckg,
										KNullDesC8,
										ptr));
										
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	
	TInt count = numberOfElementsPckg();
	for (TInt i = 0; i < count; i++)
		{
		TInt err = aSupportedDataTypes.Append(stream.ReadInt32L());
		if (err)
			{//note we don't destroy array because we don't own it
			//but we do reset it as it is incomplete
			aSupportedDataTypes.Reset();
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetSupportedInputDataTypesL - Exit 2"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::GetSupportedOutputDataTypesL
// Returns supported datatypes for recording audio.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::GetSupportedOutputDataTypesL(
								RArray<TFourCC>& aSupportedDataTypes,
								const TMMFPrioritySettings& aPrioritySettings)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetSupportedOutputDataTypesL - Enter"));
	aSupportedDataTypes.Reset();

	TMMFPrioritySettings prioritySet = aPrioritySettings;
	TMMFPrioritySettingsPckg pckg(prioritySet);

	TPckgBuf<TInt> numberOfElementsPckg;
	User::LeaveIfError(SendReceiveResult(
								EMMFDevSoundProxyGetSupportedOutputDataTypes,
								iDestinationPckg,
								pckg,
								numberOfElementsPckg));
								
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetSupportedOutputDataTypesL - Exit 1"));

	HBufC8* buf = HBufC8::NewLC(numberOfElementsPckg()*sizeof(TFourCC));
	TPtr8 ptr = buf->Des();


	User::LeaveIfError(SendReceiveResult(EMMFDevSoundProxyCopyFourCCArrayData,
										iDestinationPckg,
										KNullDesC8,
										ptr));
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	
	TInt count = numberOfElementsPckg();
	for (TInt i = 0; i < count; i++)
		{
		TInt err = aSupportedDataTypes.Append(stream.ReadInt32L());
		if (err)
			{//note we don't destroy array because we don't own it
			//but we do reset it as it is incomplete
			aSupportedDataTypes.Reset();
			User::Leave(err);
			}
		}
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buf);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetSupportedOutputDataTypesL - Exit 2"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SamplesRecorded
// Returns samples recorded so far.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::SamplesRecorded()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SamplesRecorded - Enter"));
	TPckgBuf<TInt> numSamples;
	SendReceiveResult(EMMFDevSoundProxySamplesRecorded,
					iDestinationPckg,
					KNullDesC8,
					numSamples);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SamplesRecorded - Exit"));
	return numSamples();
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SamplesPlayed
// Returns samples played so far.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::SamplesPlayed()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SamplesPlayed - Enter"));
	TPckgBuf<TInt> numSamples;
	SendReceiveResult(EMMFDevSoundProxySamplesPlayed,
					iDestinationPckg,
					KNullDesC8,
					numSamples);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SamplesPlayed - Exit"));
	return numSamples();
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetToneRepeats
// Sets tone repeats
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::SetToneRepeats(
								TInt aRepeatCount,
								const TTimeIntervalMicroSeconds&
									aRepeatTrailingSilence)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetToneRepeats - Enter"));
	TPckgBuf<TInt> countRepeat(aRepeatCount);
	TPckgBuf<TTimeIntervalMicroSeconds> repeatTS(aRepeatTrailingSilence);
	SendReceive(EMMFDevSoundProxySetToneRepeats,
				iDestinationPckg,
				countRepeat,
				repeatTS);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetToneRepeats - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetPrioritySettings
// Sets priority settings
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::SetPrioritySettings(
								const TMMFPrioritySettings& aPrioritySettings)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetPrioritySettings - Enter"));
	TPckgBuf<TMMFPrioritySettings> prioritySet(aPrioritySettings);
	SendReceive(EMMFDevSoundProxySetPrioritySettings,
				iDestinationPckg,
				prioritySet);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetPrioritySettings - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::FixedSequenceName
// Returns the name of fixed sequence for a given sequence number.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& RMMFDevSoundProxy::FixedSequenceName(TInt /*aSequenceNumber*/)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::FixedSequenceName - Enter"));
	_LIT(KNullDesC, "");
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::FixedSequenceName - Exit"));
	return KNullDesC;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::CustomInterface
// Returns a pointer to CustomInterface object.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TAny* RMMFDevSoundProxy::CustomInterface(TUid aInterfaceId)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomInterface - Enter"));
	TMMFDevSoundProxySettings set;
	set.iInterface = aInterfaceId;
	// Added for ask for custom interface
	TAny* customInterface = NULL;
	if (aInterfaceId == KMmfUidDevSoundCancelInitializeCustomInterface)
		{
		MMMFDevSoundCancelInitialize* result = this;
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomInterface - Exit"));
		return result;
		}
	TPckgBuf<TAny*> pckg2(customInterface);

	TMMFDevSoundProxySettingsPckg pckg(set);
	SendReceiveResult(EMMFDevSoundProxyCustomInterface, 
							iDestinationPckg, 
							pckg, pckg2);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomInterface - Exit"));
	return reinterpret_cast<TAny*> (customInterface);
	}
	
// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::FixedSequenceCount
// Returns the number of fixed sequences supported by DevSound.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::FixedSequenceCount()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::FixedSequenceCount - Enter"));
	TPckgBuf<TInt> fixSeqCountPckg;
	SendReceiveResult(EMMFDevSoundProxyFixedSequenceCount,
					iDestinationPckg,
					KNullDesC8,
					fixSeqCountPckg);

	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::FixedSequenceCount - Exit"));
	return fixSeqCountPckg();
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::BufferToBeFilledData
// Returns data buffer for playback.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::BufferToBeFilledData(
		TBool aRequestChunk, TMMFDevSoundProxyHwBufPckg& aSetPckg)
	{
	//SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::BufferToBeFilledData - Enter")); //Uncommenting this will produce a lot of logging!
	// Note that there will only ever be one of these requests outstanding
	// per session
	TPckgBuf<TInt> requestChunkBuf (aRequestChunk);
	TInt err = SendReceiveResult(EMMFDevSoundProxyBTBFData,
							iDestinationPckg,
							requestChunkBuf,
							aSetPckg);
	//SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::BufferToBeFilledData - Exit [%d]"), err); //Uncommenting this will produce a lot of logging!
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::BufferToBeEmptiedData
// Returns data buffer for recording.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::BufferToBeEmptiedData(
							TMMFDevSoundProxyHwBufPckg& aSetPckg)
	{
	//SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::BufferToBeEmptiedData - Enter")); //Uncommenting this will produce a lot of logging!
	// Note that there will only ever be one of these requests outstanding
	// per session
	TInt err = SendReceiveResult(EMMFDevSoundProxyBTBEData,
								iDestinationPckg,
								KNullDesC8,
								aSetPckg);
	//SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::BufferToBeEmptiedData - Exit [%d]"), err); //Uncommenting this will produce a lot of logging!
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::RegisterAsClient
// Registers the client for notification of resource avalibility.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::RegisterAsClient - Enter"));
	TMMFDevSoundProxySettings set;
	set.iNotificationEventUid = aEventType;
	TMMFDevSoundProxySettingsPckg pckg(set);
	TInt err = SendReceive(EMMFDevSoundProxyRequestResourceNotification, iDestinationPckg, pckg, aNotificationRegistrationData);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::RegisterAsClient - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::CancelRegisterAsClient
// Cancels the Registered Notification.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::CancelRegisterAsClient(TUid aEventType)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CancelRegisterAsClient - Enter"));
	TMMFDevSoundProxySettings set;
	set.iNotificationEventUid = aEventType;
	TMMFDevSoundProxySettingsPckg pckg(set);
	TInt err = SendReceiveResult(EMMFDevSoundProxyCancelRequestResourceNotification, iDestinationPckg, KNullDesC8, pckg);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::CancelRegisterAsClient - Exit [%d]"), err);
	return err;
	}
	
// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::GetResourceNotificationData
// Returns the Notification data which the client needs to resume playing.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::GetResourceNotificationData(TUid aEventType, TDes8& aNotificationData)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetResourceNotificationData - Enter"));
	TMMFDevSoundProxySettings set;
	set.iNotificationEventUid = aEventType;
	TMMFDevSoundProxySettingsPckg pckg(set);
	TInt err = SendReceiveResult(EMMFDevSoundProxyGetResourceNotificationData, iDestinationPckg, pckg, aNotificationData);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::GetResourceNotificationData - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::WillResumePlay
// Wait for the clients to resume play back even after the default timeout 
// expires. Unless the client cancels the notification request or completes 
// no other client gets notification.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::WillResumePlay()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::WillResumePlay - Enter"));
	TInt err = SendReceive(EMMFDevSoundProxyWillResumePlay, iDestinationPckg);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::WillResumePlay - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::EmptyBuffers
// Empties the play buffers below DevSound without causing the codec 
// to be deleted.
// -----------------------------------------------------------------------------
//

EXPORT_C TInt RMMFDevSoundProxy::EmptyBuffers()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::EmptyBuffers - Enter"));
	TInt error = SendReceive(EMMFDevSoundProxyEmptyBuffers, iDestinationPckg); 
	
	if(error==KErrNone)
	    {
	    if (iState==EPlayingBufferWait)
	        {
	        // Empty buffers terminates the buffer cycle
	        iState = EPlaying;
	        }
	    }
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::EmptyBuffers - Exit [%d]"), error);
	return error;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::CancelInitialize
// Cancels the initialization process
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::CancelInitialize()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CancelInitialize - Enter"));
	TInt err=KErrNone;
	
	if (iState==EInitializing)
		{
		err = SendReceive(EMMFDevSoundProxyCancelInitialize, iDestinationPckg);
		iState = EIdle;
		}
	else
		{
		err = KErrNotReady;
		}
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::CancelInitialize - Exit [%d]"), err);
	return err;
	}


// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::SetClientThreadInfo
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::SetClientThreadInfo(TThreadId& aTid)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SetClientThreadInfo - Enter"));
	TPckgBuf<TThreadId> threadId(aTid);
	TInt err = SendReceive(EMMFDevSoundProxySetClientThreadInfo, iDestinationPckg, threadId);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::SetClientThreadInfo - Exit [%d]"), err);
	return err;
	}


// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::CustomCommandSync()
// Send Message synchronously to DevSound server and return the result to client
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::CustomCommandSync(
							const TMMFMessageDestinationPckg&  aDestination,
							TInt aFunction,
							const TDesC8& aDataTo1,
							const TDesC8& aDataTo2,
							TDes8& aDataFrom)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomCommandSync - Enter"));
	TInt err = SendReceiveResult(aFunction,
							aDestination,
							aDataTo1,
							aDataTo2,
							aDataFrom);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::CustomCommandSync - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::RMMFDevSoundProxy()
// Send Message synchronously to DevSound server and return the result to client
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RMMFDevSoundProxy::CustomCommandSync(
							const TMMFMessageDestinationPckg&  aDestination,
							TInt aFunction,
							const TDesC8& aDataTo1,
							const TDesC8& aDataTo2)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomCommandSync - Enter"));
	TInt err =  SendReceive(aFunction, aDestination, aDataTo1, aDataTo2);
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::CustomCommandSync - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::RMMFDevSoundProxy()
// Send Message asynchronously to DevSound server
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::CustomCommandAsync(
								const TMMFMessageDestinationPckg& aDestination,
								TInt aFunction,
								const TDesC8& aDataTo1,
								const TDesC8& aDataTo2,
								TDes8& aDataFrom,
								TRequestStatus& aStatus )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomCommandAsync - Enter"));
	SendReceiveResult(aFunction,
					aDestination,
					aDataTo1,
					aDataTo2,
					aDataFrom,
					aStatus);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomCommandAsync - Exit"));
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::RMMFDevSoundProxy()
// Send Message asynchronously to DevSound server
// -----------------------------------------------------------------------------
//
EXPORT_C void RMMFDevSoundProxy::CustomCommandAsync(
								const TMMFMessageDestinationPckg& aDestination,
								TInt aFunction,
								const TDesC8& aDataTo1,
								const TDesC8& aDataTo2,
								TRequestStatus& aStatus )
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomCommandAsync - Enter"));
	SendReceive(aFunction, aDestination, aDataTo1, aDataTo2, aStatus);
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::CustomCommandAsync - Exit"));
	}

// implementation of a simple CustomCommand() scheme
EXPORT_C TInt RMMFDevSoundProxy::SyncCustomCommand(TUid aUid, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SyncCustomCommand - Enter"));
	TMMFMessageDestinationPckg dest(TMMFMessageDestination(aUid, KMMFObjectHandleDevSound));

	TInt err = KErrNone;
	if (aOutParam==NULL)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SyncCustomCommand - Exit"));
		err = SendReceive(EMMFDevSoundProxySyncCustomCommand, dest, aParam1, aParam2);
		return err;
		}
	else
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::SyncCustomCommand - Exit"));
		err = SendReceiveResult(EMMFDevSoundProxySyncCustomCommandResult, dest, aParam1, aParam2, *aOutParam);
		return err;
		}
	}

EXPORT_C void RMMFDevSoundProxy::AsyncCustomCommand(TUid aUid, TRequestStatus& aStatus, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::AsyncCustomCommand - Enter"));
	TMMFMessageDestination dest(aUid, KMMFObjectHandleDevSound);
	iCustIntPckg = dest;
	if (aOutParam==NULL)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::AsyncCustomCommand - Exit"));
		SendReceive(EMMFDevSoundProxyAsyncCustomCommand, iCustIntPckg, aParam1, aParam2, aStatus);
		}
	else
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::AsyncCustomCommand - Exit"));
		SendReceiveResult(EMMFDevSoundProxyAsyncCustomCommandResult, iCustIntPckg, aParam1, aParam2, *aOutParam, aStatus);		
		}
	}

EXPORT_C TInt RMMFDevSoundProxy::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::GetTimePlayed - Enter"));
	TTimeIntervalMicroSeconds time(0);
	TPckgBuf<TTimeIntervalMicroSeconds> timePckg(time);
	TInt err = SendReceiveResult(EMMFDevSoundProxyGetTimePlayed, iDestinationPckg, KNullDesC8, timePckg);
	if(err==KErrNone)
		{
		aTime = timePckg();
		}
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::GetTimePlayed - Exit [%d]"), err);
	return err;
	}

EXPORT_C TBool RMMFDevSoundProxy::IsResumeSupported()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::IsResumeSupported - Enter"));
	TPckgBuf<TBool> isResumeSupported;
	TInt err = SendReceiveResult(EMMFDevSoundProxyIsResumeSupported, 
								iDestinationPckg, 
								KNullDesC8, 
								isResumeSupported);
	if(err == KErrNone)
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::IsResumeSupported - Exit"));
		return isResumeSupported();
		}
	else
		{
		SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::IsResumeSupported - Exit"));
		return EFalse;
		}
	}

EXPORT_C TInt RMMFDevSoundProxy::Resume()
	{
	SYMBIAN_DEBPRN0(_L("RMMFDevSoundProxy[0x%x]::Resume - Enter"));
	TInt err = KErrNone;
	if (!iDevSoundObserver ||  iState <= EInitialized  )
		{
		err = KErrNotReady;
		}
	else if(iState == ETonePlaying && iToneMode != ESequence)
		{
		SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::Resume - Exit [%d]"), err);
		return KErrNotSupported;
		}
	else
		{
	    if (iState==ERecordingInLastBufferCycle)
            {
            // if we're in a last buffer cycle and get Resume() we have to be careful as the
            // server side sent a PausedRecordCompleteEvent and did not actually request a buffer!
            // just record the fact we've done this and wait until RecordData() is called
            // don't actually resume until then!
            iState = ERecordingResumingInLastBufferCycle;
            }
	    else if (iState == ERecordingResumingInLastBufferCycle)
	        {
            //Do Nothing as you can't resume twice on last buffer 
	        }
	    else
	        {
	        err = SendReceive(EMMFDevSoundProxyResume,  
	                    iDestinationPckg);	        
	        }
		}
	SYMBIAN_DEBPRN1(_L("RMMFDevSoundProxy[0x%x]::Resume - Exit [%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RMMFDevSoundProxy::StartReceivingMsgQueueHandlerEventsL()
// Starts message queue handler (A/O) to monitor client side events
// -----------------------------------------------------------------------------
//
void RMMFDevSoundProxy::StartReceivingMsgQueueHandlerEventsL(MMMFDevSoundCustomInterfaceObserver& aDevSoundCIObserver)
	{
	if (iMsgQueueHandler)
		{
		iMsgQueueHandler->Cancel();
		}
	else
		{
		iMsgQueueHandler = CMsgQueueHandler::NewL(this,
												*this,
												&iMsgQueue,
												aDevSoundCIObserver);
		}

	iMsgQueueHandler->ReceiveEvents();
	}
	
// MDevSoundObserver 
// intercept the calls from the msgQueueHandler going back to the client, so we can track real state

void RMMFDevSoundProxy::InitializeComplete(TInt aError)
	{
	SYMBIAN_CHECK(iState==EInitializing, Panic(EMMFDevSoundProxyInitCompleteInWrongState));
	if (aError==KErrNone)
		{
		iState = EInitialized;
		}
	else 
		{
		iState = EIdle;
		}
	iDevSoundObserver->InitializeComplete(aError);
	}
	
void RMMFDevSoundProxy::ToneFinished(TInt aError)
	{
	SYMBIAN_CHECK(iState==ETonePlaying, Panic(EMMFDevSoundProxyToneFinishedInWrongState));
	iState = EInitialized;
	iDevSoundObserver->ToneFinished(aError);
	}
	
void RMMFDevSoundProxy::PlayError(TInt aError)
	{
	SYMBIAN_CHECK(iState==EPlaying||iState==EPlayingBufferWait, Panic(EMMFDevSoundProxyPlayErrorInWrongState)); 
	iState = EInitialized;
	iDevSoundObserver->PlayError(aError);
	}
	
void RMMFDevSoundProxy::RecordError(TInt aError)
	{
	SYMBIAN_CHECK(iState==ERecording||iState==ERecordingBufferWait, Panic(EMMFDevSoundProxyRecordErrorInWrongState)); 
	iState = EInitialized;
	iDevSoundObserver->RecordError(aError);
	}
	
void RMMFDevSoundProxy::BufferToBeFilled(CMMFBuffer* aBuffer)
	{
	SYMBIAN_CHECK(iState==EPlaying, Panic(EMMFDevSoundProxyBTBFInWrongState));
	iState = EPlayingBufferWait;
	iBuffer = static_cast<CMMFDataBuffer*>(aBuffer); // cache buffer for use in PlayData() later
	iDevSoundObserver->BufferToBeFilled(aBuffer);
	}
	
void RMMFDevSoundProxy::BufferToBeEmptied(CMMFBuffer* aBuffer)
	{
	SYMBIAN_CHECK(iState==ERecording, Panic(EMMFDevSoundProxyBTBEInWrongState));
	if (aBuffer->LastBuffer())
	    {
	    // this is end of recording. Assume have an empty buffer. Different state so that Resume() is handled as special case.
	    iState = ERecordingInLastBufferCycle;
	    }
	else
	    {
	    iState = ERecordingBufferWait;	    
	    }
	iDevSoundObserver->BufferToBeEmptied(aBuffer);	
	}
	
void RMMFDevSoundProxy::ConvertError(TInt /*aError*/)
	{
	SYMBIAN_CHECK(EFalse, Panic(EMMFDevSoundProxyUnexpectedConvError));
	}
	
void RMMFDevSoundProxy::DeviceMessage(TUid aMessageType, const TDesC8& aMsg)
	{
	iDevSoundObserver->DeviceMessage(aMessageType, aMsg);
	}
	
void RMMFDevSoundProxy::SendEventToClient(const TMMFEvent& aEvent)
	{
	iDevSoundObserver->SendEventToClient(aEvent);
	}


//  End of File
