
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

#ifndef __TSU_MMF_AFMT_NEWL_H__
#define __TSU_MMF_AFMT_NEWL_H__

#include <testframework.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatasink.h>

// This class is a helper class for all Audio Format NewL unit tests.
class CTestStep_MMF_AFMT_NewL : public CTestStep_MMF_AFMT
	{
public: // Constructor/Destructor
	CTestStep_MMF_AFMT_NewL(){}
	virtual ~CTestStep_MMF_AFMT_NewL(){}

	
protected: // New functions 
	
	/**
	 * Initialises the member variables required for the NewL tests using CMmfDescriptors as the 
	 * source to pass to the NewL call.
	 * @param aNewL - The NewL call to make
	 * @param aFilename - The filename to read the data from for the Descriptor
	 * @param aUid - The UID of the Format (Wav, Au or Raw)
	 */
	void InitialiseDescriptorTestsL(const ENewLType& aNewL, 
									const TDesC& aFilename, 
									const TInt& aUid);

	/**
	 * Initialises the member variables required for the NewL tests using CMmfFile as the 
	 * source to pass to the NewL call.
	 * @param aNewL - The NewL call to make
	 * @param aFilename - The filename to read the data from for the Descriptor
	 * @param aUid - The UID of the Format (Wav, Au or Raw)
	 */
	void InitialiseFileTestsL(	const ENewLType& aNewL, 
								const TDesC& aFilename, 
								const TInt& aUid);

	
	/** 
	 * This function verifies that the iFormat handle is valid, and that the
	 * format of the object has been set up correctly.
	 * @param aUid - The UID that DataSourceType() should match
	 * @param aChannels - The number of channels that NumChannels() should match
	 * @param aSampRate - The Sample Rate that SampleRate() should match
	 * @param aFourCC - The FourCC code that SourceDataTypeCode() should match.
	 * @return TVerdict - EPass if all data is as expected, else EFail.
	 */
	TVerdict VerifyDecodeNewL(	const TUid& aUid , 
								const TUint& aChannels, 
								const TUint& aSampRate, 
								const TFourCC& aFourCC);

	/** 
	 * This function verifies that the iFormat handle is valid, and that the
	 * format of the object has been set up correctly.
	 * @param aUid - The UID that DataSinkType() should match
	 * @param aChannels - The number of channels that NumChannels() should match
	 * @param aSampRate - The Sample Rate that SampleRate() should match
	 * @param aFourCC - The FourCC code that SinkDataTypeCode() should match.
	 * @return TVerdict - EPass if all data is as expected, else EFail.
	 */
	TVerdict VerifyEncodeNewL(	const TUid& aUid , 
								const TUint& aChannels, 
								const TUint& aSampRate, 
								const TFourCC& aFourCC);


	/** 
	 * This function makes the specified NewL call for a CMmfXXXFormatDecode object
	 * @param aNewL - Which NewL function to call
	 */
	void RunDecodeNewL( ENewLType aNewL = KNewLUndefined );


	/** 
	 * This function makes the specified NewL call for a CMmfXXXFormatEncode object
	 * @param aNewL - Which NewL function to call
	 */
	void RunEncodeNewL( ENewLType aNewL = KNewLUndefined );


	/** 
	 * This function sets up the variables required for a FormatDecode NewL call.
	 * @param aIsFile - 'ETrue' if DataSource is file based, 'EFalse' if Descriptor based.
	 * @param aNewL - Enum inicating which NewL function should be called.
	 * @param aFilename - The filename containing the audio data.
	 * @param aUid - Only required if aNewL is KNewLUid.
	 */
	void SetupNewL( TBool aIsFile, const ENewLType& aNewL, 
					const TDesC& aFilename, const TInt& aUid);

	};



#endif
