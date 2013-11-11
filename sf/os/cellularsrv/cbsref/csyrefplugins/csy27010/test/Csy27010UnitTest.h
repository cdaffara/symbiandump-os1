//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This defines the Unit Tests dealing with CSY 0710 related
// *               functionality as documented in the project's Unit Test Spec.
// *
// *               The tests may be run either interactively or automatically.
// *
// *               The tests are dependant on a predictable response from
// *               the BP side according to the test spec.
// *
// *               The automated tests achieve this predictable response by
// *               using the Tester component.
//

// Csy0710UnitTest.h

/** @file Csy0710UnitTest.h
 *
 */
 
#ifndef CSY0710UNITTEST_H__
#define CSY0710UNITTEST_H__

#include "unitTestBase.h"

_LIT(KCsy0710UnitTest,       "Csy0710UnitTest");
_LIT(KCsy0710UnitTestLog,    "Csy0710UnitTestLog.txt");
_LIT(KCsy0710UnitTestScript, "Csy0710UnitTest.cpp;");

const TInt KMaxCommPorts = 4;

const TInt  KMaxDataSessions = 3;


class CCsy0710UnitTest : public CUnitTestBase
	{
public:
	static void CreateTestL(); //NewL() is called within this function

	static CCsy0710UnitTest* NewL(const TDesC& aUnitTestName,
								  const TDesC& aLogFileName,
								  const TDesC& aTestSpecName,
								  const TDesC& aTestScriptName);
	~CCsy0710UnitTest();

private:
	CCsy0710UnitTest(const TDesC& aUnitTestName,const TDesC& aLogFileName,
				 const TDesC& aTestSpecName,const TDesC& aTestScriptName);

	void ConstructL();

	// from CUnitTestBase
	virtual void RunTestsL(TBool aEndInd,TTestMode aTestMode,
						   TInt& aErrCode, TInt& aLeaveCode, 
		                   TInt aApiNumber,TInt aTestNumber);

	// tests to be called within RunTestsL()
	TInt CsyAL(TInt aTstNum);  // 1
	TInt CsyBL(TInt aTstNum);  // 2
	TInt CsyCL(TInt aTstNum);  // 3
	TInt CsyDL(TInt aTstNum);  // 4
	TInt CsyEL(TInt aTstNum);  // 5
	TInt CsyFL(TInt aTstNum);  // 6
	TInt CsyGL(TInt aTstNum);  // 7
	TInt CsyHL(TInt aTstNum);  // 8

private:

	void PrintTestSpecIdentifiers();
	void PrintTestDetails(TInt aTstSpecNumber);

	TBool HasTestRangesDefined();

	// Helper functions
	void CloseAllResources();

	TInt LoadSerialDeviceDrivers();
	TInt OpenAllCommPorts();
	TInt CloseAllCommPorts();
	TInt GetCsyInfo();
	TInt OpenCommPortFail();
	TInt OpenCmdPortTwice();
	TInt OpenCmdPortInvalidName();
	TInt SendCmdOneCommPort(TInt aPortNum);
	TInt SendCmdsOneCommPort(TInt aPortNum);
	TInt CancelWriteOneCommPort(TInt aPortNum);
	TInt RecvDataOneCommPort(TInt aPortNum);
	TInt ReadDataFromCommPort(TInt aPortNum);
	TInt VerifyFlowControl(TInt aPortNum);
	TInt Verify0710Compliance();
	TInt VerifyStartup();
	TInt VerifyStartupFlowControl(TInt aPortNum);
	TInt VerifyShutdown(TInt aTestSubset);

private:
	RCommServ   iCommServer;
	RComm       iCommPort[KMaxCommPorts + 1];
	RComm		iDataPort;
	TName       iCmdPortName[KMaxCommPorts];
	TName       iDataPort1Name;
	};

#endif//END Csy0710UnitTest_H__



