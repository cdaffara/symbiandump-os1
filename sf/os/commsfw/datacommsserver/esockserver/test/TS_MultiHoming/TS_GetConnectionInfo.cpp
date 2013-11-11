// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
 
#include "TS_GetConnectionInfo.h"


/* 
 * GetConnectionInfo
 * Displays info on named RConnrction
 */
CTS_GetConnectionInfo::CTS_GetConnectionInfo()
	{
	iTestStepName = KGetConnectionInfo;
	}

CTS_GetConnectionInfo::~CTS_GetConnectionInfo()
	{	
	}


enum TVerdict CTS_GetConnectionInfo::doTestStepPreambleL(void)
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KGetConnectionInfo,KOomTest,iIsOOMTest)))
		iIsOOMTest=EFalse;
	return EPass;
	}

void CTS_GetConnectionInfo::PrintFeature(TUint& aMask, TUint aFeature, const TDesC& aDes)
	{
	if(aMask&aFeature)
		{
		aMask&=~aFeature;
		LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, aDes);
		if(aMask)
			LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, _L(","));
		}
	}


enum TVerdict CTS_GetConnectionInfo::doTestStepL(void)
/**
 * Displays info on named RConnrction
 * @return The test step verdict
 */
	{
	// Function now returns number of connections so no error check
	iOwnerSuite->DisplayConnectionDetailsL();
	return EPass;

	/*if (KErrNone == err)
		return EPass;
	else
		{
		TPtrC Errortxt = EpocErrorToText(err);
		LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr, KELogger, &Errortxt);
		}
	return EFail;*/
	}

