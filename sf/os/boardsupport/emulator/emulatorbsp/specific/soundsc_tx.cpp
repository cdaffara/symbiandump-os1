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
// wins\specific\soundsc_tx.cpp
// Emulator playback functions for the shared chunk sound driver PDD.
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#include "winssoundsc.h"
	
_LIT(KSoundScPddName,"SoundSc.Wins");
_LIT(KTxSoundDriverThreadName,"SoundDriverPlayThread");
const TInt KSoundDriverThreadPriority=26;		// One less than DFC thread 0

GLDEF_C TInt RateInSamplesPerSecond(TSoundRate aRate)
	{
	switch(aRate)
		{
		case ESoundRate7350Hz: 	return(7350);
		case ESoundRate8000Hz: 	return(8000);
		case ESoundRate8820Hz: 	return(8820);
		case ESoundRate9600Hz: 	return(9600);
		case ESoundRate11025Hz: return(11025);
		case ESoundRate12000Hz: return(12000);
		case ESoundRate14700Hz:	return(14700);
		case ESoundRate16000Hz: return(16000);
		case ESoundRate22050Hz: return(22050);
		case ESoundRate24000Hz: return(24000);
		case ESoundRate29400Hz: return(29400);
		case ESoundRate32000Hz: return(32000);
		case ESoundRate44100Hz: return(44100);
		case ESoundRate48000Hz: return(48000);
		default: return(0);
		};
	}

// This utility function is used instead of WaitForSingleObject() for places
// where the API call is made from either the driver thread or the play 
// thread.If the call is made from the driver thread, the thread is removed from 
// the kernel for the duration of the WaitForSingleObject() call.
GLDEF_C DWORD WaitForSingleObjectDualThread(HANDLE hHandle,DWORD dwMilliseconds)
	{
	TBool epocThread = (NKern::CurrentContext() == NKern::EInterrupt)?EFalse:ETrue;
	if (epocThread)
		Emulator::Escape();
	DWORD dwRet = WaitForSingleObject(hHandle, dwMilliseconds);
	if (epocThread)
		Emulator::Reenter();
	return dwRet;
	}
	
/**
Remove an audio buffer from the head of the specified buffer list.
Each list holds buffers which are waiting to be transferred. These lists are only used when no audio hardware is present.
@param aList The pending buffer list from which the buffer should be removed (either record or playback).
@return A pointer to the audio buffer reoved or NULL if the list is empty.
*/
GLDEF_C WAVEHDR* RemoveFromPendingList(WAVEHDR** aList)
	{
	WAVEHDR* buffer;
	
	buffer=aList[0];
	if (buffer)
		{
		// Move any remaining up one in the list.
		WAVEHDR* b;
		do
			{
			b=aList[1];
			*aList++=b;
			}
		while(b);
		}
	return(buffer);
	}

/**
Add an audio buffer to the tail of the the specified buffer list.
Each list holds buffers which are waiting to be transferred. These lists are only used when no audio hardware is present.
@param aBuffer The audio buffer to be added.
@param aList The pending buffer list into which the buffer should be added (either record or playback).
*/
GLDEF_C void AddToPendingList(WAVEHDR* aBuffer,WAVEHDR** aList)
	{
	while (*aList)
		aList++;
	*aList=aBuffer;
	}

/**
The thread function for the play windows thread.
This function is always executed in windows thread context.
*/
LOCAL_C TUint PlayThreadFunction(DWinsSoundScTxPdd *aSoundPdd)
	{
	aSoundPdd->PlayThread();
	return 0;
	}
	
/**
The waveform output callback function. This can receive the following messages:-
WOM_OPEN when the output device is opened, WOM_CLOSE when the output device is closed,
and WOM_DONE each time a data block play transfer is completed (i.e. completion of waveOutWrite).
This function is always executed in windows thread context.
*/
LOCAL_C void CALLBACK WaveOutProc(HWAVEOUT /*hwo*/, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD /*dwParam2*/)
	{
	if (uMsg == WOM_DONE)
		{
		DWinsSoundScTxPdd* pdd = (DWinsSoundScTxPdd*)dwInstance;
		pdd->WaveOutProc((WAVEHDR*)dwParam1);
		}
	}
		
THostLock::THostLock() : iLocked(EFalse)
	{
	Lock();
	}

THostLock::THostLock(TBool /*aLock*/) : iLocked(EFalse)
	{
	}

THostLock::~THostLock()
	{
	if (iLocked)
		Unlock();
	}

void THostLock::Lock()
	{
	__ASSERT_DEBUG(!iLocked, PANIC());
	Emulator::Lock();
	iLocked = ETrue;
	}

void THostLock::Unlock()
	{
	__ASSERT_DEBUG(iLocked, PANIC());
	Emulator::Unlock();
	iLocked = EFalse;
	}

TCondHostLock::TCondHostLock() : THostLock(EFalse)
	{
	iEpocThread = (NKern::CurrentContext() == NKern::EInterrupt)?EFalse:ETrue;
	Lock();
	}

void TCondHostLock::Lock()
	{
	if (iEpocThread)
		THostLock::Lock();	
	}

void TCondHostLock::Unlock()
	{
	if (iEpocThread)
		THostLock::Unlock();	
	}
	
/**
Standard export function for PDDs. This creates a DPhysicalDevice derived object,
in this case - DWinsSoundScPddFactory.
*/
DECLARE_STANDARD_PDD()
	{
	return new DWinsSoundScPddFactory;
	}
	
/**
Constructor for the sound PDD factory class.
*/
DWinsSoundScPddFactory::DWinsSoundScPddFactory()
	{	
	__KTRACE_SND(Kern::Printf(">DWinsSoundScPddFactory::DWinsSoundScPddFactory"));

//	iDfcQ=NULL;
	
	// Support units KSoundScTxUnit0 & KSoundScRxUnit0.
    iUnitsMask=(1<<KSoundScRxUnit0)|(1<<KSoundScTxUnit0);

    // Set version number for this device.
	iVersion=RSoundSc::VersionRequired();
	}

/**
Destructor for the sound PDD factory class.
*/
DWinsSoundScPddFactory::~DWinsSoundScPddFactory()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScPddFactory::~DWinsSoundScPddFactory"));
	
	// Destroy the kernel thread.
	if (iDfcQ)
		iDfcQ->Destroy();
	}
	
/**
Second stage constructor for the sound PDD factory class.
This must at least set a name for the driver object.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScPddFactory::Install()
	{
	TInt r=KErrNone;
	if (iDfcQ==NULL)
		{
		// Create a new sound driver DFC queue (and associated kernel thread). 
		r=Kern::DynamicDfcQCreate(iDfcQ,KSoundDriverThreadPriority,KTxSoundDriverThreadName);
		}
	
	if (r==KErrNone)
		{
		r=SetName(&KSoundScPddName); 			// Set the name of the driver object
		}
	
	__KTRACE_SND(Kern::Printf("<DWinsSoundScPddFactory::Install - %d",r));
	return(r);
	}
	
/**
Returns the PDD's capabilities. This is not used by the Symbian OS device driver framework
or by the LDD.
@param aDes A descriptor to write capabilities information into.
*/
void DWinsSoundScPddFactory::GetCaps(TDes8& /*aDes*/) const
	{}

/**
Called by the kernel's device driver framework to check if this PDD is suitable for use
with a logical channel.
This is called in the context of the client thread which requested the creation of a logical
channel - through a call to RBusLogicalChannel::DoCreate().
The thread is in a critical section.
@param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate().
@param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate() - not used.
@param aVer The version number of the logical channel which will use this physical channel. 
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScPddFactory::Validate(TInt aUnit, const TDesC8* /*aInfo*/, const TVersion& aVer)
	{
	// Check that the version specified is compatible.
	if (!Kern::QueryVersionSupported(RSoundSc::VersionRequired(),aVer))
		return(KErrNotSupported);
	
	// Check the unit number is compatible
	if (aUnit!=KSoundScTxUnit0 && aUnit!=KSoundScRxUnit0)
		return(KErrNotSupported);
		
	return(KErrNone);
	}

/**
Called by the kernel's device driver framework to create a physical channel object.
This is called in the context of the client thread which requested the creation of a logical
channel - through a call to RBusLogicalChannel::DoCreate().
The thread is in a critical section.
@param aChannel Set by this function to point to the created physical channel object.
@param aUnit The unit argument supplied by the client to RBusLogicalChannel::DoCreate().
@param aInfo The info argument supplied by the client to RBusLogicalChannel::DoCreate().
@param aVer The version number of the logical channel which will use this physical channel. 
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScPddFactory::Create(DBase*& aChannel, TInt aUnit, const TDesC8* /*anInfo*/, const TVersion& /*aVer*/)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScPddFactory::Create"));

	// Create the appropriate PDD channel object.
	TInt r=KErrNoMemory;
	if (aUnit==KSoundScRxUnit0)
		{
		DWinsSoundScRxPdd* pD=new DWinsSoundScRxPdd;
		aChannel=pD;
		if (pD)
			r=pD->DoCreate(this);
		}
		
	else
		{
		DWinsSoundScTxPdd* pD=new DWinsSoundScTxPdd;
		aChannel=pD;
		if (pD)
			r=pD->DoCreate(this);
		}
	return(r);
	}
	
/**
Constructor for the WINS shared chunk playback PDD.
This function is always executed in driver thread context.
*/
DWinsSoundScTxPdd::DWinsSoundScTxPdd()
	: iDfc(DWinsSoundScTxPdd::PlayDfc,this,2)
	{		
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::DWinsSoundScTxPdd"));
	
//	iVolume=0;
//	iDriverThreadSem=0;
//	iPlayThread=0;
//	iPlayThreadMutex=0;
//	iPlayThreadSem=0;
//	iStopSemaphore=0;
//	iDeathSemaphore=0;
//	iPlayCommand=ESendData;
//	iPlayCommandArg0=0;
//	iPlayCommandArg1=0;
//	iPendingPlay=0;
//	iPlayThreadError=0;
//	iWaveformBufMgr=NULL;
//	iCompletedPlayBufHdrMask=0;	
//	iPlayBufferSize=0;
//	iNoHardware=EFalse;
//	iPlayTimerEvent=0;
//	iTimerID=0;
//	iTimerActive=EFalse;
//	iWinWaveVolume=0;
	}
	
/**
Destructor for the WINS shared chunk playback PDD.
This function is always executed in driver thread context.
*/
DWinsSoundScTxPdd::~DWinsSoundScTxPdd()
	{
	// If the Windows thread started up successfully, signal it to shut down and wait for it to do so
	if (iPlayThreadRunning)
		{
		// Signal the windows thread to close down the play device and exit the windows thread.
		iDeathSemaphore = CreateSemaphore(NULL, 0, 2, NULL);
		PlayThreadCommand(EExit);

		// Wait for the play thread to terminate.
		if (iDeathSemaphore)
			{
			Emulator::Escape();
			WaitForSingleObject(iDeathSemaphore, INFINITE); 
			Emulator::Reenter();

			__HOST_LOCK;
			CloseHandle(iDeathSemaphore);
			}
		}

	if (iPlayTimerEvent)
		CloseHandle(iPlayTimerEvent); 
	if (iPlayThreadSem)
		CloseHandle(iPlayThreadSem); 
	if (iPlayThread)
		CloseHandle(iPlayThread);
	if (iDriverThreadSem)
		CloseHandle(iDriverThreadSem); 	
	
	if (iWaveformBufMgr)
		delete iWaveformBufMgr;
	}
	
/**
Second stage constructor for the WINS shared chunk playback PDD.
Note that this constructor is called before the second stage constructor for the LDD so it is not
possible to call methods on the LDD here.
This function is always executed in driver thread context.
@param aPhysicalDevice A pointer to the factory class that is creating this PDD.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/	
TInt DWinsSoundScTxPdd::DoCreate(DWinsSoundScPddFactory* aPhysicalDevice)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::DoCreate"));

	iPhysicalDevice=aPhysicalDevice;
	
	// Set up the correct DFC queue.
	iDfc.SetDfcQ(iPhysicalDevice->iDfcQ);
	
	SetCaps();	// Setup the capabilities of this device.
	
	// Setup the default audio configuration
	iSoundConfig.iChannels=2;
	iSoundConfig.iRate=ESoundRate48000Hz;
	iSoundConfig.iEncoding=ESoundEncoding16BitPCM;
	iSoundConfig.iDataFormat=ESoundDataFormatInterleaved;
	
	__HOST_LOCK;

	// Query the waveform device capabilities using the default device identifier in order
	// to check if there is a functioning waveform device present.  Note that some versions of
	// Windows (such as Windows Server 2003) will actually return MMSYSERR_NOERROR when this is
	// called, even if there is no waveform device present, so we have a further check in
	// when waveOutOpen() is called
	WAVEOUTCAPS waveOutCaps;
	MMRESULT res = waveOutGetDevCaps(WAVE_MAPPER,&waveOutCaps,sizeof(WAVEOUTCAPS));
#ifdef FORCE_NO_HARDWARE
	res=MMSYSERR_NOERROR+1;
#endif
	if (res != MMSYSERR_NOERROR)
		iNoHardware = ETrue;
	
	__HOST_LOCK_OFF;
	
	// Create the windows waveform audio buffer manager.
	iWaveformBufMgr=new TWaveformBufMgr(ESoundDirPlayback,!iNoHardware);
	if (!iWaveformBufMgr)
		return(KErrNoMemory);
	
	// Create the driver thread semaphore.
	iDriverThreadSem = CreateSemaphore(NULL,0,0x7fffffff,NULL);
	if (!iDriverThreadSem)
		return(KErrNoMemory);
	
	// Create the play windows thread.
	if ((iPlayThread=CreateWin32Thread(EThreadEvent,(LPTHREAD_START_ROUTINE)PlayThreadFunction,(void *)this, FALSE))==NULL)
		return(KErrNoMemory);
	SetThreadPriority(iPlayThread,THREAD_PRIORITY_HIGHEST);
	__ASSERT_ALWAYS(ResumeThread(iPlayThread) != 0xffffffff, PANIC()); // Windows Unexpected Error

	// Wait to be notified of successful thread initialization
	Emulator::Escape();
	WaitForSingleObject(iDriverThreadSem,INFINITE);
	Emulator::Reenter();

	// If the Windows thread started up successfully, indicate this fact so that when shutting down we know
	// to signal to the thread to exit
	if (iPlayThreadError == KErrNone)
		iPlayThreadRunning = ETrue;

	return(iPlayThreadError);
	}

/**
Called from the LDD to return the DFC queue to be used by this device.
This function is always executed in driver thread context.
@return The DFC queue to use.
*/	
TDfcQue* DWinsSoundScTxPdd::DfcQ(TInt /*aUnit*/)
	{
	return(iPhysicalDevice->iDfcQ);
	}

/** 
Called from the LDD to return the shared chunk create information to be used by this device.
This function is always executed in driver thread context.
@param aChunkCreateInfo A chunk create info. object to be to be filled with the settings
						required for this device.
*/		
void DWinsSoundScTxPdd::GetChunkCreateInfo(TChunkCreateInfo& aChunkCreateInfo)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::GetChunkCreateInfo"));

	aChunkCreateInfo.iType=TChunkCreateInfo::ESharedKernelMultiple;
	aChunkCreateInfo.iMapAttr=0;
	aChunkCreateInfo.iOwnsMemory=ETrue; 				// Using RAM pages.
	aChunkCreateInfo.iDestroyedDfc=NULL; 				// No chunk destroy DFC.
	}
	
/**
Called from the LDD to return the capabilities of this device.
This function is always executed in driver thread context.
@param aCapsBuf A packaged TSoundFormatsSupportedV02 object to be filled with the play
				capabilities of this device. This descriptor is in kernel memory and can be accessed directly.
@see TSoundFormatsSupportedV02.
*/
void DWinsSoundScTxPdd::Caps(TDes8& aCapsBuf) const
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::Caps"));
	
	// Copy iCaps back.
	TPtrC8 ptr((const TUint8*)&iCaps,sizeof(iCaps));
	aCapsBuf.FillZ(aCapsBuf.MaxLength());
	aCapsBuf=ptr.Left(Min(ptr.Length(),aCapsBuf.MaxLength()));	
	}
	
/**
Called from the LDD to return the maximum transfer length in bytes that this device can support in a single data transfer.
@return The maximum transfer length in bytes.
*/
TInt DWinsSoundScTxPdd::MaxTransferLen() const
	{
	return(KWinsMaxAudioTransferLen);		// 32K
	}		
	
/**
Called from the LDD to power up the sound device.
This function is always executed in driver thread context.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScTxPdd::PowerUp()
	{
	return(KErrNone);
	}
	
/**
Called from the LDD to configure or reconfigure the device using the the configuration supplied.
This function is always executed in driver thread context.
@param aConfigBuf A packaged TCurrentSoundFormatV02 object which contains the new configuration settings.
				  This descriptor is in kernel memory and can be accessed directly.
@return KErrNone if successful, otherwise one of the other system wide error codes.
@see TCurrentSoundFormatV02.
*/	
TInt DWinsSoundScTxPdd::SetConfig(const TDesC8& aConfigBuf)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::SetConfig"));

	// Cannot change the configuration while the device is open and playing. (LDD should prevent
	// this anyway but better safe than sorry).
	if (iPlayDeviceHandle)
		return(KErrInUse);
	
	// Save the current settings so we can restore them if there is a problem with the new ones.
	TCurrentSoundFormatV02 saved=iSoundConfig;
	
	// Read the new configuration from the LDD.
	TPtr8 ptr((TUint8*)&iSoundConfig,sizeof(iSoundConfig));
	Kern::InfoCopy(ptr,aConfigBuf);
	
	// Open the play device with the new settings to check they are supported. Then close it
	// again - don't leave it open yet.
	TInt r = CreatePlayDevice(ESetConfig);
	if (r==KErrNone)
		ClosePlayDevice();
	else
		iSoundConfig=saved;	// Restore the previous settings
	
	return(r);
	}
	
const TInt KdBToLinear[KSoundMaxVolume+1] = 
	{
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,
	0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0202,0x0202,0x0202,0x0202,0x0202,0x0202,0x0202,0x0202,0x0202,0x0303,
	0x0303,0x0303,0x0303,0x0303,0x0303,0x0404,0x0404,0x0404,0x0404,0x0505,0x0505,0x0505,0x0505,0x0606,0x0606,0x0606,
	0x0707,0x0707,0x0808,0x0808,0x0909,0x0909,0x0a0a,0x0a0a,0x0b0b,0x0b0b,0x0c0c,0x0d0d,0x0e0e,0x0e0e,0x0f0f,0x1010,
	0x1111,0x1212,0x1313,0x1414,0x1616,0x1717,0x1818,0x1a1a,0x1b1b,0x1d1d,0x1e1e,0x2020,0x2222,0x2424,0x2626,0x2929,
	0x2b2b,0x2e2e,0x3030,0x3333,0x3636,0x3939,0x3d3d,0x4040,0x4444,0x4848,0x4c4c,0x5151,0x5656,0x5b5b,0x6060,0x6666,
	0x6c6c,0x7272,0x7979,0x8080,0x8888,0x9090,0x9898,0xa2a2,0xabab,0xb5b5,0xc0c0,0xcbcb,0xd7d7,0xe4e4,0xf2f2,0xffff,	
	};	
	
/**
Called from the LDD to set the play volume.
This function is always executed in driver thread context.
@param aVolume The play volume to be set - a value in the range 0 to 255. The value 255 equates
	to the maximum volume and each value below this equates to a 0.5dB step below it.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScTxPdd::SetVolume(TInt aVolume)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::SetVolume"));
	
	// The documentation for waveOutSetVolume() says: "Volume settings are interpreted logarithmically" but evidence suggests these are really
	// interpreted linearly. Hence, use a lookup table to convert from dB to linear. At the same time convert from an 8-bit wide range to a 
	// 16-bit wide range.
	iVolume=KdBToLinear[aVolume];
	
	// Only update the volume on the output device itself if this is currently open.
	if (iPlayDeviceHandle)
		waveOutSetVolume(iPlayDeviceHandle, iVolume | (iVolume << 16));	
	return(KErrNone);
	}	
		
/**
Called from the LDD to prepare the audio device for playback.
This function is always executed in driver thread context.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScTxPdd::StartTransfer()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::StartTransfer"));
	
	// Convert the enum representing the current sample rate into an integer
	TInt samplesPerSecond=RateInSamplesPerSecond(iSoundConfig.iRate);
	if (samplesPerSecond==0)
		return(KErrNotSupported);

	// Now convert the sample rate into the number of bytes per second and save for later use
	iBytesPerSecond=samplesPerSecond;
	if (iSoundConfig.iChannels==2)
		iBytesPerSecond*=2;
	if (iSoundConfig.iEncoding==ESoundEncoding16BitPCM)
		iBytesPerSecond*=2;

	// Open the play device with the current settings.
	iPendingPlay=0;
	iCompletedPlayBufHdrMask=0;					// Reset the completion status mask
	TInt r = CreatePlayDevice(EStartTransfer);
	return(r);
	}
	
/**
Called from the LDD to initiate the playback of a portion of data to the audio device. 
When the transfer is complete, the PDD signals this event using the LDD function PlayCallback().
This function is always executed in driver thread context.
@param aTransferID A value assigned by the LDD to allow it to uniquely identify a particular transfer fragment.
@param aLinAddr The linear address within the shared chunk of the start of the data to be played.
@param aPhysAddr The physical address within the shared chunk of the start of the data to be played.
@param aNumBytes The number of bytes to be played. 
@return KErrNone if the transfer has been initiated successfully;
  		KErrNotReady if the device is unable to accept the transfer for the moment;
		otherwise one of the other system-wide error codes.
*/
TInt DWinsSoundScTxPdd::TransferData(TUint aTransferID,TLinAddr aLinAddr,TPhysAddr /*aPhysAddr*/,TInt aNumBytes)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::TransferData(ID:%xH)",aTransferID));
	
	// Check that we can accept the request
	if (aNumBytes>KWinsMaxAudioTransferLen)
		return(KErrArgument);
	if (iPendingPlay>=iWaveformBufMgr->iNumWaveformBufs)	// LDD may issue multiple data transfers per buffer.
		return(KErrNotReady);
	
	// Signal the windows thread to initiate the playback of a buffers worth of data to the waveout device.
	iPendingPlay++;
	PlayThreadCommand(ESendData,aTransferID,aLinAddr,aNumBytes);
	
	// Although the windows thread runs at a higher priority, its not safe to assume we will always get pre-empted at this
	// point while the the higher priority thread processes and completes the request. Instead we need to wait until it
	// signals back completion of the command.
	Emulator::Escape();
	WaitForSingleObject(iDriverThreadSem,INFINITE);
	Emulator::Reenter();	

	return(iPlayThreadError);
	}	
	
/**
Called from the LDD to terminate the playback of a data to the device and to release any resources necessary for playback.
This is called soon after the last pending play request from the client has been completed. Once this function had been
called, the LDD will not issue any further TransferData() commands without first issueing a StartTransfer() command.
This function is always executed in driver thread context.
*/	
void DWinsSoundScTxPdd::StopTransfer()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::StopTransfer"));
	
	// Signal the windows thread to stop and close the device.
	iStopSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	PlayThreadCommand(EStop);

	// Wait for the windows thread to stop and close the device.
	if (iStopSemaphore)
		{
		Emulator::Escape();
		WaitForSingleObject(iStopSemaphore, INFINITE);  
		Emulator::Reenter();

		__HOST_LOCK;
		CloseHandle(iStopSemaphore);
		iStopSemaphore = NULL;
		}
	iPendingPlay=0;
	iCompletedPlayBufHdrMask=0;					// Reset the completion status mask
	
	// Make sure the DFC is not queued.
	iDfc.Cancel();
	}
	
/**
Called from the LDD to halt the playback of data to the sound device but not to release any resources necessary for
playback.
If possible, any active transfer should be suspended in such a way that it can be resumed later - starting from next
sample following the one last played.
This function is always executed in driver thread context.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScTxPdd::PauseTransfer()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::PauseTransfer"));
	
	// Signal the windows thread to pause playback on the waveout device.
	PlayThreadCommand(EPause);
	    
	return(KErrNone);
	}
	
/**
Called from the LDD to resume the playback of data to the sound device following a request to halt playback.
If possible, any transfer which was active when the device was halted should be resumed - starting from next sample
following the one last played. Once complete, it should be reported using PlayCallback()
as normal. 
This function is always executed in driver thread context.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScTxPdd::ResumeTransfer()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::ResumeTransfer"));
	
	// Signal the windows thread to resume playback on the waveout device.
	PlayThreadCommand(EResume);
	
	return(KErrNone);
	}
			
/**
Called from the LDD to power down the sound device.
This function is always executed in driver thread context.
*/
void DWinsSoundScTxPdd::PowerDown()
	{}
	
/**
Called from the LDD to handle a custom configuration request.
@param aFunction A number identifying the request.
@param aParam A 32-bit value passed to the driver. Its meaning depends on the request.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScTxPdd::CustomConfig(TInt /*aFunction*/,TAny* /*aParam*/)
	{
	return(KErrNotSupported);
	}

/**
Called from the LDD to find out how many microseconds of data have been played.  This is called
in the context of the DFC thread.
@param aTimeTransferred	A reference to a variable into which to place the number of microseconds of audio.
@param aStatus			The current status of this channel
@return KErrNone if time is valid or KErrNotSupported.
*/
TInt DWinsSoundScTxPdd::TimeTransferred(TInt64& aTimePlayed, TInt /*aStatus*/)
	{
	TInt r=KErrGeneral;
	TInt64 ms=0;

	MMTIME time;
	time.wType=TIME_BYTES;

	if(iPlayDeviceHandle == 0)
		{
		// Have not started playback yet, or have stopped.
		aTimePlayed = 0;
		return KErrNone;
		}

	// If no hardware is present then we need to simulate the amount of time that has passed during
	// playback.  The # of microseconds can be found in the iSimulatedUSecPlayed member, but this is
	// only updated when the emulation timer triggers.  To improve the accuracy of the time returned,
	// we use the Windows system timer to determine the # of milliseconds that have passed since the
	// last time the timer triggered
	if(iNoHardware)
		{
		// Determine the # of milliseconds that have passed since the last timer triggered
		DWORD currentTime = timeGetTime();
		DWORD timeSinceLastEvent = (currentTime - iLastTimerEventTime);

		// If playback is paused then the Windows system timer will continue, so take this into
		// account and subtract the # of milliseconds we have been paused for
		if (iPauseTime)
			timeSinceLastEvent -= (currentTime - iPauseTime);

		// Clamp the resulting value to the duration of the timer, to prevent the millisecond count
		// going backwards if Windows is busy and latency becomes an issue
		if (timeSinceLastEvent > iSimulatedMsecDuration)
			timeSinceLastEvent = iSimulatedMsecDuration;

		// Now we know the value of the time passed down to a millisecond accuracy
		aTimePlayed = (iSimulatedUSecPlayed + (timeSinceLastEvent * 1000));
		return KErrNone;
		}
	
	// Get the number of bytes played by the Windows audio system
	if (waveOutGetPosition(iPlayDeviceHandle,&time,sizeof(time))==MMSYSERR_NOERROR)
		{
		// If requesting the number of bytes played is not supported, wType will be
		// changed to what was actually returned, so check for this and don't continue
		// if we got anything other than bytes
		if (time.wType==TIME_BYTES)
			{
			// It's all good.  Convert the number of bytes played into microseconds and return it
			ms=((time.u.cb/iBytesPerSecond)*1000);
			TUint remainder=(time.u.cb%iBytesPerSecond);
			ms+=((remainder*1000)/iBytesPerSecond);
			ms*=1000;
			aTimePlayed=ms;
			r=KErrNone;
			}
		}

	return(r);
	}

/** 
Constructor for the windows playback waveform audio buffer abstraction.
*/
TWaveformAudioBuf::TWaveformAudioBuf()
	{
	memclr(&iBufHdr,sizeof(WAVEHDR));
	iIsPrepared=EFalse;
	iIsInUse=EFalse;
	iWaveformBufMgr=NULL;
	iBufNum=0;
	}
		
/** 
Prepare the waveform audio buffer for playback or record.
@param aBufAddr A pointer to the address of the waveform buffer.
@param aBufLength The length in bytes of the waveform buffer.
@param aDeviceHandle The handle to the waveform audio device.
*/		
void TWaveformAudioBuf::Prepare(char* aBufAddr,TInt aBufLength,TInt aDeviceHandle)
	{
	iBufHdr.lpData = aBufAddr;
	iBufHdr.dwBufferLength = aBufLength;
	iBufHdr.dwBytesRecorded = 0;
    iBufHdr.dwUser = iBufNum;
    if (iWaveformBufMgr->iDirection==ESoundDirPlayback)
		iBufHdr.dwFlags = WHDR_DONE;					// Initialise all to done so we can check for underflow.
	else
		iBufHdr.dwFlags = 0;
    iBufHdr.dwLoops = 0;
    iBufHdr.lpNext = NULL;
	iBufHdr.reserved = 0;

	if (iWaveformBufMgr->iIsHardware)
		{
		if (iWaveformBufMgr->iDirection==ESoundDirPlayback)
			DoPrepareOut((HWAVEOUT)aDeviceHandle);
		else
			DoPrepareIn((HWAVEIN)aDeviceHandle);
		}
	iIsPrepared=ETrue;
	iIsInUse=EFalse;	
	}
	
/**
Cleanup the preparation performed when the waveform audio buffer was prepared for playback or record.
@param aDeviceHandle The handle to the waveform audio device.
*/	
void TWaveformAudioBuf::Unprepare(TInt aDeviceHandle)
	{
	if (iWaveformBufMgr->iIsHardware && iIsPrepared)
		{
		if (iWaveformBufMgr->iDirection==ESoundDirPlayback)
			DoUnprepareOut((HWAVEOUT)aDeviceHandle);
		else
			DoUnprepareIn((HWAVEIN)aDeviceHandle);
		}
	iIsPrepared=EFalse;	
	iIsInUse=EFalse;
	}	

/** 
Prepare the waveform audio buffer for playback.
@param aPlayDeviceHandle The handle to the waveform audio output device.
*/		
void TWaveformAudioBuf::DoPrepareOut(HWAVEOUT aPlayDeviceHandle)
	{
	MMRESULT res = waveOutPrepareHeader(aPlayDeviceHandle,&iBufHdr,sizeof(WAVEHDR));
	__KTRACE_SND(Kern::Printf("   waveOutPrepareHeader(BufNo:%d Pos:%x Len:%d)-%d",iBufNum,iBufHdr.lpData,iBufHdr.dwBufferLength,res));
	__ASSERT_ALWAYS(res==MMSYSERR_NOERROR,Kern::Fault("DWinsSoundScTxPddWOPH", res)); //WaveOutPrepareHeader error.
	}
	
/**
Cleanup the preparation performed when the waveform audio buffer was prepared for playback.
@param aPlayDeviceHandle The handle to the waveform audio output device.
*/	
void TWaveformAudioBuf::DoUnprepareOut(HWAVEOUT aPlayDeviceHandle)
	{
	MMRESULT res = waveOutUnprepareHeader(aPlayDeviceHandle,&iBufHdr,sizeof(WAVEHDR));
	__KTRACE_SND(Kern::Printf("   waveOutUnprepareHeader(BufNo:%d)-%d",iBufNum,res));
	__ASSERT_ALWAYS(res==MMSYSERR_NOERROR,Kern::Fault("DWinsSoundScTxPddWOUH",res)); //WaveOutUnprepareHeader error.	
	}

/**
Constructor for the waveform audio buffer manager. 
*/
TWaveformBufMgr::TWaveformBufMgr(TSoundDirection aDirection,TBool aIsHardware)
	: iDirection(aDirection), iIsHardware(aIsHardware)
	{
	iWaveformAudioBuf=NULL;
	iNumWaveformBufs=0;
	iWaveformBufSize=0;
	iPendingBufList=NULL;
	}
	
/**
Destructor for the waveform audio buffer manager.
*/	
TWaveformBufMgr::~TWaveformBufMgr()
	{
	if (iWaveformAudioBuf)
		delete[] iWaveformAudioBuf;
	if (iPendingBufList)
		delete iPendingBufList;
	}	

/*
Re-allocate the number of waveform audio buffers that are available for data transfer according to the
current shared chunk configuration. Then, for each buffer that exists within the shared chunk, prepare one of the waveform audio
buffers just created so that it is aligned with the shared chunk buffer. 
@param aBufConfig A buffer configuration object specifying the geometry of the current shared chunk buffer configuration.
@param aChunkBase The address in the kernel process for the start of the shared chunk.
@param aDeviceHandle The handle to the waveform audio device.
@return KErrNone if successful, otherwise one of the other system wide error codes. 
@pre The thread must be in a critical section.    
*/	
TInt TWaveformBufMgr::ReAllocAndUpdate(TSoundSharedChunkBufConfig* aBufConfig,TLinAddr aChunkBase,TInt aDeviceHandle)
	{
	__KTRACE_SND(Kern::Printf(">TWaveformBufMgr::ReAllocAndUpdate"));
	
	// Check if the number of windows waveform audio buffers that are required has changed.
	TInt required=Max(aBufConfig->iNumBuffers,KMinWaveHdrBufCount);
	if (iNumWaveformBufs != required)
		{
		// The number has changed. First, re-allocate the required number of windows waveform data blocks.
		if (iWaveformAudioBuf)
			{			
			delete[] iWaveformAudioBuf;
			iWaveformAudioBuf=NULL;
			}
			
		// If we are emulating an audio device then delete any pending buffer list previously created.	
		if (!iIsHardware && iPendingBufList)
			{
			delete iPendingBufList;
			iPendingBufList=NULL;
			}
		iNumWaveformBufs = 0;	
		
		iWaveformAudioBuf=new TWaveformAudioBuf[required];
		if (!iWaveformAudioBuf)
			return(KErrNoMemory);
		for (TInt i=0; i<required ; i++)
			{
			iWaveformAudioBuf[i].SetWaveformBufMgr(this);
			iWaveformAudioBuf[i].SetBufNum(i);
			}
			
		// If we are emulating an audio device then allocate a new pending buffer list.	
		if (!iIsHardware)
			{
			iPendingBufList=(WAVEHDR**)Kern::AllocZ((required+1)*sizeof(WAVEHDR*));
			if (!iPendingBufList)
				return(KErrNoMemory);
			}
		iNumWaveformBufs = required;		
		}
	
	// The most common situation is that request start offsets coincide with the start of one of the
	// shared chunk buffers. Hence, begin by preparing a windows waveform audio buffer for each shared chunk
	// buffer - aligned with this start address.				
	TInt* bufOffsetList=&aBufConfig->iBufferOffsetListStart;	// The buffer offset list.
	for (TInt i=0; i<aBufConfig->iNumBuffers ; i++) 
		{
		char* bufAddr=(char*)(aChunkBase+bufOffsetList[i]);
		iWaveformAudioBuf[i].Prepare(bufAddr,aBufConfig->iBufferSizeInBytes,aDeviceHandle); 
		}
	iWaveformBufSize=aBufConfig->iBufferSizeInBytes;		
	return(KErrNone);
	}

/**
Acquire an appropriate waveform audio buffer to be used either to send a data block to the waveform output device or
receive a data block from the waveform input device.
This function is always executed in windows thread context.
@param aStartAddress A pointer to the address of the data block to be played/recorded.
@param aBufLength The length in bytes of the data block to be played/recorded.
@param aDeviceHandle The handle to the waveform audio device.
@return A pointer to an appropriate waveform audio buffer to be used to transfer the data block.
*/
TWaveformAudioBuf* TWaveformBufMgr::AcquireBuf(char* aStartAddress,TInt aBufLength,TInt aDeviceHandle)
	{
	// See if there's a appropriate waveform audio buffer already prepared. We only need to worry about the start address,
	// the length can be adjusted later if necessary.
	TInt i;
	for (i=0; i<iNumWaveformBufs ; i++)
		{
		if (iWaveformAudioBuf[i].iIsPrepared && iWaveformAudioBuf[i].iBufHdr.lpData==aStartAddress && !iWaveformAudioBuf[i].iIsInUse)
			break;
		}
	if (i>=iNumWaveformBufs)
		{
		// None already prepared which are appropriate so prepare one now. See if there are any not yet prepared.
		for (i=0; i<iNumWaveformBufs ; i++)
			{
			if (!iWaveformAudioBuf[i].iIsPrepared)
				{
				iWaveformAudioBuf[i].Prepare(aStartAddress,aBufLength,aDeviceHandle);
				break;
				}
			}
			
		// All are prepared already so we need to re-prepare one specially.
		if (i>=iNumWaveformBufs)
			{
			for (i=0; i<iNumWaveformBufs ; i++)
				{
				if (!iWaveformAudioBuf[i].iIsInUse)
					{
					iWaveformAudioBuf[i].Unprepare(aDeviceHandle);
					iWaveformAudioBuf[i].Prepare(aStartAddress,aBufLength,aDeviceHandle);
					break;
					}
				}
			__ASSERT_ALWAYS(i>=0,PANIC());
			}
		}
		
	__KTRACE_SND(Kern::Printf("<TWaveformBufMgr:AcquireBuf - BufNo:%d",i));
	iWaveformAudioBuf[i].iIsInUse=ETrue;	
	return(&iWaveformAudioBuf[i]);	
	}
	
/**
The waveform output callback function to handle data block transfer completion.
This function is always executed in windows thread context.
@param aHdr A pointer to the header for the waveform audio buffer just transferred.
*/	
void DWinsSoundScTxPdd::WaveOutProc(WAVEHDR* aHdr)
	{
	TInt waveBufId=aHdr->dwUser;				// Work out which waveform audio buffer is completing.

	StartOfInterrupt();
	iCompletedPlayBufHdrMask|=(1<<waveBufId);	// Update the completion status mask
	iDfc.Add();									// Queue PlayDfc().
	EndOfInterrupt();
	}
	
/**
The DFC used to handle data block transfer completion.
This function is always executed in driver thread context.
@param aPtr A pointer to the physical channel object.
*/	
void DWinsSoundScTxPdd::PlayDfc(TAny* aPtr)
	{	
	TInt i;
	DWinsSoundScTxPdd& drv=*(DWinsSoundScTxPdd*)aPtr;
	
	// More than 1 transfer may have completed so loop until all completions are handled
	while (drv.iCompletedPlayBufHdrMask)
		{
		// Find the buffer ID of the next transfer that has completed
		for (i=0 ; i<32 && !(drv.iCompletedPlayBufHdrMask&(1<<i)) ; i++) {}
		__ASSERT_ALWAYS(i<drv.iWaveformBufMgr->iNumWaveformBufs,PANIC());
		__e32_atomic_and_ord32(&drv.iCompletedPlayBufHdrMask, ~(1u<<i)); // Clear this bit in the mask
		
		// Update the status of the waveform audio buffer which is completing
		TWaveformAudioBuf& buf=drv.iWaveformBufMgr->iWaveformAudioBuf[i];
		buf.iIsInUse=EFalse;
	
		// Callback the LDD passing the information for the transfer that has completed
		drv.iPendingPlay--;
		__KTRACE_SND(Kern::Printf("   Write complete(BufNo:%x Pos:%x Len:%d)",i,buf.iBufHdr.lpData,buf.iBufHdr.dwBufferLength));
		drv.Ldd()->PlayCallback(buf.iTransferID,KErrNone,buf.iBufHdr.dwBufferLength);
		}
	}
		
/**
Issue a request from the driver thread to the windows thread to execute a command.
@param aCommand The identifier of the command to be executed.
@param aArg0 A first command argument, its meaning depends on the command.
@param aArg1 A second command argument, its meaning depends on the command.
@param aArg2 A third command argument, its meaning depends on the command.
This function is always executed in driver thread context.
*/
void DWinsSoundScTxPdd::PlayThreadCommand(TThreadCommand aCommand,TInt aArg0,TInt aArg1,TInt aArg2)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd:PlayThreadCommand"));
	iPlayCommand = aCommand;
	iPlayCommandArg0 = aArg0;
	iPlayCommandArg1 = aArg1;
	iPlayCommandArg2 = aArg2;

	__HOST_LOCK;

	ReleaseSemaphore(iPlayThreadSem,1,NULL);
	}

/**
Pass a value from the windows thread to the driver thread.
This function is always executed in windows thread context.
@param aError The value to the passed to the driver thread.
*/
void DWinsSoundScTxPdd::PlayThreadNotifyDriver(TInt aError)
	{
	iPlayThreadError = aError;
	BOOL ret = ReleaseSemaphore(iDriverThreadSem,1,NULL);
	__ASSERT_ALWAYS(ret == TRUE, PANIC()); //Unexpected Windows Error
	}

#pragma warning(disable : 4702) // unreachable code
/**
Open the waveform output device for playback. Use a default device identifier in order to select a device
capable of meeting the current audio configuration. 
This function can be executed in either driver thread or windows thread context.
@pre The data member DWinsSoundScTxPdd::iSoundConfig must be setup with the current audio configuration.
*/
TInt DWinsSoundScTxPdd::OpenWaveOutDevice()
	{
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	TUint16 bitsPerSample = 8;

	switch (iSoundConfig.iEncoding)
		{
		case ESoundEncoding8BitPCM:
			break;
		case ESoundEncoding16BitPCM:
			bitsPerSample = 16;
			break;
		default:
			return KErrNotSupported;
		};

	TInt rateInSamplesPerSecond=RateInSamplesPerSecond(iSoundConfig.iRate);
	format.nChannels = TUint16(iSoundConfig.iChannels);
	format.nSamplesPerSec = rateInSamplesPerSecond;
	format.nAvgBytesPerSec = rateInSamplesPerSecond * iSoundConfig.iChannels * bitsPerSample / 8;
	format.nBlockAlign = TUint16(iSoundConfig.iChannels * bitsPerSample / 8);
	format.wBitsPerSample = bitsPerSample;
	format.cbSize = 0;

	MMRESULT res = MMSYSERR_NOERROR;

	__COND_HOST_LOCK;		
	if (iNoHardware)
		{
		timeBeginPeriod(KMMTimerRes);
		iPlayDeviceHandle = (HWAVEOUT)1;	
		}
	else
		{
		res = waveOutOpen(&iPlayDeviceHandle, WAVE_MAPPER, &format, (DWORD)::WaveOutProc, (DWORD)this, CALLBACK_FUNCTION);

		// On some builds of Windows (such as Windows Server 2003), the waveOutGetDevCaps() trick in
		// DoCreate() won't work, so we have another special check for missing hardware here
		if ((res == MMSYSERR_NODRIVER) || (res == MMSYSERR_BADDEVICEID))
			{
			// Pretend there was no error and switch into hardware emulation mode
			res = MMSYSERR_NOERROR;
			iNoHardware = ETrue;
			iPlayDeviceHandle = (HWAVEOUT)1;	
			iWaveformBufMgr->iIsHardware = EFalse;
			timeBeginPeriod(KMMTimerRes);
			}
		}

	if(iNoHardware)
		{
		iSimulatedUSecPlayed = 0;
		}

	switch (res)
		{
		case MMSYSERR_NOERROR: // No error
			return(KErrNone);
		case MMSYSERR_ALLOCATED: // Specified resource is already allocated.
			return(KErrInUse);
		case WAVERR_BADFORMAT: // Attempted to open with an unsupported waveform-audio format
			return(KErrNotSupported);
		case MMSYSERR_NOMEM: // Unable to allocate or lock memory.
			return(KErrNoMemory);
		default:
			return(KErrUnknown);
		}
	}
#pragma warning(default : 4702) // unreachable code
	
/**
Open the audio output device.
This function can be executed in either driver thread or windows thread context.
@pre The data members DWinsSoundScTxPdd::iSoundConfig and DWinsSoundScTxPdd::iVolume must be setup with the current
audio configuration.
*/
TInt DWinsSoundScTxPdd::CreatePlayDevice(TCreatePlayDeviceMode aMode)
	{
	// Check if the waveform output device is already open.
	if (iPlayDeviceHandle)
		return(KErrNone);
	
	WaitForSingleObjectDualThread(iPlayThreadMutex, INFINITE);

	__COND_HOST_LOCK;

	// Open the waveform output device for playback.
	TInt err = OpenWaveOutDevice();
	if (err != KErrNone)
		{
		ReleaseMutex(iPlayThreadMutex);
		return(err);
		}
		
	__HOST_LOCK_OFF;
	
	if (aMode==EInit && !iNoHardware)
		{
		// Remember the existing volume setting and use this as the default.
 		waveOutGetVolume(iPlayDeviceHandle,&iWinWaveVolume);
 		iVolume = iWinWaveVolume;
		}

	if (aMode==EStartTransfer)
		{
		if (!iNoHardware)
			{
			// Set the volume of the waveform output device.
			waveOutSetVolume(iPlayDeviceHandle, iVolume | (iVolume << 16));
			}
		
		// Now, re-allocate a set of the waveform audio blocks in advance of playing any data. Also, prepare one of these
		// for each buffer within the shared chunk. Need to be in critical section while re-allocating the audio blocks.
		NKern::ThreadEnterCS();
		err=iWaveformBufMgr->ReAllocAndUpdate(Ldd()->BufConfig(),Ldd()->ChunkBase(),(TInt)iPlayDeviceHandle);
		NKern::ThreadLeaveCS(); 
		}
	ReleaseMutex(iPlayThreadMutex);

	return(err);
	}

/**
Close down the play device.
This function can be executed in either driver thread or windows thread context.
*/
void DWinsSoundScTxPdd::ClosePlayDevice()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd:ClosePlayDevice"));
	
	WaitForSingleObjectDualThread(iPlayThreadMutex, INFINITE);

	HWAVEOUT handle = iPlayDeviceHandle;

	__COND_HOST_LOCK;

	if (iNoHardware)
		timeEndPeriod(KMMTimerRes);
		
	if (handle)
		{
		// Stop playback on the waveout device. (Windows resets the current position to zero and marks all pending
		// playback buffers as done).
		if (!iNoHardware)
			waveOutReset(handle);
		
		// Un-prepare all the waveform audio buffers.
		for (TInt i=0 ; i<iWaveformBufMgr->iNumWaveformBufs ; i++)
			iWaveformBufMgr->iWaveformAudioBuf[i].Unprepare((TInt)handle);
		
		// Close the waveout device.
		if (!iNoHardware)
			{
			waveOutSetVolume(handle,iWinWaveVolume);	// Restore the original volume setting before closing the driver.
			waveOutClose(handle);
			}
		
		iPlayDeviceHandle = NULL;
		}
		
	ReleaseMutex(iPlayThreadMutex);
	}
		
/**
The thread function for the play windows thread.
This function is always executed in windows thread context.
@pre The data members DWinsSoundScTxPdd::iSoundConfig and DWinsSoundScTxPdd::iVolume must be setup with the current
audio configuration.
*/
void DWinsSoundScTxPdd::PlayThread()
	{
	iPlayThreadSem = CreateSemaphore(NULL,0,0x7FFFFFFF,NULL);
	iPlayTimerEvent = CreateEvent(NULL,TRUE, FALSE, NULL);
	iPlayThreadMutex = CreateMutex(NULL,FALSE,NULL);
	__ASSERT_ALWAYS(iPlayThreadSem && iPlayTimerEvent, PANIC());  //no windows memory

	HANDLE objects[2];
	objects[0] = iPlayThreadSem;		// Indicates command from driver thread
	objects[1] = iPlayTimerEvent;		// Indicates timer gone off
	
	// Open the play device, then close it again. This is so we can return an error early to the client
	// if there is a problem.
	TInt ret = CreatePlayDevice(EInit);
	if (ret != KErrNone)
		{
		PlayThreadNotifyDriver(ret);
		return;
		}
	ClosePlayDevice();

	// Signal driver of successful setup
	PlayThreadNotifyDriver(KErrNone);
	ResetEvent(iPlayTimerEvent);

	// Wait for the timer to expire or a command.
	FOREVER
		{
		DWORD ret = WaitForMultipleObjectsEx(2,objects,FALSE,INFINITE,TRUE);
		__KTRACE_SND(Kern::Printf("   PlayThread resumed"));
		switch (ret)
			{
			case WAIT_OBJECT_0:	// Command received from the driver thread.
				if (ProcessPlayCommand(iPlayCommand,iPlayCommandArg0,iPlayCommandArg1,iPlayCommandArg2)==KErrCompletion)
					return; // ********* Exit thread **************
				break;
			case WAIT_OBJECT_0+1:
				HandlePlayTimerEvent();
				break;
			}
		}
	}
	
/**
Process a request from the driver thread to execute a command.
This function is always executed in windows thread context.
@param aCommand The identifier of the command to be executed.
@param aArg0 A first command argument, its meaning depends on the command.
@param aArg1 A second command argument, its meaning depends on the command.
@param aArg2 A third command argument, its meaning depends on the command.
@return KErrCompletion if the command to exit the windows thread has been received;
		KErrNone otherwise;
*/	
TInt DWinsSoundScTxPdd::ProcessPlayCommand(TThreadCommand aCommand,TInt aArg0,TInt aArg1,TInt aArg2)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd:ProcessPlayCommand(%d)",aCommand));
	switch(aCommand)
		{
		case ESendData:	// Initiate the playback of a buffers worth of data to the waveout device.
			{
			// Acquire a windows waveform audio buffer for the transfer.
			char* startAddress=(char*)aArg1;
			TInt bytesToPlay=aArg2; 
			__ASSERT_ALWAYS(bytesToPlay>0,PANIC());
			TWaveformAudioBuf* waveformAudioBuf=iWaveformBufMgr->AcquireBuf(startAddress,bytesToPlay,(TInt)iPlayDeviceHandle);
			waveformAudioBuf->iTransferID=(TUint)aArg0;
			waveformAudioBuf->iBufHdr.dwBufferLength=bytesToPlay;
			
			if (!iNoHardware)
				{
				// This machine has a waveout device present. Send the buffer to the waveout device.
				waveformAudioBuf->iBufHdr.dwFlags &= ~WHDR_DONE;	// Clear the done flag
				MMRESULT res = waveOutWrite(iPlayDeviceHandle,&waveformAudioBuf->iBufHdr,sizeof(WAVEHDR));
				__KTRACE_SND(Kern::Printf("   WaveOutWrite(ID:%x Pos:%x Len:%d)-%d",aArg0,aArg1,aArg2,res));
				__ASSERT_ALWAYS(res == MMSYSERR_NOERROR,PANIC()); // WaveOutWrite Error	
				}
			else 
				{
				// This machine has no audio hardware present so simulate the waveout device using a timer.
				AddToPendingList(&waveformAudioBuf->iBufHdr,iWaveformBufMgr->iPendingBufList);		// Queue the buffer on the pending list
				
				// Check if the timer needs starting/re-starting
				if (!iTimerActive)
					{
					iLastTimerEventTime = timeGetTime();
					StartTimer(&waveformAudioBuf->iBufHdr);
					}
				}
			
			// Signal the driver thread that we have started playing the next buffer.
			PlayThreadNotifyDriver(KErrNone);
		
			break;
			}
			
		case EStop:	// Terminate the playing of data to the waveout device.
			if (iNoHardware)
				{
				// This machine has no audio hardware present so simulates the waveout device using a timer.
				StopTimer(ETrue);	// Stop the timer and cancel any buffers pending
				}
			ClosePlayDevice();		// Close down the play device.	
			
			// Signal the driver thread that we have completed the command.
			if (iStopSemaphore)
				{
				LONG prev;
				ReleaseSemaphore(iStopSemaphore,1,&prev);
				}
			break;

		case EExit:	// Close down the play device and exit the windows thread.
			{
			if (!iNoHardware)
				{
				// This machine has a waveout device present.
				if (iPlayDeviceHandle)
					{
					waveOutReset(iPlayDeviceHandle);   					// Stop playback on the waveout device.
					waveOutSetVolume(iPlayDeviceHandle,iWinWaveVolume);	// Restore the original volume setting before closing the driver.
					waveOutClose(iPlayDeviceHandle);					// Close the waveout device.
					}
				}
			else
				{
				// This machine has no audio hardware present so simulates the waveout device using a timer.
				StopTimer(ETrue);	// Stop the timer and cancel any buffers pending
				}
			// Logically the playback device is now shut so clear the handle.
			iPlayDeviceHandle = 0;
				
			// Signal the driver thread that we have completed the command.	
			if (iDeathSemaphore)
				{
				LONG prev;
				ReleaseSemaphore(iDeathSemaphore,1,&prev);
				}
			return(KErrCompletion); 		// ********* Exit thread **************
			}

		case EPause:	// Halt the playback of data to the waveout device.
			if (!iNoHardware)
				waveOutPause(iPlayDeviceHandle); // Pause playback on the waveout device. Windows saves current position.
			else
				{
				StopTimer(EFalse);				 // Just stop the timer. Don't cancel any pending buffers.
				iPauseTime = timeGetTime();
				}
			break;

		case EResume:	// Resume the playback of data to the waveout device.
			if (!iNoHardware)
				waveOutRestart(iPlayDeviceHandle);	// Resume playback on the waveout device.
			else
				{
				// Check if there are more audio buffers waiting to be resumed
				WAVEHDR* buf=iWaveformBufMgr->iPendingBufList[0];
				if (buf)
					{
					// Before restarting the emulation timer, determine how long we were paused for and
					// add that time to the time the timer last triggered.  This will allow us to continue
					// as though we had never been paused
					DWORD currentTime = timeGetTime();
					iLastTimerEventTime += (currentTime - iPauseTime);
					iPauseTime = 0;
					StartTimer(buf);			// Re-start the timer
					}
				}
			break;
		} 
	return(KErrNone);
	}
	
/**
Handle a timer expiry event. This is only used when no audio hardware is present, with a timer expiry corresponding
to the end of a data block transfer.
This function is always executed in windows thread context.
*/
void DWinsSoundScTxPdd::HandlePlayTimerEvent()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd:HandlePlayTimerEvent"));
	ResetEvent(iPlayTimerEvent);	// Reset the event 

	iSimulatedUSecPlayed += (1000 * iSimulatedMsecDuration);
	
	// Remove the audio buffer just completed from the pending list and save it for the driver thread.
	WAVEHDR* buf=RemoveFromPendingList(iWaveformBufMgr->iPendingBufList);
	__ASSERT_ALWAYS(buf != NULL,PANIC());	
	TInt waveBufId=buf->dwUser;					// Work out which waveform audio buffer is completing.

	// Check if there are more audio buffers waiting to be played
	buf=iWaveformBufMgr->iPendingBufList[0];
	if (buf)
		{
		iLastTimerEventTime = timeGetTime();
		StartTimer(buf);						// Re-start the timer
		}
	else
		iTimerActive=EFalse;	
		
	// Notify that another audio buffer has been completed.
	StartOfInterrupt();
	iCompletedPlayBufHdrMask|=(1<<waveBufId);	// Update the completion status mask
	iDfc.Add();
	EndOfInterrupt();
	return;
	}
			
/**
Initialise the data member DWinsSoundScTxPdd::iCaps with the capabilities of this audio device.
*/	
void DWinsSoundScTxPdd::SetCaps()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScTxPdd::SetCaps"));
	
	// The data transfer direction for this unit is play.
	iCaps.iDirection=ESoundDirPlayback;
	
	// Assume this unit supports mono or stereo.
	iCaps.iChannels=KSoundMonoChannel|KSoundStereoChannel;
	
	// Assume this unit supports all sample rates.
	iCaps.iRates=(KSoundRate7350Hz|KSoundRate8000Hz|KSoundRate8820Hz|KSoundRate9600Hz|KSoundRate11025Hz|
				  KSoundRate12000Hz|KSoundRate14700Hz|KSoundRate16000Hz|KSoundRate22050Hz|KSoundRate24000Hz|
				  KSoundRate29400Hz|KSoundRate32000Hz|KSoundRate44100Hz|KSoundRate48000Hz);
	
	// Assume this unit supports 8bit and 16bit PCM encoding.
	iCaps.iEncodings=(KSoundEncoding8BitPCM|KSoundEncoding16BitPCM);
	
	// This unit only supports interleaved data format
	iCaps.iDataFormats=KSoundDataFormatInterleaved;
	
	// The minimum request size that the device can support. 
	iCaps.iRequestMinSize=0;	// No restriction
	
	// The request alignment that this device requires. 
	iCaps.iRequestAlignment=0;	// No restriction
	
	// This unit is not capable of detecting changes in hardware configuration.
	iCaps.iHwConfigNotificationSupport=EFalse;
	}
	
/**
Start the audio timer.
The timer is only used when no audio hardware is present on the machine. This is in order to introduce a delay which is
equivelent to that incurred when transferring audio data over a real audio device.
@param aBuffer The audio buffer which would have been transferred had a real audio device been present. This contains
	information on the number of bytes to transfer.
*/	
void DWinsSoundScTxPdd::StartTimer(WAVEHDR* aBuffer)
	{
	// First, need to calculate the duration of the timer in milliseconds.
	TInt bytesToPlay=aBuffer->dwBufferLength;
	iSimulatedMsecDuration = bytesToPlay*1000;
	iSimulatedMsecDuration /= (RateInSamplesPerSecond(iSoundConfig.iRate) * iSoundConfig.iChannels);
	if (iSoundConfig.iEncoding==ESoundEncoding16BitPCM)
		iSimulatedMsecDuration /= 2;
	if (iSoundConfig.iEncoding==ESoundEncoding24BitPCM)
		iSimulatedMsecDuration /= 3;
	if (iSimulatedMsecDuration<=0)
		iSimulatedMsecDuration=1;	// Round up to 1ms or timeSetEvent() will return an error.

	// If we have been paused and are now restarting, determine the amount of time that we were paused
	// and subtract that from the time until the next timer trigger.  If this is not done then the time
	// played or recorded will not be calculated correctly
	DWORD pauseTime = (timeGetTime() - iLastTimerEventTime);
	MMRESULT res = timeSetEvent((iSimulatedMsecDuration - pauseTime), KMMTimerRes, (LPTIMECALLBACK)iPlayTimerEvent, 0, TIME_ONESHOT | TIME_CALLBACK_EVENT_SET);
	__ASSERT_ALWAYS(res != NULL,PANIC()); 	// timeSetEvent error.	
	iTimerID = res;							// Save the identifier for the new timer event.
	iTimerActive=ETrue;
	}

/**
Stop the audio timer.
The timer is only used when no audio hardware is present on the machine. This is in order to introduce a delay which is
equivelent to that incurred when transferring audio data over a real audio device.
@param aCancellAll Set to ETrue in order to discard any buffers queued on the pending buffer list. EFalse otherwise.
*/	
void DWinsSoundScTxPdd::StopTimer(TBool aCancelAll)
	{
	if (iTimerActive)
		{
		MMRESULT res = timeKillEvent(iTimerID);
		__ASSERT_ALWAYS(res == TIMERR_NOERROR,PANIC()); // timeKillEvent error	
		
		if (aCancelAll)
			{
			WAVEHDR* b;
			do
				b=RemoveFromPendingList(iWaveformBufMgr->iPendingBufList);
			while(b);
			}
		}
	iTimerActive=EFalse;
	}	

