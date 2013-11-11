// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "TE_c32base.h"

#ifndef TE_C32_H
#define TE_C32_H

class CTestLoopback : public CC32TestStep
	{
public:
	CTestLoopback();
	~CTestLoopback(){};
	virtual enum TVerdict doTestStepL();
	static TInt DataReadThread(TAny* aArg);
private:
	RSemaphore iSem;
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	TInt iState3;		// test 3
	TInt iState4;		// test 3
	};

class CTestInit : public CC32TestStep
	{
public:
	CTestInit();
	~CTestInit(){};
	virtual enum TVerdict doTestStepL();
	TVerdict doTestStepPreambleL() { return EPass; }
	TVerdict doTestStepPostambleL() { return EPass; }
	};


class CTestAllocMoreWriteMem : public CC32TestStep
	{
public:
	CTestAllocMoreWriteMem();
	~CTestAllocMoreWriteMem() {};
	virtual enum TVerdict doTestStepL();
	};


class CTestMemoryWriteFail : public CC32TestStep
	{
public:
	CTestMemoryWriteFail();
	~CTestMemoryWriteFail() {};
	virtual enum TVerdict doTestStepL();
	};


class CTestResetBuffers : public CC32TestStep
	{
public:
	CTestResetBuffers();
	~CTestResetBuffers() {};
	virtual enum TVerdict doTestStepL();
	};


class CTestCorrectDataRead : public CC32TestStep
	{
public:
	CTestCorrectDataRead();
	~CTestCorrectDataRead() {};
	virtual enum TVerdict doTestStepL();
	};

class CTestMaxBufferOverflow : public CC32TestStep
	{
public:
	CTestMaxBufferOverflow();
	~CTestMaxBufferOverflow() {};
	virtual enum TVerdict doTestStepL();
	};

class CTestDoubleStart : public CC32TestStep
	{
public:
	CTestDoubleStart();
	~CTestDoubleStart(){};

 	virtual TVerdict doTestStepPreambleL( void );
	virtual TVerdict doTestStepPostambleL( void );
	virtual enum TVerdict doTestStepL();
	static TInt StartC32Thread(TAny* aArg);

private:
	//RSemaphore iSem;
	TInt iState1;		// test 1
	};

class CTestOpen : public CC32TestStep
	{
public:
	CTestOpen();
	~CTestOpen(){};
	virtual enum TVerdict doTestStepL();
	};
	
class CTestInfo : public CC32TestStep
	{
public:
	CTestInfo();
	~CTestInfo(){};
	virtual enum TVerdict doTestStepL();
	};
	
class CTestDoubleRead : public CC32TestStep
	{
public:
	CTestDoubleRead();
	~CTestDoubleRead(){};
	static TInt DoubleReadThread(TAny* aArg);
	virtual enum TVerdict doTestStepL();
private:
	TBuf16<15> iBuffer;
	TInt iState1;
	TInt iState2;	
	TInt iState3;	
	TInt iState4;	
	};
	
class CTestTimerAllocation : public CC32TestStep
	{
public:
	CTestTimerAllocation();
	~CTestTimerAllocation(){};
	static void TestTimerAllocationL(TInt aAllocFailValue);
	static TInt TimerThread(TAny* aArg);
	virtual enum TVerdict doTestStepL();
	};
	
class CTestConfigPanic : public CC32TestStep
	{
public:
	CTestConfigPanic();
	~CTestConfigPanic(){};
	static TInt ConfigPanic(TAny* aArg);
	virtual enum TVerdict doTestStepL();
private:
	TInt iState1; 		 // test 1
	TInt iState2;		 // test 2
	TInt iState3;		 // test 3
	};

class CTestBadDescriptorPanic : public CC32TestStep
	{
public:
	CTestBadDescriptorPanic();
	~CTestBadDescriptorPanic(){};
	static TInt BadDescriptor(TAny* aArg);
	virtual enum TVerdict doTestStepL();
private:
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	TInt iState3;		// test 3	
	};


class CTestBusyReturn : public CC32TestStep
	{
public:
	CTestBusyReturn();
	~CTestBusyReturn(){};
	static TInt CreateThread(TAny* aArg);
	static TInt OpenAndReadThreadL(TAny* aArg);
	virtual enum TVerdict doTestStepL();
private:
	RSemaphore iSem;	
	TBuf16<255> iBuffer;
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	TInt iState3;		// test 3
	};	


class CTestCancel : public CC32TestStep
	{
public:
	CTestCancel();
	~CTestCancel(){};
	static TInt CancelThread(TAny* aArg);
	virtual enum TVerdict doTestStepL();
private:
	TBuf16<255> iBuffer;
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	TInt iState3;		// test 3
	TInt iState4;		// test 4
	TInt iState5;
	TInt iState6;
	TInt iState7;
};	

class CTestGetSetConfig : public CC32TestStep
	{
public:
	CTestGetSetConfig();
	~CTestGetSetConfig(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestTimeOut : public CC32TestStep
	{
public:
	CTestTimeOut();
	~CTestTimeOut(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestThreadPanic : public CC32TestStep
	{
public:
	CTestThreadPanic();
	~CTestThreadPanic(){};
	virtual enum TVerdict doTestStepL();
private:
	static TInt OpenExclusiveThread(TAny* aArg);
	static TInt OpenSharedThread(TAny* aArg);
private:
	RSemaphore iSem1;	
	RSemaphore iSem2;
	TBuf16<255> iBuffer;
	TInt iState1;		// test 1
	TInt iState2;		// test 2
	TInt iState3;		// test 3
	TInt iState4;		// test 4
	};		

class CTestBufferOptions : public CC32TestStep
	{
public:
	CTestBufferOptions();
	~CTestBufferOptions(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestOOMConnection : public CC32TestStep
	{
public:
	CTestOOMConnection();
	~CTestOOMConnection(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestOOMPortCreation : public CC32TestStep
	{
public:
	CTestOOMPortCreation();
	~CTestOOMPortCreation(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestSetSignalsPanicBug : public CC32TestStep
	{
public:
	CTestSetSignalsPanicBug();
	~CTestSetSignalsPanicBug(){};
	virtual enum TVerdict doTestStepL();
	};			

class CTestStartServerThread : public CC32TestStep
	{
public:
	CTestStartServerThread();
	~CTestStartServerThread(){};
	virtual enum TVerdict doTestStepL();
	};				

class CTestCapsV02 : public CC32TestStep
	{
public:
	CTestCapsV02();
	~CTestCapsV02(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestOpenPortsWithHighNumbers : public CC32TestStep
	{
public:
	CTestOpenPortsWithHighNumbers();
	~CTestOpenPortsWithHighNumbers(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestReadWrite : public CC32TestStep
	{
public:
	CTestReadWrite();
	~CTestReadWrite(){};
	virtual enum TVerdict doTestStepL();
	};
	
class CTestReadWrite2 : public CC32TestStep
	{
public:
	CTestReadWrite2();
	~CTestReadWrite2(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestSignalLines : public CC32TestStep
	{
public:
	CTestSignalLines();
	~CTestSignalLines(){};
	virtual enum TVerdict doTestStepL();
	};		
	
class CTestQueryreceiveBuffer : public CC32TestStep
	{
public:
	CTestQueryreceiveBuffer();
	~CTestQueryreceiveBuffer(){};
	virtual enum TVerdict doTestStepL();
	};		
	
class CTestWrite0 : public CC32TestStep
	{
public:
	CTestWrite0();
	~CTestWrite0(){};
	virtual enum TVerdict doTestStepL();
	};		
	
class CTestReadOneOrMore : public CC32TestStep
	{
public:
	CTestReadOneOrMore();
	~CTestReadOneOrMore(){};
	virtual enum TVerdict doTestStepL();
	};		
	
class CTestBufferReadWrite : public CC32TestStep
	{
public:
	CTestBufferReadWrite();
	~CTestBufferReadWrite(){};
	virtual enum TVerdict doTestStepL();
	};			
	
class CTestBufferreadOneOrMore : public CC32TestStep
	{
public:
	CTestBufferreadOneOrMore();
	~CTestBufferreadOneOrMore(){};
	virtual enum TVerdict doTestStepL();
	};			

class CTestNotifySignals : public CC32TestStep
	{
public:
	CTestNotifySignals();
	~CTestNotifySignals(){};
	virtual enum TVerdict doTestStepL();
	};		
	
class CTestRxDataAvailable : public CC32TestStep
	{
public:
	CTestRxDataAvailable();
	~CTestRxDataAvailable(){};
	virtual enum TVerdict doTestStepL();
	};		

class CTestHelloEllo : public CC32TestStep
	{
public:
	CTestHelloEllo();
	~CTestHelloEllo(){};
	virtual enum TVerdict doTestStepL();
	};			

class CLsc32ListAvailablePorts	: public CC32TestStep
	{
public:
	CLsc32ListAvailablePorts();
	~CLsc32ListAvailablePorts(){};
	virtual enum TVerdict doTestStepL();
	};			

class CLsc32ListNumberOfLoadedCSYs	: public CC32TestStep
	{
public:
	CLsc32ListNumberOfLoadedCSYs();
	~CLsc32ListNumberOfLoadedCSYs(){};
	virtual enum TVerdict doTestStepL();
	};		

class CLsc32LoadUnloadCsy : public CC32TestStep
	{
public:
	CLsc32LoadUnloadCsy();
	~CLsc32LoadUnloadCsy(){};
	virtual enum TVerdict doTestStepL();
	};		

class CLsc32GetPortInfo : public CC32TestStep		
	{
public:
	CLsc32GetPortInfo();
	~CLsc32GetPortInfo(){};
	virtual enum TVerdict doTestStepL();
	};					

class CDummyTest : public CC32TestStep		
	{
public:
	CDummyTest();
	~CDummyTest(){};
	virtual enum TVerdict doTestStepL();
private:
	void WaitingTestsL(TCommRole aRole);
	void DummyClientTestsL(TCommRole aRole, TBool aHeapCheck);
	void DummyPreemptableTestsL(TCommRole aRole);
	void ShowSignals(TUint asig, TCommRole aRole);
	void ShowFlow(TFlowControl aFlow);
	void ShowConfig(TCommNotificationV01 aConfig);
	void ShowCapsV02(TCommCapsV02 aCaps);
	void ShowCapsV01(TCommCapsV01 aCaps);
	RComm iDPort0; //< comm port 0
	RComm iDPort1; //< comm port 1
	RComm iDPort2; //< comm port 2
	};		

class CTestDCEPort : public CC32TestStep		
	{
public:
	CTestDCEPort();
	~CTestDCEPort(){};
	virtual enum TVerdict doTestStepL();
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepPostambleL();
	};
	
class CTestPlatSecOpenPortDummy : public CC32TestStep
	{
public:
	CTestPlatSecOpenPortDummy();
	~CTestPlatSecOpenPortDummy(){};
	virtual enum TVerdict doTestStepL();
	};


class CTestOpenWhenAvailable : public CC32TestStep
	{
public:
	virtual enum TVerdict doTestStepL();
	};


#endif //TE_C32_H



