
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
// Header file: Basic  tests.
// 
//

/**
 @file OpenFileByHandlePlayTextFile.h
*/

#ifndef OPENFILEBYHANDLEPlayTextFile_H__
#define OPENFILEBYHANDLEPlayTextFile_H__

#include "TSI_MMFVCLNT.h"

/**
 * Play Video file
 * 
 * @class CTestMmfVclntOpenFilePlayTextFile
 *
 */
class CTestMmfVclntOpenFilePlayTextFile : public CTestMmfVclntStep , public MVideoPlayerUtilityObserver
	{
public:
	static CTestMmfVclntOpenFilePlayTextFile* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aPlay);
	static CTestMmfVclntOpenFilePlayTextFile* NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aPlay);
	virtual TVerdict DoTestStepL();
	// MVideoPlayerUtilityObserver
	virtual void MvpuoOpenComplete(TInt aError);
	virtual void MvpuoPrepareComplete(TInt aError);
	virtual void MvpuoFrameReady(CFbsBitmap& aFrame, TInt aError);
	virtual void MvpuoPlayComplete(TInt aError);
	virtual void MvpuoEvent(const TMMFEvent& aEvent);
protected:
	CTestMmfVclntOpenFilePlayTextFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aPlay);
	TVerdict PerformTestStepL();

	TBuf<KNameBufSize> iSectName;			// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;			// Key name for retrieving filename
	TInt iError;
	const TBool iPlay;						// Does this test require playing the video
	TTimeIntervalMicroSeconds iDuration;	// Stores duration of video
	};

#endif	// OPENFILEBYHANDLEPlayTextFile_H__
