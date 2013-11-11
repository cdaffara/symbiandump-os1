// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\lffsdev.cpp
// 
//

#include <flash_media.h>
#include <emulator.h>
#include <property.h>

#define FLASH_FAULT()	Kern::Fault("LFFSDEV",__LINE__)

//#define M18_EMULATION // Test for Control Mode operation

/********************************************
 * Driver definitions
 ********************************************/

/** LFFS image name */
const CHAR KLfsFileName[] = "LFSLDRV.BIN";


//-- default values for LFFS related entries in "epoc.ini" file
const TInt      KDefaultFlashSize = 0x400000;   //-- "FlashSize"        entry, default 4MB
const TInt      KEraseBlockSize   = 0x20000;	//-- "FlashEraseSize"   entry, default 128KB
const TUint32   KFlashEraseTime   =	1000000;	//-- "FlashEraseTime"   entry, default 1s
const TInt      KWriteBlockSize   = 64;         //-- "FlashWriteSize"   entry
const TInt      KFlashWriteTime   = 406;		//-- "FlashWriteTime"   entry, default 406us
const TInt      KResumeTime       = 5000;		//-- "FlashResumeTime"  entry, default 5ms

//-- other possible LFFS related entries in "epoc.ini" file:
//-- "FlashHoldOffTime"   default value = iEraseTime/10

//-- "FlashForceImgMount" default value = 0. If not 0 LFFS image will be mounted as it is, even if it doesn't have TLfsParams structure in the end.
//-- Moreover, it won't be zero filled and TLfsParams structure won't be written at the end of the image file.
//-- shall be useful for dealing with real images from the real devices


/** 
    This cunning structure is supposed to be at the very end of the LFFS image file. If it is not foung there, 
    the image gets zero-filled (depends on "FlashForceImgMount" flag in epoc.ini). 
*/

struct TLfsParams
	{
	TInt iEraseSize;
	};

#ifdef _DEBUG
/***************************************************
 * ControlIO command types - for debug builds, only
 ***************************************************/

enum TCtrlIoTypes
	{
	//KCtrlIoRww=0,
	KCtrlIoTimeout=1
	};
#endif


const TInt KDataBufSize=1024;

/********************************************
 * Media driver class
 ********************************************/
class DMediaDriverFlashWin32 : public DMediaDriverFlash
	{
public:
	enum TState
		{
		EIdle=0,
		EWriting=1,
		EEraseNoSuspend=2,
		EErase=3,
		ESuspendPending=4,
		ESuspending=5,
		ESuspended=6,
		EErasePending=7
		};
public:
	DMediaDriverFlashWin32(TInt aMediaId);
public:
	// replacing pure virtual - FLASH device specific stuff
	virtual TInt Initialise();
	virtual TUint32 EraseBlockSize();
	virtual TUint32 TotalSize();
	virtual TInt DoRead();
	virtual TInt DoWrite();
	virtual TInt DoErase();
	virtual TInt Caps(TLocalDriveCapsV2& caps);
public:
	void HandleEvent();
	void StartTimer(TInt aMicros);
	void StartErase();
	void SuspendErase();
	void CompleteErase();
	void CompleteWrite();
	void CompleteSuspend();
	void StartPendingRW();
	void ReadFlashParameters();
public:
	static void TimerFn(TAny* aPtr);
	static void EventDfc(TAny* aPtr);
#ifdef _DEBUG
public:
    enum TCtrlIoState {/*EIdle=0,ECtrlIoWaitWr=1,ECtrlIoWaitRd=2,ECtrlIoWrActive=3,*/ECtrlIoTimeOutPrep=4};
    TInt ControlIO(TInt aCommand,TAny* aParam1,TAny* /*aParam2*/);
	TInt Request(TLocDrvRequest& m);
#endif
public:
	TState iState;
	
	TInt iSize;
	TInt iEraseBlockSize;
	TInt iEraseTime;
	TInt iSuspendHoldOffTime;
	TInt iResumeTime;
	TInt iWriteBlockSize;
	TInt iWriteTime;
    
	HANDLE iFile;
	HANDLE iMapping;
	TUint8* iBase;
	TUint8* iData;		// data being written
	
	NTimer iTimer;
	TDfc iEventDfc;
	TUint32 iTickPeriod;
	TUint32 iEraseCounter;
	TUint32 iErasePos;
	TInt iTimerExtra;

#ifdef _DEBUG
public:
    TUint8 iCtrlIoState;
#endif
	};

DMediaDriverFlashWin32::DMediaDriverFlashWin32(TInt aMediaId)
	:	DMediaDriverFlash(aMediaId),
		iTimer(&TimerFn,this),
		iEventDfc(&EventDfc,this,NULL,2),
		iTickPeriod(NKern::TickPeriod())
	{
	// iState=EIdle;
#ifdef _DEBUG
    //iCtrlIoState=EIdle;
#endif

	}

void DMediaDriverFlashWin32::ReadFlashParameters()
//
// Read the flash parameters from the ini file, or use defaults
//
	{
   
	iSize = Property::GetInt("FlashSize", KDefaultFlashSize);

	TInt nblocks = Property::GetInt("FlashEraseBlocks", 0);
	if (nblocks == 0)
		nblocks = iSize/Property::GetInt("FlashEraseSize", KEraseBlockSize);

	iEraseBlockSize = iSize / nblocks;
	__ASSERT_ALWAYS(iEraseBlockSize * nblocks == iSize, FLASH_FAULT());
	__ASSERT_ALWAYS((iEraseBlockSize & (iEraseBlockSize-1)) == 0, FLASH_FAULT());

	iEraseTime = Property::GetInt("FlashEraseTime", KFlashEraseTime);
	
	iSuspendHoldOffTime = Property::GetInt("FlashHoldOffTime", iEraseTime/10);
	iResumeTime = Property::GetInt("FlashResumeTime", KResumeTime);
	iWriteBlockSize = Property::GetInt("FlashWriteSize", KWriteBlockSize);
	__ASSERT_ALWAYS((iWriteBlockSize & (iWriteBlockSize-1)) == 0, FLASH_FAULT());

	iWriteTime = Property::GetInt("FlashWriteTime", KFlashWriteTime);
	
	}

TInt DMediaDriverFlashWin32::Initialise()
	{
	iEventDfc.SetDfcQ(iPrimaryMedia->iDfcQ);
	iData=(TUint8*)Kern::Alloc(KDataBufSize);
	if (!iData)
		return KErrNoMemory;

	ReadFlashParameters();

	// locate/open the file that models the flash
	CHAR filename[MAX_PATH];
	strcpy(filename, Property::GetString("EmulatorMediaPath"));
	if (!Emulator::CreateAllDirectories(filename))
		return Emulator::LastError();
	strcat(filename, KLfsFileName);
	
	iFile = CreateFileA(filename, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL);
	if (iFile == INVALID_HANDLE_VALUE)
		return Emulator::LastError();

	TLfsParams p;
	p.iEraseSize = -1;
	DWORD bytes;
	TBool valid;
	
    //-- try to read TLfsParams structure from the end of the image file.
	if (SetFilePointer(iFile, iSize, NULL, FILE_BEGIN) != -1
		&& ReadFile(iFile, &p, sizeof(TLfsParams), &bytes, NULL)
		&& p.iEraseSize == iEraseBlockSize)
	{
        valid = ETrue; //-- read it OK.
	}
	else
	{//-- couldn't read TLfsParams structure from the end of the image file.
     //-- if "FlashForceImgMount" parameter from epoc.ini is 0 or not present,
     //-- zero-fill the lffs image file and write TLfsParams structure at the end of the file.

		const TInt forceImgMount = Property::GetInt("FlashForceImgMount", 0);
        if(!forceImgMount)
		{
		p.iEraseSize = iEraseBlockSize;
		if (SetFilePointer(iFile,iSize, NULL, FILE_BEGIN) == -1
			|| !WriteFile(iFile, &p, sizeof(p), &bytes, NULL)
			|| !SetEndOfFile(iFile))
			return Emulator::LastError();
		    
		valid = EFalse;
		}
        else
        {//-- mount LFFS image forcingly.
            valid = ETrue;
        }
	}

	iMapping = CreateFileMappingA(iFile, NULL, PAGE_READWRITE, 0, iSize, NULL);
	if (iMapping == NULL)
		return Emulator::LastError();

	iBase = (TUint8*)MapViewOfFile(iMapping, FILE_MAP_WRITE, 0, 0, iSize);
	if (iBase == NULL)
		return Emulator::LastError();

	//-- zero-fill media image it doesn't contain TLfsParams data at the very end.
	if (!valid)
	    {
		memclr(iBase, iSize);
        }

	return KErrNone;
	}

TUint32 DMediaDriverFlashWin32::EraseBlockSize()
	{
	return iEraseBlockSize;
	}

TUint32 DMediaDriverFlashWin32::TotalSize()
	{
	return iSize;
	}

TInt DMediaDriverFlashWin32::DoRead()
	{
	if (iWriteReq)
		return 1;	// write in progress so defer read
	
	__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:DoRead"));

	if (iState==EErasePending)
		{
		iTimer.Cancel();
		iState = ESuspended;
		}
	if (iState==EIdle || iState==ESuspended)
		{
		// can do the read now
		TInt pos=(TInt)iReadReq->Pos();
		TInt len=(TInt)iReadReq->Length();

		TPtrC8 des(iBase+pos,len);
		TInt r=iReadReq->WriteRemote(&des,0);
		Complete(EReqRead,r);
		if (iState==ESuspended)
			StartErase();
		}
	else if (iState==EErase)
		{
		// erase in progress - suspend it
		SuspendErase();
		}
	else if (iState==EEraseNoSuspend)
		iState=ESuspendPending;
	// wait for suspend to complete
	return KErrNone;
	}

TInt DMediaDriverFlashWin32::DoWrite()
	{
	if (iReadReq)
		return 1;	// read in progress so defer write

	if (iState==EErasePending)
		{
		iTimer.Cancel();
		iState = ESuspended;
		}
	if (iState==EIdle || iState==ESuspended)
		{
		// can start the write now
		__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:Write Pos=%08x Length=%08x RemDesOff=%08x",
											(TInt)iWriteReq->Pos(),(TInt)iWriteReq->Length(),iWriteReq->RemoteDesOffset()));
		if (iState==EIdle)
			iState=EWriting;
		TInt pos = (TInt)iWriteReq->Pos();
		TInt end = pos + (TInt)iWriteReq->Length();
		pos &= ~(iWriteBlockSize-1);
		end = (end + iWriteBlockSize-1) & ~(iWriteBlockSize-1);
		StartTimer(((end-pos)/iWriteBlockSize) * iWriteTime);
		}
	else if (iState==EErase)
		{
		// erase in progress - suspend it
		SuspendErase();
		}
	else if (iState==EEraseNoSuspend)
		iState=ESuspendPending;
	// wait for suspend to complete
	return KErrNone;
	}

void DMediaDriverFlashWin32::CompleteWrite()
//
// Do the actual write in the completion
// Transfer data in blocks from the client and AND it to the 'flash'
//
	{
	__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:WriteComplete"));

	TInt r = KErrNone;
	TUint8* flash = iBase + (TInt)iWriteReq->Pos();
	TInt len = (TInt)iWriteReq->Length();
	TInt offset = 0;
	while (len > 0)
		{
		TInt size = Min(len, KDataBufSize);
		TPtr8 des(iData,size);
		r = iWriteReq->ReadRemote(&des, offset);
		if (r!=KErrNone)
			break;
		len -= size;
		offset += size;
		const TUint8* ptr = iData;
		do
			{
			*flash++ &= *ptr++;
			} while (--size > 0);
		}

	if (iState == EWriting)
		iState = EIdle;
	Complete(EReqWrite,r);
	if (iState == ESuspended)
		StartErase();
	}

TInt DMediaDriverFlashWin32::DoErase()
	{
	if (iReadReq || iWriteReq)
		return 1;		// read or write in progress so defer this request
	TInt pos=(TUint32)iEraseReq->Pos();
	TInt len=(TUint32)iEraseReq->Length();
	__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:DoErase %d@%08x",len,pos));
	if (len!=iEraseBlockSize)
		return KErrArgument;	// only allow single-block erase
	if (pos & (iEraseBlockSize-1))
		return KErrArgument;	// start position must be on erase block boundary
	__ASSERT_ALWAYS(iState==EIdle,FLASH_FAULT());
	iErasePos=pos;
	StartErase();
	return KErrNone;
	}

void DMediaDriverFlashWin32::StartTimer(TInt aMicros)
	{
	aMicros += iTimerExtra - (iTickPeriod>>1);
	if (aMicros < 0)
		{
		iTimerExtra = aMicros + (iTickPeriod>>1);
		iEventDfc.Enque();		// go off 'immediately'
		}
	else
		{
		iTimerExtra = 0;
		iTimer.OneShot(aMicros / iTickPeriod);
		}
	}

void DMediaDriverFlashWin32::TimerFn(TAny* aPtr)
	{
	((DMediaDriverFlashWin32*)aPtr)->iEventDfc.Add();
	}

void DMediaDriverFlashWin32::EventDfc(TAny* aPtr)
	{
	((DMediaDriverFlashWin32*)aPtr)->HandleEvent();
	}

void DMediaDriverFlashWin32::HandleEvent()
	{
	__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:Event %d", iState));
	switch (iState)
		{
	case ESuspended:
	case EWriting:	// write completed
		{
#ifdef _DEBUG
		if(iCtrlIoState==ECtrlIoTimeOutPrep)
			{
			iState=EIdle;
			iCtrlIoState=EIdle;
			Complete(EReqWrite,KErrNotReady);
			}
		else
#endif
			CompleteWrite();
		break;
		}
	case EEraseNoSuspend:
		{
#ifdef _DEBUG
		if(iCtrlIoState==ECtrlIoTimeOutPrep)
			{
			iState=EIdle;
			iCtrlIoState=EIdle;
			Complete(EReqErase,KErrNotReady);
			}
		else
			{
#endif
		TInt remain = iEraseCounter - NKern::TickCount();
		if (remain <= 0)
			CompleteErase();
		else
			{
			iState=EErase;
			StartTimer(remain * iTickPeriod);
			}
#ifdef _DEBUG
			}
#endif
		break;
		}
	case EErasePending:
		StartErase();
		break;
	case EErase:	// erase completed
		CompleteErase();
		break;
	case ESuspendPending:
		if (TInt(iEraseCounter - NKern::TickCount()) <= 0)
			CompleteErase();
		else
			SuspendErase();
		break;
	case ESuspending:
		CompleteSuspend();
		break;
	default:
		__KTRACE_OPT(KPANIC,Kern::Printf("iState=%d",iState));
		FLASH_FAULT();
		}
	}

void DMediaDriverFlashWin32::StartErase()
//
// Continue an erase - iEraseCounter has the remaining time for the erase
//
	{
	__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:StartErase %08x",iBase+iErasePos));
	switch (iState)
		{
	case ESuspended:
		iState = EErasePending;
		StartTimer(iResumeTime);
		break;
	case EIdle:	// starting to erase
		iEraseCounter = iEraseTime;
	case EErasePending:
		{
		iState = EEraseNoSuspend;
		TUint32 remain = iEraseCounter;
		iEraseCounter = NKern::TickCount() + remain/iTickPeriod;
		StartTimer(Min(remain, iSuspendHoldOffTime));
		}
		break;
	default:
		__KTRACE_OPT(KPANIC,Kern::Printf("iState=%d",iState));
		FLASH_FAULT();
		}
	}

void DMediaDriverFlashWin32::SuspendErase()
	{
	__ASSERT_ALWAYS(iState==EErase || iState==ESuspendPending,FLASH_FAULT());
	__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:SuspendErase %08x",iBase+iErasePos));
	iTimer.Cancel();
	TInt remain = Max(0, TInt(iEraseCounter - NKern::TickCount()));
	iEraseCounter = remain * iTickPeriod;
	iState = ESuspending;
	StartTimer(0);
	}

void DMediaDriverFlashWin32::CompleteSuspend()
	{
	// erase suspend completion
	__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:SuspendComplete"));

	iState = ESuspended;
	// start any pending read or write requests
	StartPendingRW();
	}

void DMediaDriverFlashWin32::CompleteErase()
//
// Do the actual erase in the completion
//
	{
	// erase completion
	__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:EraseComplete"));

	memset(iBase + iErasePos, 0xff, iEraseBlockSize);

	// complete the erase request
	iState = EIdle;
	Complete(EReqErase,KErrNone);

	// start any pending read or write requests
	StartPendingRW();
	}


void DMediaDriverFlashWin32::StartPendingRW()
	{
	// start any pending read or write requests
	if (iReadReq)
		DoRead();
	if (iWriteReq)
		DoWrite();
	}

#ifdef _DEBUG
// Override the base class version in order to provide access to ControlIO
//
TInt DMediaDriverFlashWin32::Request(TLocDrvRequest& m)
	{
	TInt r;
	TInt id=m.Id();
	__KTRACE_OPT(KLOCDRV,Kern::Printf(">DMediaDriverFlashWin32::Request %d",id));
	if (id!=DLocalDrive::EControlIO)
		{
		r=DMediaDriverFlash::Request(m);
		return r;
		}
	r=ControlIO((TInt)m.iArg[0],m.iArg[1],m.iArg[2]);
	DMediaDriver::Complete(m,r);
	return r;
	}

TInt DMediaDriverFlashWin32::ControlIO(TInt aCommand,TAny* /*aParam1*/,TAny* /*aParam2*/)
	{
	switch (aCommand)
		{
		case(KCtrlIoTimeout):
			{
			__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash:DoControlIO invoked for KCtrlIoTimeout"));
			// The aim of this test is simulate a flash timeout (and so exercise the consequent
			// actions of the software that initiated the request)				
			if(iCtrlIoState!=EIdle)
				{
				__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash: ControlIO request before previous completed"));
				return KErrServerBusy;
				}
			else
				{
				__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash: ControlIO timeout initiated"));
				iCtrlIoState=ECtrlIoTimeOutPrep;
				}
			break;
			}
		
		default:
			{
			__KTRACE_OPT(KLOCDRV,Kern::Printf("Flash: ERROR - unrecognised ControlIO command %d",aCommand));
			FLASH_FAULT();
			break;
			}
		}
	return KErrNone;
	}
	
#endif

TInt DMediaDriverFlashWin32::Caps(TLocalDriveCapsV2& aCaps)
// On return, aCaps data contains capability information about the 
// flash device, in the form of a class derived from TLocalDriveCapsV2. 
// The size of the derived class should not exceed KMaxLocalDriveCapsLength 
// which is defined and used in e32\drivers\locmedia\locmedia.cpp. If a 
// larger sized capabilities class is used, and this code is modified to 
// write to member data beyond KMaxLocalDriveCapsLength this will cause a 
// fault.
	{
	// Invoke the base class method then update the sizes for
	// Control Mode and Object Mode as required.
	DMediaDriverFlash::Caps(aCaps);
#if defined (M18_EMULATION)
	TLocalDriveCapsV7* caps = &((TLocalDriveCapsV7&)(aCaps));
	caps->iControlModeSize=16;
	caps->iObjectModeSize=1024;
    __KTRACE_OPT( KLOCDRV, Kern::Printf("MLFS: ) ControlModeSize UPDATED as=0x%x", caps->iControlModeSize) );
    __KTRACE_OPT( KLOCDRV, Kern::Printf("MLFS: ) ObjectModeSize UPDATED as=0x%x", caps->iObjectModeSize) );
#endif
	return KErrCompletion;	// synchronous completion
	}

DMediaDriverFlash* DMediaDriverFlash::New(TInt aDevice)
	{
	return new DMediaDriverFlashWin32(aDevice);
	}




