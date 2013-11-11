// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests for packet loopback CSY
// 
//

/**
 @file
*/

#include <e32property.h>
#include <c32comm.h>
#include <e32def.h>
#include <e32base.h>

#include <test/testexecutelog.h>

#include "LoopbackTestStep.h"
#include "LoopbackConfig.h"

_LIT(KCsyName,"PKTLOOPBACK");
_LIT(KPortName1,"PKTLOOPBACK::501");
const TUint KPortUnitNum1 = 501;
_LIT(KPortName2,"PKTLOOPBACK::502");
const TUint KPortUnitNum2 = 502;
_LIT(KPortName3,"PKTLOOPBACK::503");
_LIT(KPortName4,"PKTLOOPBACK::504");
_LIT8(KWriteBuf1,"1Packet data1");
_LIT8(KWriteBuf2,"2Packet data2");
_LIT8(KWriteBuf3,"3Packet data3");
_LIT8(KWriteBuf4,"4Packet data4");

const TUint KConfiguredPortDelayMicroseconds = 100000;
const TUint KConfiguredPortLongDelayMicroseconds = 1000000;

const TUint KTooSmallBufferSize = 4;
const TUint KRegularBufferSize = 64;
const TUint KHugeBufferSize = 10000;

const TUint KLoopackConfigTestNumber1 = 1;
const TUint KLoopackConfigTestNumber2 = 2;
const TUint KLoopackConfigTestNumber3 = 3;
const TUint KLoopackConfigTestNumber4 = 4;
const TUint KLoopackConfigTestNumber5 = 5;
const TUint KLoopackConfigTestNumber6 = 6;
const TUint KLoopackConfigTestNumber7 = 7;
const TUint KLoopackConfigTestNumber8 = 8;
const TUint KLoopackConfigTestNumber9 = 9;
const TUint KLoopackConfigTestNumber10 = 10;
const TUint KLoopackConfigTestNumber11 = 11;
const TUint KLoopackConfigTestNumber12 = 12;
const TUint KLoopackConfigTestNumber13 = 13;

CLoopbackTestStepBase::CLoopbackTestStepBase(TInt aTestNumber)
	{
	iTestNumber = aTestNumber;
	}

CLoopbackTestStepBase::~CLoopbackTestStepBase()
	{
	iCommPort1.Close();
	iCommPort2.Close();
	iCommPort3.Close();
	iCommPort4.Close();
	iCommServer.Close();
	}

void CLoopbackTestStepBase::SetTestNumber(TInt aTestNumber)
  	{
  	TInt dummy;
	TInt ret = RProperty::Get(KUidPSPacketLoopbackCsyCategory, KPSLoopbackCsyTestNumber, dummy);
	if(ret == KErrNotFound)
		{
		RProperty::Define(KUidPSPacketLoopbackCsyCategory, KPSLoopbackCsyTestNumber, 
							KPSLoopbackCsyTestNumberKeyType);
		}		
	ret = RProperty::Set(KUidPSPacketLoopbackCsyCategory, KPSLoopbackCsyTestNumber, aTestNumber);
  	}

void CLoopbackTestStepBase::SetFlowControl(TUint aUnit, TBool aFlowControlOn)
  	{
  	TInt dummy;
	TInt ret = RProperty::Get(KUidPSCsyFlowControlCategory, aUnit, dummy);
	if(ret == KErrNotFound)
		{
		RProperty::Define(KUidPSCsyFlowControlCategory, aUnit, KPSLoopbackCsyFlowControlOnKeyType);
		}
	ret = RProperty::Set(KUidPSCsyFlowControlCategory, aUnit, aFlowControlOn);
  	}

void CLoopbackTestStepBase::SetReadResult(TUint aUnit, TInt aReadResult)
  	{
  	TInt dummy;
	TInt ret = RProperty::Get(KUidPSCsyReadResultCategory, aUnit, dummy);
	if(ret == KErrNotFound)
		{
		RProperty::Define(KUidPSCsyReadResultCategory, aUnit, KUidPSCsyReadResultCategoryKeyType);
		}
	ret = RProperty::Set(KUidPSCsyReadResultCategory, aUnit, aReadResult);
  	}

void CLoopbackTestStepBase::SetWriteResult(TUint aUnit, TInt aWriteResult)
  	{
  	TInt dummy;
	TInt ret = RProperty::Get(KUidPSCsyWriteResultCategory, aUnit, dummy);
	if(ret == KErrNotFound)
		{
		RProperty::Define(KUidPSCsyWriteResultCategory, aUnit, KUidPSCsyWriteResultCategoryKeyType);
		}
	ret = RProperty::Set(KUidPSCsyWriteResultCategory, aUnit, aWriteResult);
  	}

TVerdict CLoopbackTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;
	SetTestNumber(iTestNumber);

	TestErrorCodeL( iCommServer.Connect(), _L("Connect to comm server") );
	TestErrorCodeL( iCommServer.LoadCommModule(KCsyName), _L("Loading comm module") );

	return TestStepResult();
	}
	
TVerdict CLoopbackTestStepBase::doTestStepPostambleL()
	{
	User::LeaveIfError(KErrNone);	// To prevent LeaveScan warning

	iCommPort1.Close();
	iCommPort2.Close();
	iCommPort3.Close();
	iCommPort4.Close();
	iCommServer.Close();

	__UHEAP_MARKEND;
	return EPass;
	}
	
void CLoopbackTestStepBase::TestErrorCodeL(TInt aErrCode, const TDesC& aMessage)
	{	
	TestErrorCodeL(aErrCode, KErrNone, aMessage);
	}

void CLoopbackTestStepBase::TestErrorCodeL(TInt aErrCode, TInt aExpectedErrCode, const TDesC& aMessage)
	{	
	if(aExpectedErrCode == aErrCode)
		{
		INFO_PRINTF3(_L("[%S]. err[%d], as expected. OK."), &aMessage, aErrCode);
		}
	else
		{
		ERR_PRINTF5(_L("Failed: [%S]. err[%d], expected [%d]. Leaving with [%d])."), &aMessage, aErrCode, aExpectedErrCode, aErrCode);
		User::Leave(aErrCode);
		}
	}

void CLoopbackTestStepBase::TestBooleanTrueL(TInt aBool, const TDesC& aMessage)
	{	
	if(aBool)
		{
		INFO_PRINTF2(_L("[%S] is true, as expected. OK."), &aMessage);
		}
	else
		{
		ERR_PRINTF2(_L("Failed: [%S] is false. Leaving with KErrGeneral)."), &aMessage);
		User::Leave(KErrGeneral);
		}
	}

void CLoopbackTestStepBase::TestErrorCodeAndDescriptorL(TInt aErrCode, TInt aExpectedErrCode, const TDesC8& aDesC, const TDesC8& aExpectedDesC, const TDesC& aMessage)
	{
	if ( (aExpectedErrCode == aErrCode) && (aDesC.Compare(aExpectedDesC) == 0) )
		{
		INFO_PRINTF4(_L("[%S]. err[%d], and string [%S] as expected. OK."), &aMessage, aErrCode, &aExpectedDesC);
		}
	else if (aExpectedErrCode == aErrCode)
		{
		ERR_PRINTF5(_L("Failed: [%S]. err[%d], expected [%d]. Leaving with [%d])."), &aMessage, aErrCode, aExpectedErrCode, aErrCode);
		User::Leave(aErrCode);
		}
	else
		{
		ERR_PRINTF4(_L("Failed: [%S]. String [%S] received, expected [%S]. Leaving with KErrGeneral."), &aMessage, &aDesC, &aExpectedDesC);
		User::Leave(KErrGeneral);
		}
	}
	
void CLoopbackTestStepBase::TestErrorCodeAndDescriptorL(TInt aErrCode, const TDesC8& aDesC, const TDesC8& aExpectedDesC, const TDesC& aMessage)
	{
	TestErrorCodeAndDescriptorL(aErrCode, KErrNone, aDesC, aExpectedDesC, aMessage);
	}
	
CLoopbackTestStep1::~CLoopbackTestStep1()
	{
	}

CLoopbackTestStep1::CLoopbackTestStep1() : CLoopbackTestStepBase(KLoopackConfigTestNumber1)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep1);
	}
	
TVerdict CLoopbackTestStep1::doTestStepL()
/**
Test that two different sets of loopback ports can read/write to their corresponding loopback port
The test's ini file is configured to have commPort1 and commPort2 as loopback ports, and 
commPort3 and commPort4 as loopback ports
*/
	{
	// open the 4 ports
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
	TestErrorCodeL( iCommPort3.Open(iCommServer, KPortName3, ECommExclusive, ECommRoleDCE), _L("Opening comm port 3") );
	TestErrorCodeL( iCommPort4.Open(iCommServer, KPortName4, ECommExclusive, ECommRoleDCE), _L("Opening comm port 4") );

	// There are a total of 4 writes (one to and from each port)
	// 2 of the reads occur before the write and 2 after
	
	// Issue the two reads before the writes
	TRequestStatus readStatus1, readStatus2;
	TBuf8<KRegularBufferSize> readBuf1;
	TBuf8<KRegularBufferSize> readBuf2;
	iCommPort1.ReadOneOrMore(readStatus1, readBuf1);
	iCommPort2.ReadOneOrMore(readStatus2, readBuf2);
	
	// Issue the writes on each port
	TRequestStatus writeStatus1, writeStatus2, writeStatus3, writeStatus4;
	iCommPort1.Write(writeStatus1, KWriteBuf1);
	iCommPort2.Write(writeStatus2, KWriteBuf2);
	iCommPort3.Write(writeStatus3, KWriteBuf3);
	iCommPort4.Write(writeStatus4, KWriteBuf4);

	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));
	User::WaitForRequest(writeStatus2);
	TestErrorCodeL(writeStatus2.Int(), _L("Writing to comm port 2"));
	User::WaitForRequest(writeStatus3);
	TestErrorCodeL(writeStatus3.Int(), _L("Writing to comm port 3"));
	User::WaitForRequest(writeStatus4);
	TestErrorCodeL(writeStatus4.Int(), _L("Writing to comm port 4"));

	// Check that the first 2 reads completed successfully
	User::WaitForRequest(readStatus1);
	TestErrorCodeAndDescriptorL(readStatus1.Int(), readBuf1, KWriteBuf2, _L("Read from comm port 1"));
	User::WaitForRequest(readStatus2);
	TestErrorCodeAndDescriptorL(readStatus2.Int(), readBuf2, KWriteBuf1, _L("Read from comm port 2"));

	// Issue the 2 reads after the writes and make sure they complete successfully
	TRequestStatus readStatus3, readStatus4;
	TBuf8<KRegularBufferSize> readBuf3;
	TBuf8<KRegularBufferSize> readBuf4;

	iCommPort3.ReadOneOrMore(readStatus3, readBuf3);
	User::WaitForRequest(readStatus3);
	TestErrorCodeAndDescriptorL(readStatus3.Int(), readBuf3, KWriteBuf4, _L("Read from comm port 3"));

	iCommPort4.ReadOneOrMore(readStatus4, readBuf4);
	User::WaitForRequest(readStatus4);
	TestErrorCodeAndDescriptorL(readStatus4.Int(), readBuf4, KWriteBuf3, _L("Read from comm port 4"));

	return TestStepResult();
	}

CLoopbackTestStep2::~CLoopbackTestStep2()
	{
	}

CLoopbackTestStep2::CLoopbackTestStep2() : CLoopbackTestStepBase(KLoopackConfigTestNumber2)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep2);
	}
	
TVerdict CLoopbackTestStep2::doTestStepL()
/**
Test that writes are successfully queued and sent.
The 2 ports opened are configured as loopback ports in the ini file
*/
	{
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
		
	// Issue 2 writes immediately one after the other in each direction
	TRequestStatus writeStatus1, writeStatus2, writeStatus3, writeStatus4;
	iCommPort1.Write(writeStatus1, KWriteBuf1);
	iCommPort1.Write(writeStatus2, KWriteBuf2);
	
	iCommPort2.Write(writeStatus3, KWriteBuf3);
	iCommPort2.Write(writeStatus4, KWriteBuf4);
	
	TBool writesSuccessful = EFalse;
	User::WaitForRequest(writeStatus1);
	User::WaitForRequest(writeStatus2);
	User::WaitForRequest(writeStatus3);
	User::WaitForRequest(writeStatus4);
	if (writeStatus1 == KErrNone && writeStatus2 == KErrNone && 
		writeStatus3 == KErrNone && writeStatus4 == KErrNone)
		{
		writesSuccessful = ETrue;
		}
	TestBooleanTrueL(writesSuccessful, _L("Writing to comm ports successful"));

	// Issue the reads
	TRequestStatus readStatus1, readStatus2, readStatus3, readStatus4;
	TBuf8<KRegularBufferSize> readBuf1, readBuf2, readBuf3, readBuf4;
	iCommPort2.ReadOneOrMore(readStatus1, readBuf1);
	User::WaitForRequest(readStatus1);
	iCommPort2.ReadOneOrMore(readStatus2, readBuf2);
	User::WaitForRequest(readStatus2);
	iCommPort1.ReadOneOrMore(readStatus3, readBuf3);
	iCommPort1.ReadOneOrMore(readStatus4, readBuf4);
	User::WaitForRequest(readStatus4);
	User::WaitForRequest(readStatus3);

	// Make sure the requests are all successful
	TestBooleanTrueL(readStatus1 == KErrNone && readStatus2 == KErrNone && 
					 readStatus3 == KErrNone && readStatus4 == KErrNone,
					 _L("Check that reading from comm ports returned no errors"));
	TestBooleanTrueL(readBuf1.Compare(KWriteBuf1) == 0 && readBuf2.Compare(KWriteBuf2) == 0 &&
					 readBuf3.Compare(KWriteBuf3) == 0 || readBuf4.Compare(KWriteBuf4) == 0,
					 _L("Check that correct values were read from comm ports"));

	return TestStepResult();
	}

CLoopbackTestStep3::~CLoopbackTestStep3()
	{
	}

CLoopbackTestStep3::CLoopbackTestStep3() : CLoopbackTestStepBase(KLoopackConfigTestNumber3)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep3);
	}
	
TVerdict CLoopbackTestStep3::doTestStepL()
/**
Test the 3 different orders in which read/write operations can occur as follows:
	1. The read is issued, then the write is issued, then the data is sent across the port
	2. The write is issued, then the read is issued, then the data is sent across the port
	3. The write is issued, then the data is sent across the port, then the read is issued
*/
	{
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );

	TRequestStatus readStatus1, readStatus2;
	TBuf8<KRegularBufferSize> readBuf1, readBuf2;
	TRequestStatus writeStatus1;

	// 1. The read is issued, then the write is issued, then the data is sent across the port
	
	// fist issue the read
	iCommPort1.ReadOneOrMore(readStatus1, readBuf1);

	// second issue the write
	iCommPort2.Write(writeStatus1, KWriteBuf1);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));
		
	User::WaitForRequest(readStatus1);
	TestErrorCodeAndDescriptorL(readStatus1.Int(), readBuf1, KWriteBuf1, _L("Read from comm port 1"));

	// 2. The write is issued, then the read is issued, then the data is sent across the port
	
	// first issue the write
	iCommPort1.Write(writeStatus1, KWriteBuf2);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));
		
	// second issue the read before the data has the chance to be sent across the port
	iCommPort2.ReadOneOrMore(readStatus2, readBuf2);
	User::WaitForRequest(readStatus2);
	TestErrorCodeAndDescriptorL(readStatus2.Int(), readBuf2, KWriteBuf2, _L("Read from comm port 2"));
		
	// 3. The write is issued, then the data is sent across the port, then the read is issued
	
	// first issue the write
	iCommPort1.Write(writeStatus1, KWriteBuf2);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));
	// second wait until the data has the chance to be sent across the port (this delay is configured
	// to be 0.1s in the ini file, so we wait a bit longer)
	User::After(2*KConfiguredPortDelayMicroseconds);
		
	// third the read is issued
	iCommPort2.ReadOneOrMore(readStatus2, readBuf2);
	User::WaitForRequest(readStatus2);
	TestErrorCodeAndDescriptorL(readStatus2.Int(), readBuf2, KWriteBuf2, _L("Read from comm port 2"));
		
	return TestStepResult();
	}

CLoopbackTestStep4::~CLoopbackTestStep4()
	{
	}

CLoopbackTestStep4::CLoopbackTestStep4() : CLoopbackTestStepBase(KLoopackConfigTestNumber4)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep4);
	}
	
TVerdict CLoopbackTestStep4::doTestStepL()
/**
Test reading/writing to an un-opened port, or a port whose loopback port is un-opened
*/
	{
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );

	TRequestStatus readStatus;
	TBuf8<KRegularBufferSize> readBuf;
	TRequestStatus writeStatus;

	// issue read request before port is opened
	iCommPort1.ReadOneOrMore(readStatus, readBuf);
	// iCommPort2's loopback port is not yet opened, so write should fail
	iCommPort1.Write(writeStatus, KWriteBuf1);
	User::WaitForRequest(writeStatus);
	TestErrorCodeL(writeStatus.Int(), KErrNotReady, _L("Writing to comm port 1 before port 2 is opened"));
		
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );

	iCommPort2.Write(writeStatus, KWriteBuf2);
	User::WaitForRequest(writeStatus);

	// now wait for read request which should complete from before iCommPort2 was opened
	User::WaitForRequest(readStatus);

	TestErrorCodeL(writeStatus.Int(), _L("Writing to comm port 1 after port 2 is opened"));
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf, KWriteBuf2, _L("Read from comm port 2"));
		
	iCommPort2.Close();

	// iCommPort1 is now closed, so both write should fail
	iCommPort1.Write(writeStatus, KWriteBuf1);
	User::WaitForRequest(writeStatus);
	TestErrorCodeL(writeStatus.Int(), KErrNotReady, _L("Writing to comm port 1 after port 2 is closed"));
		
	return TestStepResult();
	}

CLoopbackTestStep5::~CLoopbackTestStep5()
	{
	}

CLoopbackTestStep5::CLoopbackTestStep5() : CLoopbackTestStepBase(KLoopackConfigTestNumber5)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep5);
	}
	
TVerdict CLoopbackTestStep5::doTestStepL()
/**
Test filling up the read and write queues
In this test, the read and write queues are set to be 1 entry in size
*/
	{
	INFO_PRINTF1(_L("Starting Loopback test step 5"));

	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );

	TRequestStatus readStatus1;
	TBuf8<KRegularBufferSize> readBuf1;
	TRequestStatus writeStatus1, writeStatus2;

	// Issue 2 writes immediately after each other. Since the queue is only set to 1 entry
	// it will be full after the first write, so the second should return with KErrOverflow
	iCommPort2.Write(writeStatus1, KWriteBuf1);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 2"));
		
	iCommPort2.Write(writeStatus2, KWriteBuf2);
	User::WaitForRequest(writeStatus2);
	TestErrorCodeL(writeStatus2.Int(), KErrOverflow, _L("Writing to comm port 2"));
		
	// wait until the data has the chance to be sent across the port (this delay is configured
	// to be 1s in the ini file, so we wait a bit longer)
	User::After(2*KConfiguredPortLongDelayMicroseconds);
		
	// After this delay, the buffer should have been 'sent' to the read queue of the loopback port
	// Again, issue 2 writes immediately after each other, the first one filling up the queue
	// meaning the second will return with KErrOverflow
	iCommPort2.Write(writeStatus2, KWriteBuf2);
	User::WaitForRequest(writeStatus2);
	TestErrorCodeL(writeStatus2.Int(), _L("Writing to comm port 2"));
		
	iCommPort2.Write(writeStatus2, KWriteBuf2);
	User::WaitForRequest(writeStatus2);
	TestErrorCodeL(writeStatus2.Int(), KErrOverflow, _L("Writing to comm port 2"));
		
	// Wait for the amount of time it should take for the data to cross the port.
	// After the delay the buffer should not be sent because the read buffer on the loopback port is full
	User::After(2*KConfiguredPortLongDelayMicroseconds);
		
	iCommPort2.Write(writeStatus2, KWriteBuf2);
	User::WaitForRequest(writeStatus2);
	TestErrorCodeL(writeStatus2.Int(), KErrOverflow, _L("Writing to comm port 2"));
		
	// Issue reads for the 2 writes that have been sent successfully
	// Since the second read has not yet been sent across the port, it should take more than the 
	// time configured to send a buffer 

	// Get the time before the reads
	TTime beforeTime;
	beforeTime.UniversalTime();
	
	iCommPort1.ReadOneOrMore(readStatus1, readBuf1);
	User::WaitForRequest(readStatus1);
	TestErrorCodeAndDescriptorL(readStatus1.Int(), readBuf1, KWriteBuf1, _L("Read from comm port 1"));

	iCommPort1.ReadOneOrMore(readStatus1, readBuf1);
	User::WaitForRequest(readStatus1);

	// Get the time after the read
	TTime afterTime;
	afterTime.UniversalTime();

	// Make sure the read was successful
	TestErrorCodeAndDescriptorL(readStatus1.Int(), readBuf1, KWriteBuf2, _L("Read from comm port 1"));
	
	// Make sure the read took at least the amount of time as the configured delay
	// We need a long time-out here because the resolution of the timer may cause it to expire prematurely
	const TTimeIntervalMicroSeconds delayTime((TInt)(0.5*KConfiguredPortLongDelayMicroseconds));
	TTime netTime = afterTime - delayTime;
	TestBooleanTrueL(netTime > beforeTime, _L("Check there was a delay writing to the port"));
	
	return TestStepResult();
	}

CLoopbackTestStep6::~CLoopbackTestStep6()
	{
	}

CLoopbackTestStep6::CLoopbackTestStep6() : CLoopbackTestStepBase(KLoopackConfigTestNumber6)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep6);
	}
	
TVerdict CLoopbackTestStep6::doTestStepL()
/**
Test reading an entry with too small a read buffer
*/
	{
	INFO_PRINTF1(_L("Starting Loopback test step 6"));

	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
	
	TRequestStatus readStatus1;
	TBuf8<KTooSmallBufferSize> smallReadBuf;
	TBuf8<KRegularBufferSize> largeEnoughReadBuf1;
	TRequestStatus writeStatus2;

	iCommPort2.Write(writeStatus2, KWriteBuf1);
	User::WaitForRequest(writeStatus2);
	TestErrorCodeL(writeStatus2.Int(), _L("Writing to comm port 1"));
		
	// Issue read that should fail from too small of a buffer
	iCommPort1.ReadOneOrMore(readStatus1, smallReadBuf);
	User::WaitForRequest(readStatus1);
	TestErrorCodeL(readStatus1.Int(), KErrOverflow, _L("Check that read fails becuase too small buffer supplied"));

	// Issue read that should succeed
	iCommPort1.ReadOneOrMore(readStatus1, largeEnoughReadBuf1);
	User::WaitForRequest(readStatus1);
	TestErrorCodeAndDescriptorL(readStatus1.Int(), largeEnoughReadBuf1, KWriteBuf1, _L("Read from comm port 1"));

	return TestStepResult();
	}

CLoopbackTestStep7::~CLoopbackTestStep7()
	{
	}

CLoopbackTestStep7::CLoopbackTestStep7() : CLoopbackTestStepBase(KLoopackConfigTestNumber7)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep7);
	}
	
TVerdict CLoopbackTestStep7::doTestStepL()
/**
Test issuing write with too large of a buffer
*/
	{
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
	
	// Create the large buffer
	HBufC8 *hugeHeapDescriptor = HBufC8::NewLC(KHugeBufferSize);
	TPtr8 hugeDescriptor = hugeHeapDescriptor->Des();
	hugeDescriptor.AppendFill('a', KHugeBufferSize);
	
	// Issue the write with the buffer that is too large
	TRequestStatus writeStatus1, readStatus1;
	iCommPort2.Write(writeStatus1, hugeDescriptor);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), KErrArgument, _L("Writing to comm port 1"));

	// Make sure write still works with normal sized buffer
	iCommPort2.Write(writeStatus1, KWriteBuf1);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));
		
	TBuf8<KRegularBufferSize> readBuf;
	iCommPort1.ReadOneOrMore(readStatus1, readBuf);
	User::WaitForRequest(readStatus1);
	if (readStatus1 != KErrNone)
		{
		INFO_PRINTF1(_L("Failed read"));
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(hugeHeapDescriptor);

	return TestStepResult();
	}

CLoopbackTestStep8::~CLoopbackTestStep8()
	{
	}

CLoopbackTestStep8::CLoopbackTestStep8() : CLoopbackTestStepBase(KLoopackConfigTestNumber8)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep8);
	}
	
TVerdict CLoopbackTestStep8::doTestStepL()
/**
Test wrapping around the queue by issuing more writes than the queue size (configured to 2 in the ini file)
*/
	{
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
	
	TRequestStatus writeStatus, readStatus;
	iCommPort2.Write(writeStatus, KWriteBuf1);
	User::WaitForRequest(writeStatus);
	TestErrorCodeL(writeStatus.Int(), _L("Writing to comm port 1"));

	iCommPort2.Write(writeStatus, KWriteBuf2);
	User::WaitForRequest(writeStatus);
	TestErrorCodeL(writeStatus.Int(), _L("Writing to comm port 1"));
		
	TBuf8<KRegularBufferSize> readBuf;
	iCommPort1.ReadOneOrMore(readStatus, readBuf);
	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf, KWriteBuf1, _L("Read from comm port 1"));

	iCommPort2.Write(writeStatus, KWriteBuf3);
	User::WaitForRequest(writeStatus);
	TestErrorCodeL(writeStatus.Int(), _L("Writing to comm port 1"));
		
	iCommPort1.ReadOneOrMore(readStatus, readBuf);
	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf, KWriteBuf2, _L("Read from comm port 1"));

	iCommPort1.ReadOneOrMore(readStatus, readBuf);
	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf, KWriteBuf3, _L("Read from comm port 1"));

	return TestStepResult();
	}

CLoopbackTestStep9::~CLoopbackTestStep9()
	{
	}

CLoopbackTestStep9::CLoopbackTestStep9() : CLoopbackTestStepBase(KLoopackConfigTestNumber9)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep9);
	}
	
TVerdict CLoopbackTestStep9::doTestStepL()
/**
Test read cancel
*/
	{
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
	
	TRequestStatus writeStatus, readStatus;
	iCommPort1.Write(writeStatus, KWriteBuf1);
	User::WaitForRequest(writeStatus);
	TestErrorCodeL(writeStatus.Int(), _L("Writing to comm port 1"));

	TBuf8<KRegularBufferSize> readBuf;
	iCommPort2.ReadOneOrMore(readStatus, readBuf);
	// Cancel the buffer before it has the chance to complete
	iCommPort2.ReadCancel();
	User::WaitForRequest(readStatus);
	TestErrorCodeL(readStatus.Int(), KErrCancel, _L("Check that cancelled request returns KErrCancel"));
	
	// Make sure the read still works
	iCommPort2.ReadOneOrMore(readStatus, readBuf);
	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf, KWriteBuf1, _L("Read from comm port 1"));

	return TestStepResult();
	}

CLoopbackTestStep10::~CLoopbackTestStep10()
	{
	}

CLoopbackTestStep10::CLoopbackTestStep10() : CLoopbackTestStepBase(KLoopackConfigTestNumber10)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep10);
	}
	
TVerdict CLoopbackTestStep10::doTestStepL()
/**
Test flow control
*/
	{
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
	
	TRequestStatus writeStatus1;
	iCommPort1.Write(writeStatus1, KWriteBuf1);
	SetFlowControl(KPortUnitNum1, ETrue);
	SetFlowControl(KPortUnitNum2, ETrue);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));

	TRequestStatus timerStatus, readStatus;
	TBuf8<KRegularBufferSize> readBuf;
	iCommPort2.ReadOneOrMore(readStatus, readBuf);

	RTimer timer;
	timer.CreateLocal();
	timer.After(timerStatus, 3*KConfiguredPortDelayMicroseconds);
	
	User::WaitForRequest(readStatus, timerStatus);
	
	SetFlowControl(KPortUnitNum1, EFalse);
	SetFlowControl(KPortUnitNum2, EFalse);
	TestBooleanTrueL(timerStatus == KErrNone && readStatus != KErrNone, _L("Check that setting flow control causes read to be stopped"));

	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf, KWriteBuf1, _L("Read from comm port 1"));

	return TestStepResult();
	}

CLoopbackTestStep11::~CLoopbackTestStep11()
	{
	}

CLoopbackTestStep11::CLoopbackTestStep11() : CLoopbackTestStepBase(KLoopackConfigTestNumber11)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep11);
	}
	
TVerdict CLoopbackTestStep11::doTestStepL()
/**
Test setting read result
*/
	{
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
	
	TRequestStatus writeStatus1;
	iCommPort1.Write(writeStatus1, KWriteBuf1);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));

	TRequestStatus readStatus;
	TBuf8<KRegularBufferSize> readBuf;
	iCommPort2.ReadOneOrMore(readStatus, readBuf);

	SetReadResult(KPortUnitNum2, KErrNotFound);

	User::WaitForRequest(readStatus);
	
	// Make sure everything still works
	SetReadResult(KPortUnitNum2, KErrNone);
	TestErrorCodeL(readStatus.Int(), KErrNotFound, _L("Check that SetReadResult causes Read to return error"));

	iCommPort1.Write(writeStatus1, KWriteBuf2);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));

	iCommPort2.ReadOneOrMore(readStatus, readBuf);
	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf, KWriteBuf2, _L("Read from comm port 1"));

	// Now test failing a write
	SetWriteResult(KPortUnitNum2, KErrNotFound);
	iCommPort2.Write(writeStatus1, KWriteBuf3);
	User::WaitForRequest(writeStatus1);
	SetWriteResult(KPortUnitNum2, KErrNone);
	TestErrorCodeL(writeStatus1.Int(), KErrNotFound, _L("Check that SetWriteResult causes Write to return error"));

	// Make sure we can't read this
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal();
	iCommPort1.ReadOneOrMore(readStatus, readBuf);
	timer.After(timerStatus, 3*KConfiguredPortDelayMicroseconds);
	
	User::WaitForRequest(readStatus, timerStatus);
	TestBooleanTrueL(timerStatus == KErrNone && readStatus != KErrNone, _L("Write that was configured to fail completed successfully"));
	
	return TestStepResult();
	}

CLoopbackTestStep12::~CLoopbackTestStep12()
	{
	}

CLoopbackTestStep12::CLoopbackTestStep12() : CLoopbackTestStepBase(KLoopackConfigTestNumber12)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep12);
	}
	
_LIT8(KWriteBuf,"12345");
_LIT8(KDoubleWriteBuf,"1234512345");
_LIT8(KWriteBufTabTerminator,"12345\t");
_LIT8(KWriteBufNewlineTerminator,"12345\n");
_LIT8(KWriteBufTabThenNewlineTerminator,"12345\t12345\n12345");
_LIT8(KWriteBufNewlineThenTabTerminator,"12345\n12345\t12345");
const TInt KWriteBufSegmentSize = 5;

TVerdict CLoopbackTestStep12::doTestStepL()
/**
Test setting read result
*/
	{
	TRequestStatus readStatus;
	TBuf8<KWriteBufSegmentSize> readBuf;
	TRequestStatus writeStatus1;

	TCommConfig commConfigBuf;
	TCommConfigV01& commConfig = commConfigBuf();
	commConfig.iTerminator[0] = '\t';
	commConfig.iTerminator[1] = '\n';
	commConfig.iTerminatorCount = 2;
	
	TestErrorCodeL( iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort1.SetConfig(commConfigBuf), _L("Opening comm port 1") );
	TestErrorCodeL( iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE), _L("Opening comm port 2") );
	TestErrorCodeL( iCommPort2.SetConfig(commConfigBuf), _L("Opening comm port 2") );
		
	// reads and writes are same size
	iCommPort1.Write(writeStatus1, KWriteBuf);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));

	iCommPort2.ReadOneOrMore(readStatus, readBuf);
	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf, KWriteBuf, _L("Read from comm port 1"));
	

	// test ReadOneOrMore
	iCommPort1.Write(writeStatus1, KWriteBufTabThenNewlineTerminator);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Writing to comm port 1"));

	TBuf8<KRegularBufferSize> readBuf2;
	iCommPort2.ReadOneOrMore(readStatus, readBuf2);
	User::WaitForRequest(readStatus);
	if (readBuf2.Compare(KWriteBufTabTerminator) != 0 || readStatus != KErrNone)
		{
		INFO_PRINTF1(_L("Failed write"));
		SetTestStepResult(EFail);
		}
	
	iCommPort2.ReadOneOrMore(readStatus, readBuf2);
	User::WaitForRequest(readStatus);
	TestErrorCodeL(readStatus.Int(), _L("Error in read status"));
	TestErrorCodeL(readBuf2.Compare(KWriteBufNewlineTerminator), _L("Incorrect buffer received"));
	
	iCommPort2.ReadOneOrMore(readStatus, readBuf2);
	User::WaitForRequest(readStatus);
	if (readBuf2.Compare(KWriteBuf) != 0 || readStatus != KErrNone)
		{
		INFO_PRINTF1(_L("Failed write"));
		SetTestStepResult(EFail);
		}
	
	iCommPort1.Write(writeStatus1, KWriteBufNewlineThenTabTerminator);
	User::WaitForRequest(writeStatus1);
	TestErrorCodeL(writeStatus1.Int(), _L("Failed write"));

	iCommPort2.ReadOneOrMore(readStatus, readBuf2);
	User::WaitForRequest(readStatus);
	TestBooleanTrueL(readBuf2.Compare(KWriteBufNewlineTerminator) == 0 && readStatus == KErrNone, _L("Failed read"));
	
	iCommPort2.ReadOneOrMore(readStatus, readBuf2);
	User::WaitForRequest(readStatus);
	TestBooleanTrueL(readBuf2.Compare(KWriteBufTabTerminator) == 0 && readStatus == KErrNone, _L("Failed read"));
	
	iCommPort2.ReadOneOrMore(readStatus, readBuf2);
	User::WaitForRequest(readStatus);
	TestBooleanTrueL(readBuf2.Compare(KWriteBuf) == 0 || readStatus == KErrNone, _L("Failed read"));

	return TestStepResult();
	}

CLoopbackTestStep13::~CLoopbackTestStep13()
	{
	}

CLoopbackTestStep13::CLoopbackTestStep13() : CLoopbackTestStepBase(KLoopackConfigTestNumber13)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KLoopbackTestStep13);
	}
	
TVerdict CLoopbackTestStep13::doTestStepL()
/**
Test setting read result
*/
	{
	TRequestStatus readStatus;
	TRequestStatus writeStatus;

	TCommConfig commConfigBuf;
	TCommConfigV01& commConfig = commConfigBuf();
	commConfig.iTerminator[0] = '\t';
	commConfig.iTerminator[1] = '\n';
	commConfig.iTerminatorCount = 2;
	
	TInt err = iCommPort1.Open(iCommServer, KPortName1, ECommExclusive, ECommRoleDCE);
	err = iCommPort1.SetConfig(commConfigBuf);
	TestErrorCodeL(err, _L("Failed loading port"));
		
	err = iCommPort2.Open(iCommServer, KPortName2, ECommExclusive, ECommRoleDCE);
	err = iCommPort2.SetConfig(commConfigBuf);
	TestErrorCodeL(err, _L("Failed loading port"));
	
	// make sure Read works
	iCommPort1.Write(writeStatus, KWriteBufNewlineThenTabTerminator);
	User::WaitForRequest(writeStatus);
	TestErrorCodeL(writeStatus.Int(), _L("Write failed"));

	TBuf8<KRegularBufferSize> readBuf2;
	iCommPort2.Read(readStatus, readBuf2);
	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf2, KWriteBufNewlineTerminator, _L("Read from comm port 1"));
	
	iCommPort2.Read(readStatus, readBuf2);
	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf2, KWriteBufTabTerminator, _L("Read from comm port 1"));
	
	TRequestStatus timerStatus;
	TBuf8<2*KWriteBufSegmentSize> readBuf4;
	iCommPort2.Read(readStatus, readBuf4);

	RTimer timer;
	timer.CreateLocal();
	timer.After(timerStatus, 10*KConfiguredPortDelayMicroseconds);

	User::WaitForRequest(readStatus, timerStatus);
	TestBooleanTrueL(timerStatus == KErrNone && readStatus != KErrNone, _L("Check that read didn't complete"));
	
	iCommPort1.Write(writeStatus, KWriteBuf);
	User::WaitForRequest(writeStatus);
	TestErrorCodeL(writeStatus.Int(), _L("Failed write"));

	User::WaitForRequest(readStatus);
	TestErrorCodeAndDescriptorL(readStatus.Int(), readBuf4, KDoubleWriteBuf, _L("Read from comm port 1"));

	return TestStepResult();
	}

