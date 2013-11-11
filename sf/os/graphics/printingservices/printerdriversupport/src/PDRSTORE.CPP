// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <pdrstore.h>
#include <banddev.h>
#include "PDRBODY.H"
#include "PDRSTD.H"
#include "pdrtext.h"

EXPORT_C CPdrResources::CPdrResources():
	iNumResources(0),
	iResourceList(NULL)
	{
	__DECLARE_NAME(_S("CPdrResources"));
	}

EXPORT_C void CPdrResources::RestoreL(CStreamStore& aStore, TStreamId aStreamId)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore, aStreamId);
	iNumResources = stream.ReadInt32L();
	iResourceList = new(ELeave) TPdrResource[iNumResources];
	for (TInt i = 0; i < iNumResources; i++)
		iResourceList[i].InternalizeL(stream);
	CleanupStack::PopAndDestroy();
	}

EXPORT_C CPdrResources::~CPdrResources()
	{
	delete[] iResourceList;
	iResourceList = NULL;
	iNumResources = 0;
	}

EXPORT_C TPtrC8 CPdrResources::ResourceString(TInt anId) const
	{
	TPtrC8 ptr;
	TPdrResource* pEnd = iResourceList + iNumResources;
	TPdrResource* p ;
	for( p = iResourceList; (p < pEnd) && (p->iId != anId); p++)
		{
		}
	if (p < pEnd)
		ptr.Set(p->iString);
	return ptr;
	}

CInfoFont::CInfoFont(TInt aBaselineOffsetInPixels, const TFontSpec& aFontSpecInTwips, TInt aFontInfoHeightInTwips, TInt aHeightInPixels, CPdrTranslates* aTranslates, const TDesC8& aCommandString, CPdrDevice* aPdrDevice):
	CFont(),
	iCommandString(aCommandString),	
	iBaselineOffsetInPixels(aBaselineOffsetInPixels),
	iFontSpecInTwips(aFontSpecInTwips),
	iFontInfoHeightInTwips(aFontInfoHeightInTwips),
	iHeightInPixels(aHeightInPixels),
	iFontInfo(NULL),
	iTranslates(aTranslates),
	iPdrDevice(aPdrDevice),
	iRealFont(NULL)
	{
	CreateBandedFontIfRequired();
	}

CInfoFont::~CInfoFont()
	{
	if (iRealFont)
		if (iPdrDevice->iControl)
			((CPdrControl*)(iPdrDevice->iControl))->BandedDevice()->ReleaseFont(iRealFont);
	}

void CInfoFont::CreateBandedFontIfRequired()
	{
	if (!iRealFont)
		{
		if (iPdrDevice->iControl)
			{
			if (((CPdrControl*)(iPdrDevice->iControl))->BandedDevice())
				((CPdrControl*)(iPdrDevice->iControl))->BandedDevice()->GetNearestFontToDesignHeightInTwips(iRealFont, iFontSpecInTwips);
			}
		}
	}

EXPORT_C TUid CInfoFont::DoTypeUid() const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return TUid::Uid(KCInfoFontUidVal);
	}

EXPORT_C TInt CInfoFont::DoHeightInPixels() const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return iHeightInPixels;
	}

EXPORT_C TInt CInfoFont::DoAscentInPixels() const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return Height(iFontInfo->iAscentInPixels);
	}

EXPORT_C TInt CInfoFont::DoCharWidthInPixels(TChar aChar) const
	{
	TInt width;
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	if (RepertoireContains(aChar))
		{
		width = iFontInfo->CharWidthInPixels(TUint(aChar));
		width = Width(width);
		}
	else
		{
		if (iRealFont)
			width = iRealFont->CharWidthInPixels(TUint(aChar));
		else
			width = 0;
		}
	return width;
	}

EXPORT_C TInt CInfoFont::DoTextWidthInPixels(const TDesC &aText) const
	{
	TMeasureTextOutput output;
	TInt advance_width = MeasureText(aText,NULL,&output);
	return Max(advance_width,output.iBounds.Width());
	}

EXPORT_C TInt CInfoFont::DoBaselineOffsetInPixels() const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return iBaselineOffsetInPixels;
	}

EXPORT_C TInt CInfoFont::DoTextCount(const TDesC &aText, TInt aWidthInPixels) const
	{
	TInt count = 0;
	TInt width = 0;
	TInt length = aText.Length();
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	for (count = 0; (count < length) && ((width += MeasureText(aText.Mid(count, 1))) < aWidthInPixels); )
		{
		count++;
		}
	return count;
	}

EXPORT_C TInt CInfoFont::DoTextCount(const TDesC &aText, TInt aWidthInPixels, TInt &aExcessWidthInPixels) const
	{
	TInt count = TextCount(aText, aWidthInPixels);
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	aExcessWidthInPixels = aWidthInPixels - MeasureText(aText.Left(count));
	return count;
	}

EXPORT_C TInt CInfoFont::DoMaxCharWidthInPixels() const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	TInt width = Width(iFontInfo->iMaxCharWidthInPixels);
	if (iRealFont)
		return Max(iRealFont->MaxCharWidthInPixels(),width);
	return width;
	}

EXPORT_C TInt CInfoFont::DoMaxNormalCharWidthInPixels() const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	TInt width = Width(iFontInfo->iMaxNormalCharWidthInPixels);
	if (iRealFont)
		return Max(iRealFont->MaxNormalCharWidthInPixels(),width);
	return width;
	}

EXPORT_C TFontSpec CInfoFont::DoFontSpecInTwips() const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return iFontSpecInTwips;	
	}

EXPORT_C HBufC8* CInfoFont::TranslateStringL(const TDesC& aString) const  
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return iTranslates->TranslateStringL(aString);
	}

EXPORT_C TPtrC8 CInfoFont::CommandString() const
	{
	TPtrC8 ptr;
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	ptr.Set(iCommandString);
	return ptr;
	}

EXPORT_C TBool CInfoFont::RepertoireContains(TChar aChar) const
	{
	CFontInfo* fontInfo = FontInfo();
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	for (TInt i = 0; i < fontInfo->NumCodeSections(); i++)
		{
		if (((TInt)(TUint)aChar >= fontInfo->CodeSection(i).iStart) && ((TInt)(TUint)aChar<= fontInfo->CodeSection(i).iEnd))
			return ETrue;
		}
	return EFalse;
	}

EXPORT_C TBool CInfoFont::AllCharsInFontRepertoire(const TDesC& aString, TInt& aFirstCharNotInRepertoire, TInt& aLength) const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	for (aFirstCharNotInRepertoire = 0; aFirstCharNotInRepertoire < aString.Length(); aFirstCharNotInRepertoire++)
		if (!(RepertoireContains(aString[aFirstCharNotInRepertoire])))
			{
			if (aFirstCharNotInRepertoire == 0)
				{	// Work out length
				for (aLength = aFirstCharNotInRepertoire; aLength < aString.Length(); aLength++)
					if ((RepertoireContains(aString[aLength])))
						break;
				}
			return EFalse;
			}
	return ETrue;
	}

CFont* CInfoFont::RealFont() const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return iRealFont;
	}

TInt CInfoFont::Width(TInt aNum) const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return (aNum * iFontSpecInTwips.iHeight + (iFontInfoHeightInTwips / 2)) / iFontInfoHeightInTwips;
	}

TInt CInfoFont::Height(TInt aNum) const
	{
	CONST_CAST(CInfoFont*,this)->CreateBandedFontIfRequired();
	return (aNum * iFontSpecInTwips.iHeight + (iFontInfoHeightInTwips / 2)) / iFontInfoHeightInTwips;
	}

EXPORT_C void TTypefaceFontsEntry::InternalizeL(RReadStream& aStream)
	{
	aStream >> iStreamId;
	iNotInPortrait = aStream.ReadUint8L();
	iNotInLandscape = aStream.ReadUint8L();
	}

EXPORT_C CPdrModelInfo::CPdrModelInfo():
	iFlags(0),
	iKPixelWidthInTwips(0),
	iKPixelHeightInTwips(0),
	iPortraitOffsetInPixels(),
	iLandscapeOffsetInPixels(),
//	iMinMarginsInPixels(),
	iDisplayMode(EGray2),
	iNumTypefaceFonts(0),
	iTypefaceFontsEntryList(NULL),
	iResourcesStreamId(KNullStreamId),
	iSpareStreamId(KNullStreamId)
	{
	__DECLARE_NAME(_S("CPdrModelInfo"));
	}

EXPORT_C CPdrModelInfo::~CPdrModelInfo()
	{
	delete[] iTypefaceFontsEntryList;
	}

EXPORT_C void CPdrModelInfo::InternalizeL(RReadStream& aStream)
	{
	TInt pdrtranversion = aStream.ReadInt32L();
	if (pdrtranversion < KPdrtranVersion)
		User::Leave(KErrNotSupported);
	iFlags = aStream.ReadInt32L();
	iKPixelWidthInTwips = aStream.ReadInt32L();
	iKPixelHeightInTwips = aStream.ReadInt32L();
	iPortraitOffsetInPixels.iX = aStream.ReadInt32L();
	iPortraitOffsetInPixels.iY = aStream.ReadInt32L();
	iLandscapeOffsetInPixels.iX = aStream.ReadInt32L();
	iLandscapeOffsetInPixels.iY = aStream.ReadInt32L();
	iMinMarginsInPixels.InternalizeL(aStream);
	iDisplayMode = (TDisplayMode)aStream.ReadInt32L();
	iNumTypefaceFonts = aStream.ReadInt32L();
	iTypefaceFontsEntryList = new(ELeave) TTypefaceFontsEntry[iNumTypefaceFonts];
	TTypefaceFontsEntry* pEnd = iTypefaceFontsEntryList+iNumTypefaceFonts;
	for(TTypefaceFontsEntry* p = iTypefaceFontsEntryList; p < pEnd; p++)
		p->InternalizeL(aStream);
	aStream >> iResourcesStreamId; 
	aStream >> iSpareStreamId; 
	}

CPdrTypefaceStore::CPdrTypefaceStore(CStreamStore& aStore, TInt aKPixelHeightInTwips, CPdrDevice* aPdrDevice):
	iPdrDevice(aPdrDevice),
	iStore(&aStore),
	iKPixelHeightInTwips(aKPixelHeightInTwips)
	{
	}

void CPdrTypefaceStore::ConstructL(TInt aNumTypefaceFonts, TTypefaceFontsEntry* aTypefaceFontsEntryList, TPageSpec::TPageOrientation aPageOrientation)
	{
	CTypefaceStore::ConstructL();
	iTranslatesList = new(ELeave) CArrayPtrFlat<CPdrTranslates>(8);
	iTypefaceFontsList = new(ELeave) CArrayPtrFlat<CTypefaceFonts>(8);
	iPortraitTypefaceFontsList = new(ELeave) CArrayPtrFlat<CTypefaceFonts>(8);
	iLandscapeTypefaceFontsList = new(ELeave) CArrayPtrFlat<CTypefaceFonts>(8);
	iFontInfoList = new(ELeave) CArrayPtrFlat<CFontInfo>(8);
	for (TInt i = 0; i < aNumTypefaceFonts; i++)
		{
		CTypefaceFonts* typefacefonts = new(ELeave) CTypefaceFonts;
		CleanupStack::PushL(typefacefonts);
		RStoreReadStream stream;
		stream.OpenLC(*iStore, aTypefaceFontsEntryList[i].iStreamId);
		typefacefonts->InternalizeL(stream);
		CleanupStack::PopAndDestroy();
		iTypefaceFontsList->AppendL(typefacefonts);
		CleanupStack::Pop();
		typefacefonts->iTranslates=TranslatesL(typefacefonts->iTranslates.AsId());
		if (!aTypefaceFontsEntryList[i].iNotInPortrait)
			iPortraitTypefaceFontsList->AppendL(typefacefonts);
		if (!aTypefaceFontsEntryList[i].iNotInLandscape)
			iLandscapeTypefaceFontsList->AppendL(typefacefonts);
		}
	SetPageOrientation(aPageOrientation);
	}

EXPORT_C CPdrTypefaceStore* CPdrTypefaceStore::NewL(CStreamStore& aStore, TInt aNumTypefacesFonts, TTypefaceFontsEntry* aTypefaceFontsEntryList, TPageSpec::TPageOrientation aPageOrientation, TInt aKPixelHeightInTwips, CPdrDevice* aPdrDevice)
	{
	CPdrTypefaceStore* typefacestore = new(ELeave) CPdrTypefaceStore(aStore, aKPixelHeightInTwips, aPdrDevice);
	CleanupStack::PushL(typefacestore);
	typefacestore->ConstructL(aNumTypefacesFonts, aTypefaceFontsEntryList, aPageOrientation);
	CleanupStack::Pop();
	return typefacestore;
	}

EXPORT_C CPdrTypefaceStore::~CPdrTypefaceStore()
	{
	if (iTranslatesList)
		iTranslatesList->ResetAndDestroy();
	delete iTranslatesList;
	if (iTypefaceFontsList)
		iTypefaceFontsList->ResetAndDestroy();
	delete iTypefaceFontsList;
	delete iPortraitTypefaceFontsList;
	delete iLandscapeTypefaceFontsList;
	if (iFontInfoList)
		iFontInfoList->ResetAndDestroy();
	delete iFontInfoList;
	}

/**
@internalTechnology
*/
EXPORT_C TInt CPdrTypefaceStore::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/**
@internalTechnology
*/
EXPORT_C TInt CPdrTypefaceStore::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	aFont = NULL;
	const TInt count = iCurrentTypefaceFontsList->Count();
	if (count)
		{
		TInt index = 0;
		TTypeface typeface = aFontSpec.iTypeface;
		for (index = 0; (index < count) && typeface.iName.CompareF((*iCurrentTypefaceFontsList)[index]->Typeface().iName); index++)
			{ // tries matching typeface name
			}
		if (index == count)
			{
			if (!typeface.IsSymbol())
				{
				for (index = 0; (index < count) && (((*iCurrentTypefaceFontsList)[index]->Typeface().IsSymbol()) ||
													(typeface.IsProportional() != (*iCurrentTypefaceFontsList)[index]->Typeface().IsProportional()) || 
													(typeface.IsSerif() != (*iCurrentTypefaceFontsList)[index]->Typeface().IsSerif())); index++)
					{ // tries matching typeface flags
					}
				if (index == count)
					for (index = 0; (index < count) && (((*iCurrentTypefaceFontsList)[index]->Typeface().IsSymbol()) ||
														(typeface.IsProportional() != (*iCurrentTypefaceFontsList)[index]->Typeface().IsProportional())); index++)
						{ // tries matching typeface flag
						}
				if (index == count)
					for (index = 0; (index < count) && ((*iCurrentTypefaceFontsList)[index]->Typeface().IsSymbol()); index++)
						{ // tries matching typeface flag
						}
				}
			else
				{
				for (index = 0; (index < count) && (!(*iCurrentTypefaceFontsList)[index]->Typeface().IsSymbol()); index++)
					{ // finds first symbol typeface
					}
				}
			}
		if (index == count)
			index = 0;
		CTypefaceFonts* typefacefonts = (*iCurrentTypefaceFontsList)[index];
		if (typefacefonts->NumFontHeights())
			{
			TFontSpec fontspec(aFontSpec);
			fontspec.iTypeface = typefacefonts->Typeface();
			TInt i = GetNearestFontHeightIndex(index, aFontSpec.iHeight);
			fontspec.iHeight = typefacefonts->FontHeightInTwips(i);
			if (fontspec.iFontStyle.PrintPosition() != EPrintPosNormal)
				{
				i = GetNearestFontHeightIndex(index, SuperSubHeight(fontspec.iHeight, fontspec.iFontStyle.PrintPosition()));
				}
			fontspec.iFontStyle = GetNearestFontStyle(index, i, fontspec.iFontStyle);
			TInt heightintwips = typefacefonts->FontHeightInTwips(i);
			TInt height = VerticalTwipsToPixels(heightintwips);
			if (IsFontLoaded(aFont, fontspec, height))
				return KErrNone;
			TInt baselineoffset = BaselineOffset(VerticalTwipsToPixels(fontspec.iHeight), fontspec.iFontStyle.PrintPosition());
			TInt fontinfoheight = ((fontspec.iHeight * typefacefonts->FontInfoHeightInTwips(i) + (heightintwips / 2))) / heightintwips;
			CPdrTranslates* translates = typefacefonts->iTranslates;
			TCommandString commandstring;
			typefacefonts->CommandString(commandstring, i);
			TStreamId fontinfostreamid = typefacefonts->Style(i, fontspec.iFontStyle)->iFontInfoStreamId;
			TRAPD(ret, aFont = NewFontL(baselineoffset, fontspec, fontinfoheight, height, translates, commandstring, fontinfostreamid));
			return ret;
			}
		}
	return KErrNotFound;
	}

/**
@internalTechnology
*/
EXPORT_C TInt CPdrTypefaceStore::GetNearestFontToMaxHeightInTwips(CFont*& /*aFont*/, const TFontSpec& /*aFontSpec*/, TInt /* aMaxHeight */)
	{
	return KErrNotSupported;
	}

EXPORT_C TInt CPdrTypefaceStore::NumTypefaces() const
	{
	return iCurrentTypefaceFontsList->Count();
	}

EXPORT_C TInt CPdrTypefaceStore::FontHeightInTwips(TInt aTypefaceIndex, TInt aHeightIndex) const
	{
	TInt height = 0;
	__ASSERT_DEBUG((aTypefaceIndex >= 0) && (aTypefaceIndex < NumTypefaces()), Panic(EPdrHeightIndexOutOfRange));
	CTypefaceFonts* typefacefonts = (*iCurrentTypefaceFontsList)[aTypefaceIndex];
	height = typefacefonts->FontHeightInTwips(aHeightIndex);
	return height;
	}

EXPORT_C void CPdrTypefaceStore::TypefaceSupport(TTypefaceSupport &aTypefaceSupport, TInt aTypefaceIndex) const
	{
	__ASSERT_DEBUG((aTypefaceIndex >= 0) && (aTypefaceIndex < NumTypefaces()), Panic(EPdrHeightIndexOutOfRange));
	CTypefaceFonts* typefacefonts = (*iCurrentTypefaceFontsList)[aTypefaceIndex];
	aTypefaceSupport.iTypeface = typefacefonts->Typeface();
	aTypefaceSupport.iIsScalable = typefacefonts->IsScalable();
	aTypefaceSupport.iNumHeights = typefacefonts->NumFontHeights();
	aTypefaceSupport.iMinHeightInTwips = FontHeightInTwips(aTypefaceIndex, 0);  // Font heights must be in ascending order
	aTypefaceSupport.iMaxHeightInTwips = FontHeightInTwips(aTypefaceIndex, aTypefaceSupport.iNumHeights - 1);
	}

EXPORT_C void CPdrTypefaceStore::SetPageOrientation(TPageSpec::TPageOrientation aPageOrientation)
	{
	if (aPageOrientation == TPageSpec::EPortrait)
		iCurrentTypefaceFontsList = iPortraitTypefaceFontsList;
	else
		iCurrentTypefaceFontsList = iLandscapeTypefaceFontsList;
	}

CFontInfo* CPdrTypefaceStore::FontInfoL(TStreamId aStreamId) const
	{
	CFontInfo* fontinfo;
	TInt i;
	const TInt count = iFontInfoList->Count();
	for (i = 0; (i < count) && ((*iFontInfoList)[i]->iStreamId != aStreamId); i++)
		{  // Searches for FontInfo with same Id
		}
	if (i < count)  // Found
		fontinfo = (*iFontInfoList)[i];
	else			// Not found
		{
		RStoreReadStream stream;
		fontinfo = new(ELeave) CFontInfo(aStreamId);
		CleanupStack::PushL(fontinfo);
		stream.OpenLC(*iStore, aStreamId);
		fontinfo->InternalizeL(stream);
		CleanupStack::PopAndDestroy();
		iFontInfoList->AppendL(fontinfo);
		CleanupStack::Pop();
		}
	return fontinfo;
	}

CPdrTranslates* CPdrTypefaceStore::TranslatesL(TStreamId aStreamId) const
	{
	CPdrTranslates* translates;
	TInt i;
	const TInt count = iTranslatesList->Count();
	for (i = 0; (i < count) && ((*iTranslatesList)[i]->iStreamId != aStreamId); i++)
		{  // Searches for Translate with same Id
		}
	if (i < count)  // Found
		translates = (*iTranslatesList)[i];
	else			// Not found
		{
		RStoreReadStream stream;
		translates = new(ELeave) CPdrTranslates;
		CleanupStack::PushL(translates);
		translates->iStreamId = aStreamId;
		stream.OpenLC(*iStore, aStreamId);
		translates->InternalizeL(stream);
		CleanupStack::PopAndDestroy();
		iTranslatesList->AppendL(translates);
		CleanupStack::Pop();
		}
	return translates;
	}

TInt CPdrTypefaceStore::GetNearestFontHeightIndex(TInt aTypefaceIndex, TInt aHeightInTwips) const
	{
	CTypefaceFonts* typefacefonts = (*iCurrentTypefaceFontsList)[aTypefaceIndex];
	TInt i;
	TInt size = typefacefonts->NumFontHeights();
	for (i = size - 1; (i > 0) && (aHeightInTwips < typefacefonts->FontHeightInTwips(i)); i--)
		{  //  Finds fontheight less than or equal to fontspec height
		}
	return i;
	}

TFontStyle CPdrTypefaceStore::GetNearestFontStyle(TInt aTypefaceIndex, TInt aHeightIndex, const TFontStyle& aFontStyle) const
	{
	TFontStyle fontstyle(aFontStyle);
	CTypefaceFonts* typefacefonts = (*iCurrentTypefaceFontsList)[aTypefaceIndex];
	while (!typefacefonts->Style(aHeightIndex, fontstyle)->iIsAvailable)
		{  //  finds first available style
		if ((fontstyle.StrokeWeight() == EStrokeWeightBold) && (fontstyle.Posture() == EPostureItalic))
			fontstyle.SetPosture(EPostureUpright);
		else
			{
			fontstyle.SetPosture(EPostureUpright);
			fontstyle.SetStrokeWeight(EStrokeWeightNormal);
			}
		}
	return fontstyle;
	}

TBool CPdrTypefaceStore::IsFontLoaded(CFont*& aFont, const TFontSpec& aFontSpecInTwips, TInt aHeightInPixels) const
/**
@see CFbsTypefaceStore::IsFontLoaded
@see CFontStore::IsFontLoaded
*/
	{
	TInt i = 0;
	const TInt count = iFontAccess->Count();
	for (; i < count &&
			!(aHeightInPixels  == (*iFontAccess)[i].iFont->HeightInPixels() &&
			  aFontSpecInTwips == (*iFontAccess)[i].iFont->FontSpecInTwips()); i++)
		{
		}
	if (i < count)
		{
		aFont = (*iFontAccess)[i].iFont;
		(*iFontAccess)[i].iAccessCount++;
		return ETrue;
		}
	return EFalse;
	}

CInfoFont* CPdrTypefaceStore::NewFontL(TInt aBaselineOffsetInPixels, const TFontSpec& aFontSpecInTwips, TInt aFontInfoHeightInTwips, TInt aHeightInPixels, CPdrTranslates* aTranslates, const TDesC8& aCommandString, TStreamId aFontInfoStreamId)
	{
	CInfoFont* infofont = new(ELeave) CInfoFont(aBaselineOffsetInPixels, aFontSpecInTwips, aFontInfoHeightInTwips, aHeightInPixels, aTranslates, aCommandString, iPdrDevice);
	CleanupStack::PushL(infofont);
	AddFontL(infofont);
	CleanupStack::Pop();
	TRAPD(ret, infofont->iFontInfo = FontInfoL(aFontInfoStreamId));
	if (ret != KErrNone)
		{
		ReleaseFont(infofont);
		User::Leave(ret);
		}
	return infofont;
	}

TInt CPdrTypefaceStore::VerticalTwipsToPixels(TInt aTwipsHeight) const
	{
	return (1000 * aTwipsHeight + (iKPixelHeightInTwips / 2)) / iKPixelHeightInTwips;
	}
