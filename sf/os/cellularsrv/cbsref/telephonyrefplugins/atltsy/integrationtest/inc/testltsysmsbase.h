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
// Implementation of the Class CTestLtsySmsBase
// 
//

#ifndef TESTLTSYSMSBASE_H_
#define TESTLTSYSMSBASE_H_

// user include
class CTestLtsyModel;
#include "te_integration_stltsysuitestepbase.h"
#include "testltsymodel.h"

//constant defination
const TInt16 KMessageLen(165);

// class declaration
/**
 * This class provide the common functionalities related to the sms.
 * 
 */
class CTestLtsySmsBase : public CTe_integration_stltsySuiteStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	virtual ~CTestLtsySmsBase();
	CTestLtsySmsBase(CTestLtsyModel& aTestModel);
protected:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	RMobileSmsMessaging& Sms();
	RMobileSmsStore& SmsStore();
	void OpenPhoneL();
	void OpenSmsL();
	void OpenSmsStoreL();
	void CloseSmsL();
	void CloseSmsStoreL();
	void GetIndex();
protected:
	CTestLtsyModel& iModel;
	TInt iIndexNum;
	TInt iStoreIndex;
	};

#endif /*TESTLTSYSMSBASE_H_*/
