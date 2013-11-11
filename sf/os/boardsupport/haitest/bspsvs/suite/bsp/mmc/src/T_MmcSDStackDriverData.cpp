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

#include "T_MmcSDStackDriverData.h"
#include "MmcTestUtils.h"

//MMCSD Commands index
/*@{*/

_LIT(KCmdStackAdjustPartialRead, 						"AdjustPartialRead");
_LIT(KCmdStackAllocSession, 							"AllocSession");
_LIT(KCmdStackBufferInfo, 								"BufferInfo");
_LIT(KCmdStackDemandPagingInfo, 						"DemandPagingInfo");
_LIT(KCmdStackEffectiveModes, 							"EffectiveModes");
_LIT(KCmdStackAlterEffectiveModes, 						"AlterEffectiveModes");
_LIT(KCmdStackGetBufferInfo, 							"GetBufferInfo");
_LIT(KCmdStackHasCardsPresent, 							"HasCardsPresent");
_LIT(KCmdStackHasSessionsQueued, 						"HasSessionsQueued");
_LIT(KCmdStackInit, 									"Init");
_LIT(KCmdStackInitStackInProgress, 						"InitStackInProgress");
_LIT(KCmdStackMMCSocket, 								"MMCSocket");
_LIT(KCmdStackMachineInfo, 								"MachineInfo");
_LIT(KCmdStackMaxCardsInStack, 							"MaxCardsInStack");
_LIT(KCmdStackReadPasswordStore, 						"ReadPasswordStore");
_LIT(KCmdStackWritePasswordStore, 						"WritePasswordStore");
_LIT(KCmdStackProgramPeriodInMs, 						"ProgramPeriodMs");
_LIT(KCmdStackStop, 									"Stop");
_LIT(KCmdStackReportPowerUp, 							"ReportPowerUp");
_LIT(KCmdStackReportPowerDown, 							"ReportPowerDown");
_LIT(KCmdStackCardP, 									"CardP");

// ini file tags
_LIT(KPrdStart, 										"prdstart");
_LIT(KPrdEnd, 											"prdend");
_LIT(KPrdExpectedStart, 								"expectedprdstart");
_LIT(KPrdExpectedEnd, 									"expectedprdend");
_LIT(KExpectedBufLen, 									"expectedBuflen");
_LIT(KExpectedMinorBufLen, 								"expectedMinorBuflen");
_LIT(KExpectedMaxCardsInStack, 							"maxcards");
_LIT(KProgramPeriodMs, 									"programperiodms");
_LIT(KInProgress, 										"inprogress");
_LIT(KSessionsQueued, 									"sessionsqueued");
_LIT(KCardsPresent, 									"cardspresent");
_LIT(KExpectedTotalSockets, 							"expectedtotalsockets");
_LIT(KExpectedTotalMediaChanges, 						"expectedtotalmediachanges");
_LIT(KExpectedTotalPrimarySupplies, 					"expectedtotalprimarysupplies");
_LIT(KExpectedSPIMode, 									"expectedspimode");
_LIT(KExpectedBaseBusNumber, 							"expectedbasebusnumber");
_LIT(KHasFlagSupportsSPIMode, 							"hasflagsupportsspimode");
_LIT(KHasFlagSupportsDoubleBuffering, 					"hasflagsupportsdoublebuffering");
_LIT(KHasFlagSupportsR7, 								"hasflagsupportsr7");
_LIT(KHasFlagDma8BitAddressing, 						"hasflagdma8bitaddressing");
_LIT(KHasFlagDma16BitAddressing, 						"hasflagdma16bitaddressing");
_LIT(KHasFlagDma32BitAddressing, 						"hasflagdma32bitaddressing");
_LIT(KHasFlagDma64BitAddressing, 						"hasflagdma64bitaddressing");
_LIT(KHasFlagSupportsDMA, 								"hasflagsupportsdma");
_LIT(KHasFlagMaxTransferLength_256K, 					"hasflagmaxtransferlength256k");
_LIT(KHasFlagMaxTransferLength_512K, 					"hasflagmaxtransferlength512k");
_LIT(KHasFlagMaxTransferLength_1M, 						"hasflagmaxtransferlength1m");
_LIT(KHasFlagMaxTransferLength_2M, 						"hasflagmaxtransferlength2m");
_LIT(KHasFlagMaxTransferLength_4M, 						"hasflagmaxtransferlength4m");
_LIT(KHasFlagMaxTransferLength_8M, 						"hasflagmaxtransferlength8m");
_LIT(KHasFlagMaxTransferLength_16M, 					"hasflagmaxtransferlength16m");
_LIT(KPassword, 										"password");
_LIT(KIsNull, 											"isnull");
_LIT(KEffectiveModes, 									"effectivemodes");
_LIT(KEffectiveModesFunctionId, 						"emfunctionid");
_LIT(KEffectiveModesData, 								"emdata");
_LIT(KDriveCount, 										"drivecount");
_LIT(KNumPages, 										"numpages");
_LIT(KPagingType, 										"pagingtype");
_LIT(KWriteProtected, 									"writeprotected");
_LIT(KCardNumber, 										"cardnumber");
_LIT(KSocketWrapperName,								"socketwrappername");


// TEffectiveModesFunctionId enum strings
_LIT(KEffectiveModesRemoveMode, 						"EEffectiveModesRemoveMode");
_LIT(KEffectiveModesSetBusClockInKhz, 					"EEffectiveModesSetBusClockInKhz");
_LIT(KEffectiveModesSetBusyTimeOutInMcs, 				"EEffectiveModesSetBusyTimeOutInMcs");
_LIT(KEffectiveModesSetCRCRetries, 						"EEffectiveModesSetCRCRetries");
_LIT(KEffectiveModesSetDataTimeOutInMcs, 				"EEffectiveModesSetDataTimeOutInMcs");
_LIT(KEffectiveModesSetMode, 							"EEffectiveModesSetMode");
_LIT(KEffectiveModesSetOpCondBusyTimeout, 				"EEffectiveModesSetOpCondBusyTimeout");
_LIT(KEffectiveModesSetPollAttempts, 					"EEffectiveModesSetPollAttempts");
_LIT(KEffectiveModesSetResponseTimeOutInTicks,			"EEffectiveModesSetResponseTimeOutInTicks");
_LIT(KEffectiveModesSetTicksClockIn, 					"EEffectiveModesSetTicksClockIn");
_LIT(KEffectiveModesSetTicksClockOut, 					"EEffectiveModesSetTicksClockOut");
_LIT(KEffectiveModesSetTimeOutRetries, 					"EEffectiveModesSetTimeOutRetries");
_LIT(KInvalidEffectiveModesFunctionId,					"EInvalidEffectiveModesFunctionId");	

/**	Enum as a descriptor  				Enum integar value													
 *	In this case these enums represent TEffectiveModesFunctionId
 *                                                                             
 * 																											      							
 */																														
const CDataWrapperBase::TEnumEntryTable	CT_MMCSDStackDriverData::iEnumEffectiveModesFunctionIdTable [] =
	{
//	Enum as a descriptor								Enum
	KEffectiveModesRemoveMode,							RMMCSDTestControllerInterface::EStackEffectiveModesRemoveMode,
	KEffectiveModesSetBusClockInKhz,					RMMCSDTestControllerInterface::EStackEffectiveModesSetBusClockInKhz,
	KEffectiveModesSetBusyTimeOutInMcs,					RMMCSDTestControllerInterface::EStackEffectiveModesSetBusyTimeOutInMcs,
	KEffectiveModesSetCRCRetries,						RMMCSDTestControllerInterface::EStackEffectiveModesSetCRCRetries,
	KEffectiveModesSetDataTimeOutInMcs,					RMMCSDTestControllerInterface::EStackEffectiveModesSetDataTimeOutInMcs,
	KEffectiveModesSetMode,								RMMCSDTestControllerInterface::EStackEffectiveModesSetMode,
	KEffectiveModesSetOpCondBusyTimeout,				RMMCSDTestControllerInterface::EStackEffectiveModesSetOpCondBusyTimeout,
	KEffectiveModesSetPollAttempts,						RMMCSDTestControllerInterface::EStackEffectiveModesSetPollAttempts,
	KEffectiveModesSetResponseTimeOutInTicks,			RMMCSDTestControllerInterface::EStackEffectiveModesSetResponseTimeOutInTicks,
	KEffectiveModesSetTicksClockIn,						RMMCSDTestControllerInterface::EStackEffectiveModesSetTicksClockIn,
	KEffectiveModesSetTicksClockOut,					RMMCSDTestControllerInterface::EStackEffectiveModesSetTicksClockOut,
	KEffectiveModesSetTimeOutRetries,					RMMCSDTestControllerInterface::EStackEffectiveModesSetTimeOutRetries,
	KInvalidEffectiveModesFunctionId,					-1
	};

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * static Construction
 *
 *
 * @return					A pointer to T_MMCSDStackDriverData
 *
 * @leave					System wide error
 */
CT_MMCSDStackDriverData* CT_MMCSDStackDriverData::NewL()
	{
	CT_MMCSDStackDriverData* ret = new (ELeave) CT_MMCSDStackDriverData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


/**
 * Constructor
 *
 * @return					N/A
 *
 * @leave					N/A
 */
CT_MMCSDStackDriverData::CT_MMCSDStackDriverData()
: CT_MmcSDDriverData()
	{
	}

/**
 * 2nd Phase constructor
 *
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::ConstructL()
	{
	CT_MmcSDDriverData::ConstructL();
	}

/**
 * Destructor
 *
 * @return					N/A
 *
 * @leave					N/A
 */
CT_MMCSDStackDriverData::~CT_MMCSDStackDriverData()
	{
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
TBool CT_MMCSDStackDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;

	if (aCommand == KCmdStackAdjustPartialRead)
		{
		DoCmdAdjustPartialRead(aSection);
		}
	else if (aCommand == KCmdStackAllocSession)
		{
		DoCmdAllocSession();
		}
	else if (aCommand == KCmdStackBufferInfo)
		{
		DoCmdBufferInfo(aSection);
		}
	else if (aCommand == KCmdStackDemandPagingInfo)
		{
		DoCmdDemandPagingInfo(aSection);
		}
	else if (aCommand == KCmdStackEffectiveModes)
		{
		DoCmdEffectiveModes(aSection);
		}
	else if (aCommand == KCmdStackGetBufferInfo)
		{
		DoCmdGetBufferInfo(aSection);
		}
	else if (aCommand == KCmdStackHasCardsPresent)
		{
		DoCmdHasCardsPresent(aSection);
		}
	else if (aCommand == KCmdStackHasSessionsQueued)
		{
		DoCmdHasSessionsQueued(aSection);
		}
	else if (aCommand == KCmdStackInit)
		{
		DoCmdInit();
		}
	else if (aCommand == KCmdStackInitStackInProgress)
		{
		DoCmdInitStackInProgress(aSection);
		}
	else if (aCommand == KCmdStackMMCSocket)
		{
		DoCmdMMCSocketL(aSection);
		}
	else if (aCommand == KCmdStackMachineInfo)
		{
		DoCmdMachineInfo(aSection);
		}
	else if (aCommand == KCmdStackMaxCardsInStack)
		{
		DoCmdMaxCardsInStack(aSection);
		}
	else if (aCommand == KCmdStackReadPasswordStore)
		{
		DoCmdPasswordStore(aSection);
		}
	else if (aCommand == KCmdStackWritePasswordStore)
		{
		DoCmdWritePasswordStore(aSection);
		}
	else if (aCommand == KCmdStackProgramPeriodInMs)
		{
		DoCmdProgramPeriodInMilliSeconds(aSection);
		}
	else if (aCommand == KCmdStackStop)
		{
		DoCmdStop(aSection);
		}
	else if (aCommand == KCmdStackReportPowerUp)
		{
		DoCmdReportPowerUp(aSection);
		}
	else if (aCommand == KCmdStackReportPowerDown)
		{
		DoCmdReportPowerDown(aSection);
		}
	else if (aCommand == KCmdStackCardP)
		{
		DoCmdCardP(aSection);
		}
	else if (aCommand == KCmdStackAlterEffectiveModes)
		{
		DoCmdAlterEffectiveModes(aSection);
		}
	else
		{
		ret = CT_MmcSDDriverData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}
	return ret;
	}


/**
 * Process command that will result in call to DMMCStack::AdjustPartial kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void  CT_MMCSDStackDriverData::DoCmdAdjustPartialRead(const TDesC& aSection)
	{
	TPartialReadData prd;
	prd.iStart = 0;
	prd.iEnd = 0;

	if(! GetUintFromConfig(aSection, KPrdStart(), prd.iStart) )
		{
		ERR_PRINTF1(_L("Failed to get prdstart from ini file"));
		SetBlockResult(EFail);
		}

	if(!GetUintFromConfig(aSection, KPrdEnd(), prd.iEnd))
		{
		ERR_PRINTF1(_L("Failed to get prdend from ini file"));
		SetBlockResult(EFail);		
		}

	INFO_PRINTF3(_L("Calling MMCStack::AdjustPartialRead(start=%d, end=%d)"), prd.iStart, prd.iEnd);
	TInt err = iMmcSDController->StackAdjustPartialRead(prd);

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::AdjustPartialRead() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF3(_L("MMCStack::AdjustPartialRead() = %d, %d"), prd.iPhysStart, prd.iPhysEnd);

		TUint expectedStart;
		if( !GetUintFromConfig(aSection, KPrdExpectedStart(), expectedStart) )
			{
			ERR_PRINTF1(_L("Failed to get KPrdExpectedStart from ini file"));
			SetBlockResult(EFail);
			}
								
		else if(prd.iPhysStart != expectedStart )
			{
			ERR_PRINTF3(_L("phys start(%d) != expectedStart(%d)"), prd.iPhysStart, expectedStart);
			SetBlockResult(EFail);
			}

		TUint expectedEnd;
		if( !GetUintFromConfig(aSection, KPrdExpectedEnd(), expectedEnd) )
			{
			ERR_PRINTF1(_L("Failed to get KPrdExpectedEnd from ini file"));
			SetBlockResult(EFail);			
			}
		else if(prd.iPhysEnd != expectedEnd )
			{
			ERR_PRINTF3(_L("MMCStack::AdjustPartialRead phys end(%d) != expected end(%d)"), prd.iPhysEnd, expectedEnd);
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::AllocSession kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void  CT_MMCSDStackDriverData::DoCmdAllocSession()
	{
	INFO_PRINTF1(_L("Calling MMCStack::AllocSession()"));
	iMmcSDController->StackAllocSession();
	}

/**
 * Process command that will result in call to DMMCStack::BufferInfo kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdBufferInfo(const TDesC& aSection)
	{
	TBufferInfo bufferInfo;
	INFO_PRINTF1(_L("Calling MMCStack::BufferInfo()"));
	TInt err = iMmcSDController->StackBufferInfo(bufferInfo);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::BufferInfo() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF4(_L("BufferInfo() iBuf(%x) iBufLen(%d) iMinorBufLen(%d)"),
				bufferInfo.iBuf, bufferInfo.iBufLen, bufferInfo.iMinorBufLen);

		// check the expected length
		TInt expectedBufLen;
		if( ! GetIntFromConfig(aSection, KExpectedBufLen(), expectedBufLen ))
			{
			ERR_PRINTF1(_L("Failed to get KExpectedBufLen from ini file"));
			SetBlockResult(EFail);
			}
		
		else if(bufferInfo.iBufLen != expectedBufLen )
			{
			ERR_PRINTF3(_L("bufferInfo.iBufLen (%d) != expectedBufLen(%d)"),
					bufferInfo.iBufLen , expectedBufLen);
			SetBlockResult(EFail);
			}

		// check the expected minor length
		TInt expectedMinorBufLen;
		if( !GetIntFromConfig(aSection, KExpectedMinorBufLen(), expectedMinorBufLen ))
			{
			ERR_PRINTF1(_L("Failed to get KExpectedMinorBufLen from ini file"));
			SetBlockResult(EFail);
			}
		else if(bufferInfo.iMinorBufLen != expectedMinorBufLen )
			{
			ERR_PRINTF3(_L("bufferInfo.iMinorBufLen (%d) != expectedMinorBufLen(%d)"),
					bufferInfo.iMinorBufLen , expectedMinorBufLen);
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::GetBufferInfo kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdGetBufferInfo(const TDesC& aSection)
	{
	TBufferInfo bufferInfo;
	INFO_PRINTF1(_L("Calling MMCStack::GetBufferInfo()"));
	TInt err = iMmcSDController->StackGetBufferInfo(bufferInfo);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::GetBufferInfo() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF3(_L("GetBufferInfo() iBuf(%x) iBufLen(%d)"),
				bufferInfo.iBuf, bufferInfo.iBufLen);

		// check the expected length
		TInt expectedBufLen;
		if( !GetIntFromConfig(aSection, KExpectedBufLen(), expectedBufLen ))
			{
			ERR_PRINTF1(_L("Failed to get KExpectedBufLen from ini file"));
			SetBlockResult(EFail);
			}
		else if(bufferInfo.iBufLen != expectedBufLen )
			{
			ERR_PRINTF3(_L("bufferInfo.iBufLen (%d) != expectedBufLen(%d)"),
					bufferInfo.iBufLen , expectedBufLen);
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::DemandPagingInfo kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdDemandPagingInfo(const TDesC& aSection)
	{
	TTCDemandPagingInfo info;
	INFO_PRINTF1(_L("Calling MMCStack::DemandPagingInfo()"));
	TInt err = iMmcSDController->StackDemandPagingInfo(info);
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::DemandPagingInfo() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF4(_L("info.iPagingDriveList(%x) info.iDriveCount(%d) info.iPagingType(%d)"),
				info.iPagingDriveList, info.iDriveCount, info.iPagingType);
		INFO_PRINTF4(_L("info.iReadShift(%d) info.iNumPages(%d) info.iWriteProtected(%d)"),
				info.iReadShift, info.iNumPages, info.iWriteProtected);
		
		// read expected values here		
		TInt driveCount;
		if(!GetIntFromConfig(aSection, KDriveCount(), driveCount))
			{
			ERR_PRINTF1(_L("Failed to read KDriveCount from ini file"));
			SetBlockResult(EFail);
			}
		else
			{
			if(driveCount != info.iDriveCount)
				{
				ERR_PRINTF3(_L("drive counts do not match actual(%d), expected(%d)"), info.iDriveCount, driveCount);
				SetBlockResult(EFail);				
				}	
			}

		TUint pagingType;
		if(!GetUintFromConfig(aSection, KPagingType(), pagingType))
			{
			ERR_PRINTF1(_L("Failed to read pagingType from ini file"));
			SetBlockResult(EFail);
			}
		else
			{
			if(pagingType != info.iPagingType)
				{
				ERR_PRINTF3(_L("pagingType do not match actual(%d), expected(%d)"), info.iPagingType, pagingType);
				SetBlockResult(EFail);				
				}	
			}
		
		
		TUint numPages;
		if(!GetUintFromConfig(aSection, KNumPages(), numPages))
			{
			ERR_PRINTF1(_L("Failed to read num pages from ini file"));
			SetBlockResult(EFail);
			}
		else
			{
			if(numPages != info.iNumPages)
				{
				ERR_PRINTF3(_L("numPages do not match actual(%d), expected(%d)"), info.iNumPages, numPages);
				SetBlockResult(EFail);				
				}	
			}

		TBool writeProtected;
		if(!GetBoolFromConfig(aSection, KWriteProtected(), writeProtected))
			{
			ERR_PRINTF1(_L("Failed to read writeProtected from ini file"));
			SetBlockResult(EFail);
			}
		else
			{
			if(writeProtected != info.iWriteProtected)
				{
				ERR_PRINTF3(_L("writeProtected do not match actual(%d), expected(%d)"), info.iWriteProtected, writeProtected);
				SetBlockResult(EFail);				
				}	
			}
		
		}
	}

/**
 * Process command that will result in call to DMMCStack::EffectiveModes kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdEffectiveModes(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Calling MMCStack::EffectiveModes()"));

	TUint32 actualModes;
	TInt err = iMmcSDController->StackEffectiveModes(actualModes);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DMMCStack::EffectiveModes() Error (%d)"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF3(_L("DMMCStack::EffectiveModes() modes(0x%x / %d)"), actualModes, actualModes);

		TInt expectedModes;
		if(GetHexFromConfig(aSection, KEffectiveModes(), expectedModes))
			{

			if(((TUint32)expectedModes) != actualModes)
				{
				ERR_PRINTF5(_L("actual (%d)(%x) and expected(%d)(%x) effectiveModes do not match"), actualModes, actualModes, expectedModes, expectedModes);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("DMMCStack::EffectiveModes() Could not read expectedModes from ini file"));
			SetError(err);
			}
		}
	}

/**
 * Process command that will result in an update of Effective Modes variable on kernel side
 * for a call to DMMCStack::EffectiveModes
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdAlterEffectiveModes(const TDesC& aSection)
	{	
	TInt	funcId;
	TUint	data;
	
	// read funtion ID from section
	if (!GetEnumFromConfig(aSection, KEffectiveModesFunctionId(), iEnumEffectiveModesFunctionIdTable, funcId))
		{
		ERR_PRINTF1(_L("Failed to read Function ID DoCmdEffectiveModes"));
		SetBlockResult(EFail);
		}
	// read data param from section - all params are 32 bit or less	
	else if (!GetUintFromConfig(aSection, KEffectiveModesData(), data))
		{
		ERR_PRINTF2(_L("Failed to read DATA DoCmdEffectiveModes funcId(%d)"), funcId);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF3( _L("Calling DoCmdEffectiveModes funcid(%d) data(%u)"), funcId, data);
		TInt err = iMmcSDController->StackAlterEffectiveModes((RMMCSDTestControllerInterface::TEffectiveModesFunctionId) funcId, (TAny*)data);
		if (err != KErrNone)
			{
			ERR_PRINTF3(_L("DoCmdEffectiveModes funcid(%d) Error(%d"), funcId, err);
			SetError(err);
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::HasCardsPresent kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdHasCardsPresent(const TDesC& aSection)
	{
	INFO_PRINTF1( _L("CT_MMCSDStackDriverData:::DoCmdHasCardsPresent called"));

	TBool actualCardspresent;
	TInt err = iMmcSDController->StackHasCardsPresent(actualCardspresent);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DMMCStack::DoCmdHasCardsPresent() Error (%d)"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DMMCStack::DoCmdHasCardsPresent() actual cards present (%d)"), actualCardspresent);

		TBool expectedCardsPresent;
		if(!GetBoolFromConfig(aSection, KCardsPresent(), expectedCardsPresent))
			{
			ERR_PRINTF1(_L("Failed to get KCardsPresent from ini file"));
			SetBlockResult(EFail);
			}
		else if(actualCardspresent != expectedCardsPresent)
			{
			ERR_PRINTF3(_L("actual (%d) and expected(%d) cards present do not match"), actualCardspresent, expectedCardsPresent);
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::HasSessionsQueued kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdHasSessionsQueued(const TDesC& aSection)
	{
	INFO_PRINTF1( _L(" CT_MMCSDStackDriverData::DoCmdHasSessionsQueued called"));

	TBool actualSessionsQueued;
	TInt err = iMmcSDController->StackHasSessionsQueued(actualSessionsQueued);

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DMMCStack::DoCmdHasSessionsQueued() Error (%d)"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DMMCStack::DoCmdHasSessionsQueued() actual sessions queued (%d)"), actualSessionsQueued);

		TBool expectedInProgress;
		if(!GetBoolFromConfig(aSection, KSessionsQueued(), expectedInProgress))
			{
			ERR_PRINTF1(_L("Failed to get KSessionsQueued from ini file"));
			SetBlockResult(EFail);
			}
		else if(actualSessionsQueued != expectedInProgress)
			{
			ERR_PRINTF3(_L("actual (%d) and expected(%d) sessions queued do not match"), actualSessionsQueued, expectedInProgress);
			SetBlockResult(EFail);
			}
		}
	}
/**
 * Process command that will result in call to DMMCStack::InitStackInProgress kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdInitStackInProgress(const TDesC& aSection)
	{
	INFO_PRINTF1( _L(" CT_MMCSDStackDriverData::DoCmdInitStackInProgress called"));

	TBool actualInProgress;
	TInt err = iMmcSDController->StackInitStackInProgress(actualInProgress);

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DMMCStack::InitStackInProgress() Error (%d)"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DMMCStack::InitStackInProgress() actualInProgress(%d)"), actualInProgress);

		TBool expectedInProgress;
		if(!GetBoolFromConfig(aSection, KInProgress(), expectedInProgress))
			{
			ERR_PRINTF1(_L("Failed to get KInProgress from ini file"));
			SetBlockResult(EFail);
			}
		else if(actualInProgress != expectedInProgress)
			{
			ERR_PRINTF3(_L("actual (%d) and expected(%d) in progress do not match"), actualInProgress, expectedInProgress);
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::MMCSocket kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdMMCSocketL(const TDesC& aSection)
	{
	INFO_PRINTF1( _L(" CT_MMCSDStackDriverData::DoCmdMMCSocket called"));
	// can't use DMMCSocket type on user side
	TAny* socketPtr= NULL;

	TInt err = iMmcSDController->StackMMCSocket(&socketPtr);

	if (err!=KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::Socket() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("MMCStack::Socket(): ptr=%x"), socketPtr);
		TBool expectedIsNull = EFalse;
		if(!GetBoolFromConfig(aSection, KIsNull(), expectedIsNull))
			{
			ERR_PRINTF1(_L("Failed to get isnull from ini file"));
			SetBlockResult(EFail);
			}

		if (expectedIsNull)
			{
			if (socketPtr != NULL)
				{
				ERR_PRINTF1(_L("Socket ptr is not NULL"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			if (!socketPtr)
				{
				ERR_PRINTF1(_L("Socket ptr is NULL"));
				SetBlockResult(EFail);
				}
			else
				{
				TPtrC	socketWrapperName;
				if( GetStringFromConfig(aSection, KSocketWrapperName(), socketWrapperName) )
					{
					TAny* expectedSocketPtr = GetDataObjectL(socketWrapperName);
					INFO_PRINTF2(_L("expectedSocketPtr = %x"), expectedSocketPtr);

					if( expectedSocketPtr == NULL )
						{
						ERR_PRINTF1(_L("MMCStack::Socket(): expectedSocketPtr is NULL"));
						SetBlockResult(EFail);
						}
					else if( socketPtr != expectedSocketPtr )
						{
						ERR_PRINTF3(_L("socketPtr(%x) != expectedSocketPtr(%x)"), socketPtr, expectedSocketPtr);
						SetBlockResult(EFail);
						}
					}
				else
					{
					ERR_PRINTF1(_L("MMCStack::Socket(): Error in Socket Wrapper Name from INI file"));
					SetBlockResult(EFail);
					}
				}
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::MachineInfo kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdMachineInfo(const TDesC& aSection)
	{
	INFO_PRINTF1( _L(" CT_MMCSDStackDriverData::DoCmdMachineInfo called XXX"));

	TMachineInfo machineInfo;
	TInt err = iMmcSDController->StackMachineInfo(machineInfo);

	if (err !=KErrNone)
		{
		ERR_PRINTF2(_L("DMMCStack::MachineInfo() Error (%d)"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DMMCStack::MachineInfo(): iTotalSockets=%d"), machineInfo.iTotalSockets);
		INFO_PRINTF2(_L("DMMCStack::MachineInfo(): iTotalMediaChanges=%d"), machineInfo.iTotalMediaChanges);
		INFO_PRINTF2(_L("DMMCStack::MachineInfo(): iTotalPrimarySupplies=%d"), machineInfo.iTotalPrimarySupplies);
		INFO_PRINTF2(_L("DMMCStack::MachineInfo(): iSPIMode=%d"), machineInfo.iSPIMode);
		INFO_PRINTF2(_L("DMMCStack::MachineInfo(): iBaseBusNumber=%d"), machineInfo.iBaseBusNumber);

		if (machineInfo.iSupportsSPIMode)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag ESupportsSPIMode"));
			}
		if (machineInfo.iSupportsDoubleBuffering)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag ESupportsDoubleBuffering"));
			}
		if (machineInfo.iSupportsR7)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag ESupportsR7"));
			}
  		if( machineInfo.iDma8BitAddressing )
  			{
  			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EDma8BitAddressing"));
  			}
  		if( machineInfo.iDma16BitAddressing )
  			{
  			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EDma16BitAddressing"));
  			}
  		if( machineInfo.iDma32BitAddressing )
  			{
  			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EDma32BitAddressing"));
  			}
  		if( machineInfo.iDma64BitAddressing )
  			{
  			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EDma64BitAddressing"));
  			}
  		if( machineInfo.iSupportsDMA )
  			{
  			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag ESupportsDMA"));
  			}
		if (machineInfo.iMaxTransferLength_256K)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EMaxTransferLength_256K"));
			}
		if (machineInfo.iMaxTransferLength_512K)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EMaxTransferLength_512K"));
			}
		if (machineInfo.iMaxTransferLength_1M)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EMaxTransferLength_1M"));
			}
		if (machineInfo.iMaxTransferLength_2M)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EMaxTransferLength_2M"));
			}
		if (machineInfo.iMaxTransferLength_4M)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EMaxTransferLength_4M"));
			}
		if (machineInfo.iMaxTransferLength_8M)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EMaxTransferLength_8M"));
			}
		if (machineInfo.iMaxTransferLength_16M)
			{
			INFO_PRINTF1(_L("DMMCStack::MachineInfo(): Has flag EMaxTransferLength_16M"));
			}

		TInt expectedInt;
		TBool expectedBool;

		if( !GetIntFromConfig(aSection, KExpectedTotalSockets(), expectedInt))
			{
			ERR_PRINTF1(_L("Failed to get KExpectedTotalSockets from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iTotalSockets != expectedInt )
			{
			ERR_PRINTF3(_L("totalSockets(%d) != expectedTotalSockets(%d)"), machineInfo.iTotalSockets, expectedInt);
			SetBlockResult(EFail);
			}

		if( !GetIntFromConfig(aSection, KExpectedTotalMediaChanges(), expectedInt))
			{
			ERR_PRINTF1(_L("Failed to get expected total media changes from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iTotalMediaChanges != expectedInt )
			{
			ERR_PRINTF3(_L("totalMediaChanges(%d) != expectedTotalMediaChanges(%d)"), machineInfo.iTotalMediaChanges, expectedInt);
			SetBlockResult(EFail);
			}

		if( !GetIntFromConfig(aSection, KExpectedTotalPrimarySupplies(), expectedInt))
			{
			ERR_PRINTF1(_L("Failed to get KExpectedTotalPrimarySupplies from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iTotalPrimarySupplies != expectedInt )
			{
			ERR_PRINTF3(_L("totalPrimarySupplies(%d) != expectedTotalPrimarySupplies(%d)"), machineInfo.iTotalPrimarySupplies, expectedInt);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KExpectedSPIMode(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KExpectedSPIMode from ini file"));
			SetBlockResult(EFail);
			}		
		else if(machineInfo.iSPIMode != expectedBool )
			{
			ERR_PRINTF3(_L("SPIMode(%d) != expectedSPIMode(%d)"), machineInfo.iSPIMode, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetIntFromConfig(aSection, KExpectedBaseBusNumber(), expectedInt))
			{
			ERR_PRINTF1(_L("Failed to get KExpectedBaseBusNumber from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iBaseBusNumber != expectedInt )
			{
			ERR_PRINTF3(_L("baseBusNumber(%d) != expectedBaseBusNumber(%d)"), machineInfo.iBaseBusNumber, expectedInt);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagSupportsSPIMode(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagSupportsSPIMode from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iSupportsSPIMode != expectedBool )
			{
			ERR_PRINTF3(_L("supportsSPIMode(%d) != expectedSupportsSPIMode(%d)"), machineInfo.iSupportsSPIMode, expectedBool);
			SetBlockResult(EFail);
			}

		if(!GetBoolFromConfig(aSection, KHasFlagSupportsDoubleBuffering(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagSupportsDoubleBuffering from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iSupportsDoubleBuffering != expectedBool )
			{
			ERR_PRINTF3(_L("supportsDoubleBuffering(%d) != expectedSupportsDoubleBuffering(%d)"), machineInfo.iSupportsDoubleBuffering, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagSupportsR7(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagSupportsR7 from ini file"));
			SetBlockResult(EFail);

			}
		else if(machineInfo.iSupportsR7 != expectedBool )
			{
			ERR_PRINTF3(_L("supportsR7(%d) != expectedSupportsR7(%d)"), machineInfo.iSupportsR7, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagDma8BitAddressing(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagDma8BitAddressing from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iDma8BitAddressing != expectedBool )
			{
			ERR_PRINTF3(_L("Dma8BitAddressing(%d) != expectedDma8BitAddressing(%d)"), machineInfo.iDma8BitAddressing, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagDma16BitAddressing(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagDma16BitAddressing from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iDma16BitAddressing != expectedBool )
			{
			ERR_PRINTF3(_L("Dma16BitAddressing(%d) != expectedDma16BitAddressing(%d)"), machineInfo.iDma16BitAddressing, expectedBool);
			SetBlockResult(EFail);
			}		

		if( !GetBoolFromConfig(aSection, KHasFlagDma32BitAddressing(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagDma32BitAddressing from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iDma32BitAddressing != expectedBool )
			{
			ERR_PRINTF3(_L("Dma32BitAddressing(%d) != expectedDma32BitAddressing(%d)"), machineInfo.iDma32BitAddressing, expectedBool);
			SetBlockResult(EFail);
			}	
		
		if( !GetBoolFromConfig(aSection, KHasFlagDma64BitAddressing(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagDma64BitAddressing from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iDma64BitAddressing != expectedBool )
			{
			ERR_PRINTF3(_L("Dma64BitAddressing(%d) != expectedDma64BitAddressing(%d)"), machineInfo.iDma64BitAddressing, expectedBool);
			SetBlockResult(EFail);
			}	
		
		if( !GetBoolFromConfig(aSection, KHasFlagSupportsDMA(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagSupportsDMA from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iSupportsDMA != expectedBool )
			{
			ERR_PRINTF3(_L("SupportsDMA(%d) != expectedSupportsDMA(%d)"), machineInfo.iSupportsDMA, expectedBool);
			SetBlockResult(EFail);
			}			
	
		if( !GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_256K(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagMaxTransferLength_256K( from ini file"));
			SetBlockResult(EFail);
			}
		else if(machineInfo.iMaxTransferLength_256K != expectedBool )
			{
			ERR_PRINTF3(_L("maxTransferLength_256K(%d) != expectedMaxTransferLength_256K(%d)"), machineInfo.iMaxTransferLength_256K, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection,  KHasFlagMaxTransferLength_512K(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get  KHasFlagMaxTransferLength_512K from ini file"));
			SetBlockResult(EFail);

			}
		else if(machineInfo.iMaxTransferLength_512K != expectedBool )
			{
			ERR_PRINTF3(_L("maxTransferLength_512K(%d) != expectedMaxTransferLength_512K(%d)"), machineInfo.iMaxTransferLength_512K, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_1M(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagMaxTransferLength_1M from ini file"));
			SetBlockResult(EFail);

			}
		else if(machineInfo.iMaxTransferLength_1M != expectedBool )
			{
			ERR_PRINTF3(_L("maxTransferLength_1M(%d) != expectedMaxTransferLength_1M(%d)"), machineInfo.iMaxTransferLength_1M, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_2M(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagMaxTransferLength_2M from ini file"));
			SetBlockResult(EFail);

			}
		else if(machineInfo.iMaxTransferLength_2M != expectedBool )
			{
			ERR_PRINTF3(_L("maxTransferLength_2M(%d) != expectedMaxTransferLength_2M(%d)"), machineInfo.iMaxTransferLength_2M, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_4M(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagMaxTransferLength_4M from ini file"));
			SetBlockResult(EFail);			
			}
		else if(machineInfo.iMaxTransferLength_4M != expectedBool )
			{
			ERR_PRINTF3(_L("maxTransferLength_4M(%d) != expectedMaxTransferLength_4M(%d)"), machineInfo.iMaxTransferLength_4M, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_8M(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagMaxTransferLength_8M from ini file"));
			SetBlockResult(EFail);						
			}
		else if(machineInfo.iMaxTransferLength_8M != expectedBool )
			{
			ERR_PRINTF3(_L("maxTransferLength_8M(%d) != expectedMaxTransferLength_8M(%d)"), machineInfo.iMaxTransferLength_8M, expectedBool);
			SetBlockResult(EFail);
			}

		if( !GetBoolFromConfig(aSection, KHasFlagMaxTransferLength_16M(), expectedBool))
			{
			ERR_PRINTF1(_L("Failed to get KHasFlagMaxTransferLength_16M from ini file"));
			SetBlockResult(EFail);			

			}
		else if(machineInfo.iMaxTransferLength_16M != expectedBool )
			{
			ERR_PRINTF3(_L("maxTransferLength_16M(%d) != expectedMaxTransferLength_16M(%d)"), machineInfo.iMaxTransferLength_16M, expectedBool);
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::MaxCardsInStack kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */

void CT_MMCSDStackDriverData::DoCmdMaxCardsInStack(const TDesC& aSection)
	{
	INFO_PRINTF1( _L(" CT_MMCSDStackDriverData::DoCmdMaxCardsInStack called"));

	TUint actualMaxCardsInStack;
	iMmcSDController->StackMaxCardsInStack(actualMaxCardsInStack);

	TUint expectedMaxCardsInStack;
	if(!GetUintFromConfig(aSection, KExpectedMaxCardsInStack(), expectedMaxCardsInStack))
		{
		ERR_PRINTF1(_L("Failed to get  KExpectedMaxCardsInStack from ini file"));
		SetBlockResult(EFail);			

		}
	else if(actualMaxCardsInStack != expectedMaxCardsInStack)
		{
		ERR_PRINTF3(_L("MaxCardsInStack actualMaxCardsInStack (%d) != expectedMaxCardsInStack(%d)")
				,actualMaxCardsInStack, expectedMaxCardsInStack);
		SetBlockResult(EFail);
		}
	}

/**
 * Process command that will result in call to DMMCStack::PasswordStore kernel side
 * and retrieve the password in the store
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdPasswordStore(const TDesC& aSection)
	{
	INFO_PRINTF1( _L(" CT_MMCSDStackDriverData::DoCmdPasswordStore called"));

	TStackPasswordStoreData pwdStoreData;
	TInt err = iMmcSDController->StackReadPasswordStore(pwdStoreData);

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::DoCmdPasswordStore() Error %d"), err);
		SetError(err);
		}
	else
		{
		if (!pwdStoreData.iPwdStorePtr)
			{
			ERR_PRINTF1(_L("MMCStack::DoCmdPasswordStore() retrieved a NULL pointer"));
			SetError(err);
			}
		else
			{
			INFO_PRINTF2( _L("Password Store ptr (%x)"), pwdStoreData.iPwdStorePtr);

			TBuf<KMmcPwdLen> pwd;
			pwd.Copy(pwdStoreData.iPassword);
			INFO_PRINTF2( _L("PwdStore Password (%S)"), &pwd);

			TPtrC tmp;
			if (GetStringFromConfig(aSection, KPassword(), tmp))
				{
				INFO_PRINTF2( _L("Expected password (%S)"), &tmp);
				if(tmp != pwd)
					{
					ERR_PRINTF1( _L("PASSWORD does not match"));
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF1( _L("failed to read password"));
				SetBlockResult(EFail);	
				}
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::PasswordStore kernel side
 * and update the password in the store
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdWritePasswordStore(const TDesC& aSection)
	{

	INFO_PRINTF1( _L(" CT_MMCSDStackDriverData::DoCmdWritePasswordStore called"));

	// read the password to set from ini file			
	TPtrC tmp;
	if (GetStringFromConfig(aSection, KPassword(), tmp))
		{
		INFO_PRINTF2( _L("Attempting to write password (%S) to password store"), &tmp);

		TStackPasswordStoreData pwdStoreData;
		pwdStoreData.iPassword.Copy(tmp);
		TInt err = iMmcSDController->StackWritePasswordStore(pwdStoreData);
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("Wrote password ok"));
			}
		else
			{
			ERR_PRINTF2( _L("StackWritePasswordStore failed err(%d)"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF1( _L("Failed to read PASSWORD to write from ini file"));
		SetBlockResult(EFail);
		}
	}

/**
 * Process command that will result in call to DMMCStack::ProgramPeriodMs kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdProgramPeriodInMilliSeconds(const TDesC& aSection)
	{
	TInt actualPeriod;
	INFO_PRINTF1(_L("Calling MMCStack::ProgramPeriodInMilliSeconds()"));
	TInt err = iMmcSDController->StackProgramPeriodInMilliSeconds(actualPeriod);

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DMMCStack::ProgramPeriodInMilliSeconds() Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DMMCStack::ProgramPeriodInMilliSeconds() actualPeriod(%d)"), actualPeriod);

		TInt expectedPeriod;
		if (GetIntFromConfig(aSection, KProgramPeriodMs(), expectedPeriod)
				&& actualPeriod != expectedPeriod)
			{
			ERR_PRINTF3(_L("actual (%d) and expected(%d) periods do not match"), actualPeriod, expectedPeriod);
			SetBlockResult(EFail);
			}
		}
	}

/**
 * Process command that will result in call to DMMCStack::Stop kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdStop(const TDesC& /*aSection*/)
	{
	INFO_PRINTF1(_L("Calling MMCStack::Stop()"));
	TInt err = iMmcSDController->StackStop();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::Stop() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Process command that will result in call to DMMCStack::Init kernel side
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdInit()
	{
	INFO_PRINTF1(_L("Calling MMCStack::Init()"));
	TInt err = iMmcSDController->StackInit();
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::Init() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Process command that will result in call to DMMCStack::ReportPowerUp kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdReportPowerUp(const TDesC& /*aSection*/)
	{
	INFO_PRINTF1(_L("Calling MMCStack::ReportPowerUp()"));
	TInt err = iMmcSDController->StackReportPowerUp();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::ReportPowerUp() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Process command that will result in call to DMMCStack::ReportPowerDown kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdReportPowerDown(const TDesC& /*aSection*/)
	{
	INFO_PRINTF1(_L("Calling MMCStack::ReportPowerDown()"));
	TInt err = iMmcSDController->StackReportPowerDown();
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("MMCStack::ReportPowerDown() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Process command that will result in call to DMMCStack::CardP kernel side
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDStackDriverData::DoCmdCardP(const TDesC& aSection)
	{
	TCardPtr cardPtr;
	if( GetIntFromConfig(aSection, KCardNumber(), cardPtr.iCardNumber) )
		{
		INFO_PRINTF2(_L("Calling MMCStack::CardP(%d)"), cardPtr.iCardNumber);
		TInt err = iMmcSDController->StackCardP(cardPtr);
	
		if( err!=KErrNone )
			{
			ERR_PRINTF2(_L("MMCStack::CardP() Error %d"), err);
			SetError(err);
			}
		else
			{
			INFO_PRINTF2(_L("MMCStack::CardP(): ptr=%x"), cardPtr.iCardPtr);
			TBool	expectedIsNull;
			if( GetBoolFromConfig(aSection, KIsNull(), expectedIsNull) )
				{
				if( expectedIsNull )
					{
					if( cardPtr.iCardPtr != NULL )
						{
						ERR_PRINTF1(_L("Card ptr is not NULL"));
						SetBlockResult(EFail);
						}
					}
				else
					{
					if( cardPtr.iCardPtr == NULL )
						{
						ERR_PRINTF1(_L("Card ptr is NULL"));
						SetBlockResult(EFail);
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
		ERR_PRINTF1(_L("Card Number not given in ini file"));
		SetBlockResult(EFail);			
		}		
	}
