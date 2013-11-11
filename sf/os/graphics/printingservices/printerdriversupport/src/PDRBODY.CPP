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

#include "PDRBODY.H"
#include "PDRSTD.H"

TPdrResource::TPdrResource():
	iId(0),
	iString()
	{
	}

void TPdrResource::InternalizeL(RReadStream& aStream)
	{
	iId = aStream.ReadUint8L();
	aStream >> iString;
	}

CPdrTranslation::CPdrTranslation():
	iFrom(0),
	iTo(NULL)
	{
	__DECLARE_NAME(_S("CPdrTranslation"));
	}

CPdrTranslation::~CPdrTranslation()
	{
	delete iTo;
	}

void CPdrTranslation::InternalizeL(RReadStream& aStream)
	{
	iFrom = aStream.ReadUint16L();
	iTo=HBufC8::NewL(aStream,KMaxCommandStringMaxLength);
	}

CPdrTranslates::CPdrTranslates():
	iStreamId(KNullStreamId),
	iNumTranslations(0),
	iTranslationList(NULL)
	{
	__DECLARE_NAME(_S("CPdrTranslates"));
	}

void CPdrTranslates::InternalizeL(RReadStream& aStream)
	{
	TInt size=aStream.ReadInt32L();
	iTranslationList = new(ELeave) CPdrTranslation*[size];
	for (TInt i=0; i<size; i++)
		{
		iTranslationList[i]=new(ELeave) CPdrTranslation;
		iNumTranslations++;
		iTranslationList[i]->InternalizeL(aStream);
		}
	}

CPdrTranslates::~CPdrTranslates()
	{
	for (; iNumTranslations>0; iNumTranslations--)
		delete iTranslationList[iNumTranslations-1];
	delete[] iTranslationList;
	}

HBufC8* CPdrTranslates::TranslateStringL(const TDesC& aString) const 
	{
	CPdrTranslation** pEnd=iTranslationList+iNumTranslations;
	TInt length1=aString.Length(),length2=length1;
	if (iNumTranslations)
		{
		for (TInt i=0; i<length1; i++)
			{
			for (CPdrTranslation** p=iTranslationList; p<pEnd; p++)
				if (aString[i]==(*p)->iFrom)
					length2+=(*p)->iTo->Des().Length()-1;
			}
		}
	HBufC8* string2=HBufC8::NewL(length2);
	string2->Des().Copy(aString);	
	if (iNumTranslations)
		{
		CleanupStack::PushL(string2);	
		HBufC8* string1=HBufC8::NewL(length1);
		string1->Des().Copy(aString);	
		TInt j=0;
		for (TInt i=0; i<length1; i++)
			{
			for (CPdrTranslation** p=iTranslationList; p<pEnd; p++)
				{
				if (aString[i]==(*p)->iFrom)
					{
					if ((*p)->iTo->Des().Length()==1)
						{
						string2->Des()[j]=(*p)->iTo->Des()[0];
						}
					else
						{
						string2->Des().SetLength(j);
						string2->Des().Append((*p)->iTo->Des());
						j+=(*p)->iTo->Des().Length()-1;
						if ((i+1)<length1)
							{
							string2->Des().SetLength(j+1);
							string2->Des().Append(string1->Des().Mid(i+1));
							}
						}
					}
				}
			j++;
			}
		delete string1;
		CleanupStack::Pop();
		}
	return string2;
	}

CWidthsCodeSection::CWidthsCodeSection():
	iNumWidths(0),
	iWidthList(NULL)
	{
	__DECLARE_NAME(_S("CWidthsCodeSection"));
	}

void CWidthsCodeSection::InternalizeL(RReadStream& aStream)
	{
	iCodeSection.iStart = aStream.ReadUint16L();
	iCodeSection.iEnd = aStream.ReadUint16L();
	iNumWidths = aStream.ReadInt32L();
	iWidthList = new(ELeave) TUint16[iNumWidths];
	TUint16* pEnd = iWidthList+iNumWidths;
	for (TUint16* p=iWidthList; p<pEnd; p++)
		*p = aStream.ReadUint16L();
	}

CWidthsCodeSection::~CWidthsCodeSection()
	{
	delete[] iWidthList;
	iNumWidths=0;
	iWidthList=NULL;
	}

CFontInfo::CFontInfo(TStreamId aStreamId):
	iStreamId(aStreamId),
	iAscentInPixels(0),
	iMaxCharWidthInPixels(0),
	iMaxNormalCharWidthInPixels(0),
	iNumCodeSections(0),
	iCodeSectionList(NULL)
	{
	__DECLARE_NAME(_S("CFontInfo"));
	}

void CFontInfo::InternalizeL(RReadStream &aStream)
	{
	iAscentInPixels = aStream.ReadUint16L();
	iMaxCharWidthInPixels = aStream.ReadUint16L();
	iMaxNormalCharWidthInPixels = aStream.ReadUint16L();
	TInt size = aStream.ReadInt32L();
	iCodeSectionList = new(ELeave) CWidthsCodeSection*[size];
	for (TInt i=0; i<size; i++)
		{
		iCodeSectionList[i]=new(ELeave) CWidthsCodeSection;
		iNumCodeSections++;
		iCodeSectionList[i]->InternalizeL(aStream);
		}
	}

CFontInfo::~CFontInfo()
	{
	for (; iNumCodeSections>0; iNumCodeSections--)
		delete iCodeSectionList[iNumCodeSections-1];
	delete[] iCodeSectionList;
	}

TInt CFontInfo::CharWidthInPixels(TChar aChar) const
	{
	TInt width=0,code=TUint(aChar);
	for (TInt i=0; i<iNumCodeSections; i++)
		{
		CWidthsCodeSection* p=iCodeSectionList[i];
		if ((code>=p->iCodeSection.iStart) && (code<=p->iCodeSection.iEnd))
			{
			if (p->iNumWidths==1)
				width=p->iWidthList[0];
			else 
				width = *((p->iWidthList)+(code-p->iCodeSection.iStart));
			}
		}
	return width;
	}

TInt CFontInfo::NumCodeSections() const
	{
	return iNumCodeSections;
	}

TCodeSection CFontInfo::CodeSection(TInt anIndex) const
	{
	return iCodeSectionList[anIndex]->iCodeSection;
	}

TPdrStyle::TPdrStyle():
	iIsAvailable(EFalse),
	iFontInfoStreamId(KNullStreamId)
	{
	}

void TPdrStyle::InternalizeL(RReadStream &aStream)
	{
	iIsAvailable = aStream.ReadUint8L();
	aStream >> iFontInfoStreamId;
	}	

TPdrFontHeight::TPdrFontHeight():
	iCommandString(),
	iHeightInTwips(0),
	iWidthScale(1),
	iStyle() 
	{
	}

void TPdrFontHeight::InternalizeL(RReadStream& aStream)
	{
	aStream >> iCommandString;
	iHeightInTwips = aStream.ReadInt32L();
	iWidthScale = aStream.ReadInt32L();
	for (TInt style=EStyleNormal; style<(EStyleBoldItalic+1); style++)
		iStyle[style].InternalizeL(aStream);
	}

TPdrScalableFontHeight::TPdrScalableFontHeight():
	iCommandString(),
	iHeightMinInTwips(0),
	iHeightMaxInTwips(0),
	iHeightDeltaInTwips(0),
	iStyle() 
	{
	}

void TPdrScalableFontHeight::InternalizeL(RReadStream& aStream)
	{
	aStream >> iCommandString;
	iHeightMinInTwips = aStream.ReadInt32L();
	iHeightMaxInTwips = aStream.ReadInt32L();
	iHeightDeltaInTwips = aStream.ReadInt32L();
	for (TInt style=EStyleNormal; style<(EStyleBoldItalic+1); style++)
		iStyle[style].InternalizeL(aStream);
	}

CTypefaceFonts::CTypefaceFonts():
	iTypeface(),
	iNumFontHeights(0),
	iFontHeightList(NULL),
	iScalableFontHeight(NULL),
	iTranslates()
	{
	__DECLARE_NAME(_S("CTypefaceFonts"));
	}

void CTypefaceFonts::InternalizeL(RReadStream& aStream)
	{
	iTypeface.InternalizeL(aStream);
	TInt isscalable = aStream.ReadInt8L();
 	if (!isscalable)
		{
		iNumFontHeights = aStream.ReadInt32L();
		iFontHeightList = new(ELeave) TPdrFontHeight[iNumFontHeights];
		TPdrFontHeight *pStart=iFontHeightList, *pEnd=pStart+iNumFontHeights;
		for (TPdrFontHeight *p=pStart; p<pEnd; p++)
			p->InternalizeL(aStream);
		}
	else
		{
		iScalableFontHeight = new(ELeave) TPdrScalableFontHeight;
		iScalableFontHeight->InternalizeL(aStream);
		}
	aStream >> iTranslates;
	}

CTypefaceFonts::~CTypefaceFonts()
	{
	if (!IsScalable())
		{
		iNumFontHeights=0;
		delete[] iFontHeightList;
		iFontHeightList=NULL;
		}
	else
		{
		delete iScalableFontHeight;
		iScalableFontHeight=NULL;
		}
	}

TInt CTypefaceFonts::IsScalable() const
	{
	return !iNumFontHeights;
	}

TInt CTypefaceFonts::NumFontHeights() const
	{
	TInt num;
	if (!IsScalable())
		num = iNumFontHeights;
	else
		num = ((iScalableFontHeight->iHeightMaxInTwips-iScalableFontHeight->iHeightMinInTwips)/iScalableFontHeight->iHeightDeltaInTwips) + 1;
	return num;
	}

TInt CTypefaceFonts::FontHeightInTwips(TInt aHeightIndex) const
	{
	TInt height=0;
	__ASSERT_DEBUG((aHeightIndex>=0) && (aHeightIndex<NumFontHeights()),Panic(EPdrHeightIndexOutOfRange));
	if (!IsScalable())
		height = iFontHeightList[aHeightIndex].iHeightInTwips;
	else
		height = iScalableFontHeight->iHeightMinInTwips + (iScalableFontHeight->iHeightDeltaInTwips*aHeightIndex);
	return height;
	}

TInt CTypefaceFonts::FontInfoHeightInTwips(TInt aHeightIndex) const
	{
	TInt height=0;
	__ASSERT_DEBUG((aHeightIndex>=0) && (aHeightIndex<NumFontHeights()),Panic(EPdrHeightIndexOutOfRange));
	if (!IsScalable())
		height = iFontHeightList[aHeightIndex].iHeightInTwips/iFontHeightList[aHeightIndex].iWidthScale;
	else
		height = KScalableWidthTableHeightInTwips;
	return height;
	}

void CTypefaceFonts::CommandString(TDes8& aDes,TInt aHeightIndex) const
	{
	__ASSERT_DEBUG((aHeightIndex>=0) && (aHeightIndex<NumFontHeights()),Panic(EPdrHeightIndexOutOfRange));
	if (!IsScalable())
		aDes=iFontHeightList[aHeightIndex].iCommandString;
	else
		{
		TInt heightinpoints=FontHeightInTwips(aHeightIndex)/KTwipsPerPoint;
		aDes.Format(iScalableFontHeight->iCommandString,heightinpoints);
		}
	}

TPdrStyle* CTypefaceFonts::Style(TInt aHeightIndex,TFontStyle& aFontStyle) const
	{
	TPdrStyle* style=NULL;
	TStyleIndex index=StyleIndex(aFontStyle);
	__ASSERT_DEBUG((aHeightIndex>=0) && (aHeightIndex<NumFontHeights()),Panic(EPdrHeightIndexOutOfRange));
	if (!IsScalable())
		style = iFontHeightList[aHeightIndex].iStyle+index;
	else
		style = iScalableFontHeight->iStyle+index;
	return style;
	}

TTypeface CTypefaceFonts::Typeface()
	{
	return iTypeface;
	}

TStyleIndex CTypefaceFonts::StyleIndex(TFontStyle& aFontStyle) const
	{
	TStyleIndex index;
	if (aFontStyle.StrokeWeight()==EStrokeWeightNormal)
		{
		if (aFontStyle.Posture()==EPostureUpright)
			index=EStyleNormal;
		else
			index=EStyleItalic;
		}
	else
		{
		if (aFontStyle.Posture()==EPostureUpright)
			index=EStyleBold;
		else
			index=EStyleBoldItalic;
		}
	return index;
	}
