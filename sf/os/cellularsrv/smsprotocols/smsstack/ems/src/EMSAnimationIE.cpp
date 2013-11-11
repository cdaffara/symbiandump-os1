/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Implements the Enhanced SMS Animation Information Element.
*
*/


/**
 *  @file
 *  
 *  Implements  CEmsPreDefAnimationIE and CEmsAnimationIE classes
 */

#include <emsanimationie.h>
#include <w32std.h>

/**
 *  Creates a new Predifined animation class.
 *  @param aAnimValue number of the predefined animations
 */
EXPORT_C CEmsPreDefAnimationIE* CEmsPreDefAnimationIE::NewL(TAnimType aAnimValue)
/**
 *  @capability None
 */
	{
	if(aAnimValue > EDevil)User::Leave(KErrArgument);
	CEmsPreDefAnimationIE* self = new (ELeave) CEmsPreDefAnimationIE(aAnimValue);
	return self;
	}

EXPORT_C CEmsInformationElement* CEmsPreDefAnimationIE::DuplicateL() const
/**
 *  @capability None
 */
	{
	CEmsPreDefAnimationIE* copy = new (ELeave) CEmsPreDefAnimationIE();
	CleanupStack::PushL(copy);
	copy->CopyL(*this);
	CleanupStack::Pop(copy);
	return copy;
	}

EXPORT_C void CEmsPreDefAnimationIE::CopyL(const CEmsPreDefAnimationIE& aSrc)
/**
 *  @capability None
 */
	{
	CEmsInformationElement::CopyL(aSrc);
	iAnimNumber = aSrc.iAnimNumber;
	}

EXPORT_C CEmsPreDefAnimationIE::TAnimType  CEmsPreDefAnimationIE::Animation() const
/**
 *  @capability None
 */
	{
	return iAnimNumber;
	}

EXPORT_C void CEmsPreDefAnimationIE::SetAnimationL(TAnimType  aValue)
/**
 *  @capability None
 */
	{
	 if(aValue > EDevil)User::Leave(KErrArgument);
	 iAnimNumber = aValue;
	}


CEmsPreDefAnimationIE::CEmsPreDefAnimationIE(TAnimType  aAnimValue) :
	CEmsInformationElement(CSmsInformationElement::ESmsEnhancedPredefinedAnimation), iAnimNumber(aAnimValue)
	{
	iEncodedBodyLength=EIESize;
	}

CEmsPreDefAnimationIE::CEmsPreDefAnimationIE() :
	CEmsInformationElement(CSmsInformationElement::ESmsEnhancedPredefinedAnimation)
	{
	iEncodedBodyLength=EIESize;
	}

/**
 *  Encodes the raw Ems information element into a data buffer that can be use to create a normal information element.
 *  
 *  @param aPtr the buffer to be used which is to contain the data
 *  @param TBool boolean to indicate if it is for serialisation or encoding
 */
void CEmsPreDefAnimationIE::EncodeBodyL(TPtr8 aPtr, TBool) const
	{
	// We don't need to worry about the aIsForSerialisation flag since
	// encoding and serialisation will result in the same data
	aPtr.Append(static_cast<TUint8>(iAnimNumber));
	}

/**
 *  Decodes the raw data out of an information element into this class.
 *  
 *  @param aPtr The raw predefined animation data
 *  @param TBool boolean to indicate if it is from serialisation
 *  @leave KErrargument If the size of the data does not match what is expected.
 */
void CEmsPreDefAnimationIE::DecodeBodyL(const TPtrC8 aPtr, TBool)
	{
	__ASSERT_ALWAYS(aPtr.Length()==EIESize, User::Leave(KErrArgument));
	iAnimNumber = static_cast<TAnimType>(aPtr[0]);
	__ASSERT_DEBUG(iAnimNumber<=EDevil, User::Leave(KErrArgument));
	}

// --------------------------------------------------------------------------------------------

/**
 *  Creates a new of an animation object.
 *  
 *  @return CEmsAnimationIE* the newly created object.
 */
CEmsAnimationIE* CEmsAnimationIE::NewL()
	{
	CEmsAnimationIE* self = new (ELeave) CEmsAnimationIE();
	return self;
	}

/**
 *  Creates a new EMS animation object from a given bitmap
 *  
 *  @param aBitmap a 4 frame Bitmap that is converted into an EMS animation type.
 *  @return CEmsAnimationIE* the newly created object.
 *  @leave KErrTooBig if the animation does not match the size of a Small/Large animation.
 */
EXPORT_C CEmsAnimationIE* CEmsAnimationIE::NewL(const CFbsBitmap& aBitmap)
/**
 *  @capability None
 */
	{
	CEmsAnimationIE* self = new (ELeave) CEmsAnimationIE();
	CleanupStack::PushL(self);
	self->ConstructL(aBitmap);
	CleanupStack::Pop(self);
	return self;
	}

/**
 *  Creates a Copy of the object.
 *  
 *  @return CEmsAnimationIE* the newly created object.
 */
EXPORT_C CEmsInformationElement* CEmsAnimationIE::DuplicateL() const
/**
 *  @capability None
 */
	{
	CEmsAnimationIE* copy = new (ELeave) CEmsAnimationIE();
	CleanupStack::PushL(copy);
	copy->CopyL(*this);
	CleanupStack::Pop(copy);
	return copy;
	}

/**
 *  Copys the information element out of the source into this object.
 *  
 *  @param aSrc the source object to copy from.
 */
EXPORT_C void CEmsAnimationIE::CopyL(const CEmsAnimationIE& aSrc)
/**
 *  @capability None
 */
	{
	__ASSERT_ALWAYS(aSrc.iData, User::Leave(KErrCorrupt));
	CEmsInformationElement::CopyL(aSrc);
	delete iData;
	iData=NULL;
	iData = aSrc.iData->AllocL();
	}

CEmsAnimationIE::CEmsAnimationIE() : CEmsInformationElement(CSmsInformationElement::ESmsEnhancedLargeAnimation) {};

EXPORT_C CEmsAnimationIE::~CEmsAnimationIE()
/**
 *  @capability None
 */
	{
	delete iData;
	}

/**
 *  Constructs the object from the given FbsBitmap
 *  Will convert colour bitmaps into the correct format.
 *  
 *  @see
 *  @param aBitmap the source bitmap containing the 4 frames.
 *  @leave KErrTooBig if the animation will not fit within a single pdu.
 *  @leave KErrTooBig if the animation is not one of the Two animation sizes.
 */
void CEmsAnimationIE::ConstructL(const CFbsBitmap& aBitmap)
	{
	__ASSERT_ALWAYS(aBitmap.SizeInPixels().iWidth%8==0, User::Leave(KErrCorrupt));

	TSize frameSize = aBitmap.SizeInPixels();

	////////////////////////////////////////////////////

	frameSize.iWidth /= EFramesPerMsg;
	__ASSERT_ALWAYS(frameSize==KSmallAnimSize || frameSize==KLargeAnimSize, User::Leave(KErrTooBig));
	if(frameSize==KSmallAnimSize)
		{
		iEncodedBodyLength = EIESmallSize;
		}
	else
		{
		iEncodedBodyLength = EIELargeSize;
		}

	iIdentifier = (frameSize == KSmallAnimSize) ? CSmsInformationElement::ESmsEnhancedSmallAnimation : CSmsInformationElement::ESmsEnhancedLargeAnimation;


	iData = HBufC8::NewL(frameSize.iWidth*frameSize.iHeight/2); // div PixelsPerByte * NumFrames = /2

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(frameSize, EGray2));

	// create an off-screen device and context
	CGraphicsContext* bitGc;
	CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(dev);
	User::LeaveIfError(dev->CreateContext(bitGc));
	CleanupStack::PushL(bitGc);

	for (TInt frame=0; frame<EFramesPerMsg; ++frame)
		{
		TRect srcRect(TPoint(0,0),frameSize);
		TRect dstRect(TPoint(frameSize.iWidth*frame,0), frameSize);
		bitGc->DrawBitmap(srcRect, &aBitmap, dstRect);
		HBufC8* pictureData = CEmsPictureIE::CreatePictureBufferL(*bitmap);
		CleanupStack::PushL(pictureData);
		iData->Des().Append(*pictureData);
		CleanupStack::PopAndDestroy(pictureData);
		}

	CleanupStack::PopAndDestroy(3, bitmap);
	}

/**
 *  Encodes the animation object into its raw format. (no IE id)
 *  
 *  @param aPtr the buffer to be used which is to contain the data
 *  @param TBool boolean to indicate if it is for serialisation or encoding
 */
void CEmsAnimationIE::EncodeBodyL(TPtr8 aPtr, TBool) const
	{
	aPtr.Append(*iData);
	}

/**
 *  Decodes the raw data out of an information element into this class.
 *  
 *  @param aPtr The raw predefined animation data
 *  @param TBool boolean to indicate if it is from serialisation
 *  @leave KErrargument If the size of the data does not match what is expected.
 */
void CEmsAnimationIE::DecodeBodyL(const TPtrC8 aPtr, TBool)
	{
	__ASSERT_ALWAYS(aPtr.Length()==EIESmallSize || aPtr.Length()==EIELargeSize, User::Leave(KErrCorrupt));

	delete iData;
	iData = NULL;
	iData = aPtr.AllocL();
	}


EXPORT_C CFbsBitmap* CEmsAnimationIE::GetBitmapL() const
/**
 *  @capability None
 */
	{
	TSize size;
	if (iData->Length()==EIESmallSize)
		size = KSmallAnimSize;
	else
		size = KLargeAnimSize;
	////////////////////////////////////
    __ASSERT_ALWAYS(size.iWidth*size.iHeight*EFramesPerMsg==iData->Length()*8, User::Leave(KErrCorrupt));
	TInt frameDataLength = iData->Length()/EFramesPerMsg;

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(size.iWidth*EFramesPerMsg, size.iHeight), EGray2));

	// create an off-screen device and context
	CFbsBitGc* bitGc;
	CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(dev);
	User::LeaveIfError(dev->CreateContext(bitGc));
	CleanupStack::PushL(bitGc);

	bitGc->SetBrushColor(KRgbBlack);
	bitGc->SetPenColor(KRgbBlack);
	bitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	bitGc->SetDrawMode(CGraphicsContext::EDrawModePEN);

	// Draw all frames into the master bitmap
	for (TInt frame=0; frame<EFramesPerMsg; frame++)
		{
		TPtrC8 frameData(iData->Mid(frame*frameDataLength, frameDataLength));

		CFbsBitmap* pic;
		pic = GetBitmapL(size, frameData);

		TPoint topLeft(size.iWidth*frame,0);
		bitGc->BitBlt(topLeft,pic);
		delete pic;
		};

	CleanupStack::PopAndDestroy(2, dev);
	CleanupStack::Pop(bitmap);
	return bitmap;
	}

CFbsBitmap* CEmsAnimationIE::GetBitmapL(const TSize& aSize, const TDesC8& aDataBuf)const
	{
	// bitmap must have a width that is a x of 8.
	__ASSERT_ALWAYS(aSize.iWidth%8==0, User::Leave(KErrCorrupt));
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);

	TInt numLineBytes = aSize.iWidth/8;
	User::LeaveIfError(bitmap->Create(aSize,EGray2));

	HBufC8* working = HBufC8::NewLC(numLineBytes);

	TPtr8 convertedScanLine(working->Des());
	convertedScanLine.SetLength(numLineBytes);


	TUint8 byte = 0;
	const TUint8* adr=NULL;

	for (TInt line =0; line < aSize.iHeight; line++)
		{
		adr = &(aDataBuf[(numLineBytes) * line]);
		TPtrC8 scanLine(adr, numLineBytes);

		for (TInt word=0; word<numLineBytes; word++)
			{
			// Convert EMS Black=1 to Epoc Black=0
			// and Convert little endean to big.
			byte = 0;
			for (TInt bit=0; bit<8; bit++)
				byte |= (scanLine[word] & (1<<bit)) ? 0 : (1<<(7-bit));

			convertedScanLine[word] = byte;
			}

		bitmap->SetScanLine(convertedScanLine, line);
		}

	CleanupStack::PopAndDestroy(working);
	CleanupStack::Pop(bitmap);
	return bitmap;
	}
