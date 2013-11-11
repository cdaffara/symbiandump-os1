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
// OpenFileByFileHandle_PlayTone.h
// Header file: Tone Test: Open file by handle.
// 
//

/**
 @file OpenFileByFileHandle_PlayTone.h
*/

#ifndef OpenFileByFileHandle_PlayTone_h__
#define OpenFileByFileHandle_PlayTone_h__

#include "TestTone.h"

/** 
 * Play a tone file
 *
 * @class CTestMmfAclntOpenToneFile
 *
 */

class CTestMmfAclntOpenToneFile : public CTestMmfAclntTone
	{
public:
	static CTestMmfAclntOpenToneFile* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName );
	virtual TVerdict DoTestStepL();
	virtual TVerdict DoTestStepPreambleL();

protected:	
	CTestMmfAclntOpenToneFile( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName );

	TInt iFrequency;
	TBuf<KNameBufSize> iSectName;
	TBuf<KNameBufSize> iKeyName;
	};

#endif	// OpenFileByFileHandle_PlayTone_h__
