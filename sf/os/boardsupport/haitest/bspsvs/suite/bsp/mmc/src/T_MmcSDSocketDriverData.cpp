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

#include "T_MmcSDSocketDriverData.h"
#include "MmcTestUtils.h"


//MMCSD Commands index
/*@{*/

_LIT(KCmdAdjustPartialRead, 			"AdjustPartialRead");
_LIT(KCmdCardIsPresent, 				"CardIsPresent");	
_LIT(KCmdGetBufferInfo, 				"GetBufferInfo");
_LIT(KCmdInit, 							"Init");
_LIT(KCmdInitiatePowerUpSequence, 		"InitiatePowerUpSequence");
_LIT(KCmdMachineInfo, 					"MachineInfo");
_LIT(KCmdPrepareStore, 					"PrepareStore");
_LIT(KCmdReset1, 						"Reset1");
_LIT(KCmdReset2, 						"Reset2");
_LIT(KCmdResetInactivity, 				"ResetInactivity");
_LIT(KCmdStack, 						"Stack");
_LIT(KCmdiState_Get, 					"iState_Get");
_LIT(KCmdiType_Get, 					"iType_Get");
_LIT(KCmdiDoorOpened_Get, 				"iDoorOpened_Get");
_LIT(KCmdiStandby_Get, 					"iStandby_Get");
_LIT(KCmdPowerUp, 						"PowerUp");

// ini file tags
_LIT(KCardPresent,						"cardpresent");
_LIT(KExpectedBufLen, 					"expectedbuflen");		
_LIT(KOldPassword, 						"oldpassword");		
_LIT(KNewPassword, 						"newpassword");		
_LIT(KPrepStoreFunc, 					"prepstorefn");
_LIT(KBusNumber, 						"busnum");
_LIT(KPrdStart, 						"prdstart");
_LIT(KPrdEnd, 							"prdend");
_LIT(KPrdExpectedStart, 				"expectedprdstart");
_LIT(KPrdExpectedEnd, 					"expectedprdend");
_LIT(KExpectedTotalSockets, 			"expectedtotalsockets");
_LIT(KExpectedTotalMediaChanges, 		"expectedtotalmediachanges");
_LIT(KExpectedTotalPrimarySupplies, 	"expectedtotalprimarysupplies");
_LIT(KExpectedSPIMode, 					"expectedspimode");
_LIT(KExpectedBaseBusNumber, 			"expectedbasebusnumber");
_LIT(KHasFlagSupportsSPIMode, 			"hasflagsupportsspimode");
_LIT(KHasFlagSupportsDoubleBuffering, 	"hasflagsupportsdoublebuffering");
_LIT(KHasFlagSupportsR7, 				"hasflagsupportsr7");
_LIT(KHasFlagDma8BitAddressing, 		"hasflagdma8bitaddressing");
_LIT(KHasFlagDma16BitAddressing, 		"hasflagdma16bitaddressing");
_LIT(KHasFlagDma32BitAddressing, 		"hasflagdma32bitaddressing");
_LIT(KHasFlagDma64BitAddressing, 		"hasflagdma64bitaddressing");
_LIT(KHasFlagSupportsDMA, 				"hasflagsupportsdma");
_LIT(KHasFlagMaxTransferLength_256K, 	"hasflagmaxtransferlength256k");
_LIT(KHasFlagMaxTransferLength_512K, 	"hasflagmaxtransferlength512k");
_LIT(KHasFlagMaxTransferLength_1M, 		"hasflagmaxtransferlength1m");
_LIT(KHasFlagMaxTransferLength_2M, 		"hasflagmaxtransferlength2m");
_LIT(KHasFlagMaxTransferLength_4M, 		"hasflagmaxtransferlength4m");
_LIT(KHasFlagMaxTransferLength_8M, 		"hasflagmaxtransferlength8m");
_LIT(KHasFlagMaxTransferLength_16M, 	"hasflagmaxtransferlength16m");
_LIT(KIsNull, 							"isnull");
_LIT(KState, 							"state");
_LIT(KType, 							"type");
_LIT(KDoorOpened, 						"dooropened");
_LIT(KStandby, 							"standby");

// DLocalDrive::TRequestId enum strings
_LIT(KCaps,								"ECaps");
_LIT(KRead,								"ERead");
_LIT(KWrite,							"EWrite");
_LIT(KFormat,							"EFormat");
_LIT(KEnlarge,							"EEnlarge");
_LIT(KReduce,							"EReduce");
_LIT(KForceMediaChange,					"EForceMediaChange");
_LIT(KPasswordLock,						"EPasswordLock");
_LIT(KPasswordUnlock,					"EPasswordUnlock");
_LIT(KPasswordClear,					"EPasswordClear");
_LIT(KReadPasswordStore,				"EReadPasswordStore");
_LIT(KWritePasswordStore,				"EWritePasswordStore");
_LIT(KPasswordStoreLengthInBytes,		"EPasswordStoreLengthInBytes");
_LIT(KControlIO,						"EControlIO");
_LIT(KPasswordErase,					"EPasswordErase");
_LIT(KDeleteNotify,						"EDeleteNotify");
_LIT(KGetLastErrorInfo,					"EGetLastErrorInfo");
_LIT(KInvalidRequestId,					"EInvalidRequestId");	

/**	Enum as a descriptor  				Enum integar value													
 *	In this case these enums represent DLocalDrive::TRequestId																					
 *                                                                             
 * 																											      							
 */																														
const CDataWrapperBase::TEnumEntryTable	CT_MMCSDSocketDriverData::iEnumTRequestIdTable [] =
	{
//	Enum as a descriptor				Enum
	KCaps,								0,
	KRead,								1,
	KWrite,								2,
	KFormat,							3,
	KEnlarge,							4,
	KReduce,							5,
	KForceMediaChange,					6,
	KPasswordLock,						7,
	KPasswordUnlock,					8,
	KPasswordClear,						9,
	KReadPasswordStore,					10,
	KWritePasswordStore,				11,
	KPasswordStoreLengthInBytes,		12,
	KControlIO,							13,
	KPasswordErase,						14,
	KDeleteNotify,						15,
	KGetLastErrorInfo,					16,
	KInvalidRequestId,					-1
	};

// TPBusState enum strings
_LIT(KPBusCardAbsent,					"EPBusCardAbsent");
_LIT(KPBusOff,							"EPBusOff");
_LIT(KPBusPoweringUp,					"EPBusPoweringUp");
_LIT(KPBusOn,							"EPBusOn");
_LIT(KPBusPsuFault,						"EPBusPsuFault");
_LIT(KPBusPowerUpPending,				"EPBusPowerUpPending");
_LIT(KInvalidState,						"EInvalidState");	

/**	Enum as a descriptor  				Enum integar value													
 *	In this case these enums represent TPBusState
 *                                                                             
 * 																											      							
 */																														
const CDataWrapperBase::TEnumEntryTable	CT_MMCSDSocketDriverData::iEnumTPBusStateTable [] =
	{
//	Enum as a descriptor				Enum
	KPBusCardAbsent,					EBusCardAbsent,
	KPBusOff,							EBusOff,
	KPBusPoweringUp,					EBusPoweringUp,
	KPBusOn,							EBusOn,
	KPBusPsuFault,						EBusPsuFault,
	KPBusPowerUpPending,				EBusPowerUpPending,
	KInvalidState,						-1	
	};

// TPBusType enum strings
_LIT(KPBusTypeNone,						"EPBusTypeNone");
_LIT(KPBusTypePcCard,					"EPBusTypePcCard");
_LIT(KPBusTypeMultiMedia,				"EPBusTypeMultiMedia");
_LIT(KPBusTypeUSB,						"EPBusTypeUSB");
_LIT(KInvalidType,						"EInvalidType");	

/**	Enum as a descriptor  				Enum integar value													
 *	In this case these enums represent TPBusType
 *                                                                             
 * 																											      							
 */																														
const CDataWrapperBase::TEnumEntryTable	CT_MMCSDSocketDriverData::iEnumTPBusTypeTable [] =
	{
//	Enum as a descriptor				Enum
	KPBusTypeNone,						EBusTypeNone,
	KPBusTypePcCard,					EBusTypePcCard,
	KPBusTypeMultiMedia,				EBusTypeMultiMedia,
	KPBusTypeUSB,						EBusTypeUSB,
	KInvalidType,						-1
	};

/*@}*/


/**
 * Create a new Socket Driver Data wrapper
 *
 * @return					A Socket Driver Data wrapper
 *
 * @leave					System wide error
 */
CT_MMCSDSocketDriverData* CT_MMCSDSocketDriverData::NewL()
	{
	CT_MMCSDSocketDriverData*	ret=new (ELeave) CT_MMCSDSocketDriverData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Construction
 *
 * @return					N/A
 */
CT_MMCSDSocketDriverData::CT_MMCSDSocketDriverData()
:	CT_MmcSDDriverData()
	{
	}

/**
 * Second phase construction
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDSocketDriverData::ConstructL()
	{
	CT_MmcSDDriverData::ConstructL();
	}

/**
 * Public destructor
 *
 * @return					N/A
 */
CT_MMCSDSocketDriverData::~CT_MMCSDSocketDriverData()
	{
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return					pointer to the object that the data wraps
 */
TAny* CT_MMCSDSocketDriverData::GetObject()
	{
	// Can't use DMMCSocket type on user side, so get pointer from Controller
	TAny* socketPtr = NULL;
	iMmcSDController->Socket(&socketPtr);
	return socketPtr;
	}

/**
 * Process a command read from the script file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
TBool CT_MMCSDSocketDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if( aCommand == KCmdAdjustPartialRead )
		{
		DoCmdAdjustPartialRead(aSection);
		}
	else if( aCommand==KCmdCardIsPresent )
		{
		DoCmdCardIsPresent(aSection);
		}	
	else if( aCommand == KCmdGetBufferInfo )
		{
		DoCmdGetBufferInfo(aSection);
		}
	else if( aCommand == KCmdInit )
		{
		DoCmdInit();
		}
	else if( aCommand == KCmdInitiatePowerUpSequence )
		{
		DoCmdInitiatePowerUpSequence();
		}
	else if( aCommand==KCmdMachineInfo )
		{
		DoCmdMachineInfo(aSection);
		}	
	else if( aCommand == KCmdPrepareStore )
		{
		DoCmdPrepareStore(aSection);
		}
	else if( aCommand == KCmdReset1 )
		{
		DoCmdReset1();
		}
	else if( aCommand == KCmdReset2 )
		{
		DoCmdReset2();
		}
	else if( aCommand == KCmdResetInactivity )
		{
		DoCmdResetInactivity(aSection);
		}
	else if( aCommand == KCmdStack )
		{
		DoCmdStack(aSection);
		}
	else if (aCommand == KCmdiState_Get)
		{
		DoCmdiState_Get(aSection);
		}
	else if (aCommand == KCmdiType_Get)
		{
		DoCmdiType_Get(aSection);
		}
	else if (aCommand == KCmdiDoorOpened_Get)
		{
		DoCmdiDoorOpened_Get(aSection);
		}
	else if (aCommand == KCmdiStandby_Get)
		{
		DoCmdiStandby_Get(aSection);
		}
	else if (aCommand == KCmdPowerUp)
		{
		DoCmdPowerUpL(aSection, aAsyncErrorIndex);
		}
	else
		{
		ret=CT_MmcSDDriverData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}
	return ret;
	}


/**
 * Process command to call DMMCSocket::AdjustPartialRead
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdAdjustPartialRead(const TDesC& aSection)
	{	
	TPartialReadData prd;
	
	if( !GetUintFromConfig(aSection, KPrdStart(), prd.iStart) )
		{
		ERR_PRINTF1(_L("Physical Start value not given in ini file"));
		SetBlockResult(EFail);		
		}
	else if( !GetUintFromConfig(aSection, KPrdEnd(), prd.iEnd) )
		{
		ERR_PRINTF1(_L("Physical End value not given in ini file"));
		SetBlockResult(EFail);		
		}
	else
		{
		INFO_PRINTF3(_L("Calling MMCSocket::AdjustPartialRead(start=%u, end=%u)"), prd.iStart, prd.iEnd);
		TInt err = iMmcSDController->SocketAdjustPartialRead(prd);
		
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("MMCSocket::AdjustPartialRead() Error %d"), err);
			SetError(err);
			}
		else
			{
			INFO_PRINTF3(_L("MMCSocket::AdjustPartialRead() = %u, %u"), prd.iPhysStart, prd.iPhysEnd);
	
			TUint expectedStart;
			if( GetUintFromConfig(aSection, KPrdExpectedStart(), expectedStart) )
				{
				if( prd.iPhysStart != expectedStart )
					{
					ERR_PRINTF3(_L("phys start(%u) != expectedStart(%u)"), prd.iPhysStart, expectedStart);
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF1(_L("Expected Start value not given in ini file"));
				SetBlockResult(EFail);		
				}
			
			TUint expectedEnd;
			if( GetUintFromConfig(aSection, KPrdExpectedEnd(), expectedEnd) )
				{
				if( prd.iPhysEnd != expectedEnd )
					{
					ERR_PRINTF3(_L("phys end(%u) != expected end(%u)"), prd.iPhysEnd, expectedEnd);
					SetBlockResult(EFail);			 
					}				
				}
			else
				{
				ERR_PRINTF1(_L("Expected End value not given in ini file"));
				SetBlockResult(EFail);		
				}
			}			
		}
	}

/**
 * Process command to call DMMCSocket::CardIsPresent
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdCardIsPresent(const TDesC& aSection)
	{
	TBool cardPresent = EFalse; 	
	INFO_PRINTF1(_L("Calling MMCSocket::CardIsPresent()"));
	TInt err = iMmcSDController->SocketCardIsPresent(cardPresent);
		
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::CardIsPresent() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MMCSocket::CardIsPresent() = %d"), cardPresent);
		TBool	expectedCardPresent;
		if( GetBoolFromConfig(aSection, KCardPresent(), expectedCardPresent) )
			{
			if( cardPresent != expectedCardPresent )
				{
				ERR_PRINTF3(_L("cardPresent(%d) != expectedCardPresent(%d)"), cardPresent, expectedCardPresent);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected CardPresent value not given in ini file"));
			SetBlockResult(EFail);		
			}
		}
	}

/**
 * Process command to call DMMCSocket::GetBufferInfo
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdGetBufferInfo(const TDesC& aSection)
	{
	TBufferInfo bufferInfo;
	INFO_PRINTF1(_L("Calling MMCSocket::GetBufferInfo()"));
	TInt err = iMmcSDController->SocketGetBufferInfo(bufferInfo);	
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::GetBufferInfo() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF3(_L("GetBufferInfo() iBuf(%x) iBufLen(%d)"),
				bufferInfo.iBuf, bufferInfo.iBufLen);
	
		// check the optional expected length
		TInt expectedBufLen;		
		if( GetIntFromConfig(aSection, KExpectedBufLen(), expectedBufLen) )
			{
			if( bufferInfo.iBufLen != expectedBufLen )	
				{
				ERR_PRINTF3(_L("bufferInfo.iBufLen (%d) != expectedBufLen(%d)"), bufferInfo.iBufLen , expectedBufLen);
				SetBlockResult(EFail);
				}		
			}
		}
	}

/**
 * Process command to call DMMCSocket::Init
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdInit()
	{
	INFO_PRINTF1(_L("Calling MMCSocket::Init()"));
	TInt err = iMmcSDController->SocketInit();	

	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCSocket::Init() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Process command to call DMMCSocket::InitiatePowerUpSequence
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdInitiatePowerUpSequence()
	{
	INFO_PRINTF1(_L("Calling MMCSocket::InitiatePowerUpSequence()"));
	TInt err = iMmcSDController->SocketInitiatePowerUpSequence();	
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::InitiatePowerUpSequence() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Process command to call DMMCSocket::MachineInfo
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdMachineInfo(const TDesC& aSection)
	{
	TMachineInfo machineInfo;
	INFO_PRINTF1(_L("Calling MMCSocket::MachineInfo()"));
	TInt err = iMmcSDController->SocketMachineInfo(machineInfo);	
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::MachineInfo() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MMCSocket::MachineInfo(): iTotalSockets=%d"), machineInfo.iTotalSockets);
		INFO_PRINTF2(_L("MMCSocket::MachineInfo(): iTotalMediaChanges=%d"), machineInfo.iTotalMediaChanges);
		INFO_PRINTF2(_L("MMCSocket::MachineInfo(): iTotalPrimarySupplies=%d"), machineInfo.iTotalPrimarySupplies);
		INFO_PRINTF2(_L("MMCSocket::MachineInfo(): iSPIMode=%d"), machineInfo.iSPIMode);
		INFO_PRINTF2(_L("MMCSocket::MachineInfo(): iBaseBusNumber=%d"), machineInfo.iBaseBusNumber);

  		if( machineInfo.iSupportsSPIMode )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag ESupportsSPIMode"));
  			}
  		if( machineInfo.iSupportsDoubleBuffering )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag ESupportsDoubleBuffering"));
  			}
  		if( machineInfo.iSupportsR7 )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag ESupportsR7"));
  			}
  		if( machineInfo.iDma8BitAddressing )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EDma8BitAddressing"));
  			}
  		if( machineInfo.iDma16BitAddressing )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EDma16BitAddressing"));
  			}
  		if( machineInfo.iDma32BitAddressing )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EDma32BitAddressing"));
  			}
  		if( machineInfo.iDma64BitAddressing )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EDma64BitAddressing"));
  			}
  		if( machineInfo.iSupportsDMA )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag ESupportsDMA"));
  			}
  		if( machineInfo.iMaxTransferLength_256K )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EMaxTransferLength_256K"));
  			}
  		if( machineInfo.iMaxTransferLength_512K )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EMaxTransferLength_512K"));
  			}
  		if( machineInfo.iMaxTransferLength_1M )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EMaxTransferLength_1M"));
  			}
  		if( machineInfo.iMaxTransferLength_2M )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EMaxTransferLength_2M"));
  			}
  		if( machineInfo.iMaxTransferLength_4M )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EMaxTransferLength_4M"));
  			}
  		if( machineInfo.iMaxTransferLength_8M )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EMaxTransferLength_8M"));
  			}
  		if( machineInfo.iMaxTransferLength_16M )
  			{
  			INFO_PRINTF1(_L("MMCSocket::MachineInfo(): Has flag EMaxTransferLength_16M"));
  			}
		
		TInt expectedInt;
		TBool expectedBool;
		
		if( GetIntFromConfig(aSection, KExpectedTotalSockets(), expectedInt) )
			{
			if( machineInfo.iTotalSockets != expectedInt )
				{
				ERR_PRINTF3(_L("totalSockets(%d) != expectedTotalSockets(%d)"), machineInfo.iTotalSockets, expectedInt);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected Total Sockets not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetIntFromConfig(aSection, KExpectedTotalMediaChanges(), expectedInt) )
			{
			if( machineInfo.iTotalMediaChanges != expectedInt )
				{
				ERR_PRINTF3(_L("totalMediaChanges(%d) != expectedTotalMediaChanges(%d)"), machineInfo.iTotalMediaChanges, expectedInt);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected Media Changes not given in ini file"));
			SetBlockResult(EFail);			
			}
		
		if( GetIntFromConfig(aSection, KExpectedTotalPrimarySupplies(), expectedInt) )
			{
			if( machineInfo.iTotalPrimarySupplies != expectedInt )
				{
				ERR_PRINTF3(_L("totalPrimarySupplies(%d) != expectedTotalPrimarySupplies(%d)"), machineInfo.iTotalPrimarySupplies, expectedInt);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected Primary Supplies not given in ini file"));
			SetBlockResult(EFail);			
			}
		
		if( GetBoolFromConfig(aSection, KExpectedSPIMode(), expectedBool) )
			{
			if( machineInfo.iSPIMode != expectedBool )
				{
				ERR_PRINTF3(_L("SPIMode(%d) != expectedSPIMode(%d)"), machineInfo.iSPIMode, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected SPI Mode not given in ini file"));
			SetBlockResult(EFail);			
			}
		
		if( GetIntFromConfig(aSection, KExpectedBaseBusNumber(), expectedInt) )
			{
			if( machineInfo.iBaseBusNumber != expectedInt )
				{
				ERR_PRINTF3(_L("baseBusNumber(%d) != expectedBaseBusNumber(%d)"), machineInfo.iBaseBusNumber, expectedInt);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected Base Bus Number not given in ini file"));
			SetBlockResult(EFail);			
			}
		
		if( GetBoolFromConfig(aSection, KHasFlagSupportsSPIMode(), expectedBool) )
			{
			if( machineInfo.iSupportsSPIMode != expectedBool )
				{
				ERR_PRINTF3(_L("supportsSPIMode(%d) != expectedSupportsSPIMode(%d)"), machineInfo.iSupportsSPIMode, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Supports SPI Mode not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagSupportsDoubleBuffering(), expectedBool) )
			{
			if( machineInfo.iSupportsDoubleBuffering != expectedBool )
				{
				ERR_PRINTF3(_L("supportsDoubleBuffering(%d) != expectedSupportsDoubleBuffering(%d)"), machineInfo.iSupportsDoubleBuffering, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Supports Double Buffering not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagSupportsR7(), expectedBool) )
			{
			if( machineInfo.iSupportsR7 != expectedBool )
				{
				ERR_PRINTF3(_L("supportsR7(%d) != expectedSupportsR7(%d)"), machineInfo.iSupportsR7, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Supports R7 not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagDma8BitAddressing(), expectedBool) )
			{
			if( machineInfo.iDma8BitAddressing != expectedBool )
				{
				ERR_PRINTF3(_L("Dma8BitAddressing(%d) != expectedDma8BitAddressing(%d)"), machineInfo.iDma8BitAddressing, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Dma 8Bit Addressing not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagDma16BitAddressing(), expectedBool) )
			{
			if( machineInfo.iDma16BitAddressing != expectedBool )
				{
				ERR_PRINTF3(_L("Dma16BitAddressing(%d) != expectedDma16BitAddressing(%d)"), machineInfo.iDma16BitAddressing, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Dma 16Bit Addressing not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagDma32BitAddressing(), expectedBool) )
			{
			if( machineInfo.iDma32BitAddressing != expectedBool )
				{
				ERR_PRINTF3(_L("Dma32BitAddressing(%d) != expectedDma32BitAddressing(%d)"), machineInfo.iDma32BitAddressing, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Dma 32Bit Addressing not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagDma64BitAddressing(), expectedBool) )
			{
			if( machineInfo.iDma64BitAddressing != expectedBool )
				{
				ERR_PRINTF3(_L("Dma64BitAddressing(%d) != expectedDma64BitAddressing(%d)"), machineInfo.iDma64BitAddressing, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Dma 64Bit Addressing not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagSupportsDMA(), expectedBool) )
			{
			if( machineInfo.iSupportsDMA != expectedBool )
				{
				ERR_PRINTF3(_L("SupportsDMA(%d) != expectedSupportsDMA(%d)"), machineInfo.iSupportsDMA, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Supports DMA not given in ini file"));
			SetBlockResult(EFail);			
			}
		
		if( GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_256K(), expectedBool) )
			{
			if( machineInfo.iMaxTransferLength_256K != expectedBool )
				{
				ERR_PRINTF3(_L("maxTransferLength_256K(%d) != expectedMaxTransferLength_256K(%d)"), machineInfo.iMaxTransferLength_256K, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Max Transfer Length 256K not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_512K(), expectedBool) )
			{
			if( machineInfo.iMaxTransferLength_512K != expectedBool )
				{
				ERR_PRINTF3(_L("maxTransferLength_512K(%d) != expectedMaxTransferLength_512K(%d)"), machineInfo.iMaxTransferLength_512K, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Max Transfer Length 512K not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_1M(), expectedBool) )
			{
			if( machineInfo.iMaxTransferLength_1M != expectedBool )
				{
				ERR_PRINTF3(_L("maxTransferLength_1M(%d) != expectedMaxTransferLength_1M(%d)"), machineInfo.iMaxTransferLength_1M, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Max Transfer Length 1M not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_2M(), expectedBool) )
			{
			if( machineInfo.iMaxTransferLength_2M != expectedBool )
				{
				ERR_PRINTF3(_L("maxTransferLength_2M(%d) != expectedMaxTransferLength_2M(%d)"), machineInfo.iMaxTransferLength_2M, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Max Transfer Length 2M not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_4M(), expectedBool) )
			{
			if( machineInfo.iMaxTransferLength_4M != expectedBool )
				{
				ERR_PRINTF3(_L("maxTransferLength_4M(%d) != expectedMaxTransferLength_4M(%d)"), machineInfo.iMaxTransferLength_4M, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Max Transfer Length 4M not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_8M(), expectedBool) )
			{
			if( machineInfo.iMaxTransferLength_8M != expectedBool )
				{
				ERR_PRINTF3(_L("maxTransferLength_8M(%d) != expectedMaxTransferLength_8M(%d)"), machineInfo.iMaxTransferLength_8M, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Max Transfer Length 8M not given in ini file"));
			SetBlockResult(EFail);			
			}

		if( GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_16M(), expectedBool) )
			{
			if( machineInfo.iMaxTransferLength_16M != expectedBool )
				{
				ERR_PRINTF3(_L("maxTransferLength_16M(%d) != expectedMaxTransferLength_16M(%d)"), machineInfo.iMaxTransferLength_16M, expectedBool);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Has Flag Max Transfer Length 16M not given in ini file"));
			SetBlockResult(EFail);			
			}
		}
	}

/**
 * Process command to call DMMCSocket::PrepareStore
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdPrepareStore(const TDesC& aSection)
	{	
	TPasswordPrepareStoreData pwdData;
	TPtrC oldPassword;
	TPtrC newPassword;

	if( !GetIntFromConfig(aSection, KBusNumber(), pwdData.iBus) )
		{
		ERR_PRINTF1(_L("Bus Number not given in ini file"));
		SetBlockResult(EFail);			
		}
	else if( !GetStringFromConfig(aSection, KOldPassword(), oldPassword) )
		{
		ERR_PRINTF1(_L("Old password value not given in ini file"));
		SetBlockResult(EFail);			
		}
	else if( !GetStringFromConfig(aSection, KNewPassword(), newPassword) )
		{
		ERR_PRINTF1(_L("New password value not given in ini file"));
		SetBlockResult(EFail);			
		}
	else if( !GetEnumFromConfig(aSection, KPrepStoreFunc(), iEnumTRequestIdTable, pwdData.iFunc) )
		{
		ERR_PRINTF1(_L("Function ID not given in ini file"));
		SetBlockResult(EFail);			
		}
	else
		{
		pwdData.iOldPassword.Copy(oldPassword);
		pwdData.iNewPassword.Copy(newPassword);
		
	 	INFO_PRINTF5(_L("Calling MMCSocket::PrepareStore(bus=%d, func=%d, oldPassword=%S, newPassword=%S)"), pwdData.iBus, pwdData.iFunc, &oldPassword, &newPassword);
		TInt err = iMmcSDController->SocketPrepareStore(pwdData);	     		
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("MMCSocket::PrepareStore() Error %d"), err);
			SetError(err);
			}		
		}
	}

/**
 * Process command to call DMMCSocket::Reset1
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdReset1()
	{
	INFO_PRINTF1(_L("Calling MMCSocket::Reset1()"));
	TInt err = iMmcSDController->SocketReset1();	
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::Reset1() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Process command to call DMMCSocket::Reset2
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdReset2()
	{
	INFO_PRINTF1(_L("Calling MMCSocket::Reset2()"));
	TInt err = iMmcSDController->SocketReset2();	
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::Reset2() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Process command to call DMMCSocket::ResetInactivity
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdResetInactivity(const TDesC& aSection)
	{
	TInt	busNumber;
	if( GetIntFromConfig(aSection, KBusNumber(), busNumber) )
		{
		INFO_PRINTF2(_L("Calling MMCSocket::ResetInactivity(bus=%d)"), busNumber);
		TInt err = iMmcSDController->SocketResetInactivity(busNumber);	
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("MMCSocket::ResetInactivity() Error %d"), err);
			SetError(err);
			}		
		}
	else
		{
		ERR_PRINTF1(_L("Bus Number not given in ini file"));
		SetBlockResult(EFail);			
		}
	}

/**
 * Process command to call DMMCSocket::Stack
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MMCSDSocketDriverData::DoCmdStack(const TDesC& aSection)
	{
	TStackPtr stackPtr;
	if( GetIntFromConfig(aSection, KBusNumber(), stackPtr.iBus) )
		{
		INFO_PRINTF2(_L("Calling MMCSocket::Stack(%d)"), stackPtr.iBus);
		TInt err = iMmcSDController->SocketStack(stackPtr);
	
		if( err!=KErrNone )
			{
			ERR_PRINTF2(_L("MMCSocket::Stack() Error %d"), err);
			SetError(err);
			}
		else
			{
			INFO_PRINTF2(_L("MMCSocket::Stack(): ptr=%x"), stackPtr.iStackPtr);
			TBool	expectedIsNull;
			if( GetBoolFromConfig(aSection, KIsNull(), expectedIsNull) )
				{
				if( expectedIsNull )
					{
					if( stackPtr.iStackPtr != NULL )
						{
						ERR_PRINTF1(_L("Stack ptr is not NULL"));
						SetBlockResult(EFail);
						}
					}
				else
					{
					if( stackPtr.iStackPtr == NULL )
						{
						ERR_PRINTF1(_L("Stack ptr is NULL"));
						SetBlockResult(EFail);
						}
					else
						{
						TAny* expectedStackPtr = NULL;	
						err = iMmcSDController->Stack(&expectedStackPtr);
						if( err!=KErrNone )
							{
							ERR_PRINTF2(_L("MMCStack Error %d"), err);
							SetBlockResult(EFail);
							}
						else if( stackPtr.iStackPtr != expectedStackPtr )
							{
							ERR_PRINTF3(_L("stackPtr(%x) != expectedStackPtr(%x)"), stackPtr.iStackPtr, expectedStackPtr);
							SetBlockResult(EFail);
							}
						}
					}				
				}
			else
				{
				ERR_PRINTF1(_L("Expected Is Null not given in ini file"));
				SetBlockResult(EFail);			
				}
			}		
		}
	else
		{
		ERR_PRINTF1(_L("Bus Number not given in ini file"));
		SetBlockResult(EFail);			
		}		
	}

/**
 * Process command that will result in getting DMMCSocket::iState kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDSocketDriverData::DoCmdiState_Get(const TDesC& aSection)
	{
	TBusState state;
	TInt err = iMmcSDController->SocketiState_Get(state);
	
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::iState() Error %d"), err);
		SetError(err);
		}
	else
		{
		switch(state)
			{
			case EBusCardAbsent:
				{
				INFO_PRINTF2(_L("MMCSocket::iState()=EPBusCardAbsent(%d)"), state);
				break;
				}
			case EBusOff:
				{
				INFO_PRINTF2(_L("MMCSocket::iState()=EPBusOff(%d)"), state);
				break;
				}
			case EBusPoweringUp:
				{
				INFO_PRINTF2(_L("MMCSocket::iState()=EPBusPoweringUp(%d)"), state);
				break;
				}
			case EBusOn:
				{
				INFO_PRINTF2(_L("MMCSocket::iState()=EPBusOn(%d)"), state);
				break;
				}
			case EBusPsuFault:
				{
				INFO_PRINTF2(_L("MMCSocket::iState()=EPBusPsuFault(%d)"), state);
				break;
				}
			case EBusPowerUpPending:
				{
				INFO_PRINTF2(_L("MMCSocket::iState()=EPBusPowerUpPending(%d)"), state);
				break;
				}
			default:
				{
				INFO_PRINTF2(_L("MMCSocket::iState() Unknown state=%d"), state);
				break;
				}
			}		
		
		TInt expectedState;
		if( GetEnumFromConfig(aSection, KState(), iEnumTPBusStateTable, expectedState) )
			{
			if( state != expectedState )
				{
				ERR_PRINTF3(_L("State (%d) != Expected state (%d)"), state, expectedState);
				SetBlockResult(EFail);			
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected state not given in ini file"));
			SetBlockResult(EFail);			
			}		
		}
	}

/**
 * Process command that will result in getting DMMCSocket::iStandby kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDSocketDriverData::DoCmdiStandby_Get(const TDesC& aSection)
	{
	TBool standby;
	TInt err = iMmcSDController->SocketiStandby_Get(standby);
	
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::iStandby() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MMCSocket::iStandby()=%d"), standby);
		TBool expectedStandby;
		if( GetBoolFromConfig(aSection, KStandby(), expectedStandby) )
			{
			if( standby != expectedStandby )
				{
				ERR_PRINTF3(_L("Standby (%d) != Expected standby (%d)"), standby, expectedStandby);
				SetBlockResult(EFail);			
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected standby not given in ini file"));
			SetBlockResult(EFail);			
			}		
		}
	}

/**
 * Process command that will result in getting DMMCSocket::iDoorOpened kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDSocketDriverData::DoCmdiDoorOpened_Get(const TDesC& aSection)
	{
	TBool doorOpened;
	TInt err = iMmcSDController->SocketiDoorOpened_Get(doorOpened);
	
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::iDoorOpened() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MMCSocket::iDoorOpened()=%d"), doorOpened);
		TBool expectedDoorOpened;
		if( GetBoolFromConfig(aSection, KDoorOpened(), expectedDoorOpened) )
			{
			if( doorOpened != expectedDoorOpened )
				{
				ERR_PRINTF3(_L("DoorOpened (%d) != Expected doorOpened (%d)"), doorOpened, expectedDoorOpened);
				SetBlockResult(EFail);			
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected doorOpened not given in ini file"));
			SetBlockResult(EFail);			
			}		
		}
	}

/**
 * Process command that will result in getting DMMCSocket::iType kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDSocketDriverData::DoCmdiType_Get(const TDesC& aSection)
	{
	TCardBusType type;
	TInt err = iMmcSDController->SocketiType_Get(type);
	
	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("MMCSocket::iType() Error %d"), err);
		SetError(err);
		}
	else
		{
		switch(type)
			{
			case EBusTypeNone:
				{
				INFO_PRINTF2(_L("MMCSocket::iType()=EPBusTypeNone(%d)"), type);
				break;
				}
			case EBusTypePcCard:
				{
				INFO_PRINTF2(_L("MMCSocket::iType()=EPBusTypePcCard(%d)"), type);
				break;
				}
			case EBusTypeMultiMedia:
				{
				INFO_PRINTF2(_L("MMCSocket::iType()=EPBusTypeMultiMedia(%d)"), type);
				break;
				}
			case EBusTypeUSB:
				{
				INFO_PRINTF2(_L("MMCSocket::iType()=EPBusTypeUSB(%d)"), type);
				break;
				}
			default:
				{
				INFO_PRINTF2(_L("MMCSocket::iType() Unknown type=%d"), type);
				break;
				}
			}		
		
		TInt expectedType;
		if( GetEnumFromConfig(aSection, KType(), iEnumTPBusTypeTable, expectedType) )
			{
			if( type != expectedType )
				{
				ERR_PRINTF3(_L("Type (%d) != Expected type (%d)"), type, expectedType);
				SetBlockResult(EFail);			
				}
			}
		else
			{
			ERR_PRINTF1(_L("Expected type not given in ini file"));
			SetBlockResult(EFail);			
			}		
		}
	}

/**
 * Process command that will result in call to DMMCSocket::PowerUp kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDSocketDriverData::DoCmdPowerUpL(const TDesC& aSection, TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("DoCmdSocketPowerUpL called()"));
	CActiveCallbackWrap* activeCallbackWrap = CreateActiveCallbackL(RMMCSDTestControllerInterface::ESocketPowerUp, GetExpectedAsyncError(aSection));	
	iMmcSDController->SocketPowerUp(activeCallbackWrap->ActiveCallback().iStatus);
	activeCallbackWrap->ActiveCallback().Activate(aAsyncErrorIndex);
	IncOutstanding();
	}
