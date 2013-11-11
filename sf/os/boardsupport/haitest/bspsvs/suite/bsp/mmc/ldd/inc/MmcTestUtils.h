/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef __MMC_TEST_UTILS_H__
#define __MMC_TEST_UTILS_H__


#ifdef __KERNEL_MODE__
#include <mmc.h>
#include <locmedia.h>
#endif  //  __KERNEL_MODE__


//Test Types
enum TMmcTestType
	{
	EMmcGeneralTest,
	EMmcTestPsu,
	EMmcTestMediaChange,
	EMmcTestStack,
	EMmcTestSocket
	};

//Bus States
enum TBusState
	{
	EBusCardAbsent=0,
	EBusOff=1,
	EBusPoweringUp=2,
	EBusOn=3,
	EBusPsuFault=4,
	EBusPowerUpPending=5,
	};

//Psu States
enum TDPsuState
	{
	EDPsuOff,
	EDPsuOnCurLimit,
	EDPsuOnFull
	};
	
//Voltage Check Method
enum TDPsuVoltChkMethod
	{
	EDPsuChkComparator,
    EDPsuChkADCType,
    EDPsuChkADCType2
    };

//Media States
enum TMediaStates
	{
	EOpened,
	EClosed,
	};

//Bus Types
enum TCardBusType
	{
	EBusTypeNone,
	EBusTypePcCard,
	EBusTypeMultiMedia,
	EBusTypeUSB,
	};

//Media Types
enum TMmcMediaType
	{
	EMmcROM,
	EMmcFlash,
	EMmcIO,
	EMmcOther,
	EMmcNotSupported
	};
	
#ifdef __KERNEL_MODE__
	//Used for Password Store
	const TInt KPasswordStoreCIDLength = KMMCCIDLength + sizeof(TInt32);	
#endif
	
const TInt KMmcPwdLen = 16;

/**
 * Stores test case enums as 2 16 bit values from supplied TInt
 *
 */
class TTCFunctionId
	{
public:

#ifdef __KERNEL_MODE__

	explicit TTCFunctionId(TInt aValue) : iValue(aValue) {}

#endif

	TTCFunctionId(TInt aFunctionId, TMmcTestType aTestType) 
		{ iValue =  ((aFunctionId << 16 ) + (aTestType & 0xFFFF)); }  

	
	TMmcTestType TestType() 
		{ return (TMmcTestType)(iValue & 0xFFFF); }  // TestType stored in lower 16 bits	

	TInt FunctionId() 
		{ return (iValue >> 16); } 

	TInt iValue;
	};	

/**
 * Used to wrap data passed to kernel side for DMMCStack::PartialRead and DMMCSocket::PartialRead
 *
 */
class TPartialReadData
	{
public:
	TUint iStart;
	TUint iEnd;
	TUint iPhysStart;
	TUint iPhysEnd;		
	};

/**
 * Used to wrap data passed to kernel side for DMMCStack::ReadPasswordData
 *
 */
class TStackPasswordStoreData
	{
	
public:
	TStackPasswordStoreData() : iPwdStorePtr(NULL) {}
	TBuf8<KMmcPwdLen> 		iPassword;
	TAny*					iPwdStorePtr; 			
	
#ifdef __KERNEL_MODE__
	TBool	CreateFormattedPassword(TDes8 &aFormattedPassword, const TCID& aCID);
	void	SetFormattedPassword(const TDesC8 &aFormattedPassword);
#endif
	};

/**
 * Used to wrap data passed to kernel side for DMMCSocket::PrepareStore
 *
 */
class TPasswordPrepareStoreData
	{
	
public:
	TPasswordPrepareStoreData() {}
	TInt					iBus;
	TInt				 	iFunc;
	TBuf8<KMmcPwdLen> 		iOldPassword;
	TBuf8<KMmcPwdLen> 		iNewPassword;
	};

/**
 * Class used for retrieving buffer info (socket & stack)
 *
 */
class TBufferInfo
	{
public:
	TBufferInfo() : iBuf(NULL), iBufLen(0), iMinorBufLen(0) {}
	TUint8*	iBuf;			// pointer to start of buffer
	TInt 	iBufLen;		// the length of allocated buffer
	TInt 	iMinorBufLen;  	// the length of minor buffer (only valid from DMMCStack::BufferInfo)
	};

/**
 * Used to wrap data passed to kernel side for DMMCStack::CardP
 *
 */
class TCardPtr
	{
public:
TCardPtr() : iCardPtr(NULL), iCardNumber(0) {}
	TAny* 	iCardPtr;		// pointer to TMMCard
	TInt 	iCardNumber;	// the card number
	};
		
/**
 * Used to wrap data passed to kernel side for DMMCSocket::Stack
 *
 */
class TStackPtr
	{
public:
TStackPtr() : iStackPtr(NULL), iBus(0) {}
	TAny* 	iStackPtr;		// pointer to DMMCStack
	TInt 	iBus;			// the bus number
	};
	
/**
 * Class used for retrieving machine info (socket & stack)
 *
 */
class TMachineInfo
	{
public:

#ifdef __KERNEL_MODE__
	TMachineInfo(const TMMCMachineInfo& aMachineInfo);
#else
	TMachineInfo() : iTotalSockets(0), iTotalMediaChanges(0), iTotalPrimarySupplies(0), iSPIMode(EFalse), iBaseBusNumber(0)
					, iSupportsSPIMode(EFalse), iSupportsDoubleBuffering(EFalse), iSupportsR7(EFalse), iDma8BitAddressing(EFalse)
					, iDma16BitAddressing(EFalse), iDma32BitAddressing(EFalse), iDma64BitAddressing(EFalse), iSupportsDMA(EFalse)
					, iMaxTransferLength_256K(EFalse), iMaxTransferLength_512K(EFalse), iMaxTransferLength_1M(EFalse)
					, iMaxTransferLength_2M(EFalse), iMaxTransferLength_4M(EFalse), iMaxTransferLength_8M(EFalse), iMaxTransferLength_16M(EFalse)
					{}
#endif
	
public:
	/** The total number of MultiMediaCard slots for this stack */
	TInt	iTotalSockets;
	/** Not currently used */
	TInt	iTotalMediaChanges;
	/** Not currently used */
	TInt	iTotalPrimarySupplies;
	/** Indicates whether the SPI protocol is being used or not */
	TBool	iSPIMode;
	/** The number of the first peripheral bus slot claimed by the MultiMediaCard controller */
	TInt	iBaseBusNumber;
	/** Set if hardware supports SPI mode */
	TBool	iSupportsSPIMode;
	/** Set if the PSL is enabled for double-buffered data transfers */
	TBool	iSupportsDoubleBuffering;
	/** Set if the PSL supports response type R7 */
	TBool	iSupportsR7;
	/** Set this bit if the hardware DMA controller utilises 8-Bit Addressing */
	TBool	iDma8BitAddressing;
	/** Set this bit if the hardware DMA controller utilises 16-Bit Addressing */
	TBool	iDma16BitAddressing;
	/** Set this bit if the hardware DMA controller utilises 32-Bit Addressing */
	TBool	iDma32BitAddressing;
	/** Set this bit if the hardware DMA controller utilises 64-Bit Addressing */
	TBool	iDma64BitAddressing;
	/** Set this if the hardware supports DMA and can cope with being given a physical address */
	TBool	iSupportsDMA;
	/** Set if the hardware is unable to perform data transfers of more than 256K */
	TBool	iMaxTransferLength_256K;
	/** Set if the hardware is unable to perform data transfers of more than 512K */
	TBool	iMaxTransferLength_512K;
	/** Set if the hardware is unable to perform data transfers of more than 1M */
	TBool	iMaxTransferLength_1M;
	/** Set f the hardware is unable to perform data transfers of more than 2M */
	TBool	iMaxTransferLength_2M;
	/** Set if the hardware is unable to perform data transfers of more than 4M */
	TBool	iMaxTransferLength_4M;
	/** Set if the hardware is unable to perform data transfers of more than 8M */
	TBool	iMaxTransferLength_8M;
	/** Set if the hardware is unable to perform data transfers of more than 16M */
	TBool	iMaxTransferLength_16M;
	
	};
				   		   
/**
 * Class used for retrieving card info
 *
 */
class TMMCCardInfo
	{
public:
	inline TMMCCardInfo()
		  {memset(this,0x00,sizeof(TMMCCardInfo)); iMediaType=EMmcNotSupported;}
public:
	/** Indicates the card is Ready */
	TBool iIsReady;
	/** Indicates the card is locked */
	TBool iIsLocked;
	/** Holds Card CID register */
	TUint8 iCID[16];
	/** Holds Card CSD register */
	TUint8 iCSD[16];
	/** Card address */
	TUint16 iRCA;
	/** MMC Media type */
	TMmcMediaType iMediaType;
	/** Device Size*/
    TInt64 iCardSizeInBytes;
    /** Maximum Read Block Length  */
    TInt iMaxReadBlLen;
	/** READ_BL_LEN*/
	TUint iReadBlLen;
	/** Max Write data block length */
	TUint iWriteBlLen;
	/** Is partial block read allowed */
	TBool iReadBlPartial;
	/** Is partial block write allowed */
	TBool iWriteBlPartial;
	/** Is read block misalignment allowed */
	TBool iReadBlkMisalign;
	/** Is write block misalignment allowed */
	TBool iWriteBlkMisalign;
	/** Maximum read current in milli ampere */
    TInt iReadCurrentInMilliAmps;
    /** Maximum write current in milli ampere */
    TInt iWriteCurrentInMilliAmps;
    /** MMC/SD Specification, version */
	TUint iSpecVers;
	/** Asynchronous part of data access time */
	TUint iTAAC;
	/** Defines clock dependent factor of the data access time */
	TUint iNSAC;
	/** Clock frequency */
	TUint iTransferSpeed;
	/** Card command class */
	TUint iCommandRegister;
	/** This will be updated to ETrue if the card belongs to high capacity */
	TBool iHighCapacity;
	/** Flag to indentity card type  */
	TUint32 iFlags;
	
#ifdef __KERNEL_MODE__
	void SetCardInfo(const TMMCard& aCard);
#endif
	
	};
	
/**
 * Class used for retrieving SD card info
 *
 */
class TSDCardInfo : public TMMCCardInfo
	{
public:
	inline TSDCardInfo() : iBusWidth(0), iProtectedAreaSize(0)
	{;}

public:
	TInt iBusWidth;
	TUint32 iProtectedAreaSize;
};


/**
 * Mirrors the DMMCStack::DemandPagingInfo on the user side
 *
 */
class TTCDemandPagingInfo	
	{
public:
	const TInt* iPagingDriveList;
	TInt iDriveCount;
	TUint iPagingType;
	TInt iReadShift;
	TUint iNumPages;
	TBool iWriteProtected;
	};
	
/**
 * Class used for retrieving extended CSD info
 *
 */
class TExtendedCSDInfo
	{
public:
	
#ifdef __KERNEL_MODE__
	TExtendedCSDInfo(const TExtendedCSD& aExtendedCSD);
#else
	inline TExtendedCSDInfo() {memset(this,0x00,sizeof(TExtendedCSDInfo));}
#endif
	
	// this class holds extended csd info as members rather than bitmasks on a 128 bit buffer
	// as described in the mmc spec.  It is also neccessary as mmc.h cannot be included in user side code 

public:
	TUint	iSupportedCmdSet;
	TUint	iSectorCount;
	TUint	iMinPerfWrite8Bit52Mhz;
	TUint	iMinPerfRead8Bit52Mhz;
	TUint	iMinPerfWrite8Bit26Mhz_4Bit52Mhz;
	TUint	iMinPerfRead8Bit26Mhz_4Bit52Mhz;
	TUint	iMinPerfWrite4Bit26Mhz;
	TUint	iMinPerfRead4Bit26Mhz;
	TUint	iPowerClass26Mhz360V;
	TUint	iPowerClass52Mhz360V;
	TUint	iPowerClass26Mhz195V;
	TUint	iPowerClass52Mhz195V;
	TUint	iCardType;
	TUint	iCSDStructureVer;
	TUint	iExtendedCSDRev;
	TUint	iCmdSet;
	TUint	iCmdSetRev;
	TUint	iPowerClass;
	TUint	iHighSpeedTiming;
	};
	
/**
 * Class used for retrieving CSD info
 *
 */
class TCSDInfo
	{
	public:
	
#ifdef __KERNEL_MODE__
	TCSDInfo(const TCSD& aCSD);
#else 
	inline TCSDInfo() {memset(this,0x00,sizeof(TCSDInfo)); iMediaType=EMultiMediaNotSupported;}
#endif
	
	// mirrors TMMCMediaTypeEnum
	enum TMMCMediaTypeEnumInfo { EMultiMediaROM, EMultiMediaFlash, EMultiMediaIO, EMultiMediaOther, EMultiMediaNotSupported };
	 
	// this class holds csd info as members rather than bitmasks on a 128 bit buffer
	// as described in the mmc spec.  It is also neccessary as mmc.h cannot be included in user side code 
	
public:
	TUint iCSDStructure;
	TUint iSpecVers;
	TUint iReserved120;
	TUint iTAAC;
	TUint iNSAC;
	TUint iTranSpeed;
	TUint iCCC;
	TUint iReadBlLen;
	TBool iReadBlPartial;
	TBool iWriteBlkMisalign;
	TBool iReadBlkMisalign;
	TBool iDSRImp;
	TUint iReserved74;
	TUint iCSize;
	TUint iVDDRCurrMin;
	TUint iVDDRCurrMax;
	TUint iVDDWCurrMin;
	TUint iVDDWCurrMax;
	TUint iCSizeMult;
	TUint iEraseGrpSize;
	TUint iEraseGrpMult;
	TUint iWPGrpSize;
	TBool iWPGrpEnable;
	TUint iDefaultECC;
	TUint iR2WFactor;
	TUint iWriteBlLen;
	TBool iWriteBlPartial;
	TUint iReserved16;
	TBool iFileFormatGrp;
	TBool iCopy;
	TBool iPermWriteProtect;
	TBool iTmpWriteProtect;
	TUint iFileFormat;
	TUint iECC;
	TUint iCRC;
	TUint iDeviceSize;
	TMMCMediaTypeEnumInfo iMediaType;
	TUint iReadBlockLength;
	TUint iWriteBlockLength;
	TUint iEraseSectorSize;
	TUint iEraseGroupSize;
	TUint iMinReadCurrentInMilliamps;
	TUint iMinWriteCurrentInMilliamps;
	TUint iMaxReadCurrentInMilliamps;
	TUint iMaxWriteCurrentInMilliamps;
	TUint iMaxTranSpeedInKilohertz;

	};	


#endif // MMC_TEST_UTILS

