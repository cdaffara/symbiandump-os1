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
//

#include <f32file.h>
#include <fbs.h>
#include <bitmap.h>
#include <bautils.h>
#include "../sfbs/UTILS.H"
#include "TSecureFBS.h"



LOCAL_C RFs fs;

//----------------------TEST DATA-------------------------------------
//RC_ROM12 is a ROM type mbm
//RC_RAM16 is a File Store type mbm
//TFBS inside TFBS_RSC is a File Store type mbm
//Bitmap for testing purposes
_LIT(KBmp1_Z,"z:\\system\\data\\RC_ROM12.mbm");
_LIT(KBmp2_Z,"z:\\system\\data\\RC_RAM16.mbm");

//Private path in C:
_LIT(KBmp2_CP,"c:\\private\\10273364\\RC_RAM16.mbm");
//Private path in Z:
_LIT(KBmp1_ZP,"z:\\private\\10273364\\RC_ROM12.mbm");
_LIT(KBmp2_ZP,"z:\\private\\10273364\\RC_RAM16.mbm");
_LIT(KRscHeader_ZP,"z:\\private\\10273364\\RscHeader3.bin");
_LIT(KDummyRsc_ZP,"z:\\private\\10273364\\DummyRscFile3.rsc");
_LIT(KTbsRsc_ZP,"z:\\private\\10273364\\TFBS_RSC.rsc");
_LIT(KInvalid_Bitmap,"z:\\private\\10273364\\invalid.mbm");

//Other private path 00999999 used for data caging test
_LIT(KBmp1_ZOP,"z:\\private\\00999999\\RC_ROM12.mbm");
_LIT(KBmp2_ZOP,"z:\\private\\00999999\\RC_RAM16.mbm");

_LIT(KPlatsecBegin,"*PlatSec* ERROR - BEGIN NEGATIVE PLATSEC TESTING");
_LIT(KPlatsecEnd,"*PlatSec* ERROR - END NEGATIVE PLATSEC TESTING");
//---------------------------------------------------------------------

CTFbsSecure::CTFbsSecure(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

//--------------------UTILITY Function---------------------------------
//This function is here to measure the offset in the rsc file
TInt CTFbsSecure::FileSizeL(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(fs, aFileName, EFileRead));
	CleanupClosePushL(file);
	TInt size = 0;
	User::LeaveIfError(file.Size(size));
	CleanupStack::PopAndDestroy(&file);
	return size;
	}
	
//--------------------TEST CASE Function-------------------------------
/**

-Using two type of bitmaps Rom and File Store Bitmap
-Testing will focus on File Store type bitmap only
-Loading a bitmap in private data cage, expect KErrNone
-Loading a bitmap in other private data cage, expect KErrPermissionDenied(-46)
-Loading a bitmap from a public path, expect KErrNone
-Loading a bitmap from an rsc file inside the private data path
*/
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0585

	@SYMTestCaseDesc
	TestCase1 for testing data caging properties.
	
	@SYMTestActions
	Using two type of bitmaps Rom and File Store Bitmap.
	Testing will focus on File Store type bitmap only.
	Loading a bitmap in private data cage, expect KErrNone.
	Loading a bitmap in other private data cage, expect KErrPermissionDenied(-46).
	Loading a bitmap from a public path, expect KErrNone.
	Loading a bitmap from an rsc file inside the private data path.

	@SYMTestExpectedResults
	Test should pass
*/
void CTFbsSecure::TestCase1()
	{
	INFO_PRINTF1(_L("Test Case 1"));
    TInt skipRomBitmapTests = EFalse;
	TUint32* romAddress = NULL;
	if(!CFbsBitmap::IsFileInRom(KBmp1_ZP, romAddress)) //any ROM bitmap
		{
		INFO_PRINTF2(_L("Skipping ROM bitmap tests since file \"%S\" is reported to not be a ROM bitmap."),
				&KBmp1_ZP);
		INFO_PRINTF1(_L("This should only occur on non-XIP ROMs, e.g. NAND ROMs, where ROM bitmaps aren't supported."));
		skipRomBitmapTests = ETrue;
		}

	TInt ret;
	if(!skipRomBitmapTests)
		{
		//Loading a ROM bitmap from its own private data cage \private\00099999
		CFbsBitmap bmp1;
		ret=bmp1.Load(KBmp1_ZP,0);
		TEST(ret==KErrNone);
		TEST(bmp1.IsRomBitmap());
		
		//Loading a ROM bitmap from other private data cage \private\00999999
		//This test works if PlatSecEnforcement is ON otherwise it will return KErrNone
		CFbsBitmap bmp2;
		ret=bmp2.Load(KBmp1_ZOP,0);
		if (PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) && PlatSec::IsCapabilityEnforced(ECapabilityAllFiles))
			TEST(ret==KErrPermissionDenied);
		else
			TEST(ret==KErrNone);
		
		//Loading a ROM bitmap from a public data cage \system\data
		CFbsBitmap bmp3;
		ret=bmp3.Load(KBmp1_Z,0);
		TEST(ret==KErrNone);
		}
	
	//Loading a File Store bitmap from its own private data cage \private\00099999
	CFbsBitmap bmp4;
	ret=bmp4.Load(KBmp2_ZP,0);
	TEST(ret==KErrNone);
	TEST(!bmp4.IsRomBitmap());
	
	//Loading a File Store bitmap from other own private data cage \private\00099999
	CFbsBitmap bmp5;
	ret=bmp5.Load(KBmp2_ZOP,0);
	if (PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement) && PlatSec::IsCapabilityEnforced(ECapabilityAllFiles))
		TEST(ret==KErrPermissionDenied);
	else
		TEST(ret==KErrNone);
	
	//Loading a File Store bitmap from a public data cage \system\data
	CFbsBitmap bmp6;
	ret=bmp6.Load(KBmp2_Z,0);
	TEST(ret==KErrNone);
	
	//Loading a bitmap file from an rsc file inside the private data path
	CFbsBitmap bmp7;
	TInt fileoffset=0;
	//Getting the file offset for the mbm file embedded inside the rsc
	TRAP(ret,fileoffset=FileSizeL(KRscHeader_ZP)+FileSizeL(KDummyRsc_ZP));
	TEST(ret==KErrNone);
	ret=bmp7.Load(KTbsRsc_ZP,0,EFalse,fileoffset);
	TEST(ret==KErrNone);

	//Loading a File Store bitmap from own private data cage in C:
	CFbsBitmap bmp8;
	ret=bmp8.Load(KBmp2_CP,0);
	TEST(ret==KErrNone);
	}

/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0586

	@SYMTestCaseDesc
	TestCase2 for testing of invalid arguments such as file name,id, file offset

	@SYMTestActions
	Test focus on File Store bitmap
	Loading an invalid bitmap file name expect KErrNotFound
	Loading a bitmap with invalid id
	Loading a bitmap with invalid file offset
	
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbsSecure::TestCase2()
	{
	INFO_PRINTF1(_L("Test Case 2"));
	TInt ret;
	//Loading an invalid bitmap KInvalid_Bitmap
	CFbsBitmap bmp1;
	ret=bmp1.Load(KInvalid_Bitmap,0);
	TEST(ret==KErrNotFound);
	
	//Loading a bitmap with invalid id,KBmp1_ZP only has one bitmap
	CFbsBitmap bmp2;
	ret=bmp2.Load(KBmp2_ZP,1);
	TEST(ret==KErrEof);
	
	//Loading a bitmap with invalid File offset
	CFbsBitmap bmp3;
	ret=bmp3.Load(KTbsRsc_ZP,0,EFalse,0);
	TEST(ret==KErrNotSupported);
	}

/**


*/
/**
	@SYMTestCaseID
	GRAPHICS-FBSERV-0587

	@SYMTestCaseDesc
	TestCase3 for testing of the sharing of bitmaps using iShareIfLoaded
	this indirectly test the removal of the FileInfo and yet preserver 
	preserve the same functionality in 9.0 without any regression

	@SYMTestActions
	bmp1 load with iShareIfLoaded=ETrue and bmp2 load same file with ETrue
 	Expect the Handle() and DataAddress() to be the same as there will be
 	only one raw memory data stored in the global heap for these 2 instances of
 	the same bitmap
 
	bmp3 load with iShareIfLoaded=EFalse and bmp4 load same file with ETrue
 	Expect the Handle() and DataAddress() to be different as bmp1 does not want
 	to share the bitmap,so bmp2 will have its own Handle() and a completely pointer
 	to the bitmap raw data
 
	@SYMTestExpectedResults
	Test should pass
*/
void CTFbsSecure::TestCase3()
	{
	INFO_PRINTF1(_L("Test Case 3"));
	TInt ret;
		
	//Loading first bitmap with ShareIfLoaded
	CFbsBitmap* bmp1=new (ELeave) CFbsBitmap;
	ret=bmp1->Load(KBmp2_ZP,0,ETrue);
	TEST(ret==KErrNone);
	TEST(!bmp1->IsRomBitmap());

	//Loading second bitmap with ShareIfLoaded
	CFbsBitmap* bmp2=new (ELeave) CFbsBitmap;
	ret-bmp2->Load(KBmp2_ZP,0,ETrue);
	TEST(ret==KErrNone);
	TEST(!bmp2->IsRomBitmap());

	//Test the file sharing properties do exist here
	TEST(bmp1->Handle()==bmp2->Handle());
	TEST(bmp1->DataAddress()==bmp2->DataAddress());
	
	TInt fileoffset=0;
	TRAP(ret,fileoffset=FileSizeL(KRscHeader_ZP)+FileSizeL(KDummyRsc_ZP));
	TEST(ret==KErrNone);
	//Loading third bitmap with NO ShareIfLoaded
	CFbsBitmap* bmp3=new (ELeave) CFbsBitmap;
	ret=bmp3->Load(KTbsRsc_ZP,1,EFalse,fileoffset);
	TEST(ret==KErrNone);
	TEST(!bmp3->IsRomBitmap());
		
	//Loading fourth bitmap(similar to 3) with ShareIfLoaded
	CFbsBitmap* bmp4=new (ELeave) CFbsBitmap;
	ret=bmp4->Load(KTbsRsc_ZP,1,ETrue,fileoffset);
	TEST(ret==KErrNone);
	TEST(!bmp4->IsRomBitmap());
	
	//Test the file sharing properties does not exist here
	TEST(bmp3->Handle()!=bmp4->Handle());
	TEST(bmp3->DataAddress()!=bmp4->DataAddress());

	delete bmp1;
	delete bmp2;
	delete bmp3;
	delete bmp4;
	
	}

/**
@SYMTestCaseID GRAPHICS-SYSLIB-FBSERV-CT-0002
@SYMTestCaseDesc Testing for the secure bitmap loading in FBSERV
@SYMTestPriority High
@SYMTestActions  Testing the loading of a bitmap from its own data cage
				 Testing for permission denied when loading from other private data
				 Testing for invalid arguments
				 Testing for bitmap sharing
@SYMTestExpectedResults The test must not fail.
@SYMPREQ 280 General Datacaging
*/

void CTFbsSecure::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFbsSecureStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTFbsSecureStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0585"));
		RDebug::Print(KPlatsecBegin);
		TestCase1();
		RDebug::Print(KPlatsecEnd);
		break;
	case 2:
		((CTFbsSecureStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0586"));
		TestCase2();
	case 3:
		((CTFbsSecureStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0587"));
		TestCase3();
		break;
	case 4:
		((CTFbsSecureStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTFbsSecureStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTFbsSecureStep*)iStep)->RecordTestResultL();
	}

//--------------
__CONSTRUCT_STEP__(FbsSecure)


void CTFbsSecureStep::TestSetupL()
	{
	User::LeaveIfError(fs.Connect());

	//Creating secure private path for testing purpose
	//This should create the private\10099999 directory
	User::LeaveIfError(fs.CreatePrivatePath(EDriveC));

	//copying files to the secure path
	BaflUtils::CopyFile(fs,KBmp2_Z,KBmp2_CP);
	}
	
void CTFbsSecureStep::TestClose()
	{
	//Deleting any bitmaps copied to C:
	DeleteDataFile(KBmp2_CP);
	fs.Close();
	}

//This function is here to delete the bitmaps in C after finished with it
void CTFbsSecureStep::DeleteDataFile(const TDesC& aFullName)
	{
	TEntry entry;
	if(fs.Entry(aFullName, entry) == KErrNone)
		{
		RDebug::Print(_L("Deleting \"%S\" file.\n"), &aFullName);
		TInt err = fs.SetAtt(aFullName, 0, KEntryAttReadOnly);
		if(err != KErrNone) 
			{
			RDebug::Print(_L("Error %d changing \"%S\" file attributes.\n"), err, &aFullName);
			}
		err = fs.Delete(aFullName);
		if(err != KErrNone) 
			{
			RDebug::Print(_L("Error %d deleting \"%S\" file.\n"), err, &aFullName);
			}
		}
	}
