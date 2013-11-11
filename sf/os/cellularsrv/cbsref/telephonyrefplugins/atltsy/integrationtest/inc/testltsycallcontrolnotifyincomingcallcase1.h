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
// @file testltsycallcontrolnotifyincomingcallcase1.h
// 

#ifndef TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE1_H_
#define TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE1_H_

//user include
#include "testltsycallcontrolbase.h"

class CTestLtsyCallControlNotifyIncomingCallCase1 : public CTestLtsyCallControlBase
	{
public:
	virtual ~CTestLtsyCallControlNotifyIncomingCallCase1();
	
	CTestLtsyCallControlNotifyIncomingCallCase1(CTestLtsyModel& aModel);
	
protected:
	virtual TVerdict doTestStepL();	
	};

_LIT(KTestLtsyCallControlNotifyIncomingCallCase1,   "CTestLtsyCallControlNotifyIncomingCallCase1");
#endif /*TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE1_H_*/
