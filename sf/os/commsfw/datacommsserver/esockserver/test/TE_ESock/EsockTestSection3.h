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
// This is the header file for ESOCK test section 3
// 
//


#if (!defined __ESOCKTEST_3_H__)
#define __ESOCKTEST_3_H__
#include "TestStepEsock.h"


class CEsockTest3_1 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest3_1();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest3_2 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest3_2();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest3_3 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest3_3();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest3_4 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest3_4();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockTest3_5 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest3_5();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest3_6 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest3_6();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
		
	private:

	    enum TVerdict subTest2(RTimer	&aWatchdog,
	    					   RSocket	&aSocket,
	    					   TDes8	&aBuffer,
	    					   TInt		aExpectedStatus);

	    TInetAddr iAddr;
		TInt      iErrorPort;
		TInt      iValidPort;	
		TInt      iNumTries;	
	};

#endif //(__ESOCKTEST_3_H__)

