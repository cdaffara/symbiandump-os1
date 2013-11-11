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


#ifdef __KERNEL_MODE__

#include "MmcTestUtils.h"

/**
 * Convert kernel Card object to user side object
 *
 * @param const TMMCard& aCard - The card object
 *
 * @return N/A
 *
 * @leave N/A
 */
void TMMCCardInfo::SetCardInfo(const TMMCard& aCard)
	{
    iIsReady = aCard.IsPresent();
    iIsLocked = aCard.IsLocked();
	TCID* cid=(TCID*)&(aCard.CID());
	TInt i;
	for (i=0;i<16;i++)
		{
		iCID[i] = cid->At(i);
		}
	const TCSD& csd = aCard.CSD();
	for (i=0;i<16;i++)
		{
		iCSD[i] = csd.At(i);
		}
    iRCA = TUint16(aCard.RCA());
    iMediaType = (TMmcMediaType)aCard.MediaType();
    iCardSizeInBytes = aCard.DeviceSize64();
    iMaxReadBlLen = aCard.MaxReadBlLen();
    iReadBlLen = csd.ReadBlockLength();
    iWriteBlLen = csd.WriteBlockLength();
    iReadBlPartial = csd.ReadBlPartial();
    iWriteBlPartial = csd.WriteBlPartial();
    iReadBlkMisalign = csd.ReadBlkMisalign();
    iWriteBlkMisalign = csd.WriteBlkMisalign();
    iReadCurrentInMilliAmps = csd.MaxReadCurrentInMilliamps();
    iWriteCurrentInMilliAmps = csd.MaxWriteCurrentInMilliamps();
    iSpecVers = csd.SpecVers();
    iTAAC = csd.TAAC();
    iNSAC = csd.NSAC();
    iTransferSpeed = csd.TranSpeed();
    iCommandRegister = csd.CCC();
    iHighCapacity = aCard.IsHighCapacity();
 	iFlags = aCard.iFlags;
	}

/**
 * Convert kernel Machine Info object to user side object
 *
 * @param const TMMCMachineInfo& aMachineInfo - The Machine Info object
 *
 * @return N/A
 *
 * @leave N/A
 */
TMachineInfo::TMachineInfo(const TMMCMachineInfo& aMachineInfo)
	{
	iTotalSockets = aMachineInfo.iTotalSockets;
	iTotalMediaChanges = aMachineInfo.iTotalMediaChanges;
	iTotalPrimarySupplies = aMachineInfo.iTotalPrimarySupplies;
	iSPIMode = aMachineInfo.iSPIMode & 0x1;
	iBaseBusNumber = aMachineInfo.iBaseBusNumber;

	if( aMachineInfo.iFlags & TMMCMachineInfo::ESupportsSPIMode )
		{
		iSupportsSPIMode = ETrue;
		}
	else
		{
		iSupportsSPIMode = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::ESupportsDoubleBuffering )
		{
		iSupportsDoubleBuffering = ETrue;
		}
	else
		{
		iSupportsDoubleBuffering = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::ESupportsR7 )
		{
		iSupportsR7 = ETrue;
		}
	else
		{
		iSupportsR7 = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EDma8BitAddressing )
		{
		iDma8BitAddressing = ETrue;
		}
	else
		{
		iDma8BitAddressing = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EDma16BitAddressing )
		{
		iDma16BitAddressing = ETrue;
		}
	else
		{
		iDma16BitAddressing = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EDma32BitAddressing )
		{
		iDma32BitAddressing = ETrue;
		}
	else
		{
		iDma32BitAddressing = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EDma64BitAddressing )
		{
		iDma64BitAddressing = ETrue;
		}
	else
		{
		iDma64BitAddressing = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::ESupportsDMA )
		{
		iSupportsDMA = ETrue;
		}
	else
		{
		iSupportsDMA = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EMaxTransferLength_256K )
		{
		iMaxTransferLength_256K = ETrue;
		}
	else
		{
		iMaxTransferLength_256K = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EMaxTransferLength_512K )
		{
		iMaxTransferLength_512K = ETrue;
		}
	else
		{
		iMaxTransferLength_512K = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EMaxTransferLength_1M )
		{
		iMaxTransferLength_1M = ETrue;
		}
	else
		{
		iMaxTransferLength_1M = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EMaxTransferLength_2M )
		{
		iMaxTransferLength_2M = ETrue;
		}
	else
		{
		iMaxTransferLength_2M = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EMaxTransferLength_4M )
		{
		iMaxTransferLength_4M = ETrue;
		}
	else
		{
		iMaxTransferLength_4M = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EMaxTransferLength_8M )
		{
		iMaxTransferLength_8M = ETrue;
		}
	else
		{
		iMaxTransferLength_8M = EFalse;
		}
	if( aMachineInfo.iFlags & TMMCMachineInfo::EMaxTransferLength_16M )
		{
		iMaxTransferLength_16M = ETrue;
		}
	else
		{
		iMaxTransferLength_16M = EFalse;
		}
	}

/**
 * Convert kernel CSD object to user side object
 *
 * @param const TCSD& aCSD - The CSD object
 *
 * @return N/A
 *
 * @leave N/A
 */
TCSDInfo::TCSDInfo(const TCSD& aCSD)
	{
	iCSDStructure = aCSD.CSDStructure();
	iSpecVers = aCSD.SpecVers();
	iReserved120 = aCSD.Reserved120();
	iTAAC = aCSD.TAAC();
	iNSAC = aCSD.NSAC();
	iTranSpeed = aCSD.TranSpeed();
	iCCC = aCSD.CCC();
	iReadBlLen = aCSD.ReadBlLen();
	iReadBlPartial = aCSD.ReadBlPartial();
	iWriteBlkMisalign = aCSD.WriteBlkMisalign();
	iReadBlkMisalign = aCSD.ReadBlkMisalign();
	iDSRImp = aCSD.DSRImp();
	iReserved74 = aCSD.Reserved74();
	iCSize = aCSD.CSize();
	iVDDRCurrMin = aCSD.VDDRCurrMin();
	iVDDRCurrMax = aCSD.VDDRCurrMax();
	iVDDWCurrMin = aCSD.VDDWCurrMin();
	iVDDWCurrMax = aCSD.VDDWCurrMax();
	iCSizeMult = aCSD.CSizeMult();
	iEraseGrpSize = aCSD.EraseGrpSize();
	iEraseGrpMult = aCSD.EraseGrpMult();
	iWPGrpSize = aCSD.WPGrpSize();
	iWPGrpEnable = aCSD.WPGrpEnable();
	iDefaultECC = aCSD.DefaultECC();
	iR2WFactor = aCSD.R2WFactor();
	iWriteBlLen = aCSD.WriteBlLen();
	iWriteBlPartial = aCSD.WriteBlPartial();
	iReserved16 = aCSD.Reserved16();
	iFileFormatGrp = aCSD.FileFormatGrp();
	iCopy = aCSD.Copy();
	iPermWriteProtect = aCSD.PermWriteProtect();
	iTmpWriteProtect = aCSD.TmpWriteProtect();
	iFileFormat = aCSD.FileFormat();
	iECC = aCSD.ECC();
	iCRC = aCSD.CRC();
	iDeviceSize = aCSD.DeviceSize();
	iReadBlockLength = aCSD.ReadBlockLength();
	iWriteBlockLength = aCSD.WriteBlockLength();
	iEraseSectorSize = aCSD.EraseSectorSize();
	iEraseGroupSize = aCSD.EraseGroupSize();
	iMinReadCurrentInMilliamps = aCSD.MinReadCurrentInMilliamps();
	iMinWriteCurrentInMilliamps = aCSD.MinWriteCurrentInMilliamps();
	iMaxReadCurrentInMilliamps = aCSD.MaxReadCurrentInMilliamps();
	iMaxWriteCurrentInMilliamps = aCSD.MaxWriteCurrentInMilliamps();
	iMaxTranSpeedInKilohertz = aCSD.MaxTranSpeedInKilohertz();

	// Convert Kernel side TMMCMediaTypeEnum to User side TMMCMediaTypeEnumInfo
	switch (aCSD.MediaType())
		{
		case EMultiMediaROM:
			iMediaType = EMultiMediaROM;
			break;
			
		case EMultiMediaFlash:
			iMediaType = EMultiMediaFlash;
			break;
		
		case EMultiMediaIO:
			iMediaType = EMultiMediaIO;
			break;
		
		case EMultiMediaOther:
			iMediaType = EMultiMediaOther;
			break;
		
		default:
			iMediaType = EMultiMediaNotSupported;
			break;
		}
	}

/**
 * Convert kernel Extended CSD object to user side object
 *
 * @param const TExtendedCSD& aExtendedCSD - The Extended CSD object
 *
 * @return N/A
 *
 * @leave N/A
 */
TExtendedCSDInfo::TExtendedCSDInfo(const TExtendedCSD& aExtendedCSD)
	{
	iSupportedCmdSet = aExtendedCSD.SupportedCmdSet();
	iSectorCount = aExtendedCSD.SectorCount();
	iMinPerfWrite8Bit52Mhz = aExtendedCSD.MinPerfWrite8Bit52Mhz();
	iMinPerfRead8Bit52Mhz = aExtendedCSD.MinPerfRead8Bit52Mhz();
	iMinPerfWrite8Bit26Mhz_4Bit52Mhz = aExtendedCSD.MinPerfWrite8Bit26Mhz_4Bit52Mhz();
	iMinPerfRead8Bit26Mhz_4Bit52Mhz = aExtendedCSD.MinPerfRead8Bit26Mhz_4Bit52Mhz();
	iMinPerfWrite4Bit26Mhz = aExtendedCSD.MinPerfWrite4Bit26Mhz();
	iMinPerfRead4Bit26Mhz = aExtendedCSD.MinPerfRead4Bit26Mhz();
	iPowerClass26Mhz360V = aExtendedCSD.PowerClass26Mhz360V();
	iPowerClass52Mhz360V = aExtendedCSD.PowerClass52Mhz360V();
	iPowerClass26Mhz195V = aExtendedCSD.PowerClass26Mhz195V();
	iPowerClass52Mhz195V = aExtendedCSD.PowerClass52Mhz195V();
	iCardType = aExtendedCSD.CardType();
	iCSDStructureVer = aExtendedCSD.CSDStructureVer();
	iExtendedCSDRev = aExtendedCSD.ExtendedCSDRev();
	iCmdSet = aExtendedCSD.CmdSet();
	iCmdSetRev = aExtendedCSD.CmdSetRev();
	iPowerClass = aExtendedCSD.PowerClass();
	iHighSpeedTiming = aExtendedCSD.HighSpeedTiming();
	}

/**
 * Create a formatted password from CID to write to Password Store
 *
 * @param TDes8 &aFormattedPassword - The buffer to hold the formatted password
 * @param const TCID& aCID - The CID
 *
 * @return N/A
 *
 * @leave N/A
 */
TBool TStackPasswordStoreData::CreateFormattedPassword(TDes8 &aFormattedPassword, const TCID& aCID)
	{
	TBool success = EFalse;
	if (aFormattedPassword.MaxLength() >= KMMCCIDLength)
		{
		aFormattedPassword.Zero();
		aFormattedPassword.SetLength(KMMCCIDLength);
		aCID.Copy(&aFormattedPassword[0]);
		TUint8 lenBuf[sizeof(TInt32)];		// TInt32, big endian
		TMMC::BigEndian4Bytes(lenBuf, TInt32(iPassword.Length()));
		aFormattedPassword.Append(&lenBuf[0], sizeof(TInt32));
		aFormattedPassword.Append(iPassword);
		success = ETrue;
		}
	return success;
	}

/**
 * Extract the password string from a formatted password
 *
 * @param TDes8 &aFormattedPassword - The formatted password
 *
 * @return N/A
 *
 * @leave N/A
 */
void TStackPasswordStoreData::SetFormattedPassword(const TDesC8 &aFormattedPassword)
	{
	iPassword.Copy(&aFormattedPassword[KPasswordStoreCIDLength], aFormattedPassword.Length()-KPasswordStoreCIDLength);
	}

#else

#error compiling kernel side only src file on user side

#endif
