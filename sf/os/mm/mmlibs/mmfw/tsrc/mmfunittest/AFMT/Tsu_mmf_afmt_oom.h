
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

#ifndef __TSU_MMF_AFMT_OOM_H__
#define __TSU_MMF_AFMT_OOM_H__

#include <testframework.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatasink.h>



// This class is a helper class for all Audio Format OutOfMemory unit tests.
class CTestStep_MMF_AFMT_OOM : public CTestStep_MMF_AFMT
	{
public: // Constructor/Destructor
	CTestStep_MMF_AFMT_OOM(){}
	virtual ~CTestStep_MMF_AFMT_OOM(){}

protected:
	/**
	 * Setups up member variables before running a test, then runs all four
	 * NewL functions for a given CMmfXXXFormat object (as determined by
	 * the aUid).
	 * @param aUid - Specifies which object to run the tests on.
	 * @TVerdict EPass if all tests passed, else EFail.
	 */
	TVerdict RunAllOomTestsL(const TUint& aUid);	

private:
	/**
	 * This function runs the Out-of-memory test for the specified NewL function.
	 * @param aNewL - Specifies which NewL function to call.
	 * @return TBool Returns ETrue if everything went Ok, EFalse if there was some
	 * error.
	 */
	TBool RunOomTestL(const ENewLType& aNewL);


	/** 
	 * This function runs the correct NewL on the correct object.
	 * @param aNewL - Enum inicating which NewL function should be called.
	 */
	void RunNewL(const ENewLType& aNewL);


	/** 
	 * This function sets up the variables required for a FormatDecode NewL call.
	 * @param aBool - 'ETrue' if DataSource is file based, 'EFalse' if Descriptor based.
	 * @param aNewL - Enum inicating which NewL function should be called.
	 * @param aFilename - The filename containing the audio data.
	 * @param aUid - Only required if aNewL is KNewLUid.
	 */
	void SetupNewL( const TBool& aBool, 
					const ENewLType& aNewL, 
					const TDesC& aFilename, 
					const TInt& aUid=0);	
	};


#endif
