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

#include <mmf/server/mmffile.h>						// for TMMFFileParams and CMMFFile
#include <mmf/server/mmfformat.h>						// For CMMFFormatDecode
#include <mmf/server/mmfdes.h>							// for TMMFDescriptorParams and CMMFDescriptor
#include <mmf/server/mmfdatasource.h> 
#include <mmf/plugin/mmfformatimplementationuids.hrh>	// for KMmfUidFormatWAVRead UIDs

//#include "mmfwavFormat.h"					// for KGSMAvgBytesPerSecond

// Test system includes
#include "TSU_MMF_AFMT.h"
#include "Tsu_mmf_afmt_Duration.h"
#include "TSU_MMF_AFMTSuite.h"


#include <mda/common/audio.hrh>
#include <mda/common/gsmaudio.hrh>						// for  KUidMdaGsmWavCodecDefine def


// Due to rounding errors, header sizes etc, it can be difficult to accurately work out
// the duration of a sound clip.  These tests shall use +- a boundary value to check
// that the returned value is within a give range.
const TUint KDurationBounds = 50000;  // 50 milliseconds. 




	
/**
 * This function runs the duration test.  You should call SetupL of the base
 * class before running this function to set up the test file and test objects.
 * @param aDuration - The expected duration of the test file.
 * @return TVerdict EPass on success, else EFail.
 */
TVerdict CTestStep_MMF_AFMT_DurationL::RunDurationTest( const TInt64& aDuration )
	{
	TVerdict verdict;
	TTimeIntervalMicroSeconds intval;

	// Make the call to the CMmfXXXFormat object to check the frame time interval.
	// Which object we use depends on whether we are using an Encode/Decode object.
	// These objects are setup by calling SetupL(..) in the baseclass.
	if (iDecode) 
		{	
		intval = iFormatDec->Duration(KUidMediaTypeAudio);
		}
	else 
		{
		intval = iFormatEnc->Duration(KUidMediaTypeAudio);
		}

	TInt64 i64 = intval.Int64();

	// Check if the returned interval is above the lower and below the upper
	// bounds check values - if so, it's a pass.
	if (i64 > (aDuration-KDurationBounds) && // lower bounds check
		i64 < (aDuration+KDurationBounds) )	 // upper bounds check
		{
		verdict = EPass;
		}
	else
		{
		verdict = EFail;
		}

	return verdict;
	}

	
