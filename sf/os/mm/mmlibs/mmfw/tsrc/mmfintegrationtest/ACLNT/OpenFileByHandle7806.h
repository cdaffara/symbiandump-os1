
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
 @file OpenFileByHandle7806.h
*/

#ifndef OPENFILEBYHANDLE7806_H
#define OPENFILEBYHANDLE7806_H

#include "OpenFileByHandle7804.h"

/** 
 * Open new file.
 *
 * @class CTestMmfAclntOpenFile7806
 * 
 */
class CTestMmfAclntOpenFile7806 : public CTestMmfAclntOpenFile7804
	{
public:
	static CTestMmfAclntOpenFile7806* NewL( const TDesC& aTestName, const TDesC& aSectName );
	virtual TVerdict DoTestL( CMdaAudioPlayerUtility* aPlayer );

protected:
	CTestMmfAclntOpenFile7806( const TDesC& aTestName, const TDesC& aSectName );
	};

/**
 * alloc tests for repeats
 *
 * @class CTestMmfAclntRepeatPlayAlloc
 *
 */
class CTestMmfAclntRepeatPlayAlloc : public CTestMmfAclntOpenFile7806
	{
public:
	static CTestMmfAclntRepeatPlayAlloc* NewL( const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName );
	virtual TVerdict PerformTestL( CMdaAudioPlayerUtility* aPlayer );
protected:
	CTestMmfAclntRepeatPlayAlloc( const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName );
protected:
	TBuf<KNameBufSize> iSectName;
	TBuf<KNameBufSize> iKeyName;
	};


#endif	// OPENFILEBYHANDLE7806_H
