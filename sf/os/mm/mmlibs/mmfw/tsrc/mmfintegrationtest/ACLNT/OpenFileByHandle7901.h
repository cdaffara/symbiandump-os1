
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
 @file OpenFileByHandle7901.h
*/

#ifndef OPENFILEBYHANDLE7901_H
#define OPENFILEBYHANDLE7901_H

#include "TestRecorder.h"
#include "TSI_MMFACLNT.h"

/** 
 * Open new file.
 *
 * @class CTestMmfAclntOpenFile7901
 * 
 */
class CTestMmfAclntOpenFile7901 : public CTestMmfAclntRecord
	{
public:
	virtual TVerdict DoTestL( CMdaAudioRecorderUtility* aRecUtil );
	static CTestMmfAclntOpenFile7901* NewL( const TDesC& aTestName, const TDesC& aSectName, TBool aNegative );

protected:
	CTestMmfAclntOpenFile7901( const TDesC& aTestName, const TDesC& aSectName, TBool aNegative );
	};


#endif	// OPENFILEBYHANDLE7901_H
