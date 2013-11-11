// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MISCADAPTATION_H__
#define __MISCADAPTATION_H__

#include <ssm/ssmadaptation.h>
#include "f32file.h" 
#include "s32file.h"
/*
 SecurityCustomCommand Test Case ID

 @internalComponent
 @released
*/
enum TTestSecurityCustomCommandTestCaseID
	{
	EPINCHECK01 = 1,
	EPINCHECK02,
	EPINCHECK03,
	EPINCHECK04,
	EPINCHECK05,
	EPINCHECK06,
	EPINCHECK07,
	EPINCHECK08
	};

/**
 Collection of different adaptations.
 Implements MMiscAdaptation for different adaptations.

 @internalComponent
 @released

 @see MMiscAdaptation
*/
class CMiscAdaptationRef : public CBase, public MMiscAdaptation
	{
public:
	static CMiscAdaptationRef* NewL();

	//from MMiscAdaptation
	void Release();
	void SecurityStateChange(TInt aState, TDes8& aResponsePckg, TRequestStatus& aStatus);
	void GetGlobalStartupMode(TDes8& aModePckg, TRequestStatus& aRequest);
	void PrepareSimLanguages(TSsmLanguageListPriority aPriority, TDes8& aSizePckg, TRequestStatus& aStatus);
	void GetSimLanguagesL(CBufBase* aBuf, TInt aCount);
	void GetHiddenReset(TDes8& aHiddenResetPckg, TRequestStatus& aStatus);
	void Cancel();
	
private:
	CMiscAdaptationRef();
	void ConstructL();	
	~CMiscAdaptationRef();
	//Helper function to check for P&S Key
    TBool IsTestPsKeyDefined();

private:
	TInt iTestCaseNum;
	RFs iFs;
	RFileReadStream iFile;
	MMiscAdaptation* iSaaMiscAdaptationDll;
	RLibrary iSaaMiscAdaptationLib;
	};

#endif // __MISCADAPTATION_H__

