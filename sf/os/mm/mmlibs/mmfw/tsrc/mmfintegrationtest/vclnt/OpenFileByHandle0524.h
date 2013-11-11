
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
 @file OpenFileByHandle0524.h
*/

#ifndef OPENFILEBYHANDLE0524_H__
#define OPENFILEBYHANDLE0524_H__

#include "OpenFileByHandle0523.h"

/**
 * Play Video file
 * 
 * @class CTestMmfVclntOpenFile0524
 *
 */
class CTestMmfVclntOpenFile0524 : public CTestMmfVclntOpenFile0523
	{
public:
	static CTestMmfVclntOpenFile0524* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aRec);
	static CTestMmfVclntOpenFile0524* NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aRec);
	virtual TVerdict DoTestStepL();

protected:
	CTestMmfVclntOpenFile0524(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aRec);
	};

#endif	// OPENFILEBYHANDLE0524_H__
