// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// common defintions for plugin and caller
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian coverage test code, 
*/
#ifndef __GDCOVCOMMON_H__
#define __GDCOVCOMMON_H__

// message header single byte signature
const TUint8 KGdCoverageInfoSig = 0x56;

// commands plugin may receive
const TUint8 KGdCoverageCmdQuery = 0;
const TUint8 KGdCoverageCmdCoverRedraw = 1;

// information struct returned when querying plugin
NONSHARABLE_STRUCT(TGdCoverageInfo)
	{
	TUint8 iSignature;
	// add any command information here
	TUint8 iNumTests;
	};

#endif // __GDCOVCOMMON_H

