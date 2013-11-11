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

#include "tdirectgdidriver.h"
#include <graphics/directgdicontext.h>

CTDirectGdiDriver::CTDirectGdiDriver()
	{
	SetTestStepName(KTDirectGdiDriverStep);
	}

CTDirectGdiDriver::~CTDirectGdiDriver()
	{
	}
			
/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-DRIVER-0001

@SYMPREQ	
	PREQ39

@SYMREQ
	REQ9211 
	REQ9226 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Construction and destruction of a CDirectGdiDriver object.
	Only one object should be created as its a singleton.
	Open() should create the CDirectGdiDriverInternal object and increment the
	reference count.
	Close() should decrement the reference count and when it becomes zero, destroy
	the CDirectGdiDriver & CDirectGdiDriverInternal object.

@SYMTestActions
	Open a driver.
	Open a driver again.
	Close a driver then open it again.
	Close a driver, check that it is still open.
	Close a driver, check that it is now closed.

@SYMTestExpectedResults
	A driver can be opened and closed successfully with no errors.

@SYMTestStatus
	Implemented
*/
void CTDirectGdiDriver::TestDriverL()
	{
	_LIT(KTestName, "Driver-TestDriver"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	// Check the driver returns NULL
	CDirectGdiDriver* dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver == NULL);
	
	// Open the driver once
	TInt res = CDirectGdiDriver::Open();		
	TESTNOERRORL(res);		
	
	dgdiDriver = CDirectGdiDriver::Static();
	TESTL(dgdiDriver != NULL);
	
	// Open the driver a second time
	res = CDirectGdiDriver::Open();		
	TESTNOERRORL(res);		
	
	// Close the driver once
	dgdiDriver->Close();
	
	// Open the driver again
	res = CDirectGdiDriver::Open();		
	TESTNOERRORL(res);	
			
	// Close the driver twice, on the second close, the driver should delete 
	// itself when its internal open count reaches zero
	dgdiDriver->Close();
	TESTL(CDirectGdiDriver::Static() != NULL);	
	dgdiDriver->Close();
	TESTL(CDirectGdiDriver::Static() == NULL);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-DRIVER-0002

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9211 
	REQ9226 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Out of memory test. Test that opening a driver does not cause memory leaks.

@SYMTestActions
	Repeatedly call the "Driver-ErrorCodes" test (below) in OOM conditions to make 
	sure no memory leaks occur.

@SYMTestExpectedResults
	No memory leaks.
	
@SYMTestStatus
	Implemented
*/
void CTDirectGdiDriver::TestDriverOOM()
	{
	TInt err, tryCount = 0;
	INFO_PRINTF1(_L("Driver-TestDriverOOM"));

	do
		{
		__UHEAP_FAILNEXT(++tryCount);
		__UHEAP_MARK;
		{
		err = TestDriverErrorCodes();		
		}
		__UHEAP_MARKEND;
		} 
	while(err == KErrNoMemory);
	if(err == KErrNone)
		{
		__UHEAP_RESET;
		}
	else
		{
		TEST(err == KErrNone);
		}
	INFO_PRINTF2(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-DRIVER-0003

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9211 
	REQ9226 
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Checks the error code returned by GetError() after 	Construction and 
	destruction of a CDirectGdiDriver object.

@SYMTestActions
	Create and destroy a CDirectGdiDriver.

@SYMTestExpectedResults
	Test that the only errors returned when opening a driver are KErrNone and
	KErrNoMemory. Test that GetError() returns KErrNone after a driver has been opened.
	
@SYMTestStatus
	Implemented
*/
TInt CTDirectGdiDriver::TestDriverErrorCodes()
	{
	_LIT(KTestName, "Driver-ErrorCodes"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TInt errOpen = CDirectGdiDriver::Open(); 		
	TEST( errOpen == KErrNone || errOpen == KErrNoMemory  );
	if( errOpen != KErrNone )
		return errOpen;
	
	CDirectGdiDriver* gdiDriver = CDirectGdiDriver::Static();
	if( !gdiDriver )
		return KErrNoMemory;	
		
	TInt err = gdiDriver->GetError();	
	TESTNOERROR(err); 
	gdiDriver->Close();	//Ref count should now be 0	
	
	return err;
	}

/**
@SYMTestCaseID
GRAPHICS-DIRECTGDI-0004

@SYMPREQ PREQ39

@SYMTestCaseDesc
	Sets the error code and then checks the error code by calling GetError().
	Setting the error code should not affect the actual error code, unless GetError() is first called to reset the error code.

@SYMTestActions
	Test the error setting and reporting functionality of the driver.
	Set the error code twice with different errors each time,
	get the error - the error returned should be the first error set, not the second.

@SYMTestExpectedResults
	Error code should be KErrGeneral and not KErrNoMemory.
*/
void CTDirectGdiDriver::TestSetError()
	{
	_LIT(KTestName, "Driver-SetError"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	TInt errOpen  = CDirectGdiDriver::Open(); 	
	TEST( errOpen == KErrNone || errOpen == KErrNoMemory  );
	
	CDirectGdiDriver* gdiDriver = CDirectGdiDriver::Static();
	TEST(gdiDriver!=NULL);	
	
	gdiDriver->SetError(KErrGeneral);	
	gdiDriver->SetError(KErrNoMemory);	

	TInt err = gdiDriver->GetError();	
	TEST(err == KErrGeneral); 

	gdiDriver->Close();	//Ref count should now be 0	
	}

/**
@SYMTestCaseID
GRAPHICS-DIRECTGDI-0005

@SYMPREQ PREQ39

@SYMTestCaseDesc
	Tests the function Flush()

@SYMTestActions
	Open a DirectGDI driver
	Call Flush()
	Close the driver.

@SYMTestExpectedResults
	On software DirectGDI, Flush() does not do anything but on hardware DirectGDI, it calls vgFlush().
	No error should occur.
*/
void CTDirectGdiDriver::TestFlush()
	{
	_LIT(KTestName, "Driver-TestFlush"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}	
	TInt errOpen  = CDirectGdiDriver::Open(); 	
	TEST(errOpen == KErrNone);	
	CDirectGdiDriver* gdiDriver = CDirectGdiDriver::Static();
	TEST(gdiDriver!=NULL);		
	gdiDriver->Flush();
	TInt err = gdiDriver->GetError();	
	TEST(err == KErrNone); 
	gdiDriver->Close();	//Ref count should now be 0	
	}

/**
@SYMTestCaseID
GRAPHICS-DIRECTGDI-0006

@SYMPREQ PREQ39

@SYMTestCaseDesc
	Test that CDirectGdiDriver::GetInterface() returns KErrExtensionNotSupported
	when a non-existent extension interface is requested.

@SYMTestActions
	Open a DirectGDI driver.
	Request a extension interface using the Uif 0xFFFFFFFF.		

@SYMTestExpectedResults
	KErrExtensionNotSupported should be returned and the requested interface pointer
	should be set to NULL.
 */
void CTDirectGdiDriver::TestGetInterfaceL()
	{
	_LIT(KTestName, "Driver-TestGetInterfaceL"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}	
	
	TInt err = CDirectGdiDriver::Open(); 	
	TESTNOERRORL(err);
	
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	TESTL(driver != NULL);
	
	// Set badInterface to something other than NULL to make sure GetInterface is setting it to NULL
	TAny* badInterface = &err; 
	err = driver->GetInterface(TUid::Uid(0xFFFFFFFF), badInterface);
	TEST(err == KErrExtensionNotSupported);
	TEST(badInterface == NULL);
	
	driver->Close();
	}

/**
@SYMTestCaseID
GRAPHICS-DIRECTGDI-0007

@SYMPREQ PREQ39

@SYMTestCaseDesc
	Tests the MDirectGdiDriverCacheSize extension interface implementation provided
	by CDirectGdiDriver. Note that this extension is only implemented by the hardware
	DirectGdi adaptation, it is not provided by the software adaptation.

@SYMTestActions
	if testing on hardware DirectGdi:
		Open a DirectGDI driver.
		Attempt to get the extension interface for MDirectGdiDriverCacheSize from the driver.
			- This should succeed.
		Get the currently set maximum image cache and glyph cache size.
		Set the new image and glyph cache sizes to less than the current size.
		Set the new image and glyph cache sizes to greater than the current size.
		Check that the reported image/glyph cache sizes are correctly set.
			
	if testing on software DirectGdi:
		Open a DirectGdi driver.
		Attempt to get the extension interface for MDirectGdiDriverCacheSize from the driver.
			- This should fail.		

@SYMTestExpectedResults
    Setting the caches to a smaller size that the existing size should cause KErrArgument to be returned.
    All other APIs should succeed.
    If using Software DirectGDI, it should skip the tests.
 */
void CTDirectGdiDriver::TestCacheSizesL()
	{
	_LIT(KTestName, "Driver-TestCacheSizes"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}	
	
	TInt err = CDirectGdiDriver::Open(); 	
	TESTNOERRORL(err);
	
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	TESTL(driver != NULL);
	CleanupClosePushL(*driver);
	
	MDirectGdiDriverCacheSize* driverCacheInterface = NULL;
	err = driver->GetInterface(TUid::Uid(KDirectGdiDriverCacheSizeUid), (TAny*&)driverCacheInterface);
	if (err == KErrNone)
		{
		// MDirectGdiDriverCacheSize is implemented in the hardware (VG) adaptation
		TESTL(driverCacheInterface != NULL);
		
		// This number must be > the default image cache size for this test to pass.
		const TInt KLargeImageCacheSize = 0x500000; 
		// This number must be > the default glyph cache size for this test to pass.
		const TInt KLargeGlyphCacheSize = 0x500000;
		
		TESTNOERROR(driverCacheInterface->SetMaxImageCacheSize(KLargeImageCacheSize));
		TESTNOERROR(driverCacheInterface->SetMaxGlyphCacheSize(KLargeGlyphCacheSize));
		
		// Test the image cache size
		TEST(KErrArgument == driverCacheInterface->SetMaxImageCacheSize(-1));
		TESTNOERROR(driverCacheInterface->SetMaxImageCacheSize(KLargeImageCacheSize+1));
		TEST(driverCacheInterface->MaxImageCacheSize() == KLargeImageCacheSize+1);
				
		// Test the glyph cache size
		TEST(KErrArgument == driverCacheInterface->SetMaxGlyphCacheSize(-1));
		TESTNOERROR(driverCacheInterface->SetMaxGlyphCacheSize(KLargeGlyphCacheSize+1));
		TEST(driverCacheInterface->MaxGlyphCacheSize() == KLargeGlyphCacheSize+1);
		}
	else
		{
		// MDirectGdiDriverCacheSize is not implemented in the software adaptation of DirectGDI.
		// The only error code that should be returned is KErrExtensionNotSupported.
		TEST(err == KErrExtensionNotSupported);
		TEST(driverCacheInterface == NULL);
		INFO_PRINTF1(_L("Extension MDirectGdiDriverCacheSize not supported, skipping test."));
		}
	
	CleanupStack::PopAndDestroy(1, driver);
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTDirectGdiDriver::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();	
	INFO_PRINTF1(_L("DirectGdi Driver pre-test setup"));
	return TestStepResult();
	}
	
/** 
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

Creates background window with black & white checker board. 
This background windows is used for each test case in this file

@return TVerdict code
*/	
TVerdict CTDirectGdiDriver::doTestStepL()
	{		
	INFO_PRINTF1(_L("DirectGdi Driver Test" ));
	if (iUseDirectGdi)
		{
		RunTestsL();			
		}	
	return TestStepResult();
	}

void CTDirectGdiDriver::RunTestsL()
	{
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRIVER-0001"));
	TestDriverL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRIVER-0003"));
	TestDriverErrorCodes();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-DRIVER-0002"));
	TestDriverOOM();	
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-0004"));
	TestSetError();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-0005"));
	TestFlush();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-0006"));
	TestGetInterfaceL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-DIRECTGDI-0007"));
	TestCacheSizesL();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	}
