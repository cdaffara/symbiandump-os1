// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TSU_MMF_ACOD.h"
#include "TSU_MMF_ACODSuite.h"

#include <f32file.h>
#include <s32file.h>
#include <mmf/plugin/mmfcodecimplementationuids.hrh>

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
		case KMmfUidCodecAlawToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KAlawToPCM16ComparisonFile);
			break;
		case KMmfUidCodecPCM16ToAlaw:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToAlawComparisonFile);
			break;
		case KMmfUidCodecPCM8ToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM8ToPCM16ComparisonFile);
			break;
		case KMmfUidCodecAudioS16ToPCMS8:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCM8ComparisonFile);
			break;
		//case KMMFUidCodecPCM16SwapEndian:
		case KMMFUidCodecPCM16toPCM16B:
		case KMMFUidCodecPCM16BtoPCM16:
		case KMMFUidCodecPCMU16toPCMU16B:
		case KMMFUidCodecPCMU16BtoPCMU16:
			//This file is used for all swap endian tests other than instantiation.
			//There is no need to test ProcessL for every swap endian combination.
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCM16BComparisonFile);
			break;
		case KMmfUidCodecAudioS16ToPCMU16BE:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCMU16BComparisonFile);
			break;
		case KMmfUidCodecAudioU16BEToPCMS16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU16BToPCM16ComparisonFile);
			break;
		case KMmfUidCodecAudioS16ToPCMU16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCMU16ComparisonFile);
			break;
		case KMmfUidCodecAudioU16ToPCMS16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU16ToPCM16ComparisonFile);
			break;
		case KMmfUidCodecAudioS16ToPCMU8:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToPCMU8ComparisonFile);
			break;
		case KMmfUidCodecAudioU8ToPCMS16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCMU8ToPCM16ComparisonFile);
			break;
		case KMmfUidCodecIMAADPCMToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KAdpcmToPCM16ComparisonFile);
			break;
		case KMmfUidCodecPCM16ToIMAADPCM:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToAdpcmComparisonFile);
			break;
		case KMmfUidCodecMulawToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KMulawToPCM16ComparisonFile);
			break;
		case KMmfUidCodecPCM16ToMulaw:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToMulawComparisonFile);
			break;
		case KMmfUidCodecIMAADPCMToPCM16Stereo:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KStereoAdpcmToPCM16ComparisonFile);
			break;
		case KMmfUidCodecPCM16ToIMAADPCMStereo:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KPCM16ToStereoAdpcmComparisonFile);
			break;
		case KMmfUidCodecGSM610ToPCM16:
			aFilename.Copy(KComparisonFilesDir);
			aFilename.Append(KGSM610ToPCM16ComparisonFile);
			break;
		case KMmfUidCodecPCM16ToGSM610:
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
		case KMmfUidCodecAlawToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KAlawSourceFile);
			break;
		case KMmfUidCodecPCM16ToAlaw:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidCodecPCM8ToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM8SourceFile);
			break;
		case KMmfUidCodecAudioS16ToPCMS8:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		//case KMMFUidCodecPCM16SwapEndian:
		case KMMFUidCodecPCM16toPCM16B:
		case KMMFUidCodecPCM16BtoPCM16:
		case KMMFUidCodecPCMU16toPCMU16B:
		case KMMFUidCodecPCMU16BtoPCMU16:
			//this file is used for all swap endian tests other than instantiation.
			//there is no need to test ProcessL for every swap endian combination.
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidCodecAudioS16ToPCMU16BE:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidCodecAudioU16BEToPCMS16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCMU16BSourceFile);
			break;
		case KMmfUidCodecAudioS16ToPCMU16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidCodecAudioU16ToPCMS16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCMU16SourceFile);
			break;
		case KMmfUidCodecAudioS16ToPCMU8:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidCodecAudioU8ToPCMS16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCMU8SourceFile);
			break;
		case KMmfUidCodecIMAADPCMToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KAdpcmSourceFile);
			break;
		case KMmfUidCodecPCM16ToIMAADPCM:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidCodecMulawToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KMulawSourceFile);
			break;
		case KMmfUidCodecPCM16ToMulaw:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KPCM16SourceFile);
			break;
		case KMmfUidCodecIMAADPCMToPCM16Stereo:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KStereoAdpcmSourceFile);
			break;
		case KMmfUidCodecPCM16ToIMAADPCMStereo:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KStereoPCM16SourceFile);
			break;
		case KMmfUidCodecGSM610ToPCM16:
			aFilename.Copy(KSourceFilesDir);
			aFilename.Append(KGSM610SourceFile);
			break;
		case KMmfUidCodecPCM16ToGSM610:
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

/** @xxxx
 * This function tests codec instantiation by Uid.
 * 
 * @param	"TUid aCodecUID" 
 *			Uid of the codec under test.
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
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

/** @xxxx
 * This function tests codec instantiation by fourCC codes and preferred supplier.
 * Two new codecs have been created for the purpose of this test. They take the same 
 * fourCC codes (AAAA and BBBB) but have different suppliers.
 *
 * @return	"TBool"
 *			ETrue if the test passes.
 *			EFalse if the test fails.
 */
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

/** @xxxx
 * This functionality isn't implemented yet
 */
TBool CTestStep_MMF_ACOD::TestMemoryScribbleL(TUid aCodecUID)
	{
	aCodecUID = aCodecUID;

	INFO_PRINTF1(_L(">> This test has not been implemented, as it has not yet been determined how to test for memory scribbles."));
	return ETrue;
	}


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

//------------------------------------------------------------------
//			** TEST MMF_ACOD STEPS ***

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0001::CTest_MMF_ACOD_U_0001()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0001-HP");
	}

/** @xxxx
 * Instantiate a codec by FourCC Codes and preferred supplier
 * @test Req. under test REQ172.6.3
 */
TVerdict CTest_MMF_ACOD_U_0001::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewLPreferredSupplierL());

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}
//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0002::CTest_MMF_ACOD_U_0002()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0002-HP");
	}

/** @xxxx
 * ALAW To PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.1
 */
TVerdict CTest_MMF_ACOD_U_0002::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodeALAW, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0003::CTest_MMF_ACOD_U_0003()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0003-HP");
	}

/** @xxxx
 * ALAW To PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.1
 */
TVerdict CTest_MMF_ACOD_U_0003::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0004::CTest_MMF_ACOD_U_0004()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0004-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 100;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * ALAW To PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0004::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KAlawToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0005::CTest_MMF_ACOD_U_0005()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0005-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 25;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * ALAW To PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0005::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KAlawToPCM16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0006::CTest_MMF_ACOD_U_0006()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0006-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 5000;
		iExpectedDstBytesAdded[i] = 10000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * ALAW To PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0006::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0007::CTest_MMF_ACOD_U_0007()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0007-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * ALAW To PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0007::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0008::CTest_MMF_ACOD_U_0008()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0008-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * ALAW To PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0008::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0009::CTest_MMF_ACOD_U_0009()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0009-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * ALAW To PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0009::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KAlawToPCM16OptimumSrc, KAlawToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0010::CTest_MMF_ACOD_U_0010()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0010-HP");
//	iHeapSize = 0x20000;
	}

/** @xxxx
 * ALAW To PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0010::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	INFO_PRINTF1(_L(">> The ALAW to PCM16 codec doesn't currently store data between calls to ProcessL"));

	TRAPD(err, testOK = TestRepositionL(codecUid, KAlawToPCM16OptimumSrc, KAlawToPCM16OptimumDst+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0011::CTest_MMF_ACOD_U_0011()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0011-HP");
	}

/** @xxxx
 * PCM16 To ALAW Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.3
 */
TVerdict CTest_MMF_ACOD_U_0011::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodeALAW));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0012::CTest_MMF_ACOD_U_0012()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0012-HP");
	}

/** @xxxx
 * PCM16 To ALAW Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.3
 */
TVerdict CTest_MMF_ACOD_U_0012::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0013::CTest_MMF_ACOD_U_0013()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0013-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 25;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 To ALAW Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0013::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToAlawOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0014::CTest_MMF_ACOD_U_0014()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0014-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 100;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCM16 To ALAW Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0014::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToAlawOptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0015::CTest_MMF_ACOD_U_0015()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0015-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 5000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To ALAW Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0015::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0016::CTest_MMF_ACOD_U_0016()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0016-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To ALAW Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0016::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0017::CTest_MMF_ACOD_U_0017()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0017-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To ALAW Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0017::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0018::CTest_MMF_ACOD_U_0018()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0018-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To ALAW Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0018::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToAlawOptimumSrc, KPCM16ToAlawOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0019::CTest_MMF_ACOD_U_0019()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0019-HP");
//	iHeapSize = 0x20000;
	}

/** @xxxx
 * PCM16 To ALAW Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0019::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCM16 to ALAW codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0020::CTest_MMF_ACOD_U_0020()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0020-HP");
	}

/** @xxxx
 * PCM8 To PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.4
 */
TVerdict CTest_MMF_ACOD_U_0020::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM8, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0021::CTest_MMF_ACOD_U_0021()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0021-HP");
	}

/** @xxxx
 * PCM8 To PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.4
 */
TVerdict CTest_MMF_ACOD_U_0021::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0022::CTest_MMF_ACOD_U_0022()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0022-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 100;		
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;	
		}
	}

/** @xxxx
 * PCM8 To PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0022::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM8ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0023::CTest_MMF_ACOD_U_0023()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0023-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 25;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCM8 To PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0023::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM8ToPCM16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0024::CTest_MMF_ACOD_U_0024()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0024-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 5000;
		iExpectedDstBytesAdded[i] = 10000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM8 To PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0024::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0025::CTest_MMF_ACOD_U_0025()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0025-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM8 To PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0025::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0026::CTest_MMF_ACOD_U_0026()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0026-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM8 To PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0026::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0027::CTest_MMF_ACOD_U_0027()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0027-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM8 To PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0027::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM8ToPCM16OptimumSrc, KPCM8ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0028::CTest_MMF_ACOD_U_0028()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0028-HP");
//	iHeapSize = 0x20000;
	}

/** @xxxx
 * PCM8 To PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0028::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCM8 to PCM16 codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KPCM8ToPCM16OptimumSrc, KPCM8ToPCM16OptimumDst+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0029::CTest_MMF_ACOD_U_0029()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0029-HP");
	}

/** @xxxx
 * PCM16 To PCM8 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.5
 */
TVerdict CTest_MMF_ACOD_U_0029::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodePCM8));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0030::CTest_MMF_ACOD_U_0030()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0030-HP");
	}

/** @xxxx
 * PCM16 To PCM8 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.5
 */
TVerdict CTest_MMF_ACOD_U_0030::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0031::CTest_MMF_ACOD_U_0031()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0031-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 25;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 To PCM8 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0031::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToPCM8OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0032::CTest_MMF_ACOD_U_0032()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0032-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 100;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCM16 To PCM8 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0032::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCM8OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0033::CTest_MMF_ACOD_U_0033()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0033-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 5000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCM8 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0033::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0034::CTest_MMF_ACOD_U_0034()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0034-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCM8 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0034::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0035::CTest_MMF_ACOD_U_0035()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0035-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCM8 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0035::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0036::CTest_MMF_ACOD_U_0036()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0036-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCM8 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0036::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCM8OptimumSrc, KPCM16ToPCM8OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0037::CTest_MMF_ACOD_U_0037()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0037-HP");
	}

/** @xxxx
 * PCM16 To PCM8 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0037::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCM16 to PCM8 codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0038::CTest_MMF_ACOD_U_0038()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0038-HP");
	}

/** @xxxx
 * PCM16 To PCM16B Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.6
 */
TVerdict CTest_MMF_ACOD_U_0038::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodePCM16B));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0039::CTest_MMF_ACOD_U_0039()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0039-HP");
	}

/** @xxxx
 * PCM16B To PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.6
 */
TVerdict CTest_MMF_ACOD_U_0039::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16B, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0040::CTest_MMF_ACOD_U_0040()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0040-HP");
	}

/** @xxxx
 * PCMU16 To PCMU16B Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.6
 */
TVerdict CTest_MMF_ACOD_U_0040::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCMU16, KMMFFourCCCodePCMU16B));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0041::CTest_MMF_ACOD_U_0041()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0041-HP");
	}

/** @xxxx
 * PCMU16B To PCMU16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.6
 */
TVerdict CTest_MMF_ACOD_U_0041::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCMU16B, KMMFFourCCCodePCMU16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0042::CTest_MMF_ACOD_U_0042()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0042-HP");
	}

/** @xxxx
 * PCM16 To PCM16B Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.6
 */
TVerdict CTest_MMF_ACOD_U_0042::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0043::CTest_MMF_ACOD_U_0043()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0043-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 To PCM16B Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0043::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToPCM16BOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0044::CTest_MMF_ACOD_U_0044()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0044-HP");
	iHeapSize = 0x20000;
	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCM16 To PCM16B Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0044::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCM16BOptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0045::CTest_MMF_ACOD_U_0045()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0045-HP");
	iHeapSize = 0x20000;
	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 10000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCM16B Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0045::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0046::CTest_MMF_ACOD_U_0046()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0046-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCM16B Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0046::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0047::CTest_MMF_ACOD_U_0047()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0047-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCM16B Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0047::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0048::CTest_MMF_ACOD_U_0048()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0048-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCM16B Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0048::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCM16BOptimumSrc, KPCM16ToPCM16BOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0049::CTest_MMF_ACOD_U_0049()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0049-HP");
	}

/** @xxxx
 * PCM16 To PCM16B Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0049::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCM16 to PCM16B codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0050::CTest_MMF_ACOD_U_0050()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0050-HP");
	}

/** @xxxx
 * PCM16 To PCMU16B Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2
 */
TVerdict CTest_MMF_ACOD_U_0050::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodePCMU16B));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0051::CTest_MMF_ACOD_U_0051()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0051-HP");
	}

/** @xxxx
 * PCM16 To PCMU16B Instantiate codec by UID
 * @test Req. under test REQ172.6.1
 */
TVerdict CTest_MMF_ACOD_U_0051::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0052::CTest_MMF_ACOD_U_0052()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0052-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 To PCMU16B Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0052::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToPCMU16BOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0053::CTest_MMF_ACOD_U_0053()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0053-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16B Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0053::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU16BOptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0054::CTest_MMF_ACOD_U_0054()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0054-HP");
	iHeapSize = 0x20000;
	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 10000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16B Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0054::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0055::CTest_MMF_ACOD_U_0055()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0055-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16B Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0055::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0056::CTest_MMF_ACOD_U_0056()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0056-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16B Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0056::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0057::CTest_MMF_ACOD_U_0057()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0057-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16B Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0057::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU16BOptimumSrc, KPCM16ToPCMU16BOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0058::CTest_MMF_ACOD_U_0058()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0058-HP");
	}

/** @xxxx
 * PCM16 To PCMU16B Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0058::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCM16 to PCMU16B codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0059::CTest_MMF_ACOD_U_0059()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0059-HP");
	}

/** @xxxx
 * PCM16 To PCMU16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.7
 */
TVerdict CTest_MMF_ACOD_U_0059::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodePCMU16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0060::CTest_MMF_ACOD_U_0060()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0060-HP");
	}

/** @xxxx
 * PCM16 To PCMU16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.7 
 */
TVerdict CTest_MMF_ACOD_U_0060::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0061::CTest_MMF_ACOD_U_0061()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0061-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 To PCMU16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0061::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToPCMU16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0062::CTest_MMF_ACOD_U_0062()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0062-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0062::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0063::CTest_MMF_ACOD_U_0063()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0063-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 10000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0063::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0064::CTest_MMF_ACOD_U_0064()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0064-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0064::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0065::CTest_MMF_ACOD_U_0065()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0065-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0065::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0066::CTest_MMF_ACOD_U_0066()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0066-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0066::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU16OptimumSrc, KPCM16ToPCMU16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0067::CTest_MMF_ACOD_U_0067()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0067-HP");
	}

/** @xxxx
 * PCM16 To PCMU16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0067::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCM16 to PCMU16 codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0068::CTest_MMF_ACOD_U_0068()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0068-HP");
	}

/** @xxxx
 * PCM16 To PCMU8 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.8
 */
TVerdict CTest_MMF_ACOD_U_0068::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodePCMU8));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0069::CTest_MMF_ACOD_U_0069()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0069-HP");
	}

/** @xxxx
 * PCM16 To PCMU8 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.8
 */
TVerdict CTest_MMF_ACOD_U_0069::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0070::CTest_MMF_ACOD_U_0070()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0070-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 25;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 To PCMU8 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0070::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToPCMU8OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0071::CTest_MMF_ACOD_U_0071()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0071-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 100;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU8 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0071::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU8OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0072::CTest_MMF_ACOD_U_0072()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0072-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 5000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU8 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0072::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0073::CTest_MMF_ACOD_U_0073()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0073-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU8 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0073::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0074::CTest_MMF_ACOD_U_0074()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0074-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU8 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0074::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0075::CTest_MMF_ACOD_U_0075()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0075-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To PCMU8 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0075::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU8OptimumSrc, KPCM16ToPCMU8OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0076::CTest_MMF_ACOD_U_0076()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0076-HP");
	}

/** @xxxx
 * PCM16 To PCMU8 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0076::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCM16 to PCMU8 codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//-----------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0077::CTest_MMF_ACOD_U_0077()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0077-HP");
	}

/** @xxxx
 * PCMU16B To PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.9
 */
TVerdict CTest_MMF_ACOD_U_0077::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCMU16B, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0078::CTest_MMF_ACOD_U_0078()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0078-HP");
	}

/** @xxxx
 * PCMU16B To PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.9
 */
TVerdict CTest_MMF_ACOD_U_0078::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0079::CTest_MMF_ACOD_U_0079()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0079-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCMU16B To PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0079::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCMU16BToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0080::CTest_MMF_ACOD_U_0080()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0080-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCMU16B To PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0080::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU16BToPCM16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0081::CTest_MMF_ACOD_U_0081()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0081-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 10000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU16B To PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0081::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0082::CTest_MMF_ACOD_U_0082()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0082-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU16B To PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0082::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0083::CTest_MMF_ACOD_U_0083()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0083-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU16B To PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0083::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0084::CTest_MMF_ACOD_U_0084()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0084-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU16B To PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0084::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU16BToPCM16OptimumSrc, KPCMU16BToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0085::CTest_MMF_ACOD_U_0085()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0085-HP");
	}

/** @xxxx
 * PCMU16B To PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0085::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCMU16B to PCM16 codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0086::CTest_MMF_ACOD_U_0086()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0086-HP");
	}

/** @xxxx
 * PCMU16 To PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.25
 */
TVerdict CTest_MMF_ACOD_U_0086::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCMU16, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0087::CTest_MMF_ACOD_U_0087()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0087-HP");
	}

/** @xxxx
 * PCMU16 To PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.25
 */
TVerdict CTest_MMF_ACOD_U_0087::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0088::CTest_MMF_ACOD_U_0088()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0088-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCMU16 To PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0088::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCMU16ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0089::CTest_MMF_ACOD_U_0089()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0089-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCMU16 To PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0089::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU16ToPCM16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0090::CTest_MMF_ACOD_U_0090()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0090-HP");
	iHeapSize = 0x20000;
	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 10000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU16 To PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0090::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0091::CTest_MMF_ACOD_U_0091()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0091-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU16 To PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0091::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0092::CTest_MMF_ACOD_U_0092()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0092-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU16 To PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0092::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0093::CTest_MMF_ACOD_U_0093()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0093-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU16 To PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0093::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU16ToPCM16OptimumSrc, KPCMU16ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0094::CTest_MMF_ACOD_U_0094()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0094-HP");
	}

/** @xxxx
 * PCMU16 To PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0094::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCMU16 to PCM16 codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0095::CTest_MMF_ACOD_U_0095()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0095-HP");
	}

/** @xxxx
 * PCMU8 To PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.11
 */
TVerdict CTest_MMF_ACOD_U_0095::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCMU8, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0096::CTest_MMF_ACOD_U_0096()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0096-HP");
	}

/** @xxxx
 * PCMU8 To PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.11
 */
TVerdict CTest_MMF_ACOD_U_0096::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0097::CTest_MMF_ACOD_U_0097()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0097-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 100;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCMU8 To PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0097::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCMU8ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0098::CTest_MMF_ACOD_U_0098()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0098-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 25;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCMU8 To PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0098::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU8ToPCM16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0099::CTest_MMF_ACOD_U_0099()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0099-HP");
	iHeapSize = 0x20000;
	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 5000;
		iExpectedDstBytesAdded[i] = 10000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU8 To PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0099::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0100::CTest_MMF_ACOD_U_0100()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0100-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU8 To PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0100::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0101::CTest_MMF_ACOD_U_0101()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0101-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU8 To PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0101::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0102::CTest_MMF_ACOD_U_0102()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0102-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCMU8 To PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0102::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU8ToPCM16OptimumSrc, KPCMU8ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0103::CTest_MMF_ACOD_U_0103()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0103-HP");
	}

/** @xxxx
 * PCMU8 To PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0103::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	INFO_PRINTF1(_L(">> The PCMU8 to PCM16 codec doesn't currently store data between calls to ProcessL"));

	TRAPD(err, testOK = TestRepositionL(codecUid, KPCMU8ToPCM16OptimumSrc, KPCMU8ToPCM16OptimumDst+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0104::CTest_MMF_ACOD_U_0104()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0104-HP");
	}

/** @xxxx
 * MULAW To PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.13
 */
TVerdict CTest_MMF_ACOD_U_0104::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodeMuLAW, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0105::CTest_MMF_ACOD_U_0105()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0105-HP");
	}

/** @xxxx
 * MULAW To PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.13
 */
TVerdict CTest_MMF_ACOD_U_0105::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0106::CTest_MMF_ACOD_U_0106()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0106-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 100;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * MULAW To PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0106::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KMulawToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0107::CTest_MMF_ACOD_U_0107()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0107-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 25;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * MULAW To PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0107::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMulawToPCM16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0108::CTest_MMF_ACOD_U_0108()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0108-HP");
	iHeapSize = 0x20000;
	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 5000;
		iExpectedDstBytesAdded[i] = 10000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * MULAW To PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0108::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0109::CTest_MMF_ACOD_U_0109()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0109-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * MULAW To PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0109::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0110::CTest_MMF_ACOD_U_0110()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0110-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * MULAW To PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0110::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0111::CTest_MMF_ACOD_U_0111()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0111-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x800;
		iExpectedDstBytesAdded[i] = 0x1000;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * MULAW To PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0111::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMulawToPCM16OptimumSrc, KMulawToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0112::CTest_MMF_ACOD_U_0112()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0112-HP");
	}

/** @xxxx
 * MULAW To PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0112::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	INFO_PRINTF1(_L(">> The MULAW To PCM16 codec doesn't currently store data between calls to ProcessL"));

	TRAPD(err, testOK = TestRepositionL(codecUid, KMulawToPCM16OptimumSrc, KMulawToPCM16OptimumDst+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0113::CTest_MMF_ACOD_U_0113()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0113-HP");
	}

/** @xxxx
 * PCM16 To MULAW Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.15
 */
TVerdict CTest_MMF_ACOD_U_0113::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodeMuLAW));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0114::CTest_MMF_ACOD_U_0114()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0114-HP");
	}

/** @xxxx
 * PCM16 To MULAW Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.15
 */
TVerdict CTest_MMF_ACOD_U_0114::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0115::CTest_MMF_ACOD_U_0115()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0115-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 25;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 To MULAW Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0115::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToMulawOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0116::CTest_MMF_ACOD_U_0116()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0116-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 100;
		iExpectedDstBytesAdded[i] = 50;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * PCM16 To MULAW Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0116::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToMulawOptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0117::CTest_MMF_ACOD_U_0117()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0117-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 10000;
		iExpectedDstBytesAdded[i] = 5000;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To MULAW Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0117::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0118::CTest_MMF_ACOD_U_0118()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0118-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To MULAW Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0118::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0119::CTest_MMF_ACOD_U_0119()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0119-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;

		if((i%2)==0)//even i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		else		//odd i
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To MULAW Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0119::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0120::CTest_MMF_ACOD_U_0120()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0120-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x1000;
		iExpectedDstBytesAdded[i] = 0x800;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 To MULAW Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0120::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToMulawOptimumSrc, KPCM16ToMulawOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0121::CTest_MMF_ACOD_U_0121()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0121-HP");
	}

/** @xxxx
 * PCM16 To MULAW Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0121::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	
	INFO_PRINTF1(_L(">> The PCM16 to MULAW codec doesn't currently store data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, KDefaultBufferSize, KDefaultBufferSize+30));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0122::CTest_MMF_ACOD_U_0122()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0122-HP");
	}

/** @xxxx
 * IMAD to PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.12
 */
TVerdict CTest_MMF_ACOD_U_0122::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodeIMAD, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0123::CTest_MMF_ACOD_U_0123()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0123-HP");
	}

/** @xxxx
 * IMAD to PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.12
 */
TVerdict CTest_MMF_ACOD_U_0123::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0124::CTest_MMF_ACOD_U_0124()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0124-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * IMAD to PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0124::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KAdpcmToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0125::CTest_MMF_ACOD_U_0125()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0125-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * IMAD to PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0125::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KAdpcmToPCM16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0126::CTest_MMF_ACOD_U_0126()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0126-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		if(i<4)
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		else		
			iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * IMAD to PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0126::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0127::CTest_MMF_ACOD_U_0127()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0127-HP");

	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EProcessIncomplete;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EProcessIncomplete;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessComplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EProcessIncomplete;	
	}

/** @xxxx
 * IMAD to PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0127::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0128::CTest_MMF_ACOD_U_0128()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0128-HP");

	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EProcessIncomplete;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EProcessIncomplete;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessComplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EProcessIncomplete;	
	}

/** @xxxx
 * IMAD to PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0128::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0129::CTest_MMF_ACOD_U_0129()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0129-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x400;
		iExpectedDstBytesAdded[i] = 0xFC8;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * IMAD to PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0129::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KAdpcmToPCM16OptimumSrc, KAdpcmToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0130::CTest_MMF_ACOD_U_0130()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0130-HP");
	}

/** @xxxx
 * IMAD to PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0130::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	
	TUint src = (KAdpcmToPCM16OptimumSrc/4) + 10;

	INFO_PRINTF1(_L(">> The IMAD to PCM16 codec stores data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, src, KAdpcmToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0131::CTest_MMF_ACOD_U_0131()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0131-HP");
	}

/** @xxxx
 *  PCM16 to IMAD Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.14
 */
TVerdict CTest_MMF_ACOD_U_0131::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodeIMAD));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0132::CTest_MMF_ACOD_U_0132()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0132-HP");
	}

/** @xxxx
 * PCM16 to IMAD Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.14
 */
TVerdict CTest_MMF_ACOD_U_0132::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0133::CTest_MMF_ACOD_U_0133()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0133-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * IMAD to PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0133::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToAdpcmOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0134::CTest_MMF_ACOD_U_0134()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0134-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to IMAD Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0134::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToAdpcmOptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0135::CTest_MMF_ACOD_U_0135()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0135-HP");
	iHeapSize = 0x20000;
	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EDstNotFilled;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EDstNotFilled;	
	}

/** @xxxx
 * PCM16 to IMAD Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0135::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0136::CTest_MMF_ACOD_U_0136()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0136-HP");

	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EDstNotFilled;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EDstNotFilled;
	}

/** @xxxx
 * PCM16 to IMAD Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0136::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0137::CTest_MMF_ACOD_U_0137()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0137-HP");

	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EDstNotFilled;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EDstNotFilled;	
	}

/** @xxxx
 * PCM16 to IMAD Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0137::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0138::CTest_MMF_ACOD_U_0138()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0138-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0xFC8;
		iExpectedDstBytesAdded[i] = 0x400;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 to IMAD Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0138::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToAdpcmOptimumSrc, KPCM16ToAdpcmOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0139::CTest_MMF_ACOD_U_0139()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0139-HP");
	}

/** @xxxx
 * PCM16 to IMAD Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0139::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	
	TUint src = (KPCM16ToAdpcmOptimumSrc/4) + 10;

	INFO_PRINTF1(_L(">> The PCM16 to IMAD codec stores data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, src, KPCM16ToAdpcmOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0140::CTest_MMF_ACOD_U_0140()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0140-HP");
	}

/** @xxxx
 * IMAS to PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.17
 */
TVerdict CTest_MMF_ACOD_U_0140::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodeIMAS, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0141::CTest_MMF_ACOD_U_0141()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0141-HP");
	}

/** @xxxx
 * IMAS to PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.17
 */
TVerdict CTest_MMF_ACOD_U_0141::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0142::CTest_MMF_ACOD_U_0142()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0142-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * IMAS to PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0142::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KStereoAdpcmToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0143::CTest_MMF_ACOD_U_0143()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0143-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * IMAS to PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0143::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KStereoAdpcmToPCM16OptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0144::CTest_MMF_ACOD_U_0144()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0144-HP");
	iHeapSize = 0x20000;
	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EDstNotFilled;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EProcessIncomplete;
	}

/** @xxxx
 * IMAS to PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0144::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0145::CTest_MMF_ACOD_U_0145()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0145-HP");

	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EProcessIncomplete;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EProcessIncomplete;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessComplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EProcessIncomplete;	
	}

/** @xxxx
 * IMAS to PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0145::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0146::CTest_MMF_ACOD_U_0146()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0146-HP");

	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EProcessIncomplete;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EProcessIncomplete;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessComplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EProcessIncomplete;	
	}

/** @xxxx
 * IMAS to PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0146::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0147::CTest_MMF_ACOD_U_0147()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0147-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x400;
		iExpectedDstBytesAdded[i] = 0xF90;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * IMAS to PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0147::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KStereoAdpcmToPCM16OptimumSrc, KStereoAdpcmToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0148::CTest_MMF_ACOD_U_0148()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0148-HP");
	}

/** @xxxx
 * IMAS to PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0148::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	
	TUint src = (KStereoAdpcmToPCM16OptimumSrc/4) + 10;

	INFO_PRINTF1(_L(">> The IMAS to PCM16 codec stores data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, src, KStereoAdpcmToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0149::CTest_MMF_ACOD_U_0149()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0149-HP");
	}

/** @xxxx
 *  PCM16 to IMAS Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.18
 */
TVerdict CTest_MMF_ACOD_U_0149::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodeIMAS));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0150::CTest_MMF_ACOD_U_0150()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0150-HP");
	}

/** @xxxx
 * PCM16 to IMAS Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.18
 */
TVerdict CTest_MMF_ACOD_U_0150::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0151::CTest_MMF_ACOD_U_0151()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0151-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 50;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * IMAS to PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0151::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSize, KPCM16ToStereoAdpcmOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0152::CTest_MMF_ACOD_U_0152()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0152-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to IMAS Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0152::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToStereoAdpcmOptimumSrc, KSmallBufferSize));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0153::CTest_MMF_ACOD_U_0153()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0153-HP");
	iHeapSize = 0x20000;
	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EDstNotFilled;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessIncomplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EDstNotFilled;	
	}

/** @xxxx
 * PCM16 to IMAS Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0153::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0154::CTest_MMF_ACOD_U_0154()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0154-HP");

	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EDstNotFilled;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessComplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EDstNotFilled;
	}

/** @xxxx
 * PCM16 to IMAS Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0154::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0155::CTest_MMF_ACOD_U_0155()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0155-HP");

	iExpectedReturnValue[0].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[1].iStatus = TCodecProcessResult::EDstNotFilled;
	iExpectedReturnValue[2].iStatus = TCodecProcessResult::EDstNotFilled;	
	iExpectedReturnValue[3].iStatus = TCodecProcessResult::EProcessComplete;
	iExpectedReturnValue[4].iStatus = TCodecProcessResult::EDstNotFilled;	
	}

/** @xxxx
 * PCM16 to IMAS Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0155::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0156::CTest_MMF_ACOD_U_0156()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0156-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0xF90;
		iExpectedDstBytesAdded[i] = 0x400;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 to IMAS Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0156::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToStereoAdpcmOptimumSrc, KPCM16ToStereoAdpcmOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0157::CTest_MMF_ACOD_U_0157()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0157-HP");
	}

/** @xxxx
 * PCM16 to IMAS Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0157::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	
	TUint src = (KPCM16ToStereoAdpcmOptimumSrc/4) + 10;

	INFO_PRINTF1(_L(">> The PCM16 to IMAS codec stores data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, src, KPCM16ToStereoAdpcmOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0158::CTest_MMF_ACOD_U_0158()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0158-HP");
	}

/** @xxxx
 * GSM610 to PCM16 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.19
 */
TVerdict CTest_MMF_ACOD_U_0158::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodeGSM610, KMMFFourCCCodePCM16));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0159::CTest_MMF_ACOD_U_0159()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0159-HP");
	}

/** @xxxx
 * GSM610 to PCM16 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.19
 */
TVerdict CTest_MMF_ACOD_U_0159::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0160::CTest_MMF_ACOD_U_0160()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0160-HP");
	iHeapSize = 0x200000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}

	}

/** @xxxx
 * GSM610 to PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0160::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSizeGSM, KGSM610ToPCM16OptimumDstGSM, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0161::CTest_MMF_ACOD_U_0161()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0161-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * GSM610 to PCM16 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0161::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KGSM610ToPCM16OptimumSrcGSM, KSmallBufferSizeGSM, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0162::CTest_MMF_ACOD_U_0162()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0162-HP");
	iHeapSize = 0x20000;
	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * GSM610 to PCM16 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0162::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0163::CTest_MMF_ACOD_U_0163()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0163-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * GSM610 to PCM16 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0163::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0164::CTest_MMF_ACOD_U_0164()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0164-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessIncomplete;
		}
	}

/** @xxxx
 * GSM610 to PCM16 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0164::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0165::CTest_MMF_ACOD_U_0165()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0165-HP");
	iSingleIteration = ETrue;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0x104;
		iExpectedDstBytesAdded[i] = 0xA00;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * GSM610 to PCM16 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0165::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KGSM610ToPCM16OptimumSrc, KGSM610ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0166::CTest_MMF_ACOD_U_0166()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0166-HP");
	}

/** @xxxx
 * GSM610 to PCM16 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0166::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	
	TUint src = (KGSM610ToPCM16OptimumSrc/4) + 10;

	INFO_PRINTF1(_L(">> The GSM610 to PCM16 codec stores data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, src, KGSM610ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0167::CTest_MMF_ACOD_U_0167()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0167-HP");
	}

/** @xxxx
 *  PCM16 to GSM610 Instantiate codec by FourCC Codes
 * @test Req. under test REQ172.6.2, REQ172.11.20
 */
TVerdict CTest_MMF_ACOD_U_0167::DoTestStepL()
	{
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(KMMFFourCCCodePCM16, KMMFFourCCCodeGSM610));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0168::CTest_MMF_ACOD_U_0168()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0168-HP");
	}

/** @xxxx
 * PCM16 to GSM610 Instantiate codec by UID
 * @test Req. under test REQ172.6.1, REQ172.11.20
 */
TVerdict CTest_MMF_ACOD_U_0168::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestNewL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0169::CTest_MMF_ACOD_U_0169()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0169-HP");
	iHeapSize = 0x20000;

	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * GSM610 to PCM16 Pass in small source buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0169::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KSmallBufferSizeGSM, KPCM16ToGSM610OptimumDstGSM, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0170::CTest_MMF_ACOD_U_0170()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0170-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to GSM610 Pass in small dest buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0170::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToGSM610OptimumSrc, KSmallBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0171::CTest_MMF_ACOD_U_0171()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0171-HP");
	iHeapSize = 0x20000;
	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}	
	}

/** @xxxx
 * PCM16 to GSM610 Pass in large buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0171::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KLargeBufferSize, KLargeBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0172::CTest_MMF_ACOD_U_0172()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0172-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to GSM610 Pass in default sized buffers
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0172::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KDefaultBufferSize, KDefaultBufferSize, 0, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0173::CTest_MMF_ACOD_U_0173()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0173-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to GSM610 Pass in buffers with different max length and length
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0173::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMaxBufferLength, KDefaultBufferSize, KDefaultBufferSize, EFalse));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0174::CTest_MMF_ACOD_U_0174()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0174-HP");
	iSingleIteration = ETrue;

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0xA00;
		iExpectedDstBytesAdded[i] = 0x104;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EProcessComplete;
		}
	}

/** @xxxx
 * PCM16 to GSM610 Pass in buffers of optimum size
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0174::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToGSM610OptimumSrc, KPCM16ToGSM610OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0175::CTest_MMF_ACOD_U_0175()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0175-HP");
	}

/** @xxxx
 * PCM16 to GSM610 Reposition source pointer during conversion
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0175::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	
	TUint src = (KPCM16ToGSM610OptimumSrc/4) + 10;

	INFO_PRINTF1(_L(">> The PCM16 to GSM610 codec stores data between calls to ProcessL"));
	TRAPD(err, testOK = TestRepositionL(codecUid, src, KPCM16ToGSM610OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0176::CTest_MMF_ACOD_U_0176()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0176-HP");
	}

/** @xxxx
 * Instantiate codec that doesn't exist
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0176::DoTestStepL()
	{
//	CMMFCodec* codec = NULL;
//	TRAPD(err, codec = CMMFCodec::NewL(KMMFFourCCCodeALAW, KMMFFourCCCodePCMU16B));
	TRAPD(err, CMMFCodec::NewL(KMMFFourCCCodeALAW, KMMFFourCCCodePCMU16B));	// EABI warning removal - "variable was set but never used"

	if (err == KErrNone || err == KErrNotSupported)
		return EPass;
	else 
		{
		ERR_PRINTF2(_L(">> CTest_MMF_ACOD_U_0176::DoTestStepL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	}
//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0177::CTest_MMF_ACOD_U_0177()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0177-HP");
	}

/** @xxxx
 * Instantiate codec from fourCC codes that don't exist
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0177::DoTestStepL()
	{
	const TUint32 KFourCCCodeP1B = 0x42315020;		//(' ', 'P', '1', 'B')
	const TUint32 KFourCCCodePS16 = 0x36315350;		//('P', 'S', '1', '6')

//	CMMFCodec* codec = NULL;
//	TRAPD(err, codec = CMMFCodec::NewL(KFourCCCodeP1B, KFourCCCodePS16));
	TRAPD(err, CMMFCodec::NewL(KFourCCCodeP1B, KFourCCCodePS16));	// EABI warning removal "variable was set but never used"

	if (err == KErrNone || err == KErrNotSupported)
		return EPass;
	else 
		{
		ERR_PRINTF2(_L(">> CTest_MMF_ACOD_U_0177::DoTestStepL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	}
//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0178::CTest_MMF_ACOD_U_0178()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0178-HP");
	}

/** @xxxx
 * Test out of memory in NewL
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0178::DoTestStepL()
	{
//	CMMFCodec* codec = NULL;
	__UHEAP_FAILNEXT(1);
//	TRAPD(err, codec = CMMFCodec::NewL(KMMFFourCCCodePCMU8, KMMFFourCCCodePCM16));
	TRAPD(err, CMMFCodec::NewL(KMMFFourCCCodePCMU8, KMMFFourCCCodePCM16));	// EABI warning removal - "variable was set but never used"

	if (err == KErrNoMemory)
		return EPass;
	else 
		{
		ERR_PRINTF2(_L(">> CTest_MMF_ACOD_U_0178::DoTestStepL Leave occurred in NewL, error code %d"), err);
		return EFail;
		}
	}
//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0179::CTest_MMF_ACOD_U_0179()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0179-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * ALAW To PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0179::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KAlawToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0180::CTest_MMF_ACOD_U_0180()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0180-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * ALAW To PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0180::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KAlawToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0181::CTest_MMF_ACOD_U_0181()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0181-HP");
	}

/** @xxxx
 * ALAW To PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0181::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0182::CTest_MMF_ACOD_U_0182()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0182-HP");
	}

/** @xxxx
 * ALAW To PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0182::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0183::CTest_MMF_ACOD_U_0183()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0183-HP");
	}

/** @xxxx
 * ALAW To PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0183::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAlawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0184::CTest_MMF_ACOD_U_0184()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0184-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to ALAW Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0184::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToAlawOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0185::CTest_MMF_ACOD_U_0185()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0185-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to ALAW Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0185::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToAlawOptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0186::CTest_MMF_ACOD_U_0186()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0186-HP");
	}

/** @xxxx
 * PCM16 to ALAW Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0186::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0187::CTest_MMF_ACOD_U_0187()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0187-HP");
	}

/** @xxxx
 * PCM16 to ALAW Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0187::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0188::CTest_MMF_ACOD_U_0188()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0188-HP");
	}

/** @xxxx
 * PCM16 to ALAW Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0188::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToAlaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0189::CTest_MMF_ACOD_U_0189()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0189-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM8 To PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0189::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM8ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0190::CTest_MMF_ACOD_U_0190()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0190-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM8 To PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0190::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM8ToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0191::CTest_MMF_ACOD_U_0191()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0191-HP");
	}

/** @xxxx
 * PCM8 To PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0191::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0192::CTest_MMF_ACOD_U_0192()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0192-HP");
	}

/** @xxxx
 * PCM8 To PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0192::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0193::CTest_MMF_ACOD_U_0193()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0193-HP");
	}

/** @xxxx
 * PCM8 To PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0193::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM8ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0194::CTest_MMF_ACOD_U_0194()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0194-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to PCM8 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0194::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToPCM8OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0195::CTest_MMF_ACOD_U_0195()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0195-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to PCM8 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0195::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCM8OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0196::CTest_MMF_ACOD_U_0196()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0196-HP");
	}

/** @xxxx
 * PCM16 to PCM8 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0196::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0197::CTest_MMF_ACOD_U_0197()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0197-HP");
	}

/** @xxxx
 * PCM16 to PCM8 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0197::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0198::CTest_MMF_ACOD_U_0198()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0198-HP");
	}

/** @xxxx
 * PCM16 to PCM8 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0198::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMS8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0199::CTest_MMF_ACOD_U_0199()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0199-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * Swap Endian Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0199::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToPCM16BOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0200::CTest_MMF_ACOD_U_0200()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0200-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * Swap Endian Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0200::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCM16BOptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0201::CTest_MMF_ACOD_U_0201()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0201-HP");
	}

/** @xxxx
 * Swap Endian Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0201::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0202::CTest_MMF_ACOD_U_0202()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0202-HP");
	}

/** @xxxx
 * Swap Endian Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0202::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0203::CTest_MMF_ACOD_U_0203()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0203-HP");
	}

/** @xxxx
 * Swap Endian Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0203::DoTestStepL()
	{
	//const TUid codecUid = {KMMFUidCodecPCM16SwapEndian};
	const TUid codecUid = {KMMFUidCodecPCM16toPCM16B};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0204::CTest_MMF_ACOD_U_0204()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0204-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to PCMU16BE Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0204::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToPCMU16BOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0205::CTest_MMF_ACOD_U_0205()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0205-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to PCMU16BE Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0205::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU16BOptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0206::CTest_MMF_ACOD_U_0206()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0206-HP");
	}

/** @xxxx
 * PCM16 to PCMU16BE Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0206::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0207::CTest_MMF_ACOD_U_0207()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0207-HP");
	}

/** @xxxx
 * PCM16 to PCMU16BE Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0207::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0208::CTest_MMF_ACOD_U_0208()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0208-HP");
	}

/** @xxxx
 * PCM16 to PCMU16BE Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0208::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16BE};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0209::CTest_MMF_ACOD_U_0209()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0209-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to PCMU16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0209::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToPCMU16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0210::CTest_MMF_ACOD_U_0210()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0210-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to PCMU16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0210::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0211::CTest_MMF_ACOD_U_0211()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0211-HP");
	}

/** @xxxx
 * PCM16 to PCMU16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0211::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0212::CTest_MMF_ACOD_U_0212()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0212-HP");
	}

/** @xxxx
 * PCM16 to PCMU16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0212::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0213::CTest_MMF_ACOD_U_0213()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0213-HP");
	}

/** @xxxx
 * PCM16 to PCMU16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0213::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------
/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0214::CTest_MMF_ACOD_U_0214()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0214-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to PCMU8 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0214::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToPCMU8OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0215::CTest_MMF_ACOD_U_0215()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0215-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to PCMU8 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0215::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToPCMU8OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0216::CTest_MMF_ACOD_U_0216()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0216-HP");
	}

/** @xxxx
 * PCM16 to PCMU8 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0216::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0217::CTest_MMF_ACOD_U_0217()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0217-HP");
	}

/** @xxxx
 * PCM16 to PCMU8 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0217::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0218::CTest_MMF_ACOD_U_0218()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0218-HP");
	}

/** @xxxx
 * PCM16 to PCMU8 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0218::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioS16ToPCMU8};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0219::CTest_MMF_ACOD_U_0219()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0219-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCMU16BE to PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0219::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCMU16BToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0220::CTest_MMF_ACOD_U_0220()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0220-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCMU16BE to PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0220::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU16BToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0221::CTest_MMF_ACOD_U_0221()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0221-HP");
	}

/** @xxxx
 * PCMU16BE to PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0221::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0222::CTest_MMF_ACOD_U_0222()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0222-HP");
	}

/** @xxxx
 * PCMU16BE to PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0222::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0223::CTest_MMF_ACOD_U_0223()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0223-HP");
	}

/** @xxxx
 * PCMU16BE to PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0223::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16BEToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0224::CTest_MMF_ACOD_U_0224()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0224-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCMU16 to PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0224::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCMU16ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0225::CTest_MMF_ACOD_U_0225()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0225-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCMU16 to PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0225::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU16ToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0226::CTest_MMF_ACOD_U_0226()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0226-HP");
	}

/** @xxxx
 * PCMU16 to PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0226::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0227::CTest_MMF_ACOD_U_0227()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0227-HP");
	}

/** @xxxx
 * PCMU16 to PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0227::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0228::CTest_MMF_ACOD_U_0228()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0228-HP");
	}

/** @xxxx
 * PCMU16 to PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0228::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU16ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0229::CTest_MMF_ACOD_U_0229()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0229-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCMU8 To PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0229::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCMU8ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0230::CTest_MMF_ACOD_U_0230()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0230-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCMU8 To PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0230::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCMU8ToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0231::CTest_MMF_ACOD_U_0231()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0231-HP");
	}

/** @xxxx
 * PCMU8 To PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0231::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0232::CTest_MMF_ACOD_U_0232()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0232-HP");
	}

/** @xxxx
 * PCMU8 To PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0232::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0233::CTest_MMF_ACOD_U_0233()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0233-HP");
	}

/** @xxxx
 * PCMU8 To PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0233::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecAudioU8ToPCMS16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0234::CTest_MMF_ACOD_U_0234()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0234-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * MULAW To PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0234::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KMulawToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0235::CTest_MMF_ACOD_U_0235()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0235-HP");
	
	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * MULAW To PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0235::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KMulawToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0236::CTest_MMF_ACOD_U_0236()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0236-HP");
	}

/** @xxxx
 * MULAW To PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0236::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0237::CTest_MMF_ACOD_U_0237()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0237-HP");
	}

/** @xxxx
 * MULAW To PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0237::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0238::CTest_MMF_ACOD_U_0238()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0238-HP");
	}

/** @xxxx
 * MULAW To PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0238::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecMulawToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0239::CTest_MMF_ACOD_U_0239()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0239-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to MULAW Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0239::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToMulawOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0240::CTest_MMF_ACOD_U_0240()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0240-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to MULAW Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0240::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToMulawOptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0241::CTest_MMF_ACOD_U_0241()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0241-HP");
	}

/** @xxxx
 * PCM16 to MULAW Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0241::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0242::CTest_MMF_ACOD_U_0242()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0242-HP");
	}

/** @xxxx
 * PCM16 to MULAW Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0242::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0243::CTest_MMF_ACOD_U_0243()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0243-HP");
	}

/** @xxxx
 * PCM16 to MULAW Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0243::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToMulaw};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0244::CTest_MMF_ACOD_U_0244()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0244-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * IMAD To PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0244::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KAdpcmToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0245::CTest_MMF_ACOD_U_0245()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0245-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * IMAD To PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0245::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KAdpcmToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0246::CTest_MMF_ACOD_U_0246()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0246-HP");
	}

/** @xxxx
 * IMAD To PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0246::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0247::CTest_MMF_ACOD_U_0247()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0247-HP");
	}

/** @xxxx
 * IMAD To PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0247::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0248::CTest_MMF_ACOD_U_0248()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0248-HP");
	}

/** @xxxx
 * IMAD To PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0248::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0249::CTest_MMF_ACOD_U_0249()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0249-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to IMAD Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0249::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToAdpcmOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0250::CTest_MMF_ACOD_U_0250()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0250-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to IMAD Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0250::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToAdpcmOptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0251::CTest_MMF_ACOD_U_0251()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0251-HP");
	}

/** @xxxx
 * PCM16 to IMAD Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0251::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0252::CTest_MMF_ACOD_U_0252()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0252-HP");
	}

/** @xxxx
 * PCM16 to IMAD Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0252::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0253::CTest_MMF_ACOD_U_0253()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0253-HP");
	}

/** @xxxx
 * PCM16 to IMAD Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0253::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCM};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0254::CTest_MMF_ACOD_U_0254()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0254-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * IMAS To PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0254::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KStereoAdpcmToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0255::CTest_MMF_ACOD_U_0255()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0255-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * IMAS To PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0255::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KStereoAdpcmToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0256::CTest_MMF_ACOD_U_0256()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0256-HP");
	}

/** @xxxx
 * IMAS To PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0256::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0257::CTest_MMF_ACOD_U_0257()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0257-HP");
	}

/** @xxxx
 * IMAS To PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0257::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0258::CTest_MMF_ACOD_U_0258()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0258-HP");
	}

/** @xxxx
 * IMAS To PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0258::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecIMAADPCMToPCM16Stereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0259::CTest_MMF_ACOD_U_0259()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0259-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to IMAS Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0259::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToStereoAdpcmOptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0260::CTest_MMF_ACOD_U_0260()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0260-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to IMAS Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0260::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToStereoAdpcmOptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0261::CTest_MMF_ACOD_U_0261()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0261-HP");
	}

/** @xxxx
 * PCM16 to IMAS Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0261::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0262::CTest_MMF_ACOD_U_0262()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0262-HP");
	}

/** @xxxx
 * PCM16 to IMAS Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0262::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0263::CTest_MMF_ACOD_U_0263()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0263-HP");
	}

/** @xxxx
 * PCM16 to IMAS Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0263::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToIMAADPCMStereo};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0264::CTest_MMF_ACOD_U_0264()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0264-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * GSM610 To PCM16 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0264::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KGSM610ToPCM16OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0265::CTest_MMF_ACOD_U_0265()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0265-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * GSM610 To PCM16 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0265::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KGSM610ToPCM16OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0266::CTest_MMF_ACOD_U_0266()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0266-HP");
	}

/** @xxxx
 * GSM610 To PCM16 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0266::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0267::CTest_MMF_ACOD_U_0267()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0267-HP");
	}

/** @xxxx
 * GSM610 To PCM16 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0267::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0268::CTest_MMF_ACOD_U_0268()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0268-HP");
	}

/** @xxxx
 * GSM610 To PCM16 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0268::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecGSM610ToPCM16};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0269::CTest_MMF_ACOD_U_0269()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0269-HP");

	for (TUint i=0; i<5; i++)
		{
		iExpectedSrcBytesProcessed[i] = 0;
		iExpectedDstBytesAdded[i] = 0;
		iExpectedReturnValue[i].iStatus = TCodecProcessResult::EDstNotFilled;
		}
	}

/** @xxxx
 * PCM16 to GSM610 Pass in zero sized source buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0269::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, 0, KPCM16ToGSM610OptimumDst));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0270::CTest_MMF_ACOD_U_0270()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0270-HP");

	iExpectedLeaveErrorCode = KErrArgument;
	}

/** @xxxx
 * PCM16 to GSM610 Pass in zero sized destination buffer
 * @test Req. under test REQ172.6.4, REQ172.6.5
 */
TVerdict CTest_MMF_ACOD_U_0270::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestProcessLLoopFiveTimesL(codecUid, KPCM16ToGSM610OptimumSrc, 0));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0271::CTest_MMF_ACOD_U_0271()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0271-HP");
	}

/** @xxxx
 * PCM16 to GSM610 Test out of memory
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0271::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;

	TRAPD(err, testOK = TestNewLOutOfMemoryL(codecUid));
	if (err==KErrNone && testOK)
		TRAP(err, testOK = TestProcessLOutOfMemoryL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------


/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0272::CTest_MMF_ACOD_U_0272()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0272-HP");
	}

/** @xxxx
 * PCM16 to GSM610 Test for memory leaks
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0272::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryLeakL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

/** @xxxx
 * Constructor
 */
CTest_MMF_ACOD_U_0273::CTest_MMF_ACOD_U_0273()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = _L("MM-MMF-ACOD-U-0273-HP");
	}

/** @xxxx
 * PCM16 to GSM610 Test for memory scribbles
 * @test Req. under test REQ172.11
 */
TVerdict CTest_MMF_ACOD_U_0273::DoTestStepL()
	{
	const TUid codecUid = {KMmfUidCodecPCM16ToGSM610};
	TBool testOK = EFalse;
	TRAPD(err, testOK = TestMemoryScribbleL(codecUid));

	if (err)
		{
		INFO_PRINTF2(_L("Test left with status %d"), err);
		return EFail;
		}
	else if (!testOK)
		{
		INFO_PRINTF1(_L("Test failed"));
		return EFail; 
		}
	else
		return EPass; 
	}

//------------------------------------------------------------------

