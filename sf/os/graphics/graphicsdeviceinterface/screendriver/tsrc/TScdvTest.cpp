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

#include <hal.h>
#include <bitdraw.h>
#include "TScdvTest.h"

GLREF_C TInt ByteSize(TDisplayMode aDisplayMode,TInt aWidth);

//===================================================================
//In order to test multiple screens creation process on the Emulator, 
//you have to extend your epoc.ini file with the following lines
//_NewScreen_
//ScreenWidth 200
//ScreenHeight 300
//===================================================================

CTScdv::CTScdv(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

//The method calculates the memory (in bytes) needed for a screen 
//device with aSize size and aDisplayMode mode.
inline TInt CTScdv::ByteSize(const TSize& aSize, TDisplayMode aDisplayMode)
	{
	return ::ByteSize(aDisplayMode,aSize.iWidth) * aSize.iHeight;
	}

//The method creates a screen device for aScreenNo screen number .
//Possible leaving error codes: KErrNoMemory.
//Return result: ETrue - the device was created successfully, EFalse - not supported.
//Successfully created device will be pushed on the cleanup stack.
TBool CTScdv::CreateScreenDeviceLC(TInt aScreenNo, 
								  TDisplayMode aDisplayMode,
								  CFbsDrawDevice*& aScreenDevice)
	{
	__ASSERT_ALWAYS(aScreenDevice == NULL, User::Invariant());
	TBool res = EFalse;
	TRAPD(err, aScreenDevice = CFbsDrawDevice::NewScreenDeviceL(aScreenNo, aDisplayMode));
	if(err == KErrNone)
		{
		CleanupStack::PushL(aScreenDevice);
		res = ETrue;
		}
	else
		{
		if(err != KErrNotSupported)
			{
			User::LeaveIfError(err);
			}
		}
	return res;
	}

//The method initializes aScreenDevice screen device and allocates.
//a block of memory for it.
//Possible leaving error codes: KErrNoMemory.
//The allocated bock of memory will be pushed on the cleanup stack.
TAny* CTScdv::InitScreenDeviceLC(CFbsDrawDevice* aScreenDevice,
							   TDisplayMode aDisplayMode)
	{
	__ASSERT_ALWAYS(aScreenDevice != NULL, User::Invariant());

	TSize size = aScreenDevice->SizeInPixels();
	TInt bytesToAllocate=ByteSize(size, aDisplayMode);
	const TInt bytesRequire=bytesToAllocate;
	TUint8* devMemory = new TUint8[bytesToAllocate];
	if (!devMemory)
		{
		do	{
			bytesToAllocate/=2;
			devMemory=new TUint8[bytesToAllocate];
			} while (devMemory==NULL && bytesToAllocate>0);
		if (devMemory)
			{
			_LIT(KLog1,"Not enought memory!!  Allocated %d (0x%x) bytes when %d (0x%x) bytes required.");
			INFO_PRINTF5(KLog1,bytesToAllocate,bytesToAllocate,bytesRequire,bytesRequire);
			_LIT(KLog2,"Display Mode: %S,  Screen Size: %d,%d");
			INFO_PRINTF4(KLog2,&ColorModeName(aDisplayMode),size.iWidth,size.iHeight);
			}
		else
			{
			_LIT(KLog,"Failed to Allocate memory!!  %d (0x%x) bytes required,  Display Mode: %S,  Screen Size: %d,%d");
			INFO_PRINTF6(KLog,bytesRequire,bytesRequire,&ColorModeName(aDisplayMode),size.iWidth,size.iHeight);
			User::Leave(KErrNoMemory);
			}
		}
	CleanupStack::PushL(devMemory);

	aScreenDevice->SetUserDisplayMode(aDisplayMode);
	aScreenDevice->SetAutoUpdate(EFalse);
	aScreenDevice->SetBits(devMemory);
	return devMemory;
	}

//Mutltiple screen devices creation test
void CTScdv::TestScreenDeviceCreationL()
	{
	TDisplayMode mode[] = {	EColor256, EColor4K, EColor64K, EColor16,
							EColor16MU,	EColor16MA,	EColor16MAP	};

	const TInt KTestedDevCnt = 2;

	for(TInt ii=0;ii<TInt(sizeof(mode)/sizeof(mode[0]));++ii)
		{
		//create
		CFbsDrawDevice* dev[KTestedDevCnt];
		Mem::FillZ(dev, sizeof(dev));
		TInt cnt = 0;
		for(cnt=0;cnt<KTestedDevCnt;++cnt)
			{
			TBool res = CreateScreenDeviceLC(cnt, mode[ii], dev[cnt]);
			if(res)
				{
				InitScreenDeviceLC(dev[cnt], mode[ii]);
				_LIT(KLog,"Created Screen Device with mode %S on screen %d");
				INFO_PRINTF3(KLog,&ColorModeName(mode[ii]),cnt);
				//destroy
				CleanupStack::PopAndDestroy(2,dev[cnt]);
				}
			else
				{
				_LIT(KLog,"No support for display mode %S on screen %d");
				const TDesC* name=&ColorModeName(mode[ii]);
				INFO_PRINTF3(KLog, name, cnt);
				RDebug::Print(KLog, name, cnt);
				break;
				}
			}
		}
	}

//Bitmap device creation test
void CTScdv::TestBitmapDeviceCreationL()
	{
	TDisplayMode mode[] = {EColor256, EColor4K, EColor16M, EColor16MU, EColor16MA, EColor16MAP, EColor64K, 
						   EGray256, EGray16, EGray4, EGray2, EColor16};
	for(TInt ii=0;ii<TInt(sizeof(mode)/sizeof(mode[0]));++ii)
		{
		//create
		TSize size(10, 20);
		TUint8* bitmapMem = new (ELeave) TUint8[ByteSize(size, mode[ii])];
		CleanupStack::PushL(bitmapMem);
		CFbsDrawDevice* bitmapDev = CFbsDrawDevice::NewBitmapDeviceL(size, mode[ii], ByteSize(size, mode[ii]) / size.iHeight);
		CleanupStack::PushL(bitmapDev);
		//initialize
		bitmapDev->SetAutoUpdate(EFalse);
		bitmapDev->SetBits(bitmapMem);
		//destroy
		CleanupStack::PopAndDestroy(2);//bitmapMem, bitmapDev
		}
	}

void CTScdv::RunTestCaseL(TInt aCurTestCase)
	{
    ((CTScdvStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		INFO_PRINTF1(_L("Mutiple screen devices creation test"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0033
*/
		((CTScdvStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0033"));
		TestScreenDeviceCreationL();
		break;
	case 2:
		INFO_PRINTF1(_L("Bitmap device creation test"));
/**
	@SYMTestCaseID GRAPHICS-SCREENDRIVER-0034
*/
		((CTScdvStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCREENDRIVER-0034"));
		TestBitmapDeviceCreationL();
		break;
	case 3:
		((CTScdvStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTScdvStep*)iStep)->CloseTMSGraphicsStep();
 		TestComplete();
 		break;
		}
	((CTScdvStep*)iStep)->RecordTestResultL();
	}

//--------------
__CONSTRUCT_STEP__(Scdv)


void CTScdvStep::TestSetupL()
	{
	TInt temp = 0;
	HAL::Get(HALData::EDisplayColors, temp);//force HAL memory allocation
	}
