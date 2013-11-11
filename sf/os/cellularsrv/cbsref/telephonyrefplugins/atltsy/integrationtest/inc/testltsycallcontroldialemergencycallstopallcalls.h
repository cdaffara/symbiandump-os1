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
// @file testltsycallcontroldialemergencycallstopallcalls.h
// 

#ifndef TESTLTSYCALLCONTROLDIALEMERGENCYCALLSTOPALLCALLS_H_
#define TESTLTSYCALLCONTROLDIALEMERGENCYCALLSTOPALLCALLS_H_

//user include
#include "testltsycallcontrolbase.h"

class CTestLtsyCallControlDialEmergencyCallStopAllCall : public CTestLtsyCallControlBase
	{
	
public:
	virtual ~CTestLtsyCallControlDialEmergencyCallStopAllCall();
	
	CTestLtsyCallControlDialEmergencyCallStopAllCall(CTestLtsyModel& aModel);
	
protected:
	virtual TVerdict doTestStepL();
	};

_LIT(KTestLtsyCallControlDialEmergencyCallStopAllCall,   "CTestLtsyCallControlDialEmergencyCallStopAllCall");

#endif /*TESTLTSYCALLCONTROLDIALEMERGENCYCALLSTOPALLCALLS_H_*/
