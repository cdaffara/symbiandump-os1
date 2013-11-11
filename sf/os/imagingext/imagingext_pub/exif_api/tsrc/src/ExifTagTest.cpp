/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: ExifLibTest
*
*/
#include "ExifLibTestInc.h"

// Exif.Tag.001
// Duplicate an existing tag, and get related information of the tag.
// Created tag instance is returned.
void CExifTagTest::ExifTag001L()
	{
	HBufC8* exif = TUtils::ReadFileL(iFs, KFullExif);
	CleanupStack::PushL(exif);
	CExifRead* read = CExifRead::NewL(*exif);
	if(!read)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(read);

	const CExifTag* tag = read->GetTagL(EIfd0, KIdImageDescription);
	CExifTag* copyTag = tag->DuplicateL();
	if(!copyTag)
		User::Leave(KErrGeneral);
	CleanupStack::PushL(copyTag);

	if(copyTag->TagInfo().iId != KIdImageDescription)
		User::Leave(KErrGeneral);

	if(copyTag->TagInfo().iDataType != CExifTag::ETagAscii)
		User::Leave(KErrGeneral);

	if(copyTag->TagInfo().iDataCount != tag->TagInfo().iDataCount)
		User::Leave(KErrGeneral);

	if(copyTag->Data().Length() != tag->TagInfo().iDataCount)
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(copyTag);
	CleanupStack::PopAndDestroy(read);
	CleanupStack::PopAndDestroy(exif);
	}


// Exif.Tag.MEM.002
// Test the behavior of the previous test cases in OOM situations.
// Successfully operates or leaves with OOM error, without any memory leaks.
void CExifTagTest::ExifTag002L()
	{
	// EXIF.TAG.001 
	TInt error = KErrGeneral;
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
	for(TInt i = 1; i < 50 && error != KErrNone; i++)
#else
	for(TInt i = 1; i < 10 && error != KErrNone; i++)
#endif
		{
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )
		__UHEAP_SETFAIL( RHeap::EFailNext, i );
#else
        __UHEAP_SETFAIL( RHeap::ERandom, i );
#endif
		TRAP(error, ExifTag001L());
		if(error)
			{
			if(error != KErrNoMemory)
				{
				__UHEAP_SETFAIL( RHeap::ENone, 0 );
				User::Leave(KErrGeneral);
				}
			}
		}
	__UHEAP_SETFAIL( RHeap::ENone, 0 );
	}
