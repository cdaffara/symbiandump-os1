
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
 @file OpenFileByHandle7804.h
*/

#ifndef OPENFILEBYHANDLE7804_H
#define OPENFILEBYHANDLE7804_H

#include "TSI_MMFACLNT.h"

/** 
 * Open new file.
 *
 * @class CTestMmfAclntOpenFile7804
 * 
 */
class CTestMmfAclntOpenFile7804 : public CTestMmfAclntAudioPlayerUtil
	{
public:
	static CTestMmfAclntOpenFile7804* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL( CMdaAudioPlayerUtility* aPlayer );

protected:
	CTestMmfAclntOpenFile7804( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict PerformTestL( CMdaAudioPlayerUtility* aPlayer );

	};


#endif	// OPENFILEBYHANDLE7804_H
