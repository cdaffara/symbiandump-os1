// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TESTMIDICLIENTSHAREDHEAP_H
#define TESTMIDICLIENTSHAREDHEAP_H

#include "TSU_MMFMIDICLNT.h"


/**
 * Loads and initialises multiple midi utilities.
 *
 * @class CTestMidiClntMultiSharedHeap
 *
 * Req. under test 
 */
class CTestMidiClntMultiSharedHeap : public CTestMmfMidiClntStep
	{
public:
	CTestMidiClntMultiSharedHeap(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TBool aMixHeapStyle );
	~CTestMidiClntMultiSharedHeap() {};
	static CTestMidiClntMultiSharedHeap* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TBool aMixHeapStyle );
	// From Base Class
	TVerdict DoTestStepL();

protected:
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of Audio file
	TBool iMixHeapStyle;					// Should a mixture of shared and separate heap controllers be created?
	};


/**
 * Repeatedly creates and deletes multiple midi utilities
 *
 * @class CTestMidiClntRepeatMultiSharedHeap
 *
 * Req. under test 
 */
class CTestMidiClntRepeatMultiSharedHeap : public CTestMmfMidiClntStep
	{
public:
	CTestMidiClntRepeatMultiSharedHeap(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TBool aMixHeapStyle );
	~CTestMidiClntRepeatMultiSharedHeap() {};
	static CTestMidiClntRepeatMultiSharedHeap* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TBool aMixHeapStyle );
	// From Base Class
	TVerdict DoTestStepL();

protected:
	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of Audio file
	TBool iMixHeapStyle;					// Should a mixture of shared and separate heap controllers be created?
	};

	
#endif // TESTMIDICLIENTSHAREDHEAP_H
