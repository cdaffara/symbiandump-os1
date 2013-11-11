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
// This is the header file for ESOCK test section 9
// 
//


#if (!defined __ESOCKTEST_9_H__)
#define __ESOCKTEST_9_H__
#include "TestStepEsock.h"


class CEsockTest9_1 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest9_1();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest9_2 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest9_2();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest9_3 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest9_3();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest9_4 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest9_4();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

class CEsockTest9_5 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest9_5();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	};

NONSHARABLE_CLASS( CEsockTest9_6 ) : public CESockTestStep_OOMCapable
	{
public:
	static const TDesC& GetTestName();
	CEsockTest9_6();
	~CEsockTest9_6();

	virtual TVerdict easyTestStepL();
protected:
	TBool PerformSend( RSocket& aSocket, const TDesC8& aDesc, TSockAddr& anAddress );
	TBool PerformRecv( RSocket& aSocket, TDes8& aDesc, TSockAddr& anAddress );

	TBool CreateUdpSocket( RSocket& aSocket, TSockAddr& aAddress );

	void WaitFor( TRequestStatus& aRequestStatus, TInt aTimerPeriod );

	RTimer iGuardTimer;
	};

#endif //(__ESOCKTEST_9_H__)

