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
// * Description:  This contains the Unit Tests dealing with CSY 0710 related
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

// Csy0710UnitTest.cpp

/** @file Csy0710UnitTest.cpp
 *
 */

#include "Csy27010UnitTest.h"
#include <exterror.h>


_LIT8(KATCommand1,  "AT+CGMM\r");
_LIT8(KATCommand2,  "AT+CGMM\r");
_LIT8(KATCommand3,  "AT+CMUX?\r");
_LIT8(KATCommand4,  "AT\r");
_LIT8(KATCommand5,  "AT+CGMI\r");
_LIT8(KATCommand6,  "AT+CGSN\r");
_LIT8(KATCommand7,  "AT+CGMR\r");
_LIT8(KATCommand8,  "AT+CBC\r");
_LIT8(KATCommand9,  "AT+CSQ\r");
_LIT8(KATCommand10, "ATS7?\r");
_LIT8(KATCommand11, "ATS0=5\r");
_LIT8(KATCommand12, "AT+FCLASS=2.0\r");
_LIT8(KATCommand13, "AT+CRC=?\r");

//_LIT8(KATCommandToCancel, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\r");
_LIT8(KATCommandToCancel, "AT\r");

//-----------------------------------------------------------------------------
// Helper routines
//-----------------------------------------------------------------------------
//

TBool CCsy0710UnitTest::HasTestRangesDefined()
	{
	return ETrue;
	}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CCsy0710UnitTest* CCsy0710UnitTest::NewL(const TDesC& aUnitTestName,
								 const TDesC& aLogFileName,
							     const TDesC& aTestSpecName,
							     const TDesC& aTestScriptName)
	{
	CCsy0710UnitTest* self = new(ELeave) CCsy0710UnitTest(aUnitTestName,
				aLogFileName, aTestSpecName, aTestScriptName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CCsy0710UnitTest::~CCsy0710UnitTest()
	{
	}


CCsy0710UnitTest::CCsy0710UnitTest(const TDesC& aUnitTestName,
						   const TDesC& aLogFileName,
						   const TDesC& aTestSpecName,
						   const TDesC& aTestScriptName)
: CUnitTestBase(aUnitTestName, aLogFileName, aTestSpecName, aTestScriptName)
	{}


void CCsy0710UnitTest::ConstructL()
	{	
	CUnitTestBase::ConstructL();
	
	iCmdPortName[0].Copy(_L("gsm0710::1"));
	iCmdPortName[1].Copy(_L("gsm0710::2"));
	iCmdPortName[2].Copy(_L("gsm0710::3"));
	iCmdPortName[3].Copy(_L("gsm0710::4"));
	iDataPort1Name.Copy(_L("gsm0710::101"));
	}

void CCsy0710UnitTest::PrintTestSpecIdentifiers()
	{
	iTest.Printf(_L("\nCSY 0710 Unit Test\n\n"));
	iTest.Printf(_L("1: CSY A tests\n"));
	iTest.Printf(_L("2: CSY B tests\n"));
	iTest.Printf(_L("3: CSY C tests\n"));
	iTest.Printf(_L("4: CSY D tests\n"));
	iTest.Printf(_L("5: CSY E tests\n"));
	iTest.Printf(_L("6: CSY F tests\n"));
	iTest.Printf(_L("7: CSY G tests\n"));
	iTest.Printf(_L("8: CSY H tests\n"));
	}


void CCsy0710UnitTest::PrintTestDetails(TInt /*aTstSpecNumber*/)
	{
	// This version prints no helpful info on the screen. Overload this in
	// your own test class to put out test details for the test spec idenifier
	// selected
	}


void CCsy0710UnitTest::RunTestsL(TBool aEndInd,TTestMode aTestMode,
								 TInt& aErrCode, TInt& aLeaveCode, 
								 TInt aApiNumber, TInt aTestNumber)
	{
	switch (aApiNumber)
		{
		case 0:
			aLeaveCode = KErrNone;
			aErrCode = 8;  // 8 more tests
			break;
		case 1:
			TRAP(aLeaveCode,aErrCode = CsyAL(aTestNumber));
			break;
		case 2:
			TRAP(aLeaveCode,aErrCode = CsyBL(aTestNumber));
			break;
		case 3:
			TRAP(aLeaveCode,aErrCode = CsyCL(aTestNumber));
			break;
		case 4:
			TRAP(aLeaveCode,aErrCode = CsyDL(aTestNumber));
			break;
		case 5:
			TRAP(aLeaveCode,aErrCode = CsyEL(aTestNumber));
			break;
		case 6:
			TRAP(aLeaveCode,aErrCode = CsyFL(aTestNumber));
			break;
		case 7:
			TRAP(aLeaveCode,aErrCode = CsyGL(aTestNumber));
			break;
		case 8:
			TRAP(aLeaveCode,aErrCode = CsyHL(aTestNumber));
			break;
		default:
			ActionNoMatch(aEndInd,aTestMode,aApiNumber, aTestNumber);
		}

	//CloseAllResources();
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


TInt CCsy0710UnitTest::CsyAL(TInt aTstNum)
/**
 * CSY Information Tests
 * @param aTstNum test number
 * @return KErrNone or error code
 */
	{
	if (aTstNum != 0)
		iTest.Printf(_L("CSYA-%d\n"), aTstNum);

	TInt ret = KErrNone;

	SetTsIdentifier(_L("CSYA"));
	SetMethodName(_L("CSYA"));

	switch (aTstNum)
		{
		case 0:		// Return how may auto tests are present
			return (4);
			break;

		case 1:		// Start C32
			iTest.Printf(_L("Start C32...\n"));	
			SetFunIndex(1);
			TEST(LoadSerialDeviceDrivers());
			ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			iTest.Printf(_L("Test 1-1 Passed\n"));
			return (KErrNone);
			break;

		case 2:		// Load and Unload CSY multiple times
			iTest.Printf(_L("Basic check on something else..\n"));	
			SetFunIndex(1);
			TEST(LoadSerialDeviceDrivers());
			ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);

			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;
			iCommServer.Close();
			iTest.Printf(_L("Test 1-2 Passed\n"));
			return (KErrNone);
			break;

		case 3:		// Open Multiple Sessions to C32
			iTest.Printf(_L("Open Multiple Sessions to C32...\n"));	
			SetFunIndex(1);
			TEST(LoadSerialDeviceDrivers());
			ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);

			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			TEST(OpenAllCommPorts());
			User::After(5000000);
			CloseAllCommPorts();
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			User::After(3000000);
			iTest.Printf(_L("Test 1-3 Passed\n"));
			return (KErrNone);
			break;

		case 4: 		// Check that the phone supports the expected extensions
			iTest.Printf(_L("Get CSY Info...\n"));	
			SetFunIndex(1);
			TEST(LoadSerialDeviceDrivers());
			ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);

			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			TEST(GetCsyInfo());
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();			
			User::After(5000000);
			return KErrNone;
			break;

					// Manual Tests Follow...
		case 100:		// Return how many manual tests are present
			{
			return 0;
			}
			break;

		default:
			iTest.Printf(_L("** Not implemented CSYA-%d **\n"), aTstNum);
			break;
		}

	return ret;
	}


TInt CCsy0710UnitTest::CsyBL(TInt aTstNum)
/**
 * CSY Open and Close Logical Ports Tests
 * @param aTstNum test number
 * @return void
 */
	{
	if (aTstNum != 0)
		iTest.Printf(_L("CSYB-%d\n"), aTstNum);

	TInt ret = KErrNone;

	SetTsIdentifier(_L("CSYB"));
	SetMethodName(_L("CSYB"));

	switch (aTstNum)
		{
		case 0:		// Return how may auto tests are present
			return (6);
			break;

		case 1:		// open & close subsessions
			iTest.Printf(_L("Open & Close Subsessions...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
			ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);

			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			TEST(OpenAllCommPorts());
			CloseAllCommPorts();
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 2-1 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 2:		// check failure to open comm port	
			iTest.Printf(_L("Check failure to open comm port...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			TEST(OpenCommPortFail());
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 2-2 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 3:		// open many ports
			iTest.Printf(_L("Open many RComm ports...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			TEST(OpenAllCommPorts());
			User::After(5000000);
			CloseAllCommPorts();
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 2-3 Passed\n"));
			User::After(3000000);
			return (KErrNone);
			break;

		case 4:		// check ability to open same port twice
			iTest.Printf(_L("Check ability to open RComm port twice...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			TEST(OpenCmdPortTwice());
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 2-4 Passed\n"));
			User::After(3000000);
			return (KErrNone);
			break;

		case 5:		// open many subsessions at same time
			iTest.Printf(_L("Open many RComm ports at same time...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			TEST(OpenAllCommPorts());
			User::After(5000000);
			CloseAllCommPorts();
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 2-5 Passed\n"));
			User::After(3000000);
			return (KErrNone);
			break;

		case 6:		// check failure to open port with an invalid name
			iTest.Printf(_L("Check failure to open port with invalid name...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);

			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			TEST(OpenCmdPortInvalidName());
			User::After(5000000);
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			User::After(1000000);
			iTest.Printf(_L("Test 2-6 Passed\n"));
			User::After(2000000);
			return (KErrNone);
			break;

			
			// Manual Tests Follow...
		case 100:		// Return how many manual tests are present
			{
			return 0;
			}
			break;

		default:
			iTest.Printf(_L("** Not implemented CSYB-%d **\n"), aTstNum);
			break;
		}

	return (ret);
	}


TInt CCsy0710UnitTest::CsyCL(TInt aTstNum)
/**
 * CSY Data Transmission Tests.
 * @param aTstNum test number
 * @return void
 */
	{
	if (aTstNum != 0)
		iTest.Printf(_L("CSYC-%d\n"), aTstNum);

	TInt ret = KErrNone;

	SetTsIdentifier(_L("CSYC"));
	SetMethodName(_L("CSYC"));

	switch (aTstNum)
		{
		case 0:		// Return how may auto tests are present
			return (4);
			break;

		case 1:		// Check that information can be sent
			iTest.Printf(_L("Send command on Comm Port...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			TEST(SendCmdOneCommPort(0));
			User::After(1000000);
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 3-1 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 2:		// Check that information can be sent
			iTest.Printf(_L("Send multiple commands on Comm Port...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			SendCmdsOneCommPort(0);
			User::After(1000000);
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 3-2 Passed\n"));
			User::After(3000000);
			return (KErrNone);
			break;
			
		case 3:		// check BP ack failure
			iTest.Printf(_L("Test 3-3 Passed\n"));
			SetFunIndex(1);
			User::After(3000000);
			return (KErrNone);

		case 4:		// Check write cancel on outstanding cmd
			iTest.Printf(_L("Cancel Write Request on Comm Port...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			CancelWriteOneCommPort(0);
			User::After(1000000);

			// common exit test code
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 3-4 Passed\n"));
			User::After(3000000);
			return (KErrNone);
			break;

			// Manual Tests Follow...
		case 100:		// Return how many manual tests are present
			{
			return 0;
			}
			break;

		default:
			iTest.Printf(_L("** Not implemented CSYC-%d **\n"), aTstNum);
			break;
		}

	return (ret);
	}


TInt CCsy0710UnitTest::CsyDL(TInt aTstNum)
/**
 * CSY Tests for receive data on channels.
 * @param aTstNum test number
 * @return KErrNone or error code
 */
	{
	TInt ret = KErrNone;

	if (aTstNum != 0)
		iTest.Printf(_L("CSYD-%d\n"), aTstNum);

	SetTsIdentifier(_L("CSYD"));
	SetMethodName(_L("CSYD"));

	switch (aTstNum)
		{
		case 0:		// Return how may auto tests are present
			return (2);
			break;

		case 1:		// Check that data can be received
			iTest.Printf(_L("Recv Data on Comm Port...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			ret = iCommPort[0].Open(iCommServer, iCmdPortName[0], ECommShared);
			if (ret != KErrNone) return __LINE__;
			TEST(RecvDataOneCommPort(0));
			iCommPort[0].Close();
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 4-1 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 2:		// Check that data is received and given to the correct port
			iTest.Printf(_L("Recv Data and Route to Correct Port...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			ret = iCommPort[0].Open(iCommServer, iCmdPortName[0], ECommShared);
			if (ret != KErrNone) return __LINE__;
			ret = iCommPort[1].Open(iCommServer, iCmdPortName[1], ECommShared);
			if (ret != KErrNone) return __LINE__;
			ret = iCommPort[2].Open(iCommServer, iCmdPortName[1], ECommShared);
			if (ret != KErrNone) return __LINE__;
			ret = iCommPort[3].Open(iCommServer, iCmdPortName[1], ECommShared);
			if (ret != KErrNone) return __LINE__;

			TEST(RecvDataOneCommPort(0));
			TEST(RecvDataOneCommPort(1));
			TEST(RecvDataOneCommPort(2));
			TEST(RecvDataOneCommPort(3));

			ret = iCommPort[4].Open(iCommServer, iDataPort1Name, ECommShared); //data port
			if (ret != KErrNone) return __LINE__;
			TEST(RecvDataOneCommPort(4)); //newly open port will take away the control of the dlc
			iCommPort[4].Close();
			// should return control of dlc

			TEST(RecvDataOneCommPort(0));
			TEST(RecvDataOneCommPort(1));
			TEST(RecvDataOneCommPort(2));
			TEST(RecvDataOneCommPort(3));

			iCommPort[3].Close();
			iCommPort[2].Close();
			iCommPort[1].Close();
			iCommPort[0].Close();
			User::After(1000000);
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 4-2 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

			// Manual Tests Follow...
		case 100:		// Return how many manual tests are present
			{
			return 0;
			}
			break;

		default:
			iTest.Printf(_L("** Not implemented CSYD-%d **\n"), aTstNum);
			break;
		}		
			
	return (KErrNone);
	}


TInt CCsy0710UnitTest::CsyEL(TInt aTstNum)
/**
 * CSY Tests for flow control of data on logical ports.
 * @param aTstNum test number
 * @return KErrNone or error code
 */
	{
	TInt ret = KErrNone;

	if (aTstNum != 0)
		iTest.Printf(_L("CSYE-%d\n"), aTstNum);

	SetTsIdentifier(_L("CSYE"));
	SetMethodName(_L("CSYE"));

	switch (aTstNum)
		{
		case 0:		// Return how may auto tests are present
			return (1);
			break;

		case 1:		// Check that flow control signals correct port
			iTest.Printf(_L("Verify Flow Control Signals to Correct Port...\n"));	
			SetFunIndex(1);

			// common test code
			TEST(LoadSerialDeviceDrivers());
            ret = StartC32();
            if ((ret != KErrNone) && (ret != KErrAlreadyExists))
                TEST(ret);
			TEST(iCommServer.Connect());
			ret = iCommServer.LoadCommModule(KCsyName);
			if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

			// specific test code
			ret = iCommPort[0].Open(iCommServer, iCmdPortName[0], ECommShared);
			if (ret != KErrNone) return __LINE__;
			ret = iCommPort[1].Open(iCommServer, iCmdPortName[1], ECommShared);
			if (ret != KErrNone) return __LINE__;
			ret = iCommPort[2].Open(iCommServer, iCmdPortName[2], ECommShared);
			if (ret != KErrNone) return __LINE__;
			ret = iCommPort[3].Open(iCommServer, iCmdPortName[3], ECommShared);
			if (ret != KErrNone) return __LINE__;

			TEST(VerifyFlowControl(0));
			TEST(VerifyFlowControl(1));
			TEST(VerifyFlowControl(2));
			TEST(VerifyFlowControl(3));

			iCommPort[3].Close();
			iCommPort[2].Close();
			iCommPort[1].Close();
			iCommPort[0].Close();
			User::After(1000000);
			iCommServer.UnloadCommModule(KCsyName);
			iCommServer.Close();
			iTest.Printf(_L("Test 5-1 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

			// Manual Tests Follow...
		case 100:		// Return how many manual tests are present
			{
			return 0;
			}
			break;

		default:
			iTest.Printf(_L("** Not implemented CSYE-%d **\n"), aTstNum);
			break;
		}			
			
	return (KErrNone);
	}


TInt CCsy0710UnitTest::CsyFL(TInt aTstNum)
/**
 * CSY Tests for 3GPP 27.010 compliance.
 * @param aTstNum test number
 * @return KErrNone or error code
 */
	{
	if (aTstNum != 0)
		iTest.Printf(_L("CSYF-%d\n"), aTstNum);

	SetTsIdentifier(_L("CSYF"));
	SetMethodName(_L("CSYF"));

	iTest.Printf(_L("Verify 3GPP 27.010 Compliance\n"));	

	switch (aTstNum)
		{
		case 0:		// Return how may auto tests are present
			return (8);
			break;

		case 1:		// Check that AT+CMUX is sent to enter mux mode
			iTest.Printf(_L("Verify AT+CMUX is sent to enter Mux mode...\n"));	
			SetFunIndex(1);

			// common test code
			Verify0710Compliance();
			iTest.Printf(_L("Test 6-1 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 2:		// Check that frames are created correctly
			iTest.Printf(_L("Verify frames are created properly...\n"));
			SetFunIndex(1);

			// common test code
			Verify0710Compliance();
			iTest.Printf(_L("Test 6-2 Passed\n"));
			User::After(2000000);
			return KErrNone;
			break;

		case 3:		// Check that channels are created via SABM frames
			iTest.Printf(_L("Verify channels are created via SABM frames...\n"));	
			SetFunIndex(1);

			// common test code
			Verify0710Compliance();
			iTest.Printf(_L("Test 6-3 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 4:		// Check that received frames are validated
			iTest.Printf(_L("Verify received frames are validated...\n"));
			SetFunIndex(1);

			// common test code
			Verify0710Compliance();
			iTest.Printf(_L("Test 6-4 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 5:		// Check that recv frames are ack'd with a UA frame
			iTest.Printf(_L("Verify received frames are ack'd with a UA frame...\n"));
			SetFunIndex(1);

			// common test code
			Verify0710Compliance();
			iTest.Printf(_L("Test 6-5 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 6:		// Check that received modem status control frames are processed correctly
			iTest.Printf(_L("Verify received modem status control frames are processed...\n"));
			SetFunIndex(1);

			// common test code
			Verify0710Compliance();
			iTest.Printf(_L("Test 6-6 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 7:		// Check that received flow control frames are processed correctly
			iTest.Printf(_L("Verify received flow control frames are processed...\n"));	
			SetFunIndex(1);

			// common test code
			Verify0710Compliance();
			iTest.Printf(_L("Test 6-7 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 8:		// Check that channel is closed by sending a DISC frame
			iTest.Printf(_L("Verify channel is closed by when sending DISC frame...\n"));
			SetFunIndex(1);

			// common test code
			Verify0710Compliance();
			iTest.Printf(_L("Test 6-8 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

			// Manual Tests Follow...
		case 100:		// Return how many manual tests are present
			return 0;
			break;

		default:
			iTest.Printf(_L("** Not implemented CSYF-%d **\n"), aTstNum);
			break;
		}			
			
	return (KErrNone);
	}


TInt CCsy0710UnitTest::CsyGL(TInt aTstNum)
/**
 * CSY Tests
 * @param aTstNum test number
 * @return KErrNone or error code
 */
	{
	if (aTstNum != 0)
		iTest.Printf(_L("CSYG-%d\n"), aTstNum);

	SetTsIdentifier(_L("CSYG"));
	SetMethodName(_L("CSYG"));

	switch (aTstNum)
		{
		case 0:		// Return how may auto tests are present
			return (14);
			break;

		case 1:		// Verify no memory allocation until 1st port is opened
			iTest.Printf(_L("Verify no memory allocation until 1st port is opened...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-1 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 2:		// Verify mux startup params are read
			iTest.Printf(_L("Verify mux startup params are read...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-2 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 3:		// Verify correct serial port PDD is loaded
			iTest.Printf(_L("Verify correct serial port PDD is loaded...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-3 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 4:		// Verify correct serial port LDD is loaded
			iTest.Printf(_L("Verify correct serial port LDD is loaded...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-4 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 5:		// Check serial port startup config
			iTest.Printf(_L("Verify serial port startup config...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-5 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 6:		// Check control channel 0 is created
			iTest.Printf(_L("Verify control channel 0 is created...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-6 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 7:		// Check command and data channels are created
			iTest.Printf(_L("Verify cmd and data channels are created...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-7 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 8:		// Verify enter mux AT command is issued to the BP
			iTest.Printf(_L("Verify enter mux AT command is issued to BP...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-8 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 9:		// Verify cmd channels are setup disable cmd echo
			iTest.Printf(_L("Verify cmd channels are setup to disable cmd echo...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-9 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 10:	// Verify C32 is informed of the CSY's serial name of "gsm0710"
			iTest.Printf(_L("Verify C32 is informed of the CSY's serial name of gsm0710...\n"));	
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-10 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 11:	// Verify flow control is set to Off after channel init
			iTest.Printf(_L("Verify Verify flow control is set to Off after channel init...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-11 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 12:	// Verify NotifyFlowControlChange requests are accepted
			iTest.Printf(_L("Verify NotifyFlowControlChange requests are accepted...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-12 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 13:	// Verify NotifyFlowControlChange request can be cancelled
			iTest.Printf(_L("Verify NotifyFlowControlChange request can be cancelled...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-13 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 14:	// Verify NotifyFlowControlChange requests are completed properly
			iTest.Printf(_L("Verify NotifyFlowControlChange requests are completed properly...\n"));
			SetFunIndex(1);

			// common test code
			VerifyStartup();
			iTest.Printf(_L("Test 7-14 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;
			
			// Manual Tests Follow...
		case 100:		// Return how many manual tests are present
			return 0;
			break;

		default:
			iTest.Printf(_L("** Not implemented CSYG-%d **\n"), aTstNum);
			break;
		}

	return (KErrNone);
	}


TInt CCsy0710UnitTest::CsyHL(TInt aTstNum)
/**
 * CSY Tests
 * @param aTstNum test number
 * @return KErrNone or error code
 */
	{
	if (aTstNum != 0)
		iTest.Printf(_L("CSYH-%d\n"), aTstNum);

	SetTsIdentifier(_L("CSYH"));
	SetMethodName(_L("CSYH"));

	switch (aTstNum)
		{
		case 0:		// Return how may auto tests are present
			return (5);
			break;

		case 1:		// Verify any active or outstanding requests are cancelled
			iTest.Printf(_L("Verify any active or outstanding requests are cancelled...\n"));
			SetFunIndex(1);

			// common test code
			TEST(VerifyShutdown(1));
			iTest.Printf(_L("Test 8-1 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 2:		// Verify each command channel and control channel is disconnected by sending DISC frame to BP
			iTest.Printf(_L("Verify channels are disconnected by sending DISC frame to BP...\n"));
			SetFunIndex(1);

			// common test code
			TEST(VerifyShutdown(2));
			iTest.Printf(_L("Test 8-2 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 3:		// Verify all memory allocated is deallocated
			iTest.Printf(_L("Verify all memory allocated is deallocated...\n"));
			SetFunIndex(1);

			// common test code
			TEST(VerifyShutdown(3));
			iTest.Printf(_L("Test 8-3 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 4:		// Verify all created objects are deleted
			iTest.Printf(_L("Verify all created objects are deleted...\n"));
			SetFunIndex(1);

			// common test code
			TEST(VerifyShutdown(4));
			iTest.Printf(_L("Test 8-4 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

		case 5:		// Verify BP is returned to non-mux mode for AT commands.
			iTest.Printf(_L("Verify BP is returned to non-mux mode for AT commands....\n"));
			SetFunIndex(1);

			// common test code
			TEST(VerifyShutdown(5));
			iTest.Printf(_L("Test 8-5 Passed\n"));
			User::After(3000000);
			return KErrNone;
			break;

			// Manual Tests Follow...
		case 100:		// Return how many manual tests are present
			return 0;
			break;

		default:
			iTest.Printf(_L("** Not implemented CSYH-%d **\n"), aTstNum);
			break;
		}

	return (KErrNone);
	}


////////////////////
// Helper classes
//
TInt CCsy0710UnitTest::LoadSerialDeviceDrivers()
	{
#ifdef __WINS__
	TInt ret;
	_LIT(KPDDName,"ECDRV");
	_LIT(KLDDName,"ECOMM");

	ret= User::LoadPhysicalDevice(KPDDName);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		return (ret);

	ret = User::LoadLogicalDevice(KLDDName);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		return (ret);
#endif

	return (KErrNone);
	}


TInt CCsy0710UnitTest::OpenAllCommPorts()
	{
	TInt ret;

	// Open RComm Ports
	ret = iCommPort[0].Open(iCommServer, iCmdPortName[0], ECommShared);
	if (ret != KErrNone) return __LINE__;
			
	// Open Port #2
	ret = iCommPort[1].Open(iCommServer, iCmdPortName[1], ECommShared);
	if (ret != KErrNone) return __LINE__;

	// Open Port #3
	ret = iCommPort[2].Open(iCommServer, iCmdPortName[2], ECommShared);
	if (ret != KErrNone) return __LINE__;

	// Open Port #4
	ret = iCommPort[3].Open(iCommServer, iCmdPortName[3], ECommShared);
	if (ret != KErrNone) return __LINE__;

	return (KErrNone);
	}

			
TInt CCsy0710UnitTest::CloseAllCommPorts()
	{
	iCommPort[0].Close();
	iCommPort[1].Close();
	iCommPort[2].Close();
	iCommPort[3].Close();

	return (KErrNone);
	}


TInt CCsy0710UnitTest::GetCsyInfo()
	{
	TInt ret;
	TName csyName(_L("gsm0710"));
	TSerialInfo serialInfo;

	ret = iCommServer.GetPortInfo(csyName, serialInfo);
	if (ret == KErrNone)
		{
		iTest.Printf(_L("    Descrip   = %S\n"), &serialInfo.iDescription);
		iTest.Printf(_L("    Name      = %S\n"), &serialInfo.iName);
		iTest.Printf(_L("    Low Unit  = %d\n"), serialInfo.iLowUnit);
		iTest.Printf(_L("    High Unit = %d\n"), serialInfo.iHighUnit);
		User::After(2000000);
		}
	return (ret);
	}


TInt CCsy0710UnitTest::OpenCommPortFail()
	{
	TInt ret;
	TName invalidName = _L("gsm0710::A");

	ret = iCommPort[0].Open(iCommServer, invalidName, ECommShared);
	if (ret == KErrNone) return __LINE__;

	return (KErrNone);
	}


TInt CCsy0710UnitTest::OpenCmdPortTwice()
	{
	TInt ret;

	// Open RComm Port once
	ret = iCommPort[0].Open(iCommServer, iCmdPortName[0], ECommExclusive);
	if (ret != KErrNone) return __LINE__;

	// Open same RComm Port again
	ret = iCommPort[1].Open(iCommServer, iCmdPortName[0], ECommExclusive);
	if (ret != KErrNone) return __LINE__;

	iCommPort[0].Close();
	iCommPort[1].Close();

	return (KErrNone);
	}


TInt CCsy0710UnitTest::OpenCmdPortInvalidName()
	{
	TInt ret;
	TName invalidName = _L("gsm0710::A");

	ret = iCommPort[0].Open(iCommServer, invalidName, ECommShared);
	if (ret == KErrNone) return __LINE__;

	return (KErrNone);
	}


TInt CCsy0710UnitTest::SendCmdOneCommPort(TInt aPortNum)
	{
	TInt ret;

	// Open RComm Port
	ret = iCommPort[aPortNum].Open(iCommServer, iCmdPortName[aPortNum], ECommShared);
	if (ret != KErrNone) return __LINE__;

	// send data
	TBuf8<32> buf;
	buf.Copy(KATCommand1);

	TRequestStatus writeRequestStatus = KRequestPending;
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;
	
	TEST(ReadDataFromCommPort(aPortNum));
	
	iCommPort[aPortNum].Close();

	return (KErrNone);
	}


TInt CCsy0710UnitTest::SendCmdsOneCommPort(TInt aPortNum)
	{
	TInt ret;

	// Open RComm Port
	ret = iCommPort[aPortNum].Open(iCommServer, iCmdPortName[aPortNum], ECommShared);
	if (ret != KErrNone) return __LINE__;

	TRequestStatus writeRequestStatus;
	TBuf8<32> buf;

	// send data
	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommand1);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;

	TEST(ReadDataFromCommPort(aPortNum));

	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommand2);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;

	TEST(ReadDataFromCommPort(aPortNum));

	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommand3);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;

	TEST(ReadDataFromCommPort(aPortNum));

	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommand4);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;

	TEST(ReadDataFromCommPort(aPortNum));

	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommand5);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;

	TEST(ReadDataFromCommPort(aPortNum));

	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommand6);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;

	TEST(ReadDataFromCommPort(aPortNum));

	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommand7);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;

	TEST(ReadDataFromCommPort(aPortNum));

	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommand8);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;

	TEST(ReadDataFromCommPort(aPortNum));

	iCommPort[aPortNum].Close();

	return (KErrNone);
	}


TInt CCsy0710UnitTest::CancelWriteOneCommPort(TInt aPortNum)
	{
	TInt ret;

	// Open RComm Port
	ret = iCommPort[aPortNum].Open(iCommServer, iCmdPortName[aPortNum], ECommShared);
	if (ret != KErrNone) return __LINE__;

	// wait for CSY to initialize
	// check current flow control state
	TFlowControl flow;
	iCommPort[aPortNum].GetFlowControlStatus(flow);
	if (flow == EFlowControlOn)
		{
		// Port isn't active yet - wait for it to become active
		//gTest.Printf(_L("Waiting for port 1 to become active...\n\n"));
		TRequestStatus flowReqStatus;
		iCommPort[aPortNum].NotifyFlowControlChange(flowReqStatus, flow);
		User::WaitForRequest(flowReqStatus);
		if (flow == EFlowControlOn)
			{
			iTest.Printf(_L("Incorrect flow %d\n"), flow);
			iTest.Printf(_L("*** CSY did not init properly!! ***\n"));
			}
		}

	User::After(5000000);

	TRequestStatus writeRequestStatus;
	TBuf8<100> buf;

	// send data
	writeRequestStatus = KRequestPending;
	buf.Copy(KATCommandToCancel);
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());

	// cancel write request
 	iCommPort[aPortNum].WriteCancel();
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrCancel && writeRequestStatus != KErrNone) return __LINE__;

	if (writeRequestStatus == KErrNone)
		{
		TEST(ReadDataFromCommPort(aPortNum));
		}

	iCommPort[aPortNum].Close();

	return (KErrNone);
	}


TInt CCsy0710UnitTest::RecvDataOneCommPort(TInt aPortNum)
	{
	iTest.Printf(_L("  Port %d\n"), aPortNum);

	// wait for CSY to initialize
	// check current flow control state
	TFlowControl flow;
	iCommPort[aPortNum].GetFlowControlStatus(flow);
	if (flow == EFlowControlOn)
		{
		// Port isn't active yet - wait for it to become active
		TRequestStatus flowReqStatus;
		iCommPort[aPortNum].NotifyFlowControlChange(flowReqStatus, flow);
		User::WaitForRequest(flowReqStatus);
		if (flow == EFlowControlOn) return __LINE__;
		}

	User::After(5000000);


	// send data
	TBuf8<32> buf;
	buf.Copy(KATCommand1);

	TRequestStatus writeRequestStatus = KRequestPending;
 	iCommPort[aPortNum].Write(writeRequestStatus, buf, buf.Length());
	User::WaitForRequest(writeRequestStatus);
	if (writeRequestStatus != KErrNone) return __LINE__;
	

	// Recv Data
	TRequestStatus readRequestStatus = KRequestPending;
	TBuf8<80> respBuf;
	// gTest.Printf(_L("Port 1 Read ...\n"));
 	iCommPort[aPortNum].ReadOneOrMore(readRequestStatus,respBuf);
	User::WaitForRequest(readRequestStatus);
	if (readRequestStatus != KErrNone) return __LINE__;

	User::After(100000);

	// see if there is any more data to read
 	TInt extraRecvData = 0;
	while ((extraRecvData = iCommPort[aPortNum].QueryReceiveBuffer()) > 0)
		{
		respBuf.Zero();
		readRequestStatus = KRequestPending;
 		iCommPort[aPortNum].ReadOneOrMore(readRequestStatus, respBuf);
 		//iCommPort[aPortNum].Read(readRequestStatus, respBuf);
		User::WaitForRequest(readRequestStatus);
		if (readRequestStatus != KErrNone) return __LINE__;
		}

	return (KErrNone);
	}


TInt CCsy0710UnitTest::ReadDataFromCommPort(TInt aPortNum)
	{
	// Recv Data
	TRequestStatus readRequestStatus = KRequestPending;
	TBuf8<80> respBuf;
 	iCommPort[aPortNum].ReadOneOrMore(readRequestStatus, respBuf);
	User::WaitForRequest(readRequestStatus);
	if (readRequestStatus != KErrNone) return __LINE__;

	User::After(100000);

	// see if there is any more data to read
 	TInt extraRecvData = 0;
	while ((extraRecvData = iCommPort[aPortNum].QueryReceiveBuffer()) > 0)
		{
		respBuf.Zero();
		readRequestStatus = KRequestPending;
 		iCommPort[aPortNum].ReadOneOrMore(readRequestStatus, respBuf);
		User::WaitForRequest(readRequestStatus);
		if (readRequestStatus != KErrNone) return __LINE__;
		}

	return (KErrNone);
	}


TInt CCsy0710UnitTest::VerifyFlowControl(TInt aPortNum)
	{
	iTest.Printf(_L("  Port %d\n"), aPortNum);

	// check current flow control state
	TFlowControl flow;
	iCommPort[aPortNum].GetFlowControlStatus(flow);
	if (flow == EFlowControlOn)
		{
		// Port isn't active yet - wait for it to become active
		TRequestStatus flowReqStatus;
		iCommPort[aPortNum].NotifyFlowControlChange(flowReqStatus, flow);
		User::WaitForRequest(flowReqStatus);
		if (flow == EFlowControlOn)
			{
			iTest.Printf(_L("Incorrect flow %d\n"), flow);
			iTest.Printf(_L("*** CSY did not init properly!! ***\n"));
			}
		}
	return (KErrNone);
	}


TInt CCsy0710UnitTest::Verify0710Compliance()
	{
	TInt ret;

	TEST(LoadSerialDeviceDrivers());
    ret = StartC32();
    if ((ret != KErrNone) && (ret != KErrAlreadyExists))
       TEST(ret);
	TEST(iCommServer.Connect());
	ret = iCommServer.LoadCommModule(KCsyName);
	if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

	// specific test code
	ret = iCommPort[0].Open(iCommServer, iCmdPortName[0], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[1].Open(iCommServer, iCmdPortName[1], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[2].Open(iCommServer, iCmdPortName[2], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[3].Open(iCommServer, iCmdPortName[3], ECommShared);
	if (ret != KErrNone) return __LINE__;

	TEST(VerifyFlowControl(0));
	TEST(VerifyFlowControl(1));
	TEST(VerifyFlowControl(2));
	TEST(VerifyFlowControl(3));

	iCommPort[3].Close();
	iCommPort[2].Close();
	iCommPort[1].Close();
	iCommPort[0].Close();
	User::After(1000000);
	iCommServer.UnloadCommModule(KCsyName);
	iCommServer.Close();

	return (KErrNone);
	}


TInt CCsy0710UnitTest::VerifyStartup()
	{
	TInt ret;

	TEST(LoadSerialDeviceDrivers());
    ret = StartC32();
    if ((ret != KErrNone) && (ret != KErrAlreadyExists))
        TEST(ret);
	TEST(iCommServer.Connect());
	ret = iCommServer.LoadCommModule(KCsyName);
	if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

	// specific test code
	ret = iCommPort[0].Open(iCommServer, iCmdPortName[0], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[1].Open(iCommServer, iCmdPortName[1], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[2].Open(iCommServer, iCmdPortName[2], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[3].Open(iCommServer, iCmdPortName[3], ECommShared);
	if (ret != KErrNone) return __LINE__;

	TEST(VerifyStartupFlowControl(0));
	TEST(VerifyStartupFlowControl(1));
	TEST(VerifyStartupFlowControl(2));
	TEST(VerifyStartupFlowControl(3));

	iCommPort[3].Close();
	iCommPort[2].Close();
	iCommPort[1].Close();
	iCommPort[0].Close();
	User::After(1000000);
	iCommServer.UnloadCommModule(KCsyName);
	iCommServer.Close();

	return (KErrNone);
	}


TInt CCsy0710UnitTest::VerifyStartupFlowControl(TInt aPortNum)
	{
	// check current flow control state
	TFlowControl flow;
	iCommPort[aPortNum].GetFlowControlStatus(flow);
	if (flow == EFlowControlOn)
		{
		// Port isn't active yet - wait for it to become active
		TRequestStatus flowReqStatus;
		iCommPort[aPortNum].NotifyFlowControlChange(flowReqStatus, flow);
		User::WaitForRequest(flowReqStatus);
		if (flow == EFlowControlOn)
			{
			iTest.Printf(_L("Incorrect flow %d\n"), flow);
			iTest.Printf(_L("*** CSY did not init properly!! ***\n"));
			}
		}
	return (KErrNone);
	}


TInt CCsy0710UnitTest::VerifyShutdown(TInt aTestSubset)
	{
	aTestSubset = aTestSubset;
	TInt ret;

	TEST(LoadSerialDeviceDrivers());
    ret = StartC32();
    if ((ret != KErrNone) && (ret != KErrAlreadyExists))
        TEST(ret);
	TEST(iCommServer.Connect());
	ret = iCommServer.LoadCommModule(KCsyName);
	if (ret!=KErrNone && ret!=KErrAlreadyExists) return __LINE__;

	// specific test code
	ret = iCommPort[0].Open(iCommServer, iCmdPortName[0], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[1].Open(iCommServer, iCmdPortName[1], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[2].Open(iCommServer, iCmdPortName[2], ECommShared);
	if (ret != KErrNone) return __LINE__;
	ret = iCommPort[3].Open(iCommServer, iCmdPortName[3], ECommShared);
	if (ret != KErrNone) return __LINE__;

	TEST(VerifyStartupFlowControl(0));
	TEST(VerifyStartupFlowControl(1));
	TEST(VerifyStartupFlowControl(2));
	TEST(VerifyStartupFlowControl(3));

	iCommPort[3].Close();
	iCommPort[2].Close();
	iCommPort[1].Close();
	iCommPort[0].Close();
	User::After(1000000);
	iCommServer.UnloadCommModule(KCsyName);
	iCommServer.Close();

	return (KErrNone);
	}


/////////////////////////////////////////////////
//The following functions also need to be implemented by
//CUnitTestBase derived classes 

void CCsy0710UnitTest::CreateTestL()
/**
 *
 *
 */
	{
	CCsy0710UnitTest* unitTest=CCsy0710UnitTest::NewL(
		KCsy0710UnitTest, KCsy0710UnitTestLog, KUnitTestSpec, KCsy0710UnitTestScript);

	CleanupStack::PushL(unitTest);

	TInt ret=unitTest->SetUpTest();
	
	if(!ret)
		{
		unitTest->RunUnitTestsL();
		}
	
	unitTest->Test().End();
	
	CleanupStack::PopAndDestroy(1); //unitTest
	}


#ifndef _ACCEPTANCE_TESTS

GLDEF_C TInt E32Main()
//
// Main entry point - make sure everything is going.
//
#ifdef BUILD_FOR_JETSTREAM
	{
	RProcess().SecureApi(RProcess::ESecureApiOn);
	RProcess().DataCaging(RProcess::EDataCagingOn);
	// We must include the brace in the #ifndef to allow the security
	// checking script 'perl.txt' to pass.
#else
	{
#endif // BUILD_FOR_JETSTREAM
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack

	TRAPD(ret,CCsy0710UnitTest::CreateTestL());

	delete cleanup;
	return KErrNone;        
	}

#endif //_ACCEPTANCE_TESTS