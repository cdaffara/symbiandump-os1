// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "plat_priv.h"
#include <property.h>
#include <variant.h>
#include "pp_sdio.h" 

const TInt  KDiskSectorSize=512;

const TInt KTotalMDiskSize=0x100000; // 1MB (if changing this then also change CSD response)

// ======== Register Map ========

typedef TInt (*TAccessFunction)(TInt aTargetCard, TInt aVal, TAny* aThis, TBool aRead, TUint8& aData);

const TInt KIoMapEnd = 0xFFFFFFFF;

struct SRegisterMapInfo
	{
public:
	TUint32 iRegisterID;				// Unique ID
	
	const SRegisterMapInfo* iChildMapP; // Pointer to child register map
	
	TUint32 iAddress;					// Start Address
	TUint32 iLength;					// Register Length in Bytes
	
	const TAny*	iDataP;					// Data for auto-access (may be NULL)
	TAccessFunction iAccessFunction;	// Invoked when register is accessed
	
	TUint8 iFlags;						// Bitmap of RO(0), R/W(1) bits (8-bit only?)
	};

// ======== Card Information Structures =========



const TUint32 KCommonCisPtr = 0x1010;
const TUint32 KCommonCisLen = 0x70;

LOCAL_D const TText8 CardCommonCis[KCommonCisLen] =
	{
//	0x20,0x04,0xc0,0x12,0x00,0x00,0x21,0x02,0x0c,0x00,0x22,0x06,0x00,0x00,0x01,0x32,
//	0x00,0x00,0x91,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x15,0x32,0x01,0x00,0x48,0x41,
//	0x47,0x49,0x57,0x41,0x52,0x41,0x20,0x53,0x59,0x53,0x2d,0x43,0x4f,0x4d,0x20,0x43,
//	0x4f,0x2e,0x2c,0x4c,0x54,0x44,0x2e,0x00,0x48,0x53,0x2d,0x53,0x44,0x44,0x4b,0x2d,
//	0x30,0x30,0x32,0x20,0x56,0x65,0x72,0x2e,0x50,0x61,0x6e,0x61,0x00,0x00,0xff,0xff,
//	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
//	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff

	0x20,0x04,0xc0,0x12,0x00,0x00,0x21,0x02,0x0c,0x00,0x22,0x04,0x00,0x00,0x01,0x2A/*79*/,
	0x91,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x15,0x35,0x01,0x00,0x48,0x41,0x47,0x49,
	0x57,0x41,0x52,0x41,0x20,0x53,0x59,0x53,0x2d,0x43,0x4f,0x4d,0x20,0x43,0x4f,0x2e,
	0x2c,0x4c,0x54,0x44,0x2e,0x00,0x48,0x53,0x2d,0x53,0x44,0x44,0x4b,0x2d,0x30,0x30,
	0x32,0x20,0x56,0x65,0x72,0x2e,0x50,0x61,0x6e,0x61,0x00,0x53,0x48,0x50,0x00,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
	};

const TUint32 KFn1CisPtr = 0x2000;
const TUint32 KFn1CisLen = 0x40;

LOCAL_D const TText8 Fn1Cis[KFn1CisLen] =
	{
	0x21,0x02,0x0c,0x00,0x22,0x24,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x03,0x00,0x02,0x00,0x00,0x3c,0x00,0x00,0x00,0xc8,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x2c,0x01,0xf4,0x01,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0f,0xff,
	};

const TUint32 KFn2CisPtr = 0x3000;
const TUint32 KFn2CisLen = 0x40;

LOCAL_D const TText8 Fn2Cis[KFn2CisLen] =
	{
	0x21,0x02,0x0c,0x00,0x22,0x24,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
	0x00,0x03,0x40,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0xc8,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xfa,0x00,0xc2,0x01,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	};


// ======== Card Common Control Registers =========

TUint8 GCCCRRegSdioRevision			= 0x00;
TUint8 GCCCRRegSdSpec				= 0x00;
TUint8 GCCCRRegIoEnable				= 0x00;
TUint8 GCCCRRegIoReady				= 0x00;
TUint8 GCCCRRegIntEnable			= 0x00;
TUint8 GCCCRRegIntPending			= 0x00;
TUint8 GCCCRRegIoAbort				= 0x00;
TUint8 GCCCRRegBusInterfaceControl	= 0x00;
TUint8 GCCCRRegCardCapability		= 0x00;
TUint8 GCCCRRegCisPtrLo				= (KCommonCisPtr & 0x0000FF);
TUint8 GCCCRRegCisPtrMid			= (KCommonCisPtr & 0x00FF00) >> 8;
TUint8 GCCCRRegCisPtrHi				= (KCommonCisPtr & 0xFF0000) >> 16;
TUint8 GCCCRRegBusSuspend			= 0x00;
TUint8 GCCCRRegFunctionSelect		= 0x00;
TUint8 GCCCRRegExecFlags			= 0x00;
TUint8 GCCCRRegReadyFlags			= 0x00;
TUint8 GCCCRRegFN0BlockSizeLo		= 0x00;	// Initialises with 0x0000
TUint8 GCCCRRegFN0BlockSizeHi		= 0x00;	// Initialises with 0x0000

TUint8 GFunctionToEnable = 0x00;

LOCAL_D const SRegisterMapInfo IoMapCCCR[] =
	{
		{KCCCRRegSdioRevision,		  NULL, 0x00, 0x01, &GCCCRRegSdioRevision,		  NULL, 0x00},
		{KCCCRRegSdSpec,			  NULL, 0x01, 0x01, &GCCCRRegSdSpec,			  NULL, 0x00},
		
		{KCCCRRegIoEnable,			  NULL, 0x02, 0x01, NULL, DWinsSDIOStack::AccessIoEnable, 0x00},
		
		{KCCCRRegIoReady,			  NULL, 0x03, 0x01, &GCCCRRegIoReady,			  NULL, 0x00},
		{KCCCRRegIntEnable,			  NULL, 0x04, 0x01, &GCCCRRegIntEnable,			  NULL, 0xFF},
		{KCCCRRegIntPending,		  NULL, 0x05, 0x01, &GCCCRRegIntPending,		  NULL, 0x00},
		{KCCCRRegIoAbort,			  NULL, 0x06, 0x01, &GCCCRRegIoAbort,			  NULL, 0xFF},
		{KCCCRRegBusInterfaceControl, NULL, 0x07, 0x01, &GCCCRRegBusInterfaceControl, NULL, 0xFF},
		{KCCCRRegCardCapability,	  NULL, 0x08, 0x01, &GCCCRRegCardCapability,	  NULL, 0x00},
		{KCCCRRegCisPtrLo,			  NULL, 0x09, 0x01, &GCCCRRegCisPtrLo,			  NULL, 0x00},
		{KCCCRRegCisPtrMid,			  NULL, 0x0a, 0x01, &GCCCRRegCisPtrMid,			  NULL, 0x00},
		{KCCCRRegCisPtrHi,			  NULL, 0x0b, 0x01, &GCCCRRegCisPtrHi,			  NULL, 0x00},
		{KCCCRRegBusSuspend,		  NULL, 0x0c, 0x01, &GCCCRRegBusSuspend,		  NULL, 0xFF},
		{KCCCRRegFunctionSelect,	  NULL, 0x0d, 0x01, &GCCCRRegFunctionSelect,	  NULL, 0xFF},
		{KCCCRRegExecFlags,			  NULL, 0x0e, 0x01, &GCCCRRegExecFlags,			  NULL, 0x00},
		{KCCCRRegReadyFlags,		  NULL, 0x0f, 0x01, &GCCCRRegReadyFlags,		  NULL, 0x00},
		{KCCCRRegFN0BlockSizeLo,	  NULL, 0x10, 0x01, &GCCCRRegFN0BlockSizeLo,	  NULL, 0x00},
		{KCCCRRegFN0BlockSizeHi,	  NULL, 0x11, 0x01, &GCCCRRegFN0BlockSizeHi,	  NULL, 0x00},
		{KIoMapEnd,					  NULL, 0,	  0,	NULL,						  NULL, 0xFF}
	};


// ======== Function Basic Register 1 =========

TUint8 GFBR1RegInterfaceCode = KFBRRegSupportsCSA;
TUint8 GFBR1RegExtendedCode  = 0x00;
TUint8 GFBR1RegPowerFlags	 = 0x00;
TUint8 GFBR1RegCisPtrLo		 = (KFn1CisPtr & 0x0000FF);
TUint8 GFBR1RegCisPtrMid	 = (KFn1CisPtr & 0x00FF00) >> 8;
TUint8 GFBR1RegCisPtrHi		 = (KFn1CisPtr & 0xFF0000) >> 16;
TUint8 GFBR1RegIoBlockSizeLo = 0x00;
TUint8 GFBR1RegIoBlockSizeHi = 0x00;

TUint32 GFBR1RegCsaPtr = 0x00000000;

LOCAL_D const SRegisterMapInfo IoMapFBR1[] =
	{
		{KFBRRegInterfaceCode, NULL, 0x100, 0x01, &GFBR1RegInterfaceCode, NULL, 0x00},
		{KFBRRegExtendedCode,  NULL, 0x101, 0x01, &GFBR1RegExtendedCode,  NULL, 0x00},
		{KFBRRegPowerFlags,	   NULL, 0x102, 0x01, &GFBR1RegPowerFlags,	  NULL, 0x00},
		{KFBRRegCisPtrLo,	   NULL, 0x109, 0x01, &GFBR1RegCisPtrLo,	  NULL, 0x00},
		{KFBRRegCisPtrMid,	   NULL, 0x10a, 0x01, &GFBR1RegCisPtrMid,	  NULL, 0x00},
		{KFBRRegCisPtrHi,	   NULL, 0x10b, 0x01, &GFBR1RegCisPtrHi,	  NULL, 0x00},

		{KFBRRegCsaPtrLo,	   NULL, 0x10c, 0x01, NULL, DWinsSDIOStack::AccessCsaPointer, 0xFF},
		{KFBRRegCsaPtrMid,	   NULL, 0x10d, 0x01, NULL, DWinsSDIOStack::AccessCsaPointer, 0xFF},
		{KFBRRegCsaPtrHi,	   NULL, 0x10e, 0x01, NULL, DWinsSDIOStack::AccessCsaPointer, 0xFF},
		{KFBRRegCsaWindow,	   NULL, 0x10f, 0x01, NULL, DWinsSDIOStack::AccessCsaWindow,  0xFF},
		
		{KFBRRegIoBlockSizeLo, NULL, 0x110, 0x01, &GFBR1RegIoBlockSizeLo, NULL, 0xFF},
		{KFBRRegIoBlockSizeHi, NULL, 0x111, 0x01, &GFBR1RegIoBlockSizeHi, NULL, 0xFF},
		{KIoMapEnd,			   NULL, 0,	    0,	  NULL,					  NULL, 0x00}
	};

// ======== Function Basic Register 2 ========

TUint8 GFBR2RegInterfaceCode = KFBRRegSupportsCSA | ESdioFunctionTypeUART;
TUint8 GFBR2RegExtendedCode  = 0x00;
TUint8 GFBR2RegPowerFlags	 = 0x00;
TUint8 GFBR2RegCisPtrLo		 = (KFn2CisPtr & 0x0000FF);
TUint8 GFBR2RegCisPtrMid	 = (KFn2CisPtr & 0x00FF00) >> 8;
TUint8 GFBR2RegCisPtrHi		 = (KFn2CisPtr & 0xFF0000) >> 16;
TUint8 GFBR2RegIoBlockSizeLo = 0x00;
TUint8 GFBR2RegIoBlockSizeHi = 0x00;

TUint32 GFBR2RegCsaPtr = 0x00000000;

LOCAL_D const SRegisterMapInfo IoMapFBR2[] =
	{
		{KFBRRegInterfaceCode, NULL, 0x200, 0x01, &GFBR2RegInterfaceCode, NULL, 0x00},
		{KFBRRegExtendedCode,  NULL, 0x201, 0x01, &GFBR2RegExtendedCode,  NULL, 0x00},
		{KFBRRegPowerFlags,	   NULL, 0x202, 0x01, &GFBR2RegPowerFlags,	  NULL, 0x00},
		{KFBRRegCisPtrLo,	   NULL, 0x209, 0x01, &GFBR2RegCisPtrLo,	  NULL, 0x00},
		{KFBRRegCisPtrMid,	   NULL, 0x20a, 0x01, &GFBR2RegCisPtrMid,	  NULL, 0x00},
		{KFBRRegCisPtrHi,	   NULL, 0x20b, 0x01, &GFBR2RegCisPtrHi,	  NULL, 0x00},

		{KFBRRegCsaPtrLo,	   NULL, 0x20c, 0x01, NULL, DWinsSDIOStack::AccessCsaPointer, 0xFF},
		{KFBRRegCsaPtrMid,	   NULL, 0x20d, 0x01, NULL, DWinsSDIOStack::AccessCsaPointer, 0xFF},
		{KFBRRegCsaPtrHi,	   NULL, 0x20e, 0x01, NULL, DWinsSDIOStack::AccessCsaPointer, 0xFF},
		{KFBRRegCsaWindow,	   NULL, 0x20f, 0x01, NULL, DWinsSDIOStack::AccessCsaWindow,  0xFF},
		
		{KFBRRegIoBlockSizeLo, NULL, 0x210, 0x01, &GFBR2RegIoBlockSizeLo, NULL, 0xFF},
		{KFBRRegIoBlockSizeHi, NULL, 0x211, 0x01, &GFBR2RegIoBlockSizeHi, NULL, 0xFF},
		{KIoMapEnd,			   NULL, 0,	    0,	  NULL,					  NULL, 0x00}
	};

// ======== Function Basic Register 1 =========

const TInt KIoMapCCCR	   = 0;
const TInt KIoMapFBR1	   = 1;
const TInt KIoMapFBR2	   = 2;
const TInt KIoMapCommonCis = 3;
const TInt KIoMapFn1Cis	   = 4;
const TInt KIoMapFn2Cis	   = 5;

LOCAL_D const SRegisterMapInfo IoMapTop[] =
	{
		{KIoMapCCCR,	  IoMapCCCR, 0x00,			0xFF,		   NULL,		  NULL, 0x00},
		{KIoMapFBR1,	  IoMapFBR1, 0x100,			0xFF,		   NULL,		  NULL, 0x00},		
		{KIoMapFBR2,	  IoMapFBR2, 0x200,			0xFF,		   NULL,		  NULL, 0x00},
		{KIoMapCommonCis, NULL,		 KCommonCisPtr, KCommonCisLen, CardCommonCis, NULL, 0x00},
		{KIoMapFn1Cis,	  NULL,		 KFn1CisPtr,	KFn1CisLen,	   Fn1Cis,		  NULL, 0x00},
		{KIoMapFn2Cis,	  NULL,		 KFn2CisPtr,	KFn2CisLen,	   Fn2Cis,		  NULL, 0x00},
		{KIoMapEnd,		  NULL,		 0,				0,			   NULL,		  NULL, 0x00}
	};

const SRegisterMapInfo* FindIoEntryFromID(const SRegisterMapInfo* aIoMapP, TUint32 aID)
	{
	const SRegisterMapInfo* foundEntry = NULL;
	
	TInt currentEntry = 0;
	
	while((aIoMapP[currentEntry].iRegisterID != KIoMapEnd) && (foundEntry == NULL))
		{
		if(aIoMapP[currentEntry].iRegisterID == aID)
			{
			foundEntry = aIoMapP+currentEntry;
			}
		currentEntry++;
		}

	return(foundEntry);
	}

const SRegisterMapInfo* FindIoEntryFromAddress(const SRegisterMapInfo* aIoMapP, TUint32 aAddr)
	{
	const SRegisterMapInfo* foundEntry = NULL;
	
	TInt currentEntry = 0;
	
	while((aIoMapP[currentEntry].iRegisterID != KIoMapEnd) && (foundEntry == NULL))
		{
		const TUint32 startAddr = aIoMapP[currentEntry].iAddress;
		const TUint32 endAddr   = startAddr + aIoMapP[currentEntry].iLength - 1;
		
		if((aAddr >= startAddr) && (aAddr <= endAddr))
			{
			if(aIoMapP[currentEntry].iChildMapP)
				{
				foundEntry = FindIoEntryFromAddress(aIoMapP[currentEntry].iChildMapP, aAddr);
				}
			else
				{
				foundEntry = aIoMapP+currentEntry;
				}
			}
		currentEntry++;
		}

	return(foundEntry);
	}



// ======== error code conversion ========

GLDEF_C TInt MapLastErrorEpoc()
//
// map an Win32 error code to Epoc32 value
//
	{
	TInt res=KErrGeneral;
	switch (GetLastError())
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
		}
	return(res);
	}

GLDEF_C TMMCErr MapLastErrorMmc()
//
// map Win32 error to a TMMCErr error.
//
	{
	DWORD r=GetLastError();
	TInt res=KErrGeneral;
	switch (r)
		{
		case ERROR_SHARING_VIOLATION:
		case ERROR_LOCK_VIOLATION:
			res=KMMCErrLocked;			// KErrLocked
			break;
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			res=KMMCErrNotFound;		// KErrNotFound
			break;
		case ERROR_ALREADY_EXISTS:
		case ERROR_FILE_EXISTS:
			res=KMMCErrAlreadyExists;	// KErrAlreadyExists
			break;
		case ERROR_NOT_READY: res=KMMCErrNoCard; break;
		case ERROR_UNRECOGNIZED_VOLUME:
		case ERROR_NOT_DOS_DISK:
			res=KMMCErrGeneral;			// KErrGeneral
			break;
		case ERROR_UNRECOGNIZED_MEDIA:
		case ERROR_INVALID_NAME:
		case ERROR_NO_MORE_FILES:
			res=KMMCErrResponseCRC; 	// KErrCorrupt
			break;
		}
	return(res);
	}

// ======== DWinsSDIOStack ========

DWinsSDIOStack::DWinsSDIOStack(TInt aBus, DMMCSocket* aSocket)
  : DSDIOStack(aBus, aSocket),
	iEnableTimer(&DWinsSDIOStack::EnableTimerCallback,this)
	{	
	iAddressedCard=KBroadcastToAllCards;
//	iCMD42Failed=EFalse;
	}


TInt DWinsSDIOStack::Init()
//
// Allocate any resources. Only created once on kernel initialization so dont
// worry about cleanup if it leaves.
//
	{
	if((iCardArray = new TSDIOCardArray(this)) == NULL)
		return KErrNoMemory;

	TInt r=DMMCStack::Init();
	if(r!=KErrNone)
		return r;

	DMediaChangeBase* pMCBase = MMCSocket()->iMediaChange;
	static_cast<DWinsMMCMediaChange*>(pMCBase)->SetStackP(this);
	Wins::SetMediaChangeCallBackPtr(DWinsMMCMediaChange::MediaChangeCallBack, (TAny*)pMCBase);

	//
	// Over time memory can become fragmented, and so it is not possible to
	// allocate physically contiguous pages.  Therefore, the buffers for IO
	// are allocated at startup.
	//
	// block and erase sector size characteristics depend on the specific
	// card model, and so the initial values are estimates based on a typical
	// card.  If these do not match the actual card's block size (or erase
	// size, for SD,) then the media driver just gets a reduced or increased
	// buffer area, and its efficiency varies accordingly.
	//
	// For the WINS implementation, fragmentation does not matter because
	// DMA is not used.  The memory must still be allocated here so MEDMMC is
	// able to use it.
	//
	// The constant calculations could be folded, but this illustrates how the
	// values are derived.
	//

	// MMC - values from Hitachi 16Mb card, datasheet HB288016MM1

	// minor buffer must contain enough space for MBR or block
	const TUint mmcBlkSzLog2 = 9;				// READ_BLK_LEN and WRITE_BLK_LEN
	const TUint mmcBlkSz = 1 << mmcBlkSzLog2;
	const TInt mmcMinorBufLen = Max(KDiskSectorSize, mmcBlkSz);

	const TInt KMinMMCBlocksInBuffer = 8;
	const TInt mmcCchBufLen = KMinMMCBlocksInBuffer << mmcBlkSzLog2;

	const TInt mmcTotalBufLen = mmcMinorBufLen + mmcCchBufLen;

	// SDCard - values from 64Mb Panasonic RP-SD064

	const TUint sdBlkSzLog2 = 9;				// READ_BL_LEN and WRITE_BLK_LEN
	const TUint sdBlkSz = 1 << sdBlkSzLog2;
	const TInt sdMinorBufLen = Max(KDiskSectorSize, sdBlkSz);

	const TUint ss = 0x1f;						// SECTOR_SIZE, add 1 for sector count
	const TInt KMinSDBlocksInBuffer = 8;
	const TInt sdCchBufLen = Max(KMinSDBlocksInBuffer, ss + 1) << sdBlkSzLog2;

	const TInt sdTotalBufLen = sdMinorBufLen + sdCchBufLen;

	const TInt totalBufLen = Max(mmcTotalBufLen, sdTotalBufLen);

	iMDBuf = reinterpret_cast<TUint8*>(Kern::Alloc(totalBufLen));
	iMDBufLen = totalBufLen;

	// initialize each card on the stack
	TInt i;
	for (i = 0; i < KTotalWinsCards; ++i)
		{
		TInt r = SetupSimulatedCard(i);
		if (r != KErrNone)
			return r;
		}

	// initialize pointers to currently present cards

	// Slot zero can toggle between no card; card 0 and card 1.  The current state is
	// determined by *Wins::CurrentPBusDevicePtr() and toggled by pressing F4 when F5
	// (door open) is held down.  Because this function is only executed at startup,
	// assume start with card zero.
	iCardInfo[0] = iCardPool[0];
	for (i = 1; i < KTotalWinsCardSlots; ++i)
		{
		iCardInfo[i]=iCardPool[i+1];
		}

	return KErrNone;
	}

void DWinsSDIOStack::MachineInfo(TMMCMachineInfo& aMachineInfo)
	{
	aMachineInfo.iTotalSockets=KTotalWinsCardSlots;
	aMachineInfo.iTotalMediaChanges=0;  		// Not used at present
	aMachineInfo.iTotalPrimarySupplies=0;		// Not used at present

	aMachineInfo.iSPIMode=EFalse;
    aMachineInfo.iBaseBusNumber=0;

	__ASSERT_DEBUG(aMachineInfo.iTotalSockets<=KMaxMMCardsPerStack,
					DWinsSDIOStack::Panic(DWinsSDIOStack::EWinsMMCBadMachineInfo));
	}

void DWinsSDIOStack::AdjustPartialRead(
#ifdef _DEBUG
	const TMMCard* aCard,
#else
	const TMMCard* /*aCard*/,
#endif
	TUint32 aStart, TUint32 aEnd, TUint32* aPhysStart, TUint32* aPhysEnd) const
	{
#ifdef _DEBUG
	const TUint32 blkLen = aCard->CSD().ReadBlockLength();
	const TUint32 blkMsk = blkLen - 1;

	__ASSERT_DEBUG(aCard->CSD().ReadBlPartial(), Panic(EWinsMMCAPRNotSupp));
	__ASSERT_DEBUG(aEnd - aStart <= blkLen, Panic(EWinsMMCAPRRange));
	__ASSERT_DEBUG((aEnd & ~blkMsk) > (aStart & ~blkMsk), Panic(EWinsMMCAPRBoundary));
#endif

	*aPhysStart = aStart & ~0x3;
	*aPhysEnd = (aEnd + 0x3) & ~0x3;
	}

void DWinsSDIOStack::GetBufferInfo(TUint8** aMDBuf, TInt* aMDBufLen)
	{
	*aMDBuf = iMDBuf;
	*aMDBufLen = iMDBufLen;
	}

void DWinsSDIOStack::Panic(TWinsMMCPanic aPanic)
	{
	_LIT(KPncNm,"PBUS-MMCSD-WINS");
	Kern::PanicCurrentThread(KPncNm,aPanic);
	}

TInt DWinsSDIOStack::SetupSimulatedCard(TInt aCardNum)
//
// allocate individual card with Win32 file.  Only called at bootup, so no cleanup if fails.
//
	{
	TWinsCardInfo* cip = new TWinsCardInfo;
	if (cip == 0)
		return KErrNoMemory;

	TUint8 cid[KMMCCIDLength];
	cid[0] = 'C';
	cid[1] = 'I';
	cid[2] = 'D';
	cid[3] = TUint8('0' + aCardNum);
	TInt j;
	for (j = 4; j < KMMCCIDLength - 1; ++j)
		cid[j] = 'c';
	cid[KMMCCIDLength - 1] = '#';				// '#' = 0x23, bit zero must be 1
	cip->iCID=cid;

	cip->iPWD = new TMediaPassword;
	if (! cip->iPWD)
		{
		delete cip;
		return KErrNoMemory;
		}

	// cards in slot zero are SD
	TInt mediaAreas;
	if (aCardNum <= 1)
		{
		cip->iIsSDCard = ETrue;
		mediaAreas = 3;	// +1 for SDIO area
		}
	else
		{
		cip->iIsSDCard = EFalse;
		mediaAreas = 1;
		}

	cip->iState=ECardStateIdle;

	for (TInt area = 0; area < mediaAreas; ++area)
		{
		TInt r = CreateBinFileForCard(aCardNum, area, &cip->iAreaHandles[area]);
		if (r != KErrNone)
			return r;
		}
	iCardPool[aCardNum]=cip;
	return(KErrNone);
	}

TInt DWinsSDIOStack::CreateBinFileForCard(TInt aCardNum, TInt aAreaNum, HANDLE* aHandle)
//
// create .bin file in temp directory to contain media area of card.
//
	{
	const char* emulatorPath = Property::GetString("EmulatorMediaPath");
	if (!Emulator::CreateAllDirectories(emulatorPath))
		return Emulator::LastError();

	TBuf8<KMaxFileName> fn8(_L8(emulatorPath));
	fn8.Append(_L8("MMCCRD"));
	fn8.AppendNum(aCardNum);
	fn8.Append('A'+aAreaNum);
	fn8.Append(_L8(".BIN"));
	fn8.Append('\0');

	*aHandle = CreateFileA(
		(LPCSTR) fn8.Ptr(),					// LPCSTR lpFileName,
		GENERIC_READ | GENERIC_WRITE,		// DWORD dwDesiredAccess
		FILE_SHARE_READ | FILE_SHARE_WRITE,	// DWORD dwShareMode
		NULL,								// LPSECURITY_ATTRIBUTES lpSecurityAttributes
		OPEN_ALWAYS,						// DWORD dwCreationDisposition
		FILE_FLAG_RANDOM_ACCESS,			// DWORD dwFlagsAndAttributes
		NULL);								// HANDLE hTemplateFile

	if (*aHandle == INVALID_HANDLE_VALUE)
	    return MapLastErrorEpoc();
	
	if (	SetFilePointer(*aHandle, KTotalMDiskSize, NULL, FILE_BEGIN) == 0xffffffffu
		||	! SetEndOfFile(*aHandle) )
		{
		CloseHandle(*aHandle);
	    return MapLastErrorEpoc();
		}

	return KErrNone;
	}

void DWinsSDIOStack::SetBusConfigDefaults(TMMCBusConfig& aConfig, TUint aClock)
	{
	const TUint KWinsMaxHwInterfaceClk=104000;
	const TUint KWinsResponseTimeOut=6400;
	const TUint KWinsDataTimeOut=40000;
	const TUint KWinsBusyTimeOut=200000;

	aConfig.iBusClock = (aClock > KWinsMaxHwInterfaceClk) ? KWinsMaxHwInterfaceClk : aClock;
	aConfig.iResponseTimeOut=KWinsResponseTimeOut;
	aConfig.iDataTimeOut=KWinsDataTimeOut;
	aConfig.iBusyTimeOut=KWinsBusyTimeOut;
	}

void DWinsSDIOStack::InitClockOff()
	{
	// empty.
	}

void DWinsSDIOStack::ASSPReset()
	{
	// empty.
	}

void DWinsSDIOStack::ASSPDisengage()
	{
	// empty.
	}

void DWinsSDIOStack::DoPowerDown()
	{
	// empty.
	}


LOCAL_C TInt SetMediaPasswordEnvironmentVar(TInt aSocketNum,TInt aCardNum,const TDesC8& aPasswd)
// 
// Set the password for local drive 'aLocalDrive', card number 'aCardNum' to 'aPasswd' - as an
// environment variable. Note that the card number is only relevant where the emulated drive
// supports card hot-swapping (i.e. F4 whilst F5 is held down).
//
	{
	// Setup the appropriate environment variable string '_EPOC_LocDrv_<locDrvNum>_PWORD_<cardNum>'
	TUint16 envVar[]=L"_EPOC_Socket_X_PWORD_Y";

	envVar[13]=(TUint16)('0'+aSocketNum);
	envVar[21]=(TUint16)('0'+aCardNum);
	
	// Setup the new value of the environment variable
	TUint16	envVal[100];
	TInt len=aPasswd.Length();

	// the password may be empty if a card's password is cleared
	if (len>(100-1))
		return(KErrArgument);
	memcpy(&envVal[0],reinterpret_cast<const TUint16 *>(aPasswd.Ptr()),len);
	envVal[len>>1]='\0';

	// Now set the new value for the environment variable
	if (SetEnvironmentVariable(envVar,&envVal[0]))
		return(KErrNone);

	return KErrGeneral;
	}

LOCAL_C TInt MediaPasswordEnvironmentVar(TInt aSocketNum,TInt aCardNum,TDes8& aPasswd)
// 
// Get the password for local drive 'aLocalDrive', card number 'aCardNum' into 'aPasswd' - from
// an environment variable. Note that the card number is only relevant where the emulated drive
// supports card hot-swapping (i.e. F4 whilst F5 is held down).
//
	{
	TUint16 envVar[]=L"_EPOC_Socket_X_PWORD_Y";

	envVar[13]=(TUint16)('0'+aSocketNum);
	envVar[21]=(TUint16)('0'+aCardNum);
	
	TUint16 envVal[100];	// To hold the value of the retreived environment variable
	
	DWORD len=GetEnvironmentVariable(envVar,&envVal[0],100);
	if (len>(TUint)100)
		return(KErrGeneral);
	if (len)
		{
		// Found the requested environment variable so there is a password for this local drive / card. 
		if ((len<<1)<=KMaxMediaPassword)
			{
			aPasswd.FillZ(KMaxMediaPassword);
			aPasswd.Zero();
			aPasswd.Copy(reinterpret_cast<TUint8*>(&envVal[0]),len<<1);
			return(KErrNone);	
			}
		else	
			return(KErrGeneral);	
		}

	return(KErrNotFound);
	}

TMMCErr DWinsSDIOStack::DoPowerUpSM()
	{
	enum states
		{
		EStBegin=0,
		EStEnd
		};

	SMF_BEGIN

		if(MMCSocket()->iVcc->SetState(EPsuOnCurLimit) != KErrNone)
			return KMMCErrHardware;

		for (TInt i = 0; i < KTotalWinsCardSlots; ++i)
			{
			// if card has a password, it will be locked on power up
			TInt cardNum = (i==0) ? *Wins::CurrentPBusDevicePtr() : i + 1;
			if (	cardNum >= 0
				&&	MediaPasswordEnvironmentVar(
						MMCSocket()->iSocketNumber, cardNum, *(iCardInfo[i]->iPWD))
					==	KErrNone)
				{
				iCardInfo[i]->iIsLocked = (iCardInfo[i]->iPWD->Length() > 0);
				}
			else	
				iCardInfo[i]->iIsLocked=EFalse;

			iCardInfo[i]->iState = ECardStateIdle;
			iCardInfo[i]->iRCA=0x0001;		// Default RCA - spec 2.2, s4.2.1, 5.4
			}

		ReportPowerUp();

	SMF_END
	}

TMMCErr DWinsSDIOStack::InitClockOnSM()
	{
	enum states
		{
		EStBegin=0,
		EStEnd
		};
	SMF_BEGIN

	SMF_END
	}

void DWinsSDIOStack::AddressCard(TInt aCardNumber)
	{
	iAddressedCard = aCardNumber;
	}


TInt DWinsSDIOStack::GetTargetSlotNumber(const TRCA& anRCA)
//
// when the controller is given a command with an embedded RCA, this function
// works out which physical card slot it corresponds to.  If no card has been
// assigned the RCA then it returns -1.
//
	{
	TInt targetIdx = -1;

	for (TInt i = 0; i < KTotalWinsCardSlots; ++i)
		{
		if (iCardInfo[i]->iRCA==anRCA)
			{
			targetIdx=i;
			break;
			}
		}

	return(targetIdx);
	}

TMMCErr DWinsSDIOStack::IssueMMCCommandSM()
	{
	enum states
		{
		EStBegin=0,
		EStEnd
		};

	TMMCCommandDesc& cmd = Command();

	// If the command contains an embedded RCA then extract it	
	TRCA tgtRCA=0;
	TBool supRCA=EFalse;
	if (/*cmd.iCommand == ECmdSetRelativeAddr	||	*/cmd.iCommand == ECmdSelectCard
	||	cmd.iCommand == ECmdSendCSD			||	cmd.iCommand == ECmdSendCID
	||	cmd.iCommand == ECmdSendStatus		||	cmd.iCommand == ECmdGoInactiveState
	||	cmd.iCommand == ECmdFastIO 			||  cmd.iCommand == ECmdAppCmd )
		{
		if ((cmd.iArgument >> 16) != 0)
			{
			supRCA=ETrue;
			tgtRCA=TUint16(cmd.iArgument >> 16);
			}
		}

	// if the card contains an embedded RCA, work out which slot it corresponds to.
	// At the end of the function, this card is used to generate the R1 response.
	// Assume that if rca is supplied it either corresponds to the selected card or
	// broadcast mode is on.  (An exception is CMD7 with arg0 to deselect all cards.)

	TInt targetCard = supRCA ? GetTargetSlotNumber(tgtRCA) : iAddressedCard;
	TBool rto = EFalse;							// response timeout

	// if try to access card zero has been set to holding no card via F5 / F4 then timeout.
	if ((targetCard == 0) && *Wins::CurrentPBusDevicePtr() < 0)
		return KMMCErrResponseTimeOut;
	
	HANDLE winHandle;

	// CMD42 is a data transfer command.  That means the R1 response that it returns
	// immediately is the state it is in on receiving the data block, and not after
	// processing it.  If the data block is invalid then LOCK_UNLOCK_FAILED will be
	// set in the R1 response which is sent in reply to the next command.

	TBool nextCMD42Failed = EFalse;
	TBool lock_unlock_failed=EFalse;

	// When the card is locked, it will only respond to basic command class (0) and
	// lock card command class (7).  An exception is CMD16.  This is sent before CMD42,
	// but is classified (MMC Spec 23.2, table 5) as belonging to classes 2 and 4.
	// For data transfer commands, LOCK_UNLOCK_FAIL is set in response to the following

	TMMCCommandEnum origCmd = cmd.iCommand;

	// if targetting locked card...
	if (targetCard != KBroadcastToAllCards && iCardInfo[targetCard]->iIsLocked)
		{
		// ...and not command used in init or CMD42 sequence...
		if (!(	((cmd.iSpec.iCommandClass & (KMMCCmdClassApplication | KMMCCmdClassBasic | KMMCCmdClassLockCard)) != 0)
			||	(cmd.iCommand == ECmdSetBlockLen) || (cmd.iCommand == ECmdAppCmd) ))
			{
			lock_unlock_failed = ETrue;
			cmd.iCommand = (TMMCCommandEnum) -1;	// skip case processing
			}
		}

	SMF_BEGIN

	switch (cmd.iCommand)
		{
		case ECmdGoIdleState:	// CMD0
			if (iAddressedCard != KBroadcastToAllCards)
				iCardInfo[iAddressedCard]->iState = ECardStateIdle;
			else
				{
				for (TInt i = 0; i < KTotalWinsCardSlots; ++i)
					iCardInfo[i]->iState = ECardStateIdle;
				}
			break;

		case ECmd41:
		case ECmdSendOpCond:	// CMD1
			{
			if (iAddressedCard != KBroadcastToAllCards)
				iCardInfo[iAddressedCard]->iState = ECardStateReady;
			else
				{
				for (TInt i = 0; i < KTotalWinsCardSlots; ++i)
					iCardInfo[i]->iState = ECardStateReady;
				}

			// bit31 is set to indicate cards are not still powering up
			TUint32 r3 = KMMCWinsCardOCRValue | KMMCOCRBusy;
			TMMC::BigEndian4Bytes(cmd.iResponse, r3);
			}
			break;

		case ECmdAllSendCID:	// CMD2
			{
			TInt idx;
			if (iAddressedCard != KBroadcastToAllCards)
				{
				idx = iAddressedCard;
				__ASSERT_DEBUG(
					iCardInfo[iAddressedCard]->iState == ECardStateReady,
					DWinsSDIOStack::Panic(DWinsSDIOStack::EStkIMCBadStateCmd2));
				}
			else
				idx = FindAnyCardInStack(ECardStateReady);

			if (idx == -1)
				rto = ETrue;
			else
				{
				iCardInfo[idx]->iCID.Copy(cmd.iResponse);
				iCardInfo[idx]->iState = ECardStateIdent;
				}
			}
			break;

		case ECmdSetRelativeAddr:	// CMD3
			{
			TInt idx;
			if (iAddressedCard != KBroadcastToAllCards)
				{
				__ASSERT_DEBUG(
					iCardInfo[iAddressedCard]->iState == ECardStateIdent,
					DWinsSDIOStack::Panic(DWinsSDIOStack::EStkIMCBadStateCmd3));
				
				if (iCardInfo[iAddressedCard]->iIsSDCard)
					{
					static TUint16 RCACounter = 0x1234;
					// SD Cards publish RCAs
					++RCACounter;
					iCardInfo[iAddressedCard]->iRCA = RCACounter;
					iCardInfo[iAddressedCard]->iState = ECardStateStby;
					TUint32 r6 = TUint32(RCACounter) << 16;
					TMMC::BigEndian4Bytes(&cmd.iResponse[0],r6); // Ignore bits 47-40
					}
				else
					{
					iCardInfo[iAddressedCard]->iRCA = TUint16(cmd.iArgument >> 16);
					iCardInfo[iAddressedCard]->iState=ECardStateStby;
					}
				}
			else
				{
				// MultiMediaCards are assigned RCAs
				idx = FindOneCardInStack(ECardStateIdent);
				iCardInfo[iAddressedCard]->iRCA = TUint16(cmd.iArgument >> 16);
				iCardInfo[iAddressedCard]->iState=ECardStateStby;
				targetCard = iAddressedCard;
				}
			}
			break;

		case ECmd6:
			// if ACMD6 then change bus width
			if (cmd.iSpec.iCommandClass == KMMCCmdClassApplication)
				{
				switch (cmd.iArgument)
					{
				case 0x00:
					iCardInfo[iAddressedCard]->iBusWidth = 1;
					break;
				case 0x02:
					iCardInfo[iAddressedCard]->iBusWidth = 4;
					break;
				default:
					DWinsSDIOStack::Panic(DWinsSDIOStack::EStkIMCCmd6InvalidWidth);
					break;
					}
				}
			break;

		case ECmdSelectCard:		// CMD7
			{
			// switch to broadcast mode so the currently selected and new cards
			// receive the command simultaneously.

			TInt idx = FindAnyCardInStack(ECardStateTran);
			if (idx != -1)
				iCardInfo[idx]->iState = ECardStateStby;
			if ((iAddressedCard=targetCard) == KBroadcastToAllCards)
				rto = ETrue;
			else
				{
				iCardInfo[targetCard]->iState = ECardStateTran;
				targetCard = targetCard;
				}
			}
			break;

		case ECmdSendStatus:
			// R1 response so status return as for any other R1 command.
			if (cmd.iSpec.iCommandClass == KMMCCmdClassApplication)
				{
				__ASSERT_DEBUG(
					iCardInfo[targetCard]->iIsSDCard,
					DWinsSDIOStack::Panic(DWinsSDIOStack::EStkICMACMD13NotSD));

				memset(cmd.iDataMemoryP, 0, KSDStatusBlockLength);
				if (iCardInfo[targetCard]->iBusWidth == 1)
					cmd.iDataMemoryP[0] = 0x00 << 6;
				else	// if (iCardInfo[targetCard]->iBusWidth == 4)
					cmd.iDataMemoryP[0] = 0x02 << 6;
				cmd.iDataMemoryP[7] = 0x28;		// PROTECTED_AREA_SIZE
				}
			break;

		case ECmdReadSingleBlock:
		case ECmdReadMultipleBlock:
			{
			winHandle=iCardInfo[targetCard]->iAreaHandles[KSDUserArea];

			if ( cmd.iSpec.iUseStopTransmission && cmd.iBlockLength >= cmd.iTotalLength)
				return( KMMCErrNotSupported );

    		TMMCErr err;
			TInt pos = cmd.iArgument;
    		if (SetFilePointer(winHandle,pos,NULL,FILE_BEGIN)==0xffffffffu)
        		err=MapLastErrorMmc();
    		else
        		{
	    		DWORD res;
				TInt len = cmd.iTotalLength;
		        if (ReadFile(winHandle,(TAny*)cmd.iDataMemoryP,len,&res,NULL)==FALSE)
                    err=MapLastErrorMmc();
                else if (res!=(DWORD)len)
                    err=KMMCErrGeneral;
                else
                    err=KMMCErrNone;
				}
			if (err!=KMMCErrNone)
				return(err);
			break;
			}

		case ECmd22:
			if (cmd.iSpec.iCommandClass == KMMCCmdClassApplication)
				{
				TMMC::BigEndian4Bytes(cmd.iResponse, iMBWOKBlocks);
				}
			break;
		// ------------------------------------------------------------------
		case ECmdWriteBlock:
		case ECmdWriteMultipleBlock:
			{
			TUint32 writeLen;

			// periodically fail multi-block writes to test ACMD22 error recovery
			if (cmd.iCommand != ECmdWriteMultipleBlock)
				writeLen = cmd.iTotalLength;
			else
				{
				const TInt KMaxFailCnt = 4;
				static TInt failCnt = 0;
				const TInt KMaxFailBlock = 4;
				static TInt failBlocks = 0;
				
				failCnt = (failCnt + 1) % KMaxFailCnt;
				if (failCnt != 0)
					writeLen = cmd.iTotalLength;
				else
					{
					failBlocks = (failBlocks + 1) % KMaxFailBlock;
					
					// fail at least one block
					TInt totalBlocks = cmd.iTotalLength / cmd.iBlockLength;
					TInt blocksToFail = Min(failBlocks + 1, totalBlocks);	// fail at least one block
					iMBWOKBlocks = (totalBlocks - blocksToFail);
					writeLen = iMBWOKBlocks * cmd.iBlockLength;
					if (writeLen == 0)
						return KMMCErrDataTimeOut;
					}
				}
			
			HANDLE h=iCardInfo[targetCard]->iAreaHandles[KSDUserArea];

    		TMMCErr err;
			TInt pos = cmd.iArgument;
    		if (SetFilePointer(h, pos, NULL, FILE_BEGIN)==0xffffffffu)
        		err = MapLastErrorMmc();
    		else
        		{
	    		DWORD res;
				if (! WriteFile(h, (LPCVOID)cmd.iDataMemoryP,writeLen,&res,NULL))
                    err=MapLastErrorMmc();
                else if (res!=(DWORD)writeLen)
                    err=KMMCErrGeneral;
                else
                    err=KMMCErrNone;
				}

			if (err!=KMMCErrNone)
				return(err);
			if (writeLen != cmd.iTotalLength)
				return KMMCErrDataTimeOut;
			}
			break;

		case ECmdAppCmd:
			// targetCard == -1 when ACMD41 being sent because not yet supplied
			if (iAddressedCard != KBroadcastToAllCards)
				{
				// timeout if addressed card is not SD
				if (! iCardInfo[iAddressedCard]->iIsSDCard)
					rto = ETrue;
				}
			else
				{
				// request sent to specific non-SD card
				if (targetCard != -1 && ! iCardInfo[targetCard]->iIsSDCard)
					rto = ETrue;
				}
			break;

		case ECmdSendCSD:
			{
			iCardInfo[targetCard]->GetCSD(cmd.iResponse);
			break;
			}

		// ------------------------------------------------------------------
		case ECmdLockUnlock:
			// in EPOC, Lock() does not actually lock the card.  It just sets the
			// password.  This means that the card is still accessible to the user,
			// but must be unlocked the next time it is powered up.

			// a real card will transiently go into rcv and prg state while processing
			// this command.  When finished, it will fall back into tran state.
			// The R1 response is sent immediately after CMD42.  CIMReadWriteBlocksSM()
			// sends CMD13 to find out whether or not LOCK_UNLOCK_FAIL was set.

			// the asserts in this case protect against invalid data being sent from the
			// media driver.  A real card would fail these corrupt data blocks.

			{
			const TInt8 cmd_byte(*cmd.iDataMemoryP);
			__ASSERT_DEBUG(										// ensure not CLR_PWD && SET_PWD
				!((cmd_byte & KMMCLockUnlockClrPwd) && (cmd_byte & KMMCLockUnlockSetPwd)),
				DWinsSDIOStack::Panic(DWinsSDIOStack::EWinsMMCCorruptCommand) );
			
			__ASSERT_DEBUG(										// not actually lock a card
				!(cmd_byte & KMMCLockUnlockLockUnlock),
				DWinsSDIOStack::Panic(DWinsSDIOStack::EWinsMMCLockAttempt) );

			if (cmd_byte & KMMCLockUnlockErase)					// ERASE (not supported)
				return KMMCErrNotSupported;

			const TInt8 pwd_len = *(cmd.iDataMemoryP + 1);
			const TPtrC8 pwd(cmd.iDataMemoryP + 2, pwd_len);

			if ((cmd_byte & KMMCLockUnlockClrPwd) != 0)			// CLR_PWD == 1
				{
				__ASSERT_DEBUG(
					pwd_len >= 0 && pwd_len <= KMaxMediaPassword,
					DWinsSDIOStack::Panic(DWinsSDIOStack::EWinsMMCCorruptCommand));

				if (iCardInfo[targetCard]->iIsLocked)						// clear when locked
					nextCMD42Failed = ETrue;
				else														// clear when unlocked
					{
					if (iCardInfo[targetCard]->iPWD->Compare(pwd) != 0)		// clear when unlocked with wrong password
						nextCMD42Failed = ETrue;
					else													// clear when unlocked with right password
						{
						// Clear from password store 
						iCardInfo[targetCard]->iPWD->Zero();
						iCardInfo[targetCard]->iIsLocked = EFalse;
						nextCMD42Failed = EFalse;
						
						// Clear from environment settings
						TInt cardNum=(targetCard==0) ? *Wins::CurrentPBusDevicePtr() : 0; // Can't be -1 at this stage
						SetMediaPasswordEnvironmentVar(MMCSocket()->iSocketNumber,cardNum,*(iCardInfo[targetCard]->iPWD));
						}
					}
				}
			else if ((cmd_byte & KMMCLockUnlockSetPwd) == 0)	// SET_PWD == 0: unlock
				{
				__ASSERT_DEBUG(
					pwd_len >= 0 && pwd_len <= KMaxMediaPassword,
					DWinsSDIOStack::Panic(DWinsSDIOStack::EWinsMMCCorruptCommand) );
				
				if (! iCardInfo[targetCard]->iIsLocked)						// unlock when unlocked
					nextCMD42Failed = ETrue;
				else
					{
					if (iCardInfo[targetCard]->iPWD->Compare(pwd) != 0)		// unlock when locked with wrong password
						nextCMD42Failed = ETrue;
					else													// unlock when locked with right password
						{
						iCardInfo[targetCard]->iIsLocked = EFalse;
						nextCMD42Failed = EFalse;
						}
					}
				}
			else /* ((cmd_byte & KMMCLockUnlockSetPwd) != 0) */	// SET_PWD == 1
				{
				__ASSERT_DEBUG(
					cmd_byte & KMMCLockUnlockSetPwd,
					DWinsSDIOStack::Panic(DWinsSDIOStack::EWinsMMCCorruptCommand) );

				// if pwd_len < iCardInfo[targetCard]->iPWD->Length() then data block must be invalid.
				// This can be caused by bad user input rather than inaccurate formation.
				if (!(	pwd_len >= iCardInfo[targetCard]->iPWD->Length()
					&&	pwd_len <= iCardInfo[targetCard]->iPWD->Length() + KMaxMediaPassword ))
					{
					nextCMD42Failed = ETrue;
					}
				else
					{
					const TInt old_pwd_len = iCardInfo[targetCard]->iPWD->Length();
					TPtrC8 old_pwd(cmd.iDataMemoryP + 2, old_pwd_len);
					TPtrC8 new_pwd(cmd.iDataMemoryP + 2 + old_pwd_len, pwd_len - old_pwd_len);

					// card must not be locked and supplied current password must be correct
					if (iCardInfo[targetCard]->iIsLocked || iCardInfo[targetCard]->iPWD->Compare(old_pwd) != 0)
						nextCMD42Failed = ETrue;
					else
						{
						// Set in password store
						iCardInfo[targetCard]->iPWD->Copy(new_pwd);
						nextCMD42Failed = EFalse;
						
						// Set in environment settings
						TInt cardNum=(targetCard==0) ? *Wins::CurrentPBusDevicePtr() : 0; // Can't be -1 at this stage
						SetMediaPasswordEnvironmentVar(MMCSocket()->iSocketNumber,cardNum,*(iCardInfo[targetCard]->iPWD));
						}
					}
				}	// else /* ((cmd_byte & KMMCLockUnlockSetPwd) != 0) */
			}	// case ECmdLockUnlock
			break;

		// ------------------------------------------------------------------
		case ECmd5:
			{
			if (!iCardInfo[iAddressedCard]->iIsSDCard)
				{
				rto = ETrue;
				}
			else
				{
				// bit31 is set to indicate cards are not still powering up
				TUint32 r5 = 0;
				
				r5 |= KWinsSdioFunctionCount << KSDIOFunctionCountShift;
				r5 |= KWinsSdioMemoryPresent ? KSDIOMemoryPresent : 0;
				r5 |= KMMCWinsCardOCRValue;
				r5 |= KSDIOReady;

				TMMC::BigEndian4Bytes(cmd.iResponse, r5);
				}
			}
			break;

		case ECmd52:
			{
			if (!iCardInfo[iAddressedCard]->iIsSDCard)
				{
				rto = ETrue;
				}
			else
				{
				const TUint32 address  = (cmd.iArgument >> KSdioCmdAddressShift)  & KSdioCmdAddressMask;
				const TUint32 function = (cmd.iArgument >> KSdioCmdFunctionShift) & KSdioCmdFunctionMask;
				
				const TUint32 ioAddress = address + (0x100*function);

				const SRegisterMapInfo* entry = NULL;
				entry = FindIoEntryFromAddress(IoMapTop, ioAddress);
				
				if(!entry)
					{
					rto = ETrue;
					}
				else
					{
					if((cmd.iArgument & KSdioCmdDirMask) == KSdioCmdRead)
						{
						TUint8 dataVal = 0;
						if(entry->iAccessFunction)
							{
							entry->iAccessFunction(targetCard, entry->iRegisterID, this, ETrue, dataVal);
							}
						
						if(entry->iDataP)
							{
							TUint entryOffset = ioAddress - entry->iAddress;
							if(entryOffset >= 0 && entryOffset < entry->iLength)
								{
								dataVal = ((TUint8*)entry->iDataP)[entryOffset];
								}
							}

						TUint32 r5 = 0;

						r5 |= dataVal;
						r5 |= 0x1000;

						TMMC::BigEndian4Bytes(cmd.iResponse, r5);
						}
					else
						{
						const TBool raw = (cmd.iArgument & KSdioCmdRAW) ? ETrue : EFalse;
						TUint8 data = (TUint8)(cmd.iArgument & KSdioCmdDataMask);

						if(entry->iDataP)
							{
							*(TUint8*)(entry->iDataP) &= ~(entry->iFlags);
							*(TUint8*)(entry->iDataP) |= (data & entry->iFlags);
							}
						
						if(entry->iAccessFunction)
							{
							entry->iAccessFunction(targetCard, entry->iRegisterID, this, EFalse, data);
							}
						
						TUint32 r5 = 0;
							
						if(raw)
							{
							r5 |= data;
							}

//						r5 |= 0x1000;
						r5 |= 0x2000;

						TMMC::BigEndian4Bytes(cmd.iResponse, r5);
						}			
					}
				}
			}
			break;

		case ECmd53:
			{
			TBool a = EFalse;
			if(a)
				{
				return(KMMCErrDataTimeOut);
				}

			if (!iCardInfo[iAddressedCard]->iIsSDCard)
				{
				rto = ETrue;
				}
			else
				{
				const TUint32 address  = (cmd.iArgument >> KSdioCmdAddressShift)  & KSdioCmdAddressMask;
				const TUint32 function = (cmd.iArgument >> KSdioCmdFunctionShift) & KSdioCmdFunctionMask;
				
				const TUint32 ioAddress = address + (0x100*function);

				if((cmd.iArgument & KSdioCmdBlockMode) == KSdioCmdBlockMode)
					{
					// Block mode not supported (yet)
					rto = ETrue;
					}
				else
					{
					TUint32 byteCount = cmd.iArgument & KSdioCmdCountMask;
					TUint32 count = 0;
					TUint32 currentAddress = ioAddress;

					TUint32 inc = ((cmd.iArgument & KSdioCmdAutoInc) == KSdioCmdAutoInc) ? 1 : 0;
		
					while(count < byteCount)
						{
						const SRegisterMapInfo* entry = NULL;
						entry = FindIoEntryFromAddress(IoMapTop, currentAddress);

						if(entry)
							{
							if((cmd.iArgument & KSdioCmdDirMask) == KSdioCmdRead)
								{
								TUint8 dataVal = 0;
								if(entry->iAccessFunction)
									{
									entry->iAccessFunction(targetCard, entry->iRegisterID, this, ETrue, dataVal);
									}
								
								if(entry->iDataP)
									{
									TUint entryOffset = currentAddress - entry->iAddress;
									if(entryOffset >= 0 && entryOffset < entry->iLength)
										{
										dataVal = ((TUint8*)entry->iDataP)[entryOffset];
										}
									}

								cmd.iDataMemoryP[count] = dataVal;
								}
							else
								{
								TUint8 data = cmd.iDataMemoryP[count];

								if(entry->iDataP)
									{
									TUint entryOffset = currentAddress - entry->iAddress;
									if(entryOffset >= 0 && entryOffset < entry->iLength)
										{
										((TUint8*)entry->iDataP)[entryOffset] &= ~(entry->iFlags);
										((TUint8*)entry->iDataP)[entryOffset] |= (data & entry->iFlags);
										}

									}
								
								if(entry->iAccessFunction)
									{
									entry->iAccessFunction(targetCard, entry->iRegisterID, this, EFalse, data);
									}								
								}			
							}
							
							count++;
							currentAddress += inc;
						}

						TUint32 r5 = 0;
						
//						r5 |= 0x2000;
						r5 |= 0x1000;

						TMMC::BigEndian4Bytes(cmd.iResponse, r5);
					}
				}
			}
			break;

		// ------------------------------------------------------------------
		default:
			break;
		}

	if (rto)
		return(KMMCErrResponseTimeOut);

	cmd.iCommand = origCmd;
	// If this is an R1 or R1b response type command then return card status as a response
	if (	targetCard != -1
		&&	(cmd.iSpec.iResponseType==ERespTypeR1 || cmd.iSpec.iResponseType==ERespTypeR1B) )
		{
		TUint32 resp(
				iCardInfo[targetCard]->iState
			|	((iCardInfo[targetCard]->iIsLocked ? 1 : 0) << 25)
			|	((lock_unlock_failed ? 1 : 0) << 24) );

		if (iCMD42Failed)								// previous CMD42
			{
			resp |= KMMCStatErrLockUnlock;
			nextCMD42Failed = EFalse;
			}
		iCMD42Failed = nextCMD42Failed;
		TMMC::BigEndian4Bytes(&cmd.iResponse[0],resp); // Ignore bits 47-40
		}
	SMF_END
	}

TInt DWinsSDIOStack::AccessIoEnable(TInt /*aTargetCard*/, TInt /*aVal*/, TAny* aSelfP, TBool aRead, TUint8& aData)
//
// Access the IO Enable register
//
	{
	DWinsSDIOStack& self = *(DWinsSDIOStack*)aSelfP;
	
	if(aRead)
		{
		aData = GCCCRRegIoEnable;
		}
	else
		{
		TUint8 mask = 0;
		for(TInt i=0; i<KWinsSdioFunctionCount; i++)
			{
			mask |= (0x02 << i);
			}

		aData &= mask;

		// Disable functions first...
		GFunctionToEnable &= aData;
		GCCCRRegIoReady   &= aData;
		GCCCRRegIoEnable  &= aData;
		
		// Enabling any functions - This uses the delayed timer...
		if((GCCCRRegIoEnable & aData) != aData)
			{
			GFunctionToEnable = GCCCRRegIoEnable ^ aData;
			GCCCRRegIoEnable |= GFunctionToEnable;

			self.iEnableTimer.OneShot(KFunctionEnableDelay_uS / NKern::TickPeriod());
			}
		}

	return(KErrNone);
	}

void DWinsSDIOStack::EnableTimerCallback(TAny* /*aSelfP*/)
	{
	GCCCRRegIoReady |= GFunctionToEnable;
	}

TInt DWinsSDIOStack::AccessCsaWindow(TInt aTargetCard, TInt /*aVal*/, TAny* aSelfP, TBool aRead, TUint8& aData)
//
// Access the CSA Windoe
//
	{
	TMMCErr err = KErrNone;

	DWinsSDIOStack& self = *(DWinsSDIOStack*)aSelfP;
	
	HANDLE winHandle = self.iCardInfo[aTargetCard]->iAreaHandles[KSDIOArea];

    if (SetFilePointer(winHandle, GFBR1RegCsaPtr, NULL,FILE_BEGIN) == 0xffffffffu)
        err = MapLastErrorMmc();
    else
        {
	    DWORD res;
		TUint8 val = 0;
		TUint len = 1;
		
		BOOL rwRes = FALSE;

		if(aRead)
			{
			rwRes = ReadFile(winHandle, (TAny*)&val, len, &res, NULL);
			}
		else
			{
			val = aData;
			rwRes = WriteFile(winHandle, (TAny*)&val, len, &res, NULL);
			}

		if(rwRes == FALSE)
			{
            err = MapLastErrorMmc();
			}
        else if(res != len)
			{
            err = KMMCErrGeneral;
			}
        else
			{
			if(aRead)
				{
				aData = val;
				}

			GFBR1RegCsaPtr++;
            err = KMMCErrNone;
			}
		}

	return(err);
	}

TInt DWinsSDIOStack::AccessCsaPointer(TInt /*aTargetCard*/, TInt aVal, TAny* /*aSelfP*/, TBool aRead, TUint8& aData)
//
// Access the CSA Windoe
//
	{
	TInt err = KErrNone;

	TUint32 mask  = 0;
	TUint32 shift = 0;

	switch(aVal)
		{
		case KFBRRegCsaPtrLo:
			{
			mask  = 0x0000FF;
			shift = 0;
			break;
			}

		case KFBRRegCsaPtrMid:
			{
			mask  = 0x00FF00;
			shift = 8;
			break;
			}

		case KFBRRegCsaPtrHi:
			{
			mask  = 0xFF0000;
			shift = 16;
			break;
			}

		default:
			{
			err = KErrNotSupported;
			break;
			}
		}

	if(err == KErrNone)
		{
		if(aRead)
			{
			aData = (TUint8)((GFBR1RegCsaPtr & mask) >> shift);
			}
		else
			{
			GFBR1RegCsaPtr &= ~mask;
			GFBR1RegCsaPtr |= (TUint32)aData << shift;
			}
		}
	
	return(err);
	}

void DWinsSDIOStack::EnableSDIOInterrupt(TBool /*aEnable*/)
//
// Virtual
//
	{
	}

void DWinsSDIOStack::SetBusWidth(TUint32 /*aBusWidth*/)
//
// Virtual
//
	{
	}

TUint32 DWinsSDIOStack::MaxBlockSize() const
//
// Virtual
//
	{
	return(512);
	}


TInt DWinsSDIOStack::FindAnyCardInStack(TMMCardStateEnum aState)
//
// first first active card in supplied state.  Return -1 if
// no active card is in supplied state.
//
	{
	if (iAddressedCard != KBroadcastToAllCards)
		return (iCardInfo[iAddressedCard]->iState == aState) ? iAddressedCard : -1;
	else
		{
		for (TInt i = 0; i < KTotalWinsCardSlots; ++i)
			{
			if (iCardInfo[i]->iState == aState)
				return i;
			}

		return -1;
		}
	}

TInt DWinsSDIOStack::FindFirstCardInStack(TMMCardStateEnum aState)
//
// find card which is active on bus and in supplied state.
// There can be more than one active card in the the supplied state,
// but there should be at least one.
//
	{
	if (iAddressedCard != KBroadcastToAllCards)
		{
		__ASSERT_DEBUG(iCardInfo[iAddressedCard]->iState == aState, DWinsSDIOStack::Panic(DWinsSDIOStack::EStkFFCNotSelCard));
		return iAddressedCard;
		}
	else
		{
		TInt idx = -1;
		for (TInt i = 0; idx != -1 && i < KTotalWinsCardSlots; ++i)
			{
			if (iCardInfo[i]->iState == aState)
				idx = i;
			}

		__ASSERT_DEBUG(idx != -1, DWinsSDIOStack::Panic(DWinsSDIOStack::EStkFFCNoneSel));
		return idx;
		}
	}

TInt DWinsSDIOStack::FindOneCardInStack(TMMCardStateEnum aState)
//
// find card which is active on bus and in supplied state.
// There should be exactly one active card in the supplied state.
//
	{
	if (iAddressedCard != KBroadcastToAllCards)
		{
		__ASSERT_DEBUG(iCardInfo[iAddressedCard]->iState == aState, DWinsSDIOStack::Panic(DWinsSDIOStack::EStkFOCNotSelCard));
		return iAddressedCard;
		}
	else
		{
		TInt idx = -1;
		for (TInt i = 0; i < KTotalWinsCardSlots; ++i)
			{
			if (iCardInfo[i]->iState == aState)
				{
				__ASSERT_DEBUG(idx == -1, DWinsSDIOStack::Panic(DWinsSDIOStack::EStkFOCMultiSel));
				idx = i;
				}
			}

		__ASSERT_DEBUG(idx != -1, DWinsSDIOStack::Panic(DWinsSDIOStack::EStkFOCNoneSel));
		return idx;
		}
	}


// ======== DWinsMMCMediaChange ========

#pragma warning( disable : 4355 )	// this used in initializer list
DWinsMMCMediaChange::DWinsMMCMediaChange(TInt aMediaChangeNum)
	: DMMCMediaChange(aMediaChangeNum),
      iDoorClosedCount(0),
	  iMediaChangeEnable(ETrue),
	  iStackP(NULL)
	{
	iMediaDoorCloseReload=2; 	// Units: In theory-20ms, Actual-100ms
	}
#pragma warning( default : 4355 )

TInt DWinsMMCMediaChange::Create()
//
// Initialiser.
//
	{	
	return(DMediaChangeBase::Create());
	}

void DWinsMMCMediaChange::DoorOpenService()
//
// Handle the media change (this function, never postponed is called on media
// change interrupt). 
//
	{
	Disable();		// Disable interrupt until door closes again.
	iDoorOpenDfc.Enque();
	}

void DWinsMMCMediaChange::DoDoorOpen()
//
// Handle media door open (called on media door open interrupt). 
//
	{
	iDoorClosedCount=iMediaDoorCloseReload;
	// Just start a ticklink to poll for door closing
	iTickLink.Periodic(KMediaChangeTickInterval,DWinsMMCMediaChange::Tick,this);
    }

void DWinsMMCMediaChange::DoDoorClosed()
//
// Handle media door closing (called on media door open interrupt).
//
	{

	iTickLink.Cancel();	// Doesn't matter if wasn't enabled
	Enable();	// Re-enable door interrupts

	// While the door was open the user may have changed the card in slot 0
	if (iStackP && *Wins::CurrentPBusDevicePtr()>=0)
		iStackP->iCardInfo[0]=iStackP->iCardPool[*Wins::CurrentPBusDevicePtr()];
	}

void DWinsMMCMediaChange::ForceMediaChange()
//
// Force media change
//
	{
	DoorOpenService();
	}

TMediaState DWinsMMCMediaChange::MediaState()
//
// Return status of media changed signal.
//
	{

	if (iDoorClosedCount>0)
		return(EDoorOpen);
	return( (*Wins::MediaDoorOpenPtr())?EDoorOpen:EDoorClosed);
	}

void DWinsMMCMediaChange::Tick(TAny *aPtr)
//
// Called on the tick to poll for door closing (called on DFC).
//
	{

	((DWinsMMCMediaChange*)aPtr)->TickService();
	}

void DWinsMMCMediaChange::TickService()
//
// Called on the tick to poll for door closing (called on DFC).
//
	{

	__ASSERT_DEBUG(iDoorClosedCount>=0,DWinsSDIOStack::Panic(DWinsSDIOStack::EWinsMMCMediaChangeTickFault));
	if (!(*Wins::MediaDoorOpenPtr()))
		{
		if (iDoorClosedCount > 0)
			iDoorClosedCount--;
		if (iDoorClosedCount == 0)
			DoorClosedService();
		}
	else
		iDoorClosedCount=iMediaDoorCloseReload; // Door open so start again.
	}

void DWinsMMCMediaChange::Enable()
//
// Enable media change 
//
	{

	iMediaChangeEnable=ETrue;
	}

void DWinsMMCMediaChange::Disable()
//
// Disable media change
//
	{

	iMediaChangeEnable=EFalse;
	}

void DWinsMMCMediaChange::MediaChangeCallBack(TAny *aPtr)
//
// Static called on media change
//
	{

	DWinsMMCMediaChange* mc=(DWinsMMCMediaChange*)aPtr;
	if (mc!=NULL&&mc->iMediaChangeEnable)
		mc->DoorOpenService();
	}


// ======== TWinsCardInfo ========

void TWinsCardInfo::GetCSD(TUint8* aResp) const
	{
	// Bits 127-96
	TUint32 csd=(0x1<<30); 	/* CSD_STRUCTURE: CSD Version No 1.1 */
	csd|=		(0x2<<26); 	/* SPEC_VERS: Version 2.1 */
	csd|=		(0x0E<<16);	/* TAAC: 1mS */  
	csd|=		(0x0A<<8);	/* NSAC: 1000 */  
	csd|=		(0x59);		/* TRAN_SPEED: 5.0Mbit/s */  
	TMMC::BigEndian4Bytes(&aResp[0],csd);
	// Bits 95-64
	const TUint32 ccc = 
			KMMCCmdClassBasic | KMMCCmdClassBlockRead
		|	KMMCCmdClassBlockWrite | KMMCCmdClassLockCard;
	csd=		(ccc<<20); 	/* CCC: classes 0, 2, 4, and 7 */
	csd|=		(0x9<<16); 	/* READ_BL_LEN: 512 bytes */
	csd|=		(0x0<<15);	/* READ_BL_PARTIAL: No */  
	csd|=		(0x0<<14);	/* WRITE_BLK_MISALIGN: No */  
	csd|=		(0x0<<13);	/* READ_BLK_MISALIGN: No */  
	csd|=		(0x0<<12);	/* DSR_IMP: No DSR */ 
	csd|=		(0x0<<8);	/* C_SIZE: 1Mb */
	csd|=		(0x7F);		/* C_SIZE: 1Mb (cont)*/  
	TMMC::BigEndian4Bytes(&aResp[4],csd); 
	// Bits 63-32
	csd=		(3UL<<30); 	/* C_SIZE: 2Mb (cont) */
	csd|=		(0x1<<27); 	/* VDD_R_CURR_MIN: 1mA */
	csd|=		(0x1<<24);	/* VDD_R_CURR_MAX: 5mA */  
	csd|=		(0x2<<21); 	/* VDD_W_CURR_MIN: 5mA */
	csd|=		(0x3<<18);	/* VDD_W_CURR_MAX: 25mA */  
	csd|=		(0x0<<15);	/* C_SIZE_MULT: 0 */  
	if (! iIsSDCard)
		{
		csd|=		(0x0<<10);	/* SECTOR_SIZE: 1 write block */  
		csd|=		(0x0<<5);	/* ERASE_GRP_SIZE: 1 sector */  
		csd|=		(0x0);		/* WP_GRP_SIZE: 1 erase group */  
		}
	else
		{
		csd |= (0x00 << (46 - 32));	// ERASE_BLK_EN
		csd |= (0x1f << (39 - 32));	// SECTOR_SIZE: 32 write blocks
		csd |= (0x00 << (32 - 32));	// WP_GRP_SIZE: 1 erase sector.
		}
	TMMC::BigEndian4Bytes(&aResp[8],csd); 
	// Bits 31-0
	csd=		(0x0<<31); 	/* WP_GRP_ENABLE: No */
	csd|=		(0x0<<29); 	/* DEFAULT_ECC: ? */
	csd|=		(0x3<<26);	/* R2W_FACTOR: 8 */  
	csd|=		(0x9<<22); 	/* WRITE_BL_LEN: 512 bytes */
	csd|=		(0x0<<21);	/* WRITE_BL_PARTIAL: No */  
	csd|=		(0x0<<15);	/* FILE_FORMAT_GRP: Hard disk */  
	csd|=		(0x0<<14);	/* COPY: original */  
	csd|=		(0x0<<13);	/* PERM_WRITE_PROTECT: No */  
	csd|=		(0x0<<12);	/* TMP_WRITE_PROTECT: No */  
	csd|=		(0x0<<10);	/* FILE_FORMAT: Hard disk */  
	csd|=		(0x0<<8);	/* ECC: None */  
	csd|=		(0x0<<1);	/* CRC: ? */  
	csd|=		(0x1);		/* not used */  
	TMMC::BigEndian4Bytes(&aResp[12],csd);
	}

// ======== DWinsSDIOPsu ========


DWinsSDIOPsu::DWinsSDIOPsu(TInt aVccNum, TInt aMcId)
	: DSDIOPsu(aVccNum, aMcId)
	{}

void DWinsSDIOPsu::Init()
//
// Initialise the PSU
//
    {
	// Nothing to do
    }

void DWinsSDIOPsu::DoSetState(TPBusPsuState aState)
//
// Turn on/off the PSU. If it is possible to adjust the output voltage on this
// PSU then retreive the required voltage level from TMMCPsu::iVoltageSetting
// (which is in OCR register format).
//
    {

    switch (aState)
        {
        case EPsuOff:
            break;
        case EPsuOnFull:
            break;
        case EPsuOnCurLimit:
            break;
        }
    }

TInt DWinsSDIOPsu::VoltageInMilliVolts()
//
// Return the level of the PSU (in mV) or -ve if error.
//
    {

    return(0);
    }

void DWinsSDIOPsu::DoCheckVoltage()
//
// Check the voltage level of the PSU is as expected. Returns either KErrNone, KErrGeneral 
// to indicate the pass/fail state or KErrNotReady if the voltage check isn't complete.
//
    {

	ReceiveVoltageCheckResult(KErrNone);
    }

void DWinsSDIOPsu::PsuInfo(TPBusPsuInfo &anInfo)
//
// Return machine info relating to the MMC PSU supply
//
    {

	anInfo.iVoltageSupported=0x00040000; // 3.0V (OCR reg. format).
	anInfo.iMaxCurrentInMicroAmps=0;
	anInfo.iVoltCheckInterval=0;
	anInfo.iVoltCheckMethod=EPsuChkComparator;

	anInfo.iNotLockedTimeOut=5;
	anInfo.iInactivityTimeOut=10;
    }
