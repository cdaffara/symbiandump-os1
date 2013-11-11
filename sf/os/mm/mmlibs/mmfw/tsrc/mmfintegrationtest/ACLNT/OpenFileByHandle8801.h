
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
 @file OpenFileByHandle8801.h
*/

#ifndef OPENFILEBYHANDLE8801_H
#define OPENFILEBYHANDLE8801_H

#include "TSI_MMFACLNT.h"

/** 
 * Open new file.
 *
 * @class CTestMmfAclntOpenFile8801
 * 
 */
class CTestMmfAclntOpenFile8801 : public CTestMmfAclntAudioPlayerUtil
	{
public:
	static CTestMmfAclntOpenFile8801* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL( CMdaAudioPlayerUtility* aPlayer );

protected:
	CTestMmfAclntOpenFile8801( const TDesC& aTestName, const TDesC& aSectName );
	TVerdict PerformTestL( CMdaAudioPlayerUtility* aPlayer );

	};


#endif	// OPENFILEBYHANDLE8801_H
