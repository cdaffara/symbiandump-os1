// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "PDRSTD.H"
#include <fbs.h>
#include <bitdev.h>
#include <banddev.h>
#include <pdrstore.h>
#include "pdrtext.h"

EXPORT_C CFbsDrvDevice::CFbsDrvDevice()
	{
	__DECLARE_NAME(_S("CFbsDrvDevice"));
	}

EXPORT_C CFbsDrvDevice::~CFbsDrvDevice()
	{
	delete iFbsTypefaceStore;
	delete iGenTypefaceFontsList;
	delete iGenTypefaceFontsType;
	}

/** Creates a font from those available in the printer device's 
typeface store that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

This function is replaced by GetNearestFontToDesignHeightInTwips()

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes.
@deprecated */
EXPORT_C TInt CFbsDrvDevice::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/** Creates a font from those available in the printer device's 
typeface store that most closely matches a font specification. 

When the font is no longer needed, call ReleaseFont().

This function replaces GetNearestFontInTwips()

@param aFont On return, points to the font which most closely matches the 
specified font.
@param aFontSpec An absolute font specification. Its iHeight member is 
interpreted as being in twips.
@return KErrNone if successful; otherwise, another one of the system-wide error 
codes. */
EXPORT_C TInt CFbsDrvDevice::GetNearestFontToDesignHeightInTwips(CFont *&aFont, const TFontSpec &aFontSpec)
	{
    TRAPD(errCode, LoadTypeFaceListL());
	if(errCode != KErrNone)
		{
		return errCode;
		}
	TInt count = iGenTypefaceFontsList->Count();
	TInt count_fbs = 0;
	TTypefaceSupport support ;				// holds typeface from iGenTypefaceFontsList
	TTypeface typeface = aFontSpec.iTypeface;	// holds typeface from aFontSpec
	TBuf<KMaxTypefaceNameLength> support_name;
	TBuf<KMaxTypefaceNameLength> typeface_name;
	TBuf<KMaxTypefaceNameLength> aname;
	typeface_name.Copy(typeface.iName);
	TInt loop;
	TInt found = 0;
	TInt listindex = 0;
	TTypefaceSupport lsupport;
	// Try to match specified font name with name from typeface font list
	for (loop = 0; (loop < count) && (typeface.iName.CompareF(support.iTypeface.iName)); loop++)
		{
		TInt index = iGenTypefaceFontsList->At(loop);
		TInt type = iGenTypefaceFontsType->At(loop);
		if (type == 0)
			{
			iTypefaceStore->TypefaceSupport(support,index);
			support_name.Copy(support.iTypeface.iName);
			}
		else if(type == 1)
			{
			iFbsTypefaceStore->TypefaceSupport(support,index);
			if(support.iIsScalable)
				{
				count_fbs++;
				support_name.Copy(support.iTypeface.iName);
				}
			}
		if (!typeface.iName.CompareF(support.iTypeface.iName))
			{
			typeface_name.Copy(support.iTypeface.iName);
			found = 1;
			}
		}

	if (found)
		{
		if (loop <= iFbsTypefaceCount)
			{
			TFontSpec fontspec(aFontSpec);
			fontspec.iTypeface=support.iTypeface;
			return iFbsTypefaceStore->GetNearestFontToDesignHeightInTwips(aFont, fontspec);
			}
		else if (loop <= count)
			{
			TFontSpec fontspec(aFontSpec);
			fontspec.iTypeface=support.iTypeface;
			return iTypefaceStore->GetNearestFontToDesignHeightInTwips(aFont, fontspec);
			}
		}
	else 
		{
		if (!typeface.IsSymbol())
			{	// To match first non-symbol, serif, proportional
			TypefaceSupport(lsupport, 0);
			while ((listindex < count) && ((lsupport.iTypeface.IsSymbol() ||
				(typeface.IsProportional() != lsupport.iTypeface.IsProportional()) ||
				(typeface.IsSerif() != lsupport.iTypeface.IsSerif()))))
				{
				TypefaceSupport(lsupport, listindex);
				aname.Copy(lsupport.iTypeface.iName);
				listindex++;
				}
			if (listindex == count)
				{	// try to match first non-symbol.proportional
				listindex = 0;
				do
					{
					TypefaceSupport(lsupport, listindex);
					aname.Copy(lsupport.iTypeface.iName);
					listindex++;
					}
				while ((listindex < count) && (lsupport.iTypeface.IsSymbol() ||
					(typeface.IsProportional() != lsupport.iTypeface.IsProportional())));
				}
			if (listindex == count)
				{	// try to match first non-symbol
				listindex = 0;
				do
					{
					TypefaceSupport(lsupport, listindex);
					aname.Copy(lsupport.iTypeface.iName);
					listindex++;
					}
				while ((listindex < count) && lsupport.iTypeface.IsSymbol());
				}
			}
		else
			{	// try to match first symbol typeface
			listindex = 0;
			TypefaceSupport(lsupport, 0);

			while ((listindex < count) && !lsupport.iTypeface.IsSymbol())
				{
				TypefaceSupport(lsupport, listindex);
				aname.Copy(lsupport.iTypeface.iName);
				listindex++;
				}
			}
		if (listindex == count)
			{
			listindex = 0;
			TypefaceSupport(lsupport, listindex);
			}
		}
		
	if (listindex <= iFbsTypefaceCount)
		{
		TFontSpec fontspec(aFontSpec);
		fontspec.iTypeface = lsupport.iTypeface;
		TBuf<KMaxTypefaceNameLength> fontspec_name;
		fontspec_name.Copy(fontspec.iTypeface.iName);
		return iFbsTypefaceStore->GetNearestFontToDesignHeightInTwips(aFont, fontspec);
		}
	else
		{
		TFontSpec fontspec(aFontSpec);
		fontspec.iTypeface = lsupport.iTypeface;
		TBuf<KMaxTypefaceNameLength> fontspec_name;
		fontspec_name.Copy(fontspec.iTypeface.iName);
		return iTypefaceStore->GetNearestFontToDesignHeightInTwips(aFont, fontspec);
		}
	}

EXPORT_C void CFbsDrvDevice::ReleaseFont(CFont* aFont)
	{
	if (aFont)
		{
		if (aFont->TypeUid() != KCFbsFontUid)
			{
			iTypefaceStore->ReleaseFont(aFont);
			}
		else
			{
			iFbsTypefaceStore->ReleaseFont(aFont);
			}
		}
	}

EXPORT_C TInt CFbsDrvDevice::NumTypefaces() const
	{
	return iGenTypefaceFontsList->Count();
	}

/**
@deprecated Interface is deprecated because it is unsafe as it may leave. It is available for backward compatibility reasons only.
@see CFbsDrvDevice::LoadTypeFaceListL
*/

EXPORT_C void CFbsDrvDevice::LoadTypeFaceList()
	{
	// Trap and Ignore the ERROR code as its a non-leaving method.
	TRAP_IGNORE(LoadTypeFaceListL());
	}

/**
New Updated LoadTypeFaceList() method
@publishedAll
@released
*/
EXPORT_C void CFbsDrvDevice::LoadTypeFaceListL()
	{
	iFbsTypefaceCount = 0;
	iPdrTypefaceCount = 0;
	if (iGenTypefaceFontsList)
		iGenTypefaceFontsList->Reset();
	else
		iGenTypefaceFontsList = new (ELeave) CArrayFixFlat<TInt>(1);

	if (iGenTypefaceFontsType)
		iGenTypefaceFontsType->Reset();
	else
		iGenTypefaceFontsType = new (ELeave) CArrayFixFlat<TInt>(1);
		
	TInt loop;
	for(loop = 0; loop < iFbsTypefaceStore->NumTypefaces(); loop++)
		{
		TTypefaceSupport support;
		iFbsTypefaceStore->TypefaceSupport(support, loop);
			{
			if (support.iIsScalable)
				{
				iGenTypefaceFontsList->AppendL(loop);
				iGenTypefaceFontsType->AppendL(1);
				iFbsTypefaceCount++;
				}
			}
		}

	for (loop = 0; loop < iTypefaceStore->NumTypefaces(); loop++)
		{
		TTypefaceSupport support;
		iTypefaceStore->TypefaceSupport(support, loop);

		TBuf<KMaxTypefaceNameLength> name;
		name.Copy(support.iTypeface.iName);

		iGenTypefaceFontsList->AppendL(loop);
		iGenTypefaceFontsType->AppendL(0);
		iPdrTypefaceCount++;
		}
	}

EXPORT_C void CFbsDrvDevice::TypefaceSupport(TTypefaceSupport& aTypefaceSupport, TInt aTypefaceIndex) const
	{
	TInt index = iGenTypefaceFontsList->At(aTypefaceIndex);
	TInt type = iGenTypefaceFontsType->At(aTypefaceIndex);

	if (type == 0)
		iTypefaceStore->TypefaceSupport(aTypefaceSupport, index);
	else
		iFbsTypefaceStore->TypefaceSupport(aTypefaceSupport, index);
	}

EXPORT_C TInt CFbsDrvDevice::FontHeightInTwips(TInt aTypefaceIndex, TInt aHeightIndex) const
	{
	TInt index = iGenTypefaceFontsList->At(aTypefaceIndex);
	TInt type = iGenTypefaceFontsType->At(aTypefaceIndex);

	if (type == 0)
		return iTypefaceStore->FontHeightInTwips(index, aHeightIndex);
	else
		return iFbsTypefaceStore->FontHeightInTwips(index, aHeightIndex);
	}

EXPORT_C TInt CFbsDrvDevice::CreateContext(CGraphicsContext*& aGc)
	{
	__ASSERT_DEBUG(iControl, Panic(EPdrControlDoesNotExist));
	CPdrControl* control = (CPdrControl*)iControl;
	return control->CreateContext(aGc);
	}

EXPORT_C void CFbsDrvDevice::CreateControlL(CPrinterPort* aPrinterPort)
	{
	__ASSERT_ALWAYS(aPrinterPort, Panic(EPdrRequiresPrinterPort));
	__ASSERT_ALWAYS(!iControl, Panic(EPdrControlAlreadyExists));
	__ASSERT_DEBUG(iCurrentPageSpecInTwips.iPortraitPageSize.iWidth && iCurrentPageSpecInTwips.iPortraitPageSize.iHeight, Panic(EPdrPageSpecNotSet));
	iControl = CFbsDrvControl::NewL(this, aPrinterPort, *iStore, iModelInfo->iResourcesStreamId);
	}

/**
@deprecated Interface is deprecated because it is unsafe as it may leave. It is available for backward compatibility reasons only.
@see CFbsDrvDevice::SetModelL
*/
EXPORT_C TInt CFbsDrvDevice::SetModel(const TPrinterModelHeader& aModel, CStreamStore& aStore)
	{
	TInt ret = 0;
	TRAPD(errCode, ret=SetModelL(aModel, aStore));
	if(errCode != KErrNone)
		{
		return errCode;
		}
	return ret;
	}

EXPORT_C TInt CFbsDrvDevice::SetModelL(const TPrinterModelHeader& aModel, CStreamStore& aStore)
	{
	TInt ret = CPdrDevice::SetModel(aModel, aStore);
	if (ret == KErrNone)
		{
		iFbsTypefaceStore = CFbsTypefaceStore::NewL(this);
		LoadTypeFaceListL();
		}
	return ret;
	}

EXPORT_C TSize CFbsDrvDevice::KPixelSizeInTwips() const
	{
	return TSize(iModelInfo->iKPixelWidthInTwips, iModelInfo->iKPixelHeightInTwips);
	}

EXPORT_C void CFbsDrvDevice::Reserved_1()
	{
	}

EXPORT_C CFbsDrvControl* CFbsDrvControl::NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	CFbsDrvControl* control = new(ELeave) CFbsDrvControl(aPdrDevice, aPrinterPort);
	CleanupStack::PushL(control);
	control->ConstructL(aStore, aResourcesStreamId);
	CleanupStack::Pop();
	return control;
	}

EXPORT_C CFbsDrvControl::~CFbsDrvControl()
	{
	delete iScanLine;
	delete iCompressedScanLine;
	}

EXPORT_C CFbsDrvControl::CFbsDrvControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort):
	CPdrControl(aPdrDevice, aPrinterPort),
	iScanLine(NULL),
	iCompressedScanLine(NULL)
	{
	__DECLARE_NAME(_S("CFbsDrvControl"));
	}

EXPORT_C void CFbsDrvControl::ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId)
	{
	CPdrControl::ConstructL(aStore, aResourcesStreamId);
	}

EXPORT_C void CFbsDrvControl::SetPageSizeL()
	{
	TCommandString des;
	des.Format(iResources->ResourceString(EPdrSetPageSize), iPdrDevice->CurrentPageSpecInTwips().iPortraitPageSize.iHeight / KTwipsPerInch);
	iPageBuffer->AddBytesL(des);
	}

/**
 @return ETrue if there are non-blank bytes in scanline. 
 */
EXPORT_C TBool CFbsDrvControl::TransformBuffer()
	{	
	TInt i;
	for (i = iScanLine->Length() - 1; (i >= 0) && (iScanLine->Des()[i] == 0xFF); i--)
		{
		}
	TInt length = i + 1;
	iScanLine->Des().SetLength(length);
	TUint8* p = (TUint8*)iScanLine->Des().Ptr();
	TUint8* pEnd = p + length;
	for (; p < pEnd; p++)
		{
		TInt byte1 = *p;
		TInt byte2 = 0;
		for (TInt j = 0; j < 8; j++)
			{
			byte2 = byte2 << 1;
			byte2 |= (byte1 & 1);
			byte1 = byte1 >> 1;
			}
		*p = (TUint8)~byte2; 
		}
	return (length > 0);	// returns ETrue if there are non-blank bytes in scanline
	}

/**
 @return ETrue if the scanline is compressed successfully. 
 */
EXPORT_C TBool CFbsDrvControl::CompressBuffer()
	{
	TInt length1 = iScanLine->Des().Length();
	TInt length2 = 0;
	TUint8* p1 = (TUint8*)iScanLine->Des().Ptr();
	TUint8* p2 = (TUint8*)iCompressedScanLine->Des().Ptr();
	TUint8 repeat;
	for (TInt i = 0; (i < length1) && (length2 < length1); i += repeat + 1)
		{
		TUint8 byte = *(p1++);
		for (repeat = 0; ((i + repeat + 1) < length1) && (byte == *p1) && (repeat < 255);)
			{
			repeat++;
			p1++;
			}
		length2++;
		if (length2 < length1)
			{
			*(p2++) = repeat;
			length2++;
			if (length2 < length1)
				*(p2++) = byte;
			}
		}
	iCompressedScanLine->Des().SetLength(length2);
	return (length2 < length1);   // returns ETrue if the scanline is compressed successfully
	}

/**
 This function is intentionally a dummy. It has to be implemented because
 of an inherited pure virtual but it should always be overload by any class
 that derives from it.
*/
EXPORT_C void CFbsDrvControl::OutputBandL()
	{
	// I should probably put an assert in here.
	if (IsGraphicsBand())
		{
		TRect bandrect = iBandedDevice->BandRect();
		TSize size = bandrect.Size();
		TCommandString des;
		TBool datainband = EFalse;
		TInt numscanlines = size.iHeight;
		if (iBandedDevice->BandingDirection() == EBandingRightToLeft)
			numscanlines = size.iWidth;
		for (TInt i = 0; i < numscanlines; i++)
			{
			TInt x = bandrect.iTl.iX;
			TInt y = bandrect.iTl.iY + i;
			TPtr8 ptr = iScanLine->Des();
			if (iBandedDevice->BandingDirection() == EBandingTopToBottom)
				iBandedDevice->BandBitmap()->GetScanLine(ptr, TPoint(0, i), size.iWidth, iPdrDevice->DisplayMode());
			else
				{
				iBandedDevice->BandBitmap()->GetVerticalScanLine(ptr, numscanlines - (i + 1), iPdrDevice->DisplayMode());
				x = bandrect.iBr.iX - i;
				y = bandrect.iTl.iY;
				}
			if (TransformBuffer() && !datainband)
				{
				MoveToL(TPoint(x, y));
				if (iBandedDevice->BandingDirection() == EBandingLeftToRight)
					des.Format(iResources->ResourceString(EPdrBitmapStart), EFbsPhysicalPageOrientation);
				else
					des.Format(iResources->ResourceString(EPdrBitmapStart), EFbsLogicalPageOrientation);
				iPageBuffer->AddBytesL(des);
				datainband = ETrue;
				}
			if (datainband)
				{
				TCommandString buf = iResources->ResourceString(EPdrScanLine);
				if (CompressBuffer())
					{
					des.Format(buf, EFbsRunLength, iCompressedScanLine->Des().Length());
					iPageBuffer->AddBytesL(des);
					iPageBuffer->AddBytesL(iCompressedScanLine->Des());
					}
				else
					{
					des.Format(buf, EFbsNone, iScanLine->Des().Length());
					iPageBuffer->AddBytesL(des);
					iPageBuffer->AddBytesL(iScanLine->Des());
					}
				}
			}
		iPageBuffer->AddBytesL(iResources->ResourceString(EPdrBitmapEnd));

		iPosition.iX = iPdrDevice->OffsetInPixels().iX;
		TInt numentries = iPageText->NumEntries();
		if(numentries)
			{
			CPageTextEntry* entry;
			for(TInt y = bandrect.iTl.iY; y <= bandrect.iBr.iY; y++)
				{
				for(TInt index = 0; (index < numentries); index++)
					{
					entry = (*iPageText)[index];
					TPoint drawPos = entry->iDrawPos;
					if(drawPos.iY == y)
						OutputTextL(drawPos, entry->iTextWidthInPixels, *(entry->iTextFormat), *(entry->iText));										 //!!
					}
				}
			}
		}
	}

EXPORT_C void CFbsDrvControl::Reserved_1()
	{
	}
