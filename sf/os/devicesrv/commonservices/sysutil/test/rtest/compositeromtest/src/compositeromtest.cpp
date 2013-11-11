// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// file compositeromtest.cpp
// 
//

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <e32test.h>
#include <bafl/sysutil.h>

static RTest test(_L("CompositeRomTest"));

static const TUid manufacturerAttributeUID = {0xE1000001};

_LIT(KTest1, "SYSLIB-BAFL-CT-4063: TestMultipleRofsSections");

// Expected values
_LIT16(KTest1ManufacturerName, "MyManufacturer");
_LIT16(KTest1ModelName, "ModelNameTest");
_LIT16(KTest1ModelCode, "TestModelCode");
_LIT16(KTest1RevisionID, "16.21");
_LIT16(KTest1DefaultDeviceName, "TestDefaultDeviceName");
_LIT16(KTest1UIPlatformName, "Symbian");
static const TUint16 KTest1UIPlatformVersionMajor = 12;
static const TUint16 KTest1UIPlatformVersionMinor = 4;
_LIT16(KTest1UIPlatformVersion, "12.4");
static const TUint16 KTest1OSVersionMajor = 9;
static const TUint16 KTest1OSVersionMinor = 5;
_LIT16(KTest1OSVersion, "9.5");
_LIT16(KTest1ManufacturerAttribute, "TestAttribute");

/* Test macro and function */
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if (aValue != aExpected)
		{
		test.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		test.operator()(EFalse, aLine);
		}
	}
#define test2(a, b) Check(a, b, __LINE__)


/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4063
@SYMREQ					CR1328
@SYMTestCaseDesc		To ensure the correct attribute values are retrieved when there 
@SYMTestCaseDesc		are multiple ROFS sections which may each contain a different 
@SYMTestCaseDesc		version of the deviceattributes.ini file.
@SYMTestCaseDesc		
@SYMTestCaseDesc		Note: When run on hardware, this test needs to be part of a ROM 
@SYMTestCaseDesc		which is made up of a core ROM image and extension ROM images. 
@SYMTestCaseDesc		Each extension image should contain a different version of the 
@SYMTestCaseDesc		same INI file. The INI files should contain data which makes 
@SYMTestCaseDesc		sure the following things are covered: 
@SYMTestCaseDesc		-	There is an INI file in the core ROM image which contains at 
@SYMTestCaseDesc			least 1 device attribute. This attribute should not appear in 
@SYMTestCaseDesc			any of the other versions of the INI file which are contained 
@SYMTestCaseDesc			in the extension ROM images.
@SYMTestCaseDesc		-	There is an INI file in one of the extension ROM images which 
@SYMTestCaseDesc			contains at least 1 device attribute. This attribute should 
@SYMTestCaseDesc			not appear in the INI file contained in the core ROM image.
@SYMTestCaseDesc		-	There is an INI file in the core ROM image which contains at 
@SYMTestCaseDesc			least 1 device attribute. This attribute should appear in another 
@SYMTestCaseDesc			version of the INI file in one of the extension ROM images.
@SYMTestCaseDesc		-	There is an INI file in one of the extension ROM images which 
@SYMTestCaseDesc			contains at least 1 device attribute. This attribute should 
@SYMTestCaseDesc			appear in another version of the INI file in one of the 
@SYMTestCaseDesc			extension ROM images
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			1.	Create an instance of CDeviceTypeInformation by calling 
@SYMTestActions				SysUtil::GetDeviceTypeInfo.
@SYMTestActions			2.	Call each of CDeviceTypeInformation member functions.
@SYMTestActions			3.	Check the retrieved value and return codes.
@SYMTestExpectedResults GetUIPlatformName should return a default value and return 
@SYMTestExpectedResults CDeviceTypeInformation::KDefaultValue. The rest of the APIs 
@SYMTestExpectedResults should retrieve provisioned values and return KErrNone.
*/
void TestMultipleRofsSectionsL()
	{
	CDeviceTypeInformation* attribs = SysUtil::GetDeviceTypeInfoL();
	CleanupStack::PushL(attribs);
	
	TPtrC16 buffer;
	TInt err = attribs->GetManufacturerName(buffer);
	test2(err, KErrNone);
	TInt difference = buffer.Compare(KTest1ManufacturerName);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1ManufacturerName, &buffer);
	test2(difference, 0);
	
	err = attribs->GetModelName(buffer);
	test2(err, KErrNone);
	difference = buffer.Compare(KTest1ModelName);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1ModelName, &buffer);
	test2(difference, 0);
	
	err = attribs->GetModelCode(buffer);
	test2(err, KErrNone);
	difference = buffer.Compare(KTest1ModelCode);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1ModelCode, &buffer);
	test2(difference, 0);
	
	err = attribs->GetRevisionID(buffer);
	test2(err, KErrNone);
	difference = buffer.Compare(KTest1RevisionID);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1RevisionID, &buffer);
	test2(difference, 0);
	
	err = attribs->GetDefaultDeviceName(buffer);
	test2(err, KErrNone);
	difference = buffer.Compare(KTest1DefaultDeviceName);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1DefaultDeviceName, &buffer);
	test2(difference, 0);
	
	err = attribs->GetUIPlatformName(buffer);
	test2(err, CDeviceTypeInformation::KDefaultValue);
	difference = buffer.Compare(KTest1UIPlatformName);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1UIPlatformName, &buffer);
	test2(difference, 0);
	
	TUint16 major, minor;
	err = attribs->GetUIPlatformVersion(major, minor);
	test2(err, KErrNone);
	test.Printf(_L("Expected major value = %d\nActual major value = %d\n"), KTest1UIPlatformVersionMajor, major);
	test.Printf(_L("Expected minor value = %d\nActual minor value = %d\n\n"), KTest1UIPlatformVersionMinor, minor);
	test2(major, KTest1UIPlatformVersionMajor);
	test2(minor, KTest1UIPlatformVersionMinor);
	
	err = attribs->GetUIPlatformVersion(buffer);
	test2(err, KErrNone);
	difference = buffer.Compare(KTest1UIPlatformVersion);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1UIPlatformVersion, &buffer);
	test2(difference, 0);
	
	err = attribs->GetOSVersion(major, minor);
	test2(err, KErrNone);
	test.Printf(_L("Expected major value = %d\nActual major value = %d\n"), KTest1OSVersionMajor, major);
	test.Printf(_L("Expected minor value = %d\nActual minor value = %d\n\n"), KTest1OSVersionMinor, minor);
	test2(major, KTest1OSVersionMajor);
	test2(minor, KTest1OSVersionMinor);
	
	err = attribs->GetOSVersion(buffer);
	test2(err, KErrNone);
	difference = buffer.Compare(KTest1OSVersion);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1OSVersion, &buffer);
	test2(difference, 0);
	
	err = attribs->GetAttribute(manufacturerAttributeUID, buffer);
	test2(err, KErrNone);
	difference = buffer.Compare(KTest1ManufacturerAttribute);
	test.Printf(_L("Expected value = %S\nActualValue = %S\n\n"), &KTest1ManufacturerAttribute, &buffer);
	test2(difference, 0);
	
	CleanupStack::PopAndDestroy(1);
	}

TInt MainL ()
	{
	test.Start(KTest1);
	TestMultipleRofsSectionsL();
	
	test.End();
	test.Close();
	
	return KErrNone;
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		return KErrNoMemory;
	
	TRAPD(err, MainL());
	
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}
