// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\winssoundsc.h
// Definitions for the emulator shared chunk sound driver PDD.
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef __WINSSOUNDSC_H__
#define __WINSSOUNDSC_H__

#include <drivers/soundsc.h>
#include "nk_priv.h"
#include <emulator.h>
#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#include <mmsystem.h>
#include <mmreg.h>
#pragma warning(default : 4201)
#include <property.h>

//#define FORCE_NO_HARDWARE

//#define __KTRACE_SND(s) s;
#define __KTRACE_SND(s)

#define PANIC()	Kern::Fault("WinsSoundScPdd", __LINE__)

const TInt KMMTimerRes = 5;	// Minimum timer resolution (5mS). Timer used only when no audio hardware present.
const TInt KWinsMaxAudioTransferLen=0x8000;	// The maximum transfer length this PDD will accept (32K).
const TInt KMinWaveHdrBufCount=8;			// The minimum number of record or playback waveform audio buffers.

GLREF_C TInt RateInSamplesPerSecond(TSoundRate aRate);
GLREF_C DWORD WaitForSingleObjectDualThread(HANDLE hHandle,DWORD dwMilliseconds);
GLREF_C WAVEHDR* RemoveFromPendingList(WAVEHDR** aList);
GLREF_C void AddToPendingList(WAVEHDR* aBuffer,WAVEHDR** aList);

// Utility class used to lock the kernel for a short period while windows 
// API calls are made. This is to stop a possible deadlock from occurring when 
// a windows thread is suspended while a (windows) synchronization object is held
// and a second thread tries to gain access to that object. Typically this object 
// is declared on the stack - when the constructor is called, the kernel is locked; 
// when the object goes out of scope, the destructor unlocks the kernel.
// Used by the __HOST_LOCK macro.
class THostLock
	{
public:
	THostLock();
	~THostLock();
	void Lock();
	void Unlock();
protected:
	THostLock(TBool aLock);
private:
	TBool iLocked;
	};

// Utility class used to lock the kernel for a short period while windows 
// API calls are made. This is used instead of THostLock for functions
// which are used by both the driver thread and the play thread - 
// if the thread is a windows thread, then the kernel is not locked.
// Used by the __COND_HOST_LOCK macro.
class TCondHostLock : public THostLock
	{
public:
	TCondHostLock();
	void Lock();
	void Unlock();
private:
	TBool iEpocThread;
	};

// Forward declarations
class TWaveformBufMgr;

/**
This the abstraction for a windows waveform audio buffer.
*/
class TWaveformAudioBuf
	{
public:
	TWaveformAudioBuf();
	inline void SetWaveformBufMgr(TWaveformBufMgr* aWaveformBufMgr)
		{iWaveformBufMgr=aWaveformBufMgr;}
	inline void SetBufNum(TInt aBufNum)
		{iBufNum=aBufNum;}
	void Prepare(char* aBufAddr,TInt aBufLength,TInt aDeviceHandle);
	void Unprepare(TInt aDeviceHandle);
private:		
	void DoPrepareOut(HWAVEOUT aPlayDeviceHandle);
	void DoUnprepareOut(HWAVEOUT aPlayDeviceHandle);
	void DoPrepareIn(HWAVEIN aRecordDeviceHandle);
	void DoUnprepareIn(HWAVEIN aRecordDeviceHandle);
public:
	/** The owning waveform audio buffer manager. */
	TWaveformBufMgr* iWaveformBufMgr;
	/** Set when the waveform audio buffer is currently prepared. */
	TBool iIsPrepared;
	/** Set when the waveform audio buffer is involved in an active transfer. */
	TBool iIsInUse;
	/** The header used by windows to identify the waveform audio buffer. */
	WAVEHDR iBufHdr;
	/** A value used to identify a particular waveform audio buffer within an array of these objects. */
	TInt iBufNum;
	/** The transfer ID supplied by the LDD when the buffer is involved in an active transfer. */
	TUint iTransferID;
	friend class TWaveformBufMgr;	
	};

/**
The waveform audio buffer manager. This owns and maintains a set of windows waveform audio buffers which it makes
available to the PDD for data block transfers.
*/	
class TWaveformBufMgr
	{
public:	
	TWaveformBufMgr(TSoundDirection aDirection,TBool aIsHardware);
	~TWaveformBufMgr();
	TInt ReAllocAndUpdate(TSoundSharedChunkBufConfig* aBufConfig,TLinAddr aChunkBase,TInt aDeviceHandle);
	TWaveformAudioBuf* AcquireBuf(char* aStartAddress,TInt aBufLength,TInt aDeviceHandle);
public:
	/** The array of windows waveform audio buffer objects. There is at least one buffer object per buffer within the 
	LDD shared chunk. */
	TWaveformAudioBuf* iWaveformAudioBuf;
	/** The count of the number of audio play buffers in the waveform buffer array. */
	TInt iNumWaveformBufs;
	/** The  default size of each audio buffer in the waveform buffer array. */
	TInt iWaveformBufSize;
	/** List of waveform audio buffer objects waiting to be played/recorded - in FIFO order. Used only when
	no audio hardware is present.*/
	WAVEHDR** iPendingBufList;
	/** Set when no audio hardware is present. */
	TBool iIsHardware;
	/** The direction of the windows waveform audio buffer, record or playback. */
	TSoundDirection iDirection;
	};	
	
/**
The WINS physical device (factory class) for the shared chunk sound driver.
*/	
class DWinsSoundScPddFactory : public DPhysicalDevice
	{
public:
	DWinsSoundScPddFactory();
	~DWinsSoundScPddFactory();
	virtual TInt Install();
	virtual void GetCaps(TDes8 &aDes) const;
	virtual TInt Create(DBase*& aChannel, TInt aUnit, const TDesC8* anInfo, const TVersion &aVer);
	virtual TInt Validate(TInt aUnit, const TDesC8* anInfo, const TVersion &aVer);
private:
	/** The DFC queue (used also by the LDD). */ 
	TDynamicDfcQue* iDfcQ;
	/** DFC for terminating the DFC thread. */
   	static TDfc ExitDfc;
	friend class DWinsSoundScTxPdd;
	friend class DWinsSoundScRxPdd;
	};
	
/**
The WINS physical device driver (PDD) for the playback shared chunk sound driver.
*/
class DWinsSoundScTxPdd : public DSoundScPdd
	{
private:
	enum TThreadCommand {ESendData, EStop, EExit, EPause, EResume};
	enum TCreatePlayDeviceMode {EInit,ESetConfig,EStartTransfer};
public:
	DWinsSoundScTxPdd();
	~DWinsSoundScTxPdd();
	TInt DoCreate(DWinsSoundScPddFactory* aPhysicalDevice);
	// Implementations of the pure virtual functions inherited from DSoundScPdd.
	virtual TDfcQue* DfcQ(TInt aUnit);
	virtual void GetChunkCreateInfo(TChunkCreateInfo& aChunkCreateInfo);
	virtual void Caps(TDes8& aCapsBuf) const;
	virtual TInt MaxTransferLen() const;
	virtual TInt SetConfig(const TDesC8& aConfigBuf);
	virtual TInt SetVolume(TInt aVolume);
	virtual TInt StartTransfer();
	virtual TInt TransferData(TUint aTransferID,TLinAddr aLinAddr,TPhysAddr aPhysAddr,TInt aNumBytes);
	virtual void StopTransfer();
	virtual TInt PauseTransfer();
	virtual TInt ResumeTransfer();
	virtual TInt PowerUp();
	virtual void PowerDown();
	virtual TInt CustomConfig(TInt aFunction,TAny* aParam);
	virtual TInt TimeTransferred(TInt64& aTime, TInt aState);
public:
	void WaveOutProc(WAVEHDR* aHdr);
	void PlayThread();
private:
	void SetCaps();
	TInt OpenWaveOutDevice();
	TInt CreatePlayDevice(TCreatePlayDeviceMode aMode);
	void ClosePlayDevice();
	void PlayThreadCommand(TThreadCommand aCommand,TInt aArg0=0,TInt aArg1=0,TInt aArg2=0);
	TInt ProcessPlayCommand(TThreadCommand aCommand,TInt aArg0,TInt aArg1,TInt aArg2);
	void HandlePlayTimerEvent();
	void HandleTransferComplete();
	void PlayThreadNotifyDriver(TInt aError);
	void StartTimer(WAVEHDR* aBuffer);
	void StopTimer(TBool aCancelAll);
	static void PlayDfc(TAny* aPtr);
private:
	/** A pointer to the PDD factory. */
	DWinsSoundScPddFactory* iPhysicalDevice;
	/** The capabilities of this device. */
	TSoundFormatsSupportedV02 iCaps;
	/** The current audio configuration of this device. */
	TCurrentSoundFormatV02 iSoundConfig;
	/** The current setting for the play volume - a value in the range 0 to 0xFFFF. */
	TInt iVolume;
	/** The driver thread semaphore - used by the windows thread to signal the driver thread. */
	HANDLE iDriverThreadSem;
	/** The handle for the play windows thread. */
	HANDLE iPlayThread;
	/** ETrue if the Windows thread is running, else EFalse.  Used when shutting down to decide whether to
	signal the thread to exit. */
	TBool iPlayThreadRunning;
	/** The play thread mutuex - to serialise acccess to the play thread creation and destruction routines. */
	HANDLE iPlayThreadMutex;
	/** The play thread semaphore - indicates to the windows thread that the driver thread has issued a command. */
	HANDLE iPlayThreadSem;
	/** Semaphore to synchronise between driver thread and windows thread when closing the output device. */
	HANDLE iStopSemaphore;
	/** Semaphore to synchronise between driver thread and windows thread when closing the PDD. */
	HANDLE iDeathSemaphore;
	/** The handle on the waveform output device. */
	HWAVEOUT iPlayDeviceHandle;
	/** Used to transfer commands between the driver thread and the windows thread. */
	TThreadCommand iPlayCommand;
	/** Used to transfer commands between the driver thread and the windows thread. */
	TInt iPlayCommandArg0;
	/** Used to transfer commands between the driver thread and the windows thread. */
	TInt iPlayCommandArg1;
	/** Used to transfer commands between the driver thread and the windows thread. */
	TInt iPlayCommandArg2;
	// The number of outstanding data transfers on the waveform output device. */	
	TInt iPendingPlay;
	/** DFC which handes data block play completion. */
	TDfc iDfc;
	/** A variable used to pass a value from the windows thread to the driver thread. */
	TInt iPlayThreadError;
	/** The windows waveform audio buffer manager. */
	TWaveformBufMgr* iWaveformBufMgr;
	/** A mask used to pass information on transfers that have just completed between the window thread and the 
	driver thread. Updates to this variable typically need to happen atomically. Bit positions are converted 
	into windows waveform audio buffer IDs. */
	TUint32 iCompletedPlayBufHdrMask;
	/** Set when no audio hardware is present. */
	TBool iNoHardware;
	/** The timer event object - indicates the 'no-hardware' timer has gone off. */
	HANDLE iPlayTimerEvent;
	/** The identifier for the current 'no-hardware' timer event. */
	UINT iTimerID;
	/** Indicates whether the 'no-hardware' timer is currently active. */
	TBool iTimerActive;
	/** A variable used to save the play volume setting of the waveform output device at the point when this driver was opened. */
	DWORD iWinWaveVolume;
	/** The number of bytes (not samples) that will be played back per second, at the current sample rate. */
	TUint iBytesPerSecond;
	/** The simulated (ie. no hardware) microseconds played. */
	TInt64 iSimulatedUSecPlayed;
	/** The # of milliseconds that pass per block of data that is played. */
	UINT iSimulatedMsecDuration;
	/** The Windows system timer time at which the last block of data was played. */
	DWORD iLastTimerEventTime;
	/** The Windows system timer time at which playback was paused. */
	DWORD iPauseTime;
	};
		
/**
The WINS physical device driver (PDD) for the record shared chunk sound driver.
*/
class DWinsSoundScRxPdd : public DSoundScPdd
	{
private:
	enum TThreadCommand {ERecData, EStop, EExit, EPause, EResume};	
public:
	DWinsSoundScRxPdd();
	~DWinsSoundScRxPdd();
	TInt DoCreate(DWinsSoundScPddFactory* aPhysicalDevice);
	// Implementations of the pure virtual functions inherited from DSoundScPdd.
	virtual TDfcQue* DfcQ(TInt aUnit);
	virtual void GetChunkCreateInfo(TChunkCreateInfo& aChunkCreateInfo);
	virtual void Caps(TDes8& aCapsBuf) const;
	virtual TInt MaxTransferLen() const;
	virtual TInt SetConfig(const TDesC8& aConfigBuf);
	virtual TInt SetVolume(TInt aVolume);
	virtual TInt StartTransfer();
	virtual TInt TransferData(TUint aTransferID,TLinAddr aLinAddr,TPhysAddr aPhysAddr,TInt aNumBytes);
	virtual void StopTransfer();
	virtual TInt PauseTransfer();
	virtual TInt ResumeTransfer();
	virtual TInt PowerUp();
	virtual void PowerDown();
	virtual TInt CustomConfig(TInt aFunction,TAny* aParam);
	virtual TInt TimeTransferred(TInt64& aTime, TInt aState);
public:
	void WaveInProc(WAVEHDR* aHdr);
	void RecordThread();
private:
	void SetCaps();
	TInt OpenWaveInDevice();
	TInt CreateRecordDevice(TBool aCheckDevice=EFalse);
	void CloseRecordDevice();	
	void RecordThreadCommand(TThreadCommand aCommand,TInt aArg0=0,TInt aArg1=0,TInt aArg2=0);
	TInt ProcessRecordCommand(TThreadCommand aCommand,TInt aArg0,TInt aArg1,TInt aArg2);
	void HandleRecordTimerEvent();
	void HandleTransferComplete();
	void RecordThreadNotifyDriver(TInt aError);
	void StartTimer(WAVEHDR* aBuffer);
	void StopTimer(TBool aCancelAll);
	static void RecordDfc(TAny* aPtr);
private:
	/** A pointer to the PDD factory. */
	DWinsSoundScPddFactory* iPhysicalDevice;
	/** The capabilities of this device. */
	TSoundFormatsSupportedV02 iCaps;
	/** The current audio configuration of this device. */
	TCurrentSoundFormatV02 iSoundConfig;
	/** The driver thread semaphore - used by the windows thread to signal the driver thread. */
	HANDLE iDriverThreadSem;
	/** The handle for the record windows thread. */
	HANDLE iRecordThread;
	/** ETrue if the Windows thread is running, else EFalse.  Used when shutting down to decide whether to
	signal the thread to exit. */
	TBool iRecordThreadRunning;
	/** The record thread mutuex - to serialise acccess to the record thread creation and destruction routines. */
	HANDLE iRecordThreadMutex;
	/** The record thread semaphore - indicates to the windows thread that the driver thread has issued a command. */
	HANDLE iRecordThreadSem;
	/** Semaphore to synchronise between driver thread and windows thread when closing the input device. */
	HANDLE iStopSemaphore;
	/** Semaphore to synchronise between driver thread and windows thread when closing the PDD. */
	HANDLE iDeathSemaphore;
	/** The handle on the waveform input device. */
	HWAVEIN iRecordDeviceHandle;
	/** Used to transfer commands between the driver thread and the windows thread. */
	TThreadCommand iRecordCommand;
	/** Used to transfer commands between the driver thread and the windows thread. */
	TInt iRecordCommandArg0;
	/** Used to transfer commands between the driver thread and the windows thread. */
	TInt iRecordCommandArg1;
		/** Used to transfer commands between the driver thread and the windows thread. */
	TInt iRecordCommandArg2;
	// The number of outstanding data transfers on the waveform input device. */	
	TInt iPendingRecord;
	/** DFC which handes data block record completion. */
	TDfc iDfc;
	/** A variable used to pass a value from the windows thread to the driver thread. */
	TInt iRecordThreadError;
	/** The windows waveform audio buffer manager. */
	TWaveformBufMgr* iWaveformBufMgr;
	/** A mask used to pass information on transfers that have just completed between the window thread and the 
	driver thread. Updates to this variable typically need to happen atomically. Bit positions are converted 
	into windows waveform audio buffer IDs. */
	TUint32 iCompletedRecordBufHdrMask;
	/** Indicates when record mode is enabled. */
	TBool iRecordEnabled;
	/** Set when no audio hardware is present. */
	TBool iNoHardware;
	/** The timer event object - indicates the 'no-hardware' timer has gone off. */
	HANDLE iRecordTimerEvent;
	/** The identifier for the current 'no-hardware' timer event. */
	UINT iTimerID;
	/** Indicates whether the 'no-hardware' timer is currently active. */
	TBool iTimerActive;
	/** The number of bytes (not samples) that will be recorded back per second, at the current sample rate. */
	TUint iBytesPerSecond;
	/** The number of bytes recorded before the last pause command (when we pause the windows byte
		count is reset so we need to add this on for the TimeRecorded API). */
	TUint iBytesRecordedBeforeLastPause;
	/** The number of recorded bytes reported to the LDD. Subtracted from the windows recorded byte count 
		to calculate the last, partial buffer, transfer size */
	TUint iBytesSincePauseReportedToLdd;
	/** The # of milliseconds that pass per block of data that is recorded. */
	UINT iSimulatedMsecDuration;
	/** The Windows system timer time at which the last block of data was recorded. */
	DWORD iLastTimerEventTime;
	};	

#define __HOST_LOCK			THostLock lock
#define __HOST_LOCK_ON		lock.Lock();
#define __HOST_LOCK_OFF		lock.Unlock();
#define __COND_HOST_LOCK	TCondHostLock lock
	
#endif /* __WINSSOUNDSC_H__ */
