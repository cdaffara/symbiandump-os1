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
// @file testltsycallcontrolswapcalls.h
// 

#ifndef TESTLTSYCALLCONTROLSWAPCALLS_H_
#define TESTLTSYCALLCONTROLSWAPCALLS_H_

//user include
#include "testltsycallcontrolbase.h"

class CTestLtsyCallControlSwapCalls : public CTestLtsyCallControlBase
	{
public:
	virtual ~CTestLtsyCallControlSwapCalls();
	
	CTestLtsyCallControlSwapCalls(CTestLtsyModel& aModel);
	
protected:
	virtual TVerdict doTestStepL();		
	};

_LIT(KTestLtsyCallControlSwapCalls,   "CTestLtsyCallControlSwapCalls");
#endif /*TESTLTSYCALLCONTROLSWAPCALLS_H_*/
