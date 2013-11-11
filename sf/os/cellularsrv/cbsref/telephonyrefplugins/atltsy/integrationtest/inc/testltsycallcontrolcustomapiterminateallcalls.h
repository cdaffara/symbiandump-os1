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
// @file testltsycallcontrolcustomapiterminateallcalls.h
// 

#ifndef TESTLTSYCALLCONTROLCUSTOMAPITERMINATEALLCALLS_H_
#define TESTLTSYCALLCONTROLCUSTOMAPITERMINATEALLCALLS_H_


//user include
#include "testltsycallcontrolbase.h"

class CTestLtsyCallControlCustomAPITerminateAllCalls : public CTestLtsyCallControlBase
	{
	
public:
	virtual ~CTestLtsyCallControlCustomAPITerminateAllCalls();
	
	CTestLtsyCallControlCustomAPITerminateAllCalls(CTestLtsyModel& aModel);
	
protected:
	virtual TVerdict doTestStepL();
	};

_LIT(KTestLtsyCallControlCustomAPITerminateAllCalls,   "CTestLtsyCallControlCustomAPITerminateAllCalls");
#endif /*TESTLTSYCALLCONTROLCUSTOMAPITERMINATEALLCALLS_H_*/
