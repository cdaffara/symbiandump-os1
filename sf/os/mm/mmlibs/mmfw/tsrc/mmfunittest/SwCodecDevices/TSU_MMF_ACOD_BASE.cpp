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
// [ separated to separate module for understandability
// 12k loc not a good idea for maintenance ]
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_ACOD.h"
#include "TSU_MMF_devicesuite.h"

#include <f32file.h>
#include <s32file.h>
#include <mmfhwdeviceimplementationUids.hrh>

#ifdef __WINS__
//wins directories
_LIT(KSourceFilesDir, "c:\\mm\\mmf\\testfiles\\acod\\");
_LIT(KComparisonFilesDir, "c:\\mm\\mmf\\ref\\acod\\");
#else
/*
arm4 directories
All source and comparison files need to be copied manually onto the flash card.
This code assumes the flash card is represented by drive E: on the target board.
If this is not the case, the KSourceFilesDir and KComparisonFilesDir strings
need to be changed accordingly.
*/
_LIT(KSourceFilesDir, "e:\\");
_LIT(KComparisonFilesDir, "e:\\");
#endif

//8KHz source files
_LIT(KPCM16SourceFile,"PCM16Source.raw");
_LIT(KPCM16BSourceFile,"PCM16BSource.raw");
_LIT(KPCMU16SourceFile,"PCMU16Source.raw");
_LIT(KPCMU16BSourceFile,"PCMU16BSource.raw");
_LIT(KStereoPCM16SourceFile,"StereoPCM16Source.raw");
_LIT(KPCM8SourceFile,"PCM8Source.raw");
_LIT(KPCMU8SourceFile,"PCMU8Source.raw");
_LIT(KAlawSourceFile,"AlawSource.raw");
_LIT(KMulawSourceFile,"MulawSource.raw");
_LIT(KAdpcmSourceFile,"AdpcmSource.raw");
_LIT(KStereoAdpcmSourceFile,"StereoAdpcmSource.raw");
_LIT(KGSM610SourceFile,"GSM610Source.raw");

//comparison files
_LIT(KAlawToPCM16ComparisonFile,"AlawToPCM16Comparison.raw");
_LIT(KPCM16ToAlawComparisonFile,"PCM16ToAlawComparison.raw");
_LIT(KPCM8ToPCM16ComparisonFile,"PCM8ToPCM16Comparison.raw");
_LIT(KPCM16ToPCM8ComparisonFile,"PCM16ToPCM8Comparison.raw");
_LIT(KPCM16ToPCM16BComparisonFile,"PCM16ToPCM16BComparison.raw");
_LIT(KPCM16BToPCM16ComparisonFile,"PCM16BToPCM16Comparison.raw");
_LIT(KPCMU16ToPCMU16BComparisonFile,"PCMU16ToPCMU16BComparison.raw");
_LIT(KPCMU16BToPCMU16ComparisonFile,"PCMU16BToPCMU16Comparison.raw");
_LIT(KPCM16ToPCMU16BComparisonFile,"PCM16ToPCMU16BComparison.raw");
_LIT(KPCMU16BToPCM16ComparisonFile,"PCMU16BToPCM16Comparison.raw");
_LIT(KPCM16ToPCMU16ComparisonFile,"PCM16ToPCMU16Comparison.raw");
_LIT(KPCMU16ToPCM16ComparisonFile,"PCMU16ToPCM16Comparison.raw");
_LIT(KPCM16ToPCMU8ComparisonFile,"PCM16ToPCMU8Comparison.raw");
_LIT(KPCMU8ToPCM16ComparisonFile,"PCMU8ToPCM16Comparison.raw");
_LIT(KAdpcmToPCM16ComparisonFile,"AdpcmToPCM16Comparison.raw");
_LIT(KPCM16ToAdpcmComparisonFile,"PCM16ToAdpcmComparison.raw");
_LIT(KMulawToPCM16ComparisonFile,"MulawToPCM16Comparison.raw");
_LIT(KPCM16ToMulawComparisonFile,"PCM16ToMulawComparison.raw");
_LIT(KStereoAdpcmToPCM16ComparisonFile,"StereoAdpcmToPCM16Comparison.raw");
_LIT(KPCM16ToStereoAdpcmComparisonFile,"PCM16ToStereoAdpcmComparison.raw");
_LIT(KGSM610ToPCM16ComparisonFile,"GSM610ToPCM16Comparison.raw");
_LIT(KPCM16ToGSM610ComparisonFile,"PCM16ToGSM610Comparison.raw");

_LIT(KPreferredSupplierSourceFile,"AAAASource.raw");
_LIT(KPreferredSupplierComparisonFile,"AAAABBBBPrefSuppComparison.raw");
_LIT(KPreferredSupplier,"Preferred Supplier Test Codec 1");


const TUint32 KMMFFourCCCodeAAAA = 0x41414141; //"AAAA"
const TUint32 KMMFFourCCCodeBBBB = 0x42424242; //"BBBB"

CTestStep_MMF_ACOD::CTestStep_MMF_ACOD()
	{
	iExpectedLeaveErrorCode = KErrNone;
	iSingleIteration = EFalse;
	}

CTestStep_MMF_ACOD::~CTestStep_MMF_ACOD()
	{
	}

/** @xxxx
 * This function gets the correct comparison filename for the codec specified.
 *
 * @param	"TInt aCodecUID"
 *			Uid of codec.
 *			"TDes& aFilename"
 *			Descriptor to contain the comparison filename string.
 */
void CTestStep_MMF_ACOD::GetComparisonFilenameFromCodecUidL(TInt aCodecUID, TDes& aFilename)
	{

	switch(aCodecUID)
		{
		case KMmfUidHwDeviceAlawToPCM16: //KMmfUidHwDeviceAlawToPCM16
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KAlawToPCM16ComparisonFile);
			break;
		case KMmfUidHwDevicePCM16ToALaw: //KMmfUidHwDevicePCM16ToALaw
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToAlawComparisonFile);
			break;
		case KMmfUidHwDevicePCM8ToPCM16: //KMmfUidHwDevicePCM8ToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM8ToPCM16ComparisonFile);
			break;
		case KMmfUidHwDeviceS16ToPCMS8: //KMmfUidHwDeviceS16ToPCMS8:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCM8ComparisonFile);
			break;
		//case KMMFUidCodecPCM16SwapEndian:
		case KMMFUidHwDevicePCM16toPCMU16B:  //KMmfUidHwDevicePCM16toPCM16B
		case KMMFUidHwDevicePCM16BtoPCM16:   //KMMFUidCodecPCM16BtoPCM16
		case KMMFUidHwDevicePCMU16toPCMU16B: //KMMFUidCodecPCMU16toPCMU16B
		case KMMFUidHwDevicePCMU16BtoPCMU16: //KMMFUidCodecPCMU16BtoPCMU16
			//This file is used for all swap endian tests other than instantiation.
			//There is no need to test ProcessL for every swap endian combination.
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCM16BComparisonFile);
			break;

		//xxx GK
		//case KMMFUidHwDevicePCM16toPCMU16B ://KMMFUidHwDevicePCM16toPCMU16B:
		//	aFilename.Copy(KComparisonFilesDir);
		//	aFilename.Append(KPCM16ToPCMU16BComparisonFile);
		//	break;

		case KMmfUidHwDeviceU16BEToPCMS16://KMmfUidHwDeviceU16BEToPCMS16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU16BToPCM16ComparisonFile);
			break;

		case KMmfUidHwDeviceS16ToPCMU16://KMmfUidHwDeviceS16ToPCMU16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCMU16ComparisonFile);
			break;

		case KMmfUidHwDeviceU16ToPCMS16://KMmfUidHwDeviceU16ToPCMS16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU16ToPCM16ComparisonFile);
			break;

		case KMmfUidHwDeviceS16ToPCMU8://KMmfUidHwDeviceS16ToPCMU8:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCMU8ComparisonFile);
			break;

		case KMmfUidHwDevicePCMU8ToPCM16: //KMmfUidHwDevicePCMU8ToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU8ToPCM16ComparisonFile);
			break;

		case KMmfUidHwDeviceIMAADPCMToPCM16: //KMmfUidHwDeviceIMAADPCMToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KAdpcmToPCM16ComparisonFile);
			break;

		case KMmfUidHwDevicePCM16ToIMAADPCM: //KMmfUidHwDevicePCM16ToIMAADPCM:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToAdpcmComparisonFile);
			break;

		case KMmfUidHwDeviceMulawToPCM16: //KMmfUidHwDeviceMulawToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KMulawToPCM16ComparisonFile);
			break;

		case KMmfUidHwDevicePCM16ToMuLaw: //KMmfUidHwDevicePCM16ToMuLaw:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToMulawComparisonFile);
			break;

		case KMmfUidHwDeviceGSM610ToPCM16: //KMmfUidHwDeviceGSM610ToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KGSM610ToPCM16ComparisonFile);
			break;

		case KMmfUidHwDevicePCM16ToGSM610://KMmfUidHwDevicePCM16ToGSM610:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToGSM610ComparisonFile);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}

/** @xxxx
 * This function gets the correct comparison filename for the fourCC codes specified.
 *
 * @param	"const TFourCC& aSrcFourCC"
 *			Source fourCC code.
 *			"const TFourCC& aDstFourCC"
 *			Destination fourCC code.
 *			"TDes& aFilename"
 *			Descriptor to contain the comparison filename string.
 */
void CTestStep_MMF_ACOD::GetComparisonFilenameFromFourCCL(const TFourCC& aSrcFourCC, const TFourCC& aDstFourCC, TDes& aFilename)
	{
	if (aSrcFourCC == KMMFFourCCCodePCM16)
		{
		if (aDstFourCC == KMMFFourCCCodeALAW)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToAlawComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodePCM16B)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCM16BComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodePCM8)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCM8ComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodePCMU16B)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCMU16BComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodePCMU16)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCMU16ComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodePCMU8)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCMU8ComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodeIMAD)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToAdpcmComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodeMuLAW)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToMulawComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodeIMAS)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToStereoAdpcmComparisonFile);
			}
		else if (aDstFourCC == KMMFFourCCCodeGSM610)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToGSM610ComparisonFile);
			}
		}
	else if (aDstFourCC == KMMFFourCCCodePCM16)
		{
		if (aSrcFourCC == KMMFFourCCCodeALAW)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KAlawToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodePCM8)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM8ToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodePCM16B)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16BToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodePCMU16B)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU16BToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodePCMU16)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU16ToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodePCMU8)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU8ToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodeIMAD)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KAdpcmToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodeMuLAW)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KMulawToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodeIMAS)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KStereoAdpcmToPCM16ComparisonFile);
			}
		if (aSrcFourCC == KMMFFourCCCodeGSM610)
			{
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KGSM610ToPCM16ComparisonFile);
			}
		}
	else if	((aSrcFourCC == KMMFFourCCCodePCMU16) &&
			(aDstFourCC == KMMFFourCCCodePCMU16B))
		{
		aFilename.Copy(KComparisonFilesDir);
		aFilename.Append(KPCMU16ToPCMU16BComparisonFile);
		}
	else if	((aSrcFourCC == KMMFFourCCCodePCMU16B) &&
			(aDstFourCC == KMMFFourCCCodePCMU16))
		{
		aFilename.Copy(KComparisonFilesDir);
		aFilename.Append(KPCMU16BToPCMU16ComparisonFile);
		}
	else
		User::Leave(KErrNotSupported);
	}

/** @xxxx
 * This function gets the correct source filename for the codec specified.
 *
 * @param	"TInt aCodecUID"
 *			Uid of codec.
 *			"TDes& aFilename"
 *			Descriptor to contain the source filename string.
 */
void CTestStep_MMF_ACOD::GetSourceFilenameFromCodecUidL(TInt aCodecUID, TDes& aFilename)
	{
	switch(aCodecUID)
		{
		case KMmfUidHwDeviceAlawToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KAlawSourceFile);
			break;
		case KMmfUidHwDevicePCM16ToALaw:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidHwDevicePCM8ToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM8SourceFile);
			break;
		case KMmfUidHwDeviceS16ToPCMS8:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		//case KMMFUidCodecPCM16SwapEndian:
		case KMmfUidHwDevicePCM16toPCM16B:
		case KMMFUidHwDevicePCM16BtoPCM16:
        case KMMFUidHwDevicePCMU16toPCMU16B:
		case KMMFUidHwDevicePCMU16BtoPCMU16:
			//this file is used for all swap endian tests other than instantiation.
			//there is no need to test ProcessL for every swap endian combination.
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMMFUidHwDevicePCM16toPCMU16B:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidHwDeviceU16BEToPCMS16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCMU16BSourceFile);
			break;
		case KMmfUidHwDeviceS16ToPCMU16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidHwDeviceU16ToPCMS16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCMU16SourceFile);
			break;
		case KMmfUidHwDeviceS16ToPCMU8:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidHwDevicePCMU8ToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCMU8SourceFile);
			break;
		case KMmfUidHwDeviceIMAADPCMToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KAdpcmSourceFile);
			break;
		case KMmfUidHwDevicePCM16ToIMAADPCM:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidHwDeviceMulawToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KMulawSourceFile);
			break;
		case KMmfUidHwDevicePCM16ToMuLaw:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidHwDeviceGSM610ToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KGSM610SourceFile);
			break;
		case KMmfUidHwDevicePCM16ToGSM610:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;

		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}

/** @xxxx
 * This function gets the correct source filename for the fourCC codes specified.
 *
 * @param	"const TFourCC& aSrcFourCC"
 *			Source fourCC code.
 *			"const TFourCC& aDstFourCC"
 *			Destination fourCC code.
 *			"TDes& aFilename"
 *			Descriptor to contain the source filename string.
 */
void CTestStep_MMF_ACOD::GetSourceFilenameFromFourCCL(const TFourCC& aSrcFourCC, const TFourCC& aDstFourCC, TDes& aFilename)
	{
	aFilename.Copy(KSourceFilesDir);
	if (aSrcFourCC == KMMFFourCCCodePCM16)
		{
		if (aDstFourCC == KMMFFourCCCodeIMAS)
			aFilename.Append(KStereoPCM16SourceFile);
		else
			aFilename.Append(KPCM16SourceFile);
		}
	else if (aSrcFourCC == KMMFFourCCCodeALAW)
			aFilename.Append(KAlawSourceFile);
	else if (aSrcFourCC == KMMFFourCCCodePCM8)
			aFilename.Append(KPCM8SourceFile);
	else if (aSrcFourCC == KMMFFourCCCodePCM16B)
			aFilename.Append(KPCM16BSourceFile);
	else if (aSrcFourCC == KMMFFourCCCodePCMU16B)
			aFilename.Append(KPCMU16BSourceFile);
	else if (aSrcFourCC == KMMFFourCCCodePCMU16)
			aFilename.Append(KPCMU16SourceFile);
	else if (aSrcFourCC == KMMFFourCCCodePCMU8)
			aFilename.Append(KPCMU8SourceFile);
	else if (aSrcFourCC == KMMFFourCCCodeIMAD)
			aFilename.Append(KAdpcmSourceFile);
	else if (aSrcFourCC == KMMFFourCCCodeMuLAW)
			aFilename.Append(KMulawSourceFile);
	else if (aSrcFourCC == KMMFFourCCCodeIMAS)
			aFilename.Append(KStereoAdpcmSourceFile);
	else if (aSrcFourCC == KMMFFourCCCodeGSM610)
			aFilename.Append(KGSM610SourceFile);
	else
		User::Leave(KErrNotSupported);
	}

/** @xxxx
 * This function checks the data in iOutputDataBuffer against data in a comparison
 * file.
 *
 * @param	"const TDesC& aFilename"
 *			Full path of the comparison file to compare against.
 *			"TUint aPosition"
 *			File position from which to read the comparison data.
 * @return	"TBool"
 *			ETrue if the output is the same as the comparison data
 *			EFalse if the output is different to the comparison data
*/
TBool CTestStep_MMF_ACOD::CheckOutputAgainstComparisonFileL(const TDesC& aFilename, TUint aPosition)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile file;
	User::LeaveIfError(file.Open(fs, aFilename, EFileRead|EFileShareAny));
	CleanupClosePushL(file);

	RFileReadStream fileReadStream(file, aPosition);
	CleanupClosePushL(fileReadStream);

	//read data from comparison file into descriptor
	HBufC8* compBuffer = HBufC8::NewL(iOutputDataBuffer->Length());
	CleanupStack::PushL(compBuffer);
	TPtr8 compBufferPtr = compBuffer->Des();
	TRAPD(err, fileReadStream.ReadL(compBufferPtr, iOutputDataBuffer->Length()));
	if ((err != KErrNone) && (err != KErrEof)) User::LeaveIfError(err); //EOF not an error ?

	//compare destination buffer with comparison descriptor
	TInt result = iOutputDataBuffer->Compare(*compBuffer);

	if (result != 0)
		INFO_PRINTF3(_L(">> iOutputDataBuffer size is %d, Comparison buffer size is %d"), iOutputDataBuffer->Length(), compBuffer->Length());

	CleanupStack::PopAndDestroy(4); //compBuffer, fileReadStream, file, fs

	if (result == 0)
		return ETrue;
	else
		return EFalse;
	}

/** @xxxx
 * This function checks the number of source bytes processed against
 * iExpectedSrcBytesProcessed.
 *
 * @param	"TUint aCount"
 *			Index in the iExpectedSrcBytesProcessed array to check the value against.
 * @return	"TBool"
 *			ETrue if the source bytes processed is as expected.
 *			EFalse if the source bytes processed is not as expected.
 */
TBool CTestStep_MMF_ACOD::CheckSrcBytesProcessedL(TUint aCount)
	{
	if (iCodecProcessResult.iSrcBytesProcessed == iExpectedSrcBytesProcessed[aCount])
		return ETrue;
	else if (STATIC_CAST(TUint, iFileDataLeft) < iExpectedSrcBytesProcessed[aCount])
		{
		INFO_PRINTF4(_L(">> TCodecProcessResult::iSrcBytesProcessed is %d, Not enough source data available in file. iFileDataLeft for LOOP %d is %d"), iCodecProcessResult.iSrcBytesProcessed, aCount, iFileDataLeft);
		if (iCodecProcessResult.iSrcBytesProcessed == STATIC_CAST(TUint, iFileDataLeft))
			return ETrue;
		else
			return EFalse;
		}
	else
		{
		INFO_PRINTF4(_L(">> TCodecProcessResult::iSrcBytesProcessed is %d, iExpectedSrcBytesProcessed for LOOP %d is %d"), iCodecProcessResult.iSrcBytesProcessed, aCount, iExpectedSrcBytesProcessed[aCount]);
		return EFalse;
		}
	}

/** @xxxx
 * This function checks the number of destination bytes added against
 * iExpectedDstBytesAdded.
 *
 * @param	"TUint aCount"
 *			Index of the iExpectedDstBytesAdded array to check the value against.
 * @return	"TBool"
 *			ETrue if the dest bytes added is as expected.
 *			EFalse if the dest bytes added is not as expected.
 */
TBool CTestStep_MMF_ACOD::CheckDstBytesAddedL(TUint aCount)
	{
	if (iCodecProcessResult.iDstBytesAdded == iExpectedDstBytesAdded[aCount])
		return ETrue;
	else if (STATIC_CAST(TUint, iFileDataLeft) < iExpectedSrcBytesProcessed[aCount])
		{
		INFO_PRINTF4(_L(">> TCodecProcessResult::iDstBytesAdded is %d, Not enough source data available in file. iFileDataLeft for LOOP %d is %d"), iCodecProcessResult.iDstBytesAdded, aCount, iFileDataLeft);
		return ETrue;
		}
	else
		{
		INFO_PRINTF4(_L(">> TCodecProcessResult::iDstBytesAdded is %d, iExpectedDstBytesAdded for LOOP %d is %d"), iCodecProcessResult.iDstBytesAdded, aCount, iExpectedDstBytesAdded[aCount]);
		return EFalse;
		}
	}

/** @xxxx
 * This function checks the length of iOutputDataBuffer against iTotalDstBytesAdded.
 *
 * @return	"TBool"
 *			ETrue if the values are the same.
 *			EFalse if the values are different.
 */
TBool CTestStep_MMF_ACOD::CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL()
	{
	if (iOutputDataBuffer->Length() == STATIC_CAST(TInt, iTotalDstBytesAdded))
		return ETrue;
	else
		{
		INFO_PRINTF3(_L(">> Output Data Buffer Length is %d, iTotalDstBytesAdded is %d"), iOutputDataBuffer->Length(), iTotalDstBytesAdded);
		return EFalse;
		}
	}

/** @xxxx
 * Checks the current return value against iExpectedReturnValue.
 *
 * @param	"TUint aCount"
 *			Index of the iExpectedReturnValue array to check the value against.
 * @return	"TBool"
 *			ETrue if the return value is as expected.
 *			EFalse if the return value is not as expected.
 */
TBool CTestStep_MMF_ACOD::CheckReturnValueL(TUint aCount)
	{
	if (aCount >= 5)
		User::Leave(KErrArgument);

	if (iCodecProcessResult.iStatus == iExpectedReturnValue[aCount].iStatus)
		{
		return ETrue;
		}
	else
		{
		INFO_PRINTF4(_L(">> TCodecProcessResult::iStatus is %d, iExpectedReturnValue for LOOP %d is %d"), iCodecProcessResult.iStatus, aCount, iExpectedReturnValue[aCount].iStatus);
		return EFalse;
		}
	}

/** @xxxx
 * This function fills the source buffer with data from the file specified.
 *
 * @param	"CMMFBuffer* aSrcBuffer"
 *			Buffer to contain the data read from the source file.
 *			"const TDesC& aFileName"
 *			Name of source file to use.
 *			"TUint aPosition"
 *			Position in the file from which to read the data.
 */
void CTestStep_MMF_ACOD::FillSourceBufferFromSourceFileL(CMMFBuffer* aSrcBuffer, const TDesC& aFilename, TUint aPosition)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile file;
	User::LeaveIfError(file.Open(fs, aFilename, EFileRead|EFileShareAny));
	CleanupClosePushL(file);

	iFileDataLeft = 0;
	file.Size(iFileDataLeft);
	if (STATIC_CAST(TInt, aPosition) < iFileDataLeft)
		iFileDataLeft -= aPosition;
	else User::Leave(KErrEof);

	RFileReadStream fileReadStream(file, aPosition);
	CleanupClosePushL(fileReadStream);

	TInt srcBufferLength = STATIC_CAST(CMMFDataBuffer*, aSrcBuffer)->Data().MaxLength();

	//read data from source file into source buffer
	if(iFileDataLeft < srcBufferLength)
		fileReadStream.ReadL(STATIC_CAST(CMMFDataBuffer*, aSrcBuffer)->Data(), iFileDataLeft);
	else
		fileReadStream.ReadL(STATIC_CAST(CMMFDataBuffer*, aSrcBuffer)->Data());

	CleanupStack::PopAndDestroy(3);  //filereadstream, file, fs
	}

/** @xxxx
 * This function tests the functionality of ProcessL for the codec specified.
 *
 * @param	"TUid aCodecUID"
 *			Uid of the codec under test.
 *			"TUint aSrcBufferSize"
 *			Size of source buffer to test the codec with.
 *			"TUint aDstBufferSize"
 *			Size of destination buffer to test the codec with.
 *			"TUint aSrcBufferLength"
 *			Length of source buffer to test the codec with if less than the maxLength.
 *			aSrcBufferLength defaults to zero. It is not used unless a value is
 *			specified. A value should only be specified in cases where the buffer
 *			length is less than the maxlength.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
TBool CTestStep_MMF_ACOD::TestProcessL(TUid aCodecUID, TUint aSrcBufferSize, TUint aDstBufferSize, TUint aSrcBufferLength)
	{
	//create buffers
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(aSrcBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(aDstBufferSize);
	CleanupStack::PushL(dstBuffer);

	//Fill source buffer with data
	HBufC* srcFilename = HBufC::NewLC(KMaxPath);
	TPtr ptrSrcFilename = srcFilename->Des();
	GetSourceFilenameFromCodecUidL(aCodecUID.iUid, ptrSrcFilename);
	FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, 0);

	//if source buffer has different length to max length, crop data
	if (aSrcBufferLength > 0)
		{
		srcBuffer->Data().SetLength(aSrcBufferLength);
		}

	//instantiate codec
	CMMFCodec* codec = CMMFCodec::NewL(aCodecUID);
	CleanupStack::PushL(codec);

	TBool testOK = ETrue;
	TRAPD(err, iCodecProcessResult = codec->ProcessL(*srcBuffer, *dstBuffer));
	if (err)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestProcessL Leave occurred in ProcessL, error code %d"), err);
		testOK = EFalse;
		}
	else
		{
		//copy data to iOutputDataBuffer
		iOutputDataBuffer = HBufC8::NewL(dstBuffer->Data().Length());
		TPtr8 outputDataBufferPtr = iOutputDataBuffer->Des();
		outputDataBufferPtr.Append(dstBuffer->Data());

		iTotalDstBytesAdded = iCodecProcessResult.iDstBytesAdded;
		iTotalSrcBytesProcessed = iCodecProcessResult.iSrcBytesProcessed;

		//investigate results
		HBufC* filename = HBufC::NewLC(KMaxPath);
		TPtr ptrFilename = filename->Des();
		GetComparisonFilenameFromCodecUidL(aCodecUID.iUid, ptrFilename);

		if (!CheckOutputAgainstComparisonFileL(ptrFilename, 0))
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestProcessL CheckOutputAgainstComparisonFileL returned EFalse"));
			testOK = EFalse;
			}
		if (!CheckDstBytesAddedL(0))
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestProcessL CheckDstBytesAddedL returned EFalse"));
			testOK = EFalse;
			}
		if (!CheckSrcBytesProcessedL(0))
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestProcessL CheckSrcBytesProcessedL returned EFalse"));
			testOK = EFalse;
			}
		if (!CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL())
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestProcessL CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL returned EFalse"));
			testOK = EFalse;
			}
		if (!CheckReturnValueL(0))
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestProcessL CheckReturnValue returned EFalse"));
			testOK = EFalse;
			}

		//clean up memory
		CleanupStack::PopAndDestroy(); //filename
		delete iOutputDataBuffer;
		iOutputDataBuffer = NULL;
		}

	CleanupStack::PopAndDestroy(4); //codec, srcFilename, dstBuffer, srcBuffer

	if(!testOK)
		return EFalse;
	else
		return ETrue;
	}


/** @xxxx
 * This function tests the functionality of ProcessL for the codec specified.
 * It mimics the datapath by calling ProcessL five times in a loop, checking the
 * return value each time and passing in new buffers accordlingly.
 *
 * @param	"TUid aCodecUID"
 *			Uid of the codec under test.
 *			"TUint aSrcBufferSize"
 *			Size of source buffers to test the codec with.
 *			"TUint aDstBufferSize"
 *			Size of destination buffers to test the codec with.
 *			"TUint aSrcBufferLength"
 *			Length of source buffer to test the codec with if less than the maxLength.
 *			aSrcBufferLength defaults to zero and is not used unless a value is
 *			specified.A value should only be specified if the buffer length is less
 *			than the maxlength.
 *	 		"TBool aCheckExpectedBytesProcessed"
 *			Boolean value to determine whether to check the srcBytesProcessed and
 *			dstBytesAdded against their expected values. This defaults to true,
 *			but allows for the tests to be bypassed for non-linear codecs where the
 *			expected values are difficult to determine.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
#ifdef OLD_CRUD_REMOVE
TBool CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL(TUid aCodecUID, TUint aSrcBufferSize, TUint aDstBufferSize, TUint aSrcBufferLength, TBool aCheckExpectedBytesProcessed)
	{
	//create buffers
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(aSrcBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(aDstBufferSize);
	CleanupStack::PushL(dstBuffer);

	//read first buffer of data
	HBufC* srcFilename = HBufC::NewLC(KMaxPath);
	TPtr ptrSrcFilename = srcFilename->Des();
	GetSourceFilenameFromCodecUidL(aCodecUID.iUid, ptrSrcFilename);
	TRAPD(srcFileError, FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, 0));
	TBool endOfFile = EFalse;
	if (srcFileError == KErrEof)
		endOfFile = ETrue;
	else if (srcFileError)
		User::Leave(srcFileError);


	//if source buffer has different length to max length, crop data
	if (aSrcBufferLength > 0)
		{
		srcBuffer->Data().SetLength(aSrcBufferLength);
		}

	//instantiate codec
	CMMFCodec* codec = CMMFCodec::NewL(aCodecUID);
	CleanupStack::PushL(codec);

	TBool testOK = ETrue;
	TRAPD(err, iOutputDataBuffer = HBufC8::NewL(aDstBufferSize*5));
	User::LeaveIfError(err);
	TPtr8 outputDataBufferPtr = iOutputDataBuffer->Des();


	TUint maxIters = 0;
	if(iSingleIteration)
		{
		maxIters = 1;
		}
	else
		{
		maxIters = 5;
		}

	for (TUint i=0; i</*5*/maxIters; i++)
		{
		TRAPD(err, iCodecProcessResult = codec->ProcessL(*srcBuffer, *dstBuffer));
		if (err)
			{
			if (err == iExpectedLeaveErrorCode)
				break;
			else
				{
				ERR_PRINTF3(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL LOOP %d Leave occurred in ProcessL, error code %d"), i, err);
				testOK = EFalse;
				break;
				}
			}
		else if (iExpectedLeaveErrorCode)
				{
				ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL LOOP %d A leave was expected in ProcessL, but this didn't occur"), i);
				testOK = EFalse;
				break;
				}

		iTotalDstBytesAdded += iCodecProcessResult.iDstBytesAdded;
		iTotalSrcBytesProcessed += iCodecProcessResult.iSrcBytesProcessed;

		if (i<4)
			{
			switch (iCodecProcessResult.iStatus)
				{
				case TCodecProcessResult::EProcessComplete:
					{
					outputDataBufferPtr.Append(dstBuffer->Data());
					dstBuffer->SetPosition(0);
					if (!CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL())
						{
						ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL LOOP %d CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL returned EFalse"), i);
						testOK = EFalse;
						}
					srcBuffer->SetPosition(0);
					srcBuffer->NextFrame();

					if (!endOfFile)
						{
						TRAP(srcFileError, FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, iTotalSrcBytesProcessed));
						if (srcFileError == KErrEof)
							endOfFile = ETrue;
						else if (srcFileError)
							User::Leave(srcFileError);
						}
					if (aSrcBufferLength > 0)
						srcBuffer->Data().SetLength(aSrcBufferLength);
					break;
					}
				case TCodecProcessResult::EProcessIncomplete:
					{
					outputDataBufferPtr.Append(dstBuffer->Data());
					dstBuffer->SetPosition(0);
					if (!CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL())
						{
						ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL LOOP %d CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL returned EFalse"), i);
						testOK = EFalse;
						}
					srcBuffer->SetPosition(iCodecProcessResult.iSrcBytesProcessed + srcBuffer->Position());
					break;
					}
				case TCodecProcessResult::EDstNotFilled:
					{
					dstBuffer->SetPosition(iCodecProcessResult.iDstBytesAdded + dstBuffer->Position());

					if (!endOfFile)
						{
						TRAP(srcFileError, FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, iTotalSrcBytesProcessed));
						if (srcFileError == KErrEof)
							endOfFile = ETrue;
						else if (srcFileError)
							User::Leave(srcFileError);
						}
					if (aSrcBufferLength > 0)
						srcBuffer->Data().SetLength(aSrcBufferLength);
					break;
					}
				default:
					User::Leave(KErrCorrupt); //should never get here
				}
			}
		else //last time
			{
			outputDataBufferPtr.Append(dstBuffer->Data());
			if (!CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL())
				{
				ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL LOOP %d CheckOutputDataBufferSizeAgainstTotalDstBytesAddedL returned EFalse"), i);
				testOK = EFalse;
				}
			}

		if (!CheckReturnValueL(i))
			{
			ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL LOOP %d CheckReturnValue returned EFalse"), i);
			testOK = EFalse;
			}
		if (aCheckExpectedBytesProcessed)
			{
			if (!CheckDstBytesAddedL(i))
				{
				ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL LOOP %d CheckDstBytesAddedL returned EFalse"), i);
				testOK = EFalse;
				}
			if (!CheckSrcBytesProcessedL(i))
				{
				ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL LOOP %d CheckSrcBytesProcessedL returned EFalse"), i);
				testOK = EFalse;
				}
			}

		if (endOfFile)
			break;
		}

	if (testOK)
		{
		HBufC* filename = HBufC::NewLC(KMaxPath);
		TPtr ptrFilename = filename->Des();
		GetComparisonFilenameFromCodecUidL(aCodecUID.iUid, ptrFilename);

		if (!CheckOutputAgainstComparisonFileL(ptrFilename, 0))
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestProcessLLoopFiveTimesL CheckOutputAgainstComparisonFileL returned EFalse"));
			testOK = EFalse;
			}

		CleanupStack::PopAndDestroy(); //filename
		}

	//clean up memory
	delete iOutputDataBuffer;
	iOutputDataBuffer = NULL;
	CleanupStack::PopAndDestroy(4); //codec, srcFilename, dstBuffer, srcBuffer

	if(!testOK)
		return EFalse;
	else
		return ETrue;
	}
#endif

/** @xxxx
 * This function tests how the codec handles a reposition. This test is only really
 * relevant to codecs that store data between calls to ProcessL. When a reposition
 * occurs the stored data should be flushed.
 *
 * ProcessL is called 3 times:
 * After the first call the frame number is increased by one;
 * After the second call the output data is checked to ensure that extra data HAS been
 *	written to the output buffer; The frame number is set to 5 to mimic a reposition.
 * After the third call the output data is checked to ensure that extra data HAS NOT
 *	been written to the output buffer.
 *
 * Note: It is important to use buffer sizes that will force the codec to store extra
 * data and return EProcessComplete or EDstNotFilled each time around the loop.
 *
 * @param	"TUid aCodecUID"
 *			Uid of the codec under test.
 *			"TUint aSrcBufferSize"
 *			Size of source buffer to test the codec with.
 *			"TUint aDstBufferSize"
 *			Size of destination buffer to test the codec with.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
 #ifdef OLD_CRUD_REMOVE
TBool CTestStep_MMF_ACOD::TestRepositionL(TUid aCodecUID, TUint aSrcBufferSize, TUint aDstBufferSize)
	{
	//create buffers
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(aSrcBufferSize);
	CleanupStack::PushL(srcBuffer);

	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(aDstBufferSize);
	CleanupStack::PushL(dstBuffer);

	//read first buffer of data
	HBufC* srcFilename = HBufC::NewLC(KMaxPath);
	TPtr ptrSrcFilename = srcFilename->Des();
	GetSourceFilenameFromCodecUidL(aCodecUID.iUid, ptrSrcFilename);
	FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, 0);

	//instantiate codec
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(aCodecUID));
	if (err)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestRepositionL Leave occurred in NewL, error code %d"), err);
		return EFalse;
		}
	else
		{
		CleanupStack::PushL(codec);
		}

	TBool testOK = ETrue;
	iOutputDataBuffer = HBufC8::NewL(aDstBufferSize);
	TPtr8 outputDataBufferPtr = iOutputDataBuffer->Des();

	HBufC* filename = HBufC::NewLC(KMaxPath);
	TPtr ptrFilename = filename->Des();
	GetComparisonFilenameFromCodecUidL(aCodecUID.iUid, ptrFilename);

	for (TUint i=0; i<3; i++)
		{
		iCodecProcessResult = codec->ProcessL(*srcBuffer, *dstBuffer);

		if ((iCodecProcessResult.iStatus == TCodecProcessResult::EProcessComplete) ||
			(iCodecProcessResult.iStatus == TCodecProcessResult::EDstNotFilled))
			{
			switch(i)
				{
				case 0:
					{
					srcBuffer->SetPosition(0);
					srcBuffer->SetFrameNumber(5);
					dstBuffer->SetPosition(0);

					break;
					}
				case 1:
					{
					srcBuffer->SetPosition(0);
					srcBuffer->SetFrameNumber(0);
					dstBuffer->SetPosition(0);

					outputDataBufferPtr.Copy(dstBuffer->Data());
					if (CheckOutputAgainstComparisonFileL(ptrFilename, 0))
						{
						INFO_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestRepositionL CheckOutputAgainstComparisonFileL returned ETrue for LOOP %d."), i);
						INFO_PRINTF1(_L(">> If the codec stores left over data from the previous call to ProcessL, this may indicate that the test is invalid."));
						INFO_PRINTF1(_L(">> Try using different buffer sizes."));
						}
					break;
					}
				case 2:
					{
					outputDataBufferPtr.Copy(dstBuffer->Data());
					if (!CheckOutputAgainstComparisonFileL(ptrFilename, 0))
						{
						ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestRepositionL CheckOutputAgainstComparisonFileL returned EFalse for LOOP %d"), i);
						testOK = EFalse;
						}
					break;
					}
				default:
					break;
				}
			}
		else
			{
			INFO_PRINTF3(_L(">> CTestStep_MMF_ACOD::TestRepositionL Return code was %d for LOOP %d."), iCodecProcessResult.iStatus, i);
			INFO_PRINTF1(_L(">> This indicates that the test is invalid. Try using different buffer sizes."));
			break;
			}
		}

	//clean up memory
	delete iOutputDataBuffer;
	iOutputDataBuffer = NULL;
	CleanupStack::PopAndDestroy(5); //filename, codec, srcFilename, dstBuffer, srcBuffer

	if(!testOK)
		return EFalse;
	else
		return ETrue;
	}

#endif

/** @xxxx
 * This function tests codec instantiation by Uid.
 *
 * @param	"TUid aCodecUID"
 *			Uid of the codec under test.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
 #ifdef OLD_CRUD_REMOVE
TBool CTestStep_MMF_ACOD::TestNewL(TUid aCodecUID)
	{
	//create buffers
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(dstBuffer);

	//Fill source buffer with data
	HBufC* srcFilename = HBufC::NewLC(KMaxPath);
	TPtr ptrSrcFilename = srcFilename->Des();
	GetSourceFilenameFromCodecUidL(aCodecUID.iUid, ptrSrcFilename);
	FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, 0);

	TBool testOK = ETrue;
	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(aCodecUID));
	if (err)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestNewL Leave occurred in NewL, error code %d"), err);
		testOK = EFalse;
		}
	else
		{
		CleanupStack::PushL(codec);

		//Check codec is non-null
		if (!codec)
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestNewL NewL returned NULL"));
			testOK = EFalse;
			}
		else
			{
			//Process one buffer of data
			iCodecProcessResult = codec->ProcessL(*srcBuffer, *dstBuffer);

			//copy data to iOutputDataBuffer
			iOutputDataBuffer = HBufC8::NewL(dstBuffer->Data().Length());
			TPtr8 outputDataBufferPtr = iOutputDataBuffer->Des();
			outputDataBufferPtr.Append(dstBuffer->Data());

			//investigate results
			HBufC* filename = HBufC::NewLC(KMaxPath);
			TPtr ptrFilename = filename->Des();
			GetComparisonFilenameFromCodecUidL(aCodecUID.iUid, ptrFilename);

			if (!CheckOutputAgainstComparisonFileL(ptrFilename, 0))
				{
				ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestNewL CheckOutputAgainstComparisonFileL returned EFalse"));
				testOK = EFalse;
				}
			CleanupStack::PopAndDestroy(); //filename
			}

		CleanupStack::PopAndDestroy(); //codec
		delete iOutputDataBuffer;
		iOutputDataBuffer = NULL;
		}

	CleanupStack::PopAndDestroy(3); //srcFilename, dstBuffer, srcBuffer

	if(!testOK)
		return EFalse;
	else
		return ETrue;
	}
#endif

/** @xxxx
 * This function tests codec instantiation by fourCC codes.
 *
 * @param	"const TFourCC& aSrcFourCC"
 *			Source fourCC code of the codec under test.
 *			"const TFourCC& aDstFourCC"
 *			Destination fourCC code of the codec under test.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
#ifdef OLD_CRUD_REMOVE
TBool CTestStep_MMF_ACOD::TestNewL(const TFourCC& aSrcFourCC, const TFourCC& aDstFourCC)
	{
	//create buffers
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(dstBuffer);

	//Fill source buffer with data
	HBufC* srcFilename = HBufC::NewLC(KMaxPath);
	TPtr ptrSrcFilename = srcFilename->Des();
	GetSourceFilenameFromFourCCL(aSrcFourCC, aDstFourCC, ptrSrcFilename);
	FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, 0);

	TBool testOK = ETrue;

	//create codec from uid
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(aSrcFourCC, aDstFourCC));
	if (err)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestNewL Leave occurred in NewL, error code %d"), err);
		testOK = EFalse;
		}
	else
		{
		CleanupStack::PushL(codec);

		//Check codec is non-null
		if (!codec)
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestNewL NULL CMMFCodec returned from NewL"));
			testOK = EFalse;
			}
		else
			{
			//Process one buffer of data
			iCodecProcessResult = codec->ProcessL(*srcBuffer, *dstBuffer);

			//copy data to iOutputDataBuffer
			iOutputDataBuffer = HBufC8::NewL(dstBuffer->Data().Length());
			TPtr8 outputDataBufferPtr = iOutputDataBuffer->Des();
			outputDataBufferPtr.Append(dstBuffer->Data());

			//investigate results
			HBufC* filename = HBufC::NewLC(KMaxPath);
			TPtr ptrFilename = filename->Des();
			GetComparisonFilenameFromFourCCL(aSrcFourCC, aDstFourCC, ptrFilename);

			if (!CheckOutputAgainstComparisonFileL(ptrFilename, 0))
				{
				ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestNewL CheckOutputAgainstComparisonFileL returned EFalse"));
				testOK = EFalse;
				}
			CleanupStack::PopAndDestroy(); //filename
			}

		CleanupStack::PopAndDestroy(); //codec
		delete iOutputDataBuffer;
		iOutputDataBuffer = NULL;
		}

	CleanupStack::PopAndDestroy(3); //srcFilename, dstBuffer, srcBuffer

	if(!testOK)
		return EFalse;
	else
		return ETrue;
	}
#endif

/** @xxxx
 * This function tests codec instantiation by fourCC codes and preferred supplier.
 * Two new codecs have been created for the purpose of this test. They take the same
 * fourCC codes (AAAA and BBBB) but have different suppliers.
 *
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
#ifdef OLD_CRUD_REMOVE
TBool CTestStep_MMF_ACOD::TestNewLPreferredSupplierL()
	{
	//create buffers
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(dstBuffer);

	//Fill source buffer with data
	HBufC* srcFilename = HBufC::NewLC(KMaxPath);
	TPtr ptrSrcFilename = srcFilename->Des();
	ptrSrcFilename.Copy(KSourceFilesDir);
	ptrSrcFilename.Append(KPreferredSupplierSourceFile);
	FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, 0);

	TBool testOK = ETrue;

	//create codec from uid and preferredSupplier
	CMMFCodec* codec = NULL;
	TRAPD(err, codec = CMMFCodec::NewL(KMMFFourCCCodeAAAA, KMMFFourCCCodeBBBB, KPreferredSupplier));
	if (err)
		{
		ERR_PRINTF2(_L(">> CTestStep_MMF_ACOD::TestNewLPreferredSupplierL Leave occurred in NewL, error code %d"), err);
		testOK = EFalse;
		}
	else
		{
		CleanupStack::PushL(codec);

		//Check codec is non-null
		if (!codec)
			{
			ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestNewLPreferredSupplierL NULL CMMFCodec returned from NewL"));
			testOK = EFalse;
			}
		else
			{
			//Process one buffer of data
			iCodecProcessResult = codec->ProcessL(*srcBuffer, *dstBuffer);

			//copy data to iOutputDataBuffer
			iOutputDataBuffer = HBufC8::NewL(dstBuffer->Data().Length());
			TPtr8 outputDataBufferPtr = iOutputDataBuffer->Des();
			outputDataBufferPtr.Append(dstBuffer->Data());

			//investigate results
			HBufC* filename = HBufC::NewLC(KMaxPath);
			TPtr ptrFilename = filename->Des();
			ptrFilename.Copy(KComparisonFilesDir);
			ptrFilename.Append(KPreferredSupplierComparisonFile);

			if (!CheckOutputAgainstComparisonFileL(ptrFilename, 0))
				{
				ERR_PRINTF1(_L(">> CTestStep_MMF_ACOD::TestNewLPreferredSupplierL CheckOutputAgainstComparisonFileL returned EFalse"));
				testOK = EFalse;
				}

			CleanupStack::PopAndDestroy(filename);
			}

		CleanupStack::PopAndDestroy(codec);
		delete iOutputDataBuffer;
		iOutputDataBuffer = NULL;
		}

	CleanupStack::PopAndDestroy(3, srcBuffer); //srcFilename, dstBuffer, srcBuffer

	if(!testOK)
		return EFalse;
	else
		return ETrue;
	}
#endif

/** @xxxx
 * This functionality isn't implemented yet
 */
#ifdef OLD_CRUD_REMOVE
TBool CTestStep_MMF_ACOD::TestMemoryScribbleL(TUid aCodecUID)
	{
	aCodecUID = aCodecUID;

	INFO_PRINTF1(_L(">> This test has not been implemented, as it has not yet been determined how to test for memory scribbles."));
	return ETrue;
	}
#endif

/** @xxxx
 * This function panics if any memory leaks occur during the calls to NewL or ProcessL.
 *
 * @param	"TUid aCodecUID"
 *			Uid of codec under test.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
TBool CTestStep_MMF_ACOD::TestMemoryLeakL(TUid aCodecUID)
	{
	//create buffers
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(dstBuffer);

	//Fill source buffer with data
	HBufC* srcFilename = HBufC::NewLC(KMaxPath);
	TPtr ptrSrcFilename = srcFilename->Des();
	GetSourceFilenameFromCodecUidL(aCodecUID.iUid, ptrSrcFilename);
	FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, 0);

	__MM_HEAP_MARK;
	//instantiate codec
	CMMFCodec* codec = CMMFCodec::NewL(aCodecUID);
	CleanupStack::PushL(codec);

	iCodecProcessResult = codec->ProcessL(*srcBuffer, *dstBuffer);

	CleanupStack::PopAndDestroy(4); //codec, srcFilename, dstBuffer, srcBuffer

	__MM_HEAP_MARKEND;

	return ETrue;
	}

/** @xxxx
 * This function tests how NewL deals with a memory allocation failure.
 * This test will pass if NewL leaves with the correct error code on every
 * memory allocation.
 *
 * @param	"TUid aCodecUID"
 *			Uid of codec under test.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
#ifdef OLD_CRUD_REMOVE
TBool CTestStep_MMF_ACOD::TestNewLOutOfMemoryL(TUid aCodecUID)
	{
	aCodecUID = aCodecUID;
	CMMFCodec* codec = NULL;
	TInt failCount = 1;
	TBool testComplete = EFalse;
	TBool testOK = ETrue;

	while (!testComplete)
		{
		TInt err = 0;
		__UHEAP_FAILNEXT(failCount);
		__MM_HEAP_MARK;

		TRAP(err, codec = CMMFCodec::NewL(aCodecUID));

		switch (err)
			{
			case KErrNoMemory :
				break;
			case KErrNone :
				{
				//attempt to allocate another cell. If this fails, we have tested all
				//of the memory allocs carried out by NewL.
				TAny *testAlloc = User::Alloc(1);
				if (testAlloc==NULL)
					{
					testComplete = ETrue;
					failCount --;
					}
				else
					User::Free(testAlloc);

				delete codec;
				codec = NULL;
				break;
				}
			default:
				{
				testOK = EFalse;
				testComplete = ETrue;
				break;
				}
			}

		__MM_HEAP_MARKEND;
		__UHEAP_RESET;
		failCount++;
		}

	if(!testOK)
		return EFalse;
	else
		return ETrue;
	}

#endif


/** @xxxx
 * This function tests how ProcessL would deal with a memory allocation failure.
 * For most of the codecs ProcessL doesn't allocate any memory, so this test will
 * pass when ProcessL succeeds as well as when ProcessL leaves with the correct code.
 *
 * @param	"TUid aCodecUID"
 *			Uid of codec under test.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
TBool CTestStep_MMF_ACOD::TestProcessLOutOfMemoryL(TUid aCodecUID)
	{
	//create buffers
	CMMFDataBuffer *srcBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(srcBuffer);
	CMMFDataBuffer *dstBuffer = CMMFDataBuffer::NewL(KDefaultBufferSize);
	CleanupStack::PushL(dstBuffer);

	//Fill source buffer with data
	HBufC* srcFilename = HBufC::NewLC(KMaxPath);
	TPtr ptrSrcFilename = srcFilename->Des();
	GetSourceFilenameFromCodecUidL(aCodecUID.iUid, ptrSrcFilename);
	FillSourceBufferFromSourceFileL(srcBuffer, ptrSrcFilename, 0);

	//instantiate codec
	CMMFCodec* codec = CMMFCodec::NewL(aCodecUID);
	CleanupStack::PushL(codec);
	TBool testOK = ETrue;
	TUint failCount = 1;
	TBool testComplete = EFalse;

	while (!testComplete)
		{
		__UHEAP_FAILNEXT(failCount);
		__MM_HEAP_MARK;
		TRAPD(err, iCodecProcessResult = codec->ProcessL(*srcBuffer, *dstBuffer));

		switch (err)
			{
			case KErrNoMemory :
				break;
			case KErrNone :
				{
				//attempt to allocate another cell. If this fails, we have tested all
				//of the memory allocs carried out by ProcessL.
				TAny *testAlloc = User::Alloc(1);
				if (testAlloc==NULL)
					{
					testComplete = ETrue;
					failCount --;
					}
				else
					User::Free(testAlloc);
				break;
				}
			default:
				{
				testOK = EFalse;
				testComplete = ETrue;
				break;
				}
			}

		__MM_HEAP_MARKEND;
		__UHEAP_RESET;
		failCount++;
		}

	CleanupStack::PopAndDestroy(4); //codec, srcFilename, dstBuffer, srcBuffer

	if(!testOK)
		return EFalse;
	else
		return ETrue;
	}
