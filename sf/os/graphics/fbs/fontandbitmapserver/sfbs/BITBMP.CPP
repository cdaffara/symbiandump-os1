// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <fbs.h>
#include <bitmap.h>
#include <graphicsaccelerator.h>
#include <graphics/lookuptable.h>
#include <graphics/blendingalgorithms.h>
#include <graphics/bitmapuid.h>
#include "UTILS.H"
#include <s32mem.h>
#include "ShiftedFileStore.h"
#include "CompileAssert.h"
#include "CompressionBookmark.h"
#include "BitmapCompr.h"
#include "palette.h"
#include "fbsrasterizer.h"
#include "bitmap.inl"
#include "BitwiseBitmap.inl"
#include "bitmapconst.h"

#ifdef __ARMCC__
#pragma arm
#pragma O3
#pragma Otime
#endif

const TInt KMaxPixelSize = KMaxTInt / 4; // Maximum pixel size to avoid some overflow problems
const TUint KMaxByteSize = TUint(KMaxTInt / 2); // Maximum byte size to avoid other overflow problems
enum
    {
    EFirstTime = 65536
    };
GLREF_C void Panic(TFbsPanic aPanic);

#define COLOR_VALUE(ScanLinePtr, XPos) (*((ScanLinePtr) + ((XPos) >> 5)) & ( 1 << ((XPos) & 0x1F)))

EXPORT_C void CBitwiseBitmap::operator delete(TAny *aThis)
	{
	if (((CBitwiseBitmap*)aThis)->iHeap)
		((CBitwiseBitmap*)aThis)->iHeap->Free(aThis);
	}

EXPORT_C CBitwiseBitmap::CBitwiseBitmap(RHeap* aHeap,CChunkPile* aPile):
	iUid(KCBitwiseBitmapUid),
	iSettings(ENone),
	iHeap(aHeap),
	iPile(aPile),
	iByteWidth(0),
	iDataOffset(0),
	iIsCompressedInRAM(EFalse)
	{
	//CBitwiseBitmap size can't be changed! If the bitmap is ROM based,
	//then CBitwiseBitmap object is not created, but CBitwiseBitmap pointer is
	//used to access ROM based bitmap data.
	//The following line is a part of CFbsBitmap::DoLoad(...) source code:
	//iRomPointer=(CBitwiseBitmap*)(((TUint8*)rompointer)+offset);
	//We have to preserve data compatibility with already existing ROM bitmaps.
	enum
		{
		KCBitwiseBitmapSize = 72
		};
	COMPILE_TIME_ASSERT(sizeof(CBitwiseBitmap) == KCBitwiseBitmapSize);
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	Extra()->iLockCount = 0;
	Extra()->iThreadId = TThreadId(KNullThreadId);
#endif
	Extra()->iTouchCount = 0;
	Extra()->iSerialNumber = 0;
	}

EXPORT_C CBitwiseBitmap::~CBitwiseBitmap()
	{
	Reset();
	}

EXPORT_C void CBitwiseBitmap::Reset()
	{
	if (iDataOffset)
		{
		if(iUid.iUid==KCBitwiseBitmapHardwareUid.iUid)
			{
			RHardwareBitmap hwb(iDataOffset);	// iDataOffset = handle for hardware bitmap
			hwb.Destroy();
			}
		else
			if (iPile) iPile->Free(DataAddress());
		}
	iUid.iUid = KCBitwiseBitmapUid.iUid;
	iDataOffset=0;
	iSettings.SetDisplayModes(ENone);
	iByteWidth=0;
	iHeader=SEpocBitmapHeader();
	iIsCompressedInRAM=EFalse;
	}

EXPORT_C TUid CBitwiseBitmap::Uid() const
	{
	return(iUid);
	}

EXPORT_C TInt CBitwiseBitmap::Construct(const TSize& aSize,TDisplayMode aDispMode,TUid aCreatorUid)
	{
	if (iHeap == NULL || iPile == NULL)
		return KErrNoMemory;

	if (aSize.iWidth > KMaxPixelSize || aSize.iHeight > KMaxPixelSize)
		return KErrTooBig;

	TUint8* data = NULL;
	Reset();

	iSettings.SetDisplayModes(aDispMode);
	iByteWidth = ByteWidth(aSize.iWidth,aDispMode);

	TInt64 hugeDataSize = TInt64(aSize.iHeight) * TInt64(iByteWidth);
	if (I64HIGH(hugeDataSize) != 0 || I64LOW(hugeDataSize) > KMaxByteSize)
		return KErrTooBig;

	TInt dataSize = I64LOW(hugeDataSize);
	iHeader.iBitmapSize = sizeof(SEpocBitmapHeader) + dataSize;
	iHeader.iStructSize = sizeof(SEpocBitmapHeader);
	iHeader.iSizeInPixels = aSize;
	iHeader.iSizeInTwips = TSize(0,0);
	iHeader.iBitsPerPixel = Bpp(aDispMode);
	iHeader.iColor = IsColor(aDispMode);
	iHeader.iPaletteEntries = 0;
	iHeader.iCompression = ENoBitmapCompression;

	if (aSize.iHeight && aSize.iWidth)
		{
		if(aCreatorUid!=KUidCFbsBitmapCreation)
			{
			RHardwareBitmap hwb;
			TAcceleratedBitmapInfo info;
			TInt ret = hwb.Create(aDispMode,aSize,aCreatorUid);
			if(ret==KErrNone)
				ret = hwb.GetInfo(info);
			if(ret!=KErrNone)
				{
				Reset();
				return ret;
				}

			iSettings.SetVolatileBitmap();
			data = info.iAddress;
			dataSize = info.iLinePitch*info.iSize.iHeight;
			__ASSERT_DEBUG(info.iLinePitch >= iByteWidth, ::Panic(EFbsHardwareBitmapError));
			iByteWidth = info.iLinePitch;

			iDataOffset = hwb.iHandle;	// iDataOffset = handle for hardware bitmap
			iUid.iUid = KCBitwiseBitmapHardwareUid.iUid;
#ifdef SYMBIAN_DISABLE_HARDWARE_BITMAP_WHITEFILL
			return KErrNone;
#endif
			}
		else
			{
			data = iPile->Alloc(dataSize);
			iDataOffset = data - iPile->ChunkBase();
			}
		if (!data)
			{
			iDataOffset=0;
			Reset();
			return(KErrNoMemory);	// no memory exit point
			}
		}

	if (dataSize < KMaxLargeBitmapAlloc || aDispMode == EColor4K || iUid.iUid == KCBitwiseBitmapHardwareUid.iUid)
		{
		WhiteFill(data,dataSize,aDispMode);
		}

	return KErrNone;	// success exit point
	}


EXPORT_C TInt CBitwiseBitmap::ConstructExtended(const TSize& aSize, TDisplayMode aDispMode, TUid aType, TInt aDataSize)
	{
	if (iHeap == NULL || iPile == NULL)
		return KErrNoMemory;
	if (aSize.iWidth > KMaxPixelSize || aSize.iHeight > KMaxPixelSize)
		return KErrTooBig;
	if (aType.iUid == KCBitwiseBitmapUid.iUid || aType.iUid == KCBitwiseBitmapHardwareUid.iUid)
		return KErrArgument; // make sure the extended bitmap type is not one of the standard types
	if (aDataSize > KMaxByteSize)
		return KErrTooBig;
	Reset();
	iUid = aType;
	iSettings.SetDisplayModes(aDispMode);
	iByteWidth = ByteWidth(aSize.iWidth, aDispMode);
	iHeader.iBitmapSize = sizeof(SEpocBitmapHeader) + aDataSize;
	iHeader.iStructSize = sizeof(SEpocBitmapHeader);
	iHeader.iSizeInPixels = aSize;
	iHeader.iSizeInTwips = TSize(0,0);
	iHeader.iBitsPerPixel = Bpp(aDispMode);
	iHeader.iColor = IsColor(aDispMode);
	iHeader.iPaletteEntries = 0;
	iHeader.iCompression = EProprietaryCompression;
	TUint8* data = iPile->Alloc(aDataSize);
	if (!data)
		{
		Reset();
		return KErrNoMemory;
		}
	iDataOffset = data - iPile->ChunkBase();
	return KErrNone;
	}


EXPORT_C void CBitwiseBitmap::ConstructL(RFs& aFs,const TDesC& aFilename,TInt32 aId,TUint aFileOffset)
	{
	//If aFileOffset != 0 then aFilename is a file with an embedded MBM file section at the end.
	//The implementation uses the fact that mbm files are implemented as
	//filestores and stream ID is actually the offset from the beginning of the filestore.
	//If stream ID changes its meaning in the future -
	//the method implementation has to be reviewed and changed too.
	User::LeaveIfNull(iHeap);
	User::LeaveIfNull(iPile);
	TUint fileMode = EFileRead;
	if(aFileOffset != 0) //This is a file with an embedded MBM file section at the end.
		fileMode |= EFileShareReadersOnly;
	CShiftedFileStore* filestore = CShiftedFileStore::OpenLC(aFs,aFilename,fileMode,aFileOffset);
	TStreamId streamid = filestore->Root();
	//TStreamId is the offset from the beggining of the file.
	//Obviously, if the bitmap file section is at the middle of the physical file,
	//we should add aFileOffset value to TStreamId value and use it.
	TStreamId streamid2(streamid.Value() + aFileOffset);
	RStoreReadStream readstream;
	readstream.OpenLC(*filestore,streamid2);
	TInt numbitmaps = readstream.ReadInt32L();
	if (aId < 0 || aId >= numbitmaps)
		User::Leave(KErrEof);

	TStreamId bmpstreamid;
	bmpstreamid.InternalizeL(readstream);
	TStreamId bmpstreamid2(bmpstreamid.Value() + aFileOffset);
	for (TInt count = 0; count < aId; count++)
		{
		bmpstreamid2.InternalizeL(readstream);
		bmpstreamid2 = TStreamId(bmpstreamid2.Value() + aFileOffset);
		}

	CleanupStack::PopAndDestroy();
	RStoreReadStream bmpstream;
	bmpstream.OpenLC(*filestore,bmpstreamid2);
	InternalizeL(bmpstream);
	CleanupStack::PopAndDestroy(2);
	}

EXPORT_C void CBitwiseBitmap::ConstructL(RFile& aFile,TInt32 aId,TUint aFileOffset)
	{
	//If aFileOffset != 0 then aFilename is a file with an embedded MBM file section at the end.
	//The implementation uses the fact that mbm files are implemented as
	//filestores and stream ID is actually the offset from the beginning of the filestore.
	//If stream ID changes its meaning in the future -
	//the method implementation has to be reviewed and changed too.
	User::LeaveIfNull(iHeap);
	User::LeaveIfNull(iPile);

	CShiftedFileStore* filestore = CShiftedFileStore::FromL(aFile,aFileOffset);
	CleanupStack::PushL(filestore);
	TStreamId streamid = filestore->Root();
	//TStreamId is the offset from the beggining of the file.
	//Obviously, if the bitmap file section is at the middle of the physical file,
	//we should add aFileOffset value to TStreamId value and use it.
	TStreamId streamid2(streamid.Value() + aFileOffset);
	RStoreReadStream readstream;
	readstream.OpenLC(*filestore,streamid2);
	TInt numbitmaps = readstream.ReadInt32L();
	if (aId < 0 || aId >= numbitmaps)
		User::Leave(KErrEof);
	//Retrieving the streamid of the bitmap of that id from the file
	TStreamId bmpstreamid;
	bmpstreamid.InternalizeL(readstream);
	TStreamId bmpstreamid2(bmpstreamid.Value() + aFileOffset);
	for (TInt count = 0; count < aId; count++)
		{
		bmpstreamid2.InternalizeL(readstream);
		bmpstreamid2 = TStreamId(bmpstreamid2.Value() + aFileOffset);
		}
	//Use the streamid found to initialize the bitmap raw data in the memory
	CleanupStack::PopAndDestroy(&readstream);
	RStoreReadStream bmpstream;
	bmpstream.OpenLC(*filestore,bmpstreamid2);
	InternalizeL(bmpstream);
	CleanupStack::PopAndDestroy(2,filestore);
	}


EXPORT_C void CBitwiseBitmap::ConstructL(CShiftedFileStore* aFileStore,TStreamId aStreamId)
	{
	User::LeaveIfNull(iHeap);
	User::LeaveIfNull(iPile);

	RStoreReadStream bmpstream;
	bmpstream.OpenLC(*aFileStore,aStreamId);
	InternalizeL(bmpstream);
	CleanupStack::PopAndDestroy();
	}

EXPORT_C TInt CBitwiseBitmap::CopyData(const CBitwiseBitmap& aSourceBitmap)
	{
	__ASSERT_DEBUG(iHeap && iPile, ::Panic(EFbsPanicBitmapDataCopy));
	__ASSERT_DEBUG(!iIsCompressedInRAM, ::Panic(EFbsPanicBitmapDataCopy));
	__ASSERT_DEBUG(iUid.iUid == KCBitwiseBitmapUid.iUid, ::Panic(EFbsPanicBitmapDataCopy));
	if (aSourceBitmap.iUid.iUid != KCBitwiseBitmapUid.iUid)
		return KErrNotSupported;
	const TDisplayMode displayMode = aSourceBitmap.iSettings.CurrentDisplayMode();
	__ASSERT_DEBUG(iSettings.CurrentDisplayMode() == displayMode, ::Panic(EFbsPanicBitmapDataCopy));
	if (aSourceBitmap.iHeader.iSizeInPixels.iWidth > 0)
		iHeader.iSizeInTwips.iWidth = (aSourceBitmap.iHeader.iSizeInTwips.iWidth * iHeader.iSizeInPixels.iWidth)
			/ aSourceBitmap.iHeader.iSizeInPixels.iWidth;
	if (aSourceBitmap.iHeader.iSizeInPixels.iHeight > 0)
		iHeader.iSizeInTwips.iHeight = (aSourceBitmap.iHeader.iSizeInTwips.iHeight * iHeader.iSizeInPixels.iHeight)
			/ aSourceBitmap.iHeader.iSizeInPixels.iHeight;
	TUint32* destBase = DataAddress();
	TUint32* srcBase = aSourceBitmap.DataAddress();
	if (!destBase || !srcBase)
		return KErrNone;
	TInt minPixelHeight = Min(iHeader.iSizeInPixels.iHeight, aSourceBitmap.iHeader.iSizeInPixels.iHeight);
	if (aSourceBitmap.iIsCompressedInRAM)
		{
		TUint8* dest = (TUint8*)destBase;
		TInt minPixelWidth = Min(iHeader.iSizeInPixels.iWidth, aSourceBitmap.iHeader.iSizeInPixels.iWidth);
		TPtr8 pDest(dest, iByteWidth, iByteWidth);
		TPoint pt(0, 0);
		TPoint ditherOffset(0, 0);
		TLineScanningPosition scanPos(srcBase);
		scanPos.iScanLineBuffer = HBufC8::New(aSourceBitmap.iByteWidth + 4);
		if (!scanPos.iScanLineBuffer)
			return KErrNoMemory;
		for (TInt row = 0; row < minPixelHeight; ++row)
			{
			pDest.Set(dest, iByteWidth, iByteWidth);
			pt.iY = row;
			aSourceBitmap.GetScanLine(pDest, pt, minPixelWidth, EFalse, ditherOffset, displayMode, srcBase, scanPos);
			dest += iByteWidth;
			}
		delete scanPos.iScanLineBuffer;
		}
	else
		{
		TUint8* dest = (TUint8*)destBase;
		TUint8* src = (TUint8*)srcBase;
		TInt minByteWidth = Min(iByteWidth, aSourceBitmap.iByteWidth);
		for(TInt row = 0; row < minPixelHeight; ++row)
			{
			Mem::Copy(dest, src, minByteWidth);
			dest += iByteWidth;
			src += aSourceBitmap.iByteWidth;
			}
		}
	if (iHeader.iSizeInPixels.iWidth > aSourceBitmap.iHeader.iSizeInPixels.iWidth)
		{
		TInt extraBits = (aSourceBitmap.iHeader.iSizeInPixels.iWidth * aSourceBitmap.iHeader.iBitsPerPixel) & 31;
		if (extraBits > 0)
			{
			TUint32 mask = KMaxTUint32;
			mask <<= extraBits;
			TInt destWordWidth = iByteWidth >> 2;
			TInt srcWordWidth = aSourceBitmap.iByteWidth >> 2;
			TUint32* maskAddress = destBase + srcWordWidth - 1;
			for (TInt row = 0; row < minPixelHeight; ++row)
				{
				*maskAddress |= mask;
				maskAddress += destWordWidth;
				}
			}
		}
	return KErrNone;
	}

EXPORT_C void CBitwiseBitmap::ExternalizeL(RWriteStream& aStream,const CFbsBitmap& aHandleBitmap) const
	{
	ExternalizeRectangleL(aStream,iHeader.iSizeInPixels,aHandleBitmap);
	}

EXPORT_C void CBitwiseBitmap::ExternalizeRectangleL(RWriteStream& aStream,const TRect& aRect,const CFbsBitmap& aHandleBitmap) const
	{
	if (aRect.IsEmpty())
		User::Leave(KErrArgument);
	// the bitmap must have been already prepared for data access
	if (aHandleBitmap.iUseCount == 0)
		User::Leave(KErrArgument);

	// If the bitmap is palette-compressed in RAM externalisation is currently not supported
	// Externalisation of extended bitmaps is currently not supported either
	if (iHeader.iCompression == EGenericPaletteCompression || iHeader.iCompression == EProprietaryCompression)
		User::Leave(KErrNotSupported);
	const TRect bitmapRect(iHeader.iSizeInPixels);
	TRect sourceRect(aRect);
	if (!sourceRect.Intersects(bitmapRect))
		User::Leave(KErrTooBig);
	sourceRect.Intersection(bitmapRect);

	TDisplayMode displayMode = iSettings.CurrentDisplayMode();
	const TInt scanLineByteLength = CBitwiseBitmap::ByteWidth(sourceRect.Width(),displayMode);
	const TInt rectByteSize = sourceRect.Height() * scanLineByteLength;
	TUint8* buffer = (TUint8*)User::AllocLC(scanLineByteLength);

	TPtr8 scanline(buffer,scanLineByteLength,scanLineByteLength);
	scanline.Fill(0xff);
	const TPoint zeroPoint;
	TInt compressedSize = 0;
	TInt row;

	for (row = sourceRect.iTl.iY; row < sourceRect.iBr.iY; row++)
		{
		GetScanLine(scanline,TPoint(sourceRect.iTl.iX,row),sourceRect.Width(),EFalse,zeroPoint,displayMode,aHandleBitmap.DataAddress());
		compressedSize += SizeOfDataCompressed(buffer,scanLineByteLength);
		}

	TBool compress = EFalse;
	if(compressedSize > 0)
		{
		compress = (displayMode == EColor4K) || (compressedSize < (rectByteSize >> 1) + (rectByteSize >> 2));
		}

	aStream.WriteInt32L(sizeof(SEpocBitmapHeader) + ((compress) ? compressedSize : rectByteSize));
	aStream.WriteInt32L(iHeader.iStructSize);
	aStream.WriteInt32L(sourceRect.Width());
	aStream.WriteInt32L(sourceRect.Height());
	aStream.WriteInt32L(HorizontalPixelsToTwips(sourceRect.Width()));
	aStream.WriteInt32L(VerticalPixelsToTwips(sourceRect.Height()));
	aStream.WriteInt32L(iHeader.iBitsPerPixel);
	aStream.WriteUint32L(iHeader.iColor);
	aStream.WriteInt32L(0);
	aStream.WriteUint32L(compress ? CompressionType(iHeader.iBitsPerPixel, iHeader.iColor) : ENoBitmapCompression);

	for (row = sourceRect.iTl.iY; row < sourceRect.iBr.iY; row++)
		{
		GetScanLine(scanline,TPoint(sourceRect.iTl.iX,row),sourceRect.Width(),EFalse,zeroPoint,displayMode,aHandleBitmap.DataAddress());

		if (!compress)
			aStream.WriteL(buffer,scanLineByteLength);
		else
			DoExternalizeDataCompressedL(aStream,buffer,scanLineByteLength);
		}

	CleanupStack::PopAndDestroy(); // buffer
	}

EXPORT_C void CBitwiseBitmap::InternalizeHeaderL(RReadStream& aStream,SEpocBitmapHeader& aHeader)
	{
	aHeader.iBitmapSize=aStream.ReadInt32L();
	aHeader.iStructSize=aStream.ReadInt32L();
	if (aHeader.iStructSize!=sizeof(SEpocBitmapHeader)) User::Leave(KErrCorrupt);
	aHeader.iSizeInPixels.iWidth=aStream.ReadInt32L();
	aHeader.iSizeInPixels.iHeight=aStream.ReadInt32L();
	aHeader.iSizeInTwips.iWidth=aStream.ReadInt32L();
	aHeader.iSizeInTwips.iHeight=aStream.ReadInt32L();
	aHeader.iBitsPerPixel=aStream.ReadInt32L();
	aHeader.iColor=(TInt)aStream.ReadUint32L();
	aHeader.iPaletteEntries=aStream.ReadInt32L();
	if (aHeader.iPaletteEntries != 0)
		{
		//Palettes are not supported.
		User::Leave(KErrNotSupported);
		}
	aHeader.iCompression=(TBitmapfileCompression)aStream.ReadUint32L();
	CheckHeaderIsValidL(aHeader);
	}

void CBitwiseBitmap::CheckHeaderIsValidL(const SEpocBitmapHeader& aHeader)	
	{
	//These fields are signed in the structure?
	TInt bitmapSize = aHeader.iBitmapSize;
	TInt imageHeightPix = aHeader.iSizeInPixels.iHeight;
	TInt imageWidthPix = aHeader.iSizeInPixels.iWidth;
	TInt bitsPerPixel = aHeader.iBitsPerPixel;
	TInt compression = aHeader.iCompression;
	TInt colour = aHeader.iColor;
	TBool corruptFlag = EFalse;
	
	//Need to copy the values from the structure
	TDisplayMode equivalentMode = CBitwiseBitmap::DisplayMode(aHeader.iBitsPerPixel,aHeader.iColor);
	if (equivalentMode == ENone)
		{
		User::Leave(KErrNotSupported);	
		}
	if(aHeader.iColor < 0)
		 {
		 corruptFlag = ETrue;
		 } 
	 	//easieast way to check if compression type is appropriate is to ask the compressor 
	if (compression && compression!= CBitwiseBitmap::CompressionType(bitsPerPixel,colour))
		{
		corruptFlag = ETrue;
		}
	//danger when using CBitwiseBitmap is they could panic for bad input...
	if (imageHeightPix <= 0 || imageWidthPix <= 0 || bitsPerPixel <= 0)
		{
		corruptFlag = ETrue;
		}
	const TInt KMeg = 1024 * 1024;
	//Test that scanline bytes calculation won't overflow.
	TInt bytesPerPack;	// pixel size in memory
	TInt bytesPerCompression;	// compressed unit data size
    User::LeaveIfError(CompressedFormatInfo(equivalentMode, bytesPerPack, bytesPerCompression));
	if (imageWidthPix > 2047 * KMeg / bytesPerPack)
		{
		corruptFlag = ETrue;
		}
	TInt uncompressedWidthBytes = CBitwiseBitmap::ByteWidth(imageWidthPix,equivalentMode);	//we know this won't overflow, now.
	//use top set bit indexes of 32 bit integer values to estimate when W*H multiply will overflow
	TInt exponentWidth = 0;
	TInt exponentHeight = 0;
	if (uncompressedWidthBytes & 0xffff0000)
		{
		exponentWidth += 16;
		}
	if (imageHeightPix & 0xffff0000)
		{
		exponentHeight += 16;
		}
	if (exponentWidth || exponentHeight)
		{
		if (uncompressedWidthBytes & 0xFF00FF00)
			{
			exponentWidth += 8;
			}
		if (imageHeightPix & 0xFF00FF00)
			{
			exponentHeight += 8;
			}
		if (uncompressedWidthBytes & 0xf0f0f0f0)
			{
			exponentWidth += 4;
			}
		if (imageHeightPix & 0xf0f0f0f0)
			{
			exponentHeight += 4;
			}
		if (uncompressedWidthBytes & 0xCCCCCCCC)
			{
			exponentWidth += 2;
			}
		if (imageHeightPix & 0xCCCCCCCC)
			{
				exponentHeight += 2;
		}
		if (uncompressedWidthBytes & 0xaaaaaaaa)
			{
			exponentWidth += 1;
			}
		if (imageHeightPix & 0xaaaaaaaa)
			{
			exponentHeight += 1;
			}
		TInt exponentTotal = exponentWidth + exponentHeight;
		if (exponentTotal >= 31)
			{
			//The result would defuinitely exceed a signed int
			corruptFlag = ETrue;
			}
		else if (exponentTotal == 30)
			{
			//as a bit test, both "next most significat bits" must be set to cause a carry-over,
			//but that isn't so trivial to test.
			if ((uncompressedWidthBytes >> 1) * imageHeightPix > 1024 * KMeg)
				{
				corruptFlag = ETrue;
				}
			}
		}
	if (compression)
		{
		/* estimate compressed file size limits
		byte compression uses lead code 0..127 = repeat next byte n+1 times. -1..-128 = copy next -n bytes
		16, 24, 32 use byte lead codes as above followed by words, triplets, or dwords
		1,2,4,8,16 all encode any dword alignment buffer padding data as full data values.
		32 doesn't have padding issue. 24 does not encode padding bytes.
		12 bit compression uses 0..15 spare nibble to encode short runs. 0=unique. Can never make file bigger.*/
		if (bitsPerPixel == 12)
			{
			//min file size is 1/16 of rect size
			if (bitmapSize < sizeof(SEpocBitmapHeader) + ((uncompressedWidthBytes * imageHeightPix) / 16))
				{
				corruptFlag = ETrue;	
				}
			if (bitmapSize > sizeof(SEpocBitmapHeader) + uncompressedWidthBytes * imageHeightPix)
				{
				corruptFlag = ETrue;
				}
			}
		else
			{
				TInt packedValsPerFile = (uncompressedWidthBytes / bytesPerPack) * imageHeightPix;
				//for some of the compressors 0 means a run of 2, so max 127 means a run of 129
				TInt estMinCompressedBlocksPerFile = (packedValsPerFile - 1) / 129 + 1;	
				/* Absolute minimum is blocks of 128 repeats possibly spanning multiple scanlines
				This can't be compressed by the current per-scanline compressor,
				but is acceptable to the decompressor. */
				if (bitmapSize < sizeof(SEpocBitmapHeader) + estMinCompressedBlocksPerFile * (bytesPerCompression + 1))
					{
					corruptFlag = ETrue;
					}
			/* Absolute maximum is to store every pixel as a seperate run of 1 byte.
			The current compressor would never do this... but the file is legal! */
				if (bitmapSize > sizeof(SEpocBitmapHeader) + packedValsPerFile * (bytesPerCompression + 1))
					{
					corruptFlag = ETrue;
					}
			}
		}
	else
		{
		if (bitmapSize != sizeof(SEpocBitmapHeader) + uncompressedWidthBytes * imageHeightPix)
			{
			corruptFlag = ETrue;
			}
		}
	if(corruptFlag)
		{
		User::Leave(KErrCorrupt);
		}
			
	}

/**
Internalizes the bit map contents from a stream.
@param aStream The read stream containing the bit map.
*/
EXPORT_C void CBitwiseBitmap::InternalizeL(RReadStream& aStream)
	{
	if (iHeap==NULL || iPile==NULL)
		User::Leave(KErrNoMemory);
	Reset();
	InternalizeHeaderL(aStream,iHeader);

	TDisplayMode displayMode = DisplayMode(iHeader.iBitsPerPixel,iHeader.iColor);
	if(displayMode == ENone)
		{
		Reset();
		User::Leave(KErrCorrupt);
		}

	iSettings.SetDisplayModes(displayMode);

	iByteWidth = ByteWidth(iHeader.iSizeInPixels.iWidth,iSettings.CurrentDisplayMode());

	TUint8* data=NULL;
	TInt bytesize = iByteWidth * iHeader.iSizeInPixels.iHeight;
	if (bytesize > 0)
		{
		data = iPile->Alloc(bytesize);
		iDataOffset = data - iPile->ChunkBase();
		if (!data)
			{
			iDataOffset=0;
			Reset();
			User::LeaveNoMemory();
			}
		}
	TRAPD(err,DoInternalizeL(aStream,iHeader.iBitmapSize-iHeader.iStructSize,DataAddress()));
	if (err!=KErrNone)
		{
		Reset();
		User::Leave(err);
		}
	}


void CBitwiseBitmap::DoInternalizeL(RReadStream& aStream,TInt aSrceSize,TUint32* aBase)
	{
	if (iHeader.iCompression==ENoBitmapCompression)
		aStream.ReadL((TUint8*)aBase,aSrceSize);
	else if (iHeader.iCompression < ERLECompressionLast)
		{
		TBitmapfileCompression compression = iHeader.iCompression;
		iHeader.iCompression = ENoBitmapCompression;
		iHeader.iBitmapSize = iByteWidth*iHeader.iSizeInPixels.iHeight+sizeof(SEpocBitmapHeader);
		DoInternalizeCompressedDataL(aStream,aSrceSize,aBase,compression);
		}
 	else
		CheckHeaderIsValidL(iHeader);
	}

EXPORT_C TDisplayMode CBitwiseBitmap::DisplayMode() const
	{
	return iSettings.CurrentDisplayMode();
	}

EXPORT_C TInt CBitwiseBitmap::HorizontalPixelsToTwips(TInt aPixels) const
	{
	if (iHeader.iSizeInPixels.iWidth==0)
		return(0);
	TInt twips;
	twips = (aPixels*iHeader.iSizeInTwips.iWidth+(iHeader.iSizeInPixels.iWidth/2))/iHeader.iSizeInPixels.iWidth;
	return(twips);
	}

EXPORT_C TInt CBitwiseBitmap::VerticalPixelsToTwips(TInt aPixels) const
	{
	if (iHeader.iSizeInPixels.iHeight==0)
		return(0);
	TInt twips;
	twips = (aPixels*iHeader.iSizeInTwips.iHeight+(iHeader.iSizeInPixels.iHeight/2))/iHeader.iSizeInPixels.iHeight;
	return (twips);
	}

EXPORT_C TSize CBitwiseBitmap::SizeInPixels() const
	{
	return(iHeader.iSizeInPixels);
	}

EXPORT_C TSize CBitwiseBitmap::SizeInTwips() const
	{
	return(iHeader.iSizeInTwips);
	}

EXPORT_C TInt CBitwiseBitmap::HorizontalTwipsToPixels(TInt aTwips) const
	{
	if (iHeader.iSizeInTwips.iWidth==0)
		return(0);
	TInt pixels;
	pixels = (aTwips*iHeader.iSizeInPixels.iWidth+(iHeader.iSizeInTwips.iWidth/2))/iHeader.iSizeInTwips.iWidth;
	return(pixels);
	}

EXPORT_C TInt CBitwiseBitmap::VerticalTwipsToPixels(TInt aTwips) const
	{
	if (iHeader.iSizeInTwips.iHeight==0)
		return(0);
	TInt pixels;
	pixels = (aTwips*iHeader.iSizeInPixels.iHeight+(iHeader.iSizeInTwips.iHeight/2))/iHeader.iSizeInTwips.iHeight;
	return(pixels);
	}

/**
The method retrieves the red, green, blue (RGB) color value of the pixel with
specified coordinates.
Note: The method works for uncompressed bitmaps and extended bitmaps only.
@internalComponent
@released
@pre aBase != NULL;
@param aColor It will be initialized with the pixel color value on success, otherwise
aColor value will be left unchanged.
@param aPixel Pixel coordinates.
@param aBase It points to the beginning of the bitmap data.
*/
EXPORT_C void CBitwiseBitmap::GetPixel(TRgb& aColor,const TPoint& aPixel,TUint32* aBase, CFbsRasterizer* aRasterizer) const
	{
	// This operation is not currently supported for compressed bitmaps.
	if (iHeader.iCompression != ENoBitmapCompression && iHeader.iCompression != EProprietaryCompression)
		{
		__ASSERT_DEBUG(EFalse, ::Panic(EFbsBitmapInvalidCompression));
		return;
		}
	if (!iDataOffset)
		return;

	TInt x=aPixel.iX,y=aPixel.iY;
	if (x < -iHeader.iSizeInPixels.iWidth)
		x %= iHeader.iSizeInPixels.iWidth;
	if (y < -iHeader.iSizeInPixels.iHeight)
		y %= iHeader.iSizeInPixels.iHeight;
	if (x < 0)
		x += iHeader.iSizeInPixels.iWidth;
	if (y < 0)
		y += iHeader.iSizeInPixels.iHeight;

	if (iHeader.iCompression == EProprietaryCompression)
		{
		if (aRasterizer)
			{
			TUint32* slptr = const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(x,y), 1));
			if (slptr)
				{
				aColor = GetRgbPixelEx(x, slptr);
				}
			else
				{
				// wrong rasterizer for this extended bitmap - return white pixel
				aColor = KRgbWhite;
				}
			}
		else
			{
			// no rasterizer - return white pixel
			aColor = KRgbWhite;
			}
		}
	else
		{
		aColor = GetRgbPixelEx(x,ScanLineAddress(aBase,y));
		}
	}

EXPORT_C TInt CBitwiseBitmap::GetScanLinePtr(TUint32*& aSlptr, TInt& aLength,TPoint& aPixel,TUint32* aBase, TLineScanningPosition& aLineScanningPosition) const
	{
	if (!iDataOffset)
		return KErrNone;
	if (aPixel.iX >= iHeader.iSizeInPixels.iWidth || aPixel.iX < -iHeader.iSizeInPixels.iWidth)
		aPixel.iX %= iHeader.iSizeInPixels.iWidth;
	if (aPixel.iY >= iHeader.iSizeInPixels.iHeight || aPixel.iY < -iHeader.iSizeInPixels.iHeight)
		aPixel.iY %= iHeader.iSizeInPixels.iHeight;
	if (aPixel.iX < 0)
		aPixel.iX += iHeader.iSizeInPixels.iWidth;
	if (aPixel.iY < 0)
		aPixel.iY += iHeader.iSizeInPixels.iHeight;
	if (aPixel.iX + aLength > iHeader.iSizeInPixels.iWidth)
		aLength = iHeader.iSizeInPixels.iWidth - aPixel.iX;
	if (iHeader.iCompression != ENoBitmapCompression)
		{
		return DoGetScanLinePtr(aSlptr, aPixel,aLength,aBase,aLineScanningPosition);
		}
	else
		{
		aSlptr = ScanLineAddress(aBase,aPixel.iY);
		}
	return KErrNone;
	}

EXPORT_C TInt CBitwiseBitmap::GetScanLinePtr(TUint32*& aSlptr, TPoint& aPixel,TInt aLength,TUint32* aBase, TLineScanningPosition& aLineScanningPosition) const
	{
	return GetScanLinePtr(aSlptr, aLength,aPixel,aBase, aLineScanningPosition);
	}

TUint8 CBitwiseBitmap::GetGrayPixelEx(TInt aX,TUint32* aScanlinePtr) const
	{
	// returns pixel as EGray256 value (0 - 255)

	if (iHeader.iColor)
		return TUint8(GetRgbPixelEx(aX,aScanlinePtr)._Gray256());
	else
		{
		if (!aScanlinePtr)
			return 0;

		if (aX >= iHeader.iSizeInPixels.iWidth)
			aX %= iHeader.iSizeInPixels.iWidth;

		switch (iHeader.iBitsPerPixel)
			{
			case 2:
				{
				TUint32 col = *(aScanlinePtr+(aX>>4));
				col>>=((aX&0xf)<<1);
				col&=3;
				col |= (col << 6) | (col<<4) | (col<<2);
				return TUint8(col);
				}
			case 4:
				{
				TUint32 col = *(aScanlinePtr+(aX>>3));
				col >>= ((aX&7)<<2);
				col &= 0xf;
				return TUint8(col |= (col << 4));
				}
			case 1:
				{
				TUint32 col = *(aScanlinePtr+(aX>>5));
				if (col&(1<<(aX&0x1f))) return 255 ;
				return 0;
				}
			case 8:
				return *(((TUint8*)aScanlinePtr) + aX);
			default:
				return 0;
			 }

		}
	}

TRgb CBitwiseBitmap::GetRgbPixelEx(TInt aX,TUint32* aScanlinePtr) const
	{
	// returns pixel as TRgb

	if (iHeader.iColor)
		{
		if (!aScanlinePtr)
			return KRgbBlack;

		if (aX>=iHeader.iSizeInPixels.iWidth)
			aX%=iHeader.iSizeInPixels.iWidth;

		switch (iHeader.iBitsPerPixel)
			{
			case 32:
				if (iSettings.CurrentDisplayMode() == EColor16MAP)
					return TRgb::_Color16MAP(*(aScanlinePtr + aX));
				else if (iSettings.CurrentDisplayMode() == EColor16MA)
					return TRgb::_Color16MA(*(aScanlinePtr + aX));
				//scanLineBytePointer format: BGR0 - 0RGB as INT32.
				else
					return TRgb::_Color16MU(*(aScanlinePtr + aX));
			case 24:
				{
				TUint8* scanLineBytePointer = (TUint8*)aScanlinePtr + aX * 3;
				TInt color16M = *scanLineBytePointer++;
				color16M |= (*scanLineBytePointer++) << 8;
				color16M |= (*scanLineBytePointer++) << 16;
				return TRgb::_Color16M(color16M);
				}
			case 16:
				return TRgb::_Color64K(*(((TUint16*)aScanlinePtr) + aX));
			case 12:
				return TRgb::_Color4K(*(((TUint16*)aScanlinePtr) + aX));
			case 8:
				return TRgb::Color256(*((TUint8*)aScanlinePtr + aX));
			case 4:
				{
				TUint8 colorIndex = *((TUint8*)aScanlinePtr + (aX >> 1));
				if (aX & 1)
					colorIndex >>= 4;
				return TRgb::Color16(colorIndex & 0xf);
				}
			default:
				return KRgbBlack;
			}
		}
	else
		return TRgb::_Gray256(GetGrayPixelEx(aX,aScanlinePtr));
	}

void CBitwiseBitmap::GetRgbPixelExMany(TInt aX,TUint32* aScanlinePtr, TUint32* aDest,TInt aLength) const
    {
    __ASSERT_DEBUG(aScanlinePtr && aDest, User::Invariant());
    __ASSERT_DEBUG(aX      >= 0, User::Invariant());
    __ASSERT_DEBUG(aLength >= 0, User::Invariant());
    __ASSERT_DEBUG(aX+aLength <= iHeader.iSizeInPixels.iWidth, User::Invariant());

    union {
        TUint8*  scanPtr8;
        TUint16* scanPtr16;
        TUint32* scanPtr32;
    };
    TUint32 color;
    TUint32 rawData;
    const TUint32 opaqueAlpha = 0xff000000;
    TUint32 color16MA = 0;  // // cached map to color

    if (aLength < 1)
        {
        return;
        }

    if (iHeader.iColor)
        {
        switch (iHeader.iBitsPerPixel)
            {
            case 4:
                {
                scanPtr8 = reinterpret_cast<TUint8*>(aScanlinePtr);
                TUint32 color16 = EFirstTime; // map source color: 16 color mode
                do
                    {
                    rawData = (aX & 1) ? (scanPtr8[aX >> 1] >> 4) : (scanPtr8[aX >> 1] &0x0F);
                    if ((rawData != color16))
                        { // first pixel or colour change
                        color16MA = TRgb::Color16(rawData)._Color16MA();
                        color16 = rawData;
                        }
                    *aDest++ = color16MA;
                    aX++;
                    }
                    while (--aLength);
                return;
                }
            case 8:
                {
                scanPtr8 = reinterpret_cast<TUint8*>(aScanlinePtr) + aX;
                const TUint32* lookup = DynamicPalette::DefaultColor256Util()->iColorTable;
                TUint32 color256 = EFirstTime; // map source color: 256 color
                do
                    {
                    rawData = *scanPtr8++;
                    if ((rawData != color256))
                        { // colour change or first pixel
                        color16MA = lookup[rawData];
                        color256 = rawData;
                        // translate between bgr & rgb
                        color16MA = ((color16MA & 0x0000ff) << 16) | (color16MA & 0x00ff00) | ((color16MA & 0xff0000) >> 16) | opaqueAlpha;
                        }
                    *aDest++ = color16MA;
                    }
                    while (--aLength);
                return;
                }
            case 12:
                {
                scanPtr16 = reinterpret_cast<TUint16*>(aScanlinePtr) + aX;
                TUint32 color4K = EFirstTime; // map source color: 4K color black => 16M color black
                do
                    {
                    rawData = *scanPtr16++;
                    if ((rawData != color4K))
                        { // colour change
                        color16MA = TRgb::_Color4K(rawData)._Color16MA();
                        color4K = rawData;
                        }
                    *aDest++ = color16MA;
                    }
                    while (--aLength);
                return;
                }
            case 16:
                {
                scanPtr16 = reinterpret_cast<TUint16*>(aScanlinePtr) + aX;
                const TUint16* lowAdd = Convert16to32bppLow();
                const TUint32* highAdd = Convert16to32bppHigh();
                TUint32 color64K = EFirstTime; // map source color: 64K color black => 16M color black
                do
                    {
                    rawData = *scanPtr16++;
                    if ((rawData != color64K))
                        { // colour change
                        color16MA = highAdd[rawData >> 8] | lowAdd[rawData & 0x00FF];
                        color64K = rawData;
                        }
                    *aDest++ = color16MA;
                    }
                    while (--aLength);
                return;
                }
            case 24:
                {
                scanPtr8 = reinterpret_cast<TUint8*>(aScanlinePtr) + aX*3;
                do
                    {
                    if ((aLength < 4) || (3 & (TUint32)(scanPtr8)))
                        {
                        aLength--;
                        color  =  *scanPtr8++;
                        color |= (*scanPtr8++) << 8;
                        color |= (*scanPtr8++) << 16;
                        *aDest++ = color | opaqueAlpha;
                        }
                    else
                        { // source is now TUint32 aligned - so read source as blocks of 3 TUint32's & write as 4
                        TUint32 word1, word2, word3;
                        TInt iter = (aLength / 4) - 1;
                        aLength = aLength & 0x0003;
                        do
                            {
                            word1 = *scanPtr32++; 
                            *aDest++ = word1 | opaqueAlpha;
                            word2 = *scanPtr32++; 
                            color = (word1 >> 24) | ((word2 & 0xFFFF) << 8); 
                            *aDest++ = color | opaqueAlpha;
                            word3 = *scanPtr32++; 
                            color = (word2 >> 16) | ((word3 & 0x00FF) << 16); 
                            *aDest++ = color | opaqueAlpha;
                            *aDest++ = (word3 >> 8) | opaqueAlpha;
                            }
                            while (iter--);
                        }
                    }
                    while (aLength);
                return;
                }
            case 32:
                {
                scanPtr32 = aScanlinePtr + aX;
                if(iSettings.CurrentDisplayMode() == EColor16MAP)
                    { // unrolled loop uses "Duff's Device"
                    const TUint16* normTable = PtrTo16BitNormalisationTable();
                    --aLength;
                    TInt iter = aLength / 8;
                    switch(aLength & 7) 
                        {
                        case 7:
                            do {
                                *aDest++ = PMA2NonPMAPixel(*scanPtr32++, normTable);
                        case 6:
                                *aDest++ = PMA2NonPMAPixel(*scanPtr32++, normTable);
                        case 5:
                                *aDest++ = PMA2NonPMAPixel(*scanPtr32++, normTable);
                        case 4:
                                *aDest++ = PMA2NonPMAPixel(*scanPtr32++, normTable);
                        case 3:
                                *aDest++ = PMA2NonPMAPixel(*scanPtr32++, normTable);
                        case 2:
                                *aDest++ = PMA2NonPMAPixel(*scanPtr32++, normTable);
                        case 1:
                                *aDest++ = PMA2NonPMAPixel(*scanPtr32++, normTable);
                        case 0:
                                *aDest++ = PMA2NonPMAPixel(*scanPtr32++, normTable);
                            } while (iter-- > 0);
                        }
                    }
                else if (iSettings.CurrentDisplayMode() == EColor16MU)
                    { // unrolled loop uses "Duff's Device"
                    --aLength;
                    TInt iter = aLength / 8;
                    switch(aLength & 7) 
                        {
                        case 7:
                            do {
                                *aDest++ = (*scanPtr32++) | opaqueAlpha;
                        case 6:
                                *aDest++ = (*scanPtr32++) | opaqueAlpha;
                        case 5:
                                *aDest++ = (*scanPtr32++) | opaqueAlpha;
                        case 4:
                                *aDest++ = (*scanPtr32++) | opaqueAlpha;
                        case 3:
                                *aDest++ = (*scanPtr32++) | opaqueAlpha;
                        case 2:
                                *aDest++ = (*scanPtr32++) | opaqueAlpha;
                        case 1:
                                *aDest++ = (*scanPtr32++) | opaqueAlpha;
                        case 0:
                                *aDest++ = (*scanPtr32++) | opaqueAlpha;
                            } while (iter-- > 0);
                        }
                    }
                else // EColor16MA
                    {
                    Mem::Move(aDest, scanPtr32, aLength * 4);
                    }
                return;
                }
            default:
                return;
            }
        }
    else
        {
        switch (iHeader.iBitsPerPixel)
            {
            case 1:
                {
                do
                    {
                    *aDest++ = TUint32(COLOR_VALUE(aScanlinePtr, aX) ? 0xFFFFFFFF : 0xFF000000);
                    aX++;
                    }
                while (--aLength);
                break;
                }
            case 8:
                {
                scanPtr8 = reinterpret_cast<TUint8*>(aScanlinePtr) + aX;
                do
                    {
                    rawData = *scanPtr8++;
                    color16MA = rawData | (rawData << 8) | (rawData << 16) | opaqueAlpha; 
                    *aDest++ = color16MA;
                    }
                    while (--aLength);              
                break;
                }
            default:
                {
                do
                    {
                    rawData = GetGrayPixelEx(aX++,aScanlinePtr);
                    color16MA = rawData | (rawData << 8) | (rawData << 16) | opaqueAlpha; 
                    *aDest++ = color16MA;
                    }
                    while (--aLength);              
                }
            }
        }
    }
/**
The method retrieves the RGB color values from the scanline, and converts them
into the destination screen-mode pixel format. This method handles the special
case when the destination mode is EColor16MAP (32bit with alpha values premultiplied
with the color channels. Calls GetRgbPixelExMany for values not 32 bit, as there is no
alpha information in these color modes. For color mode EColor16MU, no conversion is 
performed (as alpha is assumed to be 1).
@internalComponent
@released
@param aX	The x co-ordinate the scanline data needs to be retrieved from.
@param aScanlinePtr	The scanline pointer, i.e. the source data.
@param aDest	The pointer to the destination buffer. This is where the output is stored.
@param aLength	The number of bytes to be copied. This value must be a multiple of 4.
*/
void CBitwiseBitmap::GetRgbPixelExMany16MAP(TInt aX,TUint32* aScanlinePtr,TUint32* aDest,TInt aLength) const
    {
    __ASSERT_DEBUG(aScanlinePtr && aDest, User::Invariant());
    __ASSERT_DEBUG(aX      >= 0, User::Invariant());
    __ASSERT_DEBUG(aLength >= 0, User::Invariant());
    __ASSERT_DEBUG(aX+aLength <= iHeader.iSizeInPixels.iWidth, User::Invariant());

    TUint32* scanPtr32;

    if ((iHeader.iColor) && (iHeader.iBitsPerPixel == 32))
        {
        scanPtr32 = aScanlinePtr + aX;
        if (DisplayMode() == EColor16MAP)
            {
            Mem::Move(aDest, scanPtr32, aLength<<2);
            }
        else if(DisplayMode()==EColor16MA)
            {
            TUint32* ptrLimit = aDest + aLength;
            const TInt32 zero = 0;
            while (aDest < ptrLimit)
                {
                TUint32 value = *scanPtr32++;
                TUint32 tA = value >> 24;
                if (tA == 0)
                    {
                    *aDest++ = zero;
                    }
                else if (tA != 255) 
                    {
                    *aDest++ = NonPMA2PMAPixel(value);
                    }
                else
                    {
                    *aDest++ = value;
                    }
                }
            }
        else // DisplayMode() == EColor16MU
            {
            if (aLength--)
                { // unrolled loop uses "Duff's Device"
                const TUint32 alpha = 0xFF000000;   //set all the alpha to 0xff
                TInt iter = aLength / 8;
                switch(aLength & 7) 
                    {
                    case 7:
                        do {
                            *aDest++ = (*scanPtr32++) | alpha;
                    case 6:
                            *aDest++ = (*scanPtr32++) | alpha;
                    case 5:
                            *aDest++ = (*scanPtr32++) | alpha;
                    case 4:
                            *aDest++ = (*scanPtr32++) | alpha;
                    case 3:
                            *aDest++ = (*scanPtr32++) | alpha;
                    case 2:
                            *aDest++ = (*scanPtr32++) | alpha;
                    case 1:
                            *aDest++ = (*scanPtr32++) | alpha;
                    case 0:
                            *aDest++ = (*scanPtr32++) | alpha;
                        } while (iter-- > 0);
                    }
                }
            }
        }
    else
        {
        GetRgbPixelExMany(aX, aScanlinePtr, aDest, aLength);
        }
    }

void CBitwiseBitmap::GetRgbPixelExMany16M(TInt aX,TUint32* aScanlinePtr, TUint8* aDest,TInt aLength) const
    {
    union {
        TUint8*  scanPtr8;
        TUint16* scanPtr16;
        TUint32* scanPtr32;
    };
    union {
        TUint8*  destPtr8;
        TUint32* destPtr32;
    };
    destPtr8 = aDest;
    TUint32 rawData;

    if (!aScanlinePtr)
        {
        const TUint32 zero = 0; // conveniently KRgbBlack is 0 in EColor16M mode
        while (aLength)
            {
            if ((aLength < 4) || (3 & (TUint32)(destPtr8)))
                {
                aLength--;
                *destPtr8++ = zero;
                *destPtr8++ = zero;
                *destPtr8++ = zero;
                }
            else
                { // dest is now TUint32 aligned - write 4 pixels into 3 TUint32s
                aLength -= 4;
                *destPtr32++ = zero;
                *destPtr32++ = zero;
                *destPtr32++ = zero;
                }
            }
        return;
        }

    if (aX>=iHeader.iSizeInPixels.iWidth)
        {
        aX%=iHeader.iSizeInPixels.iWidth;
        }

    while (aLength)
        {
        // cached map to color
        TUint32 color16M = 0;
        TInt copyLength = iHeader.iSizeInPixels.iWidth - aX;
        TUint32* scanPtr32 = aScanlinePtr + aX;
        if (copyLength > aLength)
            {
            copyLength = aLength;
            }
        aLength -= copyLength;

        if (iHeader.iColor)
            {
            switch(iHeader.iBitsPerPixel)
                {
                case 4:
                    {
                    scanPtr8 = reinterpret_cast<TUint8*>(aScanlinePtr);
                    TUint32 color16 = EFirstTime; // map source color: 16 color mode
                    do
                        {
                        rawData = (aX & 1) ? (scanPtr8[aX >> 1] >> 4) : (scanPtr8[aX >> 1] &0x0F);
                        if ((rawData != color16))
                            { // first pixel or colour change
                            color16M = TRgb::Color16(rawData)._Color16M();
                            color16 = rawData;
                            }
                        *destPtr8++ = TUint8(color16M);
                        *destPtr8++ = TUint8(color16M >> 8);
                        *destPtr8++ = TUint8(color16M >> 16);
                        aX++;
                        }
                        while (--copyLength);
                    break;
                    }
                case 8:
                    {
                    scanPtr8 = reinterpret_cast<TUint8*>(aScanlinePtr) + aX;
                    const TUint32* lookup = DynamicPalette::DefaultColor256Util()->iColorTable;
                    TUint32 color256 = EFirstTime; // map source color: 256 color
                    do
                        {
                        rawData = *scanPtr8++;
                        if ((rawData != color256))
                            { // first pixel or colour change; so perform new mapping
                            color16M = lookup[rawData];
                            color256 = rawData;
                            }
                        // Note; byte order performs required bgr -> rgb conversion 
                        *destPtr8++ = TUint8(color16M >> 16);
                        *destPtr8++ = TUint8(color16M >> 8);
                        *destPtr8++ = TUint8(color16M);
                        }
                        while (--copyLength);
                    break;
                    }
                case 12:
                    {
                    scanPtr16 = reinterpret_cast<TUint16*>(aScanlinePtr) + aX;
                    TUint32 color4K = EFirstTime; // map source color: 4K color black => 16M color black
                    do
                        {
                        rawData = *scanPtr16++;
                        if ((rawData != color4K))
                            { // first pixel or colour change
                            color16M = TRgb::_Color4K(rawData)._Color16M();
                            color4K = rawData;
                            }
                        *destPtr8++ = TUint8(color16M);
                        *destPtr8++ = TUint8(color16M >> 8);
                        *destPtr8++ = TUint8(color16M >> 16);
                        }
                        while (--copyLength);
                    break;
                    }
                case 16:
                    {
                    scanPtr16 = reinterpret_cast<TUint16*>(aScanlinePtr) + aX;
                    const TUint16* lowAdd = Convert16to32bppLow();
                    const TUint32* highAdd = Convert16to32bppHigh();
                    TUint16 halfWord;
                    TUint32 color64K = EFirstTime; // 64K color black => 16M color black
                    color16M = 0;
                    do
                        {
                        halfWord = *scanPtr16++;
                        if ((halfWord != color64K))
                            { // colour change
                            color16M = highAdd[halfWord >> 8] | lowAdd[halfWord & 0x00FF];
                            color64K = halfWord;
                            }
                        *destPtr8++ = TUint8(color16M);
                        *destPtr8++ = TUint8(color16M >> 8);
                        *destPtr8++ = TUint8(color16M >> 16);
                        }
                        while (--copyLength);
                    break;
                    }
                case 24:
                    {
                    Mem::Copy(destPtr8, reinterpret_cast<TUint8*>(aScanlinePtr) + aX, copyLength * 3); // may not be aligned 
                    destPtr8 += copyLength * 3;
                    break;
                    }
                case 32:
                    {
                    scanPtr32 = aScanlinePtr + aX;
                    if (iSettings.CurrentDisplayMode() == EColor16MAP)
                        {
                        const TUint16* normTable = PtrTo16BitNormalisationTable();
                        do
                            {
                            // convert from EColor16MAP to EColor16MA first.
                            color16M = PMA2NonPMAPixel(*scanPtr32++, normTable);
                            *destPtr8++ = TUint8(color16M);
                            *destPtr8++ = TUint8(color16M >> 8);
                            *destPtr8++ = TUint8(color16M >> 16);
                            }
                            while (--copyLength);
                        }
                    else
                        { // EColor16MA or EColor16MU, keep the RGB & throw away the top byte
                        // scanLineBytePointer format: ARGB - 0RGB as INT32 or BGR0 - 0RGB as INT32.
                        do
                            {
                            if ((copyLength < 4) || (3 & (TUint32)destPtr8))
                                {
                                color16M = *scanPtr32++;
                                --copyLength;
                                *destPtr8++ = TUint8(color16M);
                                *destPtr8++ = TUint8(color16M >> 8);
                                *destPtr8++ = TUint8(color16M >> 16);
                                }
                            else // dest is TUint32 aligned: copy 4 pixels into 3 TUint32's
                                {
                                color16M = (*scanPtr32++) & 0x00FFFFFF;
                                rawData = (*scanPtr32++) & 0x00FFFFFF;
                                copyLength -= 4;
                                *destPtr32++ = color16M | (rawData << 24);
                                color16M = (*scanPtr32++) & 0x00FFFFFF;
                                *destPtr32++ = (color16M << 16) | (rawData >> 8);
                                rawData = (*scanPtr32++) & 0x00FFFFFF;
                                *destPtr32++ = (color16M >> 16) | (rawData << 8);
                                }
                            }
                            while (copyLength);
                        }
                    break;
                    }
                default:
                    break;
                }
            }
        else
            { // !iHeader.iColor
            if (iHeader.iBitsPerPixel == 8)
                {
                scanPtr8 = reinterpret_cast<TUint8*>(aScanlinePtr) + aX;
                do
                    {
                    rawData = *scanPtr8++;
                    *destPtr8++ = (TUint8)rawData;
                    *destPtr8++ = (TUint8)rawData;
                    *destPtr8++ = (TUint8)rawData;
                    }
                    while (--copyLength);
                }
            else
                {
                do
                    {
                    rawData = GetGrayPixelEx(aX++,aScanlinePtr);
                    *destPtr8++ = (TUint8)rawData;
                    *destPtr8++ = (TUint8)rawData;
                    *destPtr8++ = (TUint8)rawData;
                    }
                    while (--copyLength);
                }
            }
        aX = 0; // second copy, if any, comes from start of line
        }

    return;
    }

void CBitwiseBitmap::GenerateLineFromCompressedEightBitData(TUint8* aDestBuffer, const TPoint& aPixel,TInt aLength, TUint32* aBase,TLineScanningPosition& aLineScanningPosition) const
	{
	const TInt bitmapWidth=iByteWidth;
	const TInt pixelsPerByte=8/iHeader.iBitsPerPixel;
	const TInt startPos=aPixel.iY*bitmapWidth+aPixel.iX/pixelsPerByte;
	const TInt endPos=aPixel.iY*bitmapWidth+(aPixel.iX+aLength+pixelsPerByte-1)/pixelsPerByte;
	const TInt byteLength=endPos-startPos;
	TInt writes=byteLength;
	TUint8* destPtr = ((TUint8*)aDestBuffer);
	destPtr+=(aPixel.iX/pixelsPerByte);
	if (aLineScanningPosition.iCursorPos>startPos)
		{
		aLineScanningPosition.iSrcDataPtr=(TUint8*)aBase;
		aLineScanningPosition.iCursorPos=0;
		}
	TUint8* srcePtr = (TUint8*)aLineScanningPosition.iSrcDataPtr;
	TInt8 count=*srcePtr;
	TInt16 addition;
	if (count<0)
		addition=(TInt16) (-count);
	else
		addition=(TInt16) (count+1);
	while (aLineScanningPosition.iCursorPos+addition<startPos)
		{
		aLineScanningPosition.iCursorPos+=addition;
		if (count<0)
			{
			srcePtr+=(-count+1);
			}
		else
			{
			srcePtr+=2;  // Just skip over value
			}
		count = *srcePtr;
		if (count<0)
			addition=(TInt16) (-count);
		else
			addition=(TInt16) (count+1);
		}
	// Then scan the line
	count=0;
	while (aLineScanningPosition.iCursorPos+count<startPos+byteLength)
		{
		TBool negativeCount=EFalse;
		count=*srcePtr;
		if (count<0)
			{
			negativeCount=ETrue;
			count=(TInt8) ((-count)-1);
			}
		TUint8 value = *(srcePtr+1);
		TInt distanceToTheLineEnd=startPos+byteLength-aLineScanningPosition.iCursorPos;
		if (count<distanceToTheLineEnd)
			{
			if (!negativeCount)
				{
				srcePtr+=2;
				}
			else
				{
				srcePtr+=1;
				}
			TInt countPlusOne = (TInt)count + 1;
			TInt start = Max(0,startPos-aLineScanningPosition.iCursorPos);
			if (countPlusOne > start)
				{
				TInt length = Min(countPlusOne-start,writes);
				writes -= countPlusOne-start;
				if (length > 0)
					{
					/*Mem::Fill and Mem::Copy used in order to increase the performance*/
					if (!negativeCount)
						{
						Mem::Fill(destPtr,length,value);
						}
					else
						{
						Mem::Copy(destPtr,srcePtr+start,length);
						}
					destPtr += length;
					}
				}
			if (negativeCount)
				{
				srcePtr += countPlusOne;
				}
			aLineScanningPosition.iCursorPos += countPlusOne;
			count=0;
			}
			else
			{
			TInt correction=1;
			if (aLineScanningPosition.iCursorPos<startPos)
				{
				correction=startPos-aLineScanningPosition.iCursorPos+1;
				}
			TInt length = Min(byteLength,writes);
			writes -= length;
			/*Mem::Fill and Mem::Copy used in order to increase the performance*/
			if (!negativeCount)
				{
				Mem::Fill(destPtr,length,value);
				}
			else
				{
				Mem::Copy(destPtr,srcePtr+correction,length);
				}
			destPtr += length;
			}
		}
	aLineScanningPosition.iSrcDataPtr=(TUint8*) srcePtr;
	}

void CBitwiseBitmap::GenerateLineFromCompressedTwelveBitData(TUint8* aDestBuffer, const TPoint& aPixel,TInt aLength, TUint32* aBase, TLineScanningPosition& aLineScanningPosition) const
	{
	const TInt bitmapWidth=iByteWidth>>1;
	const TInt startPos=aPixel.iY*bitmapWidth+aPixel.iX;
	TInt writes=aLength*2;
	TUint16* destPtr = ((TUint16*)aDestBuffer);//+aPixel.iX;
	destPtr+=aPixel.iX;

	if(iPile)
		{
		::AdjustLineScanningPosition(aLineScanningPosition, aBase, bitmapWidth, startPos, iHeader.iBitmapSize - sizeof(SEpocBitmapHeader));
		}
	TUint16* srcePtr = (TUint16*)aLineScanningPosition.iSrcDataPtr;

	// Fast find the correct position to start
	TInt count=0;
	if(::Abs(aLineScanningPosition.iCursorPos - startPos) > startPos)
		{
		srcePtr = (TUint16*)aBase;
		aLineScanningPosition.iCursorPos = 0;
		}
	while (aLineScanningPosition.iCursorPos>startPos)
		{
		srcePtr--;
		TUint16 value = *srcePtr;
		count = value >> 12;
		aLineScanningPosition.iCursorPos-=(count+1);
		}
	while (aLineScanningPosition.iCursorPos<startPos)
		{
		TUint16 value = *srcePtr++;
		count = value >> 12;
		aLineScanningPosition.iCursorPos+=count+1;
		}
	if (aLineScanningPosition.iCursorPos>startPos)
		{
		aLineScanningPosition.iCursorPos-=(count+1);
		srcePtr--;
		}

	// Then scan the line
	count=0;
	while (aLineScanningPosition.iCursorPos+count<startPos+aLength)
		{
		TUint16 value = *srcePtr;
		count = value >> 12;
		value &= 0x0fff;
		TInt distanceToTheLineEnd=startPos+aLength-aLineScanningPosition.iCursorPos;
		if (count<distanceToTheLineEnd)
			{
			srcePtr++;
			for (TInt ii=0 ; ii<=count ; ii++)
				{
				if (aLineScanningPosition.iCursorPos>=startPos)
					{
					if (writes>0)
						*destPtr++ = value;
					writes-=2;
					}
				aLineScanningPosition.iCursorPos++;

				}
			count=0;
			}
		else
			{
			for (TInt ii=0 ; ii<distanceToTheLineEnd ; ii++)
				{
				writes-=2;
				*destPtr++ = value;
				if (writes==0)
					break;
				}
			}
		}
	aLineScanningPosition.iSrcDataPtr=(TUint8*) srcePtr;
	}

/**
The method generates a line from compressed 16 bpp bitmap data.
@internalComponent
@see TScanLineDecompressor
*/
void CBitwiseBitmap::GenerateLineFromCompressedSixteenBitData(TUint8* aDestBuffer,
															  const TPoint& aPixel,
															  TInt aLength,
															  TUint32* aBase,
															  TLineScanningPosition& aLineScanningPosition) const
	{
	TInt comprDataBytes = iHeader.iBitmapSize - sizeof(SEpocBitmapHeader);
	TScanLineDecompressor<E2bpp, E2bpp> decompr(aBase, comprDataBytes, iPile!=NULL);
	decompr(aDestBuffer, aPixel, aLineScanningPosition, iByteWidth, iByteWidth, aLength);
	}

/**
The method generates a line from compressed 24 bpp bitmap data.
@internalComponent
@see TScanLineDecompressor
*/
void CBitwiseBitmap::GenerateLineFromCompressed24BitData(
									 TUint8* aDestBuffer,
									 const TPoint& aPixel,
									 TInt aLength,
									 TUint32* aBase,
									 TLineScanningPosition& aLineScanningPosition) const
	{
	TInt comprDataBytes = iHeader.iBitmapSize - sizeof(SEpocBitmapHeader);
	TScanLineDecompressor<E3bpp, E3bpp> decompr(aBase, comprDataBytes, iPile!=NULL);
	decompr(aDestBuffer, aPixel, aLineScanningPosition, iByteWidth, iByteWidth, aLength);
	}

/**
The method generates a line from compressed 24 bpp to 32 bpp bitmap data .
@internalComponent
@see TScanLineDecompressor
*/
void CBitwiseBitmap::GenerateLineFromCompressed32UBitData(
									 TUint8* aDestBuffer,
									 const TPoint& aPixel,
									 TInt aLength,
									 TUint32* aBase,
									 TLineScanningPosition& aLineScanningPosition) const
	{
	TInt comprDataBytes = iHeader.iBitmapSize - sizeof(SEpocBitmapHeader);
	TScanLineDecompressor<E3bpp, E4bpp> decompr(aBase, comprDataBytes, iPile!=NULL);

	TUint32 theByteWidthSrc = iHeader.iSizeInPixels.iWidth * 3;
	decompr(aDestBuffer, aPixel, aLineScanningPosition, theByteWidthSrc, iByteWidth, aLength);
	}

/**
The method generates a line from compressed 32 bpp to 32 bpp bitmap data .
@internalComponent
@see TScanLineDecompressor
*/
void CBitwiseBitmap::GenerateLineFromCompressed32ABitData(
 									 TUint8* aDestBuffer,
 									 const TPoint& aPixel,
 									 TInt aLength,
 									 TUint32* aBase,
 									 TLineScanningPosition& aLineScanningPosition) const
	{
	TInt comprDataBytes = iHeader.iBitmapSize - sizeof(SEpocBitmapHeader);
	TScanLineDecompressor<E4bpp, E4bpp> decompr(aBase, comprDataBytes, iPile!=NULL);

	TUint32 theByteWidthSrc = iHeader.iSizeInPixels.iWidth * 4;
	decompr(aDestBuffer, aPixel, aLineScanningPosition, theByteWidthSrc, iByteWidth, aLength);
	}

TInt CBitwiseBitmap::DoGetScanLinePtr(TUint32*& aSlptr, TPoint& aPixel,TInt aLength,TUint32* aBase, TLineScanningPosition& aLineScanningPosition) const
	{
	TUint8* buf=NULL;
	HBufC8* hBuf=aLineScanningPosition.iScanLineBuffer;
	if (!hBuf)
		{
		RFbsSession* session=RFbsSession::GetSession();
		if (session)
			{
			hBuf=session->GetScanLineBuffer();
			}
		else
			{
			aSlptr=NULL;
			return KErrSessionClosed;
			}
		aLineScanningPosition.iScanLineBuffer=hBuf;
		}
	__ASSERT_ALWAYS(hBuf && hBuf->Des().MaxLength() >= iByteWidth, User::Invariant());
	buf = const_cast<TUint8*>(hBuf->Ptr());
	switch(iHeader.iCompression)
		{
		case ETwelveBitRLECompression:
			GenerateLineFromCompressedTwelveBitData(buf, aPixel,aLength, aBase, aLineScanningPosition);
			break;
		case EByteRLECompression:
			GenerateLineFromCompressedEightBitData(buf, aPixel,aLength, aBase, aLineScanningPosition);
			break;
		case ESixteenBitRLECompression:
			GenerateLineFromCompressedSixteenBitData(buf, aPixel,aLength, aBase, aLineScanningPosition);
			break;
		case ETwentyFourBitRLECompression:
			GenerateLineFromCompressed24BitData(buf, aPixel, aLength, aBase, aLineScanningPosition);
			break;
		case EThirtyTwoUBitRLECompression:
			GenerateLineFromCompressed32UBitData(buf, aPixel, aLength, aBase, aLineScanningPosition);
			break;
		case EThirtyTwoABitRLECompression:
			GenerateLineFromCompressed32ABitData(buf, aPixel, aLength, aBase, aLineScanningPosition);
			break;
		case EGenericPaletteCompression:
			GenerateLineFromPaletteCompressedData(buf, aPixel, aLength, aBase, aLineScanningPosition);
			break;
		case EProprietaryCompression:
			if (aLineScanningPosition.iRasterizer)
				{
				aSlptr = const_cast<TUint32*>(aLineScanningPosition.iRasterizer->ScanLine(Extra()->iSerialNumber, aPixel, aLength));
				if (aSlptr)
					{
					return KErrNone;
					}
				}
			WhiteFill(buf, iByteWidth, iSettings.CurrentDisplayMode());
			break;
		default:
			{
			__ASSERT_DEBUG(EFalse, ::Panic(EFbsBitmapInvalidCompression));
			return KErrNotSupported;
			}
		}
	aSlptr = (TUint32*) buf;
	return KErrNone;
	}

EXPORT_C void CBitwiseBitmap::GetScanLine(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TBool aDither,const TPoint& aDitherOffset,TDisplayMode aDispMode,TUint32* aBase,TLineScanningPosition& aLineScanningPosition) const
	{
	if (!iDataOffset)
		return;

	TPoint pixel(aPixel);
	TUint32* slptr=NULL;
	GetScanLinePtr(slptr, aLength, pixel,aBase, aLineScanningPosition);
	GetScanLine(slptr,aBuf,pixel,aLength,aDither,aDitherOffset,aDispMode);
	}

EXPORT_C void CBitwiseBitmap::GetScanLine(TUint32*& aScanLinePtr, TDes8& aDestBuf,const TPoint& aPixel,TInt aLength,TBool aDither,
	const TPoint& aDitherOffset,TDisplayMode aDestinationDispMode) const
	{
	if (!iDataOffset)
		return;
	TDisplayMode currentDisplayMode = iSettings.CurrentDisplayMode();
	if (!aScanLinePtr) // if scanline pointer is null,
		{
		WhiteFill((TUint8*)aDestBuf.Ptr(),aDestBuf.MaxLength(),currentDisplayMode);
		return;
		}
	TUint8* ptr = (TUint8*)aDestBuf.Ptr();
	// if dest pointer is not aligned
	if (!(TUint32(ptr)&3) && aDestinationDispMode == currentDisplayMode)
		{
		if (iHeader.iBitsPerPixel < 8)
			GetScanLineExBits(aDestBuf,aPixel.iX,aLength,aScanLinePtr);
		else
			GetScanLineExBytes(aDestBuf,aPixel.iX,aLength,aScanLinePtr);
		return;
		}
	//read the scanline in destination display format.
	switch (aDestinationDispMode)
		{
	case EGray2:
		GetScanLineGray2(aDestBuf,aPixel,aLength,aDither,aDitherOffset,aScanLinePtr);
		break;
	case EGray4:
		GetScanLineGray4(aDestBuf,aPixel,aLength,aDither,aDitherOffset,aScanLinePtr);
		break;
	case EGray16:
		GetScanLineGray16(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EGray256:
		GetScanLineGray256(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EColor16:
		GetScanLineColor16(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EColor256:
		GetScanLineColor256(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EColor4K:
		GetScanLineColor4K(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EColor64K:
		GetScanLineColor64K(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EColor16M:
		GetScanLineColor16M(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case ERgb:
		GetScanLineColorRgb(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EColor16MU:
		GetScanLineColor16MU(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EColor16MA:
		GetScanLineColor16MA(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	case EColor16MAP:
		GetScanLineColor16MAP(aDestBuf,aPixel,aLength,aScanLinePtr);
		break;
	default:
		aDestBuf.SetLength(0);
		break;
		};
	}

EXPORT_C void CBitwiseBitmap::GetScanLine(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TBool aDither,const TPoint& aDitherOffset,TDisplayMode aDispMode,TUint32* aBase) const
	{
	TLineScanningPosition pos(aBase);
	TUint8* base = REINTERPRET_CAST(TUint8*,aBase);
	const TCompressionBookMark* bookMarkPtr = NULL;

	GetLineScanPos(pos, bookMarkPtr, base);
	GetScanLine(aBuf,aPixel,aLength,aDither,aDitherOffset,aDispMode,aBase,pos);
	UpdateBookMark(pos, const_cast<TCompressionBookMark*>(bookMarkPtr), base);
	}

/**
Gets the bitmap’s vertical scanline starting at the specified x co-ordinate and using
the specified dither offset.
Note: The method works for uncompressed bitmaps only.
@param aBuf The buffer in which the vertical scanline will be returned.
@param aX The x co-ordinate of the vertical scanline to get.
@param aDitherOffset The dither offset of the bitmap.
@param aDispMode Format to be used to write the data to the buffer.
@param aBase The bitmap's data start address.
*/
EXPORT_C void CBitwiseBitmap::GetVerticalScanLine(TDes8& aBuf,TInt aX,TBool aDither,
												  const TPoint& aDitherOffset,
												  TDisplayMode aDispMode,
												  TUint32* aBase,
												  CFbsRasterizer* aRasterizer) const
	{
	if (iHeader.iCompression != ENoBitmapCompression && iHeader.iCompression != EProprietaryCompression)
		{
		__ASSERT_DEBUG(EFalse, ::Panic(EFbsBitmapInvalidCompression));
		return; //not supported for compressed bitmaps
		}

	if (!iDataOffset)
		{
		return;
		}
	AdjustXCoord(aX);
	TInt height=iHeader.iSizeInPixels.iHeight;
	TUint32* slptr=aBase;
	TUint8* ptr = (TUint8*)aBuf.Ptr();
	*ptr=0;
	const TInt wordwidth=iByteWidth>>2;
	TInt y = 0;

	if (iHeader.iCompression == EProprietaryCompression)
		{
		if (aRasterizer)
			{
			slptr = const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,0), 1));
			if (!slptr)
				{
				// wrong rasterizer for this extended bitmap - fill vertical scan line with white pixels
				TInt bufLength = ByteWidth(height, aDispMode);
				aBuf.SetLength(bufLength);
				WhiteFill(ptr, bufLength, aDispMode);
				return;
				}
			}
		else
			{
			// no rasterizer - fill vertical scan line with white pixels
			TInt bufLength = ByteWidth(height, aDispMode);
			aBuf.SetLength(bufLength);
			WhiteFill(ptr, bufLength, aDispMode);
			return;
			}
		}

	switch(aDispMode)
		{
		case EGray2:
			{
			TBool oddx=(aDitherOffset.iX&1);
			TBool oddy=(aDitherOffset.iY&1);
			height=Min(height,(TInt)((aBuf.MaxLength())<<3));
			aBuf.SetLength((height+7)>>3);
			TUint8 mask=1;
			for(TInt count=0;count<height;count++)
				{
				if (!mask)
					{
					mask=1;
					ptr++;
					*ptr = 0;
					}
				if (HashTo1bpp(GetGrayPixelEx(aX,slptr),oddx,oddy))
					*ptr|=mask;
				oddx^=1;
				mask<<=1;
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			 break;
			}
		case EGray4:
			{
			height=Min(height,(TInt)((aBuf.MaxLength())<<2));
			aBuf.SetLength((height+3)>>2);
			TInt shift=0;
			TUint8 col=0;
			if (iHeader.iBitsPerPixel==4 && aDither)
				{
				const TInt hasharray[4]={0,3,2,1};
				TInt index=(aDitherOffset.iX&1)+((aDitherOffset.iY&1)<<1);
				for(TInt count=0;count<height;count++,shift+=2)
					{
					if (shift==8)
						{
						shift=0;
						ptr++;
						*ptr=0;
						}
					col = TUint8(GetGrayPixelEx(aX,slptr) >> 4);
					TInt value = col / 5;
					col%=5;
					if (col>2) col--;
					if (hasharray[index]<TInt(col))
						value++;
					value<<=shift;
					*ptr|=value;
					index^=1;
					slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
					}
				}
			else
				{
				for(TInt count=0;count<height;count++,shift+=2)
					{
					if (shift==8)
						{
						shift=0;
						ptr++;
						*ptr=0;
						}
					col = TUint8(GetGrayPixelEx(aX,slptr) >> 6);
					col<<=shift;
					*ptr|=col;
					slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
					}
				}
			 break;
			}
		case EGray16:
			{
			height = Min(height,aBuf.MaxLength()<<1);
			aBuf.SetLength((height+1)>>1);
			TUint8* ptrLimit = ptr + aBuf.Length() - 1;
			while (ptr < ptrLimit)
				{
				*ptr = TUint8(GetGrayPixelEx(aX,slptr) >> 4);
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				*ptr++ |= GetGrayPixelEx(aX,slptr) & 0xf0;
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			// Fill last byte.
			// If height is odd, upper 4 bits are zeroed.
			*ptr = TUint8(GetGrayPixelEx(aX,slptr) >> 4);
			if (!(height & 1))
				{
				// Only fill upper 4 bits of last byte if height is even. 
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				*ptr++ = GetGrayPixelEx(aX,slptr) & 0xf0;
				}
			break;
			}
		case EColor16:
			{
			height=Min(height,aBuf.MaxLength()<<1);
			aBuf.SetLength((height+1)>>1);
			TUint8* ptrLimit = ptr + aBuf.Length() - 1;

			while (ptr < ptrLimit)
				{
				*ptr = TUint8(GetRgbPixelEx(aX,slptr).Color16());
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				*ptr++ |= GetRgbPixelEx(aX,slptr).Color16() << 4;
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			// Fill last byte.
			// If height is odd, upper 4 bits are zeroed.
			*ptr = TUint8(GetRgbPixelEx(aX,slptr).Color16());
			if (!(height & 1))
				{
				// Only fill upper 4 bits of last byte if height is even. 
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				*ptr++ = GetRgbPixelEx(aX,slptr).Color16() << 4;
				}
			break;
			}
		case EGray256:
			{
			height = Min(height,aBuf.MaxLength());
			aBuf.SetLength(height);
			TUint8* ptrLimit = ptr + height;
			while (ptr < ptrLimit)
				{
				*ptr++ = GetGrayPixelEx(aX,slptr);
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			break;
			}
		case EColor256:
			{
			height = Min(height,aBuf.MaxLength());
			aBuf.SetLength(height);
			TUint8* ptrLimit = ptr + height;

			while (ptr < ptrLimit)
				{
				*ptr++ = TUint8(GetRgbPixelEx(aX,slptr).Color256());
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			break;
			}
		case EColor4K:
			{
			height = Min(height,aBuf.MaxLength() >> 1);
			aBuf.SetLength(height << 1);
			TUint16* dwordPtr = (TUint16*)ptr;
			TUint16* ptrLimit = dwordPtr + height;

			while (dwordPtr < ptrLimit)
				{
				*dwordPtr++ = TUint16(GetRgbPixelEx(aX,slptr)._Color4K());
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			break;
			}
		case EColor64K:
			{
			height = Min(height,aBuf.MaxLength() >> 1);
			aBuf.SetLength(height << 1);
			TUint16* dwordPtr = (TUint16*)ptr;
			TUint16* ptrLimit = dwordPtr + height;

			while (dwordPtr < ptrLimit)
				{
				*dwordPtr++ = TUint16(GetRgbPixelEx(aX,slptr)._Color64K());
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			break;
			}
		case EColor16M:
			{
			height = Min(height,aBuf.MaxLength() / 3);
			aBuf.SetLength(height * 3);
			const TUint8* ptrLimit = ptr + (height * 3);

			while (ptr < ptrLimit)
				{
				const TInt color16M = GetRgbPixelEx(aX,slptr)._Color16M();
				*ptr++ = TUint8(color16M);
				*ptr++ = TUint8(color16M >> 8);
				*ptr++ = TUint8(color16M >> 16);
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			break;
			}
		case ERgb:
		case EColor16MU:
		case EColor16MA:
		case EColor16MAP:
			{
			height = Min(height,aBuf.MaxLength() >> 2);
			aBuf.SetLength(height << 2);
			TUint32* pixelPtr = (TUint32*)ptr;
			TUint32* pixelPtrLimit = pixelPtr + height;

			if (aDispMode == EColor16MAP && iSettings.CurrentDisplayMode() == EColor16MA)
				while (pixelPtr < pixelPtrLimit)
				{
				*pixelPtr++ = NonPMA2PMAPixel(*(slptr + aX));
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			else if (aDispMode == EColor16MAP && iSettings.CurrentDisplayMode() == EColor16MAP)
				while (pixelPtr < pixelPtrLimit)
				{
				*pixelPtr++ = *(slptr + aX);
				slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
				}
			else if (aDispMode == EColor16MU)
				while (pixelPtr < pixelPtrLimit)
					{
					*pixelPtr++ = GetRgbPixelEx(aX, slptr).Internal() | 0xFF000000;
					slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
					}
			else
				while (pixelPtr < pixelPtrLimit)
					{
					*pixelPtr++ = GetRgbPixelEx(aX, slptr).Internal();
					slptr = aRasterizer ? const_cast<TUint32*>(aRasterizer->ScanLine(Extra()->iSerialNumber, TPoint(aX,++y), 1)) : slptr + wordwidth;
					}
			break;
			}
		default:
			aBuf.SetLength(0);
		}
	}

EXPORT_C void CBitwiseBitmap::StretchScanLine(TDes8& aBuf,const TPoint& aPixel,TInt aClipStrchX,TInt aClipStrchLen,TInt aStretchLength,TInt aOrgX,TInt aOrgLen,const TPoint& aDitherOffset,TDisplayMode aDispMode,TUint32* aBase) const
	{
	TLineScanningPosition pos(aBase);
	StretchScanLine(aBuf,aPixel,aClipStrchX,aClipStrchLen,aStretchLength,aOrgX,aOrgLen,aDitherOffset,aDispMode,aBase,pos);
	}

EXPORT_C void CBitwiseBitmap::StretchScanLine(TDes8& aBuf,const TPoint& aPixel,TInt aClipStrchX,TInt aClipStrchLen,TInt aStretchLength,TInt aOrgX,TInt aOrgLen,const TPoint& aDitherOffset,TDisplayMode aDispMode,TUint32* aBase, TLineScanningPosition& aLineScanningPosition) const
	{
	if (!iDataOffset)
		return;
	TInt x=aPixel.iX,y=aPixel.iY;
	if (x>=iHeader.iSizeInPixels.iWidth || x<-iHeader.iSizeInPixels.iWidth)
		x%=iHeader.iSizeInPixels.iWidth;
	if (y>=iHeader.iSizeInPixels.iHeight || y<-iHeader.iSizeInPixels.iHeight)
		y%=iHeader.iSizeInPixels.iHeight;
	if (x<0) x+=iHeader.iSizeInPixels.iWidth;
	if (y<0) y+=iHeader.iSizeInPixels.iHeight;
	if (aStretchLength<aOrgLen)
		DoCompressScanLine(aBuf,x,y,aClipStrchX,aClipStrchLen,aStretchLength,aOrgX,aOrgLen,aDitherOffset,aDispMode,aBase,aLineScanningPosition);
	else
		DoStretchScanLine(aBuf,x,y,aClipStrchX,aClipStrchLen,aStretchLength,aOrgX,aOrgLen,aDitherOffset,aDispMode,aBase,aLineScanningPosition);
	}

EXPORT_C TUint32* CBitwiseBitmap::ScanLineAddress(TUint32* aBase,TUint aY) const
	{
	if (aY == 0 || iDataOffset == 0)
		return aBase;

	if (aY >= TUint(iHeader.iSizeInPixels.iHeight))
		aY %= iHeader.iSizeInPixels.iHeight;

	return aBase + (aY * (DataStride() >> 2));
	}

TUint32* CBitwiseBitmap::DataAddress() const
	{
	if (iDataOffset==0) return(NULL);

	if(iUid.iUid==KCBitwiseBitmapHardwareUid.iUid)   // RHardwareBitmap
		{
		RHardwareBitmap hwb(iDataOffset);	// iDataOffset = handle for hardware bitmap
		TAcceleratedBitmapInfo info;
		const TInt ret = hwb.GetInfo(info);
		return ret!=KErrNone ? NULL : (reinterpret_cast<TUint32*>(info.iAddress));
		}

	if (iHeap == NULL)
		return(reinterpret_cast<TUint32*>((TUint8*)this+iDataOffset));
	return(reinterpret_cast<TUint32*>(iPile->ChunkBase()+iDataOffset));
	}

EXPORT_C TInt CBitwiseBitmap::DataStride() const
	{
	return iByteWidth;
	}

TUint32 CBitwiseBitmap::HashTo2bpp(TUint32 aGray256,TInt aDitherIndex) const
	{
	static const TUint hasharray[4]={0,3,2,1};
	TInt gray16 = aGray256 >> 4;
	TInt gray4 = gray16 + 1;
	gray4 += gray4 << 1;
	gray4 >>= 4;
	gray16 %= 5;
	if (gray16 > 2)
		gray16--;
	if (hasharray[aDitherIndex] < TUint(gray16))
		gray4++;
	return gray4;
	}

TUint32 CBitwiseBitmap::HashTo1bpp(TUint32 aGray256,TBool aOddX,TBool aOddY) const
	{
	TUint32 aGray4 = aGray256 >> 6;
	switch(aGray4)
		{
		case 3:
			return 1;
		case 2:
			{
			if (aOddX && aOddY)
				return 0;
			else
				return 1;
			}
		case 1:
			{
			if ((aOddX && aOddY) || (!aOddX && !aOddY))
				return 1;
			}
			//coverity [fallthrough]
		default:
			return 0;
		}
	}

/**
Tests whether or not the bitmap is monochrome.
Monochrome bitmaps have a display-mode of 1 bit-per-pixel.
Note: The method works for uncompressed bitmaps only.
@param aBase Bitmap's data base address
@return True if the bitmap is monochrome; false otherwise.
*/
EXPORT_C TBool CBitwiseBitmap::IsMonochrome(TUint32* aBase) const
	{
	if (IsCompressed())
		{
		__ASSERT_DEBUG(EFalse, ::Panic(EFbsBitmapInvalidCompression));
		return EFalse;  // Not currently supported for compressed bitmaps
		}

	if (!iDataOffset)
		{
		return(EFalse);
		}
	TInt bitwidth=iHeader.iBitsPerPixel*iHeader.iSizeInPixels.iWidth;
	if(iHeader.iBitsPerPixel == 12)
		{//EColor4K mode - 1 pixel occupies 16 bits, most significant 4 bits are not used.
		bitwidth=16*iHeader.iSizeInPixels.iWidth;
		}
	TInt wordwidth=bitwidth>>5;
	TInt endshift=32-(bitwidth&0x1f);
	TInt endmask=0;
	if (endshift<32) endmask=0xffffffff>>endshift;
	TUint32* bitptr=aBase;
	//In a loop from first to last scanline:
	//Check each pixel - is it monochrome or not (pixel color must be BLACK or WHITE).
	//Get next scanline.
	TUint32* endbitptr=bitptr+wordwidth;
	for(TInt row=0;row<iHeader.iSizeInPixels.iHeight;row++)
		{
		if(iHeader.iBitsPerPixel == 24)
			{//1 word contains 1 pixel and 8 bits from the next pixel.
			for(TInt x=0;x<iHeader.iSizeInPixels.iWidth;x++)
				{
				TUint8* scanLine = reinterpret_cast <TUint8*> (bitptr) + x * 3;
				TUint color16M = *scanLine++;
				color16M |= (*scanLine++) << 8;
				color16M |= (*scanLine++) << 16;
				if (IsWordMonochrome(color16M)==EFalse)
					return(EFalse);
				}
			}
		else
			{
			TUint32* tmpbitptr=bitptr;
			while(tmpbitptr<endbitptr)
				if (IsWordMonochrome(*tmpbitptr++)==EFalse)
					return(EFalse);
			if (endmask)
				if (IsWordMonochrome(*endbitptr&endmask)==EFalse)
					return(EFalse);
			}
		bitptr+=wordwidth;
		endbitptr+=wordwidth;
		}
	return(ETrue);
	}

TBool CBitwiseBitmap::IsWordMonochrome(TUint32 aWord) const
	{
	TDisplayMode displayMode = iSettings.CurrentDisplayMode();
	switch(displayMode)
		{
		case EGray2:
			return ETrue;
		case EGray4:
			{
			TUint32 lowerbits=aWord&0x55555555;
			TUint32 upperbits=(aWord>>1)&0x55555555;
			if (lowerbits^upperbits)
				return EFalse;
			return ETrue;
			}
		case EGray16:
		case EColor16:
			{
			if (aWord==0xffffffff || aWord==0)
				return ETrue;
			for(TInt count=0;count<8;count++)
				{
				TUint32 nibble=aWord&0xf;
				if ((nibble>0) && (nibble<0xf))
					return EFalse;
				aWord>>=4;
				}
			return ETrue;
			}
		case EGray256:
		case EColor256:
			{
			TUint8* bytePtr = (TUint8*)&aWord;
			TUint8* bytePtrLimit = bytePtr + 4;
			while (bytePtr < bytePtrLimit)
				{
				if (*bytePtr && (*bytePtr != 0xff))
					return EFalse;
				bytePtr++;
				}
			return ETrue;
			}
		case EColor4K:
			{
			aWord &= 0x0fff0fff;
			TUint16 color4K = (TUint16)aWord;
			if (color4K && (color4K != 0xfff))
				return EFalse;
			color4K = (TUint16)(aWord >> 16);
			if (color4K && (color4K != 0xfff))
				return EFalse;
			return ETrue;
			}
		case EColor64K:
			{
			TUint16 color64K = (TUint16)aWord;
			if (color64K && (color64K != 0xffff))
				return EFalse;
			color64K = (TUint16)(aWord >> 16);
			if (color64K && (color64K != 0xffff))
				return EFalse;
			return ETrue;
			}
		case EColor16M:
		case EColor16MU:
		case EColor16MA:
		case EColor16MAP:
			{
			aWord &= 0xffffff;
			if (aWord && (aWord != 0x00ffffff))
				return EFalse;
			return ETrue;
			}
		default:
			return EFalse;
		}
	}

EXPORT_C TBool CBitwiseBitmap::IsLargeBitmap() const
	{
	if(iUid.iUid==KCBitwiseBitmapHardwareUid.iUid)
		return EFalse; // RHardwareBitmap

	if (iHeap==NULL) return(EFalse); // rom bitmap

	// Consider all RAM bitmaps large, so that legacy applications always
	// call LockHeap()/UnlockHeap() around DataAddress(), which allows
	// better handling of hardware acceleration caches, if present.
	// Note that, since the large bitmap threshold has always been in the
	// documentation, it is not guaranteed that legacy applications call
	// this function to determine whether a bitmap is large or not.
	return ETrue;
	}

EXPORT_C TInt CBitwiseBitmap::HardwareBitmapHandle() const
	{
	if(iUid.iUid!=KCBitwiseBitmapHardwareUid.iUid)
		return 0;
	return iDataOffset; // Really the handle
	}

/**
Set a flag to indicate that this bitmap has to be compressed in the FBServer background thread
@return KErrNone if possible to compress, KErrAlreadyExists if already compressed
*/
EXPORT_C TInt CBitwiseBitmap::CheckBackgroundCompressData()
	{
	switch (iHeader.iBitsPerPixel)
		{
	case 1:
	case 2:
	case 4:
	case 8:
	case 12:
	case 16:
	case 24:
	case 32:
		break;
	default:
		return KErrNotSupported;
		}
	// Return if the bitmap is already compressed.
	if (iHeader.iCompression != ENoBitmapCompression)
		return KErrAlreadyExists;

	// See if it's possible to allocate memory.
	if (iHeap == NULL || iPile == NULL)
		return KErrNoMemory;

	return KErrNone;
	}



/**
Compress a bitmap if possible.
If the bitmap is already compressed, or if compression yields no decrease in size, do nothing,
but return success (KErrNone).
@return KErrNone if successful, otherwise a system wide error code.
*/
EXPORT_C TInt CBitwiseBitmap::CompressData()
	{
	switch (iHeader.iBitsPerPixel)
		{
	case 1:
	case 2:
	case 4:
	case 8:
	case 12:
	case 16:
	case 24:
	case 32:
		break;
	default:
		return KErrNone;
		}
	// Return if the bitmap is already compressed.
	if (iHeader.iCompression != ENoBitmapCompression)
		return KErrNone;

	// Find out if compression is possible and return if not.
	TUint8* data = (TUint8*)DataAddress();
	TInt data_bytes = iHeader.iBitmapSize - iHeader.iStructSize;

	//Update the padding bitmap data to speedup the RLE Compression
	UpdatePaddingData((TUint32*)data);

	TInt compressed_data_bytes = (SizeOfDataCompressed((TUint8*)data,data_bytes) + 3) / 4 * 4;

	if (!data || !data_bytes)
		return KErrNone;

	//  	 if (compressed_data_bytes >= data_bytes)
	//  It now attempts to check whether compression is worth while.  (speed vs space saving)
	__ASSERT_DEBUG(KCompressionThreshold >=0 && KCompressionThreshold <= 256, ::Panic(EFbsInvalidCompressionThreshold));
	if (compressed_data_bytes >= (data_bytes * KCompressionThreshold) >> 8)
		return KErrNone;

	// See if it's possible to allocate memory.
	if (iHeap == NULL || iPile == NULL)
		return KErrNoMemory;

	// Create a buffer to receive the compressed data.
	TUint8* compressed_data = NULL;
	TInt allocSize = compressed_data_bytes;
	TBool bookMark = EFalse;
	if (allocSize > KCompressionBookMarkThreshold)
		{
		allocSize += sizeof(TCompressionBookMark) + 4;
		bookMark = ETrue;
		}
	compressed_data = iPile->Alloc(allocSize);
	if (!compressed_data)
		return KErrNoMemory;
	if (bookMark)
		{
		TCompressionBookMark emptyBookmark;
		*((TCompressionBookMark*)(compressed_data + compressed_data_bytes + 4)) = emptyBookmark;
		}
	iDataOffset = compressed_data - iPile->ChunkBase();
	iHeader.iBitmapSize = sizeof(SEpocBitmapHeader) + compressed_data_bytes;
	iHeader.iCompression = CompressionType(iHeader.iBitsPerPixel, iHeader.iColor);

	// Compress the data into a stream over the new buffer.
	TPtr8 output_ptr(compressed_data,compressed_data_bytes);
	RDesWriteStream output_stream(output_ptr);
	// This function cannot leave - but trap it anyway till I am fully satisfied about that.
	TRAP_IGNORE(DoExternalizeDataCompressedL(output_stream,data,data_bytes));
	output_stream.Close();

	iIsCompressedInRAM = ETrue;
	// Free the old data.
	iPile->Free(data);

	return KErrNone;
	}

/**
Compress a bitmap if possible.
If the bitmap is already compressed, or if compression yields no decrease in size, do nothing,
but return success (KErrNone).
@publishedAll
@param aScheme The type of bitmap file compression.
@return KErrNone if successful, otherwise a system wide error code.
*/
EXPORT_C TInt CBitwiseBitmap::CompressData(TBitmapfileCompressionScheme aScheme)
	{
	TInt err=KErrNone;
	if (aScheme==ERLECompression)
		err=CompressData();
	else if (aScheme==EPaletteCompression)
		err=PaletteCompress();
	else if (aScheme==EPaletteCompressionWithRLEFallback)
		{
		err=PaletteCompress();
		if (err==KErrNotSupported)
			err=CompressData();
		}

	return err;
	}

EXPORT_C TBool CBitwiseBitmap::IsCompressedInRAM() const
	{
	return iIsCompressedInRAM;
	}

/**
Check for a bitmap if it is compressed in some manner.
@return ETrue if successful Or EFalse if unsuccessful
@internalComponent
*/
EXPORT_C TBool CBitwiseBitmap::IsCompressed() const
	{
	return ( iHeader.iCompression != ENoBitmapCompression );
	}

EXPORT_C void CBitwiseBitmap::SetCompressionBookmark(TLineScanningPosition& aLineScanningPosition, TUint32* aBase, const CFbsBitmap* /*aFbsBitmap*/)
	{
	if (iPile == NULL) return; //Rom bitmap
	if (!iIsCompressedInRAM || (iHeader.iCompression == EGenericPaletteCompression))
		{
		return;
		}

	TInt compressed_data_bytes=iHeader.iBitmapSize-sizeof(SEpocBitmapHeader);
	if (compressed_data_bytes>KCompressionBookMarkThreshold)
		{
		TUint8* compressed_data=(TUint8*) aBase;
		TInt alignedSize=(compressed_data_bytes+3)/4*4;
		compressed_data+=alignedSize+4;
		TCompressionBookMark* bookMark=(TCompressionBookMark*) (compressed_data);
		if (!bookMark->IsCheckSumOk())
			return;
		bookMark->iCursorPos=aLineScanningPosition.iCursorPos;
		bookMark->iSrcDataOffset=aLineScanningPosition.iSrcDataPtr-((TUint8*)aBase);
		bookMark->CalculateCheckSum();
		}
	}
/**
Optimises the bitmap data for Run Length Encoding by changing unused
pixel data.
This function calculates number of padding pixels per scanline and
replaces their color with the color of the scanline's final pixel.
*/
void CBitwiseBitmap::UpdatePaddingData(TUint32* aData)
	{
	TInt stride=DataStride();
	//Do the process only for 8bpp and 16bpp.
	switch (iHeader.iBitsPerPixel)
		{
	case 8:
		{
		const TInt nPadding = stride - iHeader.iSizeInPixels.iWidth;
		if(nPadding!=1 && nPadding!=2 && nPadding!=3)
			return;
		TUint8* srcePtr = reinterpret_cast<TUint8*>(aData);
		//Find the last byte value in each scanline and assign in padding bytes
		TUint8* lastPixelPtr = srcePtr + iHeader.iSizeInPixels.iWidth - 1;
		for(TInt row=0; row<iHeader.iSizeInPixels.iHeight; row++)
			{
			TUint8 pixel = *lastPixelPtr;
			TUint8* padPtr = lastPixelPtr + 1;
			switch(nPadding)
				{
			case 3: *padPtr++ = pixel;
			case 2: *padPtr++ = pixel;
			case 1: *padPtr++ = pixel;
				}
			lastPixelPtr += stride;
			}
		break;
		}
	case 16:
		{
		TUint16* srcePtr = reinterpret_cast<TUint16*>(aData);
		stride>>=1;
		const TInt nPadding = stride - iHeader.iSizeInPixels.iWidth;
		if(nPadding!=1)
			return;
		//Find the last byte value in each scanline and assign in padding bytes
		TUint16* lastPixelPtr = srcePtr + iHeader.iSizeInPixels.iWidth - 1;
		for(TInt row=0; row<iHeader.iSizeInPixels.iHeight; row++)
			{
			TUint16 pixel = *lastPixelPtr;
			TUint16* padPtr = lastPixelPtr + 1;
			*padPtr++ = pixel;
			lastPixelPtr += stride;
			}
		break;
		}
	default:
		return;
		}
	}

void CBitwiseBitmap::WhiteFill(TUint8* aData,TInt aDataSize,TDisplayMode aDispMode)
	{
	if(aData)
		{
		if (aDispMode != EColor4K)
			Mem::Fill(aData,aDataSize,0xff);
		else
			{
			TUint16* pixelPtr = (TUint16*)aData;
			TUint16* pixelPtrLimit = pixelPtr + (aDataSize / 2);
			while (pixelPtr < pixelPtrLimit)
				*pixelPtr++ = 0x0fff;
			}		
		}
	}

TInt CBitwiseBitmap::ByteWidth(TInt aPixelWidth,TDisplayMode aDispMode)
	{
	TInt wordWidth = 0;

	switch (aDispMode)
		{
	case EGray2:
		wordWidth = (aPixelWidth + 31) / 32;
		break;
	case EGray4:
		wordWidth = (aPixelWidth + 15) / 16;
		break;
	case EGray16:
	case EColor16:
		wordWidth = (aPixelWidth + 7) / 8;
		break;
	case EGray256:
	case EColor256:
		wordWidth = (aPixelWidth + 3) / 4;
		break;
	case EColor4K:
	case EColor64K:
		wordWidth = (aPixelWidth + 1) / 2;
		break;
	case EColor16M:
		wordWidth = (((aPixelWidth * 3) + 11) / 12) * 3;
		break;
	case EColor16MU:
	case ERgb:
	case EColor16MA:
	case EColor16MAP:
		wordWidth = aPixelWidth;
		break;
	default:
		::Panic(EFbsBitmapInvalidMode);
		}

	return wordWidth * 4;
	}

TInt CBitwiseBitmap::Bpp(TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
		return 1;
	case EGray4:
		return 2;
	case EGray16:
	case EColor16:
		return 4;
	case EGray256:
	case EColor256:
		return 8;
	case EColor4K:
		return 12;
	case EColor64K:
		return 16;
	case EColor16M:
		return 24;
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		return 32;
	default:
		::Panic(EFbsBitmapInvalidMode);
		}

	return 0;
	}

TInt CBitwiseBitmap::CompressedFormatInfo(TDisplayMode aDispMode, TInt& aBytesPerPack, TInt& aBytesPerCompressed)
	{
	switch (aDispMode)
		{
		case EGray2:
		case EGray4:
		case EGray16:
		case EColor16:
		case EGray256:
		case EColor256:
			aBytesPerPack = 1;
			aBytesPerCompressed = 1;
			break;
		case EColor4K:
		case EColor64K:
			aBytesPerPack = 2;
			aBytesPerCompressed = 2;
			break;
		case EColor16M:
			aBytesPerPack = 3;
			aBytesPerCompressed = 3;
			break;
		case EColor16MU:
			aBytesPerPack = 4;
			aBytesPerCompressed = 3;		 		  //when compressed, 16MU is stored as 16M 
			break;
		case EColor16MA:
		case EColor16MAP:
			aBytesPerPack = 4;
			aBytesPerCompressed = 4;
			break;
		default:
			__ASSERT_DEBUG(0, ::Panic(EFbsBitmapInvalidMode));
			return KErrNotSupported;
		}

	return KErrNone;
	}
TInt CBitwiseBitmap::IsColor(TDisplayMode aDispMode)
	{
	switch (aDispMode)
		{
	case EGray2:
	case EGray4:
	case EGray16:
	case EGray256:
		return SEpocBitmapHeader::ENoColor;
	case EColor16:
	case EColor256:
	case EColor4K:
	case EColor64K:
	case EColor16M:
	case EColor16MU:
		return SEpocBitmapHeader::EColor;
	case EColor16MA:
		return SEpocBitmapHeader::EColorAlpha;
	case EColor16MAP:
		return SEpocBitmapHeader::EColorAlphaPM;
	default:
		::Panic(EFbsBitmapInvalidMode);
		}

	return SEpocBitmapHeader::EColorUndefined;
	}

TDisplayMode CBitwiseBitmap::DisplayMode(TInt aBpp, TInt aColor)
	{
	if (aColor)
		{
		switch (aBpp)
			{
		case 4:
			return EColor16;
		case 8:
			return EColor256;
		case 12:
			return EColor4K;
		case 16:
			return EColor64K;
		case 24:
			return EColor16M;
		case 32:
			if(aColor == SEpocBitmapHeader::EColor)
				return EColor16MU;
			else if(aColor == SEpocBitmapHeader::EColorAlphaPM)
				return EColor16MAP;
			else if(aColor == SEpocBitmapHeader::EColorAlpha)
				return EColor16MA;
			else
				return ENone;
		default:
			return ENone;
			}
		}
	else
		{
		switch (aBpp)
			{
		case 1:
			return EGray2;
		case 2:
			return EGray4;
		case 4:
			return EGray16;
		case 8:
			return EGray256;
		default:
			return ENone;
			}
		}
	}

TBitmapfileCompression CBitwiseBitmap::CompressionType(TInt aBpp, TInt aColor)
	{
	switch (aBpp)
		{
	case 1:
	case 2:
	case 4:
	case 8:
		return EByteRLECompression;
	case 12:
		return ETwelveBitRLECompression;
	case 16:
		return ESixteenBitRLECompression;
	case 24:
		return ETwentyFourBitRLECompression;
	case 32:
		__ASSERT_DEBUG((aColor==SEpocBitmapHeader::EColor) ||
				(aColor==SEpocBitmapHeader::EColorAlpha) ||
				aColor==SEpocBitmapHeader::EColorAlphaPM,
				::Panic(EFbsBitmapInvalidCompression));
		if(aColor == SEpocBitmapHeader::EColor)
			{
			return EThirtyTwoUBitRLECompression;
			}
		else
			{
			return EThirtyTwoABitRLECompression;
			}
	default:
		return ENoBitmapCompression;
		}
	}

/**
@internalComponent
@return The display mode used to create the bitmap.
*/
TDisplayMode CBitwiseBitmap::InitialDisplayMode() const
	{
	return iSettings.InitialDisplayMode();
	}

/**
The method changes current display mode of the bitmap.
Requested display mode can't be greater (bpp value) than the initial display mode.
CBitwiseBitmap instances are shared between the client and server side and
SetDisplayMode() can be called only from the client side, because its functionality depends
on the RFbsSession instance.
The method can't leave because of out of memory condition or something else - no
additional memory is allocated or "L" methods called.
The bitmap content is preserved when converting it to the requested display mode,
but there may be some loss of a quality.
@internalComponent
@param aDisplayMode Requested display mode.
@param aDataAddress Bitmap data address.
@return KErrArgument If the requested mode is invalid, or greater (bpp value) than the
initial display mode.
@return KErrNotSupported  If the bitmap is compressed, or it is a ROM bitmap,
an extended bitmap or a hardware bitmap.
@return KErrNone If the method call is successfull.
*/
TInt CBitwiseBitmap::SetDisplayMode(TDisplayMode aDisplayMode, TUint32* aDataAddress)
	{
	TDisplayMode curDisplayMode = iSettings.CurrentDisplayMode();
	//If requested mode is the same as current mode - do nothing.
	if(curDisplayMode == aDisplayMode)
		{
		return KErrNone;
		}
	//Argument and bitmap state check.
	TInt err = DisplayModeArgCheck(aDisplayMode, aDataAddress);
	if(err != KErrNone)
		{
		return err;
		}
	//data pointers and scan line width calculation.
	TInt scanLineWidthNew = CBitwiseBitmap::ByteWidth(iHeader.iSizeInPixels.iWidth, aDisplayMode);
	TInt scanLineWidthInitial = CBitwiseBitmap::ByteWidth(iHeader.iSizeInPixels.iWidth, iSettings.InitialDisplayMode());
	TInt bmpSizeInitial = scanLineWidthInitial * iHeader.iSizeInPixels.iHeight;
	TUint8* baseAddr = reinterpret_cast <TUint8*> (aDataAddress);
	TUint8* dataAddrNew = baseAddr;
	TInt yStart = 0;
	TInt yInc = 1;
	TInt yEnd = iHeader.iSizeInPixels.iHeight;
	//If requested display mode has more bits per pixel than current display mode - we have
	//to start copying operation from the end of the bitmap, otherwise we will overwrite the
	//bitmap data.
	if(aDisplayMode > curDisplayMode)
		{
		dataAddrNew += bmpSizeInitial - scanLineWidthNew;
		scanLineWidthNew = -scanLineWidthNew;
		yStart = yEnd - 1;
		yInc = -1;
		yEnd = -1;
		}
	//Change the display mode
	ChangeDisplayMode(aDisplayMode, scanLineWidthNew, dataAddrNew, aDataAddress, yStart, yInc, yEnd);
	//Move the data to the aDataAddress.
	if(aDisplayMode > curDisplayMode)
		{
		TInt bmpSizeNew = -scanLineWidthNew * iHeader.iSizeInPixels.iHeight;
		Mem::Move(baseAddr, baseAddr + bmpSizeInitial - bmpSizeNew, bmpSizeNew);
		}
	//Update the bitmap properties
	UpdateBitmapProperties(aDisplayMode);
	return KErrNone;
	}

/**
The method is caled from CBitwiseBitmap::SetDisplayMode() and
checks the aDisplayMode argument and bitmap internal state.
Requested display mode can't be greater (bpp value) than the initial display mode.
Note: The method must be called only from CBitwiseBitmap::SetDisplayMode method.
@internalComponent
@param aDisplayMode Requested display mode.
@param aDataAddress Bitmap data address.
@return KErrArgument If the requested mode is invalid, or greater (bpp value)
than the initial mode.
@return KErrNotSupported If the bitmap is compressed, or it is a ROM bitmap,
an extended bitmap or a hardware bitmap.
@return KErrNone If the method call is successfull.
@see CBitwiseBitmap::SetDisplayMode.
*/
TInt CBitwiseBitmap::DisplayModeArgCheck(TDisplayMode aDisplayMode, TUint32* aDataAddress) const
	{
	if(!aDataAddress || iHeader.iSizeInPixels.iWidth == 0 || iHeader.iSizeInPixels.iHeight == 0)
		{
		return KErrGeneral;
		}
	TBool romAddr = EFalse;
	User::IsRomAddress(romAddr, aDataAddress);
	if(romAddr ||									//ROM bitmap
	   (iUid.iUid != KCBitwiseBitmapUid.iUid) ||	//RHardwareBitmap or extended bitmap
	   IsCompressed()								//Compressed
	  )
		{
		return KErrNotSupported;
		}
	if(aDisplayMode == ENone || aDisplayMode == ERgb)
		{
		return KErrArgument;
		}
	if (iSettings.InitialDisplayMode()==EColor16 && aDisplayMode==EGray256)
		{
		return KErrArgument;
		}
	// The order of the display mode in the TDisplayMode
	// ENone,EGray2,EGray4,EGray16,EGray256,EColor16,EColor256,EColor64K,EColor16M,ERgb,EColor4K,EColor16MU

	//special case where initial mode is EColor4K & to be set to EColor64K & EColor16M which has lower enum
	if (iSettings.InitialDisplayMode()==EColor4K )
		{
		if (aDisplayMode==EColor64K || aDisplayMode==EColor16M)
			return KErrArgument;
		}

	if(aDisplayMode == EColor16MAP)
		{
		TDisplayMode mode = iSettings.InitialDisplayMode();
		if((mode == EColor16MA)||(mode == EColor16MU)||(mode == EColor16MAP))
			{
			return KErrNone;
			}
		else{
			return KErrArgument;
			}
		}
	if(iSettings.InitialDisplayMode() == EColor16MAP)
		{
		return KErrNone;
		}
	if(aDisplayMode > iSettings.InitialDisplayMode())
		{
		if (iSettings.InitialDisplayMode()>=EColor64K && aDisplayMode == EColor4K)
			{
			return KErrNone;
			}
	        if (iSettings.InitialDisplayMode()==EColor16MU &&
	        	(Bpp(aDisplayMode) == 32))
			{
			return KErrNone;
			}
		return KErrArgument;
		}

	return KErrNone;
	}

/**
The method changes current display mode of the bitmap converting bitmap scan lines
color and writting the resulting scan line to the same memory occupied by the bitmap.
No additional memory is allocated.
Note: The method must be called only from CBitwiseBitmap::SetDisplayMode method.
@internalComponent
@param aNewDisplayMode Requested display mode.
@param aScanLineWidthNew Scan line width - with the new display mode. It could be negative
if the new display mode is with less bits per pixel than the existing display mode.
@param aDataAddrNew New bitmap data - Points to the place where the copying has to start to.
@param aDataAddress Bitmap data address.
@param aYStart First scan line number.
@param aYInc Scan line increment value.
@param aYEnd Last scan line number.
@see CBitwiseBitmap::SetDisplayMode.
*/
void CBitwiseBitmap::ChangeDisplayMode( TDisplayMode aNewDisplayMode,
										TInt aScanLineWidthNew,
										TUint8* aDataAddrNew,
										TUint32* aDataAddress,
										TInt aYStart,
										TInt aYInc,
										TInt aYEnd)
	{
	const TInt KScanLineBufSizeInPixels = 256;//temporary scan line buffer size - in pixels
	const TInt KRgbBytes = 4;
	const TInt KScanLineBufSizeInBytes = KScanLineBufSizeInPixels * KRgbBytes;
	TUint8 scanLineData[KScanLineBufSizeInBytes];
	static const TInt KScanLinePixels[] =  //The rounded number of pixels - the closest number that
		{							//could fill scanLineData buffer - depending on current display mode
		0,												//ENone - INVALID mode
		KScanLineBufSizeInBytes * 8,					//EGray2
		KScanLineBufSizeInBytes * 4,					//EGray4
		KScanLineBufSizeInBytes * 2,					//EGray16
		KScanLineBufSizeInBytes * 1,					//EGray256
		KScanLineBufSizeInBytes * 2,					//EColor16
		KScanLineBufSizeInBytes * 1,					//EColor256
		KScanLineBufSizeInPixels * KRgbBytes / 2,		//EColor64K
		KScanLineBufSizeInPixels,						//EColor16M
		0,												//ERgb - INVALID mode
		KScanLineBufSizeInPixels * KRgbBytes / 2,		//EColor4K - the same as EColor64K
		KScanLineBufSizeInPixels * KRgbBytes / 4,		//EColor16MU
		KScanLineBufSizeInPixels * KRgbBytes / 4,		//EColor16MA
		KScanLineBufSizeInPixels * KRgbBytes / 4		//EColor16MAP
		};
	__ASSERT_DEBUG(aNewDisplayMode < TInt(sizeof(KScanLinePixels) / sizeof(KScanLinePixels[0])), ::Panic(EFbsBitmapInvalidMode3));
	//
	TPtr8 ptr(scanLineData, sizeof(scanLineData), sizeof(scanLineData));
	TDes8& scanLineDes = ptr;
	TLineScanningPosition lineScanningPosition(aDataAddress);
	TPoint startPixel(0, 0);
	TPoint ditherOffset(0, 0);
	//For each line:
	//1. Get a scan line in requested display mode
	//2. Copy the scan line to the destination buffer, pointed by dataAddrNew
	TUint8* dataAddrNew = aDataAddrNew;
	for(TInt i=aYStart;i!=aYEnd;i+=aYInc)
		{
		startPixel.iX = 0;
		startPixel.iY = i;
		TUint8* scanLineDataAddr = dataAddrNew;
		TInt scanLinePixelsLeft = iHeader.iSizeInPixels.iWidth;
		while(scanLinePixelsLeft > 0)
			{
			TInt pixelsCnt = KScanLinePixels[aNewDisplayMode];	//how many pixels we can get at a time - the maximum
			TInt bytesCnt = KScanLineBufSizeInBytes;			//how many bytes the pixels are - the maximum
			if(pixelsCnt > scanLinePixelsLeft)	//in that case the "while" loop will be executed for the last time
				{
				pixelsCnt = scanLinePixelsLeft;
				bytesCnt = CBitwiseBitmap::ByteWidth(pixelsCnt, aNewDisplayMode);
				}
			__ASSERT_DEBUG(pixelsCnt > 0, ::Panic(EFbsBitmapInvalidMode2));//I want to be sure - if someone adds an additional display mode - ChangeDisplayMode() source has been updated too!
			GetScanLine(scanLineDes, startPixel, pixelsCnt, EFalse,	  //Get the scan line data
						ditherOffset, aNewDisplayMode, aDataAddress, lineScanningPosition);
			Mem::Copy(scanLineDataAddr, scanLineData, bytesCnt);//copy the data to its new address
			scanLineDataAddr += bytesCnt;//increment the address
			scanLinePixelsLeft -= pixelsCnt;//decrement the count of pixels left
			startPixel.iX += pixelsCnt;//increment the X coordinate
			}//end of - while(scanLineLengthLeft > 0)
		dataAddrNew += aScanLineWidthNew;
		}//end of - for(TInt i=aYStart;i!=aYEnd;i+=aYInc)
	}

/**
The method updates CBitwiseBitmap data members regarding to the new display mode.
Note: The method must be called only from CBitwiseBitmap::SetDisplayMode method.
@internalComponent
@param aNewDisplayMode The new display mode.
@see CBitwiseBitmap::SetDisplayMode.
*/
void CBitwiseBitmap::UpdateBitmapProperties(TDisplayMode aNewDisplayMode)
	{
	iSettings.SetCurrentDisplayMode(aNewDisplayMode);
	iByteWidth = CBitwiseBitmap::ByteWidth(iHeader.iSizeInPixels.iWidth, aNewDisplayMode);
	iHeader.iBitsPerPixel = CBitwiseBitmap::Bpp(aNewDisplayMode);
	iHeader.iColor = CBitwiseBitmap::IsColor(aNewDisplayMode);
	}

/**
The method swaps the bitmap width and height.
For example: if the bitmap size is (40, 20), the new bitmap size will be (20, 40).
Bitmap content is not preserved.
@internalComponent
@param aDataAddress Bitmap data address.
@return KErrNone The call was successfull.
@return KErrAccessDenied ROM bitmap size can't be swapped.
@return KErrNotSupported Hardware or extended bitmap size can't be swapped.
*/
TInt CBitwiseBitmap::SwapWidthAndHeight(TUint32* aDataAddress)
	{
	if (iUid.iUid != KCBitwiseBitmapUid.iUid)	// RHardwareBitmap or extended bitmap
		{
		return KErrNotSupported;
		}

	TBool romAddr = EFalse;
	User::IsRomAddress(romAddr, aDataAddress);
	if (romAddr)								//ROM bitmap
		{
		return KErrAccessDenied;
		}

    //Check the new bitmap size - it should not exeed the size of the allocated memory
    TInt newWidthInBytes = CBitwiseBitmap::ByteWidth(iHeader.iSizeInPixels.iHeight, iSettings.CurrentDisplayMode());
	TInt64 hugeDataSize = TInt64(iHeader.iSizeInPixels.iWidth) * TInt64(newWidthInBytes);
    __ASSERT_ALWAYS(I64HIGH(hugeDataSize) == 0 &&
                    I64LOW(hugeDataSize) <= TUint(iHeader.iBitmapSize - iHeader.iStructSize),
                    ::Panic(EFbsBitmapSwappingImpossible));

	//Initialize the data members with the new values
	iByteWidth = CBitwiseBitmap::ByteWidth(iHeader.iSizeInPixels.iHeight, iSettings.CurrentDisplayMode());
	TInt temp = iHeader.iSizeInPixels.iWidth;
	iHeader.iSizeInPixels.iWidth = iHeader.iSizeInPixels.iHeight;
	iHeader.iSizeInPixels.iHeight = temp;
	temp = iHeader.iSizeInTwips.iWidth;
	iHeader.iSizeInTwips.iWidth = iHeader.iSizeInTwips.iHeight;
	iHeader.iSizeInTwips.iHeight = temp;
	return KErrNone;
	}

/**
Compile time check is performed on the class size - the class size must be
the same as the size of TDisplayMode type. If the class size is not the same
as  TDisplayMode type size - BC will be broken.
Note: CBitwiseBitmap::iSettings data member must be aligned on 16 bits boundary
because CBitwiseBitmap instances can be a part of the ROM image.
@internalComponent
@param aDisplayMode The display mode.
*/
CBitwiseBitmap::TSettings::TSettings(TDisplayMode aDisplayMode):
	iData(0)
	{
	//"CBitwiseBitmap::iSettings" data mamber - bit format:
	// MSB                                              LSB
	// 16         			:           8             :            8
	// Flags                  Initial display mode      Current display mode
	//
	//CBitwiseBitmap - TSettings member has been put in place of previous iDispMode
	//class member. So, TSettings data member has to occupy the same space as
	//not existing anymore iDispMode member.
	COMPILE_TIME_ASSERT(sizeof(TSettings) == sizeof(TDisplayMode));
	//We can't have TDisplayMode enum value greater than 255 because we encode it
	//int 8 bits of iData data member.
	COMPILE_TIME_ASSERT(EColorLast < 256);
	SetDisplayModes(aDisplayMode);
	}

/**
The method initializes both - current display mode and initial display mode parts of iData
with aDisplayMode parameter.
@internalComponent
@param aDisplayMode The display mode used for current display mode and initial display mode
parts of iData initialization
*/
void CBitwiseBitmap::TSettings::SetDisplayModes(TDisplayMode aDisplayMode)
	{
	iData &= 0xFFFF0000;
	iData |= TUint16(aDisplayMode << 8);
	iData |= TUint8(aDisplayMode);
	}

/**
The method initializes current display mode part of iData with aDisplayMode parameter.
@internalComponent
@param aDisplayMode The display mode used for current display mode part of iData initialization.
*/
void CBitwiseBitmap::TSettings::SetCurrentDisplayMode(TDisplayMode aDisplayMode)
	{
	iData &= 0xFFFFFF00;
	iData |= TUint8(aDisplayMode);
	}

/**
The method returns current display mode.
@internalComponent
Note: Current display mode can never be greater (bpp value) than initial display mode.
@return Current display mode.
*/
TDisplayMode CBitwiseBitmap::TSettings::CurrentDisplayMode() const
	{
	return TDisplayMode(iData & 0x000000FF);
	}

/**
The method returns initial display mode.
@internalComponent
@return The initial display mode.
*/
TDisplayMode CBitwiseBitmap::TSettings::InitialDisplayMode() const
	{
	return TDisplayMode((iData & 0x0000FF00) >> 8);
	}

/**
The method adjusts specified X coordinate if it is negative or outside the bitmap.
@internalComponent
@param  aX - a reference to x coordinate - the value might be changed after the method call.
*/
void CBitwiseBitmap::AdjustXCoord(TInt& aX) const
	{
	if (aX>=iHeader.iSizeInPixels.iWidth || aX<-iHeader.iSizeInPixels.iWidth)
		aX%=iHeader.iSizeInPixels.iWidth;
	if (aX<0)
		aX+=iHeader.iSizeInPixels.iWidth;
	}

/**
If the bitmap is compressed in RAM, the method will find its compresssion bookmark,
which is located at the end of the bitmap data and will reinitialize aLineScanPos
parameter.
@internalComponent
@param aLineScanPos Line scaning position. It is used by scan line decompression methods.
@param aComprBookMark If the bitmap is compressed in RAM, aComprBookMark will be initialized
to point to its compression bookmark data. The compression bookmark data will be used for
aLineScanPos initialization.
@param  aBase It points to the beginning of the bitmap data.
*/
void CBitwiseBitmap::GetLineScanPos(TLineScanningPosition& aLineScanPos,
									const TCompressionBookMark*& aComprBookMark,
									const TUint8* aBase) const
	{
	if (iIsCompressedInRAM && (iHeader.iCompression != EGenericPaletteCompression) && (iHeap != NULL))
		{
		TInt compressed_data_bytes = iHeader.iBitmapSize - sizeof(SEpocBitmapHeader);
		if(compressed_data_bytes > KCompressionBookMarkThreshold)
			{
			if(aBase)
				{
				TInt alignedSize = (compressed_data_bytes + 3) / 4 * 4;
				const TUint8* data = aBase + alignedSize + 4;
				aComprBookMark = reinterpret_cast <const TCompressionBookMark*> (data);
				if (aComprBookMark->IsCheckSumOk())
					{
					aLineScanPos.iSrcDataPtr = const_cast <TUint8*> (aBase) + aComprBookMark->iSrcDataOffset;
					aLineScanPos.iCursorPos = aComprBookMark->iCursorPos;
					}
				}
			}
		}
	}

/**
If the bitmap is compressed in RAM, the method will update its compresssion bookmark data,
which is located at the end of the bitmap data.
@internalComponent
@param aLineScanPos Line scaning position.
@param aComprBookMark If the bitmap is compressed in RAM, aComprBookMark points to its
compression bookmark data.
@param  aBase It points to the beginning of the bitmap data.
*/
void CBitwiseBitmap::UpdateBookMark(const TLineScanningPosition& aLineScanPos,
									TCompressionBookMark* aComprBookMark,
									const TUint8* aBase) const
	{
	if (aComprBookMark)
		{
		if (aComprBookMark->IsCheckSumOk())
			{
			aComprBookMark->iSrcDataOffset=aLineScanPos.iSrcDataPtr-aBase;
			aComprBookMark->iCursorPos=aLineScanPos.iCursorPos;
			aComprBookMark->CalculateCheckSum();
			}
		}
	}

/**
The header is exposed by CFbsBitmap so this doesn't break encapsulation.
Specifically added to allow CBitmapObject to see compression information.
@return Address of iHeader.
*/
EXPORT_C SEpocBitmapHeader CBitwiseBitmap::Header() const
	{
	return iHeader ;
	}
