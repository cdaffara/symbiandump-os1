
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
 @file OpenFileByHandle7803.h
*/

#ifndef OPENFILEBYHANDLE7803_H
#define OPENFILEBYHANDLE7803_H

#include "TSI_MMFACLNT.h"

/** 
 * Open new file.
 *
 * @class CTestMmfAclntOpenFile7803
 * 
 */
class CTestMmfAclntOpenFile7803 : public CTestMmfAclntAudioPlayerUtil
	{
public:
	static CTestMmfAclntOpenFile7803* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	virtual TVerdict DoTestL( CMdaAudioPlayerUtility* aPlayer );

protected:
	CTestMmfAclntOpenFile7803( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	TVerdict PerformTestL( CMdaAudioPlayerUtility* aPlayer );
	
private:
	TBuf<KNameBufSize> iKeyName;		// Key name for retrieving filename
	TBool iSkipShareProtected;			// skip the normal RFs::ShareProtected call
	TBool iExpectToFail;				// True if the RFile open stage should fail
	TInt iExpectedError;				// if iExpectToFail, what error do we expect
	};


#endif	// OPENFILEBYHANDLE7803_H
