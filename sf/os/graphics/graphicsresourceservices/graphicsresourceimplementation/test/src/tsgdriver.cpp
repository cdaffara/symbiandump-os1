// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Graphics Resource API Conformance Test Suite
*/

#include "tsgdriver.h"

CTSgDriver::CTSgDriver(TBool aConformanceTests) :
	CTSgTestStepBase(aConformanceTests)
	{
	INFO_PRINTF1(_L("Graphics resource component test - SgDriver Tests.\r\n"));
	}

CTSgDriver::~CTSgDriver()
	{
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgDriver::doTestStepL()
	{	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0022"));
	INFO_PRINTF1(_L("Shutting down an uninitialised driver.\r\n"));
	TestShutdownUninitialized();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-RESOURCE-0096"));
	INFO_PRINTF1(_L("Checking the driver is the correct version.\r\n"));
	TestVersion();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0099"));
	INFO_PRINTF1(_L("Checking the driver returns the correct error on multiple opens.\r\n"));
	TestMultipleOpens();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-RESOURCE-0103"));
	INFO_PRINTF1(_L("Checking GetInterface() returns correct error codes.\r\n"));
	TestGetInterface();
	RecordTestResultL();
	
	return TestStepResult();
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0022
@SYMTestCaseDesc		Shuts down an uninitialised driver.
@SYMPREQ				PREQ2637
@SYMFssID				RSgDriver::Close()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			To ensure invalid Close() calls on the driver will cause no errors.
@SYMTestActions			Call RSgDriver::Close() several times without calling RSgDriver::Open().
@SYMTestExpectedResults	No errors should be returned.
 */	
void CTSgDriver::TestShutdownUninitialized()
	{
	__UHEAP_MARK;	
	
	RSgDriver sgDriver;
	
	sgDriver.Close();
	sgDriver.Close();
	sgDriver.Close();
	
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0096
@SYMTestCaseDesc		Test the version number returned by RSgDriver is correct.
@SYMPREQ				PREQ2637
@SYMFssID				RSgDriver::Version()
@SYMTestPriority		Medium
@SYMTestType			UT
@SYMTestPurpose			To check the correct version of the driver is being used in the
						conformance tests.
@SYMTestActions			Call RSgDriver::Version().
@SYMTestExpectedResults	The correct version should be returned.
 */	
void CTSgDriver::TestVersion()
	{
	__UHEAP_MARK;	
	
	RSgDriver sgDriver;
	
	TVersion vers = sgDriver.Version();
	TEST(vers.iMajor == 1);
	TEST(vers.iMinor == 1);
	
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0099
@SYMTestCaseDesc		Test SgDriver Handles Multiple Calls to Open Correctly
@SYMPREQ				PREQ2637
@SYMFssID				RSgDriver::Open()
@SYMTestPriority		High
@SYMTestType			UT
@SYMTestPurpose			To ensure the correct error code is returned when an open
						driver is opened
@SYMTestActions			Call RSgDriver::Open().
@SYMTestExpectedResults	KErrInUse should be returned when the driver is opened for
						the second time and all subsequent times.
 */	
void CTSgDriver::TestMultipleOpens()
	{
	__UHEAP_MARK;	
	
	TInt err = KErrNone;
	RSgDriver sgDriver;
	
	err = sgDriver.Open();
	TESTE(KErrNone == err, err);
	err = sgDriver.Open();
	TESTE(KErrInUse == err, err);
	err = sgDriver.Open();
	TESTE(KErrInUse == err, err);
	err = sgDriver.Open();
	TESTE(KErrInUse == err, err);	
	sgDriver.Close();	
	
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			GRAPHICS-RESOURCE-0103
@SYMTestCaseDesc		Test that GetInterface() returns the correct error code	in 
						negative situations.
@SYMPREQ				PREQ2637
@SYMFssID				RSgDriver::GetInterface()
@SYMTestPriority		Low
@SYMTestType			UT
@SYMTestPurpose			To ensure the correct error code is returned when GetInterface() 
						is used, in a variety of sitations.
@SYMTestActions			Call RSgDriver::GetInterface() with an interface that is valid but
						not supported by the driver.			
						Call RSgDriver::GetInterface() with an interface with a Null ID.
						Call RSgDriver::GetInterface() with any interface, on a driver that
						is not yet open.
@SYMTestExpectedResults	When using an unsupported interface, KErrExtensionNotSupported should 
						be returned.
						When using an interface with the null UID, KErrArgument should be returned.
						When using any interface after the driver is closed, KErrBadHandle
						should be returned.
 */

/**
Test classes for testing with RSgImage::GetInterface()
 */
class MFakeInterface
    {
public:
    enum { EInterfaceUid = 0x88888888 };
    };
class MNullInterface
    {
public:
    enum { EInterfaceUid = KNullUidValue };
    };

void CTSgDriver::TestGetInterface()
	{
	__UHEAP_MARK;
	
	RSgDriver sgDriver;
	TInt err = sgDriver.Open();
	TESTE(err == KErrNone, err);
	
	MFakeInterface* fakeInterface = NULL;
	err = sgDriver.GetInterface(fakeInterface);
	TESTE(err == KErrExtensionNotSupported, err);	
	
	MNullInterface* nullInterface = NULL;
	err = sgDriver.GetInterface(nullInterface);
	TESTE(err == KErrArgument, err);
	
	sgDriver.Close();	
	err = sgDriver.GetInterface(fakeInterface);
	TESTE(err == KErrBadHandle, err);
	err = sgDriver.GetInterface(nullInterface);
	TESTE(err == KErrBadHandle, err);
	
	__UHEAP_MARKEND;
	}
