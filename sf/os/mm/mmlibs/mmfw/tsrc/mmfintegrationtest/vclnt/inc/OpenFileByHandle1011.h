
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
 @file OpenFileByHandle1011.h
*/

#ifndef OPENFILEBYHANDLE1011_H__
#define OPENFILEBYHANDLE1011_H__

#include "TSI_MMFVCLNT.h"

/**
 * Play Video file
 * 
 * @class CTestMmfVclntOpenFile1011
 *
 */
class CTestMmfVclntOpenFile1011 : public CTestMmfVclntStep, public MVideoRecorderUtilityObserver
	{
public:
	static CTestMmfVclntOpenFile1011* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aRec);
	static CTestMmfVclntOpenFile1011* NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aRec);
	virtual TVerdict DoTestStepL();
	// MVideoRecorderUtilityObserver
	virtual void MvruoOpenComplete(TInt aError);  
	virtual void MvruoRecordComplete(TInt aError);
	virtual void MvruoPrepareComplete(TInt aError);  
	virtual void MvruoEvent(const TMMFEvent& aEvent);


protected:
	CTestMmfVclntOpenFile1011(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aRec);
	TBuf<KNameBufSize> iSectName;		// Section name for retrieving filename
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TInt iError;
	const TBool iRec;					// Does this test require recording of a video
	TTimeIntervalMicroSeconds iDuration;// Stores duration of video
	};


#endif	// OPENFILEBYHANDLE1011_H__
