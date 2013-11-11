// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// byte_pair.cpp
// 
//

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "h_utl.h"
#include "h_ver.h"
#include <stdio.h>

#include "byte_pair.h"

#define PAGE_SIZE 4096

typedef struct IndexTableItemTag
{
	TUint16 iSizeOfCompressedPageData;	// pointer to an array TUint16[NumberOfPages]
	TUint8 *iCompressedPageData;		// pointer to an array TUint8*. Each elemet of 
										// this array point a compressed Page data	
}IndexTableItem;


typedef struct IndexTableHeaderTag
{
	TInt	iSizeOfData;					// Includes the index and compressed pages
	TInt	iDecompressedSize;
	TUint16	iNumberOfPages;
} IndexTableHeader;


class CBytePairCompressedImage
{
	public:
		static CBytePairCompressedImage* NewLC(TUint16 aNumberOfPages, TInt aSize);
		
		~CBytePairCompressedImage();
		
		void AddPage(TUint16 aPageNum, TUint8 * aPageData, TUint16 aPageSize, CBytePair *aBPE);
		int  GetPage(TUint16 aPageNum, TUint8 * aPageData, CBytePair *aBPE);
		void WriteOutTable(ostream &os);
		int  ReadInTable(istream &is, TUint & aNumberOfPages);
	
	private:
		TInt ConstructL( TUint16 aNumberOfPages, TInt aSize);
		CBytePairCompressedImage();
		
	private:
		IndexTableHeader 	iHeader;
		IndexTableItem*		iPages;
		TUint8* 			iOutBuffer; 
		
};


CBytePairCompressedImage::CBytePairCompressedImage()
{
	
}


CBytePairCompressedImage* CBytePairCompressedImage::NewLC(TUint16 aNumberOfPages, TInt aSize)
{
	CBytePairCompressedImage* self = new CBytePairCompressedImage;
	if( NULL == self)
	{
		return self;
	}
	
	if( KErrNone == self->ConstructL(aNumberOfPages, aSize))
	{
		return self;
	}
	return NULL;
}


TInt CBytePairCompressedImage::ConstructL(TUint16 aNumberOfPages, TInt aSize)
{
#ifdef __DEBUG_MSG__
	Print(EWarning,"Start ofCBytePairCompressedImage::ConstructL(%d, %d)\n", aNumberOfPages, aSize );
#endif

	iHeader.iNumberOfPages = aNumberOfPages;
	iHeader.iDecompressedSize = aSize;
	
	if( 0 != aNumberOfPages)
	{
		iPages = (IndexTableItem *) calloc(aNumberOfPages, sizeof(IndexTableItem));
		
		if( NULL == iPages )
		{
			return KErrNoMemory;
		}
	}
		
	iHeader.iSizeOfData = 	sizeof(iHeader.iSizeOfData) + 
							sizeof(iHeader.iDecompressedSize) + 
							sizeof(iHeader.iNumberOfPages) + 
							aNumberOfPages * sizeof(TUint16);
	
	iOutBuffer = (TUint8 *) calloc(4 * PAGE_SIZE, sizeof(TUint8) ); 
	
	if ( NULL == iOutBuffer)
	{
		return KErrNoMemory;
	}
	return KErrNone;
} // End of ConstructL()

CBytePairCompressedImage::~CBytePairCompressedImage()
{
#ifdef __DEBUG_MSG__
	Print(EWarning,"Destructor calling.");
#endif

	for( int i = 0; i < iHeader.iNumberOfPages; i++)
	{
		free(iPages[i].iCompressedPageData);
		iPages[i].iCompressedPageData = NULL;
	}
	
	free( iPages );
	iPages = NULL;
	
	free( iOutBuffer );
	iOutBuffer = NULL;
}


void CBytePairCompressedImage::AddPage(TUint16 aPageNum, TUint8 * aPageData, TUint16 aPageSize, CBytePair *aBPE)
{
#ifdef __DEBUG_MSG__
	Print(EWarning,"Start of AddPage(aPageNum:%d, ,aPageSize:%d)\n",aPageNum, aPageSize );
#endif

#ifdef __TEST_ONLY__

	iPages[aPageNum].iSizeOfCompressedPageData = 2;

	iPages[aPageNum].iCompressedPageData = (TUint8 *) calloc(iPages[aPageNum].iSizeOfCompressedPageData, sizeof(TUint8) );	
	
	memcpy(iPages[aPageNum].iCompressedPageData, (TUint8 *) &aPageNum, iPages[aPageNum].iSizeOfCompressedPageData);
	
	
#else

	TUint16 compressedSize = (TUint16) aBPE->Compress(iOutBuffer,aPageData,aPageSize);
	iPages[aPageNum].iSizeOfCompressedPageData = compressedSize;

#ifdef __DEBUG_MSG__
	Print(EWarning,"Compressed page size:%d\n", iPages[aPageNum].iSizeOfCompressedPageData );
#endif
	
	iPages[aPageNum].iCompressedPageData = (TUint8 *) calloc(iPages[aPageNum].iSizeOfCompressedPageData, sizeof(TUint8) );
	
	if( NULL == iPages[aPageNum].iCompressedPageData )
	{
		return;
	}
	
	memcpy(iPages[aPageNum].iCompressedPageData, iOutBuffer, iPages[aPageNum].iSizeOfCompressedPageData );
	
#endif

	iHeader.iSizeOfData += iPages[aPageNum].iSizeOfCompressedPageData;
}

void CBytePairCompressedImage::WriteOutTable(ostream &os)
{
	// Write out IndexTableHeader
	os.write((const char *) &iHeader.iSizeOfData, sizeof(iHeader.iSizeOfData));
	os.write((const char *) &iHeader.iDecompressedSize, sizeof(iHeader.iDecompressedSize));
	os.write((const char *) &iHeader.iNumberOfPages, sizeof(iHeader.iNumberOfPages));
	
	TInt i;
	// Write out IndexTableItems (size of each compressed page)
	for(i = 0; i < iHeader.iNumberOfPages; i++)
	{
		os.write((const char *) &(iPages[i].iSizeOfCompressedPageData), sizeof(TUint16));
	}
	
	// Write out compressed pages
	for(i = 0; i < iHeader.iNumberOfPages; i++)
	{
		os.write((const char*)iPages[i].iCompressedPageData, iPages[i].iSizeOfCompressedPageData); 
	}
	
}


int CBytePairCompressedImage::ReadInTable(istream &is, TUint & aNumberOfPages)
{
	// Read page index table header 
	is.read((char *)&iHeader, (sizeof(iHeader.iSizeOfData)+sizeof(iHeader.iDecompressedSize)+sizeof(iHeader.iNumberOfPages)));

	// Allocatin place to Page index table entries
	iPages = (IndexTableItem *) calloc(iHeader.iNumberOfPages, sizeof(IndexTableItem));
		
	if( NULL == iPages )
	{
		return KErrNoMemory;
	}
	TInt i;
	// Read whole Page index table 

	for(i = 0; i < iHeader.iNumberOfPages; i++)
	{
		is.read((char *) &(iPages[i].iSizeOfCompressedPageData), sizeof(TUint16));
	}

	// Read compressed data pages page by page, decompress and store them
	for(i = 0; i < iHeader.iNumberOfPages; i++)
	{

		iPages[i].iCompressedPageData = (TUint8 *) calloc(iPages[i].iSizeOfCompressedPageData, sizeof(TUint8) );
	
		if( NULL == iPages[i].iCompressedPageData )
		{
			return KErrNoMemory;
		}

		is.read((char*)iPages[i].iCompressedPageData, iPages[i].iSizeOfCompressedPageData);
	}

	aNumberOfPages = iHeader.iNumberOfPages;

	return KErrNone;
}

int  CBytePairCompressedImage::GetPage(TUint16 aPageNum, TUint8 * aPageData, CBytePair *aBPE)
{
	TUint8* pakEnd;
        
    const TInt MaxBlockSize = 0x1000;
        
   	TUint16 uncompressedSize = (TUint16) aBPE->Decompress( aPageData, 
												MaxBlockSize, 
												iPages[aPageNum].iCompressedPageData, 
												iPages[aPageNum].iSizeOfCompressedPageData, 
												pakEnd );

	return uncompressedSize;


}


void CompressPages(TUint8 * bytes, TInt size, ostream& os, CBytePair *aBPE)
{
	// Build a list of compressed pages
	TUint16 numOfPages = (TUint16) (size / PAGE_SIZE);

#ifdef __DEBUG_MSG__
	Print(EWarning,"numOfPages:%d, (size %% PAGE_SIZE:%d)\n", numOfPages, size % PAGE_SIZE);
#endif

	if ( size % PAGE_SIZE > 0)
		++numOfPages;
	
	CBytePairCompressedImage *comprImage = CBytePairCompressedImage::NewLC(numOfPages, size);
	if( NULL == comprImage)
	{
		Print(EError," NULL == comprImage\n");
		return;
	}
	
	TUint8* iPageStart;
	TUint16 iPageLen;
	TUint16 iPage = 0;
	
	while(iPage * PAGE_SIZE < size )
	{
		iPageStart = &bytes[iPage * PAGE_SIZE];
		iPageLen = (TUint16)( (iPage + 1) * PAGE_SIZE < size ? PAGE_SIZE : size - iPage * PAGE_SIZE);
		
		comprImage->AddPage(iPage, iPageStart, iPageLen, aBPE);

		++iPage;
	}
	
	// Write out index table and compressed pages
	comprImage->WriteOutTable(os);
	
	
	delete comprImage;
	comprImage = NULL;
	
}

int DecompressPages(TUint8 * bytes, istream& is, CBytePair *aBPE)
{
	TUint decompressedSize = 0;
	CBytePairCompressedImage *comprImage = CBytePairCompressedImage::NewLC(0, 0);
	if( NULL == comprImage)
	{
		Print(EError," NULL == comprImage\n");
		return KErrNoMemory;
	}

	TUint numberOfPages = 0;
	comprImage->ReadInTable(is, numberOfPages);


	TUint8* iPageStart;
	TUint16 iPage = 0;
	
	while(iPage < numberOfPages )
	{
		iPageStart = &bytes[iPage * PAGE_SIZE];
		
		decompressedSize += comprImage->GetPage(iPage, iPageStart, aBPE);

		++iPage;
	}
	
	delete comprImage;
	return decompressedSize;

}
