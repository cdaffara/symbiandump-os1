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
// @file testltsycallcontrolnotifyincomingcallcase3.h
// 

#ifndef TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE3_H_
#define TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE3_H_

//user include
#include "testltsycallcontrolbase.h"

class CTestLtsyCallControlNotifyIncomingCallCase3 : public CTestLtsyCallControlBase
	{
public:
	virtual ~CTestLtsyCallControlNotifyIncomingCallCase3();
	
	CTestLtsyCallControlNotifyIncomingCallCase3(CTestLtsyModel& aModel);
	
protected:
	virtual TVerdict doTestStepL();	
	};

_LIT(KTestLtsyCallControlNotifyIncomingCallCase3,   "CTestLtsyCallControlNotifyIncomingCallCase3");

#endif /*TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE3_H_*/
