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
 @internalComponent
*/

#include "t_pseudoappwindow.h"
#include "t_winutils.h"

EXPORT_C CTestWindow::CTestWindow(TInt aScreenNo, TDisplayMode aMode)
	{
	RDebug::Print(_L("Creating CTestWindow class\n"));
    TRAPD(err, ConstructL(aScreenNo, aMode));
	if(err)
		{
		RDebug::Print(_L("CTestWindow::ConstructL() leaves with error %d\n"), err);
		User::Exit(0);
		}
	}

CTestWindow::~CTestWindow()
	{
	if(iScreen)
		{
		delete iScreen;
		}

	if(iWindowGc)
		{
		delete iWindowGc;
		}

	iSession.Close();
	}

EXPORT_C void CTestWindow::ConstructL(TInt aScreenNo, TDisplayMode aMode)
	{
	RDebug::Print(_L("Constructing CTestWindow class\n"));
	User::LeaveIfError(iSession.Connect());

	RDebug::Print(_L("Create CWsScreenDevice\n"));
	iScreen = new(ELeave) CWsScreenDevice(iSession);
	User::LeaveIfError(iScreen->Construct(aScreenNo));

	RDebug::Print(_L("Create RWindowGroup\n"));
	iGroup = RWindowGroup(iSession);

	User::LeaveIfError(iGroup.Construct(8970 + aScreenNo, ETrue));

	RDebug::Print(_L("Create Window\n"));
	iWindow = RWindow(iSession);
	User::LeaveIfError(iWindow.Construct((RWindowTreeNode)iGroup,(TUint32)this));

	RDebug::Print(_L("Create CWindowGc\n"));
	iWindowGc = new (ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iWindowGc->Construct());

	TInt ret = iWindow.SetRequiredDisplayMode(aMode);
	RDebug::Print(_L("Window display mode set to %d\n"), ret);

	iMode = aMode;
	iWindow.Activate();
	}

EXPORT_C void CTestWindow::Rotate(TSize /*aScreenSize*/)
	{
	}

EXPORT_C void CTestWindow::DrawL()
	{
	}

void CTestWindow::SetPosition()
	{
	iWindow.SetPosition((*iLoadsaParameters)[iFrameCounter].iWindowPos);
	iSession.Flush();
	}

void CTestWindow::SetSize()
	{
	iWindow.SetSize((*iLoadsaParameters)[iFrameCounter].iWindowSize);
	iSession.Flush();
	}

void CTestWindow::SetExtent()
	{
	iWindow.SetExtent((*iLoadsaParameters)[iFrameCounter].iWindowPos, (*iLoadsaParameters)[iFrameCounter].iWindowSize);
	iSession.Flush();
	}

EXPORT_C CTestEmptyWindow* CTestEmptyWindow::NewL(TInt aScreenNo, TDisplayMode aMode, const TSize& aScreenSize)
    {
    CTestEmptyWindow* self = new (ELeave) CTestEmptyWindow(aScreenNo, aMode);
    CleanupStack::PushL(self);
    self->ConstructL(aScreenSize);
    CleanupStack::Pop(); // self;
    return self;
    }

void CTestEmptyWindow::ConstructL(const TSize& aScreenSize)
	{
	RDebug::Print(_L("Construct CTestEmptyWindow class\n"));
	iWindowDrawn = EFalse;
	iWindow.SetExtent(TPoint(0,0), aScreenSize);
	}

EXPORT_C CTestEmptyWindow::CTestEmptyWindow(TInt aScreenNo, TDisplayMode aMode) :
                                   CTestWindow(aScreenNo, aMode)
	{
	}

CTestEmptyWindow::~CTestEmptyWindow()
	{
	}

EXPORT_C void CTestEmptyWindow::Rotate(TSize /*aScreenSize*/)
	{
	}

EXPORT_C void CTestEmptyWindow::DrawL()
	{
	iWindowDrawn = ETrue;
	iWindowGc->Activate(iWindow);
	iWindow.Invalidate();
	iWindow.BeginRedraw();
	iWindow.EndRedraw();
	iWindowGc->Deactivate();

	iSession.Flush();
	}

EXPORT_C CTestEcomWindow* CTestEcomWindow::NewL(TInt aScreenNo, TDisplayMode aMode, const TPtrC& aMultiBitmapEcomFile,
                                                TInt aMultiBitmapEcomFileSize, TGceTestResults* aGceTestResults,
                                                CArrayFixFlat<TDrawParameters>* aLoadsaParameters)
    {
    CTestEcomWindow* self = new (ELeave) CTestEcomWindow(aScreenNo, aMode);
    CleanupStack::PushL(self);
    self->ConstructL(aMultiBitmapEcomFile, aMultiBitmapEcomFileSize, aGceTestResults, aLoadsaParameters);
    CleanupStack::Pop(); // self;
    return self;
    }

void CTestEcomWindow::ConstructL(const TPtrC& aMultiBitmapEcomFile, TInt aMultiBitmapEcomFileSize, TGceTestResults* aGceTestResults,
                                 CArrayFixFlat<TDrawParameters>* aLoadsaParameters)
	{
	RDebug::Print(_L("Construct CTestEcomWindow class\n"));

	iMultiBitmapEcomFile.Set(aMultiBitmapEcomFile);
	iMultiBitmapEcomFileSize = aMultiBitmapEcomFileSize;
	iLoadsaParameters = aLoadsaParameters;

	//Pass notification value back to ui
	iGceTestResults = aGceTestResults;
	}

EXPORT_C CTestEcomWindow::CTestEcomWindow(TInt aScreenNo, TDisplayMode aMode) :
                                   CTestWindow(aScreenNo, aMode)
	{
	}

CTestEcomWindow::~CTestEcomWindow()
	{
	}

EXPORT_C void CTestEcomWindow::Rotate(TSize /*aScreenSize*/)
	{
	}

EXPORT_C void CTestEcomWindow::DrawL()
	{
	if(iBitmapCounter == iMultiBitmapEcomFileSize)
		{
		iBitmapCounter = 0;
		}

	TParamChange redraw = Redraw();
	if(redraw != ENoParamChange)
		{
		if(redraw == EPositionChange)
			{
			SetPosition();
			}

		if(redraw == ESizeChange)
			{
			SetSize();
			}

		if(redraw == EExtentChange)
			{
			SetExtent();
			}
		}

	CFbsBitmap bitmap;
	iWindowGc->Activate(iWindow);
	iWindow.Invalidate();
	iWindow.BeginRedraw();

	bitmap.Load(iMultiBitmapEcomFile, iBitmapCounter);

	iWindowGc->DrawBitmap(TRect(TPoint(0,0), (*iLoadsaParameters)[0].iWindowSize), &bitmap);

	iWindow.EndRedraw();
	iWindowGc->Deactivate();
	iSession.Flush();

	iFrameCounter++;
	iBitmapCounter++;
	}

TParamChange CTestEcomWindow::Redraw()
	{
	if(iFrameCounter == 0)
		{
		iWindow.SetExtent((*iLoadsaParameters)[0].iWindowPos, (*iLoadsaParameters)[0].iWindowSize);
		iWindow.SetVisible(ETrue);

		return EFirstFrame;
		}
	else
		{
		TDrawParameters current = (*iLoadsaParameters)[iFrameCounter];
		TDrawParameters last =  (*iLoadsaParameters)[iFrameCounter - 1];

		if(current.iWindowSize != last.iWindowSize)
			{
			if(current.iWindowPos != last.iWindowPos)
				{
				return EExtentChange;
				}
			return ESizeChange;
			}
		else if(current.iWindowPos != last.iWindowPos)
			{
			return EPositionChange;
			}
		}
	return ENoParamChange;
	}

EXPORT_C CTestUiWindow* CTestUiWindow::NewL(TInt aScreenNo, TDisplayMode aMode, const TPtrC& aMultiBitmapUiFile, TInt aMultiBitmapUiFileSize,
                                   CArrayFixFlat<TRect>* aRectArray, TInt aFrameDuration, TInt aMultiBitmapUiFileTransparency,
                                   TGceTestResults* aGceTestResults,
                                   CArrayFixFlat<TDrawParameters>* aLoadsaParameters)
    {
    CTestUiWindow* self = new (ELeave) CTestUiWindow(aScreenNo ,aMode);
    CleanupStack::PushL(self);
    self->ConstructL(aMultiBitmapUiFile, aMultiBitmapUiFileSize, aRectArray, aFrameDuration,
                     aMultiBitmapUiFileTransparency, aGceTestResults,
                     aLoadsaParameters);

    CleanupStack::Pop(); // self;
    return self;
    }

void CTestUiWindow::ConstructL(const TPtrC& aMultiBitmapUiFile, TInt aMultiBitmapUiFileSize, CArrayFixFlat<TRect>* aRectArray,
                               TInt aFrameDuration, TInt aMultiBitmapUiFileTransparency, TGceTestResults* aGceTestResults,
                               CArrayFixFlat<TDrawParameters>* aLoadsaParameters)
	{
	RDebug::Print(_L("Construct CTestUiWindow class\n"));

	iSimUiDrawn = EFalse;
	iMultiBitmapUiFile.Set(aMultiBitmapUiFile);
	iMultiBitmapUiFileSize = aMultiBitmapUiFileSize;
	iMultiBitmapUiFileTransparency = aMultiBitmapUiFileTransparency;
	iRectArray = aRectArray;
	iFrameDuration = aFrameDuration;
	iLoadsaParameters = aLoadsaParameters;

	//Pass notification value back to ui
	iGceTestResults = aGceTestResults;
	}

EXPORT_C CTestUiWindow::CTestUiWindow(TInt aScreenNo, TDisplayMode aMode) :
                             CTestWindow(aScreenNo, aMode)
	{
	}

CTestUiWindow::~CTestUiWindow()
	{
	}

void CTestUiWindow::SetBitmapTransparency(CFbsBitmap* aBitmap, TUint8 aAlphaValue, TInt aAlphaByteLocation)
	{
	aBitmap->LockHeap();
	TUint32* bitmapBufferPtr = aBitmap->DataAddress();
	if(bitmapBufferPtr == 0)
		{
		RDebug::Print(_L("Error getting bitmap address, aborting\n"));
		//Unlock heap before exitting
		aBitmap->UnlockHeap();
		User::Exit(0);
		}

	TSize aBitmapSize = aBitmap->SizeInPixels();
	TUint32 noOfPixels = aBitmapSize.iHeight * aBitmapSize.iWidth;

	while(noOfPixels)
		{
		*bitmapBufferPtr = ( (*bitmapBufferPtr & ~(0xFF << (aAlphaByteLocation * 8))) | (aAlphaValue << (aAlphaByteLocation * 8)) );
		bitmapBufferPtr++;
		noOfPixels--;
		}
	aBitmap->UnlockHeap();
	}

EXPORT_C void CTestUiWindow::Rotate(const TSize& aScreenSize)
	{
	//Adjust the sizes of the UI bitmap components to reflect the aspect ratio  change
	for(TInt j=0; j<iMultiBitmapUiFileSize; j++)
		{
		(*iRectArray)[j].iTl.iX = ((*iRectArray)[j].iTl.iX*aScreenSize.iWidth)/aScreenSize.iHeight;
		(*iRectArray)[j].iTl.iY = ((*iRectArray)[j].iTl.iY*aScreenSize.iHeight)/aScreenSize.iWidth;
		(*iRectArray)[j].iBr.iX = ((*iRectArray)[j].iBr.iX*aScreenSize.iWidth)/aScreenSize.iHeight;
		(*iRectArray)[j].iBr.iY = ((*iRectArray)[j].iBr.iY*aScreenSize.iHeight)/aScreenSize.iWidth;
		}

	iWindow.SetSize(aScreenSize);
	}

EXPORT_C void CTestUiWindow::DrawL()
	{
	TParamChange redraw = Redraw();

		if(redraw == EPositionChange)
			{
			SetPosition();
			}

		if(redraw == ESizeChange)
			{
			SetSize();
			}

		if(redraw == EExtentChange)
			{
			SetExtent();
			}

		CFbsBitmap bitmap;
		iWindowGc->Activate(iWindow);
		iWindow.Invalidate();
		iWindow.BeginRedraw();

		CTestBitmap* bitmapSource = NULL;
		bitmapSource = CTestBitmap::NewL(TSize(0,0), iMode);

		for(TInt j=0; j<iMultiBitmapUiFileSize; j++)
			{
			User::LeaveIfError(bitmapSource->Bitmap().Load(iMultiBitmapUiFile, j));

			TInt lastSize = (*iLoadsaParameters)[iFrameCounter].iBitmapScale;
			TRect destRect;
			TPoint drawPos;


			destRect.SetRect(TPoint(  ((*iRectArray)[j].iTl.iX*lastSize)/100, ((*iRectArray)[j].iTl.iY*lastSize)/100),
						     TPoint(  ((*iRectArray)[j].iBr.iX*lastSize)/100, ((*iRectArray)[j].iBr.iY*lastSize)/100 ));

			drawPos = TPoint(((*iRectArray)[j].iTl.iX*lastSize)/100, ((*iRectArray)[j].iTl.iY*lastSize)/100);

			if(iMultiBitmapUiFileTransparency)
				{
				CTestBitmap* bitmapSourceAlpha = CTestBitmap::NewL(TSize(0,0), EGray256);
				User::LeaveIfError(bitmapSourceAlpha->Bitmap().Load(iMultiBitmapUiFile, j+iMultiBitmapUiFileSize));

				//Adjust the transparency controlled by the alpha bitmap
				SetBitmapAlpha(&(bitmapSourceAlpha->Bitmap()));

				CTestBitmap* bitmapDest = CTestBitmap::NewL(destRect.Size(), iMode);
				bitmapDest->Bitmap().SetSizeInTwips(bitmapSource->Bitmap().SizeInTwips());
				bitmapDest->Gc().DrawBitmap(TRect(TPoint(0,0), destRect.Size()), &(bitmapSource->Bitmap()));

				CTestBitmap* bitmapDestAlpha = CTestBitmap::NewL(destRect.Size(), EGray256);
				bitmapDestAlpha->Bitmap().SetSizeInTwips(bitmapSourceAlpha->Bitmap().SizeInTwips());
				bitmapDestAlpha->Gc().DrawBitmap(TRect(TPoint(0,0), destRect.Size()), &(bitmapSourceAlpha->Bitmap()));

				iWindowGc->AlphaBlendBitmaps(drawPos, &(bitmapDest->Bitmap()), TRect(TPoint(0,0), destRect.Size()),
				                             &(bitmapDestAlpha->Bitmap()), TPoint(0,0));

				delete bitmapSourceAlpha;
				delete bitmapDest;
				delete bitmapDestAlpha;
				}
			else
				{
				iWindowGc->DrawBitmap(destRect, &(bitmapSource->Bitmap()));
				}
			}

		delete bitmapSource;

		iWindow.EndRedraw();
		iWindowGc->Deactivate();
		iSession.Flush();

	iFrameCounter++;
	}

void CTestUiWindow::SetBitmapAlpha(CFbsBitmap* aBitmap)
	{
	aBitmap->LockHeap();
	TUint32* bitmapBufferPtr = aBitmap->DataAddress();

	TSize aBitmapSize = aBitmap->SizeInPixels();
	TUint32 noOfPixels = (aBitmapSize.iHeight * aBitmapSize.iWidth)/4;
	TUint8 alpha = (*iLoadsaParameters)[iFrameCounter].iBitmapAlpha;

	TUint32 alpha32 = (alpha << 24) | (alpha << 16) | (alpha << 8) | alpha;
	while(noOfPixels)
		{
		//Only modify grey pixels leaving the white ones untouched
		if(*bitmapBufferPtr != 0x0)
			{
			*bitmapBufferPtr = alpha32;
			}

		bitmapBufferPtr++;
		noOfPixels--;
		}
	aBitmap->UnlockHeap();
	}

TParamChange CTestUiWindow::Redraw()
	{
	if(iFrameCounter == 0)
		{
		iWindow.SetExtent((*iLoadsaParameters)[0].iWindowPos, (*iLoadsaParameters)[0].iWindowSize);
		iWindow.SetVisible(ETrue);

		return EFirstFrame;
		}
	else
		{
		TDrawParameters current = (*iLoadsaParameters)[iFrameCounter];
		TDrawParameters last =  (*iLoadsaParameters)[iFrameCounter - 1];

		if(current.iWindowSize != last.iWindowSize)
			{
			if(current.iWindowPos != last.iWindowPos)
				{
				return EExtentChange;
				}
			return ESizeChange;
			}
		else if(current.iWindowPos != last.iWindowPos)
			{
			return EPositionChange;
			}
		else if(current.iBitmapScale != last.iBitmapScale)
			{
			return EBitmapScale;
			}
		else if(current.iBitmapAlpha != last.iBitmapAlpha)
			{
			return EOpacityChange;
			}
		}
	return ENoParamChange;
	}

TDrawParameters::TDrawParameters() : iRedraw(EFalse), iBitmapScale(0), iBitmapAlpha(0), iWindowSize(0,0), iWindowPos(0,0)
	{
	}

TDrawParameters::~TDrawParameters()
	{
	}

TBool TDrawParameters::operator==(const TDrawParameters& x) const
	{
	if( (x.iBitmapScale != iBitmapScale) ||
		(x.iBitmapAlpha != iBitmapAlpha) ||
		(x.iWindowSize != iWindowSize)   ||
		(x.iWindowPos != iWindowPos) )
		{
		return ETrue;
		}

	return EFalse;
	}
