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
// @file testltsyclosephone.h
// 

#ifndef TESTTELEPHONYCLOSEPHONE_H_
#define TESTTELEPHONYCLOSEPHONE_H_

// user include
#include "TestLtsyPhoneStepBase.h"

/**
 * This class provide the step to close the shared phone.
 * 
 */
class CTestLtsyClosePhone : public CTestLtsyPhoneStepBase
/**
 * @internalAll
 * @test
 */
/**
 * @internalAll
 * @test
 */
{
public:
	virtual ~CTestLtsyClosePhone();
	CTestLtsyClosePhone(CTestLtsyModel& aTestModel);
protected:
	TVerdict doTestStepL();
};

_LIT(KTestLtsyClosePhone,   "TestLtsyClosePhone");

#endif /*TESTTELEPHONYCLOSEPHONE_H_*/
