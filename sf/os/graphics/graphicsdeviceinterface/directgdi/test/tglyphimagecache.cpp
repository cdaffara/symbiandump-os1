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
// This test is designed to run in environment with monotype font support.
// To set up environment launch ityperast.cmd  from epoc32\winscw\, and 
// then follow instruction on the screen.
// 
//

#include "tglyphimagecache.h"
#include <graphics/directgdiengine.h>
#include <e32base.h>
#include <VG/openvg.h>

// The size of the target in pixels to use for these tests
const TSize KGlyphCacheWindowSize(400, 400);

enum TFontParam
	{
	ENormal, 
	EUnderline = 0x1, 
	EStrikeThrough = 0x2,
	EBold = 0x4,
	EUnderlineBold = 0x5, 
	EItalic = 0x8,
	EItalicStrikeThrough = 0xa,
	};

class TFontType
	{
public:
	TInt iFontSize;
	TFontParam iFontParam;
	}; 

LOCAL_C void CleanCache(TAny* aPtr)
	{
	MFontGlyphImageStorage* glyphImageStorage = reinterpret_cast<MFontGlyphImageStorage*> (aPtr);
	glyphImageStorage->CleanGlyphImageCache();
	}

CTGlyphImageCache::CTGlyphImageCache()
	{
	SetTestStepName(KTGlyphImageCacheStep);
	}

CTGlyphImageCache::~CTGlyphImageCache()
	{
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0001

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test requests Glyph Image elements from storage according their Glyph Key.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Create Font Image Storage. In the cycle request aGlyph Image entry for a particular Glyph 
	code and a font ID. 
	Delete Glyph Storage.
	
@SYMTestExpectedResults
	GlyphImage function must succeed; foreground, shadow and outlined images (if applicable) must be created
*/
void CTGlyphImageCache::TestRetrieveEntryL(MFontGlyphImageStorage* aGlyphStorage, TGlyphBitmapType aGlyphType)
	{
	if(!aGlyphStorage)
		return;	
	_LIT(KTestName, "GlyphCache_RetrieveEntry"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}

	TSize size(20, 30);
	TInt bufferLength = size.iWidth * size.iHeight;
	TUint8* buffer = (TUint8*)User::AllocL(bufferLength);
	CleanupStack::PushL(buffer);
	if(aGlyphType == EMonochromeGlyphBitmap)
		{//to make decoding mechanizm work properly we need to fill the buffer
		Mem::Fill(buffer, bufferLength, 0x1);
		}
	
	TInt fontListId[] = 
		{
		1, 5, 3, 5
		};
	TInt numFont = sizeof(fontListId) / sizeof(fontListId[0]);
	TUint charCodeList[] =
		{
		1, 2, 3, 2, 3,
		};
	TInt numGlyphCode = sizeof(charCodeList) / sizeof(charCodeList[0]);

	for(TInt jj = 0; jj < numFont; jj++)
		{
		TInt fontId = fontListId[jj];
		for(TInt ii = 0; ii < numGlyphCode; ii++)
			{
			TChar glyphCode = TChar(charCodeList[ii]);
			VGImage foregroundImg = VG_INVALID_HANDLE;
			VGImage shadowImg = VG_INVALID_HANDLE;
			VGImage outlineImg = VG_INVALID_HANDLE;
			TBool res = aGlyphStorage->GlyphImage(fontId, glyphCode, aGlyphType, buffer, size, &foregroundImg, &shadowImg, &outlineImg);
			TESTNOERROR(res);
			if(res != KErrNone)
				{
				User::Leave(res); 
				}
			else
				{
				TEST(foregroundImg);
				if(aGlyphType == EFourColourBlendGlyphBitmap)
					{
					TEST(shadowImg);
					TEST(outlineImg);
					}
				}	
			}
		}
	CleanupStack::PopAndDestroy(buffer);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0002

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Drawing monochrome, anti-aliased and four colour fonts with different styles  

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Text is output with different effects (strikethrough, underline) and orientation 
	(horizontal, vertical up, vertical down) and at different sizes and positions.
	Clip region is also specified.

@SYMTestExpectedResults
	Test and reference images must match
*/
void CTGlyphImageCache::TestDrawGlyphL(MFontGlyphImageStorage* aGlyphStorage, TGlyphBitmapType aGlyphBitmapType, DrawTextDirection aDirection, DrawTextAdjustment aDrawAdjustment, TBool aClipText)
	{	
	_LIT(KTestNameTemplate, "DrawGlyph%S_Dir%d_Clip%d_Adjust%d"); 
	TBuf<128> testName;
	_LIT(KTextOutput, "Hello world! Hello world! Hello world! Hello world! Hello world! Hello world!");
	
	switch(aGlyphBitmapType)
		{
	case EMonochromeGlyphBitmap:
		_LIT(KMonochromeName, "Monochrome");
		testName.Format(KTestNameTemplate, &KMonochromeName, aDirection, aClipText, aDrawAdjustment);
		break;
	case EAntiAliasedGlyphBitmap:
		_LIT(KAntiAliasedName, "AntiAliased");
		testName.Format(KTestNameTemplate, &KAntiAliasedName, aDirection, aClipText, aDrawAdjustment);
		break;
	case EFourColourBlendGlyphBitmap:
		_LIT(KFourColourName, "FourColour");
		testName.Format(KTestNameTemplate, &KFourColourName, aDirection, aClipText, aDrawAdjustment);
		break;
	default: 
		User::Leave(KErrNotSupported);
		break;	
		}
	
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(testName);
		}
	
	ResetGc();
	_LIT(KTestFontTypefaceName,"DejaVu Sans Condensed");
	const TFontType fontType[] =
		{
		18, EStrikeThrough, 48, EUnderline, 16, EItalic, 78, EItalicStrikeThrough,

		18, EStrikeThrough, 48, EUnderlineBold, 16, ENormal, 78, EStrikeThrough, 78, EBold, 
		};
	
	TPoint pt(5, 5);
	TInt *coord = & (pt.iY);

	TSize size = iGdiTarget->SizeInPixels();
	TRect rect = size;
	rect.Shrink(20, 5);
	RRegion region(rect);
	TRect rect1;
	if(aDirection == EDrawTextHorizontal)
		{
		rect1 = TRect(size.iWidth / 2 - 40, 40, size.iWidth / 2 + 40, size.iHeight - 40);
		}
	else
		{
		rect1 = TRect(20, size.iHeight / 2 - 40, size.iWidth - 20, size.iHeight / 2 + 40);
		}	
	region.SubRect(rect1);
		
	if(aDirection == EDrawTextVerticalUp || aDirection == EDrawTextVerticalDown)
		{
		if(aDirection == EDrawTextVerticalUp)
			{
			pt.iY = size.iHeight - 5;
			}
		coord = &(pt.iX);
		}
	
	TInt arraySize = sizeof(fontType) / sizeof(fontType[0]);
	
	for(TInt ii = 0; ii < arraySize; ii++)
		{
		CFont *font = NULL;
		TFontSpec fspec(KTestFontTypefaceName, fontType[ii].iFontSize);
		fspec.iFontStyle.SetBitmapType(aGlyphBitmapType);
		
		iGc->Reset();

		if(aClipText)
			{
			iGc->SetClippingRegion(region);
			}

		if(aGlyphBitmapType == EFourColourBlendGlyphBitmap)
			{
			fspec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
			fspec.iFontStyle.SetEffects(FontEffect::EOutline, ETrue);
			fspec.iFontStyle.SetEffects(FontEffect::EDropShadow, ETrue);
			TRgb shadowColor = TRgb(0, 0, 255);
			iGc->SetTextShadowColor(shadowColor);
			TEST(shadowColor == iGc->TextShadowColor());
			iGc->SetBrushColor(TRgb(255, 0, 0));
			TEST(TRgb(255, 0, 0) == iGc->BrushColor());
			}
		else
			{
			iGc->SetBrushStyle(DirectGdi::ENullBrush);
			iGc->SetPenStyle(DirectGdi::ESolidPen);
			}	
		iGc->SetPenColor(TRgb(0, 255, 0));
		if(fontType[ii].iFontParam & EItalic)
			{
			fspec.iFontStyle.SetPosture(EPostureItalic);
			}
		if(fontType[ii].iFontParam & EBold)
			{
			fspec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
			}
			
		User::LeaveIfError(iFontStore->GetNearestFontToDesignHeightInPixels((CFont*&) font, fspec));
		if(iUseDirectGdi)
			{
			if(ii == 2)
				{
				iGc->SetFontNoDuplicate(font);
				(reinterpret_cast <CTestDirectGdiContext*> (iGc))->NoJustifyAutoUpdate();
				}
			else
				{
				iGc->SetFont(font);
				iGc->SetFontNoDuplicate(font);//shoudn't have any impact
				(reinterpret_cast <CTestDirectGdiContext*> (iGc))->SetJustifyAutoUpdate();
				}	
			}
		else
			{
			iGc->SetFont(font);
			iGc->SetFontNoDuplicate(font);//shoudn't have any impact
			}	
		TEST(iGc->HasFont());
		
		TFontSpec fontSpec = font->FontSpecInTwips();
		TGlyphBitmapType glyphBitmapType = fontSpec.iFontStyle.BitmapType();

		TEST(glyphBitmapType == aGlyphBitmapType);	
		if(fontType[ii].iFontParam & EUnderline)
			{
			iGc->SetUnderlineStyle(DirectGdi::EUnderlineOn);
			}
		else if(fontType[ii].iFontParam & EStrikeThrough)
			{
			iGc->SetStrikethroughStyle(DirectGdi::EStrikethroughOn);
			}
		
		if(EDrawTextVerticalDown != aDirection)
			{
			*coord += font->HeightInPixels() + 5;
			}
		
		if(aGlyphStorage && (ii == 0) && (arraySize > 1))
			{
			aGlyphStorage->CleanGlyphImageCache();
			aGlyphStorage->EnforceOOMFailure(ETrue);
			}
		DrawText(KTextOutput, pt, aDirection, aDrawAdjustment, ii == 1);
		if(aGlyphStorage)
			{
			aGlyphStorage->EnforceOOMFailure(EFalse);
			}
		
		if(EDrawTextVerticalDown == aDirection)
			{
			*coord += font->HeightInPixels() + 5;
			}

		if(fontType[ii].iFontParam == ENormal)
			{
			iGc->SetUnderlineStyle(DirectGdi::EUnderlineOff);
			iGc->SetStrikethroughStyle(DirectGdi::EStrikethroughOff);
			}
		iGc->ResetFont();
		iFontStore->ReleaseFont(font);
	
		TEST(!iGc->HasFont());
		}
	region.Close();
	if(aClipText)
		{
		iGc->ResetClippingRegion();
		}

	// Write the output to file.
	//
	TESTNOERROR(WriteTargetOutput(iTestParams, testName));
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0003

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Filling up the glyph image cache

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	1.	Fill glyph image storage with the same font but different glyphs until it exceeds its limit and resets the cache.
	Repeat this operation for other glyph types

	2.	Fill glyph image storage with different font and different glyph codes

@SYMTestExpectedResults
	Check that Glyph cache storage increments cache size correctly and deletes the least 
	usable tree.
*/
void CTGlyphImageCache::FillUpCacheL(MFontGlyphImageStorage* aGlyphStorage)
	{
	if(!aGlyphStorage)
		return;
	
	aGlyphStorage->CleanGlyphImageCache();
	TBuf<128> testName;
	_LIT(KTestName, "GlyphCache_FillUpCache"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	const TSize size(12, 16);
	TUint8* buffer = (TUint8*)User::AllocL(size.iWidth * size.iHeight);
	CleanupStack::PushL(buffer);
	TInt fontId = 10;
	TGlyphBitmapType glyphTypeList[] = 
		{
		EMonochromeGlyphBitmap,
		EAntiAliasedGlyphBitmap,
		EFourColourBlendGlyphBitmap,
		};
	const TInt numGlyphType = sizeof(glyphTypeList) / sizeof(glyphTypeList[0]);
	TESTL(aGlyphStorage->GlyphCacheSize() == 0);
	
	TInt glyphSizeInByte = 0;
	TInt expectedSize = 0;
	TUint32 glyphCode = 1;
	
	VGImage foregroundImg = VG_INVALID_HANDLE;
	VGImage shadowImg = VG_INVALID_HANDLE;
	VGImage outlineImg = VG_INVALID_HANDLE;
	for(TInt ii = 0; ii < numGlyphType; ii++)
		{
		TGlyphBitmapType glyphType = glyphTypeList[ii];
		//calculate the actual size of the glyph
		glyphSizeInByte = GlyphImageSizeInByte(glyphType, size);

		TUint maxGlyphSize = aGlyphStorage->MaxGlyphCacheSize() + glyphSizeInByte;
		do 
			{
			TEST(expectedSize == aGlyphStorage->GlyphCacheSize());
			TBool res = aGlyphStorage->GlyphImage(fontId, TChar(glyphCode), (TGlyphBitmapType)glyphType, buffer, size, &foregroundImg, &shadowImg, &outlineImg);
			TESTNOERRORL(res);
			glyphCode++;
			expectedSize+= glyphSizeInByte; 
			foregroundImg = VG_INVALID_HANDLE;
			shadowImg = VG_INVALID_HANDLE;
			outlineImg = VG_INVALID_HANDLE;
			}while(expectedSize <= maxGlyphSize);
		glyphCode = 1;	
		expectedSize = 0;
		TEST(glyphSizeInByte == aGlyphStorage->GlyphCacheSize());
		aGlyphStorage->CleanGlyphImageCache();
		}
		
	//now try to filling cache with different fonts and check that last usable font wont be removed
	TUint maxGlyphSize = aGlyphStorage->MaxGlyphCacheSize();
	glyphSizeInByte = GlyphImageSizeInByte(EMonochromeGlyphBitmap, size);
	do 
		{
		TEST(expectedSize == aGlyphStorage->GlyphCacheSize());
		TBool res = aGlyphStorage->GlyphImage(fontId, TChar(glyphCode), EMonochromeGlyphBitmap, buffer, size, &foregroundImg, &shadowImg, &outlineImg);
		TESTNOERRORL(res);
		glyphCode++;
		expectedSize+= glyphSizeInByte; 
		foregroundImg = VG_INVALID_HANDLE;
		shadowImg = VG_INVALID_HANDLE;
		outlineImg = VG_INVALID_HANDLE;
		}while(expectedSize <= maxGlyphSize);

	fontId += 10;
	glyphCode = 1;

	TBool res = aGlyphStorage->GlyphImage(fontId, TChar(glyphCode), EMonochromeGlyphBitmap, buffer, size, &foregroundImg, &shadowImg, &outlineImg);
	TEST(glyphSizeInByte == aGlyphStorage->GlyphCacheSize());
	TESTNOERROR(res);
	
	RArray<TUint32> listFontId;
	aGlyphStorage->FontIdInOrder(listFontId);
	TESTL(listFontId.Count() == 1);
	TEST(listFontId[0] == fontId);
	listFontId.Reset();
	
	CleanupStack::PopAndDestroy(buffer);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0004

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Tests filling the glyph image cache with different glyph images

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Specify glyphs which correspond to different fonts, glyph codes.
	Request related VGImages.
	Test the cache size is correct after each operation.
	Get the driver's MDirectGdiDriverCacheSize interface.
	Attempt to set the maximum size of the glyph cache to a size smaller than
	the existing cache size.
	Attempt to set the maximum size of the glyph cache to a size the same as
	the existing cache size.
	

@SYMTestExpectedResults
	Obtained glyph code and cache size must be correct each time.
	Setting the maximum glyph cache size to a smaller size than the existing cache
	size should fail with error KErrArgument.
	Setting the maximum glyph cache size to a size the same as the existing cache
	size should pass.
*/
void CTGlyphImageCache::FillCacheL(MFontGlyphImageStorage* aGlyphStorage)
	{
	ASSERT(aGlyphStorage);		
	
	aGlyphStorage->CleanGlyphImageCache();
	TBuf<128> testName;
	_LIT(KTestName, "GlyphCache_FillCache"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	const TSize smallFontSize = TSize(12, 16);
	const TSize smallFont1Size = TSize(10, 14);
	const TSize bigFontSize = TSize(18, 24);
	const TSize bigFont1Size = TSize(20, 26);
	const TInt smallFontId = 10;
	const TInt bigFontId = 20;
	const TInt smallFont1Id = 30;
	const TInt bigFont1Id = 40;
	VGImage foregroundImg = VG_INVALID_HANDLE;
	VGImage shadowImg = VG_INVALID_HANDLE;
	VGImage outlineImg = VG_INVALID_HANDLE;
	
	TUint8* buffer = (TUint8*)User::AllocL(bigFontSize.iWidth * bigFontSize.iHeight);
	CleanupStack::PushL(buffer);
	TESTL(aGlyphStorage->GlyphCacheSize() == 0);
	
	TInt expectedCacheSize = 0;
	TChar glyphCode = TChar(1);
	TGlyphBitmapType glyphType = EMonochromeGlyphBitmap;
	TBool res = aGlyphStorage->GlyphImage(smallFontId, glyphCode, glyphType, buffer, smallFontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	expectedCacheSize = GlyphImageSizeInByte(glyphType, smallFontSize);
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize());

	//try to retrieve the same glyph
	res = aGlyphStorage->GlyphImage(smallFontId, glyphCode, glyphType, buffer, smallFontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	//the size should be the same
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize()); 
		
	//now retrieving the same glyphCode but for the different font
	res = aGlyphStorage->GlyphImage(bigFontId, glyphCode, glyphType, buffer, bigFontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	expectedCacheSize += GlyphImageSizeInByte(glyphType, bigFontSize);
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize()); 

	//last font but different glyphCode
	glyphCode = TChar(2);
	res = aGlyphStorage->GlyphImage(bigFontId, glyphCode, glyphType, buffer, bigFontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	expectedCacheSize += GlyphImageSizeInByte(glyphType, bigFontSize);
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize()); 

	//small font and last glyphCode
	res = aGlyphStorage->GlyphImage(smallFontId, glyphCode, glyphType, buffer, smallFontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	expectedCacheSize += GlyphImageSizeInByte(glyphType, smallFontSize);
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize()); 

	//now change the type
	glyphType = EAntiAliasedGlyphBitmap;
	res = aGlyphStorage->GlyphImage(smallFont1Id, glyphCode, glyphType, buffer, smallFont1Size, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	expectedCacheSize += GlyphImageSizeInByte(glyphType, smallFont1Size);
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize()); 
	//try again
	res = aGlyphStorage->GlyphImage(smallFont1Id, glyphCode, glyphType, buffer, smallFont1Size, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize()); 

	//now change the type again
	glyphType = EFourColourBlendGlyphBitmap;
	res = aGlyphStorage->GlyphImage(bigFont1Id, glyphCode, glyphType, buffer, bigFont1Size, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	expectedCacheSize += GlyphImageSizeInByte(glyphType, bigFont1Size);
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize()); 

	//using initial value.
	glyphType = EMonochromeGlyphBitmap;
	glyphCode = TChar(1);
	res = aGlyphStorage->GlyphImage(smallFontId, glyphCode, glyphType, buffer, smallFontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	TEST(expectedCacheSize == aGlyphStorage->GlyphCacheSize());
	
	//get the driver's MDirectGdiDriverCacheSize extension interface and attempt
	//to set the maximum cache size to be smaller than the existing cache size
	CDirectGdiDriver* driver = CDirectGdiDriver::Static();
	TESTL(driver != NULL);
	MDirectGdiDriverCacheSize* driverCacheSize = NULL;
	res = driver->GetInterface(TUid::Uid(KDirectGdiDriverCacheSizeUid), (TAny*&)driverCacheSize);
	TESTNOERRORL(res);
	//save the original cache size
	TInt originalCacheSize = driverCacheSize->MaxGlyphCacheSize();
	//setting the cache size to a size smaller than the existing cache should fail
	res = driverCacheSize->SetMaxGlyphCacheSize(aGlyphStorage->GlyphCacheSize()-1);
	TEST(res == KErrArgument);	
	//setting the cache size to the same size as the existing cache should pass
	res = driverCacheSize->SetMaxGlyphCacheSize(aGlyphStorage->GlyphCacheSize());
	TESTNOERROR(res);
	TEST(aGlyphStorage->GlyphCacheSize() == driverCacheSize->MaxGlyphCacheSize());
	//reset the original driver cache size
	res = driverCacheSize->SetMaxGlyphCacheSize(originalCacheSize);
	TESTNOERROR(res);
	
	CleanupStack::PopAndDestroy(buffer);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0005

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9201 
	REQ9202 
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Tests the ordering font IDs in the glyph image cache.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Request glyph images for different fonts.

@SYMTestExpectedResults
	List of glyph cache trees must be arranged in order from most used to least used.
*/
void CTGlyphImageCache::FontListIdOrderL(MFontGlyphImageStorage* aGlyphStorage)
	{
	if(!aGlyphStorage)
		return;
	
	aGlyphStorage->CleanGlyphImageCache();
	TBuf<128> testName;
	_LIT(KTestName, "GlyphCache_FontListIdOrder"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	const TSize fontSize = TSize(12, 16);
	TUint8* buffer = (TUint8*)User::AllocL(fontSize.iWidth * fontSize.iHeight);
	CleanupStack::PushL(buffer);

	TChar glyphCode = TChar(10);
	TInt fontId = 10;
	VGImage foregroundImg = VG_INVALID_HANDLE;
	VGImage shadowImg = VG_INVALID_HANDLE;
	VGImage outlineImg = VG_INVALID_HANDLE;
	
	TGlyphBitmapType glyphType = EMonochromeGlyphBitmap;
	TInt res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);

	fontId = 20;
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);

	fontId = 30;
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);

	RArray<TUint32> fontListId;
	res = aGlyphStorage->FontIdInOrder(fontListId);
	if(res != KErrNone)
		{
		fontListId.Reset();
		User::Leave(res);
		}
	
	TESTL(fontListId.Count() == 3);
	TEST(fontListId[0] == 30);
	TEST(fontListId[1] == 20);
	TEST(fontListId[2] == 10);
	fontListId.Reset();

	//call entries again to reorder font id list
	fontId = 30;
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);
	
	fontId = 20;
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);

	fontId = 40;
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);

	fontId = 10;
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);

	fontId = 50;
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TESTNOERRORL(res);

	res = aGlyphStorage->FontIdInOrder(fontListId);
	if(res != KErrNone)
		{
		fontListId.Reset();
		User::Leave(res);
		}

	TESTL(fontListId.Count() == 5);
	TEST(fontListId[0] == 50);
	TEST(fontListId[1] == 10);
	TEST(fontListId[2] == 40);
	TEST(fontListId[3] == 20);
	TEST(fontListId[4] == 30);
	fontListId.Reset();

	CleanupStack::PopAndDestroy(buffer);
	}

/**
@SYMTestCaseID
	GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0006

@SYMPREQ 
	PREQ39

@SYMREQ
	REQ9195
	REQ9222 
	REQ9223 
	REQ9236 
	REQ9237

@SYMTestCaseDesc
	Test the operation of the glyph image cache when sent wrong parameters.

@SYMTestStatus
	Implemented

@SYMTestPriority
	High

@SYMTestActions
	Test requesting of glyph images with invalid paramaters - test an invalid glyph type
	and test requesting a glyph image with zero height and one with zero width.

@SYMTestExpectedResults
	The function must identify wrong arguments and return the expected error.
*/
void CTGlyphImageCache::WrongParameterL(MFontGlyphImageStorage* aGlyphStorage)
	{
	_LIT(KTestName, "GlyphCache_WrongParameters"); 
	if(!iRunningOomTests)
		{
		INFO_PRINTF1(KTestName);
		}
	
	aGlyphStorage->CleanGlyphImageCache();

	TChar glyphCode = TChar(10);
	TInt fontId = 10;
	VGImage foregroundImg = VG_INVALID_HANDLE;
	VGImage shadowImg = VG_INVALID_HANDLE;
	VGImage outlineImg = VG_INVALID_HANDLE;
	TSize fontSize = TSize(10, 12);
	TUint8* buffer = (TUint8*)User::AllocL(fontSize.iWidth * fontSize.iHeight);
	CleanupStack::PushL(buffer);

	TGlyphBitmapType glyphType = EDefaultGlyphBitmap; // unsupported
	
	TInt res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, fontSize, &foregroundImg, &shadowImg, &outlineImg);
	TEST(res == KErrNotSupported);
	TEST(foregroundImg == VG_INVALID_HANDLE);
	TEST(shadowImg == VG_INVALID_HANDLE);
	TEST(outlineImg == VG_INVALID_HANDLE);
	
	glyphType = EMonochromeGlyphBitmap; // supported
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, TSize(12, 0), &foregroundImg, &shadowImg, &outlineImg);
	TEST(res == KErrArgument);
	res = aGlyphStorage->GlyphImage(fontId, glyphCode, glyphType, buffer, TSize(0, 12), &foregroundImg, &shadowImg, &outlineImg);
	TEST(res == KErrArgument);
	TEST(foregroundImg == VG_INVALID_HANDLE);
	TEST(shadowImg == VG_INVALID_HANDLE);
	TEST(outlineImg == VG_INVALID_HANDLE);

	CleanupStack::PopAndDestroy(buffer);
	}

TInt CTGlyphImageCache::GlyphImageSizeInByte(TGlyphBitmapType aGlyphType, const TSize& aSize) const
	{
	TInt glyphSizeInByte = 0;	
		//calculate the actual size of the glyph
	switch(aGlyphType)
		{
	case EMonochromeGlyphBitmap:
		glyphSizeInByte = (((aSize.iWidth + 31) / 32) << 2) * aSize.iHeight;
		break;
	case EAntiAliasedGlyphBitmap:
		glyphSizeInByte = aSize.iWidth * aSize.iHeight;
		break;
	case EFourColourBlendGlyphBitmap:
		glyphSizeInByte = aSize.iWidth * aSize.iHeight;
		//we have to allocate memory for shadow and outline bitmap as well
		glyphSizeInByte *= 3;
		break;
	default :
		break;
		}
	return glyphSizeInByte;
	}
/**
Draws the text on the screen according its direction and alignment.
*/	
void CTGlyphImageCache::DrawText(const TDesC& aText, const TPoint& aPt, DrawTextDirection aDirection, DrawTextAdjustment aDrawAdjustment, TBool aUpdateJustification)
	{
	const TSize size = iGdiTarget->SizeInPixels();
	TRect rect = size;
	
	if(aDirection == EDrawTextHorizontal)
		{
		switch(aDrawAdjustment)
			{
		case EDrawFromPoint:
			iGc->DrawText(aText, NULL, aPt);
			break;
		case EDrawInBox:
			iGc->SetOrigin(aPt);
			rect = TRect(-aPt, size);
			iGc->DrawText(aText, NULL, rect);
			break;
		case EDrawRightAlignment:
			rect.iTl = aPt;
			rect.iBr.iX -= 30;
			iGc->DrawText(aText, NULL, rect, 0, DirectGdi::ERight);
			break;
			}
		}
	else if ((aDirection == EDrawTextVerticalDown) || (aDirection == EDrawTextVerticalUp))
		{
		TBool up = (aDirection==EDrawTextVerticalUp);
		switch(aDrawAdjustment)
			{
		case EDrawFromPoint:
			iGc->DrawTextVertical(aText, NULL, aPt, up);
			break;
		case EDrawInBox:
			iGc->SetOrigin(aPt);
			rect = TRect(-aPt, size);
			if(aUpdateJustification)
				{
				iGc->SetCharJustification(80, aText.Length());
				iGc->UpdateJustificationVertical(aText, NULL, up);
				}

			iGc->DrawTextVertical(aText, NULL, rect, up);
			iGc->SetOrigin(TPoint());
			break;
		case EDrawRightAlignment:
			rect.iTl = aPt;
			rect.iBr.iY -= 30;
			iGc->DrawTextVertical(aText, NULL, rect, 0, DirectGdi::ERight);
			break;
			}
		}
	}

/**
Overrides of base class virtual
@leave Gets system wide error code
@return - TVerdict code
*/
TVerdict CTGlyphImageCache::doTestStepPreambleL()
	{			
	CTDirectGdiStepBase::doTestStepPreambleL();
	return TestStepResult();
	}
	
/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/	
TVerdict CTGlyphImageCache::doTestStepL()
	{		
	// Test for each pixel format
	TInt maxPixelFormat =  iTargetPixelFormatArray.Count() - 1;
	for(TInt targetPixelFormatIndex = maxPixelFormat; targetPixelFormatIndex >= 0 ; targetPixelFormatIndex--)
		{
		iTestParams.iTargetPixelFormat = iTargetPixelFormatArray[targetPixelFormatIndex];
		SetTargetL(iTestParams.iTargetPixelFormat, EOneContextOneTarget, KGlyphCacheWindowSize);

		if(maxPixelFormat == targetPixelFormatIndex)
			{
			iMonotypeFont = DetectMonotypeFontL();
			
			if(!iMonotypeFont)
				{
				WARN_PRINTF1(_L("!! Due to running the test in environment without monotype fonts, some test cases will not \
be executed! To set up environment with monotype font support, launch ityperast.cmd \
from epoc32\\winscw\\c\\, and then follow the instruction on the screen!!"));
				}
			else
				{
				INFO_PRINTF1(_L("Monotype fonts have been detected"));
				}	
			}
		
		RunTestsL();
		if(targetPixelFormatIndex == 0)
			{
			RunOomTestsL();
			}
		}
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
Overrides of base class pure virtual function
Lists the tests to be run
*/
void CTGlyphImageCache::RunTestsL()
	{
	MFontGlyphImageStorage* glyphStorage = NULL;
	TBool testImageCache = (iUseDirectGdi && !iUseSwDirectGdi);
	if(testImageCache)
		{
		TInt err = iGc->GetInterface(TUid::Uid(KDirectGdiGetGlyphStorageUid), (TAny*&) glyphStorage);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Error while retrieving Glyph Storage Interface, err = %d"), err);
			TESTNOERRORL(err);
			}
		User::LeaveIfNull(glyphStorage);
		CleanupStack::PushL(TCleanupItem(CleanCache, glyphStorage));
		glyphStorage->CleanGlyphImageCache();
		}

	if(testImageCache)
		{
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0001"));
		TestRetrieveEntryL(glyphStorage, EAntiAliasedGlyphBitmap);
		RecordTestResultL();
		glyphStorage->CleanGlyphImageCache();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0001"));
		TestRetrieveEntryL(glyphStorage, EFourColourBlendGlyphBitmap);
		RecordTestResultL();
		glyphStorage->CleanGlyphImageCache();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0001"));
		TestRetrieveEntryL(glyphStorage, EMonochromeGlyphBitmap);
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0006"));
		WrongParameterL(glyphStorage);
		RecordTestResultL();
		SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0005"));
		FontListIdOrderL(glyphStorage);
		RecordTestResultL();
		glyphStorage->CleanGlyphImageCache();
		}

	if(!iRunningOomTests)//fonts are cached in the array, where memory won't be freed even if the font is released
		{
		if(testImageCache)
			{
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0004"));
			FillCacheL(glyphStorage);
			RecordTestResultL();
			SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0003"));
			FillUpCacheL(glyphStorage);
			RecordTestResultL();
			glyphStorage->CleanGlyphImageCache();
			}
		for(TInt ii = EDrawTextHorizontal; ii < EDrawTextLast; ii++)
			{
			for(TInt kk = EDrawFromPoint; kk <= EDrawInBox; kk++)
				{
				DrawTextDirection direction = (DrawTextDirection) ii;
				DrawTextAdjustment adjustment = (DrawTextAdjustment) kk;
		
				SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0002"));
				TestDrawGlyphL(glyphStorage, EMonochromeGlyphBitmap, direction, adjustment, EFalse);
				RecordTestResultL();
				SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0002"));
				TestDrawGlyphL(glyphStorage, EMonochromeGlyphBitmap, direction, adjustment, ETrue);
				RecordTestResultL();
				SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0002"));
				TestDrawGlyphL(glyphStorage, EAntiAliasedGlyphBitmap, direction, adjustment, EFalse);
				RecordTestResultL();
				SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0002"));
				TestDrawGlyphL(glyphStorage, EAntiAliasedGlyphBitmap, direction, adjustment, ETrue);
				RecordTestResultL();
				
				if(iMonotypeFont)
					{
					SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0002"));
					TestDrawGlyphL(glyphStorage, EFourColourBlendGlyphBitmap, direction, adjustment, EFalse);
					RecordTestResultL();
					SetTestStepID(_L("GRAPHICS-DIRECTGDI-GLYPHIMAGECACHE-0002"));
					TestDrawGlyphL(glyphStorage, EFourColourBlendGlyphBitmap, direction, adjustment, ETrue);
					RecordTestResultL();
					}
				}
			}
		}

	//next set of tests should start from clean cache to garantee consistent results
	if(testImageCache)
		{
		glyphStorage->CleanGlyphImageCache();
		CleanupStack::Pop(glyphStorage);
		}
	}

/**
See if monotype font is installed on the platform we are running on. Monotype font is required
to run the glyph tests that use the glyph bitmap type EFourColourBlendGlyphBitmap to draw
outlined and shadowed fonts.
@return ETrue if monotype is installed, EFalse if it is not installed.
 */
TBool CTGlyphImageCache::DetectMonotypeFontL()
	{
	CFont *font = NULL;
	_LIT(KTestFontTypefaceName,"DejaVu Sans Condensed");
	TFontSpec fspec(KTestFontTypefaceName, 12);
	fspec.iFontStyle.SetBitmapType(EFourColourBlendGlyphBitmap);

	fspec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
	fspec.iFontStyle.SetEffects(FontEffect::EOutline, ETrue);
	fspec.iFontStyle.SetEffects(FontEffect::EDropShadow, ETrue);

	User::LeaveIfError(iFontStore->GetNearestFontToDesignHeightInPixels((CFont*&) font, fspec));

	TFontSpec fontSpec = font->FontSpecInTwips();
	TGlyphBitmapType glyphBitmapType = fontSpec.iFontStyle.BitmapType();
	iFontStore->ReleaseFont(font);

	return (EFourColourBlendGlyphBitmap==glyphBitmapType);
	}
