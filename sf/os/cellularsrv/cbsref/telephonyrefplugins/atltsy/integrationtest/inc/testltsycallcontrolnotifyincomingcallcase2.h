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
// @file testltsycallcontrolnotifyincomingcallcase2.h
// 

#ifndef TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE2_H_
#define TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE2_H_

//user include
#include "testltsycallcontrolbase.h"

class CTestLtsyCallControlNotifyIncomingCallCase2 : public CTestLtsyCallControlBase
	{
public:
	virtual ~CTestLtsyCallControlNotifyIncomingCallCase2();
	
	CTestLtsyCallControlNotifyIncomingCallCase2(CTestLtsyModel& aModel);
	
protected:
	virtual TVerdict doTestStepL();	
	};

_LIT(KTestLtsyCallControlNotifyIncomingCallCase2,   "CTestLtsyCallControlNotifyIncomingCallCase2");

#endif /*TESTLTSYCALLCONTROLNOTIFYINCOMINGCALLCASE2_H_*/
