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

#include "T_MmcSDDriverData.h"

//MMCSD Commands index
/*@{*/
_LIT(KCmdConstructor,					"NewL");
_LIT(KCmdReadSector, 					"ReadSector");
_LIT(KCmdWriteSector, 					"WriteSector");
_LIT(KCmdReadCSD, 						"ReadCSD");
_LIT(KCmdReadExtCSD, 					"ReadExtCSD");
_LIT(KCmdCardInfo, 						"CardInfo");
_LIT(KCmdCardIsPresent, 				"CardIsPresent");
_LIT(KCmdCardIsReady, 					"CardIsReady");
_LIT(KCmdCardIsLocked, 					"CardIsLocked");
_LIT(KCmdDestructor,					"~");

_LIT(KExpectedAsyncError, 				"expectedasyncerror");
_LIT(KSectorNumber, 					"sectornumber");
_LIT(KSectorText,						"sectortext");
_LIT(KRepeats,							"repeats");
_LIT(KHighCapacity, 					"highcapacity");
_LIT(KMaxReadBlockLen, 					"maxreadblocklen");
_LIT(KReadCurrentInMilliAmps, 			"readcurrentinmilliamps");
_LIT(KTransferSpeed, 					"transferspeed");
_LIT(KSessionWrapperName,				"sessionwrappername");
_LIT(KExpectedIsPresent, 				"expectedispresent");
_LIT(KExpectedIsReady, 					"expectedisready");
_LIT(KExpectedIsLocked, 				"expectedislocked");
/*@}*/



/**
 * Construction
 *
 * @return					N/A
 */
CT_MmcSDDriverData::CT_MmcSDDriverData()
:	CDataWrapperBase()
,	iMmcSDController(NULL)
,	iStackNum(0)
,	iAsyncErrorIndex(0)
	{
	}

/**
 * Second phase construction
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MmcSDDriverData::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}

/**
 * Public destructor
 *
 * @return					N/A
 */
CT_MmcSDDriverData::~CT_MmcSDDriverData()
	{
	iFs.Close();

	iActiveCallbacks.ResetAndDestroy();
	}

/**
 * Return a pointer to the object that the data wraps
 *
 * @return					pointer to the object that the data wraps
 */
TAny* CT_MmcSDDriverData::GetObject()
	{
	return iMmcSDController;
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
TBool CT_MmcSDDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdConstructor )
		{
		DoCmdConstructorL(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if(aCommand == KCmdReadSector)
		{
		DoCmdReadSectorL(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdWriteSector)
		{
		DoCmdWriteSectorL(aSection, aAsyncErrorIndex);
		}
	else if(aCommand == KCmdReadCSD)
		{
		DoCmdReadCSD();
		}
	else if(aCommand == KCmdReadExtCSD)
		{
		DoCmdReadExtCSD();
		}
	else if(aCommand == KCmdCardInfo)
		{
		DoCmdCardInfo(aSection);
		}
	else if(aCommand == KCmdCardIsPresent)
		{
		DoCmdCardIsPresent(aSection);
		}
	else if(aCommand == KCmdCardIsReady)
		{
		DoCmdCardIsReady(aSection);
		}
	else if(aCommand == KCmdCardIsLocked)
		{
		DoCmdCardIsLocked(aSection);
		}
	else
		{
		ret=CDataWrapperBase::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}
	return ret;
	}

/**
 * Creates the RBusLogicalChannel derived interface
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MmcSDDriverData::DoCmdConstructorL(const TDesC& aSection)
	{
	TPtrC	wrapperName;
	if( GetStringFromConfig(aSection, KSessionWrapperName(), wrapperName) )
		{
		iMmcSDController = static_cast<RMMCSDTestControllerInterface*>(GetDataObjectL(wrapperName));
		INFO_PRINTF2(_L("iMmcSDController = %x"), iMmcSDController);

		if( iMmcSDController == NULL )
			{
			ERR_PRINTF1(_L("iMmcSDController is NULL-Constructor"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("Error in Session Wrapper Name from INI file-Constructor"));
		SetBlockResult(EFail);
		}
	}

/**
 * Destructor
 *
 * @return					void
 */
void CT_MmcSDDriverData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("CT_MmcSDDriverData::DoCmdDestructor()"));
	}

/**
 * Creates an active callback wrapper and adds to the list
 *
 * @param aFunctionId			The active callback function ID
 * @param aExpectedAsyncError	The active callback expected asynchronous error
 *
 * @return						The active callback wrapper created
 */
CActiveCallbackWrap* CT_MmcSDDriverData::CreateActiveCallbackL(TInt aFunctionId, TInt aExpectedAsyncError)
	{
	CActiveCallbackWrap* activeCallbackWrap = CActiveCallbackWrap::NewL(*this, aFunctionId, aExpectedAsyncError);
	iActiveCallbacks.Append(activeCallbackWrap);
	return activeCallbackWrap;
	}

/**
 * Deletes an active callback from the list
 *
 * @param activeCallback	The active callback to delete
 *
 * @return					void
 */
void CT_MmcSDDriverData::DeleteActiveCallback(CActiveCallback* activeCallback)
	{
	if(activeCallback)
		{
		// loop through all wraps until we find the appropriate item

		CActiveCallbackWrap* activeCallbackWrap = NULL;

		for(int i=0;i<iActiveCallbacks.Count();i++)
			{
			if( &(iActiveCallbacks[i]->ActiveCallback()) == activeCallback)
				{
				activeCallbackWrap = iActiveCallbacks[i];
				iActiveCallbacks.Remove(i);
				break;
				}
			}

		delete activeCallbackWrap;
		}
	}

/**
 * Reads a sector from the card
 *
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	The asynchronous error index
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MmcSDDriverData::DoCmdReadSectorL(const TDesC& aSection, TInt aAsyncErrorIndex)
	{

	INFO_PRINTF1(_L("DoCmdReadSectorL called()"));

	TInt sectorNumber;

	if(!GetIntFromConfig(aSection, KSectorNumber(), sectorNumber))
		{
		ERR_PRINTF1(_L("DoCmdReadSectorL FAILED TO READ sectorNumber"));
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("DoCmdReadSectorL sectorNumber(%d)"), sectorNumber);
		HBufC8* readData = HBufC8::NewLC(KSectorSizeInBytes);
		CActiveCallbackWrap* activeCallbackWrap = CreateActiveCallbackL(RMMCSDTestControllerInterface::EReadSector, GetExpectedAsyncError(aSection));
		activeCallbackWrap->SetDesData(readData);
		
		TPtrC 	expectedSectorText;
		if(GetStringFromConfig(aSection, KSectorText(), expectedSectorText))
			{
			TInt	noOfRepeats = 1;
			GetIntFromConfig(aSection, KRepeats(), noOfRepeats);

			HBufC8* expectedData = HBufC8::NewLC(KSectorSizeInBytes);
			activeCallbackWrap->SetExpectedDesData(expectedData);
			
			activeCallbackWrap->ExpectedDataPtr().Zero();
			for(TInt i=0;i<noOfRepeats;i++)
				{
				activeCallbackWrap->ExpectedDataPtr().Append(expectedSectorText);
				}

		 	TBuf16<KSectorSizeInBytes> printBuf;
		 	printBuf.Copy(activeCallbackWrap->ExpectedDataPtr());
			INFO_PRINTF3(_L("DoCmdReadSectorL expectedSectorText (length=%d):\n%S"), printBuf.Length(), &printBuf);

			CleanupStack::Pop(expectedData);
			}

		iMmcSDController->ReadSector(activeCallbackWrap->ActiveCallback().iStatus, sectorNumber, activeCallbackWrap->DataPtr());
		activeCallbackWrap->ActiveCallback().Activate(aAsyncErrorIndex);
		IncOutstanding();
		CleanupStack::Pop(readData);
		}
	}

/**
 * Writes a sector to the card
 *
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	The asynchronous error index
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MmcSDDriverData::DoCmdWriteSectorL(const TDesC& aSection, TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("DoCmdWriteSectorL called()"));
	TInt	sectorNumber;
	TPtrC 	sectorText;

	if(!GetIntFromConfig(aSection, KSectorNumber(), sectorNumber))
		{
		ERR_PRINTF1(_L("DoCmdWriteSectorL FAILED TO READ sectorNumber"));
		SetBlockResult(EFail);
		}
	else if(!GetStringFromConfig(aSection, KSectorText(), sectorText))
		{
		ERR_PRINTF1(_L("DoCmdWriteSectorL FAILED TO READ sectorText"));
		SetBlockResult(EFail);
		}
	else
		{
		TInt	noOfRepeats = 1;
		GetIntFromConfig(aSection, KRepeats(), noOfRepeats);

		HBufC8* writeData = HBufC8::NewLC(KSectorSizeInBytes);

		CActiveCallbackWrap* activeCallbackWrap = CreateActiveCallbackL(RMMCSDTestControllerInterface::EWriteSector, GetExpectedAsyncError(aSection));
		activeCallbackWrap->SetDesData(writeData);

		activeCallbackWrap->DataPtr().Zero();
		for(TInt i=0;i<noOfRepeats;i++)
			{
			activeCallbackWrap->DataPtr().Append(sectorText);
			}

	 	TBuf16<KSectorSizeInBytes> printBuf;
	 	printBuf.Copy(activeCallbackWrap->DataPtr());
		INFO_PRINTF4(_L("DoCmdWriteSectorL sectorNumber(%d) sectorText (length=%d):\n%S"), sectorNumber, printBuf.Length(), &printBuf);

		iMmcSDController->WriteSector(activeCallbackWrap->ActiveCallback().iStatus, sectorNumber, activeCallbackWrap->DataPtr());
		activeCallbackWrap->ActiveCallback().Activate(aAsyncErrorIndex);
		IncOutstanding();
		CleanupStack::Pop(writeData);
		}
	}

/**
 * Prints a CSD
 *
 * @param aCSDInfo			The CSD to print
 *
 * @return					void
 */
void CT_MmcSDDriverData::PrintCSD(const TCSDInfo& aCSDInfo)
	{
	INFO_PRINTF2(_L("CSDInfo->CSDStructure() = %u"), aCSDInfo.iCSDStructure);
	INFO_PRINTF2(_L("CSDInfo->SpecVers() = %u"), aCSDInfo.iSpecVers);
	INFO_PRINTF2(_L("CSDInfo->Reserved120() = %u"), aCSDInfo.iReserved120);
	INFO_PRINTF2(_L("CSDInfo->TAAC() = %u"), aCSDInfo.iTAAC);
	INFO_PRINTF2(_L("CSDInfo->NSAC() = %u"), aCSDInfo.iNSAC);
	INFO_PRINTF2(_L("CSDInfo->TranSpeed() = %u"), aCSDInfo.iTranSpeed);
	INFO_PRINTF2(_L("CSDInfo->CCC() = %u"), aCSDInfo.iCCC);
	INFO_PRINTF2(_L("CSDInfo->ReadBlLen() = %u"), aCSDInfo.iReadBlLen);
	INFO_PRINTF2(_L("CSDInfo->Reserved74() = %u"), aCSDInfo.iReserved74);
	INFO_PRINTF2(_L("CSDInfo->CSize() = %u"), aCSDInfo.iCSize);
	INFO_PRINTF2(_L("CSDInfo->VDDRCurrMin() = %u"), aCSDInfo.iVDDRCurrMin);
	INFO_PRINTF2(_L("CSDInfo->VDDRCurrMax() = %u"), aCSDInfo.iVDDRCurrMax);
	INFO_PRINTF2(_L("CSDInfo->VDDWCurrMin() = %u"), aCSDInfo.iVDDWCurrMin);
	INFO_PRINTF2(_L("CSDInfo->VDDWCurrMax() = %u"), aCSDInfo.iVDDWCurrMax);
	INFO_PRINTF2(_L("CSDInfo->CSizeMult() = %u"), aCSDInfo.iCSizeMult);
	INFO_PRINTF2(_L("CSDInfo->EraseGrpSize() = %u"), aCSDInfo.iEraseGrpSize);
	INFO_PRINTF2(_L("CSDInfo->EraseGrpMult() = %u"), aCSDInfo.iEraseGrpMult);
	INFO_PRINTF2(_L("CSDInfo->WPGrpSize() = %u"), aCSDInfo.iWPGrpSize);
	INFO_PRINTF2(_L("CSDInfo->DefaultECC() = %u"), aCSDInfo.iDefaultECC);
	INFO_PRINTF2(_L("CSDInfo->R2WFactor() = %u"), aCSDInfo.iR2WFactor);
	INFO_PRINTF2(_L("CSDInfo->WriteBlLen() = %u"), aCSDInfo.iWriteBlLen);
	INFO_PRINTF2(_L("CSDInfo->Reserved16() = %u"), aCSDInfo.iReserved16);
	INFO_PRINTF2(_L("CSDInfo->FileFormat() = %u"), aCSDInfo.iFileFormat);
	INFO_PRINTF2(_L("CSDInfo->ECC() = %u"), aCSDInfo.iECC);
	INFO_PRINTF2(_L("CSDInfo->CRC() = %u"), aCSDInfo.iCRC);
	INFO_PRINTF2(_L("CSDInfo->DeviceSize() = %u"), aCSDInfo.iDeviceSize);
	INFO_PRINTF2(_L("CSDInfo->ReadBlockLength() = %u"), aCSDInfo.iReadBlockLength);
	INFO_PRINTF2(_L("CSDInfo->WriteBlockLength() = %u"), aCSDInfo.iWriteBlockLength);
	INFO_PRINTF2(_L("CSDInfo->EraseSectorSize() = %u"), aCSDInfo.iEraseSectorSize);
	INFO_PRINTF2(_L("CSDInfo->EraseGroupSize() = %u"), aCSDInfo.iEraseGroupSize);
	INFO_PRINTF2(_L("CSDInfo->MinReadCurrentInMilliamps() = %u"), aCSDInfo.iMinReadCurrentInMilliamps);
	INFO_PRINTF2(_L("CSDInfo->MinWriteCurrentInMilliamps() = %u"), aCSDInfo.iMinWriteCurrentInMilliamps);
	INFO_PRINTF2(_L("CSDInfo->MaxReadCurrentInMilliamps() = %u"), aCSDInfo.iMaxReadCurrentInMilliamps);
	INFO_PRINTF2(_L("CSDInfo->MaxWriteCurrentInMilliamps() = %u"), aCSDInfo.iMaxWriteCurrentInMilliamps);
	INFO_PRINTF2(_L("CSDInfo->MaxTranSpeedInKilohertz() = %u"), aCSDInfo.iMaxTranSpeedInKilohertz);
	INFO_PRINTF2(_L("CSDInfo->ReadBlPartial() = %d"), aCSDInfo.iReadBlPartial);
	INFO_PRINTF2(_L("CSDInfo->WriteBlkMisalign() = %d"), aCSDInfo.iWriteBlkMisalign);
	INFO_PRINTF2(_L("CSDInfo->ReadBlkMisalign() = %d"), aCSDInfo.iReadBlkMisalign);
	INFO_PRINTF2(_L("CSDInfo->DSRImp() = %d"), aCSDInfo.iDSRImp);
	INFO_PRINTF2(_L("CSDInfo->WPGrpEnable() = %d"), aCSDInfo.iWPGrpEnable);
	INFO_PRINTF2(_L("CSDInfo->WriteBlPartial() = %d"), aCSDInfo.iWriteBlPartial);
	INFO_PRINTF2(_L("CSDInfo->FileFormatGrp() = %d"), aCSDInfo.iFileFormatGrp);
	INFO_PRINTF2(_L("CSDInfo->Copy() = %d"), aCSDInfo.iCopy);
	INFO_PRINTF2(_L("CSDInfo->PermWriteProtect() = %d"), aCSDInfo.iPermWriteProtect);
	INFO_PRINTF2(_L("CSDInfo->TmpWriteProtect() = %d"), aCSDInfo.iTmpWriteProtect);
	INFO_PRINTF2(_L("CSDInfo->MediaType() = %d"), aCSDInfo.iMediaType);
	}

/**
 * Prints an Extended CSD
 *
 * @param aExtendedCSDInfo	The Extended CSD to print
 *
 * @return					void
 */
void CT_MmcSDDriverData::PrintExtendedCSD(const TExtendedCSDInfo& aExtendedCSDInfo)
	{
	INFO_PRINTF2(_L("ExtendedCSDInfo->SupportedCmdSet() = %u"), aExtendedCSDInfo.iSupportedCmdSet);
	INFO_PRINTF2(_L("ExtendedCSDInfo->SectorCount() = %u"), aExtendedCSDInfo.iSectorCount);
	INFO_PRINTF2(_L("ExtendedCSDInfo->MinPerfWrite8Bit52Mhz() = %u"), aExtendedCSDInfo.iMinPerfWrite8Bit52Mhz);
	INFO_PRINTF2(_L("ExtendedCSDInfo->MinPerfRead8Bit52Mhz() = %u"), aExtendedCSDInfo.iMinPerfRead8Bit52Mhz);
	INFO_PRINTF2(_L("ExtendedCSDInfo->MinPerfWrite8Bit26Mhz_4Bit52Mhz() = %u"), aExtendedCSDInfo.iMinPerfWrite8Bit26Mhz_4Bit52Mhz);
	INFO_PRINTF2(_L("ExtendedCSDInfo->MinPerfRead8Bit26Mhz_4Bit52Mhz() = %u"), aExtendedCSDInfo.iMinPerfRead8Bit26Mhz_4Bit52Mhz);
	INFO_PRINTF2(_L("ExtendedCSDInfo->MinPerfWrite4Bit26Mhz() = %u"), aExtendedCSDInfo.iMinPerfWrite4Bit26Mhz);
	INFO_PRINTF2(_L("ExtendedCSDInfo->MinPerfRead4Bit26Mhz() = %u"), aExtendedCSDInfo.iMinPerfRead4Bit26Mhz);
	INFO_PRINTF2(_L("ExtendedCSDInfo->PowerClass26Mhz360V() = %u"), aExtendedCSDInfo.iPowerClass26Mhz360V);
	INFO_PRINTF2(_L("ExtendedCSDInfo->PowerClass52Mhz360V() = %u"), aExtendedCSDInfo.iPowerClass52Mhz360V);
	INFO_PRINTF2(_L("ExtendedCSDInfo->PowerClass26Mhz195V() = %u"), aExtendedCSDInfo.iPowerClass26Mhz195V);
	INFO_PRINTF2(_L("ExtendedCSDInfo->PowerClass52Mhz195V() = %u"), aExtendedCSDInfo.iPowerClass52Mhz195V);
	INFO_PRINTF2(_L("ExtendedCSDInfo->CardType() = %u"), aExtendedCSDInfo.iCardType);
	INFO_PRINTF2(_L("ExtendedCSDInfo->CSDStructureVer() = %u"), aExtendedCSDInfo.iCSDStructureVer);
	INFO_PRINTF2(_L("ExtendedCSDInfo->ExtendedCSDRev() = %u"), aExtendedCSDInfo.iExtendedCSDRev);
	INFO_PRINTF2(_L("ExtendedCSDInfo->CmdSet() = %u"), aExtendedCSDInfo.iCmdSet);
	INFO_PRINTF2(_L("ExtendedCSDInfo->CmdSetRev() = %u"), aExtendedCSDInfo.iCmdSetRev);
	INFO_PRINTF2(_L("ExtendedCSDInfo->PowerClass() = %u"), aExtendedCSDInfo.iPowerClass);
	INFO_PRINTF2(_L("ExtendedCSDInfo->HighSpeedTiming() = %u"), aExtendedCSDInfo.iHighSpeedTiming);
	}

/**
 * Reads the CSD
 * This function is not contained within a TMMCard wrapper as this is not a class under test
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDDriverData::DoCmdReadCSD()
	{
	INFO_PRINTF1(_L("DoCmdReadCSD called()"));
	TCSDInfo csdInfo;
	TInt err = iMmcSDController->ReadCSD(csdInfo);
	PrintCSD(csdInfo);

	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("DoCmdReadCSD() ReadCSD() Error %d"), err);
		SetError(err);
		}
	}

/**
 * Reads the Extended CSD
 * This function is not contained within a TMMCard wrapper as this is not a class under test
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDDriverData::DoCmdReadExtCSD()
	{
	INFO_PRINTF1(_L("DoCmdReadExtCSD called()"));
	TExtendedCSDInfo extendedCSDInfo;
	TInt err = iMmcSDController->ReadExtCSD(extendedCSDInfo);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("DoCmdReadExtCSD() Error %d"), err);
		SetError(err);
		}
	else
		{
		PrintExtendedCSD(extendedCSDInfo);
		}
	}

/**
 * Process command that will obtain card info from kernel side
 * This function is not contained within a TMMCard wrapper as this is not a class under test
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDDriverData::DoCmdCardInfo(const TDesC& aSection)
	{
	TMMCCardInfo ci;
	INFO_PRINTF1(_L("DoCmdCardInfo()"));
	TInt err = iMmcSDController->CardInfo(ci);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DoCmdCardInfo Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF5(_L("iIsReady(%d) iIsLocked(%d) iRCA(%d) iMediaType(%d)"),
				ci.iIsReady, ci.iIsLocked, ci.iRCA, ci.iMediaType );

		INFO_PRINTF6(_L("iCardSizeInBytes low(%x) high(%x) iMaxReadBlLen(%d) iReadBlLen(%d) iReadBlPartial(%d)"),
				I64LOW(ci.iCardSizeInBytes), I64HIGH(ci.iCardSizeInBytes), ci.iMaxReadBlLen, ci.iReadBlLen, ci.iReadBlPartial );

		INFO_PRINTF6(_L("iWriteBlPartial(%d) iReadBlkMisalign(%d) iWriteBlkMisalign(%d) iReadCurrentInMilliAmps(%d) iWriteCurrentInMilliAmps(%d)"),
				ci.iWriteBlPartial, ci.iReadBlkMisalign, ci.iWriteBlkMisalign, ci.iReadCurrentInMilliAmps, ci.iWriteCurrentInMilliAmps );

		INFO_PRINTF6(_L("iSpecVers(%d) iTAAC(%d) iNSAC(%d) iTransferSpeed(%d) iCommandRegister(%d)"),
				ci.iSpecVers, ci.iTAAC, ci.iNSAC, ci.iTransferSpeed, ci.iCommandRegister );

		INFO_PRINTF3(_L("iHighCapacity(%d) iFlags(%d) "), ci.iHighCapacity, ci.iFlags);

		// check against expected values

		TInt maxreadBlockLen;
		if(GetIntFromConfig(aSection, KMaxReadBlockLen(), maxreadBlockLen))
			{
			if(maxreadBlockLen != ci.iMaxReadBlLen)
				{
				ERR_PRINTF3(_L("max block lengths do not match actual(%d), expected(%d)"), ci.iMaxReadBlLen, maxreadBlockLen);
				SetBlockResult(EFail);
				}
			}

		TInt readCurrentInMilliAmps;
		if(GetIntFromConfig(aSection, KReadCurrentInMilliAmps(), readCurrentInMilliAmps))
			{
			if(readCurrentInMilliAmps!= ci.iReadCurrentInMilliAmps)
				{
				ERR_PRINTF3(_L("ReadCurrentInMilliAmps do not match actual(%d), expected(%d)"),ci.iReadCurrentInMilliAmps, readCurrentInMilliAmps);
				SetBlockResult(EFail);
				}
			}

		TUint transferSpeed;
		if(GetUintFromConfig(aSection, KTransferSpeed(), transferSpeed))
			{
			if(transferSpeed != ci.iTransferSpeed)
				{
				ERR_PRINTF3(_L("transferSpeed do not match actual(%d), expected(%d)"), ci.iTransferSpeed, transferSpeed);
				SetBlockResult(EFail);
				}
			}

		TBool highCapacity;
		if(GetBoolFromConfig(aSection, KHighCapacity(), highCapacity))
			{
			if(highCapacity != ci.iHighCapacity)
				{
				ERR_PRINTF3(_L("highCapacity do not match actual(%d), expected(%d)"), ci.iHighCapacity, highCapacity);
				SetBlockResult(EFail);
				}
			}
		}
	}

/**
 * Process command that will obtain card is present status from kernel side
 * This function is not contained within a TMMCard wrapper as this is not a class under test
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDDriverData::DoCmdCardIsPresent(const TDesC& aSection)
	{
	TBool isPresent;
	INFO_PRINTF1(_L("DoCmdCardIsPresent()"));
	TInt err = iMmcSDController->CardIsPresent(isPresent);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DoCmdCardIsPresent Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DoCmdCardIsPresent(): isPresent = %d"), isPresent);
		
		// check against expected value
		TBool expectedIsPresent;
		if(GetBoolFromConfig(aSection, KExpectedIsPresent(), expectedIsPresent))
			{
			if(expectedIsPresent != isPresent)
				{
				ERR_PRINTF3(_L("isPresent does not match actual(%d), expected(%d)"), isPresent, expectedIsPresent);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("DoCmdCardIsPresent(): No expected value given in INI file"));
			SetBlockResult(EFail);			
			}
		}
	}

/**
 * Process command that will obtain card is ready status from kernel side
 * This function is not contained within a TMMCard wrapper as this is not a class under test
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDDriverData::DoCmdCardIsReady(const TDesC& aSection)
	{
	TBool isReady;
	INFO_PRINTF1(_L("DoCmdCardIsReady()"));
	TInt err = iMmcSDController->CardIsReady(isReady);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DoCmdCardIsReady Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DoCmdCardIsReady(): isReady = %d"), isReady);
		
		// check against expected value
		TBool expectedIsReady;
		if(GetBoolFromConfig(aSection, KExpectedIsReady(), expectedIsReady))
			{
			if(expectedIsReady != isReady)
				{
				ERR_PRINTF3(_L("isReady does not match actual(%d), expected(%d)"), isReady, expectedIsReady);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("DoCmdCardIsReady(): No expected value given in INI file"));
			SetBlockResult(EFail);			
			}
		}
	}

/**
 * Process command that will obtain card is locked status from kernel side
 * This function is not contained within a TMMCard wrapper as this is not a class under test
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 */
void CT_MmcSDDriverData::DoCmdCardIsLocked(const TDesC& aSection)
	{
	TBool isLocked;
	INFO_PRINTF1(_L("DoCmdCardIsLocked()"));
	TInt err = iMmcSDController->CardIsLocked(isLocked);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("DoCmdCardIsLocked Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF2(_L("DoCmdCardIsLocked(): isLocked = %d"), isLocked);
		
		// check against expected value
		TBool expectedIsLocked;
		if(GetBoolFromConfig(aSection, KExpectedIsLocked(), expectedIsLocked))
			{
			if(expectedIsLocked != isLocked)
				{
				ERR_PRINTF3(_L("isLocked does not match actual(%d), expected(%d)"), isLocked, expectedIsLocked);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("DoCmdCardIsLocked(): No expected value given in INI file"));
			SetBlockResult(EFail);			
			}
		}
	}

/**
 * Called on completion of an asynchronous command
 *
 * @param aActive			Active Object that RunL has been called on
 * @param aIndex			Index of the active object
 *
 * @return					void
 */
void CT_MmcSDDriverData::RunL(CActive* aActive, TInt aIndex)
 	{
	TInt	err = aActive->iStatus.Int();
 	INFO_PRINTF2(_L("CT_MmcSDDriverData::RunL() called. err (%d)"), err);

 	// retrieve the active callback wrap that wraps this item
 	CActiveCallbackWrap* activeCallbackWrap=NULL;

	for(int i=0;i<iActiveCallbacks.Count();i++)
		{
		if( &(iActiveCallbacks[i]->ActiveCallback()) == aActive)
			{
			activeCallbackWrap = iActiveCallbacks[i];
			break;
			}
		}

	if(activeCallbackWrap)
		{
		switch(activeCallbackWrap->FunctionId())
			{
			case RMMCSDTestControllerInterface::EReadSector:
				{
				INFO_PRINTF1(_L("CT_MmcSDDriverData::RunL() Post processing CActiveCallbackWrap RMMCSDTestControllerInterface::EReadSector:"));
				if (err == KErrNone)
					{
					HBufC8* readData = (HBufC8*) activeCallbackWrap->Data();
					// dump the read buffer here
				 	TBuf16<KSectorSizeInBytes> printBuf;
				 	printBuf.Copy(activeCallbackWrap->DataPtr());
					INFO_PRINTF3(_L("ReadSector data (length=%d):\n%S"), printBuf.Length(), &printBuf);

					HBufC8* expectedData = (HBufC8*) activeCallbackWrap->ExpectedData();
					if (expectedData)
						{
						if (*readData != *expectedData)
							{
							TPtr8	expectedDataPtr(expectedData->Des());
						 	TBuf16<KSectorSizeInBytes> printExpectedBuf;
						 	printExpectedBuf.Copy(expectedDataPtr);
							ERR_PRINTF3(_L("ReadSector data not as expected (length=%d):\n%S"), printExpectedBuf.Length(), &printExpectedBuf);
							SetBlockResult(EFail);
							}
						}
					}
				}
				break;

			case RMMCSDTestControllerInterface::EWriteSector:
				INFO_PRINTF1(_L("CT_MmcSDDriverData::RunL() Post processing CActiveCallbackWrap RMMCSDTestControllerInterface::EWriteSector:"));
				break;
				
			case RMMCSDTestControllerInterface::ESocketPowerUp:
				INFO_PRINTF1(_L("CT_MmcSDDriverData::RunL() Post processing CActiveCallbackWrap RMMCSDTestControllerInterface::ESocketPowerUp:"));
				break;

			default:
				break;
			}

		if (err != activeCallbackWrap->ExpectedAsyncError())
			{
			ERR_PRINTF3(_L("Error (%d) != Expected error (%d)"), err, activeCallbackWrap->ExpectedAsyncError());
			SetAsyncError(aIndex, err);
			}

		DecOutstanding();
		DeleteActiveCallback((CActiveCallback*)aActive);
		}
	else
		{
		ERR_PRINTF1(_L("An unchecked active object completed"));
		SetBlockResult(EFail);
		}
 	}

/**
 * Request to cancel the asynchronous command
 *
 * @param aActive			Active Object that DoCancel has been called on
 *
 * @return					void
 */
void CT_MmcSDDriverData::DoCancel(CActive* aActive)
 	{
	TInt	err = aActive->iStatus.Int();
 	INFO_PRINTF2(_L("CT_MmcSDDriverData::DoCancelL() called. err (%d)"), err);

 	// retrieve the active callback wrap that wraps this item
 	CActiveCallbackWrap* activeCallbackWrap=NULL;

	for(int i=0;i<iActiveCallbacks.Count();i++)
		{
		if( &(iActiveCallbacks[i]->ActiveCallback()) == aActive)
			{
			activeCallbackWrap = iActiveCallbacks[i];
			break;
			}
		}

	if(activeCallbackWrap)
		{
		switch(activeCallbackWrap->FunctionId())
		{
			case RMMCSDTestControllerInterface::EReadSector:
				INFO_PRINTF1(_L("CT_MmcSDDriverData::DoCancelL() Post processing CActiveCallbackWrap RMMCSDTestControllerInterface::EReadSector:"));
				break;

			case RMMCSDTestControllerInterface::EWriteSector:
				INFO_PRINTF1(_L("CT_MmcSDDriverData::DoCancelL() Post processing CActiveCallbackWrap RMMCSDTestControllerInterface::EWriteSector:"));
				break;
				
			case RMMCSDTestControllerInterface::ESocketPowerUp:
				INFO_PRINTF1(_L("CT_MmcSDDriverData::DoCancelL() Post processing CActiveCallbackWrap RMMCSDTestControllerInterface::ESocketPowerUp:"));
				break;

			default:
				break;
		}

		if (err != activeCallbackWrap->ExpectedAsyncError())
			{
			ERR_PRINTF3(_L("Error (%d) != Expected error (%d)"), err, activeCallbackWrap->ExpectedAsyncError());
			SetBlockResult(EFail);
			}

		DecOutstanding();
		DeleteActiveCallback((CActiveCallback*)aActive);
		}
	else
		{
		ERR_PRINTF1(_L("An unchecked active object completed"));
		SetBlockResult(EFail);
		}
 	}

/**
 * Gets the expected async error code
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					The expected async error code
 */
TInt CT_MmcSDDriverData::GetExpectedAsyncError(const TDesC& aSection)
 	{
	TInt expectedAsyncError = KErrNone;
	GetIntFromConfig(aSection, KExpectedAsyncError(), expectedAsyncError);
	return expectedAsyncError;
 	}

