// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file testltsyphonestorebase.h
// 

#ifndef TESTLTSYPHONESTOREBASE_H_
#define TESTLTSYPHONESTOREBASE_H_

class CTestLtsyModel;
// user include
#include "te_integration_stltsysuitestepbase.h"
#include "testltsymodel.h"


// class declaration
/**
 * This class provide the common functionalities related to the PhoneStore
 * 
 */
class CTestLtsyPhoneStoreBase : public CTe_integration_stltsySuiteStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	virtual ~CTestLtsyPhoneStoreBase();
	         CTestLtsyPhoneStoreBase(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	RMobileONStore& PhoneONStore();
	RMobileENStore& PhoneENStore();	
	RMobilePhoneBookStore& PhoneBookStore();
	void OpenPhoneL();

protected:
	CTestLtsyModel& iModel;
	};

#endif /*TESTLTSYPHONESTOREBASE_H_*/
