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
// wins\specific\soundsc_rx.cpp
// Emulator record functions for the shared chunk sound driver PDD.
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#include "winssoundsc.h"

/**
The thread function for the record windows thread.
This function is always executed in windows thread context.
*/
LOCAL_C TUint RecordThreadFunction(DWinsSoundScRxPdd *aSoundPdd)
	{
	aSoundPdd->RecordThread();
	return 0;
	}
		
/**
The waveform input callback function. This can receive the following messages:-
WIM_OPEN when the input device is opened, WIM_CLOSE when the input device is closed,
and WIM_DATA each time a record data block has been filled (i.e. completion of waveInAddBuffer).
This function is always executed in windows thread context.
*/
LOCAL_C void CALLBACK WaveInProc(HWAVEIN /*hwi*/, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD /*dwParam2*/)
	{
	if (uMsg == WIM_DATA)
		{
		DWinsSoundScRxPdd * pdd = (DWinsSoundScRxPdd*)dwInstance;
		pdd->WaveInProc((WAVEHDR*)dwParam1);
		}
	}			

/**
Constructor for the WINS shared chunk record PDD.
This function is always executed in driver thread context.
*/
DWinsSoundScRxPdd::DWinsSoundScRxPdd()
	: iDfc(DWinsSoundScRxPdd::RecordDfc,this,2)
	{		
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::DWinsSoundScRxPdd"));
	
//	iDriverThreadSem=0;
//	iRecordThread=0;
//	iRecordThreadMutex=0;
//	iRecordThreadSem=0;
//	iStopSemaphore=0;
//	iDeathSemaphore=0;
//	iRecordDeviceHandle=0;
//	iRecordCommand=ERecData;
//	iRecordCommandArg0=0;
//	iRecordCommandArg1=0;
//	iPendingRecord=0;
//	iRecordThreadError=0;
///	iWaveformBufMgr=NULL;
//	iCompletedRecordBufHdrMask=0;
//	iRecordBufferSize=0;
//	iRecordEnabled=EFalse;
//	iNoHardware=EFalse;
//	iRecordTimerEvent=0;
//	iTimerID=0;
//	iTimerActive=EFalse;
	}
	
/**
Destructor for the WINS shared chunk record PDD.
This function is always executed in driver thread context.
*/
DWinsSoundScRxPdd::~DWinsSoundScRxPdd()
	{
	// If the Windows thread started up successfully, signal it to shut down and wait for it to do so
	if (iRecordThreadRunning)
		{
		// Signal the windows thread to close down the record device and exit the windows thread.
		iDeathSemaphore = CreateSemaphore(NULL, 0, 2, NULL);
		RecordThreadCommand(EExit);
	
		// Wait for the record thread to terminate.
		if (iDeathSemaphore)
			{
			Emulator::Escape();
			WaitForSingleObject(iDeathSemaphore, INFINITE); 
			Emulator::Reenter();

			__HOST_LOCK;
			CloseHandle(iDeathSemaphore);
			}
		}
		
	if (iRecordTimerEvent)
		CloseHandle(iRecordTimerEvent); 
	if (iRecordThreadSem)
		CloseHandle(iRecordThreadSem); 
	if (iRecordThread)
		CloseHandle(iRecordThread);
	if (iDriverThreadSem)
		CloseHandle(iDriverThreadSem); 	
	
	if (iWaveformBufMgr)
		delete iWaveformBufMgr;
	}
	
/**
Second stage constructor for the WINS shared chunk record PDD.
Note that this constructor is called before the second stage constructor for the LDD so it is not
possible to call methods on the LDD here.
This function is always executed in driver thread context.
@param aPhysicalDevice A pointer to the factory class that is creating this PDD.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/	
TInt DWinsSoundScRxPdd::DoCreate(DWinsSoundScPddFactory* aPhysicalDevice)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::DoCreate"));

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
	// when waveInOpen() is called
	WAVEINCAPS waveInCaps;
	MMRESULT res = waveInGetDevCaps(WAVE_MAPPER,&waveInCaps,sizeof(WAVEINCAPS));
#ifdef FORCE_NO_HARDWARE
	res=MMSYSERR_NOERROR+1;
#endif
	if (res != MMSYSERR_NOERROR)
		iNoHardware = ETrue;
	
	__HOST_LOCK_OFF;
	
	// Create the windows waveform audio buffer manager.
	iWaveformBufMgr=new TWaveformBufMgr(ESoundDirRecord,!iNoHardware);
	if (!iWaveformBufMgr)
		return(KErrNoMemory);
	
	// Create the driver thread semaphore.
	iDriverThreadSem = CreateSemaphore(NULL,0,0x7fffffff,NULL);
	if (!iDriverThreadSem)
		return(KErrNoMemory);
	
	// Create the record windows thread.
	if ((iRecordThread=CreateWin32Thread(EThreadEvent,(LPTHREAD_START_ROUTINE)RecordThreadFunction,(void *)this, FALSE))==NULL)
		return(Emulator::LastError());
	SetThreadPriority(iRecordThread,THREAD_PRIORITY_HIGHEST);
	__ASSERT_ALWAYS( ResumeThread(iRecordThread) != 0xffffffff, PANIC()); //Windows Unexpected Error
	
	// Wait to be notified of successful thread initialization
	Emulator::Escape();
	WaitForSingleObject(iDriverThreadSem,INFINITE);
	Emulator::Reenter();

	// If the Windows thread started up successfully, indicate this fact so that when shutting down we know
	// to signal to the thread to exit
	if (iRecordThreadError == KErrNone)
		iRecordThreadRunning = ETrue;

	return(iRecordThreadError);
	}

/**
Called from the LDD to return the DFC queue to be used by this device.
This function is always executed in driver thread context.
@return The DFC queue to use.
*/	
TDfcQue* DWinsSoundScRxPdd::DfcQ(TInt /*aUnit*/)
	{
	return(iPhysicalDevice->iDfcQ);
	}

/** 
Called from the LDD to return the shared chunk create information to be used by this device.
This function is always executed in driver thread context.
@param aChunkCreateInfo A chunk create info. object to be to be filled with the settings
						required for this device.
*/		
void DWinsSoundScRxPdd::GetChunkCreateInfo(TChunkCreateInfo& aChunkCreateInfo)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::GetChunkCreateInfo"));

	aChunkCreateInfo.iType=TChunkCreateInfo::ESharedKernelMultiple;
	aChunkCreateInfo.iMapAttr=0;
	aChunkCreateInfo.iOwnsMemory=ETrue; 				// Using RAM pages.
	aChunkCreateInfo.iDestroyedDfc=NULL; 				// No chunk destroy DFC.
	}
	
/**
Called from the LDD to return the capabilities of this device.
This function is always executed in driver thread context.
@param aCapsBuf A packaged TSoundFormatsSupportedV02 object to be filled with the record
				capabilities of this device. This descriptor is in kernel memory and can be accessed directly.
@see TSoundFormatsSupportedV02.
*/
void DWinsSoundScRxPdd::Caps(TDes8& aCapsBuf) const
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::Caps"));
	
	// Copy iCaps back.
	TPtrC8 ptr((const TUint8*)&iCaps,sizeof(iCaps));
	aCapsBuf.FillZ(aCapsBuf.MaxLength());
	aCapsBuf=ptr.Left(Min(ptr.Length(),aCapsBuf.MaxLength()));	
	}
	
/**
Called from the LDD to return the maximum transfer length in bytes that this device can support in a single data transfer.
@return The maximum transfer length in bytes.
*/
TInt DWinsSoundScRxPdd::MaxTransferLen() const
	{
	return(KWinsMaxAudioTransferLen);		// 32K
	}		
				
/**
Called from the LDD to power up the sound device.
This function is always executed in driver thread context.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScRxPdd::PowerUp()
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
TInt DWinsSoundScRxPdd::SetConfig(const TDesC8& aConfigBuf)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::SetConfig"));

	// Cannot change the configuration while the device is open and recording. (LDD should prevent
	// this anyway but better safe than sorry).
	if (iRecordDeviceHandle)
		return(KErrInUse);
	
	// Save the current settings so we can restore them if there is a problem with the new ones.
	TCurrentSoundFormatV02 saved=iSoundConfig;
	
	// Read the new configuration from the LDD.
	TPtr8 ptr((TUint8*)&iSoundConfig,sizeof(iSoundConfig));
	Kern::InfoCopy(ptr,aConfigBuf);
	
	// Open the record device with the new settings to check they are supported. Then close it
	// again - don't leave it open yet.
	TInt r = CreateRecordDevice(ETrue);
	if (r==KErrNone)
		CloseRecordDevice();
	else
		iSoundConfig=saved;	// Restore the previous settings
	
	return(r);
	}

/**
Called from the LDD to set the record level.
This function is always executed in driver thread context.
@param aLevel The record level to be set - a value in the range 0 to 255. The value 255 equates 
	   to the maximum record level and each value below this equates to a 0.5dB step below it.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScRxPdd::SetVolume(TInt /*aVolume*/)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::SetVolume"));
	
	// There's no adjustment of the record level on the wave in device.
	
	return(KErrNone);
	}
	
/**
Called from the LDD to prepare the audio device for recording.
This function is always executed in driver thread context.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScRxPdd::StartTransfer()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::StartTransfer"));

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

	iBytesRecordedBeforeLastPause = 0;
	iBytesSincePauseReportedToLdd = 0;

	iRecordEnabled=ETrue;

	// Open the record device with the current settings.
	iPendingRecord=0;
	iCompletedRecordBufHdrMask=0;					// Reset the completion status mask
	TInt r = CreateRecordDevice();
	return(r);
	}
	
/**
Called from the LDD to initiate the recording of a portion of data from the audio device. 
When the transfer is complete, the PDD signals this event using the LDD function RecordCallback().
This function is always executed in driver thread context.
@param aTransferID A value assigned by the LDD to allow it to uniquely identify a particular transfer fragment.
@param aLinAddr The linear address within the shared chunk for storing the recorded data.
@param aPhysAddr The physical address within the shared chunk for storing the recorded data.
@param aNumBytes The number of bytes to be recorded. 
@return KErrNone if the transfer has been initiated successfully;
  		KErrNotReady if the device is unable to accept the transfer for the moment;
		otherwise one of the other system-wide error codes.
*/
TInt DWinsSoundScRxPdd::TransferData(TUint aTransferID,TLinAddr aLinAddr,TPhysAddr /*aPhysAddr*/,TInt aNumBytes)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::TransferData(ID:%xH)",aTransferID));
	
	// Check that we can accept the request
	if (aNumBytes>KWinsMaxAudioTransferLen)
		return(KErrArgument);
	if (iPendingRecord>=iWaveformBufMgr->iNumWaveformBufs)	// LDD may issue multiple data transfers per buffer.
		return(KErrNotReady);
	
	// Signal the windows thread to initiate the recording of a buffers worth of data from the wavein device.
	iPendingRecord++;
	RecordThreadCommand(ERecData,aTransferID,aLinAddr,aNumBytes);
	
	// Although the windows thread runs at a higher priority, its not safe to assume we will always get pre-empted at this
	// point while the the higher priority thread processes and completes the request. Instead we need to wait until it
	// signals back completion of the command.
	Emulator::Escape();
	WaitForSingleObject(iDriverThreadSem,INFINITE);
	Emulator::Reenter();	

	return(iRecordThreadError);	
	}

/**
Called from the LDD to terminate the recording of a data from the device and to release any resources necessary for
recording.
The LDD will leave the audio device capturing record data even when there are no record requests pending from the client.
Transfer will only be terminated when the client either issues RSoundSc::CancelRecordData() or closes the channel. Once
this function had been called, the LDD will not issue  any further TransferData() commands without first issueing a
StartTransfer() command.
This function is always executed in driver thread context.
*/
void DWinsSoundScRxPdd::StopTransfer()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::StopTransfer"));
	
	// Signal the windows thread to stop it from sending any more buffers to wavein device.
	iStopSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	RecordThreadCommand(EStop);

	// Need to wait for the record thread to finish using the record handle before it's safe to close the device and 
	// set the handle to NULL.
	if (iStopSemaphore)
		{
		// Wait for the record thread to stop.
		Emulator::Escape();
		WaitForSingleObject(iStopSemaphore, INFINITE);  
		Emulator::Reenter();

		__HOST_LOCK;
		CloseHandle(iStopSemaphore);
		iStopSemaphore = NULL;
		}
		
	// Make sure the DFC is not queued.
	iDfc.Cancel();	
			
	CloseRecordDevice();							// Close down the record device.
	iPendingRecord=0;
	iCompletedRecordBufHdrMask=0;					// Reset the completion status mask
	}

/**
Called from the LDD to halt the recording of data from the sound device but not to release any resources necessary for
recording.
All active transfers should be aborted. When recording is halted the PDD signals this event with a single call of the LDD 
function RecordCallback() - reporting back any partial data already received. If transfer is resumed later, the LDD will
issue a new TransferData() request to re-commence data transfer.
This function is always executed in driver thread context.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScRxPdd::PauseTransfer()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::PauseTransfer"));
	
	// Signal the windows thread to stop recording on the wavein device - aborting any transfers queued.
	RecordThreadCommand(EPause);
	
	// Wait for the windows thread to complete the request
	Emulator::Escape();
	WaitForSingleObject(iDriverThreadSem,INFINITE);
	Emulator::Reenter();
	
	// Make sure the DFC is not queued.
	iDfc.Cancel();
	
	// The windows thread returns the total bytes recorded since the last pause (as reported by windows). 
	TUint totalRecordedSincePause = iRecordThreadError;
	TUint lastTransferLength = totalRecordedSincePause - iBytesSincePauseReportedToLdd;
	Kern::Printf("totalRecordedSincePause %d - iBytesSincePauseReportedToLdd %d = lastTransferLength %d\n",
				 totalRecordedSincePause, iBytesSincePauseReportedToLdd, lastTransferLength);

	iBytesRecordedBeforeLastPause += totalRecordedSincePause;
	iBytesSincePauseReportedToLdd = 0;

	if (iPendingRecord)
		{
		Ldd()->RecordCallback(0, KErrNone, lastTransferLength);	// We can use a NULL tranfer ID when pausing.
	
		// The LDD will abandon any other transfers queued so we can mark all buffers as not in use.
    	for (TInt i=0 ; i<iWaveformBufMgr->iNumWaveformBufs ; i++)
    		{
    		TWaveformAudioBuf* buf=&iWaveformBufMgr->iWaveformAudioBuf[i];
    		if (buf->iIsInUse)
    			buf->iIsInUse=EFalse;
    		}
		iPendingRecord=0;
		}

	// Indicate that all request to Windows for recording have been cancelled
	iCompletedRecordBufHdrMask=0;
					
	return(KErrNone);
	}
	
/**
Called from the LDD to resume the recording of data from the sound device following a request to halt recording.
Any active transfer would have been aborted when the device was halted so its just a case of re-creating the same setup
acheived following StartTransfer().
This function is always executed in driver thread context.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScRxPdd::ResumeTransfer()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::ResumeTransfer"));
	
	iPendingRecord=0;
	iCompletedRecordBufHdrMask=0; // All buffers would have completed during pausing (waveInReset) so clear mask.
	iRecordEnabled=ETrue; // Simply set the flag to enable the windows thread to restart sending buffers to wavein device. 

	// Signal the windows thread to resume recording on the wavein device.
	RecordThreadCommand(EResume);

	return(KErrNone);
	}
	
/**
Called from the LDD to power down the sound device.
This function is always executed in driver thread context.
*/
void DWinsSoundScRxPdd::PowerDown()
	{
	
	}
	
/**
Called from the LDD to handle a custom configuration request.
@param aFunction A number identifying the request.
@param aParam A 32-bit value passed to the driver. Its meaning depends on the request.
@return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DWinsSoundScRxPdd::CustomConfig(TInt /*aFunction*/,TAny* /*aParam*/)
	{
	return(KErrNotSupported);
	}
		
/**
Called from the LDD to find out how many microseconds of data have been recorded.  This is called
in the context of the DFC thread.
@param aTimeTransferred	A reference to a variable into which to place the number of microseconds of audio.
@param aStatus			The current status of this channel
@return KErrNone if time is valid or KErrNotSupported.
*/
TInt DWinsSoundScRxPdd::TimeTransferred(TInt64& aTimeRecorded, TInt aState)
	{
	TInt r=KErrGeneral;
	TInt64 ms=0;

	if(iRecordDeviceHandle == 0)
		{
		// Recording not started yet
		aTimeRecorded = 0;
		return KErrNone;
		}

	// Kern::Printf("DWinsSoundScRxPdd::TimeTransferred - (iBytesSincePauseReportedToLdd=%d)\n", iBytesSincePauseReportedToLdd);
	if (aState == DSoundScLdd::EPaused)
		{
		// Kern::Printf("DWinsSoundScRxPdd::TimeTransferred (paused) - iBytesRecordedBeforeLastPause %d\n", iBytesRecordedBeforeLastPause);
		// Just use the paused number of bytes
		ms=((iBytesRecordedBeforeLastPause/iBytesPerSecond)*1000);
		TUint remainder=(iBytesRecordedBeforeLastPause%iBytesPerSecond);
		ms+=((remainder*1000)/iBytesPerSecond);
		ms*=1000;
		aTimeRecorded=ms;
		r=KErrNone;
		}

	TInt64 bytesTransferredSincePause = 0;
	// If no hardware is present then we need to use iBytesSincePauseReportedToLdd + a fudge factor to allow
	// the number of bytes processed by the "hardware" within the current transfer.
	if(iNoHardware)
		{
		// Determine the # of milliseconds that have passed since the last timer triggered
		DWORD currentTime = timeGetTime();
		DWORD timeSinceLastEvent = (currentTime - iLastTimerEventTime);

		// Clamp the resulting value to the duration of the timer, to prevent the millisecond count
		// going backwards if Windows is busy and latency becomes an issue
		if (timeSinceLastEvent > iSimulatedMsecDuration)
			timeSinceLastEvent = iSimulatedMsecDuration;

		bytesTransferredSincePause = iBytesSincePauseReportedToLdd;
		WAVEHDR *buf = iWaveformBufMgr->iPendingBufList[0];
		if(buf)
			{
			// Add on an estimate of the progress of the current transfer
			bytesTransferredSincePause += ((buf->dwBufferLength * timeSinceLastEvent) / iSimulatedMsecDuration);
			}
		}
	else
		{
		// Get the number of bytes recorded by the Windows audio system
		MMTIME time;
		time.wType=TIME_BYTES;
		if ((waveInGetPosition(iRecordDeviceHandle,&time,sizeof(time)) != MMSYSERR_NOERROR) ||
			(time.wType != TIME_BYTES))
			{
			// If requesting the number of bytes recorded is not supported, wType will be
			// changed to what was actually returned, so check for this and don't continue
			// if we got anything other than bytes
			return KErrNotSupported;
			}
		bytesTransferredSincePause = time.u.cb;
		}

	// Kern::Printf("DWinsSoundScRxPdd::TimeTransferred - iBytesRecordedBeforeLastPause %d + bytesTransferredSincePause %d total %d (iNoHardware %d)\n", 
	// 			iBytesRecordedBeforeLastPause, TUint32(bytesTransferredSincePause), TUint32(bytesTransferredSincePause + iBytesRecordedBeforeLastPause), iNoHardware);
	// Convert the number of bytes recorded into microseconds and return it
	ms=(((bytesTransferredSincePause + iBytesRecordedBeforeLastPause)/iBytesPerSecond)*1000);
	TUint64 remainder=((bytesTransferredSincePause + iBytesRecordedBeforeLastPause)%iBytesPerSecond);
	ms+=((remainder*1000)/iBytesPerSecond);
	ms*=1000;
	aTimeRecorded=ms;
	r=KErrNone;

	return(r);
	}

/** 
Prepare the waveform audio buffer for record.
@param aRecordDeviceHandle The handle to the waveform audio input device.
*/		
void TWaveformAudioBuf::DoPrepareIn(HWAVEIN aRecordDeviceHandle)
	{
	MMRESULT res = waveInPrepareHeader(aRecordDeviceHandle,&iBufHdr,sizeof(WAVEHDR));
	__KTRACE_SND(Kern::Printf("   waveInPrepareHeader(BufNo:%d Pos:%x Len:%d)-%d",iBufNum,iBufHdr.lpData,iBufHdr.dwBufferLength,res));
	__ASSERT_ALWAYS(res==MMSYSERR_NOERROR,Kern::Fault("DWinsSoundScTxPddWIPH", res)); //WaveInPrepareHeader error.	
	}
	
/**
Cleanup the preparation performed when the waveform audio buffer was prepared for record.
@param aRecordDeviceHandle The handle to the waveform audio input device.
*/	
void TWaveformAudioBuf::DoUnprepareIn(HWAVEIN aRecordDeviceHandle)
	{
	MMRESULT res = waveInUnprepareHeader(aRecordDeviceHandle,&iBufHdr,sizeof(WAVEHDR));
	__KTRACE_SND(Kern::Printf("   waveInUnprepareHeader(BufNo:%d)-%d",iBufNum,res));
	__ASSERT_ALWAYS(res==MMSYSERR_NOERROR,Kern::Fault("DWinsSoundScTxPddWIUH",res)); //WaveInUnprepareHeader error.
	}
															
/**
The waveform input callback function to handle data block transfer completion.
This function is always executed in windows thread context.
@param aHdr A pointer to the header for the waveform audio buffer just transferred.
*/	
void DWinsSoundScRxPdd::WaveInProc(WAVEHDR* aHdr)
	{
	TInt waveBufId=aHdr->dwUser;				// Work out which waveform audio buffer is completing.
    // Kern::Printf("DWinsSoundScRxPdd::WaveInProc waveBufId %d", waveBufId);

	StartOfInterrupt();
	iCompletedRecordBufHdrMask|=(1<<waveBufId);	// Update the completion status mask
	iDfc.Add();									// Queue RecordDfc().
	EndOfInterrupt();
	}
	
/**
The DFC used to handle data block record completion.
This function is always executed in driver thread context.
@param aPtr A pointer to the physical channel object.
*/	
void DWinsSoundScRxPdd::RecordDfc(TAny* aPtr)
	{
	TInt i;
	DWinsSoundScRxPdd& drv=*(DWinsSoundScRxPdd*)aPtr;
	
	// More than 1 transfer may have completed so loop until all completions are handled
	while (drv.iCompletedRecordBufHdrMask)
		{
		// Find the buffer ID of the next transfer that has completed
		for (i=0 ; i<32 && !(drv.iCompletedRecordBufHdrMask&(1<<i)) ; i++) {}
		__ASSERT_ALWAYS(i<drv.iWaveformBufMgr->iNumWaveformBufs,PANIC());
		__e32_atomic_and_ord32(&drv.iCompletedRecordBufHdrMask, ~(1u<<i)); // Clear this bit in the mask
		
		// Update the status of the waveform audio buffer which is completing
		TWaveformAudioBuf& buf=drv.iWaveformBufMgr->iWaveformAudioBuf[i];
		buf.iIsInUse=EFalse;
	
		// Callback the LDD passing the information for the transfer that has completed
		drv.iPendingRecord--;
		__KTRACE_SND(Kern::Printf("   Read complete(BufNo:%x Pos:%x Len:%d)",i,buf.iBufHdr.lpData,buf.iBufHdr.dwBufferLength));
		drv.iBytesSincePauseReportedToLdd += buf.iBufHdr.dwBufferLength;
		drv.Ldd()->RecordCallback(buf.iTransferID,KErrNone,buf.iBufHdr.dwBufferLength);
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
void DWinsSoundScRxPdd::RecordThreadCommand(TThreadCommand aCommand,TInt aArg0,TInt aArg1,TInt aArg2)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd:RecordThreadCommand"));
	iRecordCommand = aCommand;
	iRecordCommandArg0 = aArg0;
	iRecordCommandArg1 = aArg1;
	iRecordCommandArg2 = aArg2;

	__HOST_LOCK;

	ReleaseSemaphore(iRecordThreadSem,1,NULL);
	}

/**
Pass a value from the windows thread to the driver thread.
This function is always executed in windows thread context.
@param aError The value to the passed to the driver thread.
*/
void DWinsSoundScRxPdd::RecordThreadNotifyDriver(TInt aError)
	{
	iRecordThreadError = aError;
	BOOL ret = ReleaseSemaphore(iDriverThreadSem,1,NULL);
	__ASSERT_ALWAYS(ret == TRUE,PANIC()); //Unexpected Windows Error
	}

#pragma warning(disable : 4702) // unreachable code
/**
Open the waveform input device for record. Use a default device identifier in order to select a device
capable of meeting the current audio configuration. 
This function can be executed in either driver thread or windows thread context.
@pre The data member DWinsSoundScRxPdd::iSoundConfig must be setup with the current audio configuration.
*/
TInt DWinsSoundScRxPdd::OpenWaveInDevice()
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
		iRecordDeviceHandle = (HWAVEIN)1;	
		}
	else
		{
		res = waveInOpen(&iRecordDeviceHandle, WAVE_MAPPER, &format, (DWORD)::WaveInProc, (DWORD)this, CALLBACK_FUNCTION);

		// On some builds of Windows (such as Windows Server 2003), the waveInGetDevCaps() trick in
		// DoCreate() won't work, so we have another special check for missing hardware here
		if ((res == MMSYSERR_NODRIVER) || (res == MMSYSERR_BADDEVICEID))
			{
			// Pretend there was no error and switch into hardware emulation mode
			res = MMSYSERR_NOERROR;
			iNoHardware = ETrue;
			iRecordDeviceHandle = (HWAVEIN)1;	
			iWaveformBufMgr->iIsHardware = EFalse;
			timeBeginPeriod(KMMTimerRes);
			}
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
Open the audio input device.
This function is always executed in driver thread context.
@pre The data members DWinsSoundScRxPdd::iSoundConfig must be setup with the current audio configuration.
*/
TInt DWinsSoundScRxPdd::CreateRecordDevice(TBool aCheckDevice)
	{
	// Check if the waveform input device is already open.
	if (iRecordDeviceHandle)
		return(KErrNone);

	__HOST_LOCK;

	// Open the waveform input device for recording.
	TInt err = OpenWaveInDevice();
	if (err != KErrNone)
		return(err);
	
	__HOST_LOCK_OFF;	

	if (!aCheckDevice)
		{
		// Now, re-allocate a set of the waveform audio blocks in advance of any recording. Also, prepare one of these
		// for each buffer within the shared chunk. Need to be in critical section while re-allocating the audio blocks.
		NKern::ThreadEnterCS();
		err=iWaveformBufMgr->ReAllocAndUpdate(Ldd()->BufConfig(),Ldd()->ChunkBase(),(TInt)iRecordDeviceHandle);
		NKern::ThreadLeaveCS(); 
		}
		
	return(err);
	}

/**
Close down the record device.
This function is always executed in driver thread context.
*/
void DWinsSoundScRxPdd::CloseRecordDevice()
	{
	__COND_HOST_LOCK;

	if (iNoHardware)
		timeEndPeriod(KMMTimerRes);

	HWAVEIN handle = iRecordDeviceHandle;

	if (handle)
		{
		if (!iNoHardware)
			waveInReset(handle);		// Stop recording.
		
		// Un-prepare all the waveform audio buffers.
		for (TInt i=0 ; i<iWaveformBufMgr->iNumWaveformBufs ; i++)
			iWaveformBufMgr->iWaveformAudioBuf[i].Unprepare((TInt)handle);
		
		if (!iNoHardware)
			waveInClose(handle);		// Close the wavein device.
		
		iRecordDeviceHandle = NULL;
		}
	}
		
/**
The thread function for the record windows thread.
This function is always executed in windows thread context.
@pre The data members DWinsSoundScRxPdd::iSoundConfig must be setup with the current audio configuration.
*/
void DWinsSoundScRxPdd::RecordThread()
	{
	iRecordThreadSem = CreateSemaphore(NULL,0,0x7fffffff,NULL);
	__ASSERT_ALWAYS(iRecordThreadSem,PANIC()); //No Windows Memory
	iRecordTimerEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	HANDLE objects[2];
	objects[0]=iRecordThreadSem;		// Indicates command from driver thread
	objects[1]=iRecordTimerEvent;

	// Signal driver of successful setup
	RecordThreadNotifyDriver(KErrNone);
	ResetEvent(iRecordTimerEvent);
	FOREVER
		{
		DWORD ret=WaitForMultipleObjectsEx(2,objects,FALSE,INFINITE,TRUE);

		switch (ret)
			{
			case WAIT_OBJECT_0:	// Command received from the driver thread.
				if (ProcessRecordCommand(iRecordCommand,iRecordCommandArg0,iRecordCommandArg1,iRecordCommandArg2)==KErrCompletion)
					return; // ********* Exit thread **************
				break;
			case WAIT_OBJECT_0+1:
				HandleRecordTimerEvent();
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
TInt DWinsSoundScRxPdd::ProcessRecordCommand(TThreadCommand aCommand,TInt aArg0,TInt aArg1,TInt aArg2)
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd:ProcessRecordCommand(%d)",aCommand));
	switch(aCommand)
		{	
		case ERecData:	// Initiate the recording of a buffers worth of data from the wavein device.
			{
			if (iRecordEnabled)
				{
				// Acquire a windows waveform audio buffer for the transfer.
				char* startAddress=(char*)aArg1;
				TInt bytesToRecord=aArg2; 
				__ASSERT_ALWAYS(bytesToRecord>0,PANIC());
				TWaveformAudioBuf* waveformAudioBuf=iWaveformBufMgr->AcquireBuf(startAddress,bytesToRecord,(TInt)iRecordDeviceHandle);
				waveformAudioBuf->iTransferID=(TUint)aArg0;
				waveformAudioBuf->iBufHdr.dwBufferLength=bytesToRecord;
									
				if (!iNoHardware)
					{
					// This machine has a wavein device present. Send the buffer to the wavein device.
					waveInStart(iRecordDeviceHandle); // Start input on the wavein device - safe to call this when already started.
					MMRESULT res = waveInAddBuffer(iRecordDeviceHandle,&waveformAudioBuf->iBufHdr,sizeof(WAVEHDR));
					__KTRACE_SND(Kern::Printf("   waveInAddBuffer(ID:%x Pos:%x Len:%d)-%d",aArg0,startAddress,bytesToRecord,res));
					__ASSERT_ALWAYS(res == MMSYSERR_NOERROR,PANIC());  //WaveInAddBuffer Error
					}
				else	
					{
					// This machine has no audio hardware present so simulate the wavein device using a timer.
					AddToPendingList(&waveformAudioBuf->iBufHdr,iWaveformBufMgr->iPendingBufList); // Queue the buffer on the pending list
				
					// Check if the timer needs starting/re-starting
					if (!iTimerActive)
						{
						iLastTimerEventTime = timeGetTime();
						StartTimer(&waveformAudioBuf->iBufHdr);
						}
					} 			
				}
		
			// Signal the driver thread that we have completed the command.
			RecordThreadNotifyDriver(KErrNone);
			break;
			}
			
			
		case EStop:	// Terminate the recording of data from the wavein device.
			{
			iRecordEnabled=EFalse;	// Stop the windows thread from sending any more buffers to wavein device. 
			
			if (iNoHardware)
				{
				// This machine has no audio hardware present so simulates the waveout device using a timer.
				StopTimer(ETrue);	// Stop the timer and cancel any buffers pending
				}
				
			// Leave the driver thread to close down the record device.
				
			// Signal the driver thread that we have completed the command.	
			if (iStopSemaphore)
				{
				LONG prev;
				ReleaseSemaphore(iStopSemaphore,1,&prev);
				}
			break;
			}
	
		case EExit:	// Close down the record device and exit the windows thread.
			{
			if (!iNoHardware)
				{
				// This machine has a wavein device present.
				if (iRecordDeviceHandle)
					{
					waveInReset(iRecordDeviceHandle);   // Stop recording on the wavein device.
					waveInClose(iRecordDeviceHandle);	// Close the wavein device.
					}
				}
			else
				{
				// This machine has no audio hardware present so simulates the waveout device using a timer.
				StopTimer(ETrue);	// Stop the timer and cancel any buffers pending.
				}	
			// Logically the record device is now shut so clear the handle.
			iRecordDeviceHandle = 0;
	
			// Signal the driver thread that we have completed the command.		
			if (iDeathSemaphore)
				{
				LONG prev;
				ReleaseSemaphore(iDeathSemaphore,1,&prev);
				}
			return(KErrCompletion); 		// ********* Exit thread **************
			}
			
		case EPause:	// Halt the recording of data from the wavein device.
			iRecordEnabled=EFalse;
			
			DWORD position;
			if (!iNoHardware)
				{
				// Need to try to work out how much of the current audio buffer has been filled.
				MMTIME time;
				time.wType = TIME_BYTES;
				HWAVEIN handle = iRecordDeviceHandle;
				waveInGetPosition(handle,&time,sizeof(MMTIME));
				position = time.u.cb;
			
				// Stop recording. (Windows will mark all pending audio buffers as done).
				waveInReset(handle);
				}
			else
				{
				// This machine has no audio hardware present so simulates the waveout device using a timer.

				// Determine the # of milliseconds that have passed since the last timer triggered
				DWORD currentTime = timeGetTime();
				DWORD timeSinceLastEvent = (currentTime - iLastTimerEventTime);
				
				// Clamp the resulting value to the duration of the timer, to prevent the millisecond count
				// going backwards if Windows is busy and latency becomes an issue
				if (timeSinceLastEvent > iSimulatedMsecDuration)
					timeSinceLastEvent = iSimulatedMsecDuration;
				
				TUint bytesTransferredSincePause = iBytesSincePauseReportedToLdd;
				WAVEHDR *buf = iWaveformBufMgr->iPendingBufList[0];
				if(buf)
					{
					// Add on an estimate of the progress of the current transfer
					bytesTransferredSincePause += ((buf->dwBufferLength * timeSinceLastEvent) / iSimulatedMsecDuration);
					}
				
				position = bytesTransferredSincePause;
 	
                StopTimer(ETrue);   // Stop the timer and cancel any buffers pending
				}	
			
			// Signal the driver thread that we have stopped recording - returning info. on any partially filled buffer.
			RecordThreadNotifyDriver(position);
			break;

		case EResume:
			if (iNoHardware)
				{
				// Determine how long we were paused for and add that time to the time the timer last
				// triggered.  This will allow us to continue as though we had never been paused
				iLastTimerEventTime = timeGetTime();
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
void DWinsSoundScRxPdd::HandleRecordTimerEvent()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd:HandleRecordTimerEvent"));
	ResetEvent(iRecordTimerEvent);	// Reset the event 
	
	// Remove the audio buffer just filled from the pending list and save it for the driver thread.
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
		
	// Notify that another audio buffer has been filled.
	StartOfInterrupt();
	iCompletedRecordBufHdrMask|=(1<<waveBufId);	// Update the completion status mask
	iDfc.Add();
	EndOfInterrupt();
	return;
	}
				
/**
Initialise the data member DWinsSoundScRxPdd::iCaps with the capabilities of this audio device.
*/	
void DWinsSoundScRxPdd::SetCaps()
	{
	__KTRACE_SND(Kern::Printf(">DWinsSoundScRxPdd::SetCaps"));
	
	// The data transfer direction for this unit is record.
	iCaps.iDirection=ESoundDirRecord;
	
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
void DWinsSoundScRxPdd::StartTimer(WAVEHDR* aBuffer)
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
				
	MMRESULT res = timeSetEvent(iSimulatedMsecDuration, KMMTimerRes, (LPTIMECALLBACK)iRecordTimerEvent, 0, TIME_ONESHOT | TIME_CALLBACK_EVENT_SET);
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
void DWinsSoundScRxPdd::StopTimer(TBool aCancelAll)
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

	
