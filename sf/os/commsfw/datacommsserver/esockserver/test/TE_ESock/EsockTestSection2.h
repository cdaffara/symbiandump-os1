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
// This is the header file for ESOCK test section 2
// 
//


#if (!defined __ESOCKTEST_2_H__)
#define __ESOCKTEST_2_H__
#include "TestStepEsock.h"


class CEsockTest2_1 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest2_1();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest2_2 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest2_2();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest2_3 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest2_3();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest2_4 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest2_4();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest2_5 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest2_5();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

#endif //(__ESOCKTEST_2_H__)

