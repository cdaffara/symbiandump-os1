// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <palette.h>
#include "TRGB.H"


CTRgb::CTRgb(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	INFO_PRINTF1(_L("Testing TRgb colour functions"));
	}

void CTRgb::RunTestCaseL(TInt aCurTestCase)
	{
	((CTRgbStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0001
*/
        	((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0001"));
		TestGray2();
		break;
	case 2:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0002
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0002"));
		TestGray4();
		break;
	case 3:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0003
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0003"));
		TestGray16();
		break;
	case 4:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0004
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0004"));
		TestGray256();
		break;
	case 5:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0005
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0005"));
		TestColor16();
		break;
	case 6:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0006
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0006"));
		TestColor256();
		break;
	case 7:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0007
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0007"));
		TestColor4K();
		break;
	case 8:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0008
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0008"));
		TestColor64K();
		break;
	case 9:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0009
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0009"));
		TestColor16M();
		break;
	case 10:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0010
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0010"));
		TestColor16MU();
		break;
	case 11:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0011
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0011"));
		TestColor16MA();
		break;
	case 12:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0012
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0012"));
		TestColor256Util();
		break;
	case 13:
/**
   @SYMTestCaseID          	GRAPHICS-GDI-RGB-0013
*/
		((CTRgbStep*)iStep)->SetTestStepID(_L("GRAPHICS-GDI-RGB-0013"));
		TestColor16MAP();
		break;
	case 14:
		((CTRgbStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTRgbStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	    ((CTRgbStep*)iStep)->RecordTestResultL();
	}

/**
	Test Gray2 colour set

	Cycle through each Gray2 colour & compare the grayscale value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Gray2 TRgb value with a subset of the Gray256 colour-set.
 
    Expect the conversion from index value to grayscale colour value & back again produces identical value.
	Expect the Gray2 rgb colour set forms a subset of the Gray256 colour rgb set
*/		
void CTRgb::TestGray2()
	{
	INFO_PRINTF1(_L("Gray2"));

	for (TInt index = 0; index < 2; index++)
		{
		TRgb color = TRgb::Gray2(index);
		TEST(color.Gray2() == index);
		}

	for (TUint32 value = 0; value <= 0x00ffffff; value += 31)
		{
		TRgb color(value);
		TEST(color.Gray2() == color.Gray256() / 128);
		}
	}

/**
	Cycle through each Gray4 colour & compare the grayscale value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Gray4 TRgb colour value with a subset of the Gray256 colour-set.
 
   	Expect conversion from index value to grayscale colour value & back again produces identical value.
	Expect the Gray4 rgb colour set forms a subset of the Gray256 colour rgb set
*/		
void CTRgb::TestGray4()
	{
	INFO_PRINTF1(_L("Gray4"));

	for (TInt index = 0; index < 4; index++)
		{
		TRgb color = TRgb::Gray4(index);
		TEST(color.Gray4() == index);
		}

	for (TUint32 value = 0; value <= 0x00ffffff; value += 31)
		{
		TRgb color(value);
		TEST(color.Gray4() == color.Gray256() / 64);
		}
	}

/**
   	Cycle through each Gray16 colour & compare the grayscale value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Gray16 TRgb value with a subset of the Gray256 colour-set.
 
   	Expect the conversion from index value to grayscale colour value & back again produces identical value.
	Expect the Gray16 rgb colour set forms a subset of the Gray256 colour rgb set
*/		
void CTRgb::TestGray16()
	{
	INFO_PRINTF1(_L("Gray16"));

	for (TInt index = 0; index < 16; index++)
		{
		TRgb color = TRgb::Gray16(index);
		TEST(color.Gray16() == index);
		}

	for (TUint32 value = 0; value <= 0x00ffffff; value += 31)
		{
		TRgb color(value);
		TEST(color.Gray16() == color.Gray256() / 16);
		}
	}

/**
   	Cycle through each Gray256 colour & compare the grayscale value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Gray256 TRgb colour value with the value produced by generic algorithm
 
   	Expect the conversion from index value to grayscale colour value & back again produces identical value.
	Confirm the algorithm used to produce Gray256 colour set 
*/		
void CTRgb::TestGray256()
	{
	INFO_PRINTF1(_L("Gray256"));

	for (TInt index = 0; index < 256; index++)
		{
		TRgb color = TRgb::Gray256(index);
		TEST(color.Gray256() == index);
		}

	for (TUint32 value = 0; value <= 0x00ffffff; value += 31)
		{
		TRgb color(value);
		TInt algGray256 = (((value & 0xff) * 2) + (((value >> 8) & 0xff) * 5) + ((value >> 16) & 0xff)) / 8;
		TEST(color.Gray256() == algGray256);
		}
	}

/**
   	Test 16 Colour colour set

   	Cycle through each Color16 colour & test the value used to create the colour
	against the index value retrieved from the colour palette.
	Compare the rgb value for each Color16 colour matches that returned by the DynamicPalette colour palette
 
   	Expect the RGB colour value returned matches the 16colour palette
*/		
void CTRgb::TestColor16()
	{
	INFO_PRINTF1(_L("Color16"));

	for (TInt index = 0; index < 16; index++)
		{
		TRgb color = TRgb::Color16(index);
		TEST(color.Color16() == index);
		TEST(TRgb::Color16(index) == TRgb(DynamicPalette::Color16array()[index]));
		}
	}

/**
	Test 256 colour set

    Cycle through each Color256 colour & test the value used to create the colour
	against the index value retrieved from the colour palette.
	Compare the rgb value for each Color256 colour against the rgb value returned by the DynamicPalette colour palette
	Cycle through each Color256 colour & confirm it matches the Netscape Colour Cube
							
   	Expect the RGB colour returned matches the 256 colour palette
*/
void CTRgb::TestColor256()
	{
	INFO_PRINTF1(_L("Color256"));

	const TInt mainValues[6] = {0x00, 0x33, 0x66, 0x99, 0xcc, 0xff };
	const TInt lowerValues[5] = {0x11, 0x22, 0x44, 0x55, 0x77 };
	const TInt upperValues[5] = {0x88, 0xaa, 0xbb, 0xdd, 0xee };

	TInt index;
	for (index = 0; index < 256; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Color256() == index);
		TEST(TRgb::Color256(index) == TRgb(DynamicPalette::DefaultColor256Util()->iColorTable[index]));
		}

	for (index = 0; index < 108; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == mainValues[index % 6]);
		TEST(color.Green() == mainValues[(index / 6) % 6]);
		TEST(color.Blue() == mainValues[(index / 36) % 6]);
		}
	for (; index < 113; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == color.Green());
		TEST(color.Green() == color.Blue());
		TEST(color.Blue() == lowerValues[index - 108]);
		}
	for (; index < 118; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == lowerValues[index - 113]);
		TEST(color.Green() == 0);
		TEST(color.Blue() == 0);
		}
	for (; index < 123; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == 0);
		TEST(color.Green() == lowerValues[index - 118]);
		TEST(color.Blue() == 0);
		}
	for (; index < 128; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == 0);
		TEST(color.Green() == 0);
		TEST(color.Blue() == lowerValues[index - 123]);
		}
	for (; index < 133; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == 0);
		TEST(color.Green() == 0);
		TEST(color.Blue() == upperValues[index - 128]);
		}
	for (; index < 138; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == 0);
		TEST(color.Green() == upperValues[index - 133]);
		TEST(color.Blue() == 0);
		}
	for (; index < 143; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == upperValues[index - 138]);
		TEST(color.Green() == 0);
		TEST(color.Blue() == 0);
		}
	for (; index < 148; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == color.Green());
		TEST(color.Green() == color.Blue());
		TEST(color.Blue() == upperValues[index - 143]);
		}
	for (; index < 256; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(color.Red() == mainValues[(index - 40) % 6]);
		TEST(color.Green() == mainValues[((index - 40) / 6) % 6]);
		TEST(color.Blue() == mainValues[((index - 40) / 36) % 6]);
		}
	}

/**
	Test 4096 colour set

   	Cycle through each Color4K colour & compare the colorscale value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Color4K TRgb value against that produced by the algorithm
 
   	Confirm the conversion from index value to 4096 colour value & back again produces identical value.
	Confirm the algorithm used to produce 4096 colour set
*/	
void CTRgb::TestColor4K()
	{
	INFO_PRINTF1(_L("Color4K"));

	for (TInt index = 0; index < 4096; index++)
		{
		TRgb color = TRgb::Color4K(index);
		TEST(color.Color4K() == index);
		}

	for (TUint32 value = 0; value <= 0x00ffffff; value += 31)
		{
		TRgb color(value);
		TInt color4K = ((value & 0xf00000) >> 20) | ((value & 0x00f000) >> 8) | ((value & 0x0000f0) << 4);
		TEST(color.Color4K() == color4K);
		}
	}

/**
   	Test 64K colour set

   	Cycle through each Color64K colour & compare the TRgb value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Color64K TRgb value against that produced by the algorithm
 
   	Confirm the conversion from index value to 64K colour value & back again produces identical value.
	Confirm the algorithm used to produce 64K colour set
*/	
void CTRgb::TestColor64K()
	{
	INFO_PRINTF1(_L("Color64K"));

	for (TInt index = 0; index < 65536; index++)
		{
		TRgb color = TRgb::Color64K(index);
		TEST(color.Color64K() == index);
		}

	for (TUint32 value = 0; value <= 0x00ffffff; value += 31)
		{
		TRgb color(value);
		TInt color64K = ((value & 0xf8) << 8) + ((value & 0xfc00) >> 5) + ((value & 0xf80000) >> 19);
		TEST(color.Color64K() == color64K);
		}
	}

/**
	Test 16M colour set

 	Cycle through each Color16M colour & compare the TRgb value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Color16M TRgb value against that produced by the algorithm
 
   	Confirm the conversion from index value to 16M colour value & back again produces identical value.
	Confirm the algorithm used to produce 16M colour set
*/	
void CTRgb::TestColor16M()
	{
	INFO_PRINTF1(_L("Color16M"));

	for (TUint32 value = 0; value <= 0x00ffffff; value += 31)
		{
		TRgb color(value);
		TInt color16M = ((value & 0xff0000) >> 16) | (value & 0x00ff00) | ((value & 0x0000ff) << 16);
		TRgb generatedColor = TRgb::Color16M(color16M);
		TEST(color == generatedColor);
		TEST(color.Color16M() == color16M);
		}
	}

/**
	Test 16MU colour set

   	Cycle through each Color16MU colour & compare the TRgb value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Color16MU TRgb value against that produced by the algorithm
 
   	Confirm the conversion from index value to 16MU colour value & back again produces identical value.
	Confirm the algorithm used to produce 16MU colour set
*/	
void CTRgb::TestColor16MU()
	{
	INFO_PRINTF1(_L("Color16MU"));

	for (TUint32 value = 0; value <= 0x00ffffff; value += 31)
		{
		TRgb color(value);
		TInt color16MU = ((value & 0xff0000) >> 16) | (value & 0x00ff00) | ((value & 0x0000ff) << 16);
		TRgb generatedColor = TRgb::Color16MU(color16MU);
		TEST(color == generatedColor);
		TEST(color.Color16MU() == color16MU);
		}
	}

/**
   	Test 16MA colour set

   	Cycle through each Color16MA colour & compare the TRgb value used to create the colour
	against the index value retrieved from the colour palette.
	Cycle through a series of RGB values & compare the Color16MA TRgb value against that produced by the algorithm
 
   	Confirm the conversion from index value to 16MA colour value & back again produces identical value.
	Confirm the algorithm used to produce 16MA colour set
*/
void CTRgb::TestColor16MA()
	{
	INFO_PRINTF1(_L("Color16MA"));

	for (TUint32 high = 0; high <= 0xffff; high += 51)
		for (TUint32 low = 0; low <= 0xffff; low += 51)
			{
			TUint32 value = (high << 16) + low; // '+' operator has higher precedance than '<<'  operator
			TRgb color(value);
			TInt color16MA = (0xff000000 - (value & 0xff000000)) | ((value & 0xff0000) >> 16) | (value & 0x00ff00) | ((value & 0x0000ff) << 16);
			TRgb generatedColor = TRgb::Color16MA(color16MA);
			TEST(color == generatedColor);
			TEST(color.Color16MA() == color16MA);
			}
	}

/**
   	Test TColor256Util

   	Test functionality contained within TColor256Util. 
							
   	Confirm TColor256Util converts correctly between TRgb values & the corresponding index in the colour palette
							
*/
void CTRgb::TestColor256Util()
	{
	INFO_PRINTF1(_L("TColor256Util"));

	__UHEAP_MARK;

	TColor256Util* util = new TColor256Util;
	CPalette* palette = NULL;
	TRAPD(err,palette = CPalette::NewDefaultL(EColor256));
	TEST(err==KErrNone);
	util->Construct(*palette);
	TEST(Mem::Compare((TUint8*)util,sizeof(TColor256Util),(TUint8*)DynamicPalette::DefaultColor256Util(),sizeof(TColor256Util))==0);

	TInt index;
	for (index = 0; index < 256; index++)
		{
		TRgb color = TRgb::Color256(index);
		TEST(util->Color256(index) == color);
		TEST(util->Color256(color) == index);
		}

	TRgb* rgbBuffer = new TRgb[256];
	TUint8* indexBuffer = new TUint8[256];
	for (index = 0; index < 256; index++)
		rgbBuffer[index] = TRgb::Color256(index);
	util->Color256(indexBuffer,rgbBuffer,256);
	for (index = 0; index < 256; index++)
		TEST(indexBuffer[index]==index);

	delete[] rgbBuffer;
	delete[] indexBuffer;
	delete palette;
	delete util;

	__UHEAP_MARKEND;
	}

/**
	Validate the PreMultiplied value and the Non PreMultiplied value with the expected values.
	@param aAlpha Alpha value of the color.
	@param aValue The value of the color channel(ie. one of Red,Green or Blue).
	@param aPreMulVal The PreMutiplied color value for aValue.
	@param aNonPreMulValue The Non PreMutiplied value for aValue
						   (i.e the value received by Non PreMutiplying aPreMulVal).

*/
void CTRgb::ValidatePMAndNPM(TInt aAlpha, TInt aValue, TInt aPreMulVal, TInt aNonPreMulValue)
	{
	TInt expPreMulValue = (aValue*(aAlpha+1))/256;
	TInt expNonPreMulValMin = (expPreMulValue * 255) / aAlpha;
	TInt expNonPreMulValMax = expNonPreMulValMin + 1;
	if (expNonPreMulValMax > 255)
		{
		expNonPreMulValMax = 255;
		}
	TEST(expPreMulValue == aPreMulVal);
	TEST(expNonPreMulValMin <= aNonPreMulValue && expNonPreMulValMax >= aNonPreMulValue);
	}

/**
   	DEF103742 - Test the PreMultiply and Non PreMultiply conversion.

   	Convert the color values into PreMultiplied color values and again back to
   	the Non PreMultiplied color values.
   	Compare the  converted values with the expected values to validate the functionality.
 
   	Confirm the PreMultiplied and Non PreMultiplied color values match with the expected values.
*/		
void CTRgb::TestColor16MAP()
	{
	INFO_PRINTF1(_L("Color16MAP"));
	for (TInt alpha = 0; alpha < 256; alpha += 51)
		{
		for (TUint32 value = 0; value <= 0x00ffffff; value += 0x1f1f)
			{
			TRgb color(value, alpha);
			TUint pmColor = color.Color16MAP();
			TRgb npmColor = TRgb::Color16MAP(pmColor);

			TInt pmAlpha = (pmColor & 0xFF000000) >> 24;

			// These really must be right!
			TEST(pmAlpha == alpha);
			TEST(npmColor.Alpha() == alpha);

			// These definitely ought to be right
			if (alpha == 0)	// Full transparency, expect black
				{
				TEST(pmColor == 0);
				TEST(npmColor.Internal() == 0);
				}
			else if (alpha == 255)	// Full opacity, expect roundtrip
				{
				TEST(pmColor == color.Internal());
				TEST(npmColor == color);
				}
			else
				{
				// Most awkward cases: semi-transparency.
				TInt pmRed   = (pmColor & 0x00FF0000) >> 16;
				TInt pmGreen = (pmColor & 0x0000FF00) >> 8;
				TInt pmBlue  = pmColor & 0xFF;
				ValidatePMAndNPM(alpha, color.Red(), pmRed, npmColor.Red());
				ValidatePMAndNPM(alpha, color.Green(), pmGreen, npmColor.Green());
				ValidatePMAndNPM(alpha, color.Blue(), pmBlue, npmColor.Blue());
				}
			}
		}
	}

//--------------
__CONSTRUCT_STEP__(Rgb)
