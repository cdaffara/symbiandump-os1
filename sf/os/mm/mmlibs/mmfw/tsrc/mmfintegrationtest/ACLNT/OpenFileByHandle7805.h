
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
 @file OpenFileByHandle7805.h
*/

#ifndef OPENFILEBYHANDLE7805_H
#define OPENFILEBYHANDLE7805_H

#include "OpenFileByHandle7803.h"

/** 
 * Open new file.
 *
 * @class CTestMmfAclntOpenFile7805
 * 
 */
class CTestMmfAclntOpenFile7805 : public CTestMmfAclntOpenFile7803
	{
public:
	static CTestMmfAclntOpenFile7805* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	virtual TVerdict DoTestL( CMdaAudioPlayerUtility* aPlayer );

protected:
	CTestMmfAclntOpenFile7805( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
	};


#endif	// OPENFILEBYHANDLE7805_H
