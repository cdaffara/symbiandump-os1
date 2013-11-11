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

#include <e32test.h>
#include <s32file.h>
#include "TTYPES.H"

TestPicture::TestPicture(CTTypes* aTest):
	iPicture((CTestPicture*)User::Alloc(sizeof(CTestPicture))),
	iTest(aTest)
	{
	iTest->TEST(iPicture!=NULL);
	new(iPicture) CTestPicture;
	}

TestPicture::~TestPicture()
	{
	delete iPicture;
	}

/** 
	TestPicture::Test
	
	Method to test the functionality of the TPictureCapability class (Scalability, Croppable, GetSize, NativePixelSize)
	Called from the TTypes test script
*/
void TestPicture::Test()
	{
	MTestMap map;
//
	TPictureCapability pc(TPictureCapability::ENotScaleable,EFalse);
	pc=iPicture->Capability();
	iTest->TEST(pc.iScalingType==TPictureCapability::ENotScaleable);
	iTest->TEST(!pc.iIsCroppable);
	iTest->TEST(iPicture->ScaleFactorWidth()==1000);
	iTest->TEST(iPicture->ScaleFactorHeight()==1000);
	TSize size;
	iPicture->GetSizeInTwips(size);
	iTest->TEST(size.iWidth==100);
	iTest->TEST(size.iHeight==100);
	iPicture->GetSizeInPixels(&map,size);
	iTest->TEST(size.iWidth==100);
	iTest->TEST(size.iHeight==100);
// SetSizeInPixels
	iPicture->SetSizeInPixels(&map,TSize(50,50));
	iPicture->GetSizeInTwips(size);
	iTest->TEST(size.iWidth==100);
	iTest->TEST(size.iHeight==100);

	CTestPicture2 picture;
	iTest->TEST(!picture.NativePixelSize(size));

	iPicture->iSize = TSize(1000,2000);
	CPicture*pic=iPicture; // don't need this but feel more comfident
                           // seeing the virtual func work through the base class
	iTest->TEST(pic->NativePixelSize(size));
	iTest->TEST(size.iWidth==1000);
	iTest->TEST(size.iHeight==2000);

	}

