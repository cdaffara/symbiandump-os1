
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
 @file OpenFileByHandle0036.h
*/

#ifndef OPENFILEBYHANDLE0036_H__
#define OPENFILEBYHANDLE0036_H__

#include "OpenFileByHandle0034.h"

/**
 * Play Video file
 * 
 * @class CTestMmfVclntOpenFile0036
 *
 */
class CTestMmfVclntOpenFile0036 : public CTestMmfVclntOpenFile0034
	{
public:
	static CTestMmfVclntOpenFile0036* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aPlay);
	static CTestMmfVclntOpenFile0036* NewLC(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aPlay);
	virtual TVerdict DoTestStepL();

protected:
	CTestMmfVclntOpenFile0036(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TBool aPlay);
	};

#endif	// OPENFILEBYHANDLE0036_H__
