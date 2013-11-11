// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This checks that the link-local addresses on 2 conns
// are the same
// 
//

/**
 @file
*/
 
#include "TS_CheckLinkLocalSame.h"

/* 
 * 
 */
CTS_CheckLinkLocalSame::CTS_CheckLinkLocalSame()
	{
	iTestStepName = KCheckLinkLocalSame;
	}

CTS_CheckLinkLocalSame::~CTS_CheckLinkLocalSame()
	{	
	}

enum TVerdict CTS_CheckLinkLocalSame::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KCheckLinkLocalSame,KOomTest,iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}


enum TVerdict CTS_CheckLinkLocalSame::doTestStepL(void)
	{
	TBool expectSameAddr = TRUE;
	TBool same;
	GetBoolFromConfig(KCheckLinkLocalSame,KExpectSameAddress,expectSameAddr);
	
	same = iOwnerSuite->CompareLinkLocalAddressesL();

	if (same && expectSameAddr)
		return EPass;
	else
		return EFail;
	}


