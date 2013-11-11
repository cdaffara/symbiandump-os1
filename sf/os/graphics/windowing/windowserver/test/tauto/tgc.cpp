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
#include "tgc.h"
#include "RemoteGc.h"
#include "CommandBuffer.h"
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
#include "directgdigcwrapper.h"
#include <graphics/directgdidriver.h>
#include <graphics/sgutils.h>
#include <graphics/wsdrawresource.h>
#endif

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
GLDEF_C void CopyImageToBitmapL(CFbsBitmap* aBitmap, const RSgImage& aImage, const TRect& aRect);
GLDEF_C void CopyImageToDestination(TAny* aDataAddressDest, TInt aDataStrideDest, TDisplayMode aDisplayModeDest, 
				TAny* aDataAddressSrc, TInt aDataStrideSrc, TDisplayMode aDisplayModeSrc, const TRect& aRect);
GLDEF_C void CopyImageToDestination64K(TAny* aDataAddressDest, TInt aDataStrideDest, TDisplayMode aDisplayModeDest, 
		TUint16* aDataAddressSrc, TInt aDataStrideSrc, const TRect& aRect);

GLDEF_C void CopyImageToBitmapL(CFbsBitmap* aBitmap, const RSgImage& aImage, const TRect& aRect)
	{
	TSgImageInfo info;
	TInt res = aImage.GetInfo(info);
	if(res == KErrNone)
		{
		info.iUsage = ESgUsageNone;
		info.iCpuAccess = ESgCpuAccessReadOnly;
		RSgImage image;
		res = image.Create(info, aImage);
		if(res == KErrNone)
			{	
			const TAny* dataAddressSrc = NULL; 
			TInt dataStrideSrc = 0;
			res = image.MapReadOnly(dataAddressSrc, dataStrideSrc);
			if(res == KErrNone)
				{
				const TDisplayMode displayModeDest = aBitmap->DisplayMode();
				const TDisplayMode displayModeSrc = SgUtils::PixelFormatToDisplayMode(info.iPixelFormat);
				TUint32* dataAddressDest = aBitmap->DataAddress();
				const TInt dataStrideDest = aBitmap -> DataStride();
				TSize bitmapSize = aBitmap->SizeInPixels();
				TRect rect = aRect;
				TRect rectDest = info.iSizeInPixels;
				rect.Intersection(rectDest);
				if(rect.Height() > bitmapSize.iHeight)
					{
					rect.SetHeight(bitmapSize.iHeight);
					}
				if(rect.Width() > bitmapSize.iWidth)
					{
					rect.SetWidth(bitmapSize.iWidth);
					}
				CopyImageToDestination((TAny*)dataAddressDest, dataStrideDest, displayModeDest, (TAny*)dataAddressSrc, 
								dataStrideSrc, displayModeSrc, rect);

				image.Unmap();
				}
			image.Close();
			}
		}
	}

GLDEF_C void CopyImageToDestination(TAny* aDataAddressDest, TInt aDataStrideDest, TDisplayMode aDisplayModeDest, 
				TAny* aDataAddressSrc, TInt aDataStrideSrc, TDisplayMode aDisplayModeSrc, const TRect& aRect) 
	{
	if(aRect.IsEmpty())
		return;
	
	if((aDisplayModeDest == aDisplayModeSrc) && (aDataStrideSrc == aDataStrideDest))
		{
		Mem::Copy(aDataAddressDest, aDataAddressSrc, aDataStrideDest * aRect.Height());
		return;
		}
	
	switch(aDisplayModeSrc)
		{
	case EColor64K:
		{
		CopyImageToDestination64K(aDataAddressDest, aDataStrideDest, aDisplayModeDest, 
						(TUint16*)aDataAddressSrc, aDataStrideSrc, aRect);
		break;
		}
	default:
		break;
		}
	}

GLDEF_C void CopyImageToDestination64K(TAny* aDataAddressDest, TInt aDataStrideDest, TDisplayMode aDisplayModeDest, 
		TUint16* aDataAddressSrc, TInt aDataStrideSrc, const TRect& aRect) 
	{
	const TInt bppSrc = 2;
	const TInt width = aRect.Width();
	const TInt height = aRect.Height();
	const TInt dataStrideLengthSrc = aDataStrideSrc / bppSrc;
	TUint16* dataAddressSrc =  aDataAddressSrc + aRect.iTl.iY * dataStrideLengthSrc + aRect.iTl.iX; 
	const TUint16* dataAddressSrcEnd = dataAddressSrc + dataStrideLengthSrc *  height;  

	switch(aDisplayModeDest)
		{
	case EColor64K:
		{
		TUint16* dataAddressDest = static_cast<TUint16*> (aDataAddressDest); 
		const TInt dataStrideLengthDest = aDataStrideDest / bppSrc;
		while(dataAddressSrcEnd > dataAddressSrc)
			{
			Mem::Copy(dataAddressDest, dataAddressSrc, width * bppSrc);
			dataAddressSrc += dataStrideLengthSrc;
			dataAddressDest += dataStrideLengthDest;
			}
		break;
		}
	case EColor16MU:
		{
		const TInt bppDest = 4;
		TUint32* dataAddressDest = static_cast<TUint32*> (aDataAddressDest); 
		const TInt dataStrideLengthDest = aDataStrideDest / bppDest;
		
		while(dataAddressSrcEnd > dataAddressSrc)
			{
			const TUint16* dataAddressSrcLineEnd = dataAddressSrc + width;
			TUint32* dataAddressDestCur = dataAddressDest;
			TUint16* dataAddressSrcCur = dataAddressSrc;

			while(dataAddressSrcLineEnd > dataAddressSrcCur)
				{
				*dataAddressDestCur = TRgb::Color64K(*dataAddressSrcCur).Color16MU();
				dataAddressDestCur++;
				dataAddressSrcCur++;
				}
			dataAddressSrc += dataStrideLengthSrc;
			dataAddressDest += dataStrideLengthDest;
			}
		break;
		}
	case EGray4:
		{
		TUint8* dataAddressDest = static_cast<TUint8*> (aDataAddressDest);
		const TInt dataStrideLengthDest = aDataStrideDest;
		
		while(dataAddressSrcEnd > dataAddressSrc)
			{
			const TUint8* dataAddressDstLineEnd = dataAddressDest + aDataStrideDest;
			TUint8* dataAddressDestCur = dataAddressDest;
			TUint16* dataAddressSrcCur = dataAddressSrc;

			while(dataAddressDstLineEnd > dataAddressDestCur)
				{
				*dataAddressDestCur = 0;
				for(TInt index = 0; index < 8; index +=2)
					{
					TInt col = TRgb::Color64K(*dataAddressSrcCur).Gray4();
					col <<= index;
					*dataAddressDestCur |= col;
					dataAddressSrcCur++;
					}
				dataAddressDestCur++;
				}
			dataAddressSrc += dataStrideLengthSrc;
			dataAddressDest += dataStrideLengthDest;
			}
		break;
		}
	case EColor256:
		{
		TUint8* dataAddressDest = static_cast<TUint8*> (aDataAddressDest);
		const TInt dataStrideLengthDest = aDataStrideDest;
		
		while(dataAddressSrcEnd > dataAddressSrc)
			{
			const TUint8* dataAddressDstLineEnd = dataAddressDest + aDataStrideDest;
			TUint8* dataAddressDestCur = dataAddressDest;
			TUint16* dataAddressSrcCur = dataAddressSrc;

			while(dataAddressDstLineEnd > dataAddressDestCur)
				{
				*dataAddressDestCur = TRgb::Color64K(*dataAddressSrcCur).Color256();
				dataAddressSrcCur++;
				dataAddressDestCur++;
				}
			dataAddressSrc += dataStrideLengthSrc;
			dataAddressDest += dataStrideLengthDest;
			}
		break;
		}
	default:
		break;
		}
	}

TDisplayMode DisplayModeFromPixelFormat(TUidPixelFormat aPixelFormat)
	{
	switch(aPixelFormat)
		{
	case EUidPixelFormatARGB_8888_PRE:
		return EColor16MAP;
	case EUidPixelFormatARGB_8888:
		return EColor16MA;
	case EUidPixelFormatRGB_565:
		return EColor64K;
	default:
		break;
		}
	return ENone;
	}

TUidPixelFormat PixelFormatFromDisplayMode(TDisplayMode aDisplayMode)
	{
	switch (aDisplayMode)
		{
		case EGray2:
		case EGray4:
		case EGray16:
		case EGray256:
		case EColor16:
		case EColor256:
		case EColor16M:
		case EColor16MU:
			{
			return EUidPixelFormatXRGB_8888;
			}
		case EColor4K:
			{
			return EUidPixelFormatXRGB_4444;
			}
		case EColor64K:
			{
			return EUidPixelFormatRGB_565;
			}
		case EColor16MA:
			{
			return EUidPixelFormatARGB_8888;
			}
		case EColor16MAP:
			{
			return EUidPixelFormatARGB_8888_PRE;
			}
		default:
			{
			return EUidPixelFormatUnknown;
			}
		}
	}
#endif

CTGc::CTGc(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

CTGc::~CTGc()
	{
	delete iTest;
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	SgDriver::Close();
	CDirectGdiDriver *directGdiDriver = CDirectGdiDriver::Static();
	if(directGdiDriver)
		{
		directGdiDriver->Close();
		}
#endif
	}

void CTGc::ConstructL()
	{
	_LIT(KTestName,"GC Test");
	iTest=new(ELeave) CTestBase(KTestName,this);
	
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	TInt err = CDirectGdiDriver::Open();
	User::LeaveIfError(err);
	err = SgDriver::Open();
	if(err != KErrNone)
		{
		CDirectGdiDriver *directGdiDriver = CDirectGdiDriver::Static();
		if(directGdiDriver)
			{
			directGdiDriver->Close();
			}
		User::Leave(err);
		}
#endif
	}

//Class derived from MWsGraphicResolver. Used for playing the commands from command buffer
class CWSGraphicsRes: public CBase, public MWsGraphicResolver
	{
public:
	void DrawWsGraphic(TInt /*aId*/, TBool /*aIsUid*/, const TRect& /*aRect*/, const TDesC8& /*aData*/) const
		{
		//Orveriding by giving empty implemention
		}
	};

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
//
//Class CDrawTextInContextTest
//

CDrawTextInContextTest::CDrawTextInContextTest(){}

CDrawTextInContextTest::~CDrawTextInContextTest()
	{
	delete iRefBitmap;
	delete iRefDevice;
	delete iRefBitGc;
	delete iRemoteGc;
	iMsgBuf.Close();
	delete iCommandBuffer;
	delete iWsGraphicRes;
	
	TheClient->iScreen->ReleaseFont(iFont);

	delete iDirectGdiGcWrapper;
	if(iWrapperImageTarget)
		{
		iWrapperImageTarget->Close();
		}
	delete iWrapperImageTarget;
	iWrapperImage.Close();
	iWrapperImageCollection.Close();
	}

void CDrawTextInContextTest::BaseConstructL()
	{
	//Initialise font settings
	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Series 60 Sans");
	fsp.iHeight=430;
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInTwips((CFont*&)iFont,fsp));
	
	//Initialise TTextParameter
	iParam.iStart = 27;
	iParam.iEnd = 60;
	
	//Text to draw
	iText.Set(_L("This text will not be drawnK.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Latin This text will not be drawn"));
	
	//For reference bitmap
	iRefBitmap = new(ELeave) CFbsBitmap();
	User::LeaveIfError(iRefBitmap->Create(KBitmapSize, EColor64K));
	iRefDevice = CFbsBitmapDevice::NewL(iRefBitmap);
	User::LeaveIfError(iRefDevice->CreateContext(iRefBitGc));
	
	CDirectGdiDriver* theDGdiDriver = CDirectGdiDriver::Static();
	User::LeaveIfNull(theDGdiDriver);
	
	TSgImageInfo info;
	info.iUsage = ESgUsageDirectGdiTarget | ESgUsageDirectGdiSource | ESgUsageCompositionSource;
	info.iSizeInPixels = KBitmapSize;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	
	TInt res = iWrapperImageCollection.Create(info, 1);
	User::LeaveIfError(res);
	res = iWrapperImageCollection.OpenImage(0, iWrapperImage);
	User::LeaveIfError(res);
	iWrapperImageTarget = new (ELeave) RDirectGdiImageTarget(*theDGdiDriver);
	res = iWrapperImageTarget->Create(iWrapperImage);
	User::LeaveIfError(res);
	iDirectGdiGcWrapper = CDirectGdiGcWrapper::NewL(*iWrapperImageTarget);

	//clean image-----------------
	CDirectGdiGcWrapper* directGdiGcWrapper = CDirectGdiGcWrapper::NewL(*iWrapperImageTarget);
	CleanupStack::PushL(directGdiGcWrapper);

	directGdiGcWrapper->SetDrawMode(MWsGraphicsContext::EDrawModeWriteAlpha);
	directGdiGcWrapper->SetBrushColor(KRgbWhite);
	directGdiGcWrapper->Clear();

	CleanupStack::PopAndDestroy(1, directGdiGcWrapper);
	//------------------
	
	//Used to record draw commands
	iRemoteGc = CRemoteGc::NewL(TheClient->iScreen);
	
	//Used to play recorded draw commands
	iCommandBuffer = CCommandBuffer::NewL();
	
	//Dummy class created required for CCommandBuffer::Play
	iWsGraphicRes = new (ELeave) CWSGraphicsRes();
	
	//Offset for CCommandBuffer::Play
	iOffset = TPoint(0,0);
	
	//Result of doing the test
	iHasPassedTest = EFalse;
	}

void CDrawTextInContextTest::Test()
	{	
	/* Create reference bitmap by drawing using bitgc */
	iRefBitGc->UseFont(iFont);
	DoDrawTextBitGc();
	iRefBitGc->DiscardFont();
	
	/* Drawing using CBitGcWrapper via CRemotGc*/
	
	//Capturing the commands in remote gc
	iRemoteGc->BeginDraw(KBitmapRect);
	iRemoteGc->UseFont(iFont);
	DoDrawTextRemoteGc();
	iRemoteGc->DiscardFont();
	iRemoteGc->EndDraw();
		
	//Externalize the captured commands from remote gc in to a buffer
	iRemoteGc->ExternalizeL(iMsgBuf, ETrue);

	//Internalize the buffer with captured commands (from CRemoteGC) in to CCommandBuffer
	iCommandBuffer->InternalizeL(iMsgBuf.Pckg());
	
	//Play the commands on test window using command buffer
	iCommandBuffer->Play(iOffset,&KBitmapRegion,KBitmapRect,*iWsGraphicRes,*iDirectGdiGcWrapper);
	
	//Test to see if the bitmap drawn to using CRemoteGc is the same as the reference bitmap
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	
	TSgImageInfo info;
	iWrapperImage.GetInfo(info);
	TDisplayMode displayMode = DisplayModeFromPixelFormat(info.iPixelFormat);
	bitmap->Create(info.iSizeInPixels, displayMode);
	TRect rect(info.iSizeInPixels);
	CopyImageToBitmapL(bitmap, iWrapperImage, rect);
	
	iHasPassedTest = LossyCompareBitmap(*iRefBitmap, *bitmap, KBitmapRect, EFalse);

	CleanupStack::PopAndDestroy(bitmap);
	}

TBool CDrawTextInContextTest::HasPassedTest()
	{
	return iHasPassedTest;
	}

//
// Class DrawTextInContextTestPoint
//

CDrawTextInContextTestPoint::CDrawTextInContextTestPoint(){}

CDrawTextInContextTestPoint::~CDrawTextInContextTestPoint(){}

CDrawTextInContextTestPoint* CDrawTextInContextTestPoint::NewL()
	{
	CDrawTextInContextTestPoint* self = new(ELeave) CDrawTextInContextTestPoint;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CDrawTextInContextTestPoint::ConstructL()
	{
	BaseConstructL();
	iPosition = TPoint(0,0);
	}

void CDrawTextInContextTestPoint::DoDrawTextBitGc()
	{
	iRefBitGc->DrawText(iText,&iParam,iPosition);
	}

void CDrawTextInContextTestPoint::DoDrawTextRemoteGc()
	{
	iRemoteGc->DrawText(iText,&iParam,iPosition);
	}

//
// Class DrawTextInContextTestBox
//

CDrawTextInContextTestBox::CDrawTextInContextTestBox(){}

CDrawTextInContextTestBox::~CDrawTextInContextTestBox(){}

CDrawTextInContextTestBox* CDrawTextInContextTestBox::NewL()
	{
	CDrawTextInContextTestBox* self = new(ELeave) CDrawTextInContextTestBox;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CDrawTextInContextTestBox::ConstructL()
	{
	BaseConstructL();
	iClipFillRect = TRect(10,50,640,120);
	iBaselineOffset = 40;
	iTTextAlign = CGraphicsContext::ELeft;
	}

void CDrawTextInContextTestBox::DoDrawTextBitGc()
	{
	iRefBitGc->DrawText(iText,&iParam,iClipFillRect,iBaselineOffset,iTTextAlign);
	}

void CDrawTextInContextTestBox::DoDrawTextRemoteGc()
	{
	iRemoteGc->DrawText(iText,&iParam,iClipFillRect,iBaselineOffset,iTTextAlign);
	}

//
// Class CDrawTextInContextTestPointVertical
//

CDrawTextInContextTestPointVertical::CDrawTextInContextTestPointVertical(){}

CDrawTextInContextTestPointVertical::~CDrawTextInContextTestPointVertical(){}

CDrawTextInContextTestPointVertical* CDrawTextInContextTestPointVertical::NewL()
	{
	CDrawTextInContextTestPointVertical* self = new(ELeave) CDrawTextInContextTestPointVertical;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CDrawTextInContextTestPointVertical::ConstructL()
	{
	BaseConstructL();
	iPosition = TPoint(0,0);
	iUp = EFalse;
	}

void CDrawTextInContextTestPointVertical::DoDrawTextBitGc()
	{
	iRefBitGc->DrawTextVertical(iText,&iParam,iPosition,iUp);
	}

void CDrawTextInContextTestPointVertical::DoDrawTextRemoteGc()
	{
	iRemoteGc->DrawTextVertical(iText,&iParam,iPosition,iUp);
	}

//
// Class CDrawTextInContextTestBoxVertical
//

CDrawTextInContextTestBoxVertical::CDrawTextInContextTestBoxVertical(){}

CDrawTextInContextTestBoxVertical::~CDrawTextInContextTestBoxVertical(){}

CDrawTextInContextTestBoxVertical* CDrawTextInContextTestBoxVertical::NewL()
	{
	CDrawTextInContextTestBoxVertical* self = new(ELeave) CDrawTextInContextTestBoxVertical;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CDrawTextInContextTestBoxVertical::ConstructL()
	{
	BaseConstructL();
	iClipFillRect = TRect(10,50,640,120);
	iBaselineOffset = 40;
	iUp = EFalse;
	iTTextAlign = CGraphicsContext::ELeft;
	}

void CDrawTextInContextTestBoxVertical::DoDrawTextBitGc()
	{
	iRefBitGc->DrawTextVertical(iText,&iParam,iClipFillRect,iBaselineOffset,iUp,iTTextAlign);
	}

void CDrawTextInContextTestBoxVertical::DoDrawTextRemoteGc()
	{
	iRemoteGc->DrawTextVertical(iText,&iParam,iClipFillRect,iBaselineOffset,iUp,iTTextAlign);
	}
#endif //TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA

void CleanUpFont(TAny* aFont)
	{
	//Will be called in case of a leave to release the font
	CFont* font= static_cast<CFont*>(aFont);
	TheClient->iScreen->ReleaseFont(font);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0437
@SYMPREQ            PREQ1543
@SYMTestCaseDesc    Draw text using CWindowGc and CRemoteGc with both outline and shadow
effect on.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Create a font with both outline and shadow effects. Record the commands 
(like setting colours,drawing text etc) using CRemoteGc and play the recorded commands on a window. Use the same
commands in CWindowGc and draw text on a different window
@SYMTestExpectedResults Text drawn using CWindowGc and CRemoteGc should be same
*/
void CTGc::TestOutlineAndShadowL()
	{
	TRect sourceRect(0, 0, TestWin->Size().iWidth, TestWin->Size().iHeight);
	TRegionFix<1> clippingRegion(sourceRect);
	
	CWsScreenDevice* device = TheClient->iScreen;

	_LIT(KText,"Outline and shadow");
	TFontSpec fSpec(KTestFontTypefaceName,23);
	fSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	fSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, ETrue);
	fSpec.iFontStyle.SetEffects(FontEffect::EOutline, ETrue);
	
	CFont *font;
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInPixels((CFont *&)font, fSpec));
	CleanupStack::PushL(TCleanupItem(CleanUpFont, font));
	
	CRemoteGc* remoteGc = CRemoteGc::NewL(device);
	CleanupStack::PushL(remoteGc);
	remoteGc->BeginDraw(sourceRect);
	//Capturing the commands in remote gc
	remoteGc->SetBrushColor(KRgbGreen);
	remoteGc->SetShadowColor(KRgbDarkRed);
	remoteGc->SetPenColor(KRgbBlack);
	remoteGc->UseFont(font);
	remoteGc->DrawText(KText, TPoint(2,40));
	remoteGc->DiscardFont();
	remoteGc->EndDraw();

	RWsGraphicMsgBuf msgBuf;
	CleanupClosePushL(msgBuf);
	//Externalize the captured commands from remote gc in to a buffer
	remoteGc->ExternalizeL(msgBuf, ETrue);

	CWSGraphicsRes* wsGrap = new (ELeave) CWSGraphicsRes();
	CleanupStack::PushL(wsGrap);
	
	CCommandBuffer* cmdBuf = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuf);
	//Internalize the buffer with captured commands (from CRemoteGC) 
	//in to CCommandBuffer
	cmdBuf->InternalizeL(msgBuf.Pckg());
	
	TestWin->Win()->Invalidate();
	TestWin->Win()->BeginRedraw();
	TheGc->Activate(*TestWin->Win());
	TheGc->Clear();
	//Play the commands on test window using command buffer
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	cmdBuf->Play(TPoint(0, 0), &clippingRegion, sourceRect, TheClient->iWs, *TheGc);
#else
	cmdBuf->Play(TPoint(),TRect(TestWin->Size()),*wsGrap,*TheGc);
#endif
	TheGc->Deactivate();
	TestWin->Win()->EndRedraw();
	
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());
	TheGc->Clear();
	TheGc->SetBrushColor(KRgbGreen);
	TheGc->SetShadowColor(KRgbDarkRed);
	TheGc->SetPenColor(KRgbBlack);
	TheGc->UseFont(font);
	//Draw the text on base window using CWindowGC
	TheGc->DrawText(KText, TPoint(2, 40));
	TheGc->DiscardFont();
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//Check the text drawn on base and test windows.
	CheckRect(BaseWin, TestWin, TRect(0, 0, BaseWin->Size().iWidth, BaseWin->Size().iHeight), _L("CTGc::TestOutlineAndShadowL()"));

	CleanupStack::PopAndDestroy(4, remoteGc); //cmdBuf, wsGrap, msgBuf and remoteGc
	CleanupStack::Pop();//font
	TheClient->iScreen->ReleaseFont(font);
	}

void CTGc::TestGcClipRectOrigin_DrawContent(TestWindow& aWindow, TBool bActivateBeforeRedraw /*= ETrue*/)
	{
	TSize winSize = aWindow.Size();
	TPoint gcOrigin(winSize.iWidth >> 3, winSize.iWidth >> 3);
	TRect gcClipRect(0, 0, (winSize.iWidth * 3) >> 2, (winSize.iHeight * 3) >> 2);
	TRect ellipseRect(gcClipRect);
	// Shrink the ellipse for better visibility and to fit well within the clip area.
	ellipseRect.Shrink(3, 3);
	TSize penSize(1, 1);

	aWindow.Win()->SetBackgroundColor(KRgbGreen);
	aWindow.ClearWin();
	aWindow.Win()->Invalidate();

	if(!bActivateBeforeRedraw)
		{
		aWindow.Win()->BeginRedraw();
		}

	TheGc->Activate(*(aWindow.Win()));
	TheGc->SetOrigin(gcOrigin);
	TheGc->SetClippingRect(gcClipRect);

	if(bActivateBeforeRedraw)
		{
		aWindow.Win()->BeginRedraw();
		}

	TheGc->SetBrushColor(KRgbDarkRed);
	TheGc->SetPenColor(KRgbDarkRed);
	TheGc->SetPenSize(penSize);
	TheGc->SetPenStyle(CGraphicsContext::ESolidPen);
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->DrawEllipse(ellipseRect);
	TheGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	TheGc->SetPenColor(KRgbYellow);
	TheGc->SetPenStyle(CGraphicsContext::EDashedPen);
	TheGc->DrawRect(gcClipRect);

	aWindow.Win()->EndRedraw();
	TheGc->Deactivate();
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0471
@SYMTestCaseDesc    This test is to verify that the GC correctly applies the clip rect and
					origin attributes irrespective of whether the GC is activated on the
					window before or after the BeginRedraw.
@SYMDEF				PDEF120091
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     The test has following steps:
	1. For the Test window follow the steps:
		A. Activate the GC on Test window.
		B. Set the Origin of the GC to centre of the window.
		C. Set the Clipping rectangle of the GC to half the size of the window.
		D. In the BeginDraw and EndDraw call bracket perform the following:
			a. Draw an ellipse with the rectangle smaller by 5 pixels than the clip rectangle.
			b. Draw a rectangle that is size of the clip rectangle.
		E. Deactivate the GC. 
	2. For the Base window follow the steps:
		A. In the BeginDraw and EndDraw call bracket perform the following:
			a. Activate the GC on Base window.
			b. Set the Origin of the GC to centre of the window.
			c. Set the Clipping rectangle of the GC to half the size of the window.
			d. Draw an ellipse with the rectangle smaller by 5 pixels than the clip rectangle.
			e. Draw a rectangle that is size of the clip rectangle.
			f. Deactivate the GC. 
	3. Compare Test and Base window.
@SYMTestExpectedResults Both the Test and Base window should have the complete
						non-clipped ellipse completely encapsulated within the rectangle. 
*/
void CTGc::TestGcClipRectOrigin()
	{
	TestGcClipRectOrigin_DrawContent(*BaseWin, EFalse);
	TestGcClipRectOrigin_DrawContent(*TestWin, ETrue);
	
	TheClient->Flush();
	
	CheckRect(BaseWin, TestWin, TRect(0, 0, BaseWin->Size().iWidth, BaseWin->Size().iHeight), _L("CTGc::TestGcClipRectOriginL()"));
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0469
@SYMDEF             INC116406
@SYMTestCaseDesc    Try playback on MWsGraphicsContext and CWindowGc to check that the background colour is
set correctly.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions
@SYMTestExpectedResults Text drawn using CWindowGc should use the background colour of
the window, and MWsGraphicsContext should use transparent white.
*/
void CTGc::TestResetWithBackgroundColorL()
	{
	const TRect KSourceRect(0, 0, TestWin->Size().iWidth, TestWin->Size().iHeight);
	const TRegionFix<1> KClippingRegion(KSourceRect);
	
	CWsScreenDevice* device = TheClient->iScreen;

	CRemoteGc* remoteGc = CRemoteGc::NewL(device);
	CleanupStack::PushL(remoteGc);
	
	//note this remote GC has not been activated on any window
	remoteGc->BeginDraw(KSourceRect);

	//Draw the commands in remote gc
	remoteGc->SetBrushColor(KRgbGreen);  //nothing green is seen in this test

	remoteGc->Reset(); //This resets the brush colour to the background colour of the window
					   //where playback is, in the case of playing back to a window
					   //however with a CFbsBitGc the color is transparent white, as there is no window.
	remoteGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	remoteGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
	remoteGc->DrawRect(KSourceRect);
	remoteGc->EndDraw();

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	CDirectGdiDriver* theDGdiDriver = CDirectGdiDriver::Static();
	User::LeaveIfNull(theDGdiDriver);
	
	TSgImageInfo info;
	info.iUsage = ESgUsageDirectGdiTarget | ESgUsageCompositionSource;
	info.iSizeInPixels = TSize(TestWin->Size().iWidth, TestWin->Size().iHeight);
	info.iPixelFormat = EUidPixelFormatXRGB_8888;//among display modes with alpha channel only pre-multiply alpha is supported in directGDI currently
	
	RSgImageCollection imageCollection;
	CleanupClosePushL(imageCollection);
	TInt res = imageCollection.Create(info, 1);
	User::LeaveIfError(res);
	RSgImage image;
	CleanupClosePushL(image);
	res = imageCollection.OpenImage(0, image);
	User::LeaveIfError(res);
	RDirectGdiImageTarget imageTarget(*theDGdiDriver);
	CleanupClosePushL(imageTarget);
	res = imageTarget.Create(image);
	User::LeaveIfError(res);
#endif
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	//create a bitmap
	CFbsBitmap *bitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(bitmap->Create(TSize(TestWin->Size().iWidth, TestWin->Size().iHeight), EColor16MA));
	CleanupStack::PushL(bitmap);

	CFbsBitmapDevice  *fbsDevice = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(fbsDevice);
#endif

	//prepare the command buffer for playback
	RWsGraphicMsgBuf msgBuf;
	CleanupClosePushL(msgBuf);

	//Externalize the captured commands from remote gc in to a buffer
	remoteGc->ExternalizeL(msgBuf, ETrue);

	CWSGraphicsRes* wsGrap = new (ELeave) CWSGraphicsRes();
	CleanupStack::PushL(wsGrap);

	CCommandBuffer* cmdBuf = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuf);
	cmdBuf->InternalizeL(msgBuf.Pckg());

	TRgb color;
	TRgb testColor(KRgbWhite);
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	CDirectGdiGcWrapper* directGdiGcWrapper=CDirectGdiGcWrapper::NewL(imageTarget);
	CleanupStack::PushL(directGdiGcWrapper);
	cmdBuf->Play(TPoint(),&KClippingRegion,KSourceRect,*wsGrap,*directGdiGcWrapper);

	//check that the background has been cleared to transparent white.
	image.GetInfo(info);
	info.iUsage = ESgUsageNone;
	info.iCpuAccess = ESgCpuAccessReadOnly;
	RSgImage image1;
	CleanupClosePushL(image1);
	res = image1.Create(info, image);
	const TAny* data;
	TInt stride = 0;
	res = image1.MapReadOnly(data, stride);
	User::LeaveIfError(res);
	TPoint pixel(10,10);
	TInt offset = pixel.iY * stride + pixel.iX * 4;
	TAny* non_const_data = const_cast <TAny*> (data);
	TUint8* pointData = static_cast <TUint8*> (non_const_data) + offset;
	color = *(reinterpret_cast <TRgb*> (pointData));
	image1.Unmap();
#endif
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	CFbsBitGc* fbsBitGc=NULL;
	User::LeaveIfError(fbsDevice->CreateContext(fbsBitGc));
	CleanupStack::PushL(fbsBitGc);
	fbsBitGc->Activate(fbsDevice);
	cmdBuf->Play(TPoint(0, 0), KSourceRect, *wsGrap, *fbsBitGc);
	bitmap->GetPixel(color, TPoint(10,10));
	testColor.SetAlpha(0);
#endif
	iStep->TEST(color==testColor);

	//now test drawing to a window to ensure that the brush colour is
	//the window background colour

	//display a blue window
	BaseWin->Win()->SetBackgroundColor(KRgbBlue);
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());
	TheGc->Clear();
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//start drawing the display commands with a green background
	
	BaseWin->Win()->SetBackgroundColor(KRgbYellow);
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());

	//Play the commands on test window using command buffer
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	cmdBuf->Play(TPoint(0, 0), &KClippingRegion, KSourceRect, TheClient->iWs, *TheGc);
#else
	cmdBuf->Play(TPoint(0, 0), KSourceRect, *wsGrap, *TheGc);
#endif

	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//check that the background has been cleared to yellow, using brush colour
	TPoint position = BaseWin->Win()->InquireOffset(*TheClient->iGroup->WinTreeNode());
	position.iX+=10;
	position.iY+=10;
	TheClient->iScreen->GetPixel(color, position);
	iStep->TEST(color==KRgbYellow);

	BaseWin->Win()->SetBackgroundColor(KRgbGreen); //set back to original backgroundcolor

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	CleanupStack::PopAndDestroy(9, remoteGc);
#else
	CleanupStack::PopAndDestroy(7, remoteGc);
#endif
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0481
@SYMPREQ            1841
@SYMTestCaseDesc    Create font and graphics with various effect effects. Record the commands 
(like setting colours,drawing text etc) using CRemoteGc and play the recorded commands on a window. Use the same
commands in CWindowGc and draw text on a different window
@SYMTestPriority    Medium
@SYMTestStatus      Implemented
@SYMTestActions
@SYMTestExpectedResults Text/graphics drawn using CWindowGc and CRemoteGc should be same
*/
void CTGc::TestCommandBufferL()
	{	
	CWsScreenDevice* device = TheClient->iScreen;

	_LIT(KBuffText,"Command Buffer");
	TFontSpec fSpec(KTestFontTypefaceName,23);
	fSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	fSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, ETrue);
	fSpec.iFontStyle.SetEffects(FontEffect::EOutline, ETrue);
	
	CFont *font;
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInPixels((CFont *&)font, fSpec));
	CleanupStack::PushL(TCleanupItem(CleanUpFont, font));
	
	CRemoteGc* remoteGc = CRemoteGc::NewL(device);
	CleanupStack::PushL(remoteGc);	

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	//-------create image---------
	CDirectGdiDriver* theDGdiDriver = CDirectGdiDriver::Static();
	User::LeaveIfNull(theDGdiDriver);
	const TSize KImageSize = TSize(2, 2);
	TSgImageInfo info;
	info.iUsage = ESgUsageWindowGcSource;
	info.iSizeInPixels = KImageSize;
	info.iPixelFormat = EUidPixelFormatRGB_565;
	info.iShareable = ETrue;
	const TInt stride = KImageSize.iWidth * 2;
	TUint8* buf = (TUint8*) (User::AllocL(KImageSize.iHeight * stride));
	CleanupStack::PushL(buf);
	TUint16* bufCur = ((TUint16*)buf);
	*bufCur = KRgbRed.Color64K();
	*(bufCur + 1) = KRgbRed.Color64K();
	*(bufCur + 2) = KRgbRed.Color64K();
	*(bufCur + 3) = KRgbRed.Color64K();

	RSgImage image;
	TInt res = image.Create(info, buf, stride);
	User::LeaveIfError(res);
	CleanupClosePushL(image);	
	RWsDrawableSource drawableSource(TheClient->iWs);
	res = drawableSource.Create(image, TheClient->iScreen->GetScreenNumber());
	if(res == KErrNotSupported)
		{
		INFO_PRINTF1(_L("The current screen is not supports drawable source. This test case terminates now."));
		CleanupStack::PopAndDestroy(3, remoteGc);
		CleanupStack::Pop();//font
		TheClient->iScreen->ReleaseFont(font);
		return;
		}
	User::LeaveIfError(res);
	CleanupClosePushL(drawableSource);	
	//-------end create image---------
#endif	
	remoteGc->ResetCommandBuffer();
	remoteGc->BeginDraw(TRect(0, 0, TestWin->Size().iWidth, TestWin->Size().iHeight));
	//Capturing the commands in remote gc
	remoteGc->Clear();
	remoteGc->DrawRect(TRect(10,10,30,30));
	remoteGc->Clear(TRect(10,10,11,11));
	remoteGc->CopyRect(TPoint(5,5), TRect(25,25,30,30));
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CFbsBitmap* bitmapMask = new (ELeave) CFbsBitmap;
	User::LeaveIfError(bitmap->Load(_L("Z:\\WSTEST\\WSAUTOTEST.MBM"), EMbmWsautotestCircles24b));
	User::LeaveIfError(bitmapMask->Load(_L("Z:\\WSTEST\\WSAUTOTEST.MBM"), EMbmWsautotestCircles_mask2b));
	remoteGc->BitBlt(TPoint(100,100), bitmap);
	remoteGc->BitBlt(TPoint(0,0), bitmap, TRect(0,0,1,1));	
	remoteGc->BitBltMasked(TPoint(0,5), bitmap, TRect(0,0,1,1), bitmapMask, EFalse);
	CWsBitmap* bitmapWs = new (ELeave) CWsBitmap(TheClient->iWs);
	CWsBitmap* bitmapWsMask = new (ELeave) CWsBitmap(TheClient->iWs);
	User::LeaveIfError(bitmapWs->Load(_L("Z:\\WSTEST\\TEST.MBM"), 0));
	remoteGc->BitBlt(TPoint(110,110), bitmapWs);
	remoteGc->BitBlt(TPoint(5,0), bitmapWs, TRect(0,0,1,1));
	remoteGc->BitBltMasked(TPoint(10,0), bitmap, TRect(0,0,1,1), bitmapWsMask, EFalse);
	remoteGc->SetFadingParameters(128,128);
	remoteGc->SetFaded(EFalse);	
	remoteGc->AlphaBlendBitmaps(TPoint(2,2), bitmap, TRect(0,0,1,1), bitmapMask, TPoint(2,2));
	remoteGc->AlphaBlendBitmaps(TPoint(3,3), bitmapWs, TRect(0,0,1,1), bitmapWsMask, TPoint(2,2));
	remoteGc->SetOrigin(TPoint(0,30));
	remoteGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	remoteGc->SetClippingRect(TRect(0,0,10,10));
	remoteGc->SetPenStyle(CGraphicsContext::ESolidPen);
	remoteGc->SetPenSize(TSize(1,2));
	remoteGc->UseBrushPattern(bitmap);
	remoteGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	remoteGc->SetBrushOrigin(TPoint(0,0));
	remoteGc->DrawPie(TRect(0,0,15,15),TPoint(0,8),TPoint(15,8));
	remoteGc->CancelClippingRect();
	remoteGc->DiscardBrushPattern();
	remoteGc->CancelClippingRegion();
	remoteGc->Reset();
	remoteGc->SetOrigin(TPoint(0,0));
	remoteGc->SetUnderlineStyle(EUnderlineOff);
	remoteGc->SetStrikethroughStyle(EStrikethroughOff);
	remoteGc->SetWordJustification(1,2);
	remoteGc->SetCharJustification(1,2);
	remoteGc->UseFont(font);
	remoteGc->DrawText(KBuffText,TRect(50,0,100,50),10,CGraphicsContext::ELeft,0);
	remoteGc->DrawTextVertical(KBuffText,TPoint(170,20),EFalse);
	remoteGc->DrawTextVertical(KBuffText,TRect(120,20,150,100),5,EFalse,CGraphicsContext::ELeft,0);	
	remoteGc->MoveTo(TPoint(25,150));
	remoteGc->MoveBy(TPoint(5,5));
	remoteGc->DrawLineTo(TPoint(35,160));
	remoteGc->DrawLine(TPoint(35,160),TPoint(25,150));
	remoteGc->DrawLineBy(TPoint(15,6));	
	remoteGc->Plot(TPoint(5,5));	
	remoteGc->DrawArc(TRect(0,80,10,90),TPoint(0,85),TPoint(10,85));
	remoteGc->DrawEllipse(TRect(0,90,10,100));	
	remoteGc->DrawRoundRect(TRect(30,80,50,100),TSize(5,5));	
	remoteGc->DrawBitmap(TPoint(150,150),bitmap);	
	remoteGc->DrawBitmap(TRect(160,160,170,170), bitmap);	
	remoteGc->DrawBitmap(TRect(175,175,180,180), bitmap, TRect(0,5,5,10));	
	remoteGc->DrawBitmapMasked(TRect(185,185,190,190), bitmap, TRect(0,50,5,55),bitmapMask,EFalse);
	remoteGc->DrawBitmapMasked(TRect(195,195,200,200), bitmapWs, TRect(0,50,5,55),bitmapWsMask,EFalse);		
	CArrayFixFlat<TPoint>* polyPoints = new(ELeave) CArrayFixFlat<TPoint>(3); //CArrayFixFlat
	CleanupStack::PushL(polyPoints);
	TRect rect (200,0,200,100);
	polyPoints->AppendL(rect.iTl);
	polyPoints->AppendL(rect.Center());
	polyPoints->AppendL(TPoint(rect.iBr.iX, rect.iTl.iY));	
	remoteGc->DrawPolyLine(polyPoints);
	remoteGc->DrawPolyLine(&polyPoints->At(0), 3);
	remoteGc->DrawPolygon(polyPoints, CGraphicsContext::EWinding);
	remoteGc->DrawPolygon(&polyPoints->At(0), 3, CGraphicsContext::EAlternate);
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	MWsDrawResource* dr = static_cast<MWsDrawResource*>(remoteGc->Interface(KMWsDrawResourceInterfaceUid));
	if(dr)
		dr->DrawResource(TPoint(30, 40), drawableSource);
#endif
	RRegion region;
	remoteGc->SetClippingRegion(region);
	remoteGc->DiscardFont();
	remoteGc->EndDraw();

	RWsGraphicMsgBuf msgBuf;
	CleanupClosePushL(msgBuf);
	//Externalize the captured commands from remote gc in to a buffer
	remoteGc->ExternalizeL(msgBuf, EFalse);
	
	CCommandBuffer* cmdBuf = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuf);
	const CCommandBuffer* testCmdBuf = CCommandBuffer::NewL();
	if(cmdBuf->IsIdentical(*testCmdBuf)==EFalse)
		{
		User::Panic(_L("TestCommandBufferL"), KErrGeneral);
		}
	delete testCmdBuf;
	//Internalize the buffer with captured commands (from CRemoteGC) 
	//in to CCommandBuffer
	cmdBuf->InternalizeL(msgBuf.Pckg());
	
	TheGc->Activate(*TestWin->Win());
	TestWin->Win()->Invalidate();
	
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	/*
	 * Make sure anything that can leave is done outside the
	 * BeginRedraw/EndRedraw bracket.
	 */
	CWSGraphicsRes* wsGrap=new(ELeave) CWSGraphicsRes();
	CleanupStack::PushL(wsGrap);
#endif
	/*
	 * Note we need to still do BeginRedraw/EndRedraw for the TestWin Window
	 * even though the CRemoteGc we are going to Play into TestWin already has
	 * BeginRedraw/EndRedraw commands issued into it.  Those commands just allow
	 * for replacement of draw ops already in the CRemoteGc to be replaced by
	 * new draw ops covering the same area.  The BeginRedraw/EndRedraws never 
	 * get Play()'ed into TestWin.
	 */
	TestWin->Win()->BeginRedraw();
	TheGc->Clear();
	//Play the commands on test window using command buffer
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	cmdBuf->Play(TPoint(), NULL, TRect(TestWin->Size()), TheClient->iWs, *TheGc);
#endif
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	cmdBuf->Play(TPoint(),TRect(TestWin->Size()),*wsGrap,*TheGc);
	CleanupStack::PopAndDestroy(wsGrap);
#endif
	TheGc->Deactivate();
	TestWin->Win()->EndRedraw();
	remoteGc->ResetCommandBuffer();
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());
	TheGc->Clear();
	TheGc->DrawRect(TRect(10,10,30,30));
	TheGc->Clear(TRect(10,10,11,11));
	TheGc->CopyRect(TPoint(5,5), TRect(25,25,30,30));
	TheGc->BitBlt(TPoint(100,100), bitmap);
	TheGc->BitBlt(TPoint(0,0), bitmap, TRect(0,0,1,1));	
	TheGc->BitBltMasked(TPoint(0,5), bitmap, TRect(0,0,1,1), bitmapMask, EFalse);
	TheGc->BitBlt(TPoint(110,110), bitmapWs);
	TheGc->BitBlt(TPoint(5,0), bitmapWs, TRect(0,0,1,1));
	TheGc->BitBltMasked(TPoint(10,0), bitmap, TRect(0,0,1,1), bitmapWsMask, EFalse);
	TheGc->SetFadingParameters(128,128);
	TheGc->SetFaded(EFalse);	
	TheGc->AlphaBlendBitmaps(TPoint(2,2), bitmap, TRect(0,0,1,1), bitmapMask, TPoint(2,2));
	TheGc->AlphaBlendBitmaps(TPoint(3,3), bitmapWs, TRect(0,0,1,1), bitmapWsMask, TPoint(2,2));
	TheGc->SetOrigin(TPoint(0,30));
	TheGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
	TheGc->SetClippingRect(TRect(0,0,10,10));
	TheGc->SetPenStyle(CGraphicsContext::ESolidPen);
	TheGc->SetPenSize(TSize(1,2));
	TheGc->UseBrushPattern(bitmap);
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetBrushOrigin(TPoint(0,0));
	TheGc->DrawPie(TRect(0,0,15,15),TPoint(0,8),TPoint(15,8));
	TheGc->CancelClippingRect();
	TheGc->DiscardBrushPattern();
	TheGc->CancelClippingRegion();
	TheGc->Reset();
	TheGc->SetOrigin(TPoint(0,0));
	TheGc->SetUnderlineStyle(EUnderlineOff);
	TheGc->SetStrikethroughStyle(EStrikethroughOff);
	TheGc->SetWordJustification(1,2);
	TheGc->SetCharJustification(1,2);
	TheGc->UseFont(font);
	TheGc->DrawText(KBuffText,TRect(50,0,100,50),10,CGraphicsContext::ELeft,0);
	TheGc->DrawTextVertical(KBuffText,TPoint(170,20),EFalse);
	TheGc->DrawTextVertical(KBuffText,TRect(120,20,150,100),5,EFalse,CGraphicsContext::ELeft,0);	
	TheGc->MoveTo(TPoint(25,150));
	TheGc->MoveBy(TPoint(5,5));
	TheGc->DrawLineTo(TPoint(35,160));
	TheGc->DrawLine(TPoint(35,160),TPoint(25,150));
	TheGc->DrawLineBy(TPoint(15,6));	
	TheGc->Plot(TPoint(5,5));	
	TheGc->DrawArc(TRect(0,80,10,90),TPoint(0,85),TPoint(10,85));
	TheGc->DrawEllipse(TRect(0,90,10,100));	
	TheGc->DrawRoundRect(TRect(30,80,50,100),TSize(5,5));	
	TheGc->DrawBitmap(TPoint(150,150),bitmap);	
	TheGc->DrawBitmap(TRect(160,160,170,170), bitmap);	
	TheGc->DrawBitmap(TRect(175,175,180,180), bitmap, TRect(0,5,5,10));	
	TheGc->DrawBitmapMasked(TRect(185,185,190,190), bitmap, TRect(0,50,5,55),bitmapMask,EFalse);
	TheGc->DrawBitmapMasked(TRect(195,195,200,200), bitmapWs, TRect(0,50,5,55),bitmapWsMask,EFalse);
	TheGc->DrawPolyLine(polyPoints);
	TheGc->DrawPolyLine(&polyPoints->At(0), 3);
	TheGc->DrawPolygon(polyPoints, CGraphicsContext::EWinding);
	TheGc->DrawPolygon(&polyPoints->At(0), 3, CGraphicsContext::EAlternate);
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetBrushColor(KRgbRed);
	TheGc->SetPenColor(KRgbRed);
	TheGc->DrawRect(TRect(30, 40, 32, 42));
#endif
	TheGc->DiscardFont();
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheClient->Flush();
	TheClient->WaitForRedrawsToFinish();

	//Check the text drawn on base and test windows.
	TBool err = CheckRect(BaseWin, TestWin, TRect(0, 0, BaseWin->Size().iWidth, BaseWin->Size().iHeight), _L("CTGc::TestCommandBufferL()"));
	if (err)
	    {
	    INFO_PRINTF1(_L("The CheckRect function returned error."));
	    }
	delete bitmap;
	INFO_PRINTF1(_L("bitmap deleted."));
	delete bitmapMask;
	INFO_PRINTF1(_L("bitmapMask deleted."));
	delete bitmapWs;
	INFO_PRINTF1(_L("bitmapWs deleted."));
	delete bitmapWsMask;
	INFO_PRINTF1(_L("bitmapWsMask deleted."));
	
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	CleanupStack::PopAndDestroy(7, remoteGc);
#else
	CleanupStack::PopAndDestroy(4, remoteGc);
#endif	
	CleanupStack::Pop();//font
	INFO_PRINTF1(_L("CleanupStack popped."));
	TheClient->iScreen->ReleaseFont(font);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0482
@SYMPREQ            1841
@SYMTestCaseDesc    Play empty command buffer.
@SYMTestPriority    Medium
@SYMTestStatus      Implemented
@SYMTestActions
@SYMTestExpectedResults return KErrEof error
*/
void CTGc::TestEmptyCommandBufferL()
	{
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	CWSGraphicsRes* wsGrap = new (ELeave) CWSGraphicsRes();
	CleanupStack::PushL(wsGrap);
#endif

	CCommandBuffer* cmdBuf = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuf);

	TheGc->Activate(*TestWin->Win());
	TheGc->Clear();
	//Play the commands on test window using command buffer
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	TInt err = cmdBuf->Play(TPoint(),TRect(TestWin->Size()),*wsGrap,*TheGc);
#else
	TInt err = cmdBuf->Play(TPoint(),NULL,TRect(TestWin->Size()),TheClient->iWs,*TheGc);
#endif
	if(err!=KErrEof)
		{
		User::Panic(_L("TestEmptyCommandBufferL"), KErrGeneral);
		}
	TheGc->Deactivate();
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NONNGA
	CleanupStack::PopAndDestroy(2, wsGrap); //cmdBuf, wsGrap, msgBuf and remoteGc
#else
	CleanupStack::PopAndDestroy(cmdBuf);
#endif
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
/**
@SYMTestCaseID		GRAPHICS-WSERV-0486
@SYMPREQ            PREQ2095
@SYMTestCaseDesc    Draw text using CWindowGc and CRemoteGc with both outline and shadow
effect on.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Create a font with both outline and shadow effects, also use ClippingRect 
and ClippingRegion. Record the commands using CRemoteGc and play the recorded commands on a 
bitmap using MWsGraphicsContext. Use the same commands in CWindowGc and draw text on a 
different window
@SYMTestExpectedResults Text drawn using CWindowGc and CRemoteGc(MWsGraphicsContext) should be same
*/
void CTGc::TestCRemoteGcAndMWsGraphicsContextClippingRectL()
	{
	const TRect KTestRect(0, 0, TestWin->Size().iWidth, TestWin->Size().iHeight);
	const TRegionFix<1> KTestRegion(KTestRect);
	const TRect KClippingRect1(5, 5, TestWin->Size().iWidth-10, 90);
	const TRegionFix<1> KClippingRegion(KClippingRect1);
	const TRect KClippingRect2(15, 15, TestWin->Size().iWidth-10, TestWin->Size().iHeight-10);
	
	CWsScreenDevice* device = TheClient->iScreen;
	/*
	 * On hardware, the first screen runs in 64K colors, but the second screen (TV OUT)
	 * cannot run in this mode, it instead falls back to 16M colors.  We need to ensure
	 * that we use matching color depths for our off-screen bitmaps so that accuracy is
	 * not lost since we compare bitmaps from the screen versus off-screen.
	 */
	TDisplayMode displayMode = device->DisplayMode();
	if (TDisplayModeUtils::NumDisplayModeBitsPerPixel(displayMode) == 32)
	    {
	    displayMode = EColor16MAP;
	    }
		
	_LIT(KText,"RemoteGc & MWsGraphicsContext");
	TFontSpec fSpec(KTestFontTypefaceName,23);
	fSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	fSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, ETrue);
	fSpec.iFontStyle.SetEffects(FontEffect::EOutline, ETrue);
	
	CFont *font;
	User::LeaveIfError(TheClient->iScreen->GetNearestFontToDesignHeightInPixels((CFont *&)font, fSpec));
	CleanupStack::PushL(TCleanupItem(CleanUpFont, font));

	//Record the commands using CRemoteGc
	CRemoteGc* remoteGc = CRemoteGc::NewL(device);
	CleanupStack::PushL(remoteGc);
	remoteGc->BeginDraw(KTestRect);
	//fill background with white
	remoteGc->SetPenStyle(CFbsBitGc::ENullPen);
	remoteGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	remoteGc->SetBrushColor(KRgbWhite);
	remoteGc->DrawRect(TRect(TPoint(0,0), TestWin->Size()));
	remoteGc->SetPenStyle(CFbsBitGc::ESolidPen);
	//Capturing the commands in remote gc
	remoteGc->SetClippingRect(KClippingRect2);
	remoteGc->SetClippingRegion(KClippingRegion);
	remoteGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	remoteGc->SetBrushColor(TRgb(0,150,150));
	remoteGc->DrawRect(TRect(TPoint(0,0), TSize(160,60)));
	remoteGc->SetBrushColor(TRgb(150,100,150));
	remoteGc->DrawRect(TRect(TPoint(0,60), TSize(160,60)));
	remoteGc->SetBrushColor(KRgbGreen);
	remoteGc->SetShadowColor(KRgbDarkRed);
	remoteGc->SetPenColor(KRgbBlack);
	remoteGc->UseFont(font);
	remoteGc->DrawText(KText, TPoint(2,40));
	remoteGc->DiscardFont();
	remoteGc->EndDraw();

	RWsGraphicMsgBuf msgBuf;
	CleanupClosePushL(msgBuf);
	//Externalize the captured commands from remote gc in to a buffer
	remoteGc->ExternalizeL(msgBuf, ETrue);
	
	CCommandBuffer* cmdBuf = CCommandBuffer::NewL();
	CleanupStack::PushL(cmdBuf);
	//Internalize the buffer with captured commands (from CRemoteGC) 
	//in to CCommandBuffer
	cmdBuf->InternalizeL(msgBuf.Pckg());
	
	CDirectGdiDriver* theDGdiDriver = CDirectGdiDriver::Static();
	User::LeaveIfNull(theDGdiDriver);
	
	TSgImageInfo info;
	info.iUsage = ESgUsageDirectGdiTarget | ESgUsageDirectGdiSource | ESgUsageCompositionSource;
	info.iSizeInPixels = TestWin->Size();
	info.iPixelFormat = PixelFormatFromDisplayMode(displayMode);
	
	RSgImageCollection imageCollection;
	CleanupClosePushL(imageCollection);
	TInt res = imageCollection.Create(info, 1);
	User::LeaveIfError(res);
	RSgImage image;
	CleanupClosePushL(image);
	res = imageCollection.OpenImage(0, image);
	User::LeaveIfError(res);
	RDirectGdiImageTarget imageTarget(*theDGdiDriver);
	CleanupClosePushL(imageTarget);
	
	res = imageTarget.Create(image);
	User::LeaveIfError(res);
	
	CDirectGdiGcWrapper* directGdiGcWrapper = CDirectGdiGcWrapper::NewL(imageTarget);
	CleanupStack::PushL(directGdiGcWrapper);
	
	//Dummy class created
	CWSGraphicsRes* wsGrap = new (ELeave) CWSGraphicsRes();
	CleanupStack::PushL(wsGrap);

	//Play the commands on test window using command buffer
	cmdBuf->Play(TPoint(),&KTestRegion,KTestRect,*wsGrap,*directGdiGcWrapper);

	//Set window back to same as test bitmap background
	BaseWin->Win()->SetBackgroundColor(KRgbWhite);

	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());
	TheGc->Clear();
	TheGc->SetClippingRect(KClippingRect2);
	TheGc->SetClippingRegion(KClippingRegion);
	TheGc->SetBrushStyle(CFbsBitGc::ESolidBrush);
	TheGc->SetBrushColor(TRgb(0,150,150));
	TheGc->DrawRect(TRect(TPoint(0,0), TSize(160,60)));
	TheGc->SetBrushColor(TRgb(150,100,150));
	TheGc->DrawRect(TRect(TPoint(0,60), TSize(160,60)));
	TheGc->SetBrushColor(KRgbGreen);
	TheGc->SetShadowColor(KRgbDarkRed);
	TheGc->SetPenColor(KRgbBlack);
	TheGc->UseFont(font);
	//Draw the text on base window using CWindowGC
	TheGc->DrawText(KText, TPoint(2, 40));
	TheGc->DiscardFont();
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheClient->iWs.Finish();
	TheClient->WaitForRedrawsToFinish();

	//Create a bitmap and then copy the screen to it
	TRect rc(TRect(BaseWin->Win()->AbsPosition(), BaseWin->Win()->Size()));
	CFbsBitmap *screenBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(screenBitmap->Create(rc.Size(), displayMode));
	CleanupStack::PushL(screenBitmap);
	TheClient->iScreen->CopyScreenToBitmap(screenBitmap, rc);

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);

	image.GetInfo(info);
	bitmap->Create(info.iSizeInPixels, displayMode);
	TRect rect(info.iSizeInPixels);
	CopyImageToBitmapL(bitmap, image, rect);

	//Test to see if the bitmap drawn to using CRemoteGc is the same as the screen copy bitmap
	TInt differentPixels = 0;
	res = LossyCompareBitmapRecord(*bitmap, *screenBitmap, KTestRect, EFalse, differentPixels, Logger());
	if (differentPixels != 0)
		{
		INFO_PRINTF2(_L(" Pixels different %d"), differentPixels);
		}
	TEST(res);
	CleanupStack::PopAndDestroy(10, remoteGc); //screenBitmap, imageCollection, image, imageTarget, directGdiGcWrapper, cmdBuf, wsGrap, msgBuf, remoteGc, bitmap
	CleanupStack::Pop();//font
	TheClient->iScreen->ReleaseFont(font);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0487
@SYMPREQ            PREQ2095
@SYMTestCaseDesc    Draw text using CRemoteGc and DrawText(const TDesC&,const TTextParameters*,const TPoint&)
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Create a font.  Draw text to a bitmap with the font using CFbsBitGc::DrawText. Draw text with the font using CFbsBitGc::DrawText.
Record the same DrawText commands using CRemoteGc and play the recorded commands on a 
bitmap using MWsGraphicsContext. Compare the two bitmaps.
@SYMTestExpectedResults Text drawn using CFbsBitGc and CRemoteGc(MWsGraphicsContext) should be the same
*/
void CTGc::TestCRemoteGcDrawTextInContextPointL()
	{
	CDrawTextInContextTestPoint* test = CDrawTextInContextTestPoint::NewL();
	CleanupStack::PushL(test);
	test->Test();
	TEST(test->HasPassedTest());
	CleanupStack::PopAndDestroy(); //test
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0488
@SYMPREQ            PREQ2095
@SYMTestCaseDesc    Draw text using CRemoteGc and DrawText(const TDesC&,const TTextParameters*,const TRect&,TInt,TTextAlign,TInt)
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Create a font.  Draw text to a bitmap with the font using CFbsBitGc::DrawText. Draw text with the font using CFbsBitGc::DrawText.
Record the same DrawText commands using CRemoteGc and play the recorded commands on a 
bitmap using MWsGraphicsContext. Compare the two bitmaps.
@SYMTestExpectedResults Text drawn using CFbsBitGc and CRemoteGc(MWsGraphicsContext) should be the same
*/
void CTGc::TestCRemoteGcDrawTextInContextBoxL()
	{
	CDrawTextInContextTestBox* test = CDrawTextInContextTestBox::NewL();
	CleanupStack::PushL(test);
	test->Test();
	TEST(test->HasPassedTest());
	CleanupStack::PopAndDestroy(); //test
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0489
@SYMPREQ            PREQ2095
@SYMTestCaseDesc    Draw text using CRemoteGc and DrawTextVertical(const TDesC&,const TTextParameters*,const TPoint&)
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Create a font.  Draw text to a bitmap with the font using CFbsBitGc::DrawTextVertical. Draw text with the font using CFbsBitGc::DrawText.
Record the same DrawText commands using CRemoteGc and play the recorded commands on a 
bitmap using MWsGraphicsContext. Compare the two bitmaps.
@SYMTestExpectedResults Text drawn using CFbsBitGc and CRemoteGc(MWsGraphicsContext) should be the same
*/
void CTGc::TestCRemoteGcDrawTextInContextPointVerticalL()
	{
	CDrawTextInContextTestPointVertical* test = CDrawTextInContextTestPointVertical::NewL();
	CleanupStack::PushL(test);
	test->Test();
	TEST(test->HasPassedTest());
	CleanupStack::PopAndDestroy(); //test
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0490
@SYMPREQ            PREQ2095
@SYMTestCaseDesc    Draw text using CRemoteGc and DrawTextVertical(const TDesC&,const TTextParameters*,const TRect&,TInt,TTextAlign,TInt)
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Create a font.  Draw text to a bitmap with the font using CFbsBitGc::DrawTextVertical. Draw text with the font using CFbsBitGc::DrawText.
Record the same DrawText commands using CRemoteGc and play the recorded commands on a 
bitmap using MWsGraphicsContext. Compare the two bitmaps.
@SYMTestExpectedResults Text drawn using CFbsBitGc and CRemoteGc(MWsGraphicsContext) should be the same
*/
void CTGc::TestCRemoteGcDrawTextInContextBoxVerticalL()
	{
	CDrawTextInContextTestBoxVertical* test = CDrawTextInContextTestBoxVertical::NewL();
	CleanupStack::PushL(test);
	test->Test();
	TEST(test->HasPassedTest());
	CleanupStack::PopAndDestroy(); //test
	}
#endif //TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA

/**
@SYMTestCaseID		GRAPHICS-WSERV-0494
@SYMDEF				DEF131255
@SYMTestCaseDesc    Negative test to show that using SetBrushStyle() will not panic WServ with different
					brush bitmaps.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions		Four seperate panic situations are tested:
					1)
                    Create a regular CFbsBitmap, set as brush pattern, and set brush style to EPatternedBrush.
                    Draw a line to force the playback to occur.
                    Call Finish on the GC.
                    Destroy the brush bitmap.
                    2)
                    Create a regular CFbsBitmap, set as brush pattern, and set brush style to EPatternedBrush.
                    Draw a line to force the playback to occur.
                    Destroy the brush bitmap. 
                    Call Finish on the GC.
                    3+4)
                    Create an extended bitmap, set as the brush pattern, and set the brush style to EPatternedBrush.
                    Draw a line to force the playback to occur.
                    Set the brush bitmap and style again.
                    Call Finish on the GC.
                    Destroy the brush bitmap. 
@SYMTestExpectedResults The calls to SetBrushStyle() should not cause WServ to panic when Finish() is called.
*/
void CTGc::TestGcSetBrushPatternL()
	{
	// Extended bitmap test data.
	const TUint8 KTestData[] = "TEST DATA";
	const TInt KTestDataSize = sizeof(KTestData);
	const TUid KTestExtendedBitmapUid = TUid::Uid(0xFFFFFFFF);
	
	// First try using a regular bitmap as the brush pattern.
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());	
	CFbsBitmap* bitmapRegular = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmapRegular);
	TInt res = bitmapRegular->Create(TSize(10,10), EColor64K);
	TEST(res == KErrNone);
	//Record the commands using CWindowGc.
	TheGc->UseBrushPattern(bitmapRegular);
	TheGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	// DrawLine() is only used here to force playback of the commands.
	TheGc->DrawLine(TPoint(0,0), TPoint(1,1));	
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheClient->iWs.Finish();
	CleanupStack::PopAndDestroy(1, bitmapRegular);
	
	// Secondly, try using a regular bitmap as the brush pattern, but deleting the bitmap
	// before calling Finish().
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());		
	bitmapRegular = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmapRegular);
	res = bitmapRegular->Create(TSize(10,10), EColor64K);		
	TEST(res == KErrNone);
	//Record the commands using CWindowGc.
	TheGc->UseBrushPattern(bitmapRegular);
	TheGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	TheGc->DrawLine(TPoint(0,0), TPoint(1,1));
	CleanupStack::PopAndDestroy(1, bitmapRegular);	
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheClient->iWs.Finish();
	
	// Thirdly, try using an extended bitmap (which is unsupported by DirectGDI) as 
	// the brush pattern.
	BaseWin->Win()->Invalidate();
	BaseWin->Win()->BeginRedraw();
	TheGc->Activate(*BaseWin->Win());
	// Create a dummy extended bitmap to use as a brush bitmap.
	// This is unsupported by the default implementation of DirectGDI.
	CFbsBitmap* bitmapExtended = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmapExtended);
	res = bitmapExtended->CreateExtendedBitmap(TSize(10,10), EColor64K, KTestExtendedBitmapUid, KTestData, KTestDataSize);
	TEST(res == KErrNone);	
	//Record the commands using CWindowGc.
	TheGc->UseBrushPattern(bitmapExtended);
	TheGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	TheGc->DrawLine(TPoint(0,0), TPoint(100,100));
	TheGc->UseBrushPattern(bitmapExtended);
	// Forth, do it twice so that we test the state commands and the drawops commands.
	TheGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
	TheGc->DrawLine(TPoint(0,0), TPoint(100,100));
	TheGc->Deactivate();
	BaseWin->Win()->EndRedraw();
	TheClient->iWs.Finish();
	CleanupStack::PopAndDestroy(1, bitmapExtended);	
	}

/**
@SYMTestCaseID      GRAPHICS-WSERV-0576
@SYMDEF             
@SYMTestCaseDesc    Checks window server is still able to draw a bitmap, even after the client has released its handle to the bitmap.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     - Draw the bitmap to TestWin (keeping the window hidden)
                    - Delete the bitmap
                    - Show TestWin to cause it to be drawn on screen (after the bitmap has been deleted)
                    - Draw the same bitmap (same image, different bitmap object instance) to BaseWin
                    - Compare contents of TestWin with BaseWin
@SYMTestExpectedResults TestWin and BaseWin should both show the bitmap.
*/
void CTGc::TestGcDeleteBitmap1L()
    {
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    TInt ret = bitmap->Load(TEST_BITMAP_NAME,0);
    TEST(ret == KErrNone);

    // send drawing to hidden window
    TestWin->SetVisible(EFalse);
    TestWin->Win()->Invalidate();
    TestWin->Win()->BeginRedraw();
    TheGc->Activate(*TestWin->Win());
    TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    TheGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
    TheGc->SetBrushColor(TRgb(255, 0, 0));
    TheGc->Clear();
    TheGc->BitBlt(TPoint(0,0), bitmap);
    CleanupStack::PopAndDestroy(bitmap); // before the bitmap is actually deleted, WsFbsDestroyCallBack flushes the command buffer to ensure the bitmap is duplicated in the window server thread
    bitmap = NULL;
    TheGc->Deactivate();
    TestWin->Win()->EndRedraw();
    TheClient->iWs.Flush(); // calling Flush rather than Finish, as we don't need to wait for any drawing to happen (as the window is currently hidden)

    // make window visible (forcing it to draw)
    TestWin->SetVisible(ETrue);
    TheClient->iWs.Finish(); // ensure the bitmap has been drawn on test win

    // window server should have duplicated the bitmap when the BitBlt was added to the redraw store, so drawing
    // the window now (by making it visible above) should display the bitmap on screen, even
    // though we've deleted it in this thread

    // now create the bitmap again, and draw it to the base win (for comparison with test win)
    bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    ret = bitmap->Load(TEST_BITMAP_NAME,0);
    TEST(ret == KErrNone);
    BaseWin->SetVisible(ETrue);
    BaseWin->Win()->Invalidate();
    BaseWin->Win()->BeginRedraw();
    TheGc->Activate(*BaseWin->Win());   
    TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    TheGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
    TheGc->SetBrushColor(TRgb(255, 0, 0));
    TheGc->Clear();
    TheGc->BitBlt(TPoint(0,0), bitmap);
    TheGc->Deactivate();
    BaseWin->Win()->EndRedraw();
    TheClient->iWs.Finish(); // ensure the bitmap has been drawn on base win
    
    CleanupStack::PopAndDestroy(bitmap);

    // the test bitmap should be shown in both base win and test win, so we now check that the
    // contents of base win and test win are the same
    CheckRect(BaseWin, TestWin, TRect(0, 0, BaseWin->Size().iWidth, BaseWin->Size().iHeight), _L("CTGc::TestGcDeleteBitmap1L()"));
    }

/**
@SYMTestCaseID      GRAPHICS-WSERV-0577
@SYMDEF             
@SYMTestCaseDesc    Check window server is still able to use a bitmap required by window drawing, even
                    after the client has released its handle to the bitmap. Also check window server 
                    releases the bitmap, when it's no longer used by window drawing.
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     - Clean BaseWin and TestWin from content that has been left over from previous test
                    - Draw test bitmap to TestWin
                    - Delete the bitmap
                    - Using a different bitmap object instance, duplicate bitmap (the bitmap is still used by window drawing)
                    - Delete the bitmap
                    - Draw new content to TestWin, so that previously drawn bitmap is covered
                    - Duplicate bitmap (the bitmap is no longer used by window drawing)
@SYMTestExpectedResults Bitmap duplication succeeds, when the bitmap used by window drawing, whereas
                        bitmap duplication fails, when the bitmap is no longer used by window drawing.
*/
void CTGc::TestGcDeleteBitmap2L()
    {
    //send new drawing to test and base windows, in order to cover 
    //any content has been left on them (through previous test)
    BaseWin->SetVisible(ETrue);
    BaseWin->Win()->Invalidate();
    BaseWin->Win()->BeginRedraw();
    TheGc->Activate(*BaseWin->Win());   
    TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    TheGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
    TheGc->SetBrushColor(TRgb(0, 0, 255));
    TheGc->Clear();
    TheGc->Deactivate();
    BaseWin->Win()->EndRedraw();
    
    TestWin->SetVisible(ETrue);
    TestWin->Win()->Invalidate();
    TestWin->Win()->BeginRedraw();
    TheGc->Activate(*TestWin->Win());   
    TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    TheGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
    TheGc->SetBrushColor(TRgb(0, 0, 255));
    TheGc->Clear();
    TheGc->Deactivate();
    TestWin->Win()->EndRedraw();
    
    TheClient->iWs.Flush(); 
    TheClient->iWs.Finish();
        
    //load test bitmap
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    TInt ret = bitmap->Load(_L("Z:\\WSTEST\\TESTCIRCLES.MBM"),0);
    TEST(ret == KErrNone);
    TInt bitmapHandle = bitmap->Handle();
    
    //send bitmap drawing to test window
    TestWin->Win()->Invalidate();
    TestWin->Win()->BeginRedraw();
    TheGc->Activate(*TestWin->Win());
    TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    TheGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
    TheGc->SetBrushColor(TRgb(0, 255, 0));
    TheGc->Clear();
    TheGc->BitBlt(TPoint(0,0), bitmap);
    TheGc->Deactivate();
    TestWin->Win()->EndRedraw();
    
    CleanupStack::PopAndDestroy(bitmap);

    TheClient->iWs.Flush(); 
    TheClient->iWs.Finish();
    
    //using a new bitmap object instance check that wserv can still duplicate test bitmap (even though
    //the initial bitmap object is deleted) , since there is a window segment using it
    bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    ret = bitmap->Duplicate(bitmapHandle);
    TEST(ret == KErrNone);
    CleanupStack::PopAndDestroy(bitmap);

    //send new drawing to test window, in order to cover the bitmap that was previously drawn
    TestWin->Win()->Invalidate();
    TestWin->Win()->BeginRedraw();
    TheGc->Activate(*TestWin->Win());
    TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
    TheGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
    TheGc->SetBrushColor(TRgb(0, 0, 255));
    TheGc->Clear();
    TheGc->Deactivate();
    TestWin->Win()->EndRedraw();
    
    TheClient->iWs.Flush(); 
    TheClient->iWs.Finish();
    
    //check that wserv can't duplicate test bitmap, since no window segment uses it any more
    bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    ret = bitmap->Duplicate(bitmapHandle);
    TEST(ret != KErrNone);
    CleanupStack::PopAndDestroy(bitmap);
    }

void CTGc::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTGcStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
	case 1:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0437"));
		iTest->LogSubTest(_L("CRemoteGc&CWindowGc, outline and shadow text"));
		TestOutlineAndShadowL();		
		break;	
	case 2:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0471"));
		iTest->LogSubTest(_L("Test GC clip rect and origin attributes."));		 
		TestGcClipRectOrigin();
		break;
	case 3:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0469"));
		iTest->LogSubTest(_L("CRemoteGc&CWindowGc, reset with background colour"));		
		TestResetWithBackgroundColorL();
		break;
	case 4:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0481"));
		iTest->LogSubTest(_L("CRemoteGc&CCommandBuffer, coverage tests"));
		TestCommandBufferL();
		break;			
	case 5:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0482"));
		iTest->LogSubTest(_L("CCommandBuffer, coverage tests"));
		TestEmptyCommandBufferL();
		break;
	case 6:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0494"));
		iTest->LogSubTest(_L("CWindowGc, Brush Pattern test"));
		TestGcSetBrushPatternL();
		break;
    case 7:
        ((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0576"));
        iTest->LogSubTest(_L("CWindowGc, delete bitmap 1"));
        TestGcDeleteBitmap1L();
        break;
    case 8:
        ((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0577"));
        iTest->LogSubTest(_L("CWindowGc, delete bitmap 2"));
        TestGcDeleteBitmap2L();
        break;
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	case 9:
		 ((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0471"));
		 iTest->LogSubTest(_L("Test GC clip rect and origin attributes."));
		 TestGcClipRectOrigin();
		 break;
	case 10:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0469"));
		iTest->LogSubTest(_L("CRemoteGc&CWindowGc, reset with background colour"));
		TestResetWithBackgroundColorL();
		break;
	case 11:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0486"));
		iTest->LogSubTest(_L("CRemoteGc&MWsGraphicsContext, clipping rect test"));
		TestCRemoteGcAndMWsGraphicsContextClippingRectL();
		break;
	case 12:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0487"));
		iTest->LogSubTest(_L("CRemoteGc, DrawTextInContext Position test"));
		TestCRemoteGcDrawTextInContextPointL();
		break;
	case 13:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0488"));
		iTest->LogSubTest(_L("CRemoteGc, DrawTextInContext ClipRect test"));
		TestCRemoteGcDrawTextInContextBoxL();
		break;
	case 14:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0489"));
		iTest->LogSubTest(_L("CRemoteGc, DrawTextInContext Pos Vertical  test"));
		TestCRemoteGcDrawTextInContextPointVerticalL();
		break;
	case 15:
		((CTGcStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0490"));
		iTest->LogSubTest(_L("CRemoteGc, DrawTextInContext ClipRect Vert  test"));
		TestCRemoteGcDrawTextInContextBoxVerticalL();
		break;
#endif //TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
	default:
		((CTGcStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTGcStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTGcStep*)iStep)->RecordTestResultL();
	}

__CONSTRUCT_STEP__(Gc)
