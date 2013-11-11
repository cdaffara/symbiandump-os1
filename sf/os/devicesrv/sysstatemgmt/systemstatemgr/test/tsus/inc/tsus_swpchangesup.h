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
// This dll will be used by tsus_loadplugin to change the SWP
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __TSUS_SWPCHANGESUP_H__
#define __TSUS_SWPCHANGESUP_H__

#include <e32base.h>
#include <ssm/ssmutility.h>


// define a known swp so the test step can check if it was correctly updated
_LIT(KTestSwpLoadSupFile,"ssm.swp.policy.sustest");
const TUint KTestSwpLoadSup = {0x0012AB};
const TInt KTestSwpLoadSupNewValue = 334455;

// Use this semaphore to signal the test when this plugin is run
_LIT(KTestSwpLoadSupSemaphore, "TestSwpLoadSupSemaphore"); 

/**
*/		
NONSHARABLE_CLASS(CSwpLoadSup1) : public CActive, public MSsmUtility
	{
public:
	IMPORT_C static MSsmUtility* NewL();
	
	//from MSsmUtility
	void InitializeL();
	void StartL();
	void Release();

	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	//from CBase
	CSwpLoadSup1();
	~CSwpLoadSup1();
	};

/**
*/		
NONSHARABLE_CLASS(CSwpLoadSup2) : public CActive, public MSsmUtility
	{
public:
	IMPORT_C static MSsmUtility* NewL();
	
	//from MSsmUtility
	void InitializeL();
	void StartL();
	void Release();

	//from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private:
	//from CBase
	CSwpLoadSup2();
	~CSwpLoadSup2();
	};

#endif
