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

#include "TTYPES.H"

#define UNUSED_VAR(a) a = a

TestPalette::TestPalette(CTTypes* aTest):
	iPalette(NULL), iPalette2(NULL),
	iTest(aTest)
	{}

TestPalette::~TestPalette()
	{
	}

/** 
	TestPalette::Test
	
	Method to test the functionality within the CPalette class
	Called from the TTypes test script
*/
void TestPalette::Test()
	{
	_LIT(general,"General");
	iTest->INFO_PRINTF1(general);
	TestGeneral();
	_LIT(defaults,"Defaults");
	iTest->INFO_PRINTF1(defaults);
	//TestDefaults();
	_LIT(getData,"Get Data");
	iTest->INFO_PRINTF1(getData);
	//TestGetPtr();
	}

/** 
	TestPalette::TestGeneral
	
	Spawn a new CPalette object & test basic calls to retrieve number of entries, add colours, etc
*/
void TestPalette::TestGeneral()
	{
	const TInt numcolors=50;
	TRAPD(ret,iPalette=CPalette::NewL(numcolors));
    UNUSED_VAR(ret);
	iTest->TEST(iPalette->Entries()==numcolors);
	for(TInt colcount=0;colcount<numcolors;colcount++)
		{
		TRgb color(colcount,0,0);
		iPalette->SetEntry(colcount,color);
		iTest->TEST(iPalette->GetEntry(colcount)==color);
		}
	for(TInt checkcount=0;checkcount<numcolors;checkcount++)
		{
		TRgb color(checkcount,0,0);
		iTest->TEST(iPalette->GetEntry(checkcount)==color);
		}
	iPalette->Clear();
	TRgb black(0,0,0);
	for(TInt clearcount=0;clearcount<numcolors;clearcount++)
		iTest->TEST(iPalette->GetEntry(clearcount)==black);
	for(TInt nearcount=0;nearcount<5;nearcount++)
		{
		TRgb color(nearcount*60,0,0);
		iPalette->SetEntry(nearcount,color);
		iTest->TEST(iPalette->GetEntry(nearcount)==color);
		}
	for(TInt approxcount=0;approxcount<5;approxcount++)
		{
		TRgb color(approxcount*60,0,0);
		TRgb approxl(approxcount*59,0,0);
		TRgb approxh(approxcount*61,0,0);
		TRgb nearest=iPalette->NearestEntry(approxl);
		iTest->TEST(nearest==color);
		nearest=iPalette->NearestEntry(approxh);
		iTest->TEST(nearest==color);
		}
	delete iPalette;
	iPalette = NULL;
	}

/** 
	TestPalette::TestDefaults
	
	Test creation of default colour palettes for supported/un-supported display modes
	Called from the TTypes test script
*/
void TestPalette::TestDefaults()
	{
	TRAPD(ret,iPalette=CPalette::NewDefaultL(ENone));
	iTest->TEST2(ret, KErrNotSupported);
	TRAP(ret,iPalette=CPalette::NewDefaultL(EColor16M));
	iTest->TEST2(ret, KErrNotSupported);
	TRAP(ret,iPalette=CPalette::NewDefaultL(EColor16MU));
	iTest->TEST2(ret, KErrNotSupported);
	TRAP(ret,iPalette=CPalette::NewDefaultL(ERgb));
	iTest->TEST2(ret, KErrNotSupported);
	TRAP(ret,iPalette=CPalette::NewDefaultL(EGray2));
	iTest->TEST(iPalette->GetEntry(0)==TRgb::Gray2(0));
	iTest->TEST(iPalette->GetEntry(1)==TRgb::Gray2(1));
	iTest->TEST(iPalette->Entries()==2);
	delete iPalette;
	TRAP(ret,iPalette=CPalette::NewDefaultL(EGray4));
	for(TInt g4=0;g4<4;g4++)
		iTest->TEST(iPalette->GetEntry(g4)==TRgb::Gray4(g4));
	iTest->TEST(iPalette->Entries()==4);
	delete iPalette;
	TRAP(ret,iPalette=CPalette::NewDefaultL(EGray16));
	for(TInt g16=0;g16<16;g16++)
		iTest->TEST(iPalette->GetEntry(g16)==TRgb::Gray16(g16));
	iTest->TEST(iPalette->Entries()==16);
	delete iPalette;
	TRAP(ret,iPalette=CPalette::NewDefaultL(EGray256));
	for(TInt g256=0;g256<256;g256++)
		iTest->TEST(iPalette->GetEntry(g256)==TRgb::Gray256(g256));
	iTest->TEST(iPalette->Entries()==256);
	delete iPalette;
	TRAP(ret,iPalette=CPalette::NewDefaultL(EColor16));
	for (TInt c16 = 0; c16 < 16; c16++)
		iTest->TEST(iPalette->GetEntry(c16)==TRgb::Color16(c16));
	iTest->TEST(iPalette->Entries()==16);
	delete iPalette;
	TRAP(ret,iPalette=CPalette::NewDefaultL(EColor256));
	for (TInt c256 = 0; c256 < 256; c256++)
		iTest->TEST(iPalette->GetEntry(c256)==TRgb::Color256(c256));
	iTest->TEST(iPalette->Entries()==256);
	delete iPalette;
	iPalette = NULL;
	}

/** 
	TestPalette::TestGetPtr
	
	Test the GetDataPtr() method in CPalette, to retrieve palette entries for a specified colour
	
	Called from the TTypes test script
*/
void TestPalette::TestGetPtr()
	{
	TRAPD(ret,iPalette2=CTestPalette::NewL());
    UNUSED_VAR(ret);
	iTest->TEST(iPalette2->TestGetPtr());
	TInt colorNum;
	for(colorNum=0;colorNum<CTestPalette::eNumColors;++colorNum)
		{
		TRgb color(colorNum,colorNum,colorNum);
		iPalette2->SetEntry(colorNum,color);
		}
	TPtr8 ptr(NULL,0);
	iPalette2->GetDataPtr(0,CTestPalette::eNumColors,ptr);
	const TUint8 maxValue=CTestPalette::eNumColors-1;
	TInt ii;
	for(ii=0;ii<4*CTestPalette::eNumColors;++ii)
		{
		TUint8 value=ptr[ii];
		if (value!=0 && ((ii + 1) % 4)) //skip alpha channel
			ptr[ii]=STATIC_CAST(TUint8,maxValue-value);
		}	
	for(colorNum=1;colorNum<maxValue;++colorNum)
		iTest->TEST(iPalette2->GetEntry(colorNum)==TRgb::Gray256(maxValue-colorNum));
	delete iPalette2;
	iPalette2 = NULL;
	}

/** 
	CTestPalette::TestGetPtr
	
	Part of TestPalette::TestGetPtr, retrieves a descriptor from CPalette containing entries for a given colour 
	& checks parameters are legal
	Returns boolean indiciating whether the GetDataPtr was successful or not
*/
TBool CTestPalette::TestGetPtr()
	{
	TPtr8 ptr(NULL,0);
	TUint32* data=REINTERPRET_CAST(TUint32*,iArray);
	TInt start;
	TInt length;
	for(start=0;start<eNumColors;++start)
		{
		for(length=1;length<eNumColors-start;++length)
			{
			TInt len=sizeof(TRgb)*length;
			GetDataPtr(start,length,ptr);
			if (&ptr[0]!=REINTERPRET_CAST(TUint8*,data+start) || ptr.Length()!=len || ptr.MaxLength()!=len)
				return EFalse;
			}
		}
	return ETrue;
	}
