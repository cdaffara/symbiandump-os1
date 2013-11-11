// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file for ESOCK test section 12
// 
//


#if (!defined __ESOCKTEST_12_H__)
#define __ESOCKTEST_12_H__

#include "TestStepEsock.h"

class CEsockTest12_1 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest12_1();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockTest12_2 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest12_2();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockTest12_3 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest12_3();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockTest12_4 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest12_4();
		
		virtual enum TVerdict easyTestStepL();
		
	};

#endif //(__ESOCKTEST_12_H__)

