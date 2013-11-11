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

#include "TGraphicsContext.h"
#include "TTYPES.H"


CTestTypefaceStore::CTestTypefaceStore(CTTypes* aTest) :
	iTest(aTest)
	{
	}

void CTestTypefaceStore::ConstructTestL()
	{
	ConstructL();
	}

TInt CTestTypefaceStore::AccessCount(TInt aIndex)
	{
	return(iFontAccess->At(aIndex).iAccessCount);
	}

TInt CTestTypefaceStore::Count()
	{
	return(iFontAccess->Count());
	}

void CTestTypefaceStore::AddFont(CFont* aFont)
	{
	TRAPD(ret,AddFontL(aFont));
	iTest->TEST2(ret, KErrNone);
	}

void CTestTypefaceStore::OpenFont(CFont* aFont)
	{
	TInt numfonts=Count();
	for(TInt count=0;count<numfonts;count++)
		{
		if(iFontAccess->At(count).iFont==aFont)
			{
			iFontAccess->At(count).iAccessCount++;
			return;
			}
		}
	}

TestTFStore::TestTFStore(CTTypes* aTest) :
	iTTFStore(aTest),
	iTest(aTest)
	{
	
	}

/** 
	TestTFStore::Test
	
	Method to test the functionality associated with CTypeFaceStore (abstract base) class
	The implementation is contained within CTestTypeFaceStore
	Called from the TTypes test script
*/
void TestTFStore::Test()
	{
	CFont* f1=new CTestFont;
	CFont* f2=new CTestFont;
	CFont* f3=new CTestFont;
	CFont* f4=new CTestFont;
	CFont* f5=new CTestFont;

	TRAPD(errCode, iTTFStore.ConstructTestL());
	iTest->TEST2(errCode, KErrNone);
	iTTFStore.AddFont(f1);
	iTest->TEST(iTTFStore.Count()==1);
	iTest->TEST(iTTFStore.AccessCount(0)==1);
	iTTFStore.AddFont(f2);
	iTest->TEST(iTTFStore.Count()==2);
	iTest->TEST(iTTFStore.AccessCount(0)==1);
	iTest->TEST(iTTFStore.AccessCount(1)==1);
	iTTFStore.AddFont(f3);
	iTest->TEST(iTTFStore.Count()==3);
	iTest->TEST(iTTFStore.AccessCount(0)==1);
	iTest->TEST(iTTFStore.AccessCount(1)==1);
	iTest->TEST(iTTFStore.AccessCount(2)==1);
	iTTFStore.AddFont(f4);
	iTest->TEST(iTTFStore.Count()==4);
	iTest->TEST(iTTFStore.AccessCount(0)==1);
	iTest->TEST(iTTFStore.AccessCount(1)==1);
	iTest->TEST(iTTFStore.AccessCount(2)==1);
	iTest->TEST(iTTFStore.AccessCount(3)==1);
	iTTFStore.OpenFont(f1);
	iTest->TEST(iTTFStore.Count()==4);
	iTest->TEST(iTTFStore.AccessCount(0)==2);
	iTest->TEST(iTTFStore.AccessCount(1)==1);
	iTest->TEST(iTTFStore.AccessCount(2)==1);
	iTest->TEST(iTTFStore.AccessCount(3)==1);
	iTTFStore.OpenFont(f4);
	iTest->TEST(iTTFStore.Count()==4);
	iTest->TEST(iTTFStore.AccessCount(0)==2);
	iTest->TEST(iTTFStore.AccessCount(1)==1);
	iTest->TEST(iTTFStore.AccessCount(2)==1);
	iTest->TEST(iTTFStore.AccessCount(3)==2);
	iTTFStore.AddFont(f5);
	iTest->TEST(iTTFStore.Count()==5);
	iTest->TEST(iTTFStore.AccessCount(0)==2);
	iTest->TEST(iTTFStore.AccessCount(1)==1);
	iTest->TEST(iTTFStore.AccessCount(2)==1);
	iTest->TEST(iTTFStore.AccessCount(3)==2);
	iTest->TEST(iTTFStore.AccessCount(4)==1);
	iTTFStore.OpenFont(f5);
	iTest->TEST(iTTFStore.Count()==5);
	iTest->TEST(iTTFStore.AccessCount(0)==2);
	iTest->TEST(iTTFStore.AccessCount(1)==1);
	iTest->TEST(iTTFStore.AccessCount(2)==1);
	iTest->TEST(iTTFStore.AccessCount(3)==2);
	iTest->TEST(iTTFStore.AccessCount(4)==2);
	iTTFStore.OpenFont(f4);
	iTest->TEST(iTTFStore.Count()==5);
	iTest->TEST(iTTFStore.AccessCount(0)==2);
	iTest->TEST(iTTFStore.AccessCount(1)==1);
	iTest->TEST(iTTFStore.AccessCount(2)==1);
	iTest->TEST(iTTFStore.AccessCount(3)==3);
	iTest->TEST(iTTFStore.AccessCount(4)==2);
//
	iTTFStore.ReleaseFont(f1);
	iTTFStore.ReleaseFont(f2);
	iTTFStore.ReleaseFont(f4);
	iTTFStore.ReleaseFont(f4);
	iTTFStore.ReleaseFont(f3);
	iTTFStore.ReleaseFont(f5);
	iTTFStore.ReleaseFont(f4);
	iTTFStore.ReleaseFont(f1);
	iTTFStore.ReleaseFont(f5);
	}

TestFontCache::TestFontCache(CTTypes* aTest):
	iTest(aTest)
	{
	
	}

/** 
	TestFontCache::Test()
	
	Method to test the functionality within the CFontCache class
	Called from the TTypes test script
*/
void TestFontCache::Test()
	{
	TRAPD(ret,TestL());
	iTest->TEST2(ret, KErrNone);
	}

/** 
	TestFontCache::TestL()
	
	Method to test the functionality within the CFontCache class
	Test the creation of CFontCache instance & the adding/removal of CFont fonts to the cache
	Confirm no memory leaks occur
	
*/
void TestFontCache::TestL()
	{
	User::Heap().__DbgMarkStart();

	iCache = new(ELeave) CFontCache;
	CFont* font = NULL;
	CFont* discard = NULL;
	TFontSpec fs;

	for (TInt count1 = 0; count1 < KMaxFontCacheEntries; count1++)
		{
		font = (CFont*)(count1 + 1);
		discard = iCache->AddEntryL(font,fs);
		iTest->TEST(discard==NULL);
		}

	font = (CFont*)KMaxFontCacheEntries;
	discard = iCache->AddEntryL(font,fs);
	iTest->TEST(discard==(CFont*)0x1);
	delete iCache;
	iCache = NULL;

	iCache = new(ELeave) CFontCache;

	for (TInt count2 = 0; count2 <= KMaxFontCacheEntries; count2++)
		{
		iCache->AddEntryL(NULL,fs);
		iCache->RemoveFirstEntry();
		}

	delete iCache;

	User::Heap().__DbgMarkEnd(0);
	}
