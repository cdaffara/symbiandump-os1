// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TE_SIM_SMSLIST_TEST__
#define __TE_SIM_SMSLIST_TEST__

#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimSmsListTest : public CSimTestsTestStepBase
	{
public:
	CSimSmsListTest();
 	virtual TVerdict doTestStepL();
private:
	RMobileSmsMessaging iSmsMessaging;
	};

class CSimTestGetSmspList : public CActive
	{
public:
  	static CSimTestGetSmspList* NewL(RMobileSmsMessaging& aSmsMessaging,TBool aBatch,
  								 TInt aStart,TInt aBatchSize,TInt aCount, CSimSmsListTest* aTestStep);
	~CSimTestGetSmspList();
	void Start();
protected:
	CSimTestGetSmspList(RMobileSmsMessaging& aSmsMessaging,TBool aBatch,
								 TInt aStart,TInt aBatchSize,TInt aCount, CSimSmsListTest* aTestStep);
	void ConstructL();
	void RunL();
	void DoCancel();
	TInt CheckGsmResultsL();
protected:
	CRetrieveMobilePhoneSmspList* iRetrieveSmsp;
	RMobileSmsMessaging& iSmsMessaging;
	TBool iBatch;
	TInt iStart;
	TInt iBatchSize;
	TInt iCount;
	CSimSmsListTest* iTestStep;
	};

#endif // __TE_SIM_SMSLIST_TEST__

