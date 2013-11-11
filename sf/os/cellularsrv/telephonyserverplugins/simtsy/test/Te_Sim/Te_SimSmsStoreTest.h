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

#ifndef __TE_SIM_SMSSTORE_TEST__
#define __TE_SIM_SMSSTORE_TEST__

#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimSmsStoreTest : public CSimTestsTestStepBase
	{
public:
	CSimSmsStoreTest();
 	virtual TVerdict doTestStepL();
private:
	RMobileSmsMessaging iSmsMessaging;
    RMobileSmsStore iSmsStore;			//< Sms Store
	};

class CSimTestGetSmsList : public CActive
	{
public:
	static CSimTestGetSmsList* NewL(RMobilePhoneStore& aStore, TBool aBatch,
								 TInt aStart, TInt aBatchSize, TInt aCount, CSimSmsStoreTest* aTestStep);
	~CSimTestGetSmsList();
	void Start();
protected:
	CSimTestGetSmsList(RMobilePhoneStore& aStore, TBool aBatch,
								 TInt aStart, TInt aBatchSize, TInt aCount, CSimSmsStoreTest* aTestStep);
	void ConstructL();
	void RunL();
	void DoCancel();
	TInt CheckGsmResultsL();
protected:
	CRetrieveMobilePhoneSmsList* iRetrieve;
	RMobilePhoneStore& iStore;
	TBool iBatch;
	TInt iStart;
	TInt iBatchSize;
	TInt iCount;
	CSimSmsStoreTest* iTestStep;
	};

#endif // __TE_SIM_SMSSTORE_TEST__
