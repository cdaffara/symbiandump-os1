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
// D_MEDNAND.CPP
// 
//

#define _MEDIA_DRIVER
#include <nand_media.h>
#include <emulator.h>
#include <property.h>

/********************************************
 * Driver definitions
 ********************************************/
const TInt KSimulatedDeviceId = 0xEC73;
const CHAR KNandFileName[] = "NANDDRV.BIN";

//
// Platform dependent media driver class
//
class DMediaDriverNandWin32 : public DMediaDriverNand
	{
public:
	DMediaDriverNandWin32(TMediaDevice aDevice);

	// replacing pure virtual - NAND device specific stuff
	virtual TInt Initialise();
	virtual TInt GetDeviceId(TUint8& aDeviceId, TUint8& aManufacturerId);
	virtual TInt DeviceRead(const TUint aPageAddress, TAny* aBuf, const TUint aLength);
	virtual TInt DeviceWrite(const TUint aPageAddress, TAny* aBuf, const TUint aLength);
	virtual TInt DeviceErase(const TUint aBlockAddress);
	virtual TInt DeviceClose();

public:
	HANDLE iFile;
	HANDLE iMapping;
	TUint8* iBase;
	};

LOCAL_C TInt MapLastError()
//
// Map an NT error to an Epoc/32 error.
//
	{
	__PATH_NOT_YET_TESTED;
	DWORD r=GetLastError();
	TInt res;
	switch (r)
		{
	    case ERROR_SHARING_VIOLATION : res=KErrAccessDenied; break;
	    case ERROR_LOCK_VIOLATION : res=KErrLocked; break;
	    case ERROR_FILE_NOT_FOUND: res=KErrNotFound; break;
	    case ERROR_PATH_NOT_FOUND: res=KErrPathNotFound; break;
	    case ERROR_ALREADY_EXISTS:
	    case ERROR_FILE_EXISTS:
            res=KErrAlreadyExists;
		    break;
	    case ERROR_NOT_READY: res=KErrNotReady; break;
	    case ERROR_UNRECOGNIZED_VOLUME:
	    case ERROR_NOT_DOS_DISK:
		    res=KErrUnknown;
		    break;
	    case ERROR_UNRECOGNIZED_MEDIA: res=KErrCorrupt; break;
	    case ERROR_INVALID_NAME: res=KErrBadName; break;
	    case ERROR_NO_MORE_FILES: res=KErrEof; break;
		case ERROR_DISK_FULL: res=KErrDiskFull; break;
		default: res=KErrGeneral;
		}
	return(res);
	}

TInt DMediaDriverNandWin32::Initialise()
//
// Create the simulated NAND file if necessary and
// map it into memory.
//
	{
	__PATH_TESTED;

	// locate/open the file that models the flash
	CHAR filename[MAX_PATH];
	strcpy(filename, Property::GetString("EmulatorMediaPath"));
	if (!Emulator::CreateAllDirectories(filename))
		{
		__PATH_NOT_YET_TESTED;
		return Emulator::LastError();
		}
	strcat(filename, KNandFileName);
	iFile = CreateFileA(filename, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);

	if (iFile == INVALID_HANDLE_VALUE)
		{
		__PATH_NOT_YET_TESTED;
		return Emulator::LastError();
		}

	const TUint32 flashSizeInBytes = iNumBlocks * iNumPagesPerBlock * (iNumBytesMain + iNumBytesSpare);

	SetFilePointer(iFile, flashSizeInBytes, NULL, FILE_BEGIN);
	SetEndOfFile(iFile);
	
	iMapping = CreateFileMappingA(iFile, NULL, PAGE_READWRITE, 0, flashSizeInBytes, NULL);
	if (iMapping == NULL)
		{
		__PATH_NOT_YET_TESTED;
		return Emulator::LastError();
		}

	iBase = (TUint8*)MapViewOfFile(iMapping, FILE_MAP_WRITE, 0, 0, flashSizeInBytes);
	if (iBase == NULL)
		{
		__PATH_NOT_YET_TESTED;
		return Emulator::LastError();
		}

	return KErrNone;
	}

TInt DMediaDriverNandWin32::GetDeviceId(TUint8& aDeviceId, TUint8& aManufacturerId)
	{
	__PATH_TESTED;
	
	aManufacturerId = (KSimulatedDeviceId & 0xFF00) >> 8;
	aDeviceId       = (KSimulatedDeviceId & 0xFF);
	return KErrNone;
	}

TInt DMediaDriverNandWin32::DeviceRead(const TUint aPageAddress, TAny* aBuf, const TUint aLength)
	{
	__PATH_TESTED;

	__NAND_ASSERT(aPageAddress < (iNumBlocks * iNumPagesPerBlock));
	__NAND_ASSERT(   (aLength == iNumBytesMain)
				  || (aLength == iNumBytesMain + iNumBytesSpare)
				  || (aLength == iNumBytesSpare)				 );
	
#ifdef __TEST_MEDIA_DRIVER__
		extern TBool gNoPower;
		if(gNoPower)
			return KErrBadPower;
#endif

	TUint8* readPoint;
	if (aLength == iNumBytesMain)
		{
		__PATH_TESTED;
		readPoint = iBase + (aPageAddress * (iNumBytesMain + iNumBytesSpare));
		}
	else if (aLength == iNumBytesSpare)
		{
		__PATH_TESTED;
		readPoint = iBase + (aPageAddress * (iNumBytesMain + iNumBytesSpare)) + iNumBytesMain;
		}
	else if (aLength == (iNumBytesMain + iNumBytesSpare))
		{
		__PATH_TESTED;
		readPoint = iBase + (aPageAddress * (iNumBytesMain + iNumBytesSpare));
		}
	else
		{
		__PATH_NOT_YET_TESTED;
		return KErrArgument;
		}

	for (TUint i = 0; i < aLength; i++)
		{
		((TUint8*)aBuf)[i] = readPoint[i];
		}

#ifdef __TEST_MEDIA_DRIVER__
	extern TBool gECCFail;
		if(gECCFail)
		{
			//introduce a random one bit error
			TUint8* p=(TUint8*)aBuf;
			TUint32 key=Kern::Random() % aLength;
			TUint32 shift=1<<(Kern::Random() % 8);
			p[key]=(TUint8)(p[key]&shift?p[key]-shift:p[key]+shift);
			gECCFail=EFalse;
		}
#endif

	return KErrNone;
	}

#ifdef __TEST_MEDIA_DRIVER__
extern "C" TUint32 gbbm_get_sphy_blknum(TUint32, TUint32);
#endif
TInt DMediaDriverNandWin32::DeviceWrite(const TUint aPageAddress, TAny* aBuf, const TUint aLength)
	{
	__PATH_TESTED;

	__NAND_ASSERT(aPageAddress < (iNumBlocks * iNumPagesPerBlock));
	__NAND_ASSERT(   (aLength == iNumBytesMain)
				  || (aLength == iNumBytesMain + iNumBytesSpare)
				  || (aLength == iNumBytesSpare)				 );

#ifdef __TEST_MEDIA_DRIVER__
		extern TBool gNoPower;
		extern TBool gPowerFail;
		extern TUint32 gPowerFailAfter;
		if(gNoPower)
			return KErrBadPower;
		if(gPowerFail)
			{
			if(gPowerFailAfter)
				--gPowerFailAfter;
			else
				{
				gPowerFail=EFalse;
				gNoPower=ETrue;
				extern TUint32 gNotificationType;
				extern TUint32 KNandDbgNotifyPowerDown;
				extern TInt NotifyThread();
				if(gNotificationType==KNandDbgNotifyPowerDown)
					NotifyThread();
				return KErrBadPower;
				}
			}
#endif
	
	TUint8* writePoint;
	
	if (aLength == iNumBytesMain)
		{
		__PATH_TESTED;
		writePoint = iBase + (aPageAddress * (iNumBytesMain + iNumBytesSpare));
		}
	else if (aLength == iNumBytesSpare)
		{
		__PATH_TESTED;
		writePoint = iBase + (aPageAddress * (iNumBytesMain + iNumBytesSpare)) + iNumBytesMain;
		}
	else if (aLength == (iNumBytesMain + iNumBytesSpare))
		{
		__PATH_TESTED;
		writePoint = iBase + (aPageAddress * (iNumBytesMain + iNumBytesSpare));
		}
	else
		{
		__PATH_NOT_YET_TESTED;
		return KErrArgument;
		}
	
	for (TUint i = 0; i < aLength; i++)
		{
		writePoint[i] &= ((TUint8*)aBuf)[i];
		}

#ifdef __TEST_MEDIA_DRIVER__
		extern TUint32 gFailAfter;
		extern TUint32 gFailAfterCnt;
		extern TBool gFailureType; //true-WriteFail, false-EraseFail
		extern TBool gFailON;
		if(gFailON && gFailureType)
			{
			if(gFailAfterCnt)
				--gFailAfterCnt;
			else
				{
				extern TUint32 FirstReadPUN;
				extern DMediaDriverNand* gMediaDriverPtr;		
				if(gbbm_get_sphy_blknum(0,aPageAddress/gMediaDriverPtr->iNumPagesPerBlock)==FirstReadPUN)
					{
					gFailAfterCnt=gFailAfter;
					//notify failure
					extern TUint32 gNotificationType;
					extern TUint32 KNandDbgNotifyWriteFail;
					extern TInt NotifyThread();
					if(gNotificationType==KNandDbgNotifyWriteFail)
						NotifyThread();

					return KErrCorrupt;
					}
				}
			}
#endif

	return KErrNone;
	}

TInt DMediaDriverNandWin32::DeviceErase(const TUint aBlockAddress)
	{
	__PATH_TESTED;
	
	__NAND_ASSERT(aBlockAddress < iNumBlocks);
#ifdef __TEST_MEDIA_DRIVER__
		extern TBool gNoPower;
		extern TBool gPowerFail;
		extern TUint32 gPowerFailAfter;
		if(gNoPower)
			return KErrBadPower;
		if(gPowerFail)
			{
			if(gPowerFailAfter)
				--gPowerFailAfter;
			else
				{
				gPowerFail=EFalse;
				gNoPower=ETrue;
				extern TUint32 gNotificationType;
				extern TUint32 KNandDbgNotifyPowerDown;
				extern TInt NotifyThread();
				if(gNotificationType==KNandDbgNotifyPowerDown)
					NotifyThread();
				return KErrBadPower;
				}
			}
#endif
	
	const TUint bytesPerEraseBlock = iNumPagesPerBlock * (iNumBytesMain + iNumBytesSpare);
	const TUint erasePos = aBlockAddress * bytesPerEraseBlock;
	
	TUint8* temp = iBase + erasePos;
	for (TUint i=0; i < bytesPerEraseBlock; i++)
		{
		temp[i]=0xFF;
		}
	return KErrNone;
	}

TInt DMediaDriverNandWin32::DeviceClose()
	{
	__PATH_NOT_YET_TESTED;
	TBool bRtn=UnmapViewOfFile(iBase);
	if(!bRtn)
		{
		__PATH_NOT_YET_TESTED;
		return(MapLastError());
		}

	bRtn=CloseHandle(iMapping);
	if (!bRtn)
		{
		__PATH_NOT_YET_TESTED;
		return(MapLastError());
		}

	bRtn=CloseHandle(iFile);
	if (!bRtn)
		{
		__PATH_NOT_YET_TESTED;
		return(MapLastError());
		}

	return KErrNone;
	}

DMediaDriverNandWin32::DMediaDriverNandWin32(TMediaDevice aDevice)
	:	DMediaDriverNand(aDevice)
	{
	__PATH_TESTED;
	}

DMediaDriverNand* DMediaDriverNand::New(TMediaDevice aDevice)
	{
	__PATH_TESTED;
	return new DMediaDriverNandWin32(aDevice);
	}

#ifdef __USE_CUSTOM_ALLOCATOR
/**	
	Concrete implementation of the NAND buffer allocator class
		- Uses Kern::Alloc based allocation

	@internalTechnology
 */
class TNandAllocatorWins : public TNandAllocatorBase
	{
public:
	virtual TInt AllocateBuffers(SBufferInfo& aBufferInfo);	// Initialise the allocator
	};

/**	Concrete implementation of the NAND buffer allocator class
	Creates a customallocator (using Kern::Alloc)

	@param aBufferInfo A reference to a SBufferInfo containing the details of the PSL allocated buffer
	@return Standard Symbian OS error code
	@see TNandAllocatorBase::SCellInfo
 */
TInt TNandAllocatorWins::AllocateBuffers(SBufferInfo& aBufferInfo)
	{
	const TUint32 KNumBuffers		= 4;
	const TUint32 KMaxNumBytesSpare = 16;
	const TUint32 KMaxNumBytesMain	= 512;
	const TUint32 KMaxNumBytesPage	= KMaxNumBytesSpare + KMaxNumBytesMain;

	aBufferInfo.iBytesPerSpareArray = KMaxNumBytesSpare;
	aBufferInfo.iBytesPerMainArray = KMaxNumBytesMain;
	aBufferInfo.iBufferCount = KNumBuffers;

	aBufferInfo.iBufferP = reinterpret_cast<TUint8*>(Kern::Alloc(KMaxNumBytesPage * KNumBuffers));

	return(aBufferInfo.iBufferP ? KErrNone : KErrNoMemory);
	}

/**
Create an allocator suitable for use on this target
@internalComponent
*/
TNandAllocatorBase* DMediaDriverNand::ExtensionInitAllocator()
	{
	return new TNandAllocatorWins;
	}
#endif
