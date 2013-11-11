/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* Header FNTRECRD.CPP
*
*/


#include "FNTRECRD.H"

const int KNumberOfPopularIndices = 128;
const int KNumberOfBitsInByte = 8;
const int KNumberOfBitsInTwoBytes = 16;

BitmapOffset::BitmapOffset(uint16 aBitmapOffset)
 :	iBitmapOffset(aBitmapOffset)
	{}

void BitmapOffset::Externalize(ostream& out)
	{
	out.write((char*) &iBitmapOffset,sizeof(iBitmapOffset));
	}

CharacterMetrics::CharacterMetrics()
 :	iAscentInPixels(0),
	iHeightInPixels(0),
	iLeftAdjustInPixels(0),
	iMoveInPixels(0),
	iRightAdjustInPixels(0)
	{
	}


void CharacterMetrics::Externalize(ostream& out)
	{
	out.write((char*) &iAscentInPixels, sizeof(iAscentInPixels));
	out.write((char*) &iHeightInPixels, sizeof(iHeightInPixels));
	out.write((char*) &iLeftAdjustInPixels, sizeof(iLeftAdjustInPixels));
	out.write((char*) &iMoveInPixels, sizeof(iMoveInPixels));
	out.write((char*) &iRightAdjustInPixels, sizeof(iRightAdjustInPixels));
	}

MetricDistributionMember::~MetricDistributionMember()
	{
	delete iMetric;
	}

MetricDistributionMember::MetricDistributionMember()
 :	iFrequency(0), iMetric(0)
	{}

CharacterMetrics* MetricDistributionMember::Metric() const
	{
	return iMetric;
	}

int MetricDistributionMember::Frequency() const
	{
	return iFrequency;
	}

void MetricDistributionMember::SetFrequency(int aFrequency)
	{
	iFrequency = aFrequency;
	}

void MetricDistributionMember::IncrementFrequency(int aIncrementBy)
	{
	iFrequency += aIncrementBy;
	}

void MetricDistributionMember::SetMetric(CharacterMetrics* aMetric)
	{
	iMetric = aMetric;
	}

void MetricDistributionMember::Externalize(ostream& out)
	{
	iMetric->Externalize(out);
	}

MetricDistribution::~MetricDistribution()
	{
	iCharacterMetricsList.Destroy();
	}

MetricDistribution* MetricDistribution::New()
	{
	return new MetricDistribution;
	}

MetricDistribution::MetricDistribution()
	{}

void MetricDistribution::SortMetricsByFrequency()
	{	// Only need sort the most popular 128, since after this 2 bytes will always be used
	int maxIndex = iCharacterMetricsList.Size();
	if (maxIndex > KNumberOfPopularIndices)
		{
		maxIndex = KNumberOfPopularIndices;
		}
	for(int indexToSet = 0; indexToSet < maxIndex; indexToSet++)
		{
		const CharacterMetrics& mostPopularRemaining = MostPopular(indexToSet);
		SetIndex(mostPopularRemaining, indexToSet);
		}
	}

void MetricDistribution::SetIndex(const CharacterMetrics& aMetrics, int aIndexToSet)
	{
	int currentPos = Index(aMetrics);
	if (currentPos != aIndexToSet)
		{
		MetricDistributionMember* match = iCharacterMetricsList[currentPos];
		MetricDistributionMember* swapPos = iCharacterMetricsList[aIndexToSet];

		CharacterMetrics* tempMet = match->Metric();
		const int tempFreq = match->Frequency();

		match->SetMetric(swapPos->Metric());
		match->SetFrequency(swapPos->Frequency());
		swapPos->SetMetric(tempMet);
		swapPos->SetFrequency(tempFreq);
		}
	}

void MetricDistribution::AddOrIncrementMetric(const CharacterMetrics& aMetrics, int aFrequency)
	{
	boolean match = false;
	const CharacterMetrics* trial = NULL;
	MetricDistributionMember* link = NULL;
	int index;
	int maxIndex = iCharacterMetricsList.Size();
	
	for(index = 0; index < maxIndex && !match; index++)
		{
		link = iCharacterMetricsList[index];
		if (link)
			trial = link->Metric();
		if (trial && (trial->iAscentInPixels == aMetrics.iAscentInPixels)
			&& (trial->iHeightInPixels == aMetrics.iHeightInPixels)
			&& (trial->iLeftAdjustInPixels == aMetrics.iLeftAdjustInPixels)
			&& (trial->iMoveInPixels == aMetrics.iMoveInPixels)
			&& (trial->iRightAdjustInPixels == aMetrics.iRightAdjustInPixels))
			{
			match = true;
			}
		}
	if (match)
		{
		link->IncrementFrequency(aFrequency);
		}
	else
		{
		MetricDistributionMember* newLink = new MetricDistributionMember;
		newLink->IncrementFrequency(aFrequency);
		CharacterMetrics* newMetric = new CharacterMetrics(aMetrics);
		newLink->SetMetric(newMetric);
		iCharacterMetricsList.Add(newLink);
		}
	}

const CharacterMetrics& MetricDistribution::MostPopular(int aStartIndex)
	{
	// finds the most popular metric above index aStartIndex. Allows for a fairly quick sort of the metircs to be done.
	MetricDistributionMember* link = NULL;
	const CharacterMetrics* mostPopular = NULL;
	int frequencyOfMostPopular = 0;
	int frequency = 0;
	int count;
	int total = 0; // for debugging
	const int size = iCharacterMetricsList.Size();
	for (count = aStartIndex; count < size; count++)
		{
		link = iCharacterMetricsList[count];
		frequency = link->Frequency();
		if (frequency>frequencyOfMostPopular)
			{
			mostPopular = link->Metric();
			frequencyOfMostPopular = frequency;
			}
		total += frequency;
		}
	return *mostPopular;
	}

int MetricDistribution::Index(const CharacterMetrics& aMetrics)
	{
	boolean same = false;
	CharacterMetrics* match = NULL;
	int i;
	int size = iCharacterMetricsList.Size();
	// see if we have this one already
	for (i = 0; i < size; i++)
		{
		if (iCharacterMetricsList[i])
			{
			match = iCharacterMetricsList[i]->Metric();
			}
		if ((match->iAscentInPixels == aMetrics.iAscentInPixels)
			&& (match->iHeightInPixels == aMetrics.iHeightInPixels)
			&& (match->iLeftAdjustInPixels == aMetrics.iLeftAdjustInPixels)
			&& (match->iMoveInPixels == aMetrics.iMoveInPixels)
			&& (match->iRightAdjustInPixels == aMetrics.iRightAdjustInPixels))
			{
			same = true;
			break;
			}
		}
	if (!same)
		{
		i = -1;	// not found
		}
	return i;
	}

void MetricDistribution::Externalize(ostream& out)
	{
	streamoff idOffset = iStreamId;
	out.write(reinterpret_cast<char*>(&idOffset), sizeof(idOffset));
	int32 numMetrics = iCharacterMetricsList.Size();
	out.write(reinterpret_cast<char*>(&numMetrics), sizeof(numMetrics));
	}

void MetricDistribution::ExternalizeComponents(ostream& out)
	{
	iStreamId = out.tellp();
	iCharacterMetricsList.Externalize(out);
	}

void Characters::Externalize(ostream& out)
	{
	iStreamId = out.tellp();
	iBitmapOffsetList.Externalize(out);
	}

Characters::~Characters()
	{
	iBitmapOffsetList.Destroy();
	}

ByteList::ByteList()
 :	iString(), iOffset(0)
	{
	}

void ByteList::AddBit(char aBit)
	{
	if (iOffset > 7)
		NewByte();
	const char mask = 1;
	aBit = char(aBit & mask);
	char byte = char(aBit << iOffset);
	int index = iString.Length() - 1;
	iString[index] = char(iString[index] | byte);
	iOffset++;
	}

void ByteList::NewByte()
	{
	char byte = 0;
	iString += byte;
	iOffset = 0;
	}

int ByteList::Length() const
	{
	return iString.Length();
	}

void ByteList::Externalize(ostream& out)
	{
	int32 length = iString.Length();
	out.write((char*) &length, sizeof(length));
	out.write(iString.Text(), length);
	}

void CharactersBitmap::Externalize(ostream& out)
	{
	iStreamId = out.tellp();
	iByteList.Externalize(out);
	}

void CharactersBitmap::AddIndex(int aIndex)
	{// Add index to metrics into the bitmap code section
	 // Use 1 byte for most popular indices, 2 bytes otherwise
	int power;
	if (aIndex < KNumberOfPopularIndices)
		{
		iByteList.AddBit(0);
		power = KNumberOfBitsInByte - 1;
		}
	else
		{
		iByteList.AddBit(1);
		power = KNumberOfBitsInTwoBytes - 1;
		}

	char sigBit = 0;
	// Add significant bits of index.
	for(int bitToAdd = 0; bitToAdd < power; bitToAdd++)
		{
		sigBit = char(aIndex >> bitToAdd);
		iByteList.AddBit(sigBit);
		}
	}

void BitmapCodeSection::Externalize(ostream& out)
	{
	out.write((char*) &iStart, sizeof(iStart));
	out.write((char*) &iEnd, sizeof(iEnd));
	streamoff idOffset = iCharacters.iStreamId;
	out.write(reinterpret_cast<char*>(&idOffset), sizeof(idOffset));
	idOffset = iCharactersBitmap.iStreamId;
	out.write(reinterpret_cast<char*>(&idOffset), sizeof(idOffset));
	}
	
void BitmapCodeSection::ExternalizeComponents(ostream& out)
	{
	iCharacters.Externalize(out);
	iCharactersBitmap.Externalize(out);
	}
	
FontBitmap::FontBitmap()
 :	iPosture(PostureUpright),
	iStrokeWeight(StrokeWeightNormal),
	iIsProportional(efalse),
	iCellHeightInPixels(0),
	iAscentInPixels(0),
	iMaxCharWidthInPixels(0),
	iMaxNormalCharWidthInPixels(0),
	iBitmapEncoding(0)
	{
	iCharacterMetrics = MetricDistribution::New();
	}

void FontBitmap::Externalize(ostream& out)
	{
	iStreamId = out.tellp();
	out.write((char*) &iUid, sizeof(iUid));
	out.put((char) iPosture);
	out.put((char) iStrokeWeight);
	out.put((char) iIsProportional);
	out.write((char*) &iCellHeightInPixels, sizeof(iCellHeightInPixels));
	out.write((char*) &iAscentInPixels, sizeof(iAscentInPixels));
	out.write((char*) &iMaxCharWidthInPixels, sizeof(iMaxCharWidthInPixels));
	out.write((char*) &iMaxNormalCharWidthInPixels, sizeof(iMaxNormalCharWidthInPixels));
	out.write((char*) &iBitmapEncoding, sizeof(iBitmapEncoding));
	iCharacterMetrics->Externalize(out);
	iCodeSectionList.Externalize(out);
	}

void FontBitmap::ExternalizeComponents(ostream& out)
	{
	// write out characters and chactersbitmap records
	iCharacterMetrics->ExternalizeComponents(out);
	int size = iCodeSectionList.Size();
	for (int i = 0; i < size; i++)
		{
		iCodeSectionList[i]->ExternalizeComponents(out);
		}
	}

FontBitmap::~FontBitmap()
	{
	iCodeSectionList.Destroy();
	delete iCharacterMetrics;
	}

TypefaceFontBitmap::TypefaceFontBitmap(FontBitmap* aFontBitmap)
 :	iFontBitmap(aFontBitmap),
	iFontBitmapUid(KNullUid),
	iWidthFactor(1),
	iHeightFactor(1)
	{
	}

TypefaceFontBitmap::TypefaceFontBitmap(uid aFontBitmapUid)
 :	iFontBitmap(NULL),
	iFontBitmapUid(aFontBitmapUid),
	iWidthFactor(1),
	iHeightFactor(1)
	{
	}

void TypefaceFontBitmap::Externalize(ostream& out)
	{
	if (iFontBitmap)
		out.write((char*) &iFontBitmap->iUid, sizeof(iFontBitmap->iUid));
	else
		out.write((char*) &iFontBitmapUid, sizeof(iFontBitmapUid));
	out.write((char*) &iWidthFactor, sizeof(iWidthFactor));
	out.write((char*) &iHeightFactor, sizeof(iHeightFactor));
	}

void FntTypeface::Externalize(ostream& out)
	{
	iStreamId = out.tellp();
	Typeface::Externalize(out);
	iTypefaceFontBitmapList.Externalize(out);
	}

FontStoreFile::FontStoreFile()
 :	iCollectionUid(KNullUid),
	iKPixelAspectRatio(1000),
	iCopyrightInfo(),
	iDataStreamId(0)
	{
	}

void FontStoreFile::AddTypeface(FntTypeface *aTypeface)
	{
	iTypefaceList.Add(aTypeface);
	for (int i = 0; i < aTypeface->iTypefaceFontBitmapList.Size(); i++)
		{
		if (aTypeface->iTypefaceFontBitmapList[i]->iFontBitmap)
			iFontBitmapList.Add(aTypeface->iTypefaceFontBitmapList[i]->iFontBitmap);
		}
	}

void FontStoreFile::AddFontBitmap(FontBitmap* aFontBitmap)
	{
	iFontBitmapList.Add(aFontBitmap);
	}

void FontStoreFile::Externalize(ostream& out)
	{
	ExternalizeHeader(out);
	ExternalizeComponents(out);
	}

void FontStoreFile::ExternalizeHeader(ostream& out)
	{
	out.write((char*) &KStoreWriteOnceLayoutUid, sizeof(KStoreWriteOnceLayoutUid));
	out.write((char*) &KFontStoreFileUid, sizeof(KFontStoreFileUid));
	out.write((char*) &KNullUid, sizeof(KNullUid));
	out.write((char*) &KFontStoreFileChecksum, sizeof(KFontStoreFileChecksum));
	streamoff idOffset = iStreamId;
	out.write(reinterpret_cast<char*>(&idOffset), sizeof(idOffset));
	iStreamId = out.tellp();
	out.write((char*) &KFnttranVersion, sizeof(KFnttranVersion));
	out.write((char*) &iCollectionUid, sizeof(iCollectionUid));
	out.write((char*) &iKPixelAspectRatio, sizeof(iKPixelAspectRatio));
	idOffset = iDataStreamId;
	out.write(reinterpret_cast<char*>(&idOffset), sizeof(idOffset));
	iCopyrightInfo.Externalize(out);
	}

void FontStoreFile::ExternalizeComponents(ostream& out)
	{
	iDataStreamId = out.tellp();
	iFontBitmapList.Externalize(out);
	iTypefaceList.Externalize(out);
	iFontBitmapList.ExternalizeComponents(out);
	}

boolean FontStore::Store(const String& aFilename)
	{
	boolean state = efalse;
	ofstream fout;
	String string = aFilename;
	fout.open(string.Text(), ios::binary);
	if (!fout.fail())
		{
		iFontStoreFile->Externalize(fout);
		fout.close();
		fout.open(string.Text(), ios::binary | ios::trunc);
		iFontStoreFile->Externalize(fout);
		fout.close();
		state = etrue;
		}
	return state;
	}

void FontStore::AddFontStoreFile(FontStoreFile* aFontStoreFile)
	{
	iFontStoreFile = aFontStoreFile;
	}

void FontStore::AddFontBitmap(FontBitmap *aFontBitmap)
	{
	iFontBitmapList.Add(aFontBitmap);
	}

Record* FontStore::FindFontBitmap(String& aLabel)
	{
	return iFontBitmapList.LabelToRecord(aLabel);
	}

void FontStore::AddTypeface(FntTypeface *aTypeface)
	{
	iTypefaceList.Add(aTypeface);
	}

Record* FontStore::FindTypeface(String& aLabel)
	{
	return iTypefaceList.LabelToRecord(aLabel);
	}

FontStore::FontStore()
 :	iFontStoreFile(NULL),
	iFontBitmapList(),
	iTypefaceList()
	{
	}

FontStore::~FontStore()
	{
	delete iFontStoreFile;
	iFontBitmapList.Destroy();
	iTypefaceList.Destroy();
	}
