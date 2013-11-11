
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
 @file OpenFileByHandle7908.h
*/

#ifndef OPENFILEBYHANDLE7908_H__
#define OPENFILEBYHANDLE7908_H__

#include "OpenFileByHandle7902.h"

/** 
 * Open new file.
 *
 * @class CTestMmfAclntOpenFile7908
 * 
 */

class CTestMmfAclntOpenFile7908 : public CTestMmfAclntOpenFile7902
	{
public:
	static CTestMmfAclntOpenFile7908* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse );
	static CTestMmfAclntOpenFile7908* NewLC( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse );
	virtual TVerdict DoTestStepL();

protected:
	CTestMmfAclntOpenFile7908( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TTestFormat aFormat = ENone, const TBool aCreateFile = EFalse );

private:
	TInt PerformTestStep(TInt aStepNo, TInt& aStepFailCount);
	TVerdict ExecuteStep1L();
	TVerdict ExecuteStep2L();
	TVerdict ExecuteStep3L();
	TVerdict ExecuteStep4L();
	
	CMdaAudioRecorderUtility* iRecUtil;
	CMdaAudioPlayerUtility* iPlayUtil;
	TPtrC iFilename; 
	RFs iFs;
	RFile iFile;
	} ;
	
#endif	// OPENFILEBYHANDLE7908_H__
