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
 @internalComponent - Internal Symbian test code
*/

#include "graphicsscreencomparison.h"
#include "graphicsimagecomparison.h"

/**
Creates an instance of the CTGraphicsScreenComparison class. The class is
destroyed by a call to the class destructor when no longer required.

This function will leave if memory cannot be allocated for the class object or it's
construction fails. e.g. the CSC renderstage plugin is not accessible

The function will panic if the CWsScreenDevice object has not been constructed

@param aScreenDevice, a reference to a CWsScreenDevice object created for the
screen for which image comparison is required.
@pre The CSC renderstage plugin must be accessible.
@pre The CWsScreenDevice object must have been constructed
@post The CTGraphicsScreenComparison object has been created
@return  On success, a pointer to the CTGraphicsScreenComparison object, on failure,
the function will leave or panic.
*/
EXPORT_C CTGraphicsScreenComparison* CTGraphicsScreenComparison::NewL(CWsScreenDevice& aScreenDevice)
	{
	CTGraphicsScreenComparison* self = new(ELeave) CTGraphicsScreenComparison();
	CleanupStack::PushL(self);
	self->ConstructL(aScreenDevice);
	CleanupStack::Pop(self);
	return self;
	}

void CTGraphicsScreenComparison::ConstructL(CWsScreenDevice& aScreenDevice)
	{
	iScreenSize = aScreenDevice.SizeInPixels();

	//Create a screen sized bitmap for use in screen comparison
	iBmp = new (ELeave) CFbsBitmap;

	//push the git
	User::LeaveIfError(iBmp->Create(aScreenDevice.SizeInPixels(), EColor16MU));


	//Access the screen comparison class - render stage must be available
	iCsc = static_cast<MTestScreenCapture*> (aScreenDevice.GetInterface(MTestScreenCapture::KUidTestScreenCaptureIf));
	if(!iCsc)
		{
		RDebug::Printf("Failed to create CTGraphicsScreenComparison, (CSC render stage may not be specified in wsini.ini?) , aborting");
		User::Leave(0);
		}

	//pop the git

	}

/**
Compares the contents of a rectangular region of the screen with a similarly sized
rectangular region of a reference bitmap.

@param aCompareSize, a const reference to a TSize object denoting the size of the
rectangular region for comparison. Negative and zero dimensions of this argument can
be passed in without returning a KErrArgument error.
@param aScreenPoint, a const reference to a TPoint object denoting the top left
point of the screen rectangle for comparison.
@param aBitmapPoint, a const reference to a TPoint object denoting the top left
point of the bitmap rectangle for comparison.
@param aRefBitmap, a const reference to the reference CFbsBitmap for use in comparison
@param aTestStepName, a desciptor to identify output files in the event of a mismatch.
Defaults to KNullDesC.
@pre The aRefBitmap must have been created with mode EColor16MU.
@pre The rectanglular comparison region must reside wholly inside the screen and
reference bitmap.
@pre To enable outputting of screen snapshots in the event of a mismatch, the
c:\logs\testexecute\screencomparison\ folder must exist.
@return KErrNone if the pixels contained in the screen and bitmap rectangles are an
exact match or, otherwise, the count of the first unmatched pixel. Pixels are compared
from TRUE top-left to bottom-right in the horizontal direction (TRUE to cope with
negative dimensions of the aCompareSize object) An area of zero size will return
KErrNone so long as the pre-conditions are satisfied.
KErrArgument if the pre-conditions are not met.
@post If an mismatch has occured, a snapshot of the screen is saved in an MBM file in the
c:\logs\testexecute\screencomparison\ folder
*/
EXPORT_C TInt CTGraphicsScreenComparison::CompareScreenImageL(const TSize& aCompareSize,
                                                              const TPoint& aScreenPoint,
                                                              const TPoint& aBitmapPoint,
                                                              const CFbsBitmap& aRefBitmap,
                                                              const TDesC& aTestStepName)
	{
	if(aRefBitmap.DisplayMode() != EColor16MU)
		{
		return KErrArgument;
		}

    TInt err = iCsc->ComposeScreen(*iBmp);
    if(err != KErrNone)
        {
        return err;
        }
	    
	err = CTGraphicsImageComparison::CompareBitmaps(aCompareSize,
                                                         aBitmapPoint,
                                                         aScreenPoint,
                                                         aRefBitmap,
                                                         *iBmp);
    if (err != KErrNone)
        {
        TRAP_IGNORE(SaveBitmapL(*iBmp, aTestStepName));
        }
    return err;
	}

/**
Compares the contents of a rectangular region of the screen with a reference colour.

@param aCompareSize, a const reference to a TSize object denoting the size of the
rectangular region for comparison. Negative and zero dimensions of this argument can
be passed in without returning a KErrArgument error.
@param aScreenPoint, a const reference to a TPoint object denoting the top left
point of the screen rectangle for comparison.
@param aColor, the TRgb colour value to compare the region to.
@param aTestStepName, a desciptor to identify output files in the event of a mismatch.
Defaults to KNullDesC.
@pre The rectanglular comparison region must reside wholly inside the screen
@pre To enable outputting of screen snapshots in the event of a mismatch, the
c:\logs\testexecute\screencomparison\ folder must exist.
@return KErrNone if the pixels contained in the screen rectangle matches the reference
colour or, otherwise, the count of the first unmatched pixel. Pixels are compared
from TRUE top-left to bottom-right in the horizontal direction (TRUE to cope with
negative dimensions of the aCompareSize object) An area of zero size will return
KErrNone so long as the pre-conditions are satisfied.
KErrArgument if the pre-conditions are not met.
@post If an mismatch has occured, a snapshot of the screen is saved in an MBM file in the
c:\logs\testexecute\screencomparison\ folder
*/
EXPORT_C TInt CTGraphicsScreenComparison::CompareScreenImageL(const TSize& aCompareSize,
                                                              const TPoint& aScreenPoint,
                                                              const TRgb& aColor,
                                                              const TDesC& aTestStepName)
	{
	TInt err = iCsc->ComposeScreen(*iBmp);
	if(err != KErrNone)
	    {
	    return err;
	    }

	err = CTGraphicsImageComparison::CompareBitmaps(aCompareSize,
                                                         aScreenPoint,
                                                         *iBmp,
                                                         aColor);
    if (err != KErrNone)
        {
        TRAP_IGNORE(SaveBitmapL(*iBmp, aTestStepName));
        }
    return err;
	}
/**
Returns a pointer to the underlying MTestScreenCapture class
@return the MTestScreenCapture* pointer
*/
EXPORT_C MTestScreenCapture* CTGraphicsScreenComparison::GetMTestScreenCapture() const
	{
	return iCsc;
	}

/**
CTGraphicsScreenComparison class destructor. Destroys the instance of the
CTGraphicsScreenComparison class created by a call to CTGraphicsScreenComparison::NewL()
*/
EXPORT_C CTGraphicsScreenComparison::~CTGraphicsScreenComparison()
	{
	delete iBmp;
	iBmp = 0;
	}

CTGraphicsScreenComparison::CTGraphicsScreenComparison()
	{
	}

void CTGraphicsScreenComparison::SaveBitmapL(CFbsBitmap& aBitmap, const TDesC& aTestStepName)
    {
    RBuf fileName;
    fileName.CreateL(KDir.iTypeLength + aTestStepName.Length() + KMbmSuffix.iTypeLength + KFixedNumWidth + 1);
    fileName.CleanupClosePushL();

    fileName.Insert(0, KDir);
    fileName.Append(aTestStepName);
    fileName.Append('_');
    fileName.AppendNumFixedWidth((++iSavedBitmapCounter), EDecimal, KFixedNumWidth);
    fileName.Append(KMbmSuffix);

    User::LeaveIfError(aBitmap.Save(fileName));
    CleanupStack::PopAndDestroy(&fileName);
    }
