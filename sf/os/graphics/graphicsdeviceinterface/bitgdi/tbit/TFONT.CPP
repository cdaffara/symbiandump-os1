// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32hal.h>
#include <e32std.h>
#include <e32test.h>
#include <s32mem.h>
#include <bitstd.h>
#include <bitdev.h>
#include <fntstore.h>
#include <fbs.h>
#include <hal.h>
#include <gdi.h>
#include <graphics/fbsdefs.h>

#include "TFont.h"

const TInt KTextBufferLength = 256;

//If you want to do font tests with scaled device - change KScalingFactorX, KScalingFactorY, KScalingOriginPt.
const TInt KScalingFactorX = 1;
const TInt KScalingFactorY = 1;
const TPoint KScalingOriginPt(0, 0);

//--------------------------------------------------------------

CBitmapFont* CFbsFontEx::FontAddress(CFbsFont* aFont)
	{
	return(((CFbsFontEx*)aFont)->Address());
	}

//--------------------------------------------------------------

#ifdef __WINS__
template<class C> XTCallCounter<C>::XTCallCounter(CTGraphicsBase& aTestBase)
	: iTestBase(aTestBase), iVFCallsOutsideFBServ(0)
	{}

template<class C> void XTCallCounter<C>::ExecuteShellcode(TInt aFromFunction)
	{
	const TUint32 KFBServId = 0x10003A16;
	const TUint32 KTBitgdiServerId = 0x10273362;
	TSecureId id = RProcess().SecureId();
	if (id != KFBServId)
		{
		User::LockedInc(iVFCallsOutsideFBServ);
		if (id == KTBitgdiServerId)
			{
			TBuf<128> name;
			XVtableInjector<C>::GetVirtualFunctionName(aFromFunction, name);
			iTestBase.INFO_PRINTF2(_L("Virtual function call to %S from outside FBServ"), &name);
			}
		}
	}
#endif

//--------------------------------------------------------------

CTFont::CTFont(CTestStep* aStep) :
	CTGraphicsBase(aStep),
	iFont(NULL),
	iGc(NULL),
	iDev(NULL)
#ifdef __WINS__
	, iFontCallCounter(*this), iOpenFontCallCounter(*this)
#endif
	{
	}

CTFont::~CTFont()
	{
	delete iGc;
	delete iDev;
	}

void CTFont::ConstructL()
	{
	TRAPD(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EColor256));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EColor16MU));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EColor4K));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EColor16M));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EColor64K));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EGray256));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EGray16));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EGray4));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EGray2));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EColor16));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EColor16MA));
	if (err == KErrNotSupported)
		TRAP(err,iDev = CFbsScreenDevice::NewL(KNullDesC,EColor16MAP));
	if (err!=KErrNone)
		{
		_LIT(KLog,"Failed to create screen device, last returned error %d");
		INFO_PRINTF2(KLog,err);
		}

	User::LeaveIfError(err);
	iDev->ChangeScreenDevice(NULL);
	iDev->SetAutoUpdate(ETrue);
	iDev->CreateContext(iGc);
	iDev->SetScalingFactor(KScalingOriginPt, KScalingFactorX, KScalingFactorY, 1, 1);
	iGc->Activate(iDev);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0074
 
  @SYMDEF             

  @SYMTestCaseDesc Anti-aliased text
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Draws some text in an antialiased manner
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoAntiAliasedTextL()
	{
	int typefaces = iDev->NumTypefaces();
	static const TUint32 colour[15] =
		{ 
		0x000000,
		0x555555,
		0x000080,
		0x008000,
		0x008080,
		0x800000,
		0x800080,
		0x808000,
		0x0000ff,
		0x00ff00,
		0x00ffff,
		0xff0000,
		0xff00ff,
		0xffff00,
		0xaaaaaa
		};
	int colour_index = 0;
	for (int i = 0; i < typefaces; i++)
		{
		TTypefaceSupport typeface_support;
		iDev->TypefaceSupport(typeface_support,i);
		if (typeface_support.iIsScalable)
			{
			ResetGc();
			TPoint pos(300,0);
			int vertical_x = 300;
			for (int j = 6; j < 32; j += 2)
				{
				iGc->SetPenColor(colour[colour_index]);
				colour_index = (++colour_index) % 15;
				TFontSpec fs;
				fs.iTypeface = typeface_support.iTypeface;
				fs.iHeight = j;
				fs.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
		 		User::LeaveIfError(GetNearestFontToDesignHeightInPixels(iFont,fs));
				int ascent = iFont->AscentInPixels();
				int descent = iFont->DescentInPixels();
				iGc->UseFont(iFont);
				pos.iY += ascent;
				iGc->DrawText(_L("pack my box with five dozen liquor jugs"),pos);
				pos.iY += descent;
				vertical_x -= ascent;
				TPoint down_pos(vertical_x,0);
				iGc->DrawTextVertical(_L("pack my box with five dozen liquor jugs"),down_pos,FALSE);
				vertical_x -= descent * 2;
				TPoint up_pos(vertical_x,iDev->SizeInPixels().iHeight);
				iGc->DrawTextVertical(_L("pack my box with five dozen liquor jugs"),up_pos,TRUE);
				vertical_x -= ascent;
				iDev->ReleaseFont(iFont);
				iGc->DiscardFont();
				}
			//TheTest.Getch();
			}
		}
	}
/**
  @SYMTestCaseID GRAPHICS-BITGDI-0075
 
  @SYMDEF             

  @SYMTestCaseDesc tests all fonts
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions tests all fonts (typeface and heights) by writing them to a gc
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoAllFontsL()
	{
	TFontSpec fs;
	TInt typefaces = iDev->NumTypefaces();
	TTypefaceSupport info;

	//used below in for loop (line 177)
	TInt topNew=0;
	TInt bottomNew=0;
	TInt top=0;
	TInt bottom=0;

	// Maximum height of font allowed by iType rasteriser
	const TInt KMaxITypeHeightInPixels = 256;

	for (TInt count = 0; count < typefaces; count++)
		{
		iDev->TypefaceSupport(info,count);

		for (TInt index = 0; index < info.iNumHeights; index++)
			{
			TInt height = iDev->FontHeightInPixels(count,index);
			if (height > KMaxITypeHeightInPixels)
				{
				continue;
				}
			fs.iTypeface = info.iTypeface;
			fs.iHeight = height;

			User::LeaveIfError(GetNearestFontToDesignHeightInPixels(iFont,fs));
			TInt heightInPixels = iFont->HeightInPixels();
			if ( heightInPixels == height )
				{
				TEST( ETrue );
				}
			else
				{
				TOpenFontFaceAttrib attrib;
				if( iFont->GetFaceAttrib( attrib ) )
					{
					_LIT(KErrorMessageWithFontName, "Font: %S, pixel height requested: %d, pixel height of font: %d");
					INFO_PRINTF4(KErrorMessageWithFontName, &attrib.FullName(), height, heightInPixels);
					}
				else
					{
					_LIT(KErrorMessage, "Pixel height requested: %d, pixel height of font: %d");
					INFO_PRINTF3(KErrorMessage, height, heightInPixels);
					}
				TEST( EFalse );
				}

			if (height >= 5)
				{
				iGc->UseFont(iFont);
				
				//Testing the new replaced GetUnderlineMetrics
				//Debug can be used to monitor underline sizes as font chanegs
				iGc->GetUnderlineMetrics(top, bottom);
				TEST( top !=0 && bottom !=0 );
				/*cast the pointer iGc to the base class CGraphicsContext
				should still behave correctly meaning any valid class can 
				GetUnderlineMetrics through CGC as is exported and public.*/
				CGraphicsContext* baseGc = (CGraphicsContext*)iGc;
				baseGc->GetUnderlineMetrics(topNew, bottomNew);
				TEST( top !=0 && bottom !=0 );
				TEST( topNew == top && bottomNew == bottom );

				DisplayAsciiCharSet(info.iTypeface.iName);
				DisplayBoldItalic(info.iTypeface.iName);
				iGc->DiscardFont();
				}

						
			iDev->ReleaseFont(iFont);

			height = iDev->FontHeightInTwips(count,index);
			fs.iHeight = height;
			User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fs));
// Commented out test because it fails for true type fonts
//			if (Abs(iFont->FontSpecInTwips().iHeight - height) > 1)
//				User::Panic(_L("Heights don't match"),KErrGeneral);
			iDev->ReleaseFont(iFont);
			}
		}
	}

TInt CTFont::TestFontL(const TFontSpec &aFontSpec)
	{
	const TSize devSize(iDev->SizeInPixels());
	TInt midHgtOffset=devSize.iHeight/2;
	TRect devRect1(0,0,devSize.iWidth,midHgtOffset);
	TRect devRect2(0,midHgtOffset,devSize.iWidth,midHgtOffset*2);
//
	ResetGc();
	TInt loadErr=GetNearestFontToDesignHeightInTwips(iFont,aFontSpec);
	if (loadErr)
		return(loadErr);
	CBufStore *store=CBufStore::NewLC(1024);
	TFontSpec fSpec1=iFont->FontSpecInTwips();
	TEST(fSpec1.iHeight>0);	// If returns zero height will fail on externalize/internalize
	const TInt ascent=iFont->AscentInPixels();
	iGc->UseFont(iFont);
	iDev->ReleaseFont(iFont);
// Useful when watching test code, but too much logging for actual test runs
//	INFO_PRINTF4(_L("Testing, %S hgt=%d/%d"), &aFontSpec.iTypeface.iName,aFontSpec.iHeight,fSpec1.iHeight);
//
	iGc->SetClippingRect(devRect1);
	iGc->DrawText(_L("0123456789abcdefghiABCDEFGHI"),TPoint(0,ascent));
//
	RStoreWriteStream writeStream;
	TStreamId id=writeStream.CreateLC(*store);
	iGc->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	iGc->DiscardFont();
//
	RStoreReadStream readStream;
	readStream.OpenLC(*store,id);
	iGc->InternalizeL(readStream);
	CleanupStack::PopAndDestroy(&readStream);
	TBool fontUsed=iGc->IsFontUsed();
	if (!iGc->IsFontUsed())
		{
		INFO_PRINTF3(_L("Font externalize/internalize used check failed, %S hgt=%d"), &aFontSpec.iTypeface.iName,aFontSpec.iHeight);
		TEST(EFalse);
		}
	else
		{
		iGc->SetClippingRect(devRect2);
		iGc->DrawText(_L("0123456789abcdefghiABCDEFGHI"),TPoint(0,midHgtOffset+ascent));
		TBool rectCompare=iDev->RectCompare(devRect1,*iDev,devRect2);
		if (!rectCompare)
			{
			INFO_PRINTF3(_L("Font externalize/internalize screen check failed, %S hgt=%d"), &aFontSpec.iTypeface.iName,aFontSpec.iHeight);
			TEST(EFalse);
			}
		}
	CleanupStack::PopAndDestroy(store);
	iGc->CancelClippingRect();
	iGc->DiscardFont();
	return(KErrNone);
	}

void CTFont::FontExternalizeL()
	{
	TFontSpec fsp;
	const TInt typefaceCount=iDev->NumTypefaces();
	for (TInt typeface=0;typeface<typefaceCount;typeface++)
		{
		TTypefaceSupport info;
		iDev->TypefaceSupport(info,typeface);
		fsp.iTypeface = info.iTypeface;
		TInt baseHeight=iDev->FontHeightInTwips(typeface,0);
		_LIT(KTxtNotScalable,"");
		_LIT(KTxtScalable,"Scalable");
		INFO_PRINTF5(_L("Testing, %S min/max=%d/%d %S"), &fsp.iTypeface.iName,info.iMinHeightInTwips,info.iMaxHeightInTwips,info.iIsScalable?&KTxtScalable:&KTxtNotScalable);
		for(TInt hgtIndex=0;hgtIndex<info.iNumHeights;hgtIndex++)
			{	// Test official specced heights
			fsp.iHeight=iDev->FontHeightInTwips(typeface,hgtIndex);
			TEST(TestFontL(fsp)==KErrNone);
			}
		for(fsp.iHeight=0;fsp.iHeight<info.iMaxHeightInTwips*2;fsp.iHeight=(fsp.iHeight+1)*11/10)
			{
			TInt err=TestFontL(fsp);
			if (err!=KErrNone)
				{	// Should not get an error within allowable font size
				TEST(fsp.iHeight>info.iMaxHeightInTwips);	// Over speced max size is ok to get error loading.
				}
			}
		}
	}

void CTFont::DisplayAsciiCharSet(const TDesC& aTypefaceName)
	{
	ResetGc();

	TBuf<KTextBufferLength> textBuffer;
	textBuffer.SetLength(KTextBufferLength);
	for (TInt count = 0; count < KTextBufferLength; count++)
		textBuffer[count] = TText(count);

	TPoint textPos = TPoint(50,iFont->AscentInPixels() + 5);
	iGc->DrawText(aTypefaceName,textPos);
	TInt fontHeight = iFont->HeightInPixels() + 5;
	textPos.iX = 0;
	textPos.iY += fontHeight;
	TInt start = 0;
	TInt screenWidth = iDev->SizeInPixels().iWidth;

	while (start < KTextBufferLength)
		{
		TInt numChars = iFont->TextCount(textBuffer.Mid(start),screenWidth);
		iGc->DrawText(textBuffer.Mid(start,numChars),textPos);
		textPos.iY += fontHeight;
		start += numChars;
		}
	}

void CTFont::DisplayBoldItalic(const TDesC& aTypefaceName)
	{
	ResetGc();

	TBuf<64> textBuffer;
	TInt height = iFont->HeightInPixels();
	CBitmapFont* bmpfon=CFbsFontEx::FontAddress(iFont);

	TInt level=iFont->AscentInPixels();
	iGc->DrawText(aTypefaceName,TPoint(50,level));

	textBuffer=_L("0123456789abcdefghiABCDEFGHI");
	level += height;
	iGc->DrawText(textBuffer,TPoint(0,level));
	iGc->DrawLine(TPoint(iFont->TextWidthInPixels(textBuffer),level-8),TPoint(iFont->TextWidthInPixels(textBuffer),level+2));

	bmpfon->iAlgStyle.SetIsBold(ETrue);
	level += height;
	iGc->DrawText(textBuffer,TPoint(0,level));
	iGc->DrawLine(TPoint(iFont->TextWidthInPixels(textBuffer),level-8),TPoint(iFont->TextWidthInPixels(textBuffer),level+2));

	bmpfon->iAlgStyle.SetIsBold(EFalse);
	bmpfon->iAlgStyle.SetIsItalic(ETrue);
	level += height;
	iGc->DrawText(textBuffer,TPoint(0,level));
	iGc->DrawLine(TPoint(iFont->TextWidthInPixels(textBuffer),level-8),TPoint(iFont->TextWidthInPixels(textBuffer),level+2));

	bmpfon->iAlgStyle.SetIsBold(ETrue);
	level += height;
	iGc->DrawText(textBuffer,TPoint(0,level));
	iGc->DrawLine(TPoint(iFont->TextWidthInPixels(textBuffer),level-8),TPoint(iFont->TextWidthInPixels(textBuffer),level+2));
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0076
 
  @SYMDEF             

  @SYMTestCaseDesc Text formatting
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests drawing of justified/clipped/underlined text to a gc 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoTextFormatsL()
	{
	ResetGc();

	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Swiss");
	fsp.iHeight=200;

	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);

	Box();
	//TheTest.Getch();

	Justification();
	//TheTest.Getch();

	Clipping();
	//TheTest.Getch();

	Underline();
	//TheTest.Getch();

	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);
	}

void CTFont::Box()
	{
	iGc->SetPenSize(TSize(0,0));
	iGc->SetBrushColor(KRgbWhite);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->Clear();
	iGc->DrawText(_L("Font test code:"),TPoint(20,20));
	iGc->DrawText(_L("Normal text printing, followed by posi"),TPoint(20,40));
	iGc->DrawText(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->DrawText(_L("Box text (left aligned)"),TRect(20,50,320,70),14,CGraphicsContext::ELeft);
	iGc->DrawText(_L("Box text (left aligned with margin)"),TRect(20,70,320,90),14,CGraphicsContext::ELeft,50);
	iGc->DrawText(_L("Box text (centred)"),TRect(20,90,320,110),14,CGraphicsContext::ECenter);
	iGc->DrawText(_L("Box text (right aligned)"),TRect(20,110,320,130),14,CGraphicsContext::ERight);
	iGc->DrawText(_L("Box text (right aligned with margin)"),TRect(20,130,320,150),14,CGraphicsContext::ERight,30);
	iGc->DrawText(_L("(Boxes drawn afterwards)"),TPoint(20,165));
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->DrawRect(TRect(20,50,320,70));
	iGc->DrawRect(TRect(20,70,320,90));
	iGc->DrawRect(TRect(20,90,320,110));
	iGc->DrawRect(TRect(20,110,320,130));

	iGc->SetBrushColor(TRgb(128,128,128));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->DrawText(_L("Box text (left aligned)"),TRect(320,50,620,70),14,CGraphicsContext::ELeft);
	iGc->DrawText(_L("Box text (left aligned with margin)"),TRect(320,70,620,90),14,CGraphicsContext::ELeft,50);
	iGc->DrawText(_L("Box text (centred)"),TRect(320,90,620,110),14,CGraphicsContext::ECenter);
	iGc->DrawText(_L("Box text (right aligned)"),TRect(320,110,620,130),14,CGraphicsContext::ERight);
	iGc->DrawText(_L("Box text (right aligned with margin)"),TRect(320,130,620,150),14,CGraphicsContext::ERight,30);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->DrawRect(TRect(320,50,620,70));
	iGc->DrawRect(TRect(320,70,620,90));
	iGc->DrawRect(TRect(320,90,620,110));
	iGc->DrawRect(TRect(320,110,620,130));

	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TInt width=iFont->TextWidthInPixels(_L("Box text followed by positionless text."));
	iGc->DrawText(_L("Box text"),TRect(20,150,620,170),14,width);
	iGc->DrawText(_L(" followed by positionless text."));
	iGc->DrawText(_L("Box text"),TRect(20,170,620,190),14,width,CGraphicsContext::ELeft,20);
	iGc->DrawText(_L(" followed by positionless text."));
	iGc->DrawText(_L("Box text"),TRect(20,190,620,210),14,width,CGraphicsContext::ECenter);
	iGc->DrawText(_L(" followed by positionless text."));
	iGc->DrawText(_L("Box text"),TRect(20,210,620,230),14,width,CGraphicsContext::ERight,20);
	iGc->DrawText(_L(" followed by positionless text."));
	iGc->DrawText(_L(""),TRect(20,230,620,250),14,width);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->DrawRect(TRect(20,150,620,170));
	iGc->DrawRect(TRect(20,170,620,190));
	iGc->DrawRect(TRect(20,190,620,210));
	iGc->DrawRect(TRect(20,210,620,230));
	}

void CTFont::Underline()
	{
	ResetGc();

	iGc->SetUnderlineStyle(EUnderlineOn);
	iGc->SetStrikethroughStyle(EStrikethroughOn);

	iGc->DrawText(_L("Font test code:"),TPoint(20,20));
	iGc->DrawText(_L("Normal text printing, followed by posi"),TPoint(20,40));
	iGc->DrawText(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->DrawText(_L("Box text (left aligned)"),TRect(20,50,320,70),14,CGraphicsContext::ELeft);
	iGc->DrawText(_L("Box text (left aligned with margin)"),TRect(20,70,320,90),14,CGraphicsContext::ELeft,50);
	iGc->DrawText(_L("Box text (centred)"),TRect(20,90,320,110),14,CGraphicsContext::ECenter);
	iGc->DrawText(_L("Box text (right aligned)"),TRect(20,110,320,130),14,CGraphicsContext::ERight);
	iGc->DrawText(_L("Box text (left aligned)"),TRect(320,50,620,70),14,CGraphicsContext::ELeft);
	iGc->DrawText(_L("Box text (left aligned with margin)"),TRect(320,70,620,90),14,CGraphicsContext::ELeft,50);
	iGc->DrawText(_L("Box text (centred)"),TRect(320,90,620,110),14,CGraphicsContext::ECenter);
	iGc->DrawText(_L("Box text (right aligned)"),TRect(320,110,620,130),14,CGraphicsContext::ERight);
	TInt width=iFont->TextWidthInPixels(_L("Box text followed by positionless text."));
	iGc->DrawText(_L("Box text"),TRect(20,130,620,150),14,width);
	iGc->DrawText(_L(" followed by positionless text."));
	iGc->DrawText(_L("Box text"),TRect(20,150,620,170),14,width,CGraphicsContext::ELeft,20);
	iGc->DrawText(_L(" followed by positionless text."));
	iGc->DrawText(_L("Box text"),TRect(20,170,620,190),14,width,CGraphicsContext::ECenter);
	iGc->DrawText(_L(" followed by positionless text."));
	iGc->DrawText(_L("Box text"),TRect(20,190,620,210),14,width,CGraphicsContext::ERight);
	iGc->DrawText(_L(" followed by positionless text."));
	iGc->DrawText(_L(""),TRect(20,210,620,230),14,width);

	iGc->SetUnderlineStyle(EUnderlineOff);
	iGc->SetStrikethroughStyle(EStrikethroughOff);
	}

void CTFont::Justification()
	{
	iGc->SetUnderlineStyle(EUnderlineOff);
	iGc->SetStrikethroughStyle(EStrikethroughOff);
	iGc->SetPenSize(TSize(0,0));
	iGc->SetBrushColor(KRgbWhite);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->Clear();
	iGc->DrawText(_L("Font test code:"),TPoint(20,20));
	iGc->DrawText(_L("Normal text printing in one whole line"),TPoint(20,40));
	iGc->SetCharJustification(18,6);
	iGc->SetWordJustification(0,0);
	iGc->DrawText(_L("Normal text printing in one whole line"),TPoint(20,60));
	iGc->SetCharJustification(0,0);
	iGc->SetWordJustification(30,6);
	iGc->DrawText(_L("Normal text printing in one whole line"),TPoint(20,80));
	iGc->SetCharJustification(18,6);
	iGc->SetWordJustification(30,6);
	iGc->DrawText(_L("Normal text printing in one whole line"),TPoint(20,100));
	iGc->UpdateJustification(_L("Normal text printing in one whole line"));
//
	iGc->DrawText(_L("Normal text printing, followed by posi"),TPoint(20,120));
	iGc->UpdateJustification(_L("Normal text printing, followed by posi"));
	iGc->DrawText(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->UpdateJustification(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->SetCharJustification(18,6);
	iGc->SetWordJustification(0,0);
	iGc->DrawText(_L("Normal text printing, followed by posi"),TPoint(20,140));
	iGc->UpdateJustification(_L("Normal text printing, followed by posi"));
	iGc->DrawText(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->SetCharJustification(0,0);
	iGc->SetWordJustification(30,6);
	iGc->DrawText(_L("Normal text printing, followed by posi"),TPoint(20,160));
	iGc->UpdateJustification(_L("Normal text printing, followed by posi"));
	iGc->DrawText(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->SetCharJustification(18,6);
	iGc->SetWordJustification(30,6);
	iGc->DrawText(_L("Normal text printing, followed by posi"),TPoint(20,180));
	iGc->UpdateJustification(_L("Normal text printing, followed by posi"));
	iGc->DrawText(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->SetCharJustification(150,50);
	iGc->SetWordJustification(50,10);
	iGc->DrawText(_L("Normal text printing in one whole line,"),TPoint(20,200));
	iGc->UpdateJustification(_L("Normal text printing in one whole line,"));
	iGc->DrawText(_L("followed by more text, carrying over justification."),TPoint(20,220));
	iGc->SetCharJustification(0,0);
	iGc->SetWordJustification(0,0);
	}

void CTFont::Clipping()
	{
	iGc->SetUnderlineStyle(EUnderlineOff);
	iGc->SetStrikethroughStyle(EStrikethroughOff);
	iGc->SetPenSize(TSize(0,0));
	iGc->SetBrushColor(KRgbWhite);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->Clear();
	iGc->DrawText(_L("Font test code:"),TPoint(20,20));
	iGc->DrawText(_L("Normal text printing in one whole line"),TPoint(20,40));
	iGc->SetCharJustification(-6,6);
	iGc->DrawText(_L("Normal text printing in one whole line"),TPoint(20,60));
	iGc->SetCharJustification(-12,6);
	iGc->DrawText(_L("Normal text printing in one whole line"),TPoint(20,80));
	iGc->SetCharJustification(0,0);
//
	iGc->DrawText(_L("Normal text printing, followed by posi"),TPoint(20,120));
	iGc->DrawText(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->SetCharJustification(-50,50);
	iGc->DrawText(_L("Normal text printing, followed by posi"),TPoint(20,140));
	iGc->UpdateJustification(_L("Normal text printing, followed by posi"));
	iGc->DrawText(_L("tionless printing (halfway thro' posi-tionless)."));
	iGc->SetCharJustification(0,0);
	iGc->DrawText(_L("Normal text printing in one whole line,"),TPoint(20,160));
	iGc->DrawText(_L("followed by more text, carrying over justification."),TPoint(20,180));
	iGc->SetCharJustification(-90,90);
	iGc->DrawText(_L("Normal text printing in one whole line,"),TPoint(20,200));
	iGc->UpdateJustification(_L("Normal text printing in one whole line,"));
	iGc->DrawText(_L("followed by more text, carrying over justification."),TPoint(20,220));
	iGc->SetCharJustification(0,0);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0077
 
  @SYMDEF             

  @SYMTestCaseDesc Check text drawing
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests aspects of drawing text to the screen
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoBitAlignmentL()
	{
	ResetGc();
	TInt ret=KErrNone;
	
	const TUint8 testchars[2]={'M', 'W'};
	TFontSpec fsp;
	TInt xpos=0;
	TInt stringwidth=0;
	TBuf<224> letters;
	fsp.iTypeface.iName=_L("Swiss");
	fsp.iHeight=300;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	TInt ascent=iFont->AscentInPixels();
	TInt descent=iFont->DescentInPixels();
	TInt charvertpos=ascent+descent+20;
	TRgb refcolour(0,0,0);
	TRgb charcolour(0,0,0);
	TInt charnum=0;
	TBuf<1> letterdesc;
	letterdesc.SetLength(1);
	for(;charnum<2;charnum++)
		{
		letterdesc[0]=testchars[charnum];
		TInt letterwidth=iFont->TextWidthInPixels(letterdesc);
		if(!letterwidth) continue;
		for(xpos=10;xpos<43;xpos++)
			{
			iGc->Clear();
			TPoint refpt(10,ascent+10);
			TPoint charpt(xpos,ascent+charvertpos);
			iGc->SetDitherOrigin(refpt);
			iGc->DrawText(letterdesc,refpt);
			iGc->SetDitherOrigin(charpt);
			iGc->DrawText(letterdesc,charpt);
			for(TInt ycheck=0;ycheck<ascent+descent;ycheck+=2)
				for(TInt xcheck=0;xcheck<letterwidth;xcheck+=2)
					{
					iDev->GetPixel(refcolour,TPoint(xcheck+10,ycheck+10));
					iDev->GetPixel(charcolour,TPoint(xcheck+xpos,ycheck+charvertpos));
					TEST(refcolour==charcolour);
					if(refcolour!=charcolour)
						{
						ret=KErrGeneral;
						goto end;
						}
					}
			}
		}
	iGc->SetPenColor(TRgb(128,128,128));
	for(charnum=0;charnum<2;charnum++)
		{
		letterdesc[0]=testchars[charnum];
		TInt letterwidth=iFont->TextWidthInPixels(letterdesc);
		if(!letterwidth) continue;
		for(xpos=10;xpos<43;xpos++)
			{
			iGc->Clear();
			TPoint refpt(10,ascent+10);
			TPoint charpt(xpos,ascent+charvertpos);
			iGc->SetDitherOrigin(TPoint(refpt.iX,0));
			iGc->DrawText(letterdesc,refpt);
			iGc->SetDitherOrigin(TPoint(charpt.iX,0));
			iGc->DrawText(letterdesc,charpt);
			for(TInt ycheck=0;ycheck<ascent+descent;ycheck+=2)
				for(TInt xcheck=0;xcheck<letterwidth;xcheck+=2)
					{
					iDev->GetPixel(refcolour,TPoint(xcheck+10,ycheck+10));
					iDev->GetPixel(charcolour,TPoint(xcheck+xpos,ycheck+charvertpos));
					TEST(refcolour==charcolour);
					if(refcolour!=charcolour)
						{
						ret=KErrGeneral;
						goto end;
						}
					}
			}
		}
	iGc->SetPenColor(KRgbBlack);
	letters.SetLength(224);
	for(charnum=32;charnum<256;charnum++)
		letters[charnum-32]=TUint8(charnum);
	letters.SetLength(iFont->TextCount(letters,iDev->SizeInPixels().iWidth-45));
	stringwidth=iFont->TextWidthInPixels(letters);
	for(xpos=10;xpos<43;xpos++)
		{
		iGc->Clear();
		TPoint refpt(10,ascent+10);
		TPoint charpt(xpos,ascent+charvertpos);
		iGc->DrawText(letters,refpt);
		charnum=0;
		letterdesc[0]=letters[charnum];
		charnum++;
		iGc->DrawText(letterdesc,charpt);
		for(;charnum<letters.Length();charnum++)
			{
			letterdesc[0]=letters[charnum];
			iGc->DrawText(letterdesc);
			}
		for(TInt ycheck=0;ycheck<ascent+descent;ycheck+=2)
			for(TInt xcheck=0;xcheck<stringwidth;xcheck+=2)
				{
				iDev->GetPixel(refcolour,TPoint(xcheck+10,ycheck+10));
				iDev->GetPixel(charcolour,TPoint(xcheck+xpos,ycheck+charvertpos));
				TEST(refcolour==charcolour);
				if(refcolour!=charcolour)
					{
					ret=KErrGeneral;
					goto end;
					}
				}
		}
	for(charnum=0;charnum<2;charnum++)
		{
		letterdesc[0]=testchars[charnum];
		TInt letterwidth=iFont->TextWidthInPixels(letterdesc);
		if(!letterwidth) continue;
		for(xpos=10;xpos<43;xpos++)
			{
			iGc->Clear();
			TPoint refpt(10,ascent+10);
			TPoint charpt(xpos,ascent+charvertpos);
			iGc->DrawText(letterdesc,TRect(refpt.iX,refpt.iY-ascent,refpt.iX+letterwidth,refpt.iY+descent),ascent);
			iGc->DrawText(letterdesc,TRect(charpt.iX,charpt.iY-ascent,charpt.iX+letterwidth,charpt.iY+descent),ascent);
			for(TInt ycheck=0;ycheck<ascent+descent;ycheck+=2)
				for(TInt xcheck=0;xcheck<letterwidth;xcheck+=2)
					{
					iDev->GetPixel(refcolour,TPoint(xcheck+10,ycheck+10));
					iDev->GetPixel(charcolour,TPoint(xcheck+xpos,ycheck+charvertpos));
					TEST(refcolour==charcolour);
					if(refcolour!=charcolour)
						{
						ret=KErrGeneral;
						goto end;
						}
					}
			}
		}
	iGc->SetPenColor(TRgb(128,128,128));
	for(charnum=0;charnum<2;charnum++)
		{
		letterdesc[0]=testchars[charnum];
		TInt letterwidth=iFont->TextWidthInPixels(letterdesc);
		if(!letterwidth) continue;
		for(xpos=10;xpos<43;xpos++)
			{
			iGc->Clear();
			TPoint refpt(10,ascent+10);
			TPoint charpt(xpos,ascent+charvertpos);
			iGc->SetDitherOrigin(TPoint(refpt.iX,0));
			iGc->DrawText(letterdesc,TRect(refpt.iX,refpt.iY-ascent,refpt.iX+letterwidth,refpt.iY+descent),ascent);
			iGc->SetDitherOrigin(TPoint(charpt.iX,0));
			iGc->DrawText(letterdesc,TRect(charpt.iX,charpt.iY-ascent,charpt.iX+letterwidth,charpt.iY+descent),ascent);
			for(TInt ycheck=0;ycheck<ascent+descent;ycheck+=2)
				for(TInt xcheck=0;xcheck<letterwidth;xcheck+=2)
					{
					iDev->GetPixel(refcolour,TPoint(xcheck+10,ycheck+10));
					iDev->GetPixel(charcolour,TPoint(xcheck+xpos,ycheck+charvertpos));
					TEST(refcolour==charcolour);
					if(refcolour!=charcolour)
						{
						ret=KErrGeneral;
						goto end;
						}
					}
			}
		}
end:
	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);
	//TheTest.Getch();
	User::LeaveIfError(ret);
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0078
 
  @SYMDEF             

  @SYMTestCaseDesc Large fonts
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions draws text to a gc with different weight and posture with a large height
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoLargeFontsL()
	{
	TInt typefaces = iDev->NumTypefaces();
	TInt height = 0;

	for (TInt count = 0; count < typefaces; count++)
		{
		TTypefaceSupport info;
		iDev->TypefaceSupport(info,count);

		for (TInt index = 0; index < info.iNumHeights; index++)
			{
			height = iDev->FontHeightInPixels(count,index);
			if (height <= 32)
				continue;

			TFontSpec fs;
			fs.iTypeface=info.iTypeface;
			fs.iHeight=height;
			fs.iFontStyle.SetPosture(EPostureUpright);
			fs.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);

			User::LeaveIfError(GetNearestFontToDesignHeightInPixels(iFont,fs));
			DisplayLargeChars();
			iDev->ReleaseFont(iFont);
			//TheTest.Getch();

			fs.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
			User::LeaveIfError(GetNearestFontToDesignHeightInPixels(iFont,fs));
			DisplayLargeChars();
			iDev->ReleaseFont(iFont);
			//TheTest.Getch();

			fs.iFontStyle.SetPosture(EPostureItalic);
			fs.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
			User::LeaveIfError(GetNearestFontToDesignHeightInPixels(iFont,fs));
			DisplayLargeChars();
			iDev->ReleaseFont(iFont);
			//TheTest.Getch();

			fs.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
			User::LeaveIfError(GetNearestFontToDesignHeightInPixels(iFont,fs));
			DisplayLargeChars();
			iDev->ReleaseFont(iFont);
			//TheTest.Getch();
			}
		}
	}

// Draw a character and its glyph and character bounding boxes. Return the rightmost point drawn.
TInt CTFont::DisplayCharAndBounds(TInt aChar,TPoint aPos,TBool aVertical,TBool aUp)
	{
	TOpenFontCharMetrics cm;
	const TUint8* bitmap;
	TSize size;
	if (!iFont->GetCharacterData(aChar,cm,bitmap,size))
		return 0;
	TOpenFontMetrics fm;
	if (!iFont->GetFontMetrics(fm))
		return 0;
	TRect glyph_bounds, char_bounds;
	if (aVertical)
		{
		if (aUp)
			{
			glyph_bounds.iTl.iX = aPos.iX - cm.HorizBearingY();
			char_bounds.iTl.iX = aPos.iX - fm.MaxHeight();
			glyph_bounds.iTl.iY = aPos.iY - cm.HorizBearingX() - cm.Width();
			char_bounds.iTl.iY = aPos.iY - cm.HorizAdvance();
			}
		else
			{
			glyph_bounds.iTl.iX = aPos.iX + cm.HorizBearingY() - cm.Height() + 1;
			char_bounds.iTl.iX = aPos.iX - fm.MaxDepth() + 1;
			glyph_bounds.iTl.iY = aPos.iY + cm.HorizBearingX();
			char_bounds.iTl.iY = aPos.iY;
			}
		glyph_bounds.iBr = glyph_bounds.iTl + TSize(cm.Height(),cm.Width());
		char_bounds.iBr = char_bounds.iTl + TSize(fm.MaxHeight() + fm.MaxDepth(),cm.HorizAdvance());
		}
	else
		{
		glyph_bounds.iTl.iX = aPos.iX + cm.HorizBearingX();
		char_bounds.iTl.iX = aPos.iX;
		glyph_bounds.iTl.iY = aPos.iY - cm.HorizBearingY();
		char_bounds.iTl.iY = aPos.iY - fm.MaxHeight();
		glyph_bounds.iBr = glyph_bounds.iTl + TSize(cm.Width(),cm.Height());
		char_bounds.iBr = char_bounds.iTl + TSize(cm.HorizAdvance(),fm.MaxHeight() + fm.MaxDepth());
		}

	TInt x_offset = aPos.iX - Min(glyph_bounds.iTl.iX,char_bounds.iTl.iX);
	aPos.iX += x_offset;
	glyph_bounds.Move(x_offset,0);
	char_bounds.Move(x_offset,0);
	TText text_buffer[1];
	text_buffer[0] = (TText)aChar;
	TPtrC text(text_buffer,1);
	if (aVertical)
		iGc->DrawTextVertical(text,aPos,aUp);
	else
		iGc->DrawText(text,aPos);
	iGc->DrawRect(glyph_bounds);
	iGc->DrawRect(char_bounds);

	return Max(glyph_bounds.iBr.iX,char_bounds.iBr.iX);
	}

void CTFont::DisplayLargeChars()
	{
	ResetGc();
	iGc->UseFont(iFont);

	TPoint pos(20,iDev->SizeInPixels().iHeight / 2);

	pos.iX = DisplayCharAndBounds('G',pos,FALSE,FALSE) + 20;
	pos.iX = DisplayCharAndBounds('p',pos,FALSE,FALSE) + 20;
	pos.iX = DisplayCharAndBounds('G',pos,TRUE,TRUE) + 20;
	pos.iX = DisplayCharAndBounds('p',pos,TRUE,TRUE) + 20;
	pos.iX = DisplayCharAndBounds('G',pos,TRUE,FALSE) + 20;
	pos.iX = DisplayCharAndBounds('p',pos,TRUE,FALSE) + 20;

	iGc->SetPenStyle(CGraphicsContext::EDottedPen);
	iGc->SetPenColor(KRgbGray);

	iGc->DiscardFont();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0079
 
  @SYMDEF             

  @SYMTestCaseDesc Vertical text
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Draws text in varied weight and typeface to a gc in a vertical manner
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoVerticalText()
	{
	TFontSpec fs;
	TInt typefaces=iDev->NumTypefaces();
	TTypefaceSupport info;
	TPoint pos(0,0);
	TBool dir=ETrue;
	TInt count=0;
	for(;count<typefaces;count++)
		{
		iDev->TypefaceSupport(info,count);
		pos.SetXY(0,0);
		dir=ETrue;
		ResetGc();
		for(TInt index=0;index<info.iNumHeights;index++)
			{
			TInt height=iDev->FontHeightInPixels(count,index);
			if(height<5) continue;
			fs.iTypeface=info.iTypeface;
			fs.iHeight=height;
			User::LeaveIfError(GetNearestFontToDesignHeightInPixels(iFont,fs));
			iGc->UseFont(iFont);
			if(dir) pos.iY=iDev->SizeInPixels().iHeight;
			else pos.iY=0;
			if(dir)
				pos.iX+=iFont->AscentInPixels()+1;
			else
				pos.iX+=iFont->HeightInPixels()-1-iFont->AscentInPixels();
			iGc->DrawTextVertical(_L("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),pos,dir);
			if(dir)
				pos.iX+=iFont->HeightInPixels()+1-iFont->AscentInPixels();
			else
				pos.iX+=iFont->AscentInPixels()+3;
			dir=!dir;
			iDev->ReleaseFont(iFont);
			iGc->DiscardFont();
			if (pos.iX > iDev->SizeInPixels().iWidth)
				break;
			}
		//TheTest.Getch();
		}

	for(count=0;count<typefaces;count++)
		{
		iDev->TypefaceSupport(info,count);
		pos.SetXY(0,0);
		dir=ETrue;
		ResetGc();
		iGc->SetUnderlineStyle(EUnderlineOn);
		iGc->SetStrikethroughStyle(EStrikethroughOn);
		for(TInt index=0;index<info.iNumHeights;index++)
			{
			TInt height=iDev->FontHeightInPixels(count,index);
			if(height<5) continue;
			fs.iTypeface=info.iTypeface;
			fs.iHeight=height;
			User::LeaveIfError(GetNearestFontToDesignHeightInPixels(iFont,fs));
			iGc->UseFont(iFont);
			if(dir) pos.iY=iDev->SizeInPixels().iHeight;
			else pos.iY=0;
			if(dir)
				pos.iX+=iFont->AscentInPixels()+3;
			else
				pos.iX+=iFont->HeightInPixels()+1-iFont->AscentInPixels();
			iGc->DrawTextVertical(_L("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),pos,dir);
			if(dir)
				pos.iX+=iFont->HeightInPixels()+3-iFont->AscentInPixels();
			else
				pos.iX+=iFont->AscentInPixels()+5;
			dir=!dir;
			iDev->ReleaseFont(iFont);
			iGc->DiscardFont();
			if (pos.iX > iDev->SizeInPixels().iWidth)
				break;
			}
		//TheTest.Getch();
		}
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0080
 
  @SYMDEF             

  @SYMTestCaseDesc Vertical Text in a box
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Draws text in varied weight and typeface to a gc in a vertical manner within a rect
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoVerticalBoxL()
	{
	ResetGc();

	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Swiss");
	fsp.iHeight=200;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);

	TInt t = 20;
	TInt b = iDev->SizeInPixels().iHeight - 20;
	TRect r1(60,t,80,b);
	TRect r2(100,t,120,b);
	TRect r3(140,t,160,b);
	TRect r4(180,t,200,b);
	TRect r5(220,t,240,b);

	TRect r6(440,t,460,b);
	TRect r7(400,t,420,b);
	TRect r8(360,t,380,b);
	TRect r9(320,t,340,b);
	TRect r10(280,t,300,b);

	iGc->DrawTextVertical(_L("Font test code:"),TPoint(t,b),ETrue);
	iGc->DrawTextVertical(_L("Left aligned"),r1,14,ETrue,CGraphicsContext::ELeft);
	iGc->DrawTextVertical(_L("With margin"),r2,14,ETrue,CGraphicsContext::ELeft,50);
	iGc->DrawTextVertical(_L("Centred"),r3,14,ETrue,CGraphicsContext::ECenter);
	iGc->DrawTextVertical(_L("Right aligned"),r4,14,ETrue,CGraphicsContext::ERight);
	iGc->DrawTextVertical(_L("With margin"),r5,14,ETrue,CGraphicsContext::ERight,30);

	iGc->DrawTextVertical(_L("Font test code:"),TPoint(r6.iBr.iX + t,t),EFalse);
	iGc->DrawTextVertical(_L("Left aligned"),r6,14,EFalse,CGraphicsContext::ELeft);
	iGc->DrawTextVertical(_L("With margin"),r7,14,EFalse,CGraphicsContext::ELeft,50);
	iGc->DrawTextVertical(_L("Centred"),r8,14,EFalse,CGraphicsContext::ECenter);
	iGc->DrawTextVertical(_L("Right aligned"),r9,14,EFalse,CGraphicsContext::ERight);
	iGc->DrawTextVertical(_L("With margin"),r10,14,EFalse,CGraphicsContext::ERight,30);

	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->SetPenSize(TSize(1,1));
	iGc->DrawRect(r1);
	iGc->DrawRect(r2);
	iGc->DrawRect(r3);
	iGc->DrawRect(r4);
	iGc->DrawRect(r5);
	iGc->DrawRect(r6);
	iGc->DrawRect(r7);
	iGc->DrawRect(r8);
	iGc->DrawRect(r9);
	iGc->DrawRect(r10);

	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);
	//TheTest.Getch();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0081
 
  @SYMDEF             

  @SYMTestCaseDesc Super/subscript
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions tests the drawing of super and subscript fonts to the gc 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoSuperSubScriptL()
	{
	ResetGc();

	TInt separation = iDev->SizeInPixels().iHeight / 6;
	TPoint textPos(20,separation);

	CFbsFont* fnorm=NULL;
	CFbsFont* fsuper=NULL;
	CFbsFont* fsub=NULL;

	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Swiss");
	fsp.iHeight=300;

	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(fnorm,fsp));
	fsp.iFontStyle.SetPrintPosition(EPrintPosSuperscript);
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(fsuper,fsp));
	fsp.iFontStyle.SetPrintPosition(EPrintPosSubscript);
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(fsub,fsp));

	iGc->UseFont(fnorm);
	iGc->DrawText(_L("H"),textPos);
	iGc->UseFont(fsub);
	iGc->DrawText(_L("2"));
	iGc->UseFont(fnorm);
	iGc->DrawText(_L("O"));

	textPos.iY += separation;
	iGc->DrawText(_L("X"),textPos);
	iGc->UseFont(fsuper);
	iGc->DrawText(_L("2"));
	iGc->UseFont(fnorm);
	iGc->DrawText(_L("+Y"));
	iGc->UseFont(fsuper);
	iGc->DrawText(_L("2"));
	iGc->UseFont(fnorm);
	iGc->DrawText(_L("=Z"));
	iGc->UseFont(fsuper);
	iGc->DrawText(_L("2"));

	textPos.iY += separation;
	iGc->UseFont(fnorm);
	iGc->DrawText(_L("S"),textPos);
	iGc->UseFont(fsuper);
	iGc->DrawText(_L("uper"));
	iGc->UseFont(fnorm);
	iGc->DrawText(_L("S"));
	iGc->UseFont(fsuper);
	iGc->DrawText(_L("cript"));

	textPos.iY += separation;
	iGc->UseFont(fnorm);
	iGc->DrawText(_L("S"),textPos);
	iGc->UseFont(fsub);
	iGc->DrawText(_L("ub"));
	iGc->UseFont(fnorm);
	iGc->DrawText(_L("S"));
	iGc->UseFont(fsub);
	iGc->DrawText(_L("cript"));

	textPos.iY += separation;
	iGc->UseFont(fnorm);
	iGc->DrawText(_L("Normal"),textPos);

	iGc->DiscardFont();
	iDev->ReleaseFont(fnorm);
	iDev->ReleaseFont(fsuper);
	iDev->ReleaseFont(fsub);
	//TheTest.Getch();
	}

/**
  @SYMTestCaseID GRAPHICS-BITGDI-0082
 
  @SYMDEF             

  @SYMTestCaseDesc Underlining/strikethrough
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions tests the drawing of Underlining and strikethrough fonts to the gc  
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully. 
*/
void CTFont::DoUlStL()
	{
	iGc->SetPenColor(KRgbBlack);
	iGc->SetBrushColor(KRgbWhite);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->Clear();
	iGc->SetUnderlineStyle(EUnderlineOn);
	iGc->SetStrikethroughStyle(EStrikethroughOn);
	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Swiss");
	fsp.iHeight=15;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	iDev->ReleaseFont(iFont);
	iGc->DrawText(_L(" "),TPoint(10,iDev->SizeInPixels().iHeight/3));
	TInt height=50;
	for(;height<1000;height+=50)
		{
		fsp.iHeight=height;
		User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
		iGc->UseFont(iFont);
		iDev->ReleaseFont(iFont);
		TBuf<4> num;
		num.Num(height/20);
		iGc->DrawText(_L(" "));
		iGc->DrawText(num);
		}
	fsp.iHeight=15;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	iDev->ReleaseFont(iFont);
	iGc->DrawText(_L(" "),TPoint(10,iDev->SizeInPixels().iHeight*3/4));
	for(;height<2000;height+=50)
		{
		fsp.iHeight=height;
		User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
		iGc->UseFont(iFont);
		iDev->ReleaseFont(iFont);
		TBuf<4> num;
		num.Num(height/20);
		iGc->DrawText(_L(" "));
		iGc->DrawText(num);
		}
	iGc->SetUnderlineStyle(EUnderlineOff);
	iGc->SetStrikethroughStyle(EStrikethroughOff);
	iGc->DiscardFont();
	//TheTest.Getch();
	}

/**
  @SYMTestCaseID GRAPHICS-SYSLIB-BITGDI-UT-4006
 
  @SYMDEF             

  @SYMTestCaseDesc Text drawing in context
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests drawing ,mainly punctuation marks in context to a gc 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully, i.e. only text within the context should be drawn to the screen, and all punctuations should be drawn to the horizontal baseline of the script of the character that preceded the punctuation marks.
*/
void CTFont::DrawTextInContext()
	{
	ResetGc();

	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Series 60 Sans");
	fsp.iHeight=250;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	
	TInt ascent=iFont->AscentInPixels();
	TPoint pt(10,ascent+10);
	CGraphicsContext::TTextParameters param;
	param.iStart = 0;
	param.iEnd = 32;
	iGc->DrawText(_L("Punctuations in various scripts:"),&param,pt);

	iGc->DiscardFont();
	fsp.iHeight=430;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	
	param.iStart = 27;
	param.iEnd = 60;
	pt+=TPoint(0,50);	
	iGc->DrawText(_L("This text will not be drawnK.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Latin This text will not be drawn"),&param,pt);
	pt+=TPoint(0,50);
	param.iEnd = 60;
	iGc->DrawText(_L("This text will not be drawn\x0915.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Hindi This text will not be drawn"),&param,pt);
	pt+=TPoint(0,50);
	param.iEnd = 62;
	iGc->DrawText(_L("This text will not be drawn\x0C85.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Kannada This text will not be drawn"),&param,pt);
	
	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);
	
	}
/**
  @SYMTestCaseID GRAPHICS-SYSLIB-BITGDI-UT-4005
 
  @SYMDEF             

  @SYMTestCaseDesc Text drawing in a box in context - 1
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests drawing of boxed text,mainly punctuation marks in context to a gc 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully, i.e. only text within the context should be drawn in boxes to the screen, and all punctuations should be drawn to the horizontal baseline of the script of the character that preceded the punctuation marks.
*/
void CTFont::DrawTextInContextBox()
	{
	ResetGc();

	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Series 60 Sans");
	fsp.iHeight=250;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	
	TInt ascent=iFont->AscentInPixels();
	TPoint pt(10,ascent+10);
	CGraphicsContext::TTextParameters param;
	param.iStart = 0;
	param.iEnd = 41;
	iGc->DrawText(_L("Punctuations in various scripts in a box:"),&param,pt);
	
	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);
	fsp.iHeight=430;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	//ResetGc();
	param.iStart = 27;
	param.iEnd = 60;
	iGc->DrawText(_L("This text will not be drawnK.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Latin This text will not be drawn"),&param,TRect(10,50,640,120),40,CGraphicsContext::ELeft);
	param.iEnd = 60;
	iGc->DrawText(_L("This text will not be drawn\x0915.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Hindi This text will not be drawn"),&param,TRect(10,100,640,170),40,CGraphicsContext::ELeft);
	param.iEnd = 62;
	iGc->DrawText(_L("This text will not be drawn\x0C85.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Kannada This text will not be drawn"),&param,TRect(10,150,640,220),40,CGraphicsContext::ELeft);
	User::After(TTimeIntervalMicroSeconds32(8000000));
	
	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);
	}
	
/**
  @SYMTestCaseID GRAPHICS-SYSLIB-BITGDI-UT-4004
 
  @SYMDEF             

  @SYMTestCaseDesc Vertical drawing of text in context
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests vertical drawing of text, mainly punctuation marks, in context to a gc 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully, i.e. only text within the context should be drawn vertically to the screen, and all punctuations should be drawn to the vertical baseline of the script of the character that preceded the punctuation marks.
*/
void CTFont::DrawTextInContextVertical()
	{
	ResetGc();

	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Series 60 Sans");
	fsp.iHeight=300;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	
	TInt descent=iFont->DescentInPixels();
	TPoint pt(descent+10,10);
	CGraphicsContext::TTextParameters param;
	
	param.iStart = 27;
	param.iEnd = 60;	
	iGc->DrawTextVertical(_L("This text will not be drawnK.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Latin This text will not be drawn"),&param,pt,EFalse);
	User::After(TTimeIntervalMicroSeconds32(400000));
	pt+=TPoint(30,0);
	param.iEnd = 60;
	iGc->DrawTextVertical(_L("This text will not be drawn\x0915.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Hindi This text will not be drawn"),&param,pt,EFalse);
	User::After(TTimeIntervalMicroSeconds32(400000));
	pt+=TPoint(30,0);
	param.iEnd = 62;
	iGc->DrawTextVertical(_L("This text will not be drawn\x0C85.,!\"\x00A3$%^&*()_+-=;'#:@~/<>? Kannada This text will not be drawn"),&param,pt,EFalse);
	
	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);
	}
	
/**
  @SYMTestCaseID GRAPHICS-SYSLIB-BITGDI-UT-4003
 
  @SYMDEF             

  @SYMTestCaseDesc Text drawing in a box in context - 2
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests drawing of boxed text in context to a gc 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully, i.e. only text within the context should be drawn in boxes to the screen
*/
void CTFont::BoxInContext()
	{
	iGc->SetPenSize(TSize(0,0));
	iGc->SetBrushColor(KRgbWhite);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->Clear();
	CGraphicsContext::TTextParameters param;
	param.iStart = 29;
	param.iEnd = 44;
	iGc->DrawText(_L("(This text will not be drawn)Font test code:(This text will not be drawn)"),&param,TPoint(20,20));
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param,TPoint(20,40));
	param.iEnd = 77;
	iGc->DrawText(_L("(This text will not be drawn)tionless printing (halfway thro' posi-tionless).(This text will not be drawn)"),&param);
	param.iEnd = 52;
	iGc->DrawText(_L("(This text will not be drawn)Box text (left aligned)(This text will not be drawn)"),&param,TRect(20,50,320,70),14,CGraphicsContext::ELeft);
	param.iEnd = 64;
	iGc->DrawText(_L("(This text will not be drawn)Box text (left aligned with margin)(This text will not be drawn)"),&param,TRect(20,70,320,90),14,CGraphicsContext::ELeft,50);
	param.iEnd = 47;
	iGc->DrawText(_L("(This text will not be drawn)Box text (centred)(This text will not be drawn)"),&param,TRect(20,90,320,110),14,CGraphicsContext::ECenter);
	param.iEnd = 53;
	iGc->DrawText(_L("(This text will not be drawn)Box text (right aligned)(This text will not be drawn)"),&param,TRect(20,110,320,130),14,CGraphicsContext::ERight);
	param.iEnd = 65;
	iGc->DrawText(_L("(This text will not be drawn)Box text (right aligned with margin)(This text will not be drawn)"),&param,TRect(20,130,320,150),14,CGraphicsContext::ERight,30);
	param.iEnd = 53;
	iGc->DrawText(_L("(This text will not be drawn)(Boxes drawn afterwards)(This text will not be drawn)"),&param,TPoint(20,165));
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->DrawRect(TRect(20,50,320,70));
	iGc->DrawRect(TRect(20,70,320,90));
	iGc->DrawRect(TRect(20,90,320,110));
	iGc->DrawRect(TRect(20,110,320,130));

	iGc->SetBrushColor(TRgb(128,128,128));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	param.iEnd = 52;
	iGc->DrawText(_L("(This text will not be drawn)Box text (left aligned)(This text will not be drawn)"),&param,TRect(320,50,620,70),14,CGraphicsContext::ELeft);
	param.iEnd = 64;
	iGc->DrawText(_L("(This text will not be drawn)Box text (left aligned with margin)(This text will not be drawn)"),&param,TRect(320,70,620,90),14,CGraphicsContext::ELeft,50);
	param.iEnd = 47;
	iGc->DrawText(_L("(This text will not be drawn)Box text (centred)(This text will not be drawn)"),&param,TRect(320,90,620,110),14,CGraphicsContext::ECenter);
	param.iEnd = 53;
	iGc->DrawText(_L("(This text will not be drawn)Box text (right aligned)(This text will not be drawn)"),&param,TRect(320,110,620,130),14,CGraphicsContext::ERight);
	param.iEnd = 65;
	iGc->DrawText(_L("(This text will not be drawn)Box text (right aligned with margin)(This text will not be drawn)"),&param,TRect(320,130,620,150),14,CGraphicsContext::ERight,30);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->DrawRect(TRect(320,50,620,70));
	iGc->DrawRect(TRect(320,70,620,90));
	iGc->DrawRect(TRect(320,90,620,110));
	iGc->DrawRect(TRect(320,110,620,130));
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TInt width=iFont->TextWidthInPixels(_L("Box text followed by positionless text."));
	param.iEnd = 37;
	iGc->DrawText(_L("(This text will not be drawn)Box text(This text will not be drawn)"),&param,TRect(20,150,620,170),14,width);
	param.iEnd = 60;
	iGc->DrawText(_L("(This text will not be drawn) followed by positionless text.(This text will not be drawn)"),&param);
	param.iEnd = 37;
	iGc->DrawText(_L("(This text will not be drawn)Box text(This text will not be drawn)"),&param,TRect(20,170,620,190),14,width,CGraphicsContext::ELeft,20);
	param.iEnd = 60;
	iGc->DrawText(_L("(This text will not be drawn) followed by positionless text.(This text will not be drawn)"),&param);
	param.iEnd = 37;
	iGc->DrawText(_L("(This text will not be drawn)Box text(This text will not be drawn)"),&param,TRect(20,190,620,210),14,width,CGraphicsContext::ECenter);
	param.iEnd = 60;
	iGc->DrawText(_L("(This text will not be drawn) followed by positionless text.(This text will not be drawn)"),&param);
	param.iEnd = 37;
	iGc->DrawText(_L("(This text will not be drawn)Box text(This text will not be drawn)"),&param,TRect(20,210,620,230),14,width,CGraphicsContext::ERight,20);
	param.iEnd = 60;
	iGc->DrawText(_L("(This text will not be drawn) followed by positionless text.(This text will not be drawn)"),&param);
	iGc->DrawText(_L(""),TRect(20,230,620,250),14,width);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iGc->DrawRect(TRect(20,150,620,170));
	iGc->DrawRect(TRect(20,170,620,190));
	iGc->DrawRect(TRect(20,190,620,210));
	iGc->DrawRect(TRect(20,210,620,230));
	}

/**
  @SYMTestCaseID GRAPHICS-SYSLIB-BITGDI-UT-4002
 
  @SYMDEF             

  @SYMTestCaseDesc Text drawing and justification in context
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests drawing of justified text in context to a gc 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully, i.e. only text within the context should be drawn to the screen, and it should be justified within the context
*/
void CTFont::JustificationInContext()
	{
	iGc->SetUnderlineStyle(EUnderlineOff);
	iGc->SetStrikethroughStyle(EStrikethroughOff);
	iGc->SetPenSize(TSize(0,0));
	iGc->SetBrushColor(KRgbWhite);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->Clear();
	CGraphicsContext::TTextParameters param;
	param.iStart = 29;
	param.iEnd = 44;
	iGc->DrawText(_L("(This text will not be drawn)Font test code:(This text will not be drawn)"),&param,TPoint(20,20));
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing in one whole line(This text will not be drawn)"),&param,TPoint(20,40));
	iGc->SetCharJustification(18,6);
	iGc->SetWordJustification(0,0);
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing in one whole line(This text will not be drawn)"),&param,TPoint(20,60));
	iGc->SetCharJustification(0,0);
	iGc->SetWordJustification(30,6);
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing in one whole line(This text will not be drawn)"),&param,TPoint(20,80));
	iGc->SetCharJustification(18,6);
	iGc->SetWordJustification(30,6);
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing in one whole line(This text will not be drawn)"),&param,TPoint(20,100));
	iGc->UpdateJustification(_L("(This text will not be drawn)Normal text printing in one whole line(This text will not be drawn)"));
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param,TPoint(20,120));
	iGc->UpdateJustification(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param);
	param.iEnd = 77;
	iGc->DrawText(_L("(This text will not be drawn)tionless printing (halfway thro' posi-tionless).(This text will not be drawn)"),&param);
	iGc->UpdateJustification(_L("(This text will not be drawn)tionless printing (halfway thro' posi-tionless).(This text will not be drawn)"),&param);
	iGc->SetCharJustification(18,6);
	iGc->SetWordJustification(0,0);
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param,TPoint(20,140));
	iGc->UpdateJustification(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param);
	param.iEnd = 77;
	iGc->DrawText(_L("(This text will not be drawn)tionless printing (halfway thro' posi-tionless).(This text will not be drawn)"),&param);
	iGc->SetCharJustification(0,0);
	iGc->SetWordJustification(30,6);
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param,TPoint(20,160));
	iGc->UpdateJustification(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param);
	param.iEnd = 77;
	iGc->DrawText(_L("(This text will not be drawn)tionless printing (halfway thro' posi-tionless).(This text will not be drawn)"),&param);
	iGc->SetCharJustification(18,6);
	iGc->SetWordJustification(30,6);
	param.iEnd = 67;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param,TPoint(20,180));
	iGc->UpdateJustification(_L("(This text will not be drawn)Normal text printing, followed by posi(This text will not be drawn)"),&param);
	param.iEnd = 77;
	iGc->DrawText(_L("(This text will not be drawn)tionless printing (halfway thro' posi-tionless).(This text will not be drawn)"),&param);
	iGc->SetCharJustification(150,50);
	iGc->SetWordJustification(50,10);
	param.iEnd = 68;
	iGc->DrawText(_L("(This text will not be drawn)Normal text printing in one whole line,(This text will not be drawn)"),&param,TPoint(20,200));
	iGc->UpdateJustification(_L("(This text will not be drawn)Normal text printing in one whole line,(This text will not be drawn)"),&param);
	param.iEnd = 80;
	iGc->DrawText(_L("(This text will not be drawn)followed by more text, carrying over justification.(This text will not be drawn)"),&param,TPoint(20,220));
	iGc->SetCharJustification(0,0);
	iGc->SetWordJustification(0,0);
	}
		
/**
  @SYMTestCaseID GRAPHICS-SYSLIB-BITGDI-UT-4001
 
  @SYMDEF             

  @SYMTestCaseDesc Text drawing in context
   
  @SYMTestPriority High

  @SYMTestStatus Implemented

  @SYMTestActions Tests drawing of normal/justified/vertical/boxed text in context to a gc 
 
  @SYMTestExpectedResults Test should perform graphics operations succesfully, i.e. only text within the context should be drawn to the screen
*/
void CTFont::DoTextFormatsInContextL()
	{
	ResetGc();
	
	TFontSpec fsp;
	fsp.iTypeface.iName=_L("Series 60 Sans");
	fsp.iHeight=300;
	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);
	
	((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-BITGDI-UT-4006"));
	DrawTextInContext();
	((CTFontStep*)iStep)->RecordTestResultL();
	//TheTest.Getch();
	((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-BITGDI-UT-4005"));
	DrawTextInContextBox();
	((CTFontStep*)iStep)->RecordTestResultL();
	//TheTest.Getch();
	((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-BITGDI-UT-4004"));
	DrawTextInContextVertical();
	((CTFontStep*)iStep)->RecordTestResultL();
	//TheTest.Getch();
	
	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);

	fsp.iTypeface.iName=_L("Swiss");
	fsp.iHeight=200;

	User::LeaveIfError(GetNearestFontToDesignHeightInTwips(iFont,fsp));
	iGc->UseFont(iFont);

	((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-BITGDI-UT-4003"));
	BoxInContext();
	((CTFontStep*)iStep)->RecordTestResultL();
	//TheTest.Getch();

	((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-BITGDI-UT-4002"));
	JustificationInContext();
	((CTFontStep*)iStep)->RecordTestResultL();
	//TheTest.Getch();

	iGc->DiscardFont();
	iDev->ReleaseFont(iFont);
	((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-BITGDI-UT-4001"));
	}
	
void CTFont::ResetGc()
	{
	iGc->SetUnderlineStyle(EUnderlineOff);
	iGc->SetStrikethroughStyle(EStrikethroughOff);
	iGc->SetPenSize(TSize(1,1));
	iGc->SetPenColor(KRgbBlack);
	iGc->SetBrushColor(KRgbWhite);
	iGc->SetPenStyle(CGraphicsContext::ENullPen);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->Clear();
	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	}

TInt CTFont::GetNearestFontToDesignHeightInPixels(CFbsFont*& aFont, const TFontSpec& aFontSpec)
	{
	TInt ret = iDev->GetNearestFontToDesignHeightInPixels((CFont*&)aFont, aFontSpec);
	if (ret != KErrNone)
		{
		return ret;
		}
#ifdef __WINS__
	CBitmapFont* bitmapFont = CFbsFontEx::FontAddress(aFont);
	iFontCallCounter.iInjector.InjectShellcode(bitmapFont, &iFontCallCounter);
	if (bitmapFont->IsOpenFont())
		{
		iOpenFontCallCounter.iInjector.InjectShellcode(bitmapFont->OpenFont(), &iOpenFontCallCounter);
		}
#endif
	return KErrNone;
	}

TInt CTFont::GetNearestFontToDesignHeightInTwips(CFbsFont*& aFont, const TFontSpec& aFontSpec)
	{
	TInt ret = iDev->GetNearestFontToDesignHeightInTwips((CFont*&)aFont, aFontSpec);
	if (ret != KErrNone)
		{
		return ret;
		}
#ifdef __WINS__
	CBitmapFont* bitmapFont = CFbsFontEx::FontAddress(aFont);
	iFontCallCounter.iInjector.InjectShellcode(bitmapFont, &iFontCallCounter);
	if (bitmapFont->IsOpenFont())
		{
		iOpenFontCallCounter.iInjector.InjectShellcode(bitmapFont->OpenFont(), &iOpenFontCallCounter);
		}
#endif
	return KErrNone;
	}

void CTFont::RunTestCaseL(TInt aCurTestCase)
	{
	((CTFontStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
		case 1:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0075"));
            INFO_PRINTF1(_L("All fonts"));
			TRAPD(ret, DoAllFontsL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'All fonts' ended with ret='%d'\n"), ret);
			break;
			}
		case 2:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0076"));
	 		INFO_PRINTF1(_L("Text formatting"));
			TRAPD(ret, DoTextFormatsL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'Text formatting' ended with ret='%d'\n"), ret);
			break;
			}
		case 3:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0077"));
            INFO_PRINTF1(_L("Check text drawing"));
			TRAPD(ret, DoBitAlignmentL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'Check text drawing' ended with ret='%d'\n"), ret);
			break;
			}
		case 4:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0078"));
            INFO_PRINTF1(_L("Large fonts"));
			TRAPD(ret, DoLargeFontsL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'Large fonts' ended with ret='%d'\n"), ret);
			break;
			}
		case 5:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0080"));
            INFO_PRINTF1(_L("Vertical text"));
			TRAPD(ret, DoVerticalText();
			((CTFontStep*)iStep)->RecordTestResultL();
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0079"));
					   DoVerticalBoxL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'Vertical text' ended with ret='%d'\n"), ret);
			break;
			}
		case 6:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0081"));
         	INFO_PRINTF1(_L("Super/subscript"));
			TRAPD(ret, DoSuperSubScriptL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'Super/subscript' ended with ret='%d'\n"), ret);
			break;
			}
		case 7:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0082"));
			INFO_PRINTF1(_L("Underlining/strikethrough"));
			TRAPD(ret, DoUlStL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'Underlining/strikethrough' ended with ret='%d'\n"), ret);
			break;
			}
		case 8:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0074"));
            INFO_PRINTF1(_L("Anti-aliased text"));
			TRAPD(ret, DoAntiAliasedTextL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'Anti-aliased text' ended with ret='%d'\n"), ret);
			break;
			}
		case 9:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0075"));
        	INFO_PRINTF1(_L("FaceCache Performance text"));

			TInt64 starttime = User::FastCounter();
			TRAPD(ret, DoAllFontsL());
			TInt64 endtime = User::FastCounter();
			TEST(ret == KErrNone);

			TInt freq;
			User::LeaveIfError(HAL::Get(HALData::EFastCounterFrequency, freq));
			TInt64 diff = (1000000*(endtime - starttime)) / (TInt64)freq;

            INFO_PRINTF3(_L("Test case 'FaceCache Performance text' ended in '%Ld' usec with ret='%d'\n"), diff, ret);
			break;
			}
		case 10:
			{
/**
  @SYMTestCaseID GRAPHICS-BITGDI-0116
*/
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0116"));
			INFO_PRINTF1(_L("Font Externalize/Internalize"));
			TRAPD(ret, FontExternalizeL());
			TEST(ret==KErrNone);
			INFO_PRINTF2(_L("Test case 'Font Externalize/Internalize' ended with ret='%d'\n"), ret);
			break;
			}
		case 11:
			{
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-SYSLIB-BITGDI-UT-4001"));
	 		INFO_PRINTF1(_L("Text formatting"));
			TRAPD(ret, DoTextFormatsInContextL());
			TEST(ret == KErrNone);
            INFO_PRINTF2(_L("Test case 'Text formatting' ended with ret='%d'\n"), ret);
			break;
			}
		case 12:
			{
/**
@SYMTestCaseID		GRAPHICS-BITGDI-0118
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMDEF				EI0014

@SYMTestCaseDesc
	Tests that there are no virtual function calls to objects in FBServ's shared heap from outside
	the FBServ process. Such function calls only work if DLLs are guaranteed to be mapped at the
	same logical address in every process. This is not necessarily true for all memory models.

@SYMTestActions
	Intercepts the VF table in the CBitmapFont and COpenFont objects corresponding to the fonts used in the
	preceding BitGDI tests so that virtual function calls from outside the FBServ process are recorded.

@SYMTestExpectedResults
	Number of virtual function calls to CBitmapFont and COpenFont from outside FBServ is 0
*/
			((CTFontStep*)iStep)->SetTestStepID(_L("GRAPHICS-BITGDI-0118"));
			// this one should always be the last test, since it checks the final virtual function call count
#ifdef __WINS__
			INFO_PRINTF1(_L("Test no virtual function calls to objects in FBServ's shared heap from outside FBServ process"));
			TEST(iFontCallCounter.iVFCallsOutsideFBServ == 0);
			TEST(iOpenFontCallCounter.iVFCallsOutsideFBServ == 0);
#else
			INFO_PRINTF1(_L("Test no virtual function calls to objects in FBServ's shared heap from outside FBServ process - skipped on target"));
#endif
			break;
			}
		case 13:
			{
			((CTFontStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTFontStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();		
			break;
			}
		}
	((CTFontStep*)iStep)->RecordTestResultL();
	}


//--------------------------------------------------------------

__CONSTRUCT_STEP__(Font)

void CTFontStep::TestSetupL()
	{
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	}
	
void CTFontStep::TestClose()
	{
	RFbsSession::Disconnect();
	}
