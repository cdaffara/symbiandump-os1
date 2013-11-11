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
// This is the header file for ESOCK test section 11
// 
//


#if (!defined __ESOCKTEST_11_H__)
#define __ESOCKTEST_11_H__

#include "TestStepEsock.h"

class CEsockTest11_1 : public CTestStepEsock
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest11_1();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockTest11_2 : public CTestStepEsock
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest11_2();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockTest11_3 : public CTestStepEsock
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest11_3();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockTest11_4 : public CTestStepEsock
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest11_4();
		
		virtual enum TVerdict easyTestStepL();
		
	};

#endif //(__ESOCKTEST_11_H__)

