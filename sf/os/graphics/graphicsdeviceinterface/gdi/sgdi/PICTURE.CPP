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

#include <gdi.h>
#include "GDIPANIC.h"

_LIT(KGdiCPicturePanicCategory,"CPicture");

EXPORT_C TPictureHeader::TPictureHeader():
	iPicture(NULL),
	iPictureType(KNullUid),
	iSize()
/** Constructs a default picture header. */
	{}

EXPORT_C void TPictureHeader::DeletePicture()
/** Deletes the internalised picture.

The picture can only be deleted if the picture has been internalized. */
	{
	if (iPicture.IsPtr())
		{
		delete iPicture.AsPtr();
		iPicture=NULL;
		}
	}


// Persist this object
// Does not take responsibility for storing the picture
//
EXPORT_C void TPictureHeader::ExternalizeL(RWriteStream& aStream)const
/** Externalises a picture header object to a write stream. 

The presence of this function means that the standard templated stream operator<<() 
is available to externalise objects of this class.

@param aStream The write stream. */
    {
    aStream<< iPictureType;
    aStream<< iPicture;
	aStream.WriteInt32L(iSize.iWidth);
	aStream.WriteInt32L(iSize.iHeight);
    }


// Restore this object from the specified stream
// Does not take responsibility for restoring the picture
//
EXPORT_C void TPictureHeader::InternalizeL(RReadStream& aStream)
/** Internalises a picture header object from a read stream.

The presence of this function means that the standard templated stream operator>>() 
is available to internalise objects of this class.

@param aStream The read stream. */
    {
    aStream>> iPictureType;
    aStream>> iPicture;
	iSize.iWidth=aStream.ReadInt32L();
	iSize.iHeight=aStream.ReadInt32L();
    }

EXPORT_C CPicture::CPicture():
	CBase()
	{
	__DECLARE_NAME(_S("CPicture"));
	}

EXPORT_C CPicture::~CPicture()
/** Frees all resources owned by the object prior to its destruction. */
	{}

EXPORT_C TPictureCapability CPicture::Capability() const
/** Gets the picture's capabilities.

These include whether it is scalable and croppable.

@return The capabilities of the picture. */
	{
	return TPictureCapability(TPictureCapability::ENotScaleable,EFalse);
	}

// Assumes everything goes into the head stream.
// (Must be replaced by concrete pictures that have components)
//
EXPORT_C TStreamId CPicture::StoreL(CStreamStore& aStore) const
/** Stores the picture to the specified store.

The default implementation assumes that the content of the picture is externalized 
to a single stream. The implementation may need to be changed for those derived 
classes that have components.

@param aStore The store. 
@return The ID of the (head) stream used to store the picture. */
	{
	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
	ExternalizeL(stream);  // provided by the concrete picture stream.CommitL();
	stream.CommitL();
	CleanupStack::PopAndDestroy();
	return id;
	}

EXPORT_C void CPicture::ResetToOriginal()
/** Resets the picture's scaling and cropping attributes to their original values. */
	{
	TMargins cropInTwips;
	cropInTwips.iLeft=0;
	cropInTwips.iRight=0;
	cropInTwips.iTop=0;
	cropInTwips.iBottom=0;
	SetCropInTwips(cropInTwips);
	SetScaleFactor(1000,1000);
	}

EXPORT_C void CPicture::GetSizeInPixels(MGraphicsDeviceMap* aMap,TSize& aSize) const
/** Gets the picture's size in pixels. 

This is calculated from the original size of the picture, taking cropping 
and scaling into account.

@param aMap The pixels to twips mapping interface of the graphics device 
@param aSize The size of the picture, in pixels. */
	{
	GDI_ASSERT_ALWAYS_GENERAL(aMap!=NULL,User::Panic(KGdiCPicturePanicCategory,KErrNotFound));
	TSize size;
	GetSizeInTwips(size);
	aSize.iWidth = aMap->HorizontalTwipsToPixels(size.iWidth);
	aSize.iHeight = aMap->VerticalTwipsToPixels(size.iHeight);
	}

EXPORT_C void CPicture::SetSizeInPixels(MGraphicsDeviceMap* aMap,const TSize& aSize)
/** Sets the picture's size in pixels.

@param aMap The pixels to twips mapping interface of the graphics device. 
@param aSize The size of the picture, in pixels. */
	{
	GDI_ASSERT_ALWAYS_GENERAL(aMap!=NULL,User::Panic(KGdiCPicturePanicCategory,KErrNotFound));
	TSize size;
	size.iWidth = aMap->HorizontalPixelsToTwips(aSize.iWidth);
	size.iHeight = aMap->VerticalPixelsToTwips(aSize.iHeight);
	SetSizeInTwips(size);
	}

EXPORT_C void CPicture::SetScaleFactor(TInt /*aScaleFactorWidth*/,TInt /*aScaleFactorHeight*/)
/** Sets the picture's scale factors.
	
	@param aScaleFactorWidth The width scale factor, in percent. 
	@param aScaleFactorHeight The height scale factor, in percent. */
	{
	}

EXPORT_C TInt CPicture::ScaleFactorWidth() const
/** Gets the picture's width scale factor.

@return The width scale factor, in percent. */
	{
	return 1000; // no scaling
	}

EXPORT_C TInt CPicture::ScaleFactorHeight() const
/** Gets the pictures height scale factor.

@return The height scale factor, in percent. */
	{
	return 1000; // no scaling
	}

EXPORT_C void CPicture::GetCropInTwips(TMargins& aCrop) const
/** Gets the cropping margins of a picture in twips.
	
These margins are relative to the original unscaled size of the picture.
	
@param aMargins The cropping margins of the picture, in twips */
    {
	aCrop.iLeft=0;
	aCrop.iRight=0;
	aCrop.iTop=0;
	aCrop.iBottom=0;
	}

EXPORT_C void CPicture::SetCropInTwips(const TMargins& /*aMargins*/)
/** Sets the cropping margins of a picture in twips.
	
These are relative to the original unscaled size of the picture.
	
@param aMargins The cropping margins of the picture, in twips. */
	{
	}


EXPORT_C TBool CPicture::LineBreakPossible(TUint /*aClass*/,TBool /*aBeforePicture*/,TBool /*aHaveSpaces*/) const
/** States whether a line break is possible, either before or after a picture.
	
The default implementation returns ETrue, implying that there is a break opportunity 
both before and after the picture, whether or not a space is present.

This may be overridden for special types of pictures.

@param aClass The line breaking class of the adjacent character. Line breaking 
classes are defined in the header file, tagma.h
@param aBeforePicture ETrue, if the adjacent character is before the picture; 
EFalse, if the adjacent character is afterwards.
@param aHaveSpaces ETrue, if spaces occur between the adjacent character and 
the picture; EFalse, otherwise.
@return ETrue, if a line break is possible; EFalse, otherwise. */
	{
	return TRUE;
	}



EXPORT_C TBool CPicture::NativePixelSize(TSize&)
/** Derived classes might be implemented as bitmaps, in that case it might 
be interesting to now the native pixel size of the bitmap. */
	{
	return EFalse;
	}

EXPORT_C void CPicture::GetSizeInTwips(TSize& aSize) const
/** Gets the picture's size, in twips.

This size is calculated from the original size of the picture, taking cropping 
and scaling into account.

@param aSize The size of the picture, in twips. */
	{
	TSize originalSizeInTwips;
	GetOriginalSizeInTwips(originalSizeInTwips);
	TMargins cropInTwips;
	GetCropInTwips(cropInTwips);
	aSize.iWidth = (ScaleFactorWidth()*(originalSizeInTwips.iWidth-cropInTwips.iLeft-cropInTwips.iRight))/1000;
	aSize.iHeight = (ScaleFactorHeight()*(originalSizeInTwips.iHeight-cropInTwips.iTop-cropInTwips.iBottom))/1000;
	}

EXPORT_C void CPicture::SetSizeInTwips(const TSize& aSize)
/** Sets the picture's size, in twips

@param aSize The size of the picture, in twips. */
	{
	TSize originalSizeInTwips;
	GetOriginalSizeInTwips(originalSizeInTwips);
	TMargins cropInTwips;
	GetCropInTwips(cropInTwips);
	TSize size;
	size.iWidth  = originalSizeInTwips.iWidth-cropInTwips.iLeft-cropInTwips.iRight;
	size.iHeight = originalSizeInTwips.iHeight-cropInTwips.iTop-cropInTwips.iBottom;
	TSize scalefactor;
	if(size.iWidth!=0) scalefactor.iWidth=1000*aSize.iWidth/size.iWidth;
	if(size.iHeight!=0) scalefactor.iHeight=1000*aSize.iHeight/size.iHeight;
	SetScaleFactor(scalefactor.iWidth,scalefactor.iHeight);
 	}

EXPORT_C void CPicture::AddCropInPixels(MGraphicsDeviceMap* aMap,const TMargins& aMargins)
/** Adds pixel cropping margins to the picture.

@param aMap The pixels to twips mapping interface of the graphics device 
@param aMargins The additional pixel cropping margins for the picture, in pixels. */
	{
	GDI_ASSERT_ALWAYS_GENERAL(aMap!=NULL,User::Panic(KGdiCPicturePanicCategory,KErrNotFound));
	TMargins cropInTwips;
	GetCropInTwips(cropInTwips);
	TInt scaleFactorWidth=ScaleFactorWidth();
	if(scaleFactorWidth!=0)
		{
		cropInTwips.iLeft += (1000*aMap->HorizontalPixelsToTwips(aMargins.iLeft))/scaleFactorWidth;
		cropInTwips.iRight += (1000*aMap->HorizontalPixelsToTwips(aMargins.iRight))/scaleFactorWidth;
		}
	TInt scaleFactorHeight=ScaleFactorHeight();
	if(scaleFactorHeight!=0)
		{
		cropInTwips.iTop += (1000*aMap->VerticalPixelsToTwips(aMargins.iTop))/scaleFactorHeight;
		cropInTwips.iBottom += (1000*aMap->VerticalPixelsToTwips(aMargins.iBottom))/scaleFactorHeight;
		}
	SetCropInTwips(cropInTwips);
	}

