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

#include <e32math.h>
#include <hal.h>
#include <bitdraw.h>
#include <bitdrawinterfaceid.h>
#include <graphics/gdi/gdiconsts.h>
#include "tscdvdevorientation.h"

void AutoPanic(TInt aPanic)
	{
	User::Panic(_L("Auto"),aPanic);
	}

LOCAL_C TInt PanicThreadFunc(TAny *aPointerToAny)
	{
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	SPanicParams *pointerPanicParams=(SPanicParams *)aPointerToAny;
	TInt ret;
	TRAP(ret,(*pointerPanicParams->functionName)(*pointerPanicParams->pointerDeviceOrientation,*pointerPanicParams->pointerDisplayMode,*pointerPanicParams->pointerScreenNo));
	delete CleanUpStack;
	if (ret==EWsExitReasonBad)
		AutoPanic(EAutoPanicPanicFailed);
	return(ret);
	}

CPanicTest::CPanicTest()
	{
	
	}
	
CPanicTest::~CPanicTest()
	{
	
	}
TInt CPanicTest::LaunchPanicThread(RThread &aThread, SPanicParams *aPointerPanicParams)
	{
	TBuf<32> threadName;
	threadName.Format(TRefByValue<const TDesC>(_L("AutoPanicThread%d")),iThreadNumber++);
	return(aThread.Create(threadName,PanicThreadFunc,KDefaultStackSize,KPanicThreadHeapSize,KPanicThreadHeapSize,aPointerPanicParams,EOwnerThread));
	}

TInt CPanicTest::TestPanicL(SPanicParams *aPointerPanicParams, TInt aExitReason, const TDesC &aCategory, TBool* aTestFinished)
	{
	RThread thread;
	TRequestStatus stat;
	TInt err=LaunchPanicThread(thread, aPointerPanicParams);
	if (err==KErrAlreadyExists)
		{
		// wait for kernel to clear up old threads
		// and have several attempts at starting the thread
		// if unsuccessful the first time
		for (TInt i=0;i<3;i++)
			{
			User::After(TTimeIntervalMicroSeconds32(100000));		//0.1 secs
			err=LaunchPanicThread(thread, aPointerPanicParams);
			if (err!=KErrAlreadyExists)
				break;
			}
		}
	User::LeaveIfError(err);
	thread.Logon(stat);
	User::SetJustInTime(EFalse);
	thread.Resume();
	User::WaitForRequest(stat);
	User::SetJustInTime(ETrue);

	TBool testFinished=EFalse;
	TBool testPassed=EFalse;
	if (thread.ExitType()==EExitPanic)	   // the function is panicked
		{
		// ignore code for transparency
		if(aExitReason == thread.ExitReason())  // panic reason is equal to the scdv EScreenDriverPanicInvalidParameter
			{
			testPassed=ETrue;
  			}
		iActualPanicCode=thread.ExitReason();
		testFinished=ETrue;	// Finish tests
		}
	else
		{
		if((thread.ExitCategory().Compare(aCategory)!=0)
		|| (thread.ExitReason()!=aExitReason)
		|| (thread.ExitType()!=EExitPanic)		//This last condition is always true!!!!!!
			)
			{
			testPassed=EFalse;
			}			
		iActualPanicCode=0;
		}

	if(aTestFinished)
		*aTestFinished=testFinished;
	thread.Close();
	return(testPassed);
	}

TInt CPanicTest::TestInvalidModePanicL(TPanicFunction aFunction, TScreenDriverPanic aExitReason,TDisplayMode* aPointerDispMode, TDeviceOrientation* aPointerDevOrientation,TInt* aPtrScreenNo, TBool* aTestFinished)
	{
	// function name, exit reason, and parameters
	return TestPanicL(aFunction,aExitReason,KSCDVPanicCategory, aPointerDispMode, aPointerDevOrientation, aPtrScreenNo, aTestFinished);
	}
	
TInt CPanicTest::TestPanicL(TPanicFunction aFunction, TInt aExitReason, const TDesC &aCategory, TDisplayMode* aPointerDispMode, TDeviceOrientation* aPointerDevOrientation,TInt* aPtrScreenNo, TBool* aTestFinished)
	{
	SPanicParams params;
	params.functionName=aFunction;
	params.pointerDeviceOrientation = aPointerDevOrientation;
	params.pointerDisplayMode = aPointerDispMode;
	params.pointerScreenNo = aPtrScreenNo;
	return TestPanicL(&params, aExitReason, aCategory, aTestFinished);
	}


//
//Test code
//

CTDevOrientation::CTDevOrientation(CTestStep *aTest, RArray<TInt> aArrayDispModes) :
				CTGraphicsBase(aTest),iArrayDispModesPerScreen(aArrayDispModes)
	{
	INFO_PRINTF1(_L("Screen Device Orientation tests"));
	}

CTDevOrientation::CTDevOrientation(CTestStep *aTest) :
				CTGraphicsBase(aTest)
	{
	INFO_PRINTF1(_L("Screen Device Orientation tests"));
	}

void CTDevOrientation::ConstructL()	
	{
	iPanicTest = new (ELeave) CPanicTest();
	}
	
CTDevOrientation::~CTDevOrientation()
	{
	iArrayOrientation.Close();
	iArrayDispModesPerScreen.Close();
	delete iPanicTest;
	}

/**
Create the screen device and surface interface objects for the specific screen and display mode
@internalAll
@return ETrue means valid screen device and surface interface objects are created, EFalse means a leave or an error
@param aScreenNo the screen no
@param aDisplayMode the specified display mode
@param aDrawScreenDevice 
@param aSurfaceIdInterface
*/
TBool CTDevOrientation::GetSurfaceIdInterfaceL(const TInt& aScreenNo,const TDisplayMode& aDisplayMode, CFbsDrawDevice*& aDrawScreenDevice, MSurfaceId*& aSurfaceIdInterface )
	{
	
	TInt address = NULL;
	User::LeaveIfError(HAL::Get(aScreenNo, HALData::EDisplayMemoryAddress,address));
	User::LeaveIfError(HAL::Get(aScreenNo, HALData::EDisplayXPixels, iPhysSize.iWidth));
	User::LeaveIfError(HAL::Get(aScreenNo, HALData::EDisplayYPixels, iPhysSize.iHeight));
	__ASSERT_DEBUG(iPhysSize.iWidth > 0 && iPhysSize.iHeight > 0 && address != NULL, User::Invariant());

   	aDrawScreenDevice = NULL;
	TRAPD(err, aDrawScreenDevice = CFbsDrawDevice::NewScreenDeviceL(aScreenNo, aDisplayMode));

	if(err == KErrNone)	  // as aDisplayMode is verified to be supported so this should be the case
		{  
		__ASSERT_ALWAYS(aDrawScreenDevice != NULL, User::Invariant());
		//Set other properties
    	aDrawScreenDevice->SetUserDisplayMode(aDisplayMode);
		aDrawScreenDevice->SetAutoUpdate(EFalse);
		if(!aSurfaceIdInterface)
			{
			err = aDrawScreenDevice->GetInterface(KSurfaceInterfaceID, reinterpret_cast <TAny*&> (aSurfaceIdInterface));
			}
		if (err == KErrNone && aSurfaceIdInterface != NULL)
			{
			return ETrue;
			}
		else 
			{
			delete aDrawScreenDevice;
			return EFalse;
			}
		}
	else 
	 	{
	 	// Leave with error codes
	 	INFO_PRINTF2(_L("CFbsDrawDevice::NewScreenDeviceL leaves with code =%d"), err);
	 	return EFalse;
	 	}
	}

/**
Retrive the required bpp value and pixel format based on the specified display mode
@internalAll
@return ETrue means the display mode is supported
@param aDisplayMode the specified display mode
@param aBpp the required bpp value
@param aPixelFormat the required pixel format 
*/
TBool CTDevOrientation::ReqBppAndPixelFormat(TDisplayMode aDisplayMode, TInt& aBpp, TUint32& aPixelFormat)	
	{
//	aBpp = TDisplayModeUtils::NumDisplayModeBitsPerPixel(aDisplayMode);

	switch(aDisplayMode)
		{
	case EGray2:
		aPixelFormat = 0;
        aBpp = 1;
        break;
    case EGray4:
		aPixelFormat = 0;
        aBpp = 2;
        break;
    case EGray16:
    case EColor16:
		aPixelFormat = 0;
        aBpp = 4;
        break;
    case EGray256:
    case EColor256: // 8 Bpp color mode
		aPixelFormat = 0;
		aBpp = 8;
		break;
	case EColor4K:
		aPixelFormat = EUidPixelFormatXRGB_4444; //12 Bpp color mode
		aBpp = 12;
		break;
	case EColor64K: // 16 Bpp color mode
		aPixelFormat = EUidPixelFormatRGB_565;
		aBpp = 16;
		break;
	case EColor16MU:   //24 Bpp color mode
		aPixelFormat = EUidPixelFormatXRGB_8888;
		aBpp = 24;
		break;
	case EColor16MA:   // 32 Bpp color mode
		aPixelFormat = EUidPixelFormatARGB_8888;
		aBpp = 32;
		break;
	case EColor16MAP:
		aPixelFormat = EUidPixelFormatARGB_8888_PRE;
		aBpp = 32;
		break;
		default:
		return EFalse;	
		}  
	return ETrue;
	}
	
void CTDevOrientation::TestGetSurfaceWithValidDispModeL(const MSurfaceId* aSurfaceIdInterface, const TDisplayMode& aDisplayMode, TInt aScreenNo)
	{
	// Sanity check
	__ASSERT_ALWAYS(aSurfaceIdInterface != NULL, User::Invariant());
	TSurfaceId surfaceId;
	aSurfaceIdInterface->GetSurface(surfaceId);
	INFO_PRINTF1(_L("Test GetSurface() returns valid surface Id with the specified display modes"));		
	//Check the screen no
	TEST(surfaceId.iInternal[0] == aScreenNo);
	// Hal mode
	TInt halMode = surfaceId.iInternal[1];	 // 24bpp and 32bpp are currently considered the same by HAL
	User::LeaveIfError(HAL::Get(surfaceId.iInternal[0], HALData::EDisplayBitsPerPixel, halMode));
    TUint32 pixelFormat = 0;
    TInt bpp = 0;
    if (ReqBppAndPixelFormat(aDisplayMode,bpp, pixelFormat))
    	{
    	// Check the color depth 
   		TEST(bpp == halMode || bpp == 24 && halMode == 32 || bpp == 32 && halMode == 24);
		// Check the pixel format
		TEST(surfaceId.iInternal[2] == pixelFormat);//Need to test the type as well
		// Check the surface type
		TEST(surfaceId.iInternal[3]>>24 == TSurfaceId::EScreenSurface );
		}
	else
		INFO_PRINTF1(_L("Unsupported display modes"));			
	}

/**
Retrive all the availably supported device orientation modes by the hardware.
@internalAll
@return ETrue means there are valid device orientation modes supported, EFalse means no good mode supported
@param aSurfaceIdInterface MSurfaceId* 
*/
TBool CTDevOrientation::ChooseOrientationMode( MSurfaceId* aSurfaceIdInterface)
	{
	__ASSERT_ALWAYS(aSurfaceIdInterface != NULL, User::Invariant());
	TUint orAvailable = aSurfaceIdInterface->DeviceOrientationsAvailable();
	iArrayOrientation.Reset();
	if ( orAvailable <= 15 || orAvailable > 0)
		{
		if (iArrayOrientation.Count() == 0)	   // Only fills the array once
			{
			// there is at least one mode available
			// Find one availabe device orientation mode
			for (TUint32 mode = (TUint32)EDeviceOrientationNormal; mode <= (TUint)EDeviceOrientation270CW; mode*=2)
				{
				if ( orAvailable & (TDeviceOrientation)mode)
					{
					iArrayOrientation.Append((TDeviceOrientation)mode);
					}
				}
			} 
		return ETrue;
		}
	else 
		{
		// No available mode supported
		return EFalse;
		}
	}

void CTDevOrientation::TestGetDeviceOrientation( MSurfaceId* aSurfaceIdInterface)
	{
    if (ChooseOrientationMode(aSurfaceIdInterface) && iArrayOrientation.Count() != 0)
    	{
       	INFO_PRINTF1(_L("Test DeviceOrientationAvailabe() returns one or a combination of valid modes"));		
    	}
    else
       	INFO_PRINTF1(_L("Test DeviceOrientationAvailabe() gives no valid returns"));		
	}

	
void CTDevOrientation::TestDeviceOrientationSettingandGetting( MSurfaceId* aSurfaceIdInterface)
	{
	__ASSERT_ALWAYS(aSurfaceIdInterface != NULL, User::Invariant());
    if (ChooseOrientationMode(aSurfaceIdInterface) && iArrayOrientation.Count() != 0)
    	{
       	for (TInt i = 0; i < iArrayOrientation.Count(); i++)
       		{
	       	// Set the orientation
			aSurfaceIdInterface->SetDeviceOrientation(iArrayOrientation[i]);
			INFO_PRINTF2(_L("Test SetDeviceOrientation() and GetDeviceOrientation() for DeviceOrientation Mode %d"), i);
			//Get the orientation
			TEST(iArrayOrientation[i] == aSurfaceIdInterface->DeviceOrientation());		
       		}
    	}
	else
		{
		// No avaialbe orientation modes
		INFO_PRINTF1(_L("No availabe device orientation modes supported by the device"));
		}
	}

void CTDevOrientation::TestHorizontalVerticalTwips(const CFbsDrawDevice* aDrawScreenDevice,  MSurfaceId* aSurfaceIdInterface, TDisplayMode aDisplayMode, TInt aScreenNo)
	{
    __ASSERT_ALWAYS(aSurfaceIdInterface != NULL, User::Invariant());
	TInt horTwipsRef = aDrawScreenDevice->HorzTwipsPerThousandPixels();
	TInt verTwipsRef = aDrawScreenDevice->VertTwipsPerThousandPixels();

	TUint32 pixelFormat = 0;
    TInt bpp = 0;
    // Find the required color depth and pixel format based on the aDisplayMode
    TEST(ReqBppAndPixelFormat(aDisplayMode,bpp, pixelFormat));
    
    TSurfaceId surfaceId;	
   	
   	if (ChooseOrientationMode(aSurfaceIdInterface) && iArrayOrientation.Count() != 0 )
    	{
	   	for (TInt i = 0; i < iArrayOrientation.Count(); i++)
    		{
 			// Set the orientation
			if (aSurfaceIdInterface->SetDeviceOrientation(iArrayOrientation[i]))
				{
				// Retrieve SurfaceId to test the second bit
				aSurfaceIdInterface->GetSurface(surfaceId);
				INFO_PRINTF1(_L("CFbsDrawDevice, Retrieve the surfaceId to test whether the second bit has been modified accordingly and other bits left unchanged"));		
				//Check the values
				TEST(surfaceId.iInternal[0] == aScreenNo);
				TInt halMode = surfaceId.iInternal[1] &~TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag;
				HAL::Get(surfaceId.iInternal[0], HALData::EDisplayBitsPerPixel, halMode);
		   	
				TInt horTwipsRotated = aDrawScreenDevice->HorzTwipsPerThousandPixels();
				TInt verTwipsRotated = aDrawScreenDevice->VertTwipsPerThousandPixels();
		   	
				if (iArrayOrientation[i] & (EDeviceOrientation90CW | EDeviceOrientation270CW))
					{  
					INFO_PRINTF2(_L("Test DeviceOrientation Mode %d returns swapped horizontal and vertical twip values"), i);
					TEST(horTwipsRef == verTwipsRotated);
					TEST(verTwipsRef == horTwipsRotated);
					TEST((surfaceId.iInternal[1] & TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag) != 0); //flipped
					}
				else 
					{
					INFO_PRINTF2(_L("Test DeviceOrientation Mode %d returns no swapped horizontal and vertical twip values"), i);
					TEST(verTwipsRef == verTwipsRotated);
					TEST(horTwipsRef == horTwipsRotated);
					TEST((surfaceId.iInternal[1] & TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag) == 0); //no flipped
					}
				// Check the color depth unchanged
 	  			TEST(bpp == halMode || bpp == 24 && halMode == 32 || bpp == 32 && halMode == 24);
				TEST(surfaceId.iInternal[2] == pixelFormat);//Need to test the type as well
				TEST(surfaceId.iInternal[3]>>24 == TSurfaceId::EScreenSurface );
 				}
 			else
	    		INFO_PRINTF1(_L("No attributes have been changed after calling SetDeviceOrientation()"));
    		}
    	}
    else 
    	{
		INFO_PRINTF1(_L("No availabe device orientation modes supported by the device"));
    	}

	}
	
void CTDevOrientation::TestSetDeviceOrientationWithValidMode(const CFbsDrawDevice* aDrawScreenDevice,  MSurfaceId* aSurfaceIdInterface, TDisplayMode aDisplayMode, TInt aScreenNo)
	{
	__ASSERT_ALWAYS(aSurfaceIdInterface != NULL, User::Invariant());
	TSize sizeRef = aDrawScreenDevice->SizeInPixels();
	TSize sizeRotated;
	TUint32 pixelFormat = 0;
    TInt bpp = 0;
    // Find the required color depth and pixel format based on the aDisplayMode
    TEST(ReqBppAndPixelFormat(aDisplayMode,bpp, pixelFormat));
    
    TSurfaceId surfaceId;	
   	
   	if (ChooseOrientationMode(aSurfaceIdInterface) && iArrayOrientation.Count() != 0 )
    	{
	   	for (TInt i = 0; i < iArrayOrientation.Count(); i++)
    		{
 			// Set the orientation
			if (aSurfaceIdInterface->SetDeviceOrientation(iArrayOrientation[i]))
				{
				// Retrieve SurfaceId to test the second bit
				aSurfaceIdInterface->GetSurface(surfaceId);
				INFO_PRINTF1(_L("CFbsDrawDevice, Retrieve the surfaceId to test whether the second bit has been modified accordingly and other bits left unchanged"));		
				//Check the values
				TEST(surfaceId.iInternal[0] == aScreenNo);
				TInt halMode = surfaceId.iInternal[1] &~TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag;
				HAL::Get(surfaceId.iInternal[0], HALData::EDisplayBitsPerPixel, halMode);
				// Retrieve the size in pixel after setting the device rotation
	  			sizeRotated = aDrawScreenDevice->SizeInPixels();
		   	
				if (iArrayOrientation[i] & (EDeviceOrientation90CW | EDeviceOrientation270CW))
					{  
					INFO_PRINTF2(_L("Test DeviceOrientation Mode %d returns swapped screen Height and Width"), i);
					TEST(sizeRef.iHeight == sizeRotated.iWidth);
					TEST(sizeRef.iWidth == sizeRotated.iHeight);
					TEST((surfaceId.iInternal[1] & TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag) != 0); //has set to 8 ---  flipped
					}
				else 
					{
					INFO_PRINTF2(_L("Test DeviceOrientation Mode %d returns no swapped screen Height and Width"), i);
					TEST(sizeRef.iWidth == sizeRotated.iWidth);
					TEST(sizeRef.iHeight == sizeRotated.iHeight);
					TEST((surfaceId.iInternal[1] & TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag) == 0); //has set to 0  --- no flipped
					}
				// Check the color depth unchanged
 	  			TEST(bpp == halMode || bpp == 24 && halMode == 32 || bpp == 32 && halMode == 24);
				TEST(surfaceId.iInternal[2] == pixelFormat);//Need to test the type as well
				TEST(surfaceId.iInternal[3]>>24 == TSurfaceId::EScreenSurface );
				}
			else
	    		INFO_PRINTF1(_L("No attributes have been changed after calling SetDeviceOrientation()"));
    		}
    	}
    else 
    	{
		INFO_PRINTF1(_L("No availabe device orientation modes supported by the device"));
    	}
	}


// THIS METHOD IS EXPECTED TO PANIC
LOCAL_C TInt DoSetDeviceOrientationWithInvalidModeL(TDeviceOrientation aOrientation, TDisplayMode aDisplayMode, TInt aScreenNo)
	{
	CFbsDrawDevice* drawScreenDevice =  NULL;
	TRAPD(err, drawScreenDevice = CFbsDrawDevice::NewScreenDeviceL(aScreenNo, aDisplayMode));
    CleanupStack::PushL(drawScreenDevice);
    if(err == KErrNone)
		{  
		__ASSERT_ALWAYS(drawScreenDevice != NULL, User::Invariant());
		//Set other properties
    	drawScreenDevice->SetUserDisplayMode(EColor16MA);
		drawScreenDevice->SetAutoUpdate(EFalse);
		MSurfaceId* surfaceIdInterface = NULL;
		err = drawScreenDevice->GetInterface(KSurfaceInterfaceID, reinterpret_cast <TAny*&> (surfaceIdInterface));
		if (err == KErrNone)
			{
			// Currently Panics the secondary thread
			surfaceIdInterface->SetDeviceOrientation(aOrientation);
			}
		}
	else
		{
		User::LeaveIfError(err);
		}
	CleanupStack::PopAndDestroy();
	return(EWsExitReasonBad);
	}


void CTDevOrientation::TestSetDeviceOrientationWithInvalidModeL(TDisplayMode aDisplayMode)
	{
	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes -1"));
	// Invalid bit is set
    TDeviceOrientation aInvalidMode = (TDeviceOrientation)-1;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, (TDisplayMode*)&aDisplayMode, &aInvalidMode,&iCurrScreen));

   	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 0"));
    aInvalidMode = (TDeviceOrientation)0;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, &aDisplayMode,&aInvalidMode,&iCurrScreen));

	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 3"));
    aInvalidMode = (TDeviceOrientation)3;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter,&aDisplayMode, &aInvalidMode,&iCurrScreen));

   	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 5"));
    aInvalidMode = (TDeviceOrientation)5;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, &aDisplayMode,&aInvalidMode,&iCurrScreen));

	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 7"));
    aInvalidMode = (TDeviceOrientation)7;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, &aDisplayMode,&aInvalidMode,&iCurrScreen));

	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 9"));
    aInvalidMode = (TDeviceOrientation)9;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, &aDisplayMode,&aInvalidMode,&iCurrScreen));
    
   	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 15"));
    aInvalidMode = (TDeviceOrientation)15;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, &aDisplayMode,&aInvalidMode,&iCurrScreen));

   	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 16"));
    aInvalidMode = (TDeviceOrientation)16;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter,&aDisplayMode, &aInvalidMode,&iCurrScreen));

	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 17"));
  	aInvalidMode = (TDeviceOrientation)17;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, &aDisplayMode,&aInvalidMode,&iCurrScreen));

   	INFO_PRINTF1(_L("Test SetDeviceOrientation() with invalid device orientation modes 32"));
  	aInvalidMode = (TDeviceOrientation)32;
    // Check the test results in the secondary thread
	TEST(iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, &aDisplayMode,&aInvalidMode,&iCurrScreen));

	}

void CTDevOrientation::TestValidButUnavailableOrientationL(MSurfaceId* aSurfaceIdInterface, TDisplayMode aDisplayMode)
	{

	__ASSERT_ALWAYS(aSurfaceIdInterface != NULL, User::Invariant());
    if (ChooseOrientationMode(aSurfaceIdInterface) && iArrayOrientation.Count() != 0)
    	{// Find a valid but unavailable device orientation
    	if (iArrayOrientation.Count() ==4)
    		{
			INFO_PRINTF1(_L("All four possible device orientation modes are supported"));
    		}
    	else
    		{ // There must be one valid but unsupported display  modes
    		RArray<TDeviceOrientation> arrayValidUnsupOrientation;
    		CleanupClosePushL(arrayValidUnsupOrientation); 
    		
    		for (TUint32 mode = (TUint32)EDeviceOrientationNormal; mode <= (TUint32)EDeviceOrientation270CW; mode *= 2)
    			{
    			// Insert into the arry arrayValidUnsupOrientation
    			if (iArrayOrientation.Find((TDeviceOrientation)mode) == KErrNotFound)
	    		    {
	    			arrayValidUnsupOrientation.Append((TDeviceOrientation)mode);
	    			}
    			}

    		for (TInt i = 0; i < arrayValidUnsupOrientation.Count(); i++)
 				{
 				INFO_PRINTF2(_L("Test SetDeviceOrientation() with valid but unsupported display modes %d"), i);
   				// Also retrieve the current device orientation for reference
   				TDeviceOrientation devOrientationRef = aSurfaceIdInterface->DeviceOrientation();
   				TBool panicTestPass=iPanicTest->TestInvalidModePanicL(&(DoSetDeviceOrientationWithInvalidModeL),EScreenDriverPanicInvalidParameter, &aDisplayMode, &arrayValidUnsupOrientation[i],&iCurrScreen);
   				if (!panicTestPass)
   					if (iPanicTest->iActualPanicCode==0)
   						{
   						INFO_PRINTF1(_L("Panic test failed to panic!?"));
   						}
   					else
   						{
   						INFO_PRINTF2(_L("Panic test: wrong code %i"),iPanicTest->iActualPanicCode);
   						}
   				TEST(panicTestPass);
				// Call GetDeviceOrientation() again to check the orientation mode hasnt been changed
				INFO_PRINTF1(_L("Test the current device orientation mode has not been changed when the second thread panics"));
				TEST(devOrientationRef == aSurfaceIdInterface->DeviceOrientation());	
				}
			CleanupStack::PopAndDestroy();
      		}
    	}
    else 
    	{
		// No avaialbe orientation modes
		INFO_PRINTF1(_L("No availabe device orientation modes supported by the device"));
    	}
	}
/**	Copies the given screendevice to a new screendevice
 *  Assertion is that if the old screen device is different rotation then the updated screen device gets the rotation of the old device.
 *  Try to force the devices to have different rotations before we start in order to detect that the update does occur.
 * 
 **/
void CTDevOrientation::TestSetDisplayMode(CFbsDrawDevice* aDrawScreenDevice,MSurfaceId* aSurfaceIdInterface)
	{
	// Create a new screendevice and surfaceId
	CFbsDrawDevice* inputScreenDevice = NULL;
	MSurfaceId* inputSurfaceIdInterface = NULL;
	
	TRAPD(err, inputScreenDevice = CFbsDrawDevice::NewScreenDeviceL(iCurrScreen, aDrawScreenDevice->DisplayMode()));
	if(err == KErrNone && !inputSurfaceIdInterface)
		{
		err = inputScreenDevice->GetInterface(KSurfaceInterfaceID, reinterpret_cast <TAny*&> (inputSurfaceIdInterface));
		}
	TInt beforeOrientation=aSurfaceIdInterface->DeviceOrientation();
	TInt inputOrientation=inputSurfaceIdInterface->DeviceOrientation();
	TInt alternateOrientation=(inputOrientation>1)?(inputOrientation>>1):(inputOrientation<<1);

	if (err == KErrNone)
		{
		if (beforeOrientation==inputOrientation)
			{
			// Set orientation on aDrawScreenDevice
			// (deviceOrientation*2) mod 8 
		  	INFO_PRINTF1(_L("Test SetDisplayMode() copys "));
		  	if (aSurfaceIdInterface->DeviceOrientationsAvailable()&alternateOrientation)
		  		{
				TBool didIt=aSurfaceIdInterface->SetDeviceOrientation(TDeviceOrientation(alternateOrientation));
			  	INFO_PRINTF1(_L("[done SetDeviceOrientation] SetDisplayMode() copys "));
			  	TEST(!didIt|| aSurfaceIdInterface->DeviceOrientation() == alternateOrientation);
		  		}
		  	else
		  		{
		  		INFO_PRINTF1(_L("*** source device couldn't change orientation ***"));
		  		}
			}
	  	INFO_PRINTF1(_L("Test SetDisplayMode() copys "));
		aDrawScreenDevice->SetDisplayMode(inputScreenDevice);
	  	INFO_PRINTF1(_L("[done SetDisplayMode] SetDisplayMode() copys "));
	  	
		// Compare the orientation if they are the same... right...
		if (inputSurfaceIdInterface->DeviceOrientationsAvailable()&inputOrientation)
			{
			TEST(aSurfaceIdInterface->DeviceOrientation() == inputOrientation);
			}
		else
			{	//target device doesn't support this orientation
	  		INFO_PRINTF1(_L("*** target device couldn't change orientation to honour copied source device ***"));
			TEST(aSurfaceIdInterface->DeviceOrientation() == beforeOrientation);
			}
	  	INFO_PRINTF1(_L("[done DeviceOrientation] SetDisplayMode() copys "));
		}	
	else
		{
	  	INFO_PRINTF1(_L("Error in GetInterface()"));
		}
	delete inputScreenDevice;
	}

void CTDevOrientation::RunTestCaseL(TInt aCurTestCase)
	{
	if (aCurTestCase < 9)
   		{
		//All screens
		for (TInt count = 0; count < iArrayDispModesPerScreen.Count(); count++)
			{
			iCurrScreen=count;
	    	for (TInt ind = 0;iArrayDispModesPerScreen[count]>=(1<<ind); ind++)
	    		{
	    		TDisplayMode mode= (TDisplayMode)ind;
	    		if (iArrayDispModesPerScreen[count]&(1<<ind))
	    			{
				  	INFO_PRINTF3(_L("The current screen number =%d The current display mode =%d"), count, ind);
					CFbsDrawDevice* drawScreenDevice = NULL;
					MSurfaceId* surfaceIdInterface = NULL;
					if (!GetSurfaceIdInterfaceL(count,mode, drawScreenDevice, surfaceIdInterface))
	    				{
	   		  			INFO_PRINTF1(_L("** GCE INTERFACE NOT SUPPORTED ***"));
	    				}
					else
	    				{
		
	    				CleanupStack::PushL(drawScreenDevice);
	    				((CTDevOrientationStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	    				switch(aCurTestCase)
							{
/**
@SYMTestCaseID			GRAPHICS-SCDV-0010
@SYMPREQ			PREQ1019
@SYMREQ				REQ8228,REQ8223,REQ8227		
@SYMTestPriority		High 
@SYMTestCaseDesc		This test gets a screen surface and checks the expected values of the surfaceID
@SYMTestStatus			Implemented

@SYMTestActions			Call CFbsDrawDevice::CreateScreenDeviceL(), CFbsDrawDevice::GetInterface(), MSurfaceId::GetSurface() 
@SYMTestExpectedResults This test passes with surfaceId has the expected values in each field.  				
*/
		case 1:
			((CTDevOrientationStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCDV-0010"));
			TestGetSurfaceWithValidDispModeL(surfaceIdInterface,mode, count);
			break;
/**
@SYMTestCaseID			GRAPHICS-SCDV-0011
@SYMPREQ			PREQ1019
@SYMREQ				REQ8228,REQ8223,REQ8227	
@SYMTestPriority		High 
@SYMTestCaseDesc		This test checks DeviceOrientationAvailable() returns a valid orientation mode or a combination of them
SYMTestStatus			Implemented

@SYMTestActions			Call CFbsDrawDevice::CreateScreenDeviceL(), CFbsDrawDevice::GetInterface(), MSurfaceId::DeviceOrientationAvailable() 
@SYMTestExpectedResults This test passes after checking the return values of DeviceOrientationAvailable()  
*/
		case 2:
			((CTDevOrientationStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCDV-0011"));
			TestGetDeviceOrientation(surfaceIdInterface);
			break;
/**
@SYMTestCaseID			GRAPHICS-SCDV-0012
@SYMPREQ			PREQ1019	
@SYMREQ				REQ8228,REQ8223,REQ8227
@SYMTestPriority		High 
@SYMTestCaseDesc		This test checks the change of screen size before and after calling SetDeviceOrientation with a valid orientation mode
@SYMTestStatus			Implemented

@SYMTestActions			Call CFbsDrawDevice::CreateScreenDeviceL(), CFbsDrawDevice::GetInterface(), MSurfaceId::SetDeviceOrientation(), CFbsDrawDevice::SizeInPixel() 
@SYMTestExpectedResults In the case of EDeviceOrientation90CW and EDeviceOrientation270CW the size width and height are swapped after calling SetDeviceOrientation, while in other cases, the values remained the same before and after the rotation setting.  
*/
		case 3:
			((CTDevOrientationStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCDV-0012"));
			TestSetDeviceOrientationWithValidMode(drawScreenDevice, surfaceIdInterface,mode,count);

			break;
/**
@SYMTestCaseID			GRAPHICS-SCDV-0013
@SYMPREQ			PREQ1019
@SYMREQ				REQ8228,REQ8223,REQ8227	
@SYMTestPriority		High 
@SYMTestCaseDesc		This test checks the change of horizontal and vertical twips before and after calling SetDeviceOrientation with a valid orientation mode
@SYMTestStatus			Implemented

@SYMTestActions			Call CFbsDrawDevice::CreateScreenDeviceL(), CFbsDrawDevice::GetInterface(), MSurfaceId::SetDeviceOrientation(), CFbsDrawDevice::HorzTwipsPerThousandPixels() and CFbsDrawDevice:: VertTwipsPerThousandPixels() 
@SYMTestExpectedResults In the case of EDeviceOrientation90CW and EDeviceOrientation270CW the horizontal and vertical twips are swapped after calling SetDeviceOrientation, while in other cases, the values remained the same before and after the rotation setting.  
*/
		case 4:
			((CTDevOrientationStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCDV-0013"));
			TestHorizontalVerticalTwips(drawScreenDevice, surfaceIdInterface,mode, count);
			break;
/**
@SYMTestCaseID			GRAPHICS-SCDV-0014
@SYMPREQ			PREQ1019
@SYMREQ				REQ8228,REQ8223,REQ8227	
@SYMTestPriority		High 
@SYMTestCaseDesc		This test checks the functionality of SetDeviceOrientation() and DeviceOrientation()
@SYMTestStatus			Implemented

@SYMTestActions			Call CFbsDrawDevice::CreateScreenDeviceL(), CFbsDrawDevice::GetInterface(), MSurfaceId::SetDeviceOrientation(), MSurfaceId::DeviceOrientation() 
@SYMTestExpectedResults The orientation mode value passed into SetDeviceOrientation is identical to that returned from calling DeviceOrientation().  
*/
		case 5:
			((CTDevOrientationStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCDV-0014"));
			TestDeviceOrientationSettingandGetting(surfaceIdInterface);
			break;
/**
@SYMTestCaseID			GRAPHICS-SCDV-0015
@SYMPREQ			PREQ1019
@SYMREQ				REQ8228,REQ8223,REQ8227	
@SYMTestPriority		High 
@SYMTestCaseDesc		This test checks SetDeviceOrientation does not accept an invalid device orientation mode
@SYMTestStatus			Implemented

@SYMTestActions			Call CFbsDrawDevice::CreateScreenDeviceL(), CFbsDrawDevice::GetInterface(), MSurfaceId::SetDeviceOrientation() 
@SYMTestExpectedResults This test panic with code EScreenDriverPanicInvalidParameter.  
*/
		case 6:
			((CTDevOrientationStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCDV-0015"));
			TestSetDeviceOrientationWithInvalidModeL(mode);
			break;
/**
@SYMTestCaseID			GRAPHICS-SCDV-0016
@SYMPREQ			PREQ1019
@SYMREQ				REQ8228,REQ8223,REQ8227	
@SYMTestPriority		High 
@SYMTestCaseDesc		This test checks SetDeviceOrientation does not accept a valid but unsupported device orientation mode
@SYMTestStatus			Implemented

@SYMTestActions			Call CFbsDrawDevice::CreateScreenDeviceL(), CFbsDrawDevice::GetInterface(), MSurfaceId::SetDeviceOrientation() 
@SYMTestExpectedResults This test panic with code EScreenDriverPanicInvalidParameter.  
*/		
		case 7:
			((CTDevOrientationStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCDV-0016"));
			TestValidButUnavailableOrientationL(surfaceIdInterface,mode);
			break;
/**
@SYMTestCaseID			GRAPHICS-SCDV-0017
@SYMPREQ			PREQ1019
@SYMREQ				REQ8228,REQ8223,REQ8227	
@SYMTestPriority		High 
@SYMTestCaseDesc		This test compares the two draw devices after copying one to the other
@SYMTestStatus			Implemented

@SYMTestActions			Call CFbsDrawDevice::CreateScreenDeviceL(), CFbsDrawDevice::GetInterface(), CFbsDrawDevice::SetDisplayMode(), MSurfaceId::SetDeviceOrientation() 
@SYMTestExpectedResults This test should pass all the device orientation checking.  
*/
		case 8:
			((CTDevOrientationStep*)iStep)->SetTestStepID(_L("GRAPHICS-SCDV-0017"));
			TestSetDisplayMode(drawScreenDevice,surfaceIdInterface);
			break;
							}
	    				((CTDevOrientationStep*)iStep)->RecordTestResultL();
	    				CleanupStack::PopAndDestroy();
				   		}
					}

				}
			}
		}
    else
    	{
        ((CTDevOrientationStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
        ((CTDevOrientationStep*)iStep)->CloseTMSGraphicsStep();
    	TestComplete();
    	}
		((CTDevOrientationStep*)iStep)->RecordTestResultL();
	}


//--------------------------------------------------------------

CTDevOrientationStep::CTDevOrientationStep() 
	{ 
	SetTestStepName(KTDevOrientationStep); 
	} 
	 
CTGraphicsBase* CTDevOrientationStep::CreateTestL()
	{ 
	CTGraphicsBase* theTest = NULL;
	CFbsDrawDevice* device = NULL;
	TInt numOfScreens = 0;
	TInt err = HAL::Get(HALData::EDisplayNumberOfScreens, numOfScreens);
	RArray<TInt> arrayDispModesPerScreen;
	
	CleanupClosePushL(arrayDispModesPerScreen);
	for (TUint scr=0;scr<numOfScreens;scr++)
		{
		TUint modeBits=0;
		// go through all the display modes and find out the supported ones by hardware/emulator
	    for (TUint32 ind = 0; ind <= (TUint32)EColorLast; ind++)
	    	{
	   	    TDisplayMode mode2 = (TDisplayMode)ind;

	    	TRAPD(err, device = CFbsDrawDevice::NewScreenDeviceL(scr, mode2));
	   		if (err==KErrNone)  //support this mode
				{
				modeBits+=1<<ind;
				}// otherwise, unsupport this mode does nothing
	    	delete device;
	    
			device = NULL;
	     	}
	    arrayDispModesPerScreen.Append(modeBits);
		}
	
    // Create the test class 	
	if (arrayDispModesPerScreen.Count() > 0)
		{
		// there is at least one supported display modes
		theTest = new (ELeave) CTDevOrientation(this, arrayDispModesPerScreen);
 		}
 	else
 		{
		User::Leave(KErrNotSupported);
 		}
	 	
    CleanupStack::Pop(&arrayDispModesPerScreen);
	return 	theTest;		
	} 


void CTDevOrientationStep::TestSetupL()
	{
	TInt temp = 0;
	HAL::Get(KDefaultScreenNo, HALData::EDisplayColors, temp);//force HAL memory allocation
	
	}
