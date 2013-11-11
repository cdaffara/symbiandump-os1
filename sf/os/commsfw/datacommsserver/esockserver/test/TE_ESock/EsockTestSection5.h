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
// This is the header file for ESOCK test section 5
// 
//


#if (!defined __ESOCKTEST_5_H__)
#define __ESOCKTEST_5_H__
#include "TestStepEsock.h"


class CEsockSendData : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockSendData();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockOpenUdpSocket : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockOpenUdpSocket();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockSendUdpData : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockSendUdpData();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockSendTcpData : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockSendTcpData();
		
		virtual enum TVerdict easyTestStepL();
		
	};

class CEsockTest5_1 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_1();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
	
	private:
		TInt iNumSockets;
		TInt iSockIndex2, iSockIndex3;
	};

class CEsockTest5_2 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_2();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();

	private:
		TInt iSockIndex2, iSockIndex3;
	};

class CEsockTest5_3 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_3();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();

	private:
		TInt iLogLevel;
	};

class CEsockTest5_4 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_4();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();

	private:
		TInt iLogLevel;
	};

class CEsockTest5_5 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_5();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();

	private:
		TInt iSockIndex2, iSockIndex3;
	};

class CEsockTest5_6 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_6();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();

	private:
		TInt iSockIndex2, iSockIndex3;
	};

class CEsockTest5_7 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_7();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();

	private:
		TInt iSockIndex2, iSockIndex3;
	};

class CEsockTest5_8 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_8();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();

	private:
		TInt iSockIndex2, iSockIndex3;
	};
	
class CEsockTest5_9 : public CESockTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		~CEsockTest5_9();
		
		virtual enum TVerdict easyTestStepPreambleL();
		virtual enum TVerdict easyTestStepL();
		
	};

#endif //(__ESOCKTEST_5_H__)

