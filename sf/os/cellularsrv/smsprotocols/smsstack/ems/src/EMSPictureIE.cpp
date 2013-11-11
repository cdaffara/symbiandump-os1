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
* Implements the Enhanced SMS Picture (Small/Large/Variable) Information Element.
*
*/


/**
 *  @file
 *  
 *  Implements  CEMSPictureIE class
 */


#include <emspictureie.h>

#include <fbs.h>

CEmsPictureIE* CEmsPictureIE::NewL()
	{
	CEmsPictureIE* self = new (ELeave) CEmsPictureIE();
	return self;
	}

EXPORT_C CEmsPictureIE* CEmsPictureIE::NewL(const CFbsBitmap& aBitmap)
/**
 *  @capability None
 */
	{
	CEmsPictureIE* self = new (ELeave) CEmsPictureIE();
	CleanupStack::PushL(self);
	self->ConstructL(aBitmap);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CEmsInformationElement* CEmsPictureIE::DuplicateL() const
/**
 *  @capability None
 */
	{
	CEmsPictureIE* copy = new (ELeave) CEmsPictureIE();
	CleanupStack::PushL(copy);
	copy->CopyL(*this);
	CleanupStack::Pop(copy);
	return copy;
	}

EXPORT_C TSize CEmsPictureIE::SizeInPixels() const
/**
 *  @capability None
 */
	{
	return iSize;
	}

void CEmsPictureIE::ConstructL(const CFbsBitmap& aBitmap)
	{
	__ASSERT_ALWAYS(!iPictureBuf, User::Leave(KErrCorrupt));

	iSize = aBitmap.SizeInPixels();
	if ((iSize == KSmallPicSize) || (iSize == KLargePicSize))
		{
		iIdentifier  = (iSize == KSmallPicSize) ? CSmsInformationElement::ESmsEnhancedSmallPicture : CSmsInformationElement::ESmsEnhancedLargePicture;
		iPictureBuf = CreatePictureBufferL(aBitmap);
		iEncodedBodyLength=iSize.iHeight*iSize.iWidth/8;
		}
	else
		{
		// Make sure bitmap is a x of 8 bits wide
		iIdentifier  = CSmsInformationElement::ESmsEnhancedVariablePicture;

		TSize size = aBitmap.SizeInPixels();
		iSize.iHeight = size.iHeight;
		if (size.iWidth%8 != 0)
			User::Leave(KErrCorrupt);
		iPictureBuf = CreatePictureBufferL(aBitmap);

		// The length is the picture size plus two bytes to specify
		// the width and height
		iEncodedBodyLength=iSize.iHeight*iSize.iWidth/8+2;
		}
	__ASSERT_ALWAYS(iPictureBuf->Length() <= EEnhancedMaxSize, User::Leave(KErrTooBig));
	}

HBufC8* CEmsPictureIE::CreatePictureBufferL(const CFbsBitmap& aBitmap)
	{
	__ASSERT_ALWAYS(aBitmap.SizeInPixels().iWidth%8==0, User::Leave(KErrCorrupt));

	const TSize size = aBitmap.SizeInPixels();
	const TInt numLineBytes = size.iWidth/8;
	HBufC8* pictureBuf = HBufC8::NewLC(numLineBytes * size.iHeight);

	HBufC8* scanLine = HBufC8::NewLC(numLineBytes);
	TPtr8 line(scanLine->Des());

	HBufC8* working = HBufC8::NewLC(numLineBytes);
	TPtr8 convertedScanLine(working->Des());
	convertedScanLine.SetLength(numLineBytes);

	TUint8 byte;

	for (TInt a=0; a<size.iHeight; a++)
		{
		aBitmap.GetScanLine(line, TPoint(0,a), size.iWidth, EGray2);

		for (TInt word=0; word<numLineBytes; word++)
			{
			// Convert EMS Black=1 to Epoc Black=0
			// and Convert little endean to big.
			byte = 0;
			for (TInt bit=0; bit<8; bit++)
				byte |= (line[word] & (1<<bit)) ? 0 : (1<<(7-bit));

			convertedScanLine[word] = byte;
			}

		pictureBuf->Des().Append(convertedScanLine);
		}

	CleanupStack::PopAndDestroy(2, scanLine);
	CleanupStack::Pop(pictureBuf);
	return pictureBuf;
	}

EXPORT_C void CEmsPictureIE::CopyL(const CEmsPictureIE& aSrc)
/**
 *  @capability None
 */
	{
	__ASSERT_ALWAYS(aSrc.iPictureBuf, User::Leave(KErrCorrupt));
	CEmsInformationElement::CopyL(aSrc);
	iSize = aSrc.iSize;
	delete iPictureBuf;
	iPictureBuf=NULL;
	iPictureBuf = aSrc.iPictureBuf->AllocL();
	}

/**
 *  Encodes the information element into its raw format. (no IE id)
 *  
 *  @param aPtr the buffer to be used which is to contain the data
 *  @param TBool boolean to indicate if it is for serialisation or encoding
 */
void CEmsPictureIE::EncodeBodyL(TPtr8 aPtr, TBool) const
	{
	__ASSERT_ALWAYS(iPictureBuf, User::Leave(KErrCorrupt));
	__ASSERT_ALWAYS(iSize.iWidth<255 && iSize.iHeight<255, User::Leave(KErrCorrupt));
	TBool variablePicture = ((iSize != KSmallPicSize) && (iSize != KLargePicSize));
	__ASSERT_ALWAYS((variablePicture && iIdentifier == CSmsInformationElement::ESmsEnhancedVariablePicture) ||
		(!variablePicture && iIdentifier != CSmsInformationElement::ESmsEnhancedVariablePicture), User::Leave(KErrCorrupt));

	if (variablePicture)
		{
		aPtr.Append(static_cast<TUint8>(iSize.iWidth/8));
		aPtr.Append(static_cast<TUint8>(iSize.iHeight));
		}

	aPtr.Append(*iPictureBuf);
	}


/**
 *  Decodes the raw data out of an information element into this class.
 *  
 *  @param aPtr The raw predefined animation data
 *  @param TBool boolean to indicate if it is from serialisation
 *  @leave KErrGeneral If the size of the data does not match what is expected.
 */
void CEmsPictureIE::DecodeBodyL(const TPtrC8 aPtr, TBool)
	{
	switch (iIdentifier)
		{
		case CSmsInformationElement::ESmsEnhancedSmallPicture:
			__ASSERT_ALWAYS(aPtr.Length()==ESmallBufSize, User::Leave(KErrGeneral));
			iSize = KSmallPicSize;
			break;
		case CSmsInformationElement::ESmsEnhancedLargePicture:
			__ASSERT_ALWAYS(aPtr.Length()==ELargeBufSize, User::Leave(KErrGeneral));
			iSize = KLargePicSize;
			break;
		case CSmsInformationElement::ESmsEnhancedVariablePicture:
			iSize.iWidth = static_cast<TUint8>(aPtr[0]) * 8;
			iSize.iHeight = static_cast<TUint8>(aPtr[1]);
			__ASSERT_ALWAYS(aPtr.Length()==(iSize.iWidth/8*iSize.iHeight)+2, User::Leave(KErrGeneral));
			break;
		default:
			User::Leave(KErrArgument);
		}

	delete iPictureBuf;
	iPictureBuf = NULL;
	if (iIdentifier == CSmsInformationElement::ESmsEnhancedVariablePicture)
		iPictureBuf = aPtr.Mid(2).AllocL();
	else
		iPictureBuf = aPtr.AllocL();
	}

CEmsPictureIE::CEmsPictureIE() : CEmsInformationElement(CSmsInformationElement::ESmsEnhancedSmallPicture){};

EXPORT_C CEmsPictureIE::~CEmsPictureIE()
/**
 *  @capability None
 */
	{
	delete iPictureBuf;
	}



EXPORT_C CFbsBitmap* CEmsPictureIE::GetBitmapL() const
/**
 *  @capability None
 */
	{

	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);

	TInt numLineBytes = iSize.iWidth/8;
	User::LeaveIfError(bitmap->Create(iSize,EGray2));
	HBufC8* working = HBufC8::NewLC(numLineBytes);
	TPtr8 convertedScanLine(working->Des());
	convertedScanLine.SetLength(numLineBytes);

	TUint8 byte = 0;
	TUint8* adr=NULL;

	for (TInt line =0; line < iSize.iHeight; line++)
		{
		adr = &((iPictureBuf->Des())[(numLineBytes) * line]);
		TPtr8  scanLine(adr, numLineBytes, numLineBytes);

		for (TInt word=0; word<numLineBytes; word++)
			{
			// Convert EMS Black=1 to Epoc Black=0
			// and Convert little endean to big.
			byte=0;
			for (TUint bit=0; bit<8; bit++)
				byte |= (scanLine[word] & (1<<bit)) ? 0 : (1<<(7-bit));
			convertedScanLine[word] = byte;
			}

		bitmap->SetScanLine(convertedScanLine, line);
		}
	CleanupStack::PopAndDestroy(working);
	CleanupStack::Pop(bitmap);
	return bitmap;
	}
