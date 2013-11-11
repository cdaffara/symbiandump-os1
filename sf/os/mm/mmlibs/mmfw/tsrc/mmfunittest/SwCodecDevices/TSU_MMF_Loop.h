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
//

#ifndef __TSU_MMF_LOOP_H_
#define __TSU_MMF_LOOP_H_

#include <testframework.h>
#include "TSU_MMF_ACOD.h"
#include <mmf/server/mmfswcodecwrapper.h>
#include "MMFAudioCodecBase.h"

/**
*
* class CTestStepLoopTest
* This class is responsible for testing the 
* preconditions on the ProcessL method of the codecs.
*
*/
template <class Codec>
class CTestStepLoopTest : public CTestStep_MMF_SwCodecDevices
	{
public:
	CTestStepLoopTest( TUint aTestIndex );
	virtual ~CTestStepLoopTest();
	virtual enum TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict DoTestStepPostambleL(void);
private:
	TInt CodecPreconditionTestL( TInt aSrcDataBufferSize, 
		                        TInt aSinkDataBufferSize,
								TInt aSrcPosition = 0,
								TInt aSinkPosition = 0 );
private:
	CMMFSwCodec*     iCodecUnderTest; // sw codec under test
	CMMFDataBuffer*  iSourceData;     // reference source data buffer
	CMMFDataBuffer*  iCodedData;      // coded data buffer
	};

#endif /* TSU_MMF_Loop_H_ */

