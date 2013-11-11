// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PP_MMC.CPP
// 
//

#include "plat_priv.h"
#include <property.h>
#include "variant.h"
#include "variantmediadef.h"
#include "mmci.h" 

//#define  __CARD0_NOT_LOCKABLE__
//#define  __CARD1_NOT_LOCKABLE__

const TInt  KDiskSectorSize=512;

TInt  DWinsMMCStack::TotalMDiskSize=0;		///< mmc card size for emulator
TUint DWinsMMCStack::CSIZE=0;				///< mmc card size field
TUint DWinsMMCStack::CSIZE_MULT=0;			///< mmc card size field

TInt  DWinsMMCStack::TotalWinsMMC_CardSlots = KDefault_TotalWinsCardSlots; ///< total number of MMC card slots for the emulator
TInt  DWinsMMCStack::TotalWinsMMC_Cards     = KDefault_TotalWinsCards;     ///< total number of MMC cards for the emulator


const TUint32 KCsdStructure = 0x01;	/* CSD Version No 1.1 */
const TUint32 KCsdSpecVers  = 0x03;	/* Version 3.1 */

TInt MapLastErrorEpoc()
//
// Map an NT error to an Epoc/32 error.
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

TMMCErr MapLastErrorMmc()
//
// Map an NT error to a TMMCErr error.
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


void DWinsMMCStack::MachineInfo(TMMCMachineInfo& aMachineInfo)
//
// Return machine info relating to this MultiMediaCard Stack
//
	{
	aMachineInfo.iTotalSockets=TotalWinsMMC_CardSlots;
	aMachineInfo.iTotalMediaChanges=0;  		// Not used at present
	aMachineInfo.iTotalPrimarySupplies=0;		// Not used at present

	aMachineInfo.iFlags = TMMCMachineInfo::ESupportsDoubleBuffering;
    aMachineInfo.iBaseBusNumber=0;

	__KTRACE_OPT(KPBUS1,Kern::Printf("<WinsMMC:MachineInfo"));
	__KTRACE_OPT(KPBUS1,Kern::Printf(" %d stacks",aMachineInfo.iTotalSockets));

	__ASSERT_DEBUG(aMachineInfo.iTotalSockets<=KMaxMMCardsPerStack,Panic(EWinsMMCBadMachineInfo));
	}

void DWinsMMCStack::AdjustPartialRead(
	const TMMCard* aCard, TUint32 aStart, 
#ifdef _DEBUG
	TUint32 aEnd, 
#else
	TUint32 /*aEnd*/, 
#endif
	TUint32* aPhysStart, TUint32* aPhysEnd) const
	{
	const TUint32 blkLen = 1 << aCard->MaxReadBlLen();
	const TUint32 blkMsk = blkLen - 1;
#ifdef _DEBUG
	__ASSERT_DEBUG(aCard->CSD().ReadBlPartial(), Panic(EWinsMMCAPRNotSupp));
	__ASSERT_DEBUG(aEnd - aStart <= blkLen, Panic(EWinsMMCAPRRange));
	__ASSERT_DEBUG((aEnd & ~blkMsk) > (aStart & ~blkMsk), Panic(EWinsMMCAPRBoundary));
#endif

	*aPhysStart = aStart & ~blkMsk;
	*aPhysEnd = *aPhysStart + blkLen;
	}

void DWinsMMCStack::GetBufferInfo(TUint8** aMDBuf, TInt* aMDBufLen)
	{
	*aMDBuf = iMDBuf;
	*aMDBufLen = iMDBufLen;
	}

void DWinsMMCStack::Panic(TWinsMMCPanic aPanic)
	{
	_LIT(KPncNm,"PBUS-MMC-WINS");
	Kern::PanicCurrentThread(KPncNm,aPanic);
	}

DWinsMMCStack::DWinsMMCStack(TInt aBus, DMMCSocket* aSocket)
	          :DMMCStack(aBus, aSocket)
	{
	
//	iAddressedCard=0;
//	iSecureArgDevAddr=0;
//	iSecureArgTotalLength=0;
//	iCMD42Failed=EFalse;
	}


/**
    Allocate cards.  Only called at bootup, so no cleanup if fails.
*/
TInt DWinsMMCStack::Init()
	{
	 
    //-- try to read number of mmc cards and slots from epoc.ini file
    const TInt MmcSlots = Property::GetInt("MultiMediaCardSlots");
    const TInt MmcCards = Property::GetInt("MultiMediaCardsNum");
    
    if(MmcSlots == 0 && MmcCards == 0)
        {//-- parameters not specified, do nothing; static variables are initialized with default values
        }
    else
        {
        if((MmcSlots == 0 && MmcCards >0) || (MmcSlots > 0 && MmcCards ==0))
            {//-- only one of the parameters is specified. use it as "Cards quantity"
            TotalWinsMMC_Cards     = Max(MmcSlots, MmcCards); //-- chose non zero value
            TotalWinsMMC_CardSlots = Max(1, TotalWinsMMC_Cards-1);
            }
        else
            {//-- both parameters are specified
            TotalWinsMMC_Cards     = MmcCards;
            TotalWinsMMC_CardSlots = MmcSlots;
            }
    
        }//if(!MmcSlots && !MmcCards)

    TotalWinsMMC_Cards      = Min(TotalWinsMMC_Cards, KMax_TotalWinsCards);
    TotalWinsMMC_CardSlots  = Min(TotalWinsMMC_CardSlots, KMax_TotalWinsCardSlots);

    
    if((iCardArray = new TMMCardArray(this)) == NULL)
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
	// For the WINS implementation, fragmentation does not matter because
	// DMA is not used.  The memory must still be allocated here so MEDMMC is
	// able to use it.
	//
	// The constant calculations could be folded, but this illustrates how the
	// values are derived.
	//

	// MMC only - values from Hitachi 16Mb card, datasheet HB288016MM1

	// minor buffer must contain enough space for MBR or block
	const TUint mmcBlkSzLog2 = 9;				// READ_BLK_LEN and WRITE_BLK_LEN
	const TUint mmcBlkSz = 1 << mmcBlkSzLog2;
	const TInt mmcMinorBufLen = Max(KDiskSectorSize, mmcBlkSz);

	// There are 2 slots each with up to 2 media drivers; we allocate 8 blocks for each driver.
	// It is the media drivers' responsibility to devide up the buffer space according 
	// to which slot number is allocated to it (DMmcMediaDriverFlash::iCardNumber)
	const TInt KMinMMCBlocksInBuffer = 16 * MMC0_NUMMEDIA;
	const TInt mmcCchBufLen = KMinMMCBlocksInBuffer << mmcBlkSzLog2;

	const TInt mmcTotalBufLen = mmcMinorBufLen + mmcCchBufLen;

	const TInt totalBufLen = mmcTotalBufLen;

	iMDBuf = reinterpret_cast<TUint8*>(Kern::Alloc(totalBufLen));
	iMDBufLen = totalBufLen;

	// setup card size parameters from epoc.ini
	if (TotalMDiskSize==0)
		{
		// Static member variable TotalMDiskSize initialised to zero by default. Set 
		// up static member variables TotalMDiskSize, CSIZE and CSIZE_MULT once and 
		// once only. Use INI file setting if available. Else set to default, IMb.
		TUint cardSize = Property::GetInt("MultiMediaCardSize");
		if (cardSize)
			{
			// set values to match epoc.ini settings
			SetupDiskParms(cardSize);
			}		
		else
			{
			// set default values for 1 MB drive
			TotalMDiskSize=0x100000;
			CSIZE=127;
			CSIZE_MULT=2;
			}
		}

	// Initialise each virtual card that will be used on this stack.
	TInt i;
	for (i=0 ; i<TotalWinsMMC_Cards; i++)
		{
		if ((r=SetupSimulatedCard(i))!=KErrNone)
			return(r);
		}

	// initialize pointers to currently present cards

	// Slot zero can toggle between no card; card 0 and card 1.  The current state is
	// determined by *Kern::CurrentPBusDevicePtr() and toggled by pressing F4 when F5
	// (door open) is held down.  Because this function is only executed at startup,
	// assume start with card zero.
	iCardInfo[0] = iCardPool[0];
	for (i = 1; i < TotalWinsMMC_CardSlots; ++i)
		{
		iCardInfo[i]=iCardPool[i+1];
		}

	return(KErrNone);
	}


TInt DWinsMMCStack::CreateBinFileForCard(TInt aCardNum,HANDLE* aHandle,TBool aCreateNew)
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
	fn8.Append(_L8("A.BIN"));
	fn8.Append('\0');
	*aHandle = CreateFileA(
		(LPCSTR) fn8.Ptr(),							// LPCSTR lpFileName,
		GENERIC_READ | GENERIC_WRITE,				// DWORD dwDesiredAccess
		FILE_SHARE_READ | FILE_SHARE_WRITE,			// DWORD dwShareMode
		NULL,										// LPSECURITY_ATTRIBUTES lpSecurityAttributes
		aCreateNew ? CREATE_ALWAYS : OPEN_ALWAYS,	// DWORD dwCreationDisposition
		FILE_FLAG_RANDOM_ACCESS,					// DWORD dwFlagsAndAttributes
			NULL);									// HANDLE hTemplateFile
	
	TInt fileSize = 0;
	if (*aHandle!=INVALID_HANDLE_VALUE)
	    {
        fileSize = GetFileSize(*aHandle,NULL);
        // Check whether MMC card force mount property is set.
        // Force mount MMC card image regardless of whether the size of the image file is
        // as specified in the epoc.ini.
        // Specify "MMCCardForceMount 1" in the epoc.ini to enable force mount.
        TBool forceMount = Property::GetBool("MMCCardForceMount");
        if (forceMount && fileSize != INVALID_FILE_SIZE)
            {
            TInt sizeInKB = fileSize >> 10;
            SetupDiskParms(sizeInKB);
            TotalMDiskSize = fileSize;
            }
	    }

	if (fileSize!=TotalMDiskSize)
		//
		// The Drive file already exists and size of emulated drive as configured in 
		// epoc.ini has been changed. Musn't corrupt the emulated drive so delete the 
		// drive file and start from scratch. The emulated drive contents will be 
		// erased.
		//
		{
		CloseHandle(*aHandle);
		DeleteFileA(
			(LPCSTR) fn8.Ptr());						// LPCSTR lpFileName,			
		*aHandle = CreateFileA(
			(LPCSTR) fn8.Ptr(),							// LPCSTR lpFileName,
			GENERIC_READ | GENERIC_WRITE,				// DWORD dwDesiredAccess
			FILE_SHARE_READ | FILE_SHARE_WRITE,			// DWORD dwShareMode
			NULL,										// LPSECURITY_ATTRIBUTES lpSecurityAttributes
			aCreateNew ? CREATE_ALWAYS : OPEN_ALWAYS,	// DWORD dwCreationDisposition
			FILE_FLAG_RANDOM_ACCESS,					// DWORD dwFlagsAndAttributes
				NULL);									// HANDLE hTemplateFile
		}

	if (*aHandle==INVALID_HANDLE_VALUE)
		return(MapLastErrorEpoc());

	if (SetFilePointer(*aHandle,TotalMDiskSize,NULL,FILE_BEGIN)==0xffffffffu
		||	! SetEndOfFile(*aHandle) )
		{
		CloseHandle(*aHandle);
	    return(MapLastErrorEpoc());
		}

	return KErrNone;
	}


TInt DWinsMMCStack::SetupSimulatedCard(TInt aCardNum)
//
// allocate individual card with Win32 file.  Only called at bootup, so no cleanup if fails.
//
	{
	TWinsCardInfo* cip = new TWinsCardInfo;
	if (cip == 0)
		return(KErrNoMemory);

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

	cip->iPWD=new TMediaPassword;
	if (!cip->iPWD)
		{
		delete cip;
		return(KErrNoMemory);
		}

	cip->iState=ECardStateIdle;

	HANDLE h=NULL;
	TInt err;
	if ( (err=CreateBinFileForCard(aCardNum,&h))!=KErrNone )
		{
		delete cip;
		return(err);
		}
	
	TBool forceMount = Property::GetBool("MMCCardForceMount");
	if (forceMount)
	    {
        // if Force Mount as image file size, CSIZE and CSIZE_MULT will be set 
        // again inside the CreateBinFileForCard() call above.
        cip->iForceMount = ETrue;
        cip->iForceMountCSIZE = CSIZE;
        cip->iForceMountCSIZE_MULT = CSIZE_MULT;
	    }
	else
	    {
        cip->iForceMountCSIZE = EFalse;
	    }
	
	cip->iWinHandle=h;
	iCardPool[aCardNum]=cip;
	return(KErrNone);
	}

void DWinsMMCStack::SetBusConfigDefaults(TMMCBusConfig& aConfig, TUint aClock)
//
// Fills BusConfig structure with default values
//
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

void DWinsMMCStack::InitClockOff()
//
// Switch of the identification mode clock and enable the data transfer mode
// clock instead.
//
	{
	// empty.
	}

void DWinsMMCStack::ASSPReset()
//
// Stop all activities on the host stack
//
	{
	// empty.
	}

void DWinsMMCStack::ASSPDisengage()
//
// Forced release of all ASSP resources
//
	{
	}

void DWinsMMCStack::DoPowerDown()
//
// Power down the bus
//
	{
	// Change the state of all virtual cards present to Idle
	for (TInt i=0 ; i<TotalWinsMMC_CardSlots ; i++)
		iCardInfo[i]->iState=ECardStateIdle;
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

TMMCErr DWinsMMCStack::DoPowerUpSM()
//
//	State Machine functions implemented in ASSP layer
//
	{
	enum states
		{
		EStBegin=0,
		EStEnd
		};

	SMF_BEGIN

	__KTRACE_OPT(KPBUS1, Kern::Printf("DoPowerUpSM: BEGIN"));

	if( MMCSocket()->iVcc->SetState(EPsuOnCurLimit) != KErrNone )
		return( KMMCErrHardware );

	for (TInt i=0 ; i<TotalWinsMMC_CardSlots ; i++)
		{
		// Attempt to retrieve a password for this card from environment settings (as long as this
		// isn't card0 and we are simulating this is not present)
		TInt cardNum=(i==0) ? *Wins::CurrentPBusDevicePtr() : i;
		if (cardNum>=0 && MediaPasswordEnvironmentVar(MMCSocket()->iSocketNumber,cardNum,*(iCardInfo[i]->iPWD))==KErrNone)
			{
			// Card has a password so lock it automatically on power up.		
			iCardInfo[i]->iIsLocked=(iCardInfo[i]->iPWD->Length() > 0);
			}
		else	
			iCardInfo[i]->iIsLocked=EFalse;
			
		iCardInfo[i]->iRCA=0x0001;		// Default RCA - spec 2.2, s4.2.1, 5.4
		}

		ReportPowerUp();

	SMF_END
	}

TMMCErr DWinsMMCStack::InitClockOnSM()
//
// Switch on the identification mode clock
//
	{
	enum states
		{
		EStBegin=0,
		EStEnd
		};
	SMF_BEGIN

	SMF_END
	}

TMMCErr DWinsMMCStack::ModifyCardCapabilitySM()
//
// This function provides a chance to modify the capability of paticular cards.
// Licensee may overide this function to modify certain card's capability as needed.
// A state machine is needed in derived function and function of base class should be
// called in order to act more generic behaviour.
//
    {
		enum states
			{
			EStBegin=0,
            EStDone,
			EStEnd
			};

    SMF_BEGIN

        SMF_INVOKES( DMMCStack::BaseModifyCardCapabilitySMST, EStDone )

    SMF_STATE(EStDone)

    SMF_END
    }

TInt DWinsMMCStack::GetTargetSlotNumber(TBool anRCASupplied,const TRCA& anRCA)
// 
// Attempt to determine the slot number of the target card. If the received command
// contained an RCA then 'anRCASupplied' will be ETrue - in which case, 'anRCA'
// contains the RCA in question.
// 
	{
	TInt selCardIdx = KBroadcastToAllCards;

	// if an RCA was supplied, then work out which card slot it corresponds to
	if (anRCASupplied)
		{
		for (TInt i = 0 ; i < TotalWinsMMC_CardSlots ; ++i)
			{
			if (iCardInfo[i]->iRCA==anRCA)
				{
				selCardIdx=i;
				break;
				}
			}
		}
	// else search for currently selected card
	else
		{
		for (TInt i = 0; i < TotalWinsMMC_CardSlots; ++i)
			{
			if (iCardInfo[i]->iState == ECardStateTran)
				{
				selCardIdx = i;
				break;
				}
			}
		}

	return(selCardIdx);
	}

TMMCErr DWinsMMCStack::IssueMMCCommandSM()
//
// Top level ASSP command executor
//
	{
	enum states
		{
		EStBegin=0,
		EStDoubleBuffer,
		EStCommandDone,
		EStEnd
		};

	TMMCCommandDesc& cmd = Command();

	TRCA tgtRCA=0;
	TBool supRCA=EFalse;
	// Record the RCA if it is embedded in the argument [31:16].
	if (cmd.iCommand == ECmdSetRelativeAddr	||	cmd.iCommand == ECmdSelectCard
	||	cmd.iCommand == ECmdSendCSD			||	cmd.iCommand == ECmdSendCID
	||	cmd.iCommand == ECmdSendStatus		||	cmd.iCommand == ECmdGoInactiveState
	||	cmd.iCommand == ECmdFastIO 			||  cmd.iCommand == ECmdAppCmd )
		{
		supRCA=ETrue;
		tgtRCA=TUint16(cmd.iArgument >> 16);
		}

	// Attempt to determine the target card using supplied RCA
	TInt selCardIdx=GetTargetSlotNumber(supRCA,tgtRCA);
	
	// Simulation of card swapping (i.e. F4/F5) is performed on slot 0. If this is currently
	// set to simulate no card present and the issued command is targetted specifically at the
	// card in slot 0 (i.e. not a broadcast command) then timeout.
	if (selCardIdx==0 && *Wins::CurrentPBusDevicePtr() < 0)
		return(KMMCErrResponseTimeOut);
	 
	// If an RCA was supplied but didn't coincide with the RCAs of any cards present 
	// then timeout (Ignore SET_RCA and APP_CMD as these are sent before RCAs are assigned).
	if (supRCA && selCardIdx==KBroadcastToAllCards &&
		cmd.iCommand != ECmdSetRelativeAddr && cmd.iCommand != ECmdAppCmd)
		return(KMMCErrResponseTimeOut);

	HANDLE winHandle=NULL;

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
	const TMMCCommandEnum origCmd(cmd.iCommand);
	if (	selCardIdx != KBroadcastToAllCards
		&&	iCardInfo[selCardIdx]->iIsLocked			// If locked and not in CCC 0 or 7 then skip
		&&	(	((cmd.iSpec.iCommandClass & (KMMCCmdClassBasic | KMMCCmdClassLockCard)) == 0)
			&&	cmd.iCommand != ECmdSetBlockLen ) )
		{
		lock_unlock_failed = ETrue;				// try to access locked card
		cmd.iCommand = TMMCCommandEnum(-1);		// skip command processing
		}

	SMF_BEGIN

	TBool rto = EFalse;							// response timeout
	switch (cmd.iCommand)
		{
		case ECmdGoIdleState:	// CMD0
			{
			for (TInt i = 0; i < TotalWinsMMC_CardSlots; ++i)
				iCardInfo[i]->iState = ECardStateIdle;
			}
			break;

		case ECmdSendOpCond:	// CMD1
			{
			for (TInt i = 0; i < TotalWinsMMC_CardSlots; ++i)
				iCardInfo[i]->iState = ECardStateReady;

			// bit32 is set to indicate cards are not still powering up
			TUint32 r3 = KMMCWinsCardOCRValue | KMMCOCRBusy;
			TMMC::BigEndian4Bytes(cmd.iResponse, r3);
			}
			break;

		case ECmdAllSendCID:	// CMD2
			{
			TInt idx = FindAnyCardInStack(ECardStateReady);

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
			TInt idx = FindOneCardInStack(ECardStateIdent);
			iCardInfo[idx]->iRCA = tgtRCA;
			iCardInfo[idx]->iState=ECardStateStby;
			selCardIdx = idx;					// set R1 response at end
			}
			break;

		case ECmdSelectCard:		// CMD7
			{
			// switch to broadcast mode so the currently selected and new cards
			// receive the command simultaneously.

			TInt idx = FindAnyCardInStack(ECardStateTran);
			if (idx != -1)
				iCardInfo[idx]->iState = ECardStateStby;
			iCardInfo[selCardIdx]->iState = ECardStateTran;
			}
			break;

		case ECmdSendStatus:
			// R1 response so status return as for any other R1 command.
			break;

		case ECmdReadSingleBlock:
		case ECmdReadMultipleBlock:
			{
			winHandle=iCardInfo[selCardIdx]->iWinHandle;

			if ( cmd.iSpec.iUseStopTransmission && cmd.iBlockLength >= cmd.iTotalLength)
				return( KMMCErrNotSupported );

    		TMMCErr err;
			TInt pos = cmd.iArgument;
    		if (SetFilePointer(winHandle,pos,NULL,FILE_BEGIN)==0xffffffffu)
        		err=MapLastErrorMmc();
    		else
        		{
				iBytesToTransfer = cmd.BufferLength();
				err = ReadWriteData(selCardIdx, cmd.iDataMemoryP, iBytesToTransfer, cmd.iSpec.iDirection);
				if(err == KMMCErrNone)
					{
					Session().RequestMoreData();
					SMF_WAITS(EStDoubleBuffer);
					}
				}
			if (err!=KMMCErrNone)
				return(err);
			}
			break;

		// ------------------------------------------------------------------
		case ECmdWriteBlock:
		case ECmdWriteMultipleBlock:
			{
			HANDLE h = iCardInfo[selCardIdx]->iWinHandle;

    		TMMCErr err;
			TInt pos = cmd.iArgument;
    		if (SetFilePointer(h, pos, NULL, FILE_BEGIN)==0xffffffffu)
        		err = MapLastErrorMmc();
    		else
        		{
				iBytesToTransfer = cmd.BufferLength();
				err = ReadWriteData(selCardIdx, cmd.iDataMemoryP, iBytesToTransfer, cmd.iSpec.iDirection);
				if(err == KMMCErrNone)
					{
					Session().RequestMoreData();
					SMF_WAITS(EStDoubleBuffer);
					}
				}

			if (err!=KMMCErrNone)
				return(err);
			}
			break;

		case ECmdAppCmd:
			rto = ETrue;
			break;

		case ECmdSendCSD:
			{
			iCardInfo[selCardIdx]->GetCSD(cmd.iResponse);
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
#ifdef __CARD0_NOT_LOCKABLE__
			if (*Wins::CurrentPBusDevicePtr() == 0)
				return KMMCErrNotSupported;
#endif
#ifdef __CARD1_NOT_LOCKABLE__
			if (*Wins::CurrentPBusDevicePtr() == 1)
				return KMMCErrNotSupported;
#endif
			const TInt8 cmd_byte(*cmd.iDataMemoryP);
			__ASSERT_DEBUG(										// ensure not CLR_PWD && SET_PWD
				!((cmd_byte & KMMCLockUnlockClrPwd) && (cmd_byte & KMMCLockUnlockSetPwd)),
				DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCCorruptCommand) );
			
			__ASSERT_DEBUG(										// ensure not FORCE_ERASE with CLR_PWD or SET_PWD
				!((cmd_byte & KMMCLockUnlockErase) && (cmd_byte & (KMMCLockUnlockSetPwd | KMMCLockUnlockClrPwd))),
				DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCCorruptCommand) );
			
			__ASSERT_DEBUG(										// not actually lock a card while setting the password
				((cmd_byte & (KMMCLockUnlockLockUnlock | KMMCLockUnlockSetPwd)) != (KMMCLockUnlockLockUnlock | KMMCLockUnlockSetPwd)),
				DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCLockAttempt) );

			if (cmd_byte & KMMCLockUnlockErase)								// Forced Erase of a locked card
				{
				if (iCardInfo[selCardIdx]->iIsLocked)						// Forced erase when locked
					{
					iCardInfo[selCardIdx]->iPWD->Zero();					// Remove the password
					iCardInfo[selCardIdx]->iIsLocked = EFalse;
					nextCMD42Failed = EFalse;
					
					TInt cardNum = (selCardIdx==0) ? *Wins::CurrentPBusDevicePtr() : selCardIdx;
					SetMediaPasswordEnvironmentVar(MMCSocket()->iSocketNumber,cardNum,*(iCardInfo[selCardIdx]->iPWD));

					// Erase then entire contents of the emulated drive
					HANDLE handle = iCardInfo[selCardIdx]->iWinHandle;
					CloseHandle(handle);
					iCardInfo[selCardIdx]->iWinHandle = NULL;
					if(CreateBinFileForCard(selCardIdx, &handle, ETrue) != KErrNone)
						return(MapLastErrorMmc());
					iCardInfo[selCardIdx]->iWinHandle = handle;
					}
				else														// Forced erase when unlocked (illegal)
					{
					nextCMD42Failed = ETrue;
					}
				}
			else
				{
				const TInt8 pwd_len = *(cmd.iDataMemoryP + 1);
				const TPtrC8 pwd(cmd.iDataMemoryP + 2, pwd_len);

				if ((cmd_byte & KMMCLockUnlockClrPwd) != 0)			// CLR_PWD == 1
					{
					__ASSERT_DEBUG(
						pwd_len >= 0 && pwd_len <= KMaxMediaPassword,
						DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCCorruptCommand));

					if (iCardInfo[selCardIdx]->iIsLocked)						// clear when locked
						nextCMD42Failed = ETrue;
					else														// clear when unlocked
						{
						if (iCardInfo[selCardIdx]->iPWD->Compare(pwd) != 0)		// clear when unlocked with wrong password
							{
							nextCMD42Failed = ETrue;
							lock_unlock_failed = ETrue;
							}
						else													// clear when unlocked with right password
							{
							// Clear from password store 
							iCardInfo[selCardIdx]->iPWD->Zero();
							iCardInfo[selCardIdx]->iIsLocked = EFalse;
							nextCMD42Failed = EFalse;
							
							// Clear from environment settings
							TInt cardNum=(selCardIdx==0) ? *Wins::CurrentPBusDevicePtr() : selCardIdx; // Can't be -1 at this stage
							SetMediaPasswordEnvironmentVar(MMCSocket()->iSocketNumber,cardNum,*(iCardInfo[selCardIdx]->iPWD));
							}
						}
					}
				else if ((cmd_byte & KMMCLockUnlockSetPwd) == 0)	// SET_PWD == 0: unlock
					{
					__ASSERT_DEBUG(
						pwd_len >= 0 && pwd_len <= KMaxMediaPassword,
						DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCCorruptCommand) );
					
					if (! iCardInfo[selCardIdx]->iIsLocked)						// unlock when unlocked
						nextCMD42Failed = ETrue;
					else
						{
						if (iCardInfo[selCardIdx]->iPWD->Compare(pwd) != 0)		// unlock when locked with wrong password
							{
							nextCMD42Failed = ETrue;
							lock_unlock_failed = ETrue;
							}
						else													// unlock when locked with right password
							{
							iCardInfo[selCardIdx]->iIsLocked = EFalse;
							nextCMD42Failed = EFalse;
							}
						}
					}
				else  if ((cmd_byte & KMMCLockUnlockSetPwd) == KMMCLockUnlockSetPwd)	// SET_PWD == 1
					{
					__ASSERT_DEBUG(
						cmd_byte & KMMCLockUnlockSetPwd,
						DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCCorruptCommand) );

					// if pwd_len < iCardInfo[selCardIdx]->iPWD->Length() then data block must be invalid.
					// This can be caused by bad user input rather than inaccurate formation.
					if (!(	pwd_len >= iCardInfo[selCardIdx]->iPWD->Length()
						&&	pwd_len <= iCardInfo[selCardIdx]->iPWD->Length() + KMaxMediaPassword ))
						{
						nextCMD42Failed = ETrue;
						}
					else
						{
						TUint16 env_Var[]=L"_EPOC_PWD_LEN";
						TUint16 env_Val[2];
						TInt r=GetEnvironmentVariable(env_Var,&env_Val[0],2);
						r=r;//This code is added to suppress WINS warnings
						__ASSERT_DEBUG(r!=0,Kern::PanicCurrentThread(_L("PBUS-MMC-WINS-GETENV"),0));			
						const TInt old_pwd_len=env_Val[0]-1;
						TPtrC8 old_pwd(cmd.iDataMemoryP + 2, old_pwd_len);
						TPtrC8 new_pwd(cmd.iDataMemoryP + 2 + old_pwd_len, pwd_len - old_pwd_len);
						
						// card must not be locked and supplied current password must be correct
						if (iCardInfo[selCardIdx]->iIsLocked || iCardInfo[selCardIdx]->iPWD->Compare(old_pwd) != 0)
							{
							nextCMD42Failed = ETrue;
							lock_unlock_failed = ETrue;
							}
						else
							{
							// Set in password store
							iCardInfo[selCardIdx]->iPWD->Copy(new_pwd);
							nextCMD42Failed = EFalse;
							
							// Set in environment settings
							TInt cardNum=(selCardIdx==0) ? *Wins::CurrentPBusDevicePtr() : selCardIdx; // Can't be -1 at this stage
							SetMediaPasswordEnvironmentVar(MMCSocket()->iSocketNumber,cardNum,*(iCardInfo[selCardIdx]->iPWD));
							}
						}
					}
				else  if ((cmd_byte & KMMCLockUnlockLockUnlock) == KMMCLockUnlockLockUnlock)
					{
					__ASSERT_DEBUG(
						pwd_len >= 0 && pwd_len <= KMaxMediaPassword,
						DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCCorruptCommand) );
					
					if (iCardInfo[selCardIdx]->iIsLocked)						// lock when locked
						nextCMD42Failed = ETrue;
					else
						{
						if (iCardInfo[selCardIdx]->iPWD->Compare(pwd) != 0)		// lock with wrong password
							{
							nextCMD42Failed = ETrue;
							lock_unlock_failed = ETrue;
							}
						else													// lock with right password
							{
							iCardInfo[selCardIdx]->iIsLocked = ETrue;
							nextCMD42Failed = EFalse;
							}
						}
					}
				else
					{
					__ASSERT_DEBUG(EFalse, DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCLockAttempt) );
					}
				}
			}	// case ECmdLockUnlock
			break;
		
		case ECmdSetBlockCount:		
			{
			// Only supported in version 3.1
			if(Session().iCardP->CSD().SpecVers() != 3)
				{
				return(KMMCErrNotSupported);
				}
			}
			break;
			
		// ------------------------------------------------------------------
		default:
			break;
		}

		if (rto)
			{
			return(KMMCErrResponseTimeOut);
			}

	// drop through to command done...

	SMF_STATE(EStCommandDone)

		cmd.iCommand = origCmd;
		// If this is an R1 or R1b response type command then return card status as a response
		if (	selCardIdx != KBroadcastToAllCards
			&&	(cmd.iSpec.iResponseType==ERespTypeR1 || cmd.iSpec.iResponseType==ERespTypeR1B) )
			{
			TUint32 resp(
					iCardInfo[selCardIdx]->iState
				|	((iCardInfo[selCardIdx]->iIsLocked ? 1 : 0) << 25)
				|	((lock_unlock_failed ? 1 : 0) << 24) );

			if (iCMD42Failed)								// previous CMD42
				{
				resp |= KMMCStatErrLockUnlock;
				nextCMD42Failed = EFalse;
				}
			iCMD42Failed = nextCMD42Failed;
			TMMC::BigEndian4Bytes(&cmd.iResponse[0],resp); // Ignore bits 47-40
			}

		SMF_GOTOS(EStEnd);

	SMF_STATE(EStDoubleBuffer)

		cmd.iBytesDone += iBytesToTransfer;

        if(cmd.iBytesDone < cmd.iTotalLength)
			{
			iBytesToTransfer = cmd.BufferLength();
			TMMCErr err = ReadWriteData(selCardIdx, cmd.iDataMemoryP, iBytesToTransfer, cmd.iSpec.iDirection);
			if(err == KMMCErrNone)
				{
				Session().RequestMoreData();
				SMF_WAITS(EStDoubleBuffer);
				}
			else
				{
				return(err);
				}
			}
		else
			{
			SMF_GOTOS(EStCommandDone);
			}

	SMF_END
	}


TMMCErr DWinsMMCStack::ReadWriteData(TInt aCardIdx, TUint8* aDataP, TUint32 aLength, TMMCCmdDirEnum aDir)
	{
	TMMCErr err = KMMCErrNone;

	HANDLE h = iCardInfo[aCardIdx]->iWinHandle;

	DWORD res;
	TBool success;
	
	if(aDir == EDirWrite)
		{
		success = WriteFile(h, (LPCVOID)aDataP, aLength, &res, NULL);
		}
	else
		{
		success = ReadFile(h, (LPVOID)aDataP, aLength, &res, NULL);
		}

	if (!success)
		{
		err=MapLastErrorMmc();
		}
	else if (res != (DWORD)aLength)
		{
		err=KMMCErrGeneral;
		}

	return(err);
	}


TInt DWinsMMCStack::FindAnyCardInStack(TMMCardStateEnum aState)
//
// first first active card in supplied state.  Return -1 if
// no active card is in supplied state.
//
	{
	for (TInt i = 0; i < TotalWinsMMC_CardSlots; ++i)
		{
		if (iCardInfo[i]->iState == aState)
			return i;
		}
	return -1;
	}

TInt DWinsMMCStack::FindFirstCardInStack(TMMCardStateEnum aState)
//
// find card which is active on bus and in supplied state.
// There can be more than one active card in the the supplied state,
// but there should be at least one.
//
	{
	TInt idx = -1;
	for (TInt i = 0; idx != -1 && i < TotalWinsMMC_CardSlots; ++i)
		{
		if (iCardInfo[i]->iState == aState)
			idx = i;
		}

	__ASSERT_DEBUG(idx != -1, DWinsMMCStack::Panic(DWinsMMCStack::EStkFFCNoneSel));
	return idx;
	}

TInt DWinsMMCStack::FindOneCardInStack(TMMCardStateEnum aState)
//
// find card which is active on bus and in supplied state.
// There should be exactly one active card in the supplied state.
//
	{
	TInt idx = -1;
	for (TInt i = 0; i < TotalWinsMMC_CardSlots; ++i)
		{
		if (iCardInfo[i]->iState == aState)
			{
			__ASSERT_DEBUG(idx == -1, DWinsMMCStack::Panic(DWinsMMCStack::EStkFOCMultiSel));
			idx = i;
			}
		}

	__ASSERT_DEBUG(idx != -1, DWinsMMCStack::Panic(DWinsMMCStack::EStkFOCNoneSel));
	return idx;
	}

void DWinsMMCStack::SetupDiskParms(TUint aDiskSize)
	{	
//
//	setup parms for emulated mmc disk size
//
	// force a minimum of 32 KB total size
	if (aDiskSize<32)
		aDiskSize=32;
	// first setup the CSD parameters
	CSIZE_MULT = 0;
	TUint newCSIZE = aDiskSize>>1;	// with zero multiplier 1 + size parameter = size in KB / 2
	while((newCSIZE>0xfff)&&(CSIZE_MULT<7))	
		// size parameter 12 bits, multiplier 3 bits
		{
		// size parameter too big and multiplier still has room to
		// grow so increase multiplier and reduce size parameter
		CSIZE_MULT++; 
		newCSIZE = aDiskSize>>(1+CSIZE_MULT);
		}
	CSIZE = newCSIZE;
	// as CSIZE = 1 + CSIZE
	CSIZE--;
	// restrict to 12 bits
	if (CSIZE>0xfff)
		CSIZE=0xfff;
	// now setup TotalDiskSize
	TotalMDiskSize = 512 * (1+CSIZE) * (1<<(2+CSIZE_MULT));
	}

// ======== TWinsMMCMediaChange ========

#pragma warning( disable : 4355 )	// this used in initializer list
DWinsMMCMediaChange::DWinsMMCMediaChange(TInt aMediaChangeNum)
//
// Constructor
//
	: DMMCMediaChange(aMediaChangeNum),
	  iMediaChangeEnable(ETrue)
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
// Handle media door closing
//
	{
	iTickLink.Cancel();	// Doesn't matter if wasn't enabled
	Enable();	// Re-enable door interrupts

	// While the door was open the user may have changed the card in slot 0
	if (iStackP && *Wins::CurrentPBusDevicePtr() >= 0)
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

	__ASSERT_DEBUG(iDoorClosedCount>=0,DWinsMMCStack::Panic(DWinsMMCStack::EWinsMMCMediaChangeTickFault));

	if (!(*Wins::MediaDoorOpenPtr()))
		{
		if (iDoorClosedCount > 0)
			{
			if (--iDoorClosedCount == 0)
				{
				iTickLink.Cancel();		// cancel door closed timer
				DoorClosedService();
				}
			}
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
	TUint size;
    TUint sizeMult;
    
    if (iForceMount)
        {
        size = iForceMountCSIZE;
        sizeMult = iForceMountCSIZE_MULT;
        }
    else
        {
        size = DWinsMMCStack::CSIZE;
        sizeMult = DWinsMMCStack::CSIZE_MULT;
        }
		
	// Bits 127-96
	TUint32 csd=(KCsdStructure<<30); 	/* CSD_STRUCTURE */
	csd|=		(KCsdSpecVers<<26); 	/* SPEC_VERS */
	csd|=		(0x0E<<16);				/* TAAC: 1mS */  
	csd|=		(0x0A<<8);				/* NSAC: 1000 */  
	csd|=		(0x59);					/* TRAN_SPEED: 5.0Mbit/s */  
	TMMC::BigEndian4Bytes(&aResp[0],csd);

	// Bits 95-64
	TUint32 lockBit = KMMCCmdClassLockCard;
#ifdef __CARD0_NOT_LOCKABLE__
	if (*Wins::CurrentPBusDevicePtr() == 0)
		lockBit = 0;
#endif
#ifdef __CARD1_NOT_LOCKABLE__
	if (*Wins::CurrentPBusDevicePtr() == 1)
		lockBit = 0;
#endif
	const TUint32 ccc = 
			KMMCCmdClassBasic | KMMCCmdClassBlockRead
		|	KMMCCmdClassBlockWrite | lockBit;
	csd=		(ccc<<20); 	/* CCC: classes 0, 2, 4, and 7 */
	csd|=		(0x9<<16); 	/* READ_BL_LEN: 512 bytes */
	csd|=		(0x0<<15);	/* READ_BL_PARTIAL: No */  
	csd|=		(0x0<<14);	/* WRITE_BLK_MISALIGN: No */  
	csd|=		(0x0<<13);	/* READ_BLK_MISALIGN: No */  
	csd|=		(0x0<<12);	/* DSR_IMP: No DSR */ 
	csd|=		((size>>10&3)<<8);			/* C_SIZE: MMCSz Kb */
	csd|=		((size>>2) & 0xFF);			/* C_SIZE: MMCSz Kb */
	TMMC::BigEndian4Bytes(&aResp[4],csd); 
	// Bits 63-32
	csd=		((size&3)<<30); 			/* C_SIZE: MMCSz Kb */
	csd|=		(0x1<<27); 	/* VDD_R_CURR_MIN: 1mA */
	csd|=		(0x1<<24);	/* VDD_R_CURR_MAX: 5mA */  
	csd|=		(0x2<<21); 	/* VDD_W_CURR_MIN: 5mA */
	csd|=		(0x3<<18);	/* VDD_W_CURR_MAX: 25mA */  
	csd|=		((sizeMult&0x07)<<15);		/* C_SIZE_MULT: 0 */  
	csd|=		(0x0<<10);	/* SECTOR_SIZE: 1 write block */  
	csd|=		(0x0<<5);	/* ERASE_GRP_SIZE: 1 secotr */  
	csd|=		(0x0);		/* WP_GRP_SIZE: 1 erase group */  
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

// ======== TWinsMMCPsu ========


DWinsMMCPsu::DWinsMMCPsu(TInt aVccNum, TInt aMcId)
//
// Constructor.
//
	: DMMCPsu(aVccNum, aMcId)
	{}

TInt DWinsMMCPsu::DoCreate()
//
// Initialise the PSU
//
    {
	// Nothing to do
	return KErrNone;
    }

void DWinsMMCPsu::DoSetState(TPBusPsuState aState)
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

TInt DWinsMMCPsu::VoltageInMilliVolts()
//
// Return the level of the PSU (in mV) or -ve if error.
//
    {

    return(0);
    }

void DWinsMMCPsu::DoCheckVoltage()
//
// Check the voltage level of the PSU is as expected. Returns either KErrNone, KErrGeneral 
// to indicate the pass/fail state or KErrNotReady if the voltage check isn't complete.
//
    {
	ReceiveVoltageCheckResult(KErrNone);
    }

void DWinsMMCPsu::PsuInfo(TPBusPsuInfo &anInfo)
//
// Return machine info relating to the MMC PSU supply
//
    {

	anInfo.iVoltageSupported=0x00040000; // 3.0V (OCR reg. format).
	anInfo.iMaxCurrentInMicroAmps=0;
	anInfo.iVoltCheckInterval=0;
	anInfo.iVoltCheckMethod=EPsuChkComparator;
	anInfo.iNotLockedTimeOut=0;
	anInfo.iInactivityTimeOut=5;
    }

