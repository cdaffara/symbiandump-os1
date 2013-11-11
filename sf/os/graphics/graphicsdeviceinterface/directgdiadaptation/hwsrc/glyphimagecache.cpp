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

#include "glyphimagecache.h"
#include "glyphlutab.h"
#include "vgengine.h"
#include <fbs.h>

//the image of the following size will be pre-allocated for low memory conditions.
const TSize KMaxSizeImageOOM(72, 64);
#ifdef DRAWGLYPH_MULTIPLY_MODE
// Matrix used for converting glyphs with a background colour of black and a character colour
// of white to have a background colour that is fully transparent black and a character colour
// of opaque black. 
const VGfloat KColorMatrix[20] = { 0, 0, 0, 1, // sets alpha of destination to R value of source
				                   0, 0, 0, 0,
				                   0, 0, 0, 0,
				                   0, 0, 0, 0,
				                   1, 1, 1, 0}; // sets RGB of destination to 1
#endif // DRAWGLYPH_MULTIPLY_MODE

//--------------class CFontGlyphTree --------------------
/**
Creates a new instance of the class. Will not be shared across different threads 

@param 	aFontId The unique font identifier.
@param 	aGlyphType The type for the format of a glyph bitmap.
@return The pointer to the CFontGlyphTree class instance.
 */
CFontGlyphTree* CFontGlyphTree::NewL(TUint32 aFontId, TGlyphBitmapType aGlyphType)
	{
	CFontGlyphTree*  self = new (ELeave) CFontGlyphTree(aFontId, aGlyphType);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Constructor for the image font glyph tree

@param 	aFontId The unique font identifier.
@param 	aGlyphType The type for the format of a glyph bitmap.
*/
CFontGlyphTree::CFontGlyphTree(TUint32 aFontId, TGlyphBitmapType aGlyphType) :
	iKey(_FOFF(TGlyphEntry,iGlyphCode),ECmpTUint32),
	iFontId(aFontId),
	iGlyphType(aGlyphType)
	{
	}
	
/**
Destructor for the image font glyph tree.

Destroys the VGImages, page pool and binary tree. 
*/
CFontGlyphTree::~CFontGlyphTree()
	{
	TRAP_IGNORE(DestroyAllVGImagesL());
	delete iGlyphTree;
	delete iPagePool;
	}
	
/**
Constructs memory page pool and binary tree. Glyph code will be used as a key.
*/
void CFontGlyphTree::ConstructL()
	{
	iPagePool = CMemPagePool::NewL();
	
	switch(iGlyphType)
		{
	case EFourColourBlendGlyphBitmap:
		iGlyphTree = new (ELeave) TBtreeFix<TGlyphEntryCompound, TChar> (EBtreeFast);
		((TBtreeFix<TGlyphEntryCompound, TChar> *)iGlyphTree) -> Connect(iPagePool, &iKey);
		break;
	case EMonochromeGlyphBitmap:
	case EAntiAliasedGlyphBitmap:
		iGlyphTree = new (ELeave) TBtreeFix<TGlyphEntry, TChar> (EBtreeFast);
		((TBtreeFix<TGlyphEntry, TChar> *)iGlyphTree) -> Connect(iPagePool, &iKey);
		break;
	default:
		User::Leave(KErrNotSupported);
		break;	
		}
	}
	
/**
Searches for the image entry in the binary tree. If fails, it will create a new entry.

@param	aGlyphCode General Unicode character value.
@param	aGlyphImage Glyph image	bitmap data. The data structure depends on glyph type.
@param	aGlyphImageSize Size of the glyph image.		
@param	aEntry Binary tree entry, which comprises OpenVG image(s).
@param	aDataForeground Pre-allocated buffer, which will be used for setting text VG image.

@see TGlyphEntryCompound
@see TGlyphEntry

@pre 	Rendering engine has been constructed.
@post 	Requested  OpenVG images are ready for rendering.
@return KErrNone if successful;  
		KErrNotSupported if font type is not supported,  
		otherwise one of the other system-wide error codes.
*/
template <class K>
void CFontGlyphTree::GlyphImageEntryL(TChar aGlyphCode, const TUint8* aGlyphImage, const TSize& aGlyphImageSize, K& aEntry, TUint8* aDataForeground)
	{
	TBtreePos pos;
	TBool found = EFalse;
	found = ((TBtreeFix<K, TChar> *)iGlyphTree) -> FindL(pos, aGlyphCode);
	
	if(found)
		{
		((TBtreeFix<K, TChar> *)iGlyphTree) -> ExtractAtL(pos, aEntry);
		}
	else
		{
		aEntry.iGlyphCode = aGlyphCode;
		aEntry.iForeground = VG_INVALID_HANDLE;
		TInt glyphSizeInByte = 0;
		
		switch(iGlyphType)
			{
		case EFourColourBlendGlyphBitmap:
			{
			((TGlyphEntryCompound&) aEntry).iOutline = VG_INVALID_HANDLE;
			((TGlyphEntryCompound&) aEntry).iShadow = VG_INVALID_HANDLE;
			CreateVGImageL(aGlyphImage, aGlyphImageSize, aEntry.iForeground, ((TGlyphEntryCompound&) aEntry).iOutline, ((TGlyphEntryCompound&) aEntry).iShadow, NULL, NULL, NULL);
			glyphSizeInByte = aGlyphImageSize.iWidth * aGlyphImageSize.iHeight;
			glyphSizeInByte *= 3; //foreground, shadow, outline
			break;
			}
		case EMonochromeGlyphBitmap:
			CreateVGImageL(aGlyphImage, aGlyphImageSize, EGray2, aEntry.iForeground, aDataForeground);
			glyphSizeInByte = (((aGlyphImageSize.iWidth + 31) / 32) << 2) * aGlyphImageSize.iHeight;
			break;
		case EAntiAliasedGlyphBitmap:
			CreateVGImageL(aGlyphImage, aGlyphImageSize, EGray256, aEntry.iForeground, NULL);
			glyphSizeInByte = aGlyphImageSize.iWidth * aGlyphImageSize.iHeight;
			break;
		default:
			User::Leave(KErrNotSupported);
			}

		((TBtreeFix<K, TChar> *)iGlyphTree) -> InsertL(pos, aEntry);
		iCacheSize += glyphSizeInByte;	
		}
	}

/**
Overridden function, which creates Open VG images for foreground, background, shadow and outline components of the font.
 
@param 	aGlyphImage Source bitmap data in 256 grey format. Each pixel value is an index to a constant lookup table. 
			Four entries of this table represent % of Outline, Shadow, Fill and Background colour to be used to get the final colour to be displayed on screen.
@param	aGlyphImageSize Size of the glyph bitmap image.
@param	aForeground Foreground component of the glyph.
@param	aOutline	Outline component of the glyph.
@param	aShadow 	Shadow component of the glyph. 
@param	aPreAllocForeground Pre-allocated buffer which will be used for setting text foreground VG image
@param	aPreAllocOutline Pre-allocated buffer which will be used for setting text outline VG image
@param	aPreAllocShadow Pre-allocated buffer which will be used for setting text shadow VG image

@post 	Requested OpenVG images are ready for rendering. 
*/
void CFontGlyphTree::CreateVGImageL(const TUint8* aGlyphImage, const TSize& aGlyphImageSize, VGImage& aForeground, VGImage& aOutline, VGImage& aShadow, TUint8* aPreAllocForeground, TUint8* aPreAllocOutline, TUint8* aPreAllocShadow)
	{
	TInt dataStride = aGlyphImageSize.iWidth;
	TInt targetByteCount = dataStride * aGlyphImageSize.iHeight;
		// Allocate memory and transform source into target format.
		//
	TAny* foregroundBuffer = NULL;
	TAny* outlineBuffer = NULL;
	TAny* shadowBuffer = NULL;
	TBool destroyTempBuffer = EFalse;
	
	if(aPreAllocForeground && aPreAllocOutline && aPreAllocShadow && 
			(aGlyphImageSize.iWidth <= KMaxSizeImageOOM.iWidth) && 
			(aGlyphImageSize.iHeight <= KMaxSizeImageOOM.iHeight))
		{
		foregroundBuffer = aPreAllocForeground;
		outlineBuffer = aPreAllocOutline;
		shadowBuffer = aPreAllocShadow;
		}
	else
		{
		foregroundBuffer = User::AllocL(targetByteCount);
		CleanupStack::PushL(foregroundBuffer);
		outlineBuffer = User::AllocL(targetByteCount);
		CleanupStack::PushL(outlineBuffer);
		shadowBuffer = User::AllocL(targetByteCount);
		CleanupStack::PushL(shadowBuffer);
		destroyTempBuffer = ETrue;
		}	

	TUint8* foregroundByte = static_cast <TUint8*> (foregroundBuffer); 
	TUint8* outlineByte = static_cast <TUint8*> (outlineBuffer);
	TUint8* shadowByte = static_cast <TUint8*> (shadowBuffer);
				
	const TUint8* endByte = (TUint8*)aGlyphImage + targetByteCount;
	TUint8* curSrcGlyphImage = const_cast <TUint8*> (aGlyphImage);
	
	while (curSrcGlyphImage < endByte)
		{
		*outlineByte++ = FourColorBlendLookup[*curSrcGlyphImage] [KOutlineColorIndex];
		*shadowByte++ = FourColorBlendLookup[*curSrcGlyphImage] [KShadowColorIndex];
		*foregroundByte++ = FourColorBlendLookup[*curSrcGlyphImage] [KFillColorIndex];
		curSrcGlyphImage++;
		}

	const VGImageFormat imageFormat = VG_sL_8;
	if(aForeground == VG_INVALID_HANDLE)
		{
		aForeground = vgCreateImage(imageFormat,
	                              aGlyphImageSize.iWidth, 
	                              aGlyphImageSize.iHeight,
	                              VG_IMAGE_QUALITY_NONANTIALIASED);
		if(aForeground == VG_INVALID_HANDLE)
			{
			User::Leave(KErrNoMemory);
			}
		aOutline = vgCreateImage(imageFormat,
	                              aGlyphImageSize.iWidth, 
	                              aGlyphImageSize.iHeight,
	                              VG_IMAGE_QUALITY_NONANTIALIASED);
		if(aOutline == VG_INVALID_HANDLE)
			{
			DestroyVGImage(&aForeground);
			User::Leave(KErrNoMemory);
			}

		aShadow = vgCreateImage(imageFormat,
	                              aGlyphImageSize.iWidth, 
	                              aGlyphImageSize.iHeight,
	                              VG_IMAGE_QUALITY_NONANTIALIASED);
		if(aShadow == VG_INVALID_HANDLE)
			{
			DestroyVGImage(&aForeground, &aOutline);
			User::Leave(KErrNoMemory);	
			}
		}
	
	vgImageSubData(
		aForeground, foregroundBuffer, 
		dataStride,	imageFormat, 
		0, 0,aGlyphImageSize.iWidth, aGlyphImageSize.iHeight);
	
#ifdef DRAWGLYPH_MULTIPLY_MODE
		VGImage image = vgCreateImage(VG_sARGB_8888_PRE, aGlyphImageSize.iWidth, aGlyphImageSize.iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);		
		vgColorMatrix(image, aForeground, KColorMatrix);
		vgDestroyImage(aForeground);
		aForeground = image;
#endif // DRAWGLYPH_MULTIPLY_MODE
		
	vgImageSubData(
		aOutline, outlineBuffer, 
		dataStride,	imageFormat, 
		0, 0, aGlyphImageSize.iWidth, aGlyphImageSize.iHeight);
	
#ifdef DRAWGLYPH_MULTIPLY_MODE
		image = vgCreateImage(VG_sARGB_8888_PRE, aGlyphImageSize.iWidth, aGlyphImageSize.iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);		
		vgColorMatrix(image, aOutline, KColorMatrix);
		vgDestroyImage(aOutline);
		aOutline = image;
#endif // DRAWGLYPH_MULTIPLY_MODE
		
	vgImageSubData(
		aShadow, shadowBuffer, 
		dataStride,	imageFormat, 
		0, 0, aGlyphImageSize.iWidth, aGlyphImageSize.iHeight);
	
#ifdef DRAWGLYPH_MULTIPLY_MODE
		image = vgCreateImage(VG_sARGB_8888_PRE, aGlyphImageSize.iWidth, aGlyphImageSize.iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);		
		vgColorMatrix(image, aShadow, KColorMatrix);
		vgDestroyImage(aShadow);
		aShadow = image;
#endif // DRAWGLYPH_MULTIPLY_MODE	
	
	if(destroyTempBuffer)
		{
		CleanupStack::PopAndDestroy(3, foregroundBuffer);
		}
	}

/**
Overridden function, which creates OpenVG images for monochrome and anti-aliased fonts.
 
@param 	aGlyphImage Data source bitmap in 256 or 2 Grey format. 
@param 	aGlyphImageSize Glyph image data size.
@param 	aDisplayMode Image display mode.
@param	aForeground Foreground component of the glyph.
@param	aPreAllocForeground Pre-allocated buffer which will be used for setting foreground VG image

@post 	Requested OpenVG image is ready for rendering. 

@panic 	Panic if bitmap display mode is not 256 grey or 2 grey.
*/
void CFontGlyphTree::CreateVGImageL(const TUint8* aGlyphImage, const TSize& aGlyphImageSize, TDisplayMode aDisplayMode, VGImage& aForeground, TUint8* aPreAllocForeground)
	{
	GRAPHICS_ASSERT_DEBUG((aDisplayMode == EGray256) || (aDisplayMode == EGray2), EDirectGdiPanicInvalidDisplayMode);
	GRAPHICS_ASSERT_DEBUG(aGlyphImage, EDirectGdiPanicInvalidParameter);
	
	VGImageFormat imageFormat = VG_IMAGE_FORMAT_INVALID;
	TInt  vgCompatibleSourceStride = 0x00;
	TUint32 binaryDataArray[32];
	TUint8* binaryData = NULL;
	TUint8* tempBuffer = NULL;	
	
	if(aDisplayMode == EGray256)
		{
		imageFormat = VG_sL_8;
		vgCompatibleSourceStride = aGlyphImageSize.iWidth; 
		binaryData = const_cast <TUint8*> (aGlyphImage);
		}
	else //EGray2
		{
		imageFormat = VG_BW_1;
		vgCompatibleSourceStride = ((aGlyphImageSize.iWidth + 31) / 32) << 2;
		if (aGlyphImageSize.iWidth > 30 || aGlyphImageSize.iHeight > 32)
			{
			binaryData = aPreAllocForeground;
			if(!binaryData)
				{
				tempBuffer = (TUint8*) User::AllocL(vgCompatibleSourceStride * aGlyphImageSize.iHeight);
				CleanupStack::PushL(tempBuffer);
				binaryData = tempBuffer;
				}
			DecodeBinaryDataExLarge(aGlyphImageSize, aGlyphImage, vgCompatibleSourceStride, reinterpret_cast <TUint32*> (binaryData));
			}
		else
			{
			DecodeBinaryData(aGlyphImageSize, aGlyphImage, binaryDataArray);
			binaryData = reinterpret_cast <TUint8*> (binaryDataArray);		
			}
		}

	if(aForeground == VG_INVALID_HANDLE)
		{
		aForeground = vgCreateImage(imageFormat,
                              aGlyphImageSize.iWidth, 
                              aGlyphImageSize.iHeight,
                              VG_IMAGE_QUALITY_NONANTIALIASED);
		}
	
	if (aForeground != VG_INVALID_HANDLE)
		{
			// Copy from the source image to our new VGImage
		vgImageSubData(aForeground, binaryData, vgCompatibleSourceStride,	imageFormat, 
			0,	0, aGlyphImageSize.iWidth, aGlyphImageSize.iHeight);
		
#ifdef DRAWGLYPH_MULTIPLY_MODE
		VGImage image = vgCreateImage(VG_sARGB_8888_PRE, aGlyphImageSize.iWidth, aGlyphImageSize.iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);		
		vgColorMatrix(image, aForeground, KColorMatrix);
		vgDestroyImage(aForeground);
		aForeground = image;
#endif
		}
	else
		{
		if(tempBuffer)
			{
			CleanupStack::PopAndDestroy(tempBuffer);	
			}
		User::Leave(KErrNoMemory);
		}
		
	if(tempBuffer)
		{
		CleanupStack::PopAndDestroy(tempBuffer);	
		}
	}

/**
Decodes binary data for monochrome bitmap.

@param aDataSize Image size.
@param aData Pointer to a source buffer.
@param aBinaryData Pointer to a destination buffer.
*/
void CFontGlyphTree::DecodeBinaryData(const TSize& aDataSize,
								const TUint8* aData,
								TUint32* aBinaryData) 
	{
	//Divert if the character is larger than expected; the criterion
	//for choosing this function is only a heuristic, because it's perfectly legal for
	//a character's bitmap to be wider than its escapement.
	//
	//Use a dummy value (0) for semi-ascent because this character is not italic and so semi-ascent
	//is irrelevant; it's used for pseudo-italic slanting.
	
	TInt dataheight = aDataSize.iHeight;
	TInt datalength = aDataSize.iWidth;
	
	TInt bitindex=0;
	TInt16 repeatcount=0;
	TUint32* binarydataptr=aBinaryData;
	TUint32* binarydataptrlimit;
	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		binarydataptrlimit=aBinaryData+charline+repeatcount;
		if(multilineflag)
			{
			while(binarydataptr<binarydataptrlimit)
				{
				TInt chardataoffsetptr=TInt(aData)+(bitindex>>3);
				TUint32* chardataword=(TUint32*)(chardataoffsetptr&~3);
				TInt bitshift=bitindex&7;
				bitshift+=(chardataoffsetptr&3)<<3;
				*binarydataptr=(*chardataword++)>>bitshift;
				if(bitshift) *binarydataptr|=(*chardataword<<(32-bitshift));
				bitindex+=datalength;
				binarydataptr++;
				}
			}
		else
			{
			TInt chardataoffsetptr=TInt(aData)+(bitindex>>3);
			TUint32* chardataword=(TUint32*)(chardataoffsetptr&~3);
			TInt bitshift=bitindex&7;
			bitshift+=(chardataoffsetptr&3)<<3;
			TUint32 data=(*chardataword++)>>bitshift;
			if(bitshift) data|=(*chardataword<<(32-bitshift));
			while(binarydataptr<binarydataptrlimit)
				*binarydataptr++=data;
			bitindex+=datalength;
			}
		}
	}
	
/**
Decodes binary data for extra large monochrome bitmap.

@param aDataSize Image size.
@param aData Pointer to a source buffer.
@param aStride Image data stride.
@param aBinaryData Pointer to a destination buffer.
*/
void CFontGlyphTree::DecodeBinaryDataExLarge(const TSize& aDataSize, const TUint8* aData, TInt aStride,
											TUint32* aBinaryData)
	{
	const TInt datalength = aDataSize.iWidth;
	const TInt dataheight = aDataSize.iHeight;
	TInt bitindex=0;
	TInt16 repeatcount=0;
	TUint32* slbuffer=aBinaryData;
	const TInt slwords=aStride;

	for(TInt charline=0;charline<dataheight;charline+=repeatcount) // for lines in the character...
		{
		repeatcount=Load16(aData+(bitindex>>3));
		repeatcount>>=bitindex&7;
		const TInt multilineflag=repeatcount&1;
		repeatcount>>=1;
		repeatcount&=0xf;
		bitindex+=5;
		if(multilineflag)
			{
			for(TInt currentline=0;currentline<repeatcount;currentline++)
				{
				CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength, 1);
				bitindex+=datalength;
				}
			}
		else
			{
			CopyCharLine(slbuffer,slwords,aData+(bitindex>>3),bitindex&7,datalength, repeatcount);
			bitindex+=datalength;
			}
		}
	}

/**
Copies image data line(s) to a destination.

@param aBinaryDataPtr pointer to a destination buffer.
@param aBufferWords Stride of the image.
@param aData Pointer to a source buffer.
@param aBitShift Number of bits, binary data will be shifted. 
@param aCharWidth Width of the image.
@param aRepeatCount Number of lines to copy.

@panic DGDIAdapter 1018, if a null binary data pointer is passed in.
*/
void CFontGlyphTree::CopyCharLine(TUint32*& aBinaryDataPtr,TInt aBufferWords,const TUint8* aData,TInt aBitShift,TInt aCharWidth, TInt16 aRepeatCount)
	{
	GRAPHICS_ASSERT_DEBUG(aBinaryDataPtr, EDirectGdiPanicInvalidPointer);//this shouldn't happen, as we always allocate memory prior to call this function 
	aBitShift&=7;
	TInt wordstocopy=(aCharWidth+31)>>5;
	if(wordstocopy>aBufferWords) wordstocopy=aBufferWords;
	TUint32* ptrlimit=aBinaryDataPtr+wordstocopy;
	TUint32* dataword=(TUint32*)(TInt(aData)&~3);
	aBitShift+=(TInt(aData)-TInt(dataword))<<3;
	
	TUint32* startBinaryDataPtr = aBinaryDataPtr;
	while(aBinaryDataPtr<ptrlimit)
		{
		*aBinaryDataPtr=*dataword++;
		*aBinaryDataPtr>>=aBitShift;
		if(aBitShift) *aBinaryDataPtr|=(*dataword<<(32-aBitShift));
		aBinaryDataPtr++;
		}
	
	TUint32* curStartBinaryDataPtr = aBinaryDataPtr;
	TInt byteToCopy = wordstocopy << 2;
	while(aRepeatCount > 1)
		{
		Mem::Copy(curStartBinaryDataPtr, startBinaryDataPtr, byteToCopy);
		curStartBinaryDataPtr += wordstocopy;
		
		aRepeatCount--;
		}
	aBinaryDataPtr = curStartBinaryDataPtr;
	}

/**
Destroys OpenVG images and set variables to NULL

@param aForeground Pointer to the foreground VGImage.
@param aOutline Pointer to the outline VGImage.
@param aShadow Pointer to the shadow VGImage.
*/
void CFontGlyphTree::DestroyVGImage(VGImage* aForeground, VGImage* aOutline, VGImage* aShadow)
	{
	if(aForeground && *aForeground)
		{
		vgDestroyImage(*aForeground);
		*aForeground = VG_INVALID_HANDLE;
		}
	if(aOutline && *aOutline)
		{
		vgDestroyImage(*aOutline);
		*aOutline = VG_INVALID_HANDLE;
		}
	if(aShadow && *aShadow)
		{
		vgDestroyImage(*aShadow);
		*aShadow = VG_INVALID_HANDLE;
		}
	}

/**
Goes through all entries and deletes VG images.
*/
void CFontGlyphTree::DestroyAllVGImagesL()
	{
	if(iGlyphTree)
		{
		TBool isEntry = EFalse;
		TBtreePos pos;
		isEntry = iGlyphTree -> FirstL(pos);
		//go through all entries and delete VG images
		while (isEntry)
			{
			TGlyphEntry entry;
			TGlyphEntryCompound entryC;
			(iGlyphType == EFourColourBlendGlyphBitmap) ?
					((TBtreeFix<TGlyphEntryCompound, TChar> *) iGlyphTree) -> ExtractAtL(pos, entryC) :
					((TBtreeFix<TGlyphEntry, TChar> *) iGlyphTree) -> ExtractAtL(pos, entry);
			
			if(iGlyphType == EFourColourBlendGlyphBitmap)
				{
				DestroyVGImage(&entryC.iForeground, &entryC.iOutline, &entryC.iShadow);
				}
			else
				{
				DestroyVGImage(&entry.iForeground);
				}	
			isEntry = iGlyphTree -> NextL(pos);
			}
		iCacheSize = 0;
		iGlyphTree -> ClearL();
		}
	}
	
/**
Allows OpenVG images to be created in OOM conditions. Images will not be added to the binary tree. 
*/
template <class K>
void CFontGlyphTree::GlyphImageEntryOOML(TGlyphBitmapType aGlyphType, const TUint8* aGlyphImage, const TSize& aGlyphImageSize, K& aEntry, TUint8* aData, TUint8* aDataOutline, TUint8* aDataShadow)
	{
	switch(aGlyphType)
		{
	case EFourColourBlendGlyphBitmap:
		{
		CreateVGImageL(aGlyphImage, aGlyphImageSize, aEntry.iForeground, ((TGlyphEntryCompound&) aEntry).iOutline, ((TGlyphEntryCompound&) aEntry).iShadow, aData, aDataOutline, aDataShadow);
		break;
		}
	case EMonochromeGlyphBitmap:
		CreateVGImageL(aGlyphImage, aGlyphImageSize, EGray2, aEntry.iForeground, aData);
		break;
	case EAntiAliasedGlyphBitmap:
		CreateVGImageL(aGlyphImage, aGlyphImageSize, EGray256, aEntry.iForeground, aData);
		break;
	default:
		User::Leave(KErrNotSupported);
		}
	}
	
//--------------class CFontGlyphImageStorage --------------------
/**
Constructor for the font glyph image storage.

@param aMaxCacheSize The maximum cache size in bytes. If storage exceeds this value, the least usable binary tree will be destroyed.
*/
CFontGlyphImageStorage::CFontGlyphImageStorage(TInt aMaxCacheSize) :
						iFontTreeList(128),
						iMaxCacheSize(aMaxCacheSize)
	{
	}

/**
Destructor for the font glyph image storage.

Removes and destroys all binary tree's entries in the list.
*/
CFontGlyphImageStorage::~CFontGlyphImageStorage()
	{
	CleanGlyphImageCache();
	DeletePreAllocatedImages();
	}

/**
The function will free memory by deleting the least usable font tree, if the size of the cache exceeds 
some pre-defined value. 

@see MFontGlyphImageStorage
@panic DGDIAdapter 46, if there is an inconsistency in the internal glyph cache structure.
*/
void CFontGlyphImageStorage::CleanCacheIfRequired()
	{
	if(iCacheSize > iMaxCacheSize)
		{
		if(iFontTreeList.Count() == 1)//if it is only one tree, just delete all images
			{
			TInt aOldTreeSize = iFontTreeList[0]->CacheSize();
			TRAP_IGNORE(iFontTreeList[0]->DestroyAllVGImagesL());
			GRAPHICS_ASSERT_DEBUG(0 == (iCacheSize - (aOldTreeSize - iFontTreeList[0]->CacheSize())), EDirectGdiPanicGlyphCacheDataInconsistent);
			iCacheSize = 0;
			}
		else
			{
			CFontGlyphTree* fontTree = iFontTreeList[iFontTreeList.Count() - 1];
			iFontTreeList.Remove(iFontTreeList.Count() - 1);
			GRAPHICS_ASSERT_DEBUG(iCacheSize >= fontTree->CacheSize(), EDirectGdiPanicGlyphCacheDataInconsistent);
			iCacheSize -= fontTree->CacheSize(); //since we removed the tree, we must update overall cache size to reflect this fact
			delete fontTree;
			}	
		}
	}

/**
Removes all glyph trees from the storage. 

@see MFontGlyphImageStorage
*/
void CFontGlyphImageStorage::CleanGlyphImageCache()
	{
	iFontTreeList.ResetAndDestroy();
	iCacheSize = 0;
	}

/**
@test

@return Current glyph cache size in bytes.
@see MFontGlyphImageStorage
*/
TInt CFontGlyphImageStorage::GlyphCacheSize() const
	{
	return iCacheSize;
	}

/**
Sets the maximum size in bytes of the glyph cache. Checks the current size of
the cache and sets the maximum cache size if the current cache size is smaller
or equal to aCacheSize.
@param aMaxCacheSize The maximum size in bytes to allow for the glyph cache.
@return KErrNone if the maximum cache size has been changed successfully,
KErrArgument if aMaxCacheSize is smaller than the current cache size. 
 */
TInt CFontGlyphImageStorage::SetMaxGlyphCacheSize(TInt aMaxCacheSize)
	{
	if (iCacheSize <= aMaxCacheSize)
		{
		iMaxCacheSize = aMaxCacheSize;
		return KErrNone;
		}
	
	return KErrArgument;
	}
	
/**
@return Max glyph cache size in bytes.
@see MFontGlyphImageStorage
*/
TInt CFontGlyphImageStorage::MaxGlyphCacheSize() const
	{
	return iMaxCacheSize;
	}

/**
Fills aFontListId parameter with font Id in order from most to least usable. 

@test
@see MFontGlyphImageStorage

@return KErrNone, if the insertion is successful, otherwise one of the system wide error codes.
*/
TInt CFontGlyphImageStorage::FontIdInOrder(RArray<TUint32> & aFontListId) const
	{
	TInt err = KErrNone;
	aFontListId.Reset();
	
	for(TInt index = 0; (index < iFontTreeList.Count()) && (err == KErrNone); index++)
		{
		CFontGlyphTree* glyphTree = iFontTreeList[index];
		err = aFontListId.Append(glyphTree->FontId());
		}
		
	return err;
	}

/**
Enforce the system to emulate OOM failure. As sequence pre-allocated images will be used.

@test
@see MFontGlyphImageStorage
*/
#ifdef _DEBUG
void CFontGlyphImageStorage::EnforceOOMFailure(TBool aEnforce)
	{
	iEnforceOOM = aEnforce;
	}
#else
void CFontGlyphImageStorage::EnforceOOMFailure(TBool /*aEnforce*/)
	{
	}
#endif

/**
Retrieves OpenVG images from the font image cache. If the image doesn't exist, the function will create a new one from the bitmap glyph image and 
add it into the font image cache.
Each font corresponds to a particular tree of glyph images.
The function's search is performed in two steps:
	1.	It tries to identify the glyph tree associated with the font. Otherwise a new tree will be created
	2.	Within the tree the function will search for the particular glyph entry. Otherwise a new glyph entry will be created
If the size of the cache exceeds some pre-defined value, the least usable tree with all its entries will be deleted.
If the function fails to place the element into the tree due to shortage of memory, it will 
still try to create VGImages without adding them to the binary tree.
@see	CVgEngine::DrawGlyph
@see 	MFontGlyphImageStorage

@param	aFontId Unique Font Id.
@param	aGlypCode General Unicode character value.
@param	aGlyphBitmapType A type for the format of a glyph bitmap.
@param	aGlyphImage Glyph bitmap image data.
@param	aGlyphImageSize The size of the glyph bitmap image data.
@param  aImageForeground Pointer to VGImage text foreground handle.
@param  aImageShadow Pointer to VGImage text shadow handle.
@param  aImageOutline Pointer to VGImage text outline handle.

@pre 	Rendering engine has been constructed.
@post 	Requested  OpenVG images are ready for rendering. 

@panic DGDIAdapter 1018, if a null glyph image pointer is passed in.
@return On success KErrNone, 
		KErrArgument if passed parameters are not correct,
		KErrNotSupported if functionality is not supported,	otherwise one of the other system-wide error codes.
*/
TInt CFontGlyphImageStorage::GlyphImage(TUint32 aFontId, TChar aGlypCode, TGlyphBitmapType aGlyphBitmapType, const TUint8* aGlyphImage, const TSize& aGlyphImageSize, 
				TAny* aImageForeground, TAny* aImageShadow, TAny* aImageOutline)
	{
	if((aGlyphImageSize.iHeight <= 0) || (aGlyphImageSize.iWidth <= 0))
		{
		return KErrArgument;
		}
	GRAPHICS_ASSERT_ALWAYS(aGlyphImage, EDirectGdiPanicInvalidPointer); // maybe needs to change assertion type
	GRAPHICS_ASSERT_ALWAYS(aImageForeground, EDirectGdiPanicInvalidPointer); // maybe needs to change assertion type
	GRAPHICS_ASSERT_ALWAYS((aImageShadow && aImageOutline) || (aGlyphBitmapType != EFourColourBlendGlyphBitmap), EDirectGdiPanicInvalidPointer); // maybe needs to change assertion type

	TInt res = KErrNone;
	CFontGlyphTree* fontTree = NULL;
	TInt index = 0;

	for(; index < iFontTreeList.Count(); index++)
		{
		CFontGlyphTree* fontTreeTemp = iFontTreeList[index];
		if(fontTreeTemp->FontId() == aFontId)
			{
			fontTree = fontTreeTemp;
			break;
			}
		}

	if(!fontTree)
		{ 
#ifdef _DEBUG
		if(!iEnforceOOM)
			{
#endif		
			//there is no tree for that font, thus create one
			TRAP(res, fontTree = CFontGlyphTree::NewL(aFontId, aGlyphBitmapType));
			if(res == KErrNone)
				{
				res = iFontTreeList.Insert(fontTree, 0);
				if(res != KErrNone)
					{
					delete fontTree;
					fontTree = NULL;
					}
				}
#ifdef _DEBUG
			}
#endif		
		}
	else if(index != 0)
		{//reorder the tree
		iFontTreeList.Remove(index);
		res = iFontTreeList.Insert(fontTree, 0);
		if(res != KErrNone)
			{ //we have to delete fontTree as it is not stored anywhere.
			delete fontTree;
			fontTree = NULL;
			}
		}	

#ifdef _DEBUG	
	if(iEnforceOOM && (res == KErrNone))
		{
		res = KErrNoMemory;	
		}
#endif		
	if(res == KErrNone)
		{
		CleanCacheIfRequired();
		TInt treeCacheSize = fontTree->CacheSize();
		if(aGlyphBitmapType == EFourColourBlendGlyphBitmap)
			{
			TRAP(res, fontTree->GlyphImageEntryL(aGlypCode, aGlyphImage, aGlyphImageSize, iEntryCompound, NULL));
			}
		else
			{
			TRAP(res, fontTree->GlyphImageEntryL(aGlypCode, aGlyphImage, aGlyphImageSize, iEntry, aGlyphBitmapType == EMonochromeGlyphBitmap ? iForegroundData : NULL));
			}	
		if(res == KErrNone)
			{
			iCacheSize += (fontTree->CacheSize() - treeCacheSize);
			if(aGlyphBitmapType == EFourColourBlendGlyphBitmap)
				{
				*(static_cast <VGImage*> (aImageForeground)) = iEntryCompound.iForeground;
				*(static_cast <VGImage*> (aImageShadow)) = iEntryCompound.iShadow;
				*(static_cast <VGImage*> (aImageOutline)) = iEntryCompound.iOutline;
				}
			else
				{
				if(aImageShadow)
					{
					*(static_cast <VGImage*> (aImageShadow)) = VG_INVALID_HANDLE;
					}
				if(aImageOutline)
					{
					*(static_cast <VGImage*> (aImageOutline)) = VG_INVALID_HANDLE;
					}

				*(static_cast <VGImage*> (aImageForeground)) = iEntry.iForeground;
				}	
			}
		}
		
	//create glyph images for OOM conditions, without putting the entry into the tree
	if((res == KErrNoMemory) && iImagesPreAllocated && (iImageSize.iWidth >= aGlyphImageSize.iWidth ) && (iImageSize.iHeight >= aGlyphImageSize.iHeight))
		{
		const TInt bufferSize = iImageSize.iWidth * iImageSize.iHeight;
		Mem::FillZ(iForegroundData, bufferSize);
		VGImageFormat imageFormat = VG_sL_8;
		TInt vgCompatibleSourceStride = iImageSize.iWidth;

		if(aGlyphBitmapType == EFourColourBlendGlyphBitmap)
			{
			Mem::FillZ(iShadowData, bufferSize);
			Mem::FillZ(iOutlineData, bufferSize);
			iEntryCompound.iForeground = iImageForeground;
			iEntryCompound.iOutline = iImageOutline;
			iEntryCompound.iShadow = iImageShadow;
			
			vgImageSubData(iEntryCompound.iForeground, iForegroundData, vgCompatibleSourceStride,	imageFormat, 
					0,	0, iImageSize.iWidth, iImageSize.iHeight);

			vgImageSubData(iEntryCompound.iOutline, iOutlineData, vgCompatibleSourceStride,	imageFormat, 
					0,	0, iImageSize.iWidth, iImageSize.iHeight);

			vgImageSubData(iEntryCompound.iShadow, iShadowData, vgCompatibleSourceStride,	imageFormat, 
					0,	0, iImageSize.iWidth, iImageSize.iHeight);

			TRAP_IGNORE(CFontGlyphTree::GlyphImageEntryOOML(aGlyphBitmapType, aGlyphImage, aGlyphImageSize, iEntryCompound, iForegroundData, iShadowData, iOutlineData));
			*(static_cast <VGImage*> (aImageForeground)) = iImageForeground;
			*(static_cast <VGImage*> (aImageShadow)) = iImageShadow;
			*(static_cast <VGImage*> (aImageOutline)) = iImageOutline;
			}
		else
			{
			iEntry.iForeground = iImageForeground;
			vgImageSubData(iEntry.iForeground, iForegroundData, vgCompatibleSourceStride,	imageFormat, 
				0,	0, iImageSize.iWidth, iImageSize.iHeight);
			
			TRAP_IGNORE(CFontGlyphTree::GlyphImageEntryOOML(aGlyphBitmapType, aGlyphImage, aGlyphImageSize, iEntry, iForegroundData, NULL, NULL));
			*(static_cast <VGImage*> (aImageForeground)) = iImageForeground;
			}	
		}
	
	return res;
	}

/**
Creates VGImages for use in low memory conditions
*/
TInt CFontGlyphImageStorage::PreAllocateImages()
	{
	iImageSize = KMaxSizeImageOOM;

#ifdef DRAWGLYPH_MULTIPLY_MODE
	// For image_multiply mode need 32bits for each glyph instrad of 8 for non-multiply mode.
	const TInt bufferSize = iImageSize.iWidth * iImageSize.iHeight * 4;
#else
	const TInt bufferSize = iImageSize.iWidth * iImageSize.iHeight;
#endif
	if(!iForegroundData)
		{
		iForegroundData = (TUint8*) User::Alloc(bufferSize);
		if(!iForegroundData)
			{
			return KErrNoMemory;
			}
		}
	
	if(!iShadowData)
		{
		iShadowData = (TUint8*) User::Alloc(bufferSize);
		if(!iShadowData)
			{
			return KErrNoMemory;
			}
		}

	if(!iOutlineData)
		{
		iOutlineData = (TUint8*) User::Alloc(bufferSize);
		if(!iOutlineData)
			{
			return KErrNoMemory;
			}
		}

	const VGImageFormat imageFormat = VG_sL_8;
	if(iImageForeground == VG_INVALID_HANDLE)
		{
		
		iImageForeground = vgCreateImage(imageFormat,
	                              iImageSize.iWidth, 
	                              iImageSize.iHeight,
	                              VG_IMAGE_QUALITY_NONANTIALIASED);
		if(iImageForeground == VG_INVALID_HANDLE)
			{
			return KErrNoMemory;
			}
		}

	if(iImageShadow == VG_INVALID_HANDLE)
		{
		iImageShadow = vgCreateImage(imageFormat,
	                              iImageSize.iWidth, 
	                              iImageSize.iHeight,
	                              VG_IMAGE_QUALITY_NONANTIALIASED);
		if(iImageShadow == VG_INVALID_HANDLE)
			{
			return KErrNoMemory;
			}
		}

	if(iImageOutline == VG_INVALID_HANDLE)
		{
		iImageOutline = vgCreateImage(imageFormat,
	                              iImageSize.iWidth, 
	                              iImageSize.iHeight,
	                              VG_IMAGE_QUALITY_NONANTIALIASED);
		if(iImageOutline == VG_INVALID_HANDLE)
			{
			return KErrNoMemory;
			}
		}

	iImagesPreAllocated = ETrue;	
	return KErrNone;
	}

/**
Deletes all pre-allocated images and frees buffers. 
*/
void CFontGlyphImageStorage::DeletePreAllocatedImages()
	{
	if(iForegroundData)
		{
		User::Free(iForegroundData);
		iForegroundData = NULL;
		}

	if(iShadowData)
		{
		User::Free(iShadowData);
		iShadowData = NULL;
		}

	if(iOutlineData)
		{
		User::Free(iOutlineData);
		iOutlineData = NULL;
		}
		
	if(iImageForeground != VG_INVALID_HANDLE)
		{
		vgDestroyImage(iImageForeground);
		iImageForeground = VG_INVALID_HANDLE;
		}
		
	if(iImageShadow != VG_INVALID_HANDLE)
		{
		vgDestroyImage(iImageShadow);
		iImageShadow = VG_INVALID_HANDLE;
		}
		
	if(iImageOutline != VG_INVALID_HANDLE)
		{
		vgDestroyImage(iImageOutline);
		iImageOutline = VG_INVALID_HANDLE;
		}
	iImagesPreAllocated = EFalse;	
	}
