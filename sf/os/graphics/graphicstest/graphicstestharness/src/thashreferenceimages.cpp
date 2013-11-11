// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code
*/

#include <test/testexecutestepbase.h>
#include <openfont.h>
#include <test/thashreferenceimages.h>
#include <hash.h>

_LIT(KHashIDFormat, "%S_%d_%S_%S_%d");
const TInt KLengthOfHashValue = 256;
const TInt KNumOfDisplayModes = 12;

/* This is a list of display modes used - the parameter passed to the string 
   creation function is an index into this table
 
 	EGray2,
	EGray4,
	EGray16,
	EGray256,
	EColor16,
	EColor256,
	EColor64K,
	EColor16M,
	EColor4K,
	EColor16MU,
	EColor16MA,
	EColor16MAP
	
   */

_LIT(KMode0,"EGray2");
_LIT(KMode1,"EGray4");
_LIT(KMode2,"EGray16");
_LIT(KMode3,"EGray256");
_LIT(KMode4,"EColor16");
_LIT(KMode5,"EColor256");
_LIT(KMode6,"EColor64K");
_LIT(KMode7,"EColor16M");
_LIT(KMode8,"EColor4K");
_LIT(KMode9,"EColor16MU");
_LIT(KMode10,"EColor16MA");
_LIT(KMode11,"EColor16MAP");

//Uncomment the line below to save the image of the screen to a mbm file.
//#define SAVEBITMAP

//Note: To updated existing hash values in toutlineandshadow.ini, you need to delete the previous entries along with hashid's in toutlineandshadow.ini and export this ini
//If a existing test case need updating that will change the hash value, then the hash for this test case needs to be removed from the .ini file 
//Uncomment the line below to add a new hash value for any test cases that do not already have a hash.
//#define APPEND_NEW_OR_MISSING_HASH_DATA

// Constructor
CTHashReferenceImages::CTHashReferenceImages(CTestStep* aStep, RFbsSession* aFbs):CBase(),iFbs(aFbs),iStep(aStep) { }

EXPORT_C CTHashReferenceImages* CTHashReferenceImages::NewL(CTestStep* aStep, RFbsSession* aFbs, const TDesC *aPath)
	{
	CTHashReferenceImages* ptr = new (ELeave)CTHashReferenceImages(aStep,aFbs);
	CleanupStack::PushL(ptr);
	ptr->ConstructL(aPath);
	CleanupStack::Pop();
	return ptr;
	}

void CTHashReferenceImages::ConstructL(const TDesC *aPath)
	{
	iPath = aPath->AllocL();
	}

// Destructor
EXPORT_C CTHashReferenceImages::~CTHashReferenceImages()
	{
	//nothing is owned by this class, except iPath
	delete iPath;
	}

/**
Auxilary function should be called only when we need to generate hash values from the screen and returns its hex format.
@param aHexString the output MD5 hash hex string obtained from iBitmapDevice
*/
EXPORT_C void CTHashReferenceImages::GenerateHashAndReturnInHexFormatL(TDes &aHexString)
	{
	TInt bufLen = CFbsBitmap::ScanLineLength(iBitmapDevice->SizeInPixels().iWidth, iBitmapDevice->DisplayMode());
	RBuf8 buff;
	buff.CreateL(bufLen);
	CleanupClosePushL(buff);	
	CMD5 *md = CMD5::NewL();
	CleanupStack::PushL(md);
	for (TPoint pos(0, 0); pos.iY < iBitmapDevice->SizeInPixels().iHeight; pos.iY++)
		{
		iBitmapDevice->GetScanLine(buff,pos,iBitmapDevice->SizeInPixels().iWidth,iBitmapDevice->DisplayMode());
		md->Update(buff);
		}

	TBuf8<KLengthOfHashValue> hashString;
	//md will be reset after calling CMD5::Final() as Final will call Reset.
	hashString.Copy(md->Final());
	aHexString.Zero();

	for(TInt icount=0; icount < hashString.Length(); icount++)
		{
		aHexString.AppendNumFixedWidth(hashString[icount], EHex, 4);
		}
	CleanupStack::PopAndDestroy(2, &buff);
	}

/**
Auxilary function called to compare generated hash with reference hash and report error if they don't match.
@param aHashIndex holds the hashId.
*/
EXPORT_C void CTHashReferenceImages::CompareHashValuesL(const TDesC& aHashIndex)
	{
	TBuf<KLengthOfHashValue>  hexString;
	//Gets the hash value for the current drawings
	GenerateHashAndReturnInHexFormatL(hexString);
	TPtrC hashFromConfig;
	TBool stringFound = iStep->GetStringFromConfig(iStep->ConfigSection(), aHashIndex, hashFromConfig);
	if (stringFound)
		{
		//cannot use the macro //TEST((hashFromConfig.Compare(hexString)) == 0); since iStep needs to be 
		//referenced
		iStep->testBooleanTrue((hashFromConfig.Compare(hexString)) == 0, (TText8*)__FILE__, __LINE__, ETrue);
		}
	else
		{
#ifdef APPEND_NEW_OR_MISSING_HASH_DATA
		//Hash data will be written to ini file if hash id is not found.
		//If written the status will be as INI WRITE with the new hash id and value in test log file.
		iStep->testBooleanTrue(iStep->WriteStringToConfig(iStep->ConfigSection(), aHashIndex, hexString), (TText8*)__FILE__, __LINE__);
#ifdef SAVEBITMAP
		CopyScreenToBitmapL(aHashIndex);
#endif
#else
		//Report error in case hash data is missing and APPEND_NEW_OR_MISSING_HASH_DATA is not defined
		iStep->testBooleanTrue( 0, (TText8*)__FILE__, __LINE__);
#endif
		}
	}

/**
Auxilary function called to generate Hash ID String based on the parameters (test case ID, subtest etc...) and returns
HashId looks like:Testcasecame_0010_9_Swiss_EGray2_0 (aTestCase = DrawText_0010 or DrawTextVertical_0010, aSubTestNumber = 9, aFontFaceIndex = Swiss, aDisplayMode = EGray2, aOrientation = 0)
@param aTestCase holds the testcase ID
@param aSubTestNumber holds the subtest number
@param aFontFaceIndex holds the font face index (index used in KFontFace[])
@param aDisplayMode holds the display mode
@param aOrientation holds the orientation number
*/
EXPORT_C HBufC* CTHashReferenceImages::GenerateHashIdStringLC(const TDesC& aTestCase, TInt aSubTestNumber, const TPtrC aName[], TInt aNameIndex,
												TInt aDisplayMode, TInt aOrientation)
	{
	//this is here because at file scope there is uninitialised writable data
	const TDesC * KDisplayModeNames[KNumOfDisplayModes] = 
		{
		&KMode0,
		&KMode1,
		&KMode2,
		&KMode3,
		&KMode4,
		&KMode5,
		&KMode6,
		&KMode7,
		&KMode8,
		&KMode9,
		&KMode10,
		&KMode11,
		};	
	
	TBuf<KLengthOfHashValue> tempBuffer;
	tempBuffer.Format(KHashIDFormat, &aTestCase, aSubTestNumber, &aName[aNameIndex], KDisplayModeNames[aDisplayMode], aOrientation);
	return tempBuffer.AllocLC();
	}



/**
Auxilary function called to Copy the screen to bitmap (mbm) file.
@param aHashIndex contains hashID. Bitmap is created with the aHashIndex as name
*/
EXPORT_C void CTHashReferenceImages::CopyScreenToBitmapL(const TDesC& aHashIndex)
	{
	CFbsBitmap *bitmap = new(ELeave)CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(iBitmapDevice->SizeInPixels(), iBitmapDevice->DisplayMode()));
	TRect rect = TRect(iBitmapDevice->SizeInPixels());
	CFbsBitmapDevice *device=CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(device);
	CFbsBitGc *gc;
	User::LeaveIfError(device->CreateContext(gc));
	gc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
	gc->BitBlt(TPoint(), iBitmap, rect);
	TFileName mbmFile;
	mbmFile.Format(iPath->Des(), &aHashIndex);
	bitmap->Save(mbmFile);
	delete gc;
	CleanupStack::PopAndDestroy(2);
	}

/* 
Auxilary function used to change the referenced member data (ownership is not transferred)
*/
EXPORT_C void CTHashReferenceImages::SetScreenDeviceAndBitmap(CBitmapDevice* aBitmapDevice, CFbsBitmap* aBitmap, CFbsBitGc* aGc )
	{
	iBitmapDevice = aBitmapDevice;
	iBitmap = aBitmap;
	iGc = aGc;
	}
