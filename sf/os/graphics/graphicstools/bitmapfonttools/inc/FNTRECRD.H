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
* Header FNTRECRD.H
*
*/


#ifndef __FNTRECRD_H__
#define __FNTRECRD_H__

#include "LEXICAL.H"
#include "LST.H"
#include "RECORD.H"
#include "STRNG.H"
#include "UID.H"
/**
@internalComponent
*/
const boolean PostureUpright = 0;
const boolean PostureItalic = 1;
/**
@internalComponent
*/
const boolean StrokeWeightNormal = 0;
const boolean StrokeWeightBold = 1;

class BitmapOffset
/**
@internalComponent
*/
	{
public:
	BitmapOffset(uint16 aBitmapOffset);
	void Externalize(ostream& out);
public:
	uint16 iBitmapOffset;	// restricts bitmap to 64k
	};

class CharacterMetrics
/**
@internalComponent
*/
	{
public:
	CharacterMetrics();
	void Externalize(ostream& out);
public:
	chardim iAscentInPixels;
	chardim iHeightInPixels;
	chardim iLeftAdjustInPixels;
	chardim iMoveInPixels;
	chardim iRightAdjustInPixels;
	};


class MetricDistributionMember
/**
Class which wraps up a given metric with the number of characters that use it
@internalComponent
*/
	{
public:
	~MetricDistributionMember();
	MetricDistributionMember();
public:
	CharacterMetrics* Metric() const;
	int Frequency() const;
	void IncrementFrequency(int aIncrementBy = 1);
	void SetMetric(CharacterMetrics* aMetric);
	void SetFrequency(int aFrequency);
	void Externalize(ostream& out);
private:
	int iFrequency;
	CharacterMetrics* iMetric;
	};

class MetricDistribution
/**
Class which maintains a list of metrics and the number of characters using each,
along with the ability to sort them into most popular first order.
@internalComponent
*/
	{
public:
	~MetricDistribution();
	static MetricDistribution* New();
public:
	void SortMetricsByFrequency();
	void SetIndex(const CharacterMetrics& aMetrics, int aIndex);
	int Index(const CharacterMetrics& aMetrics);
	void Externalize(ostream& out);
	void ExternalizeComponents(ostream& out);
	void AddOrIncrementMetric(const CharacterMetrics& aMetrics, int aFrequency = 1);
	const CharacterMetrics& MostPopular(int aStartIndex);
private:
	MetricDistribution();
public:
	ObjectList<MetricDistributionMember*> iCharacterMetricsList;
	streampos iStreamId;
	};

class Characters
/**
@internalComponent
*/
	{
public:
	void Externalize(ostream& out);
	ObjectList<BitmapOffset*> iBitmapOffsetList;
	~Characters();
public:
	streampos iStreamId;
	};

class ByteList
/**
@internalComponent
*/
	{
public:
	ByteList();
	void AddBit(char aBit);
	void NewByte();
	int Length() const;
	void Externalize(ostream& out);
private:
	String iString;
	char iOffset;
	};

class CharactersBitmap : public Record
/**
@internalComponent
*/
	{
public:
	void AddIndex(int aIndex);
	void Externalize(ostream& out);
public:
	ByteList iByteList;
	streampos iStreamId;
	};

class BitmapCodeSection
/**
@internalComponent
*/
	{
public:
	void Externalize(ostream& out);
	void ExternalizeComponents(ostream& out);
public:
	uint16 iStart;
	uint16 iEnd;
	Characters iCharacters;
	CharactersBitmap iCharactersBitmap;
	};	

class FontBitmap : public Record
/**
@internalComponent
*/
	{
public:
	FontBitmap();
	void Externalize(ostream& out);
	void ExternalizeComponents(ostream& out);
	~FontBitmap();
public:
	uid iUid;
	boolean iPosture;
	boolean iStrokeWeight;
	boolean iIsProportional;
	chardim iCellHeightInPixels;
	chardim iAscentInPixels;
	chardim iMaxCharWidthInPixels;
	chardim iMaxNormalCharWidthInPixels;
	int32 iBitmapEncoding;
	ObjectList<BitmapCodeSection*> iCodeSectionList;
	MetricDistribution* iCharacterMetrics;
	};
 
class TypefaceFontBitmap
/**
@internalComponent
*/
	{
public:
	TypefaceFontBitmap(FontBitmap* aFontBitmap);
	TypefaceFontBitmap(uid aFontBitmapUid);
	void Externalize(ostream& out);	
public:
	FontBitmap* iFontBitmap;
	uid iFontBitmapUid;
	char iWidthFactor;
	char iHeightFactor;
	};

class FntTypeface : public Record, public Typeface
/**
@internalComponent
*/
	{
public:
	void Externalize(ostream& out);
public:
	ObjectList<TypefaceFontBitmap*> iTypefaceFontBitmapList;
	};

class FontStoreFile : public Record
/**
@internalComponent
*/
	{
public:
	FontStoreFile();
	void AddTypeface(FntTypeface* aTypeface);
	void AddFontBitmap(FontBitmap* aFontBitmap);
	void Externalize(ostream& out);
private:
	void ExternalizeHeader(ostream& out);
	void ExternalizeComponents(ostream& out);
public:
	uid iCollectionUid;
	int32 iKPixelAspectRatio;
	ObjectList<String*> iCopyrightInfo;
private:
	streampos iDataStreamId;
	RecordList iFontBitmapList;
	RecordList iTypefaceList;
	};

class FontStore
/**
@internalComponent
*/
	{
public:
	boolean Store(const String& aFilename);
	void AddFontStoreFile(FontStoreFile* aFontStoreFile);
	void AddFontBitmap(FontBitmap* aFontBitmap);
	Record* FindFontBitmap(String& aLabel);
	void AddTypeface(FntTypeface* aTypeface);
	Record* FindTypeface(String& aLabel);
	FontStore();
	~FontStore();
private:
	FontStoreFile* iFontStoreFile;
	RecordList iFontBitmapList;
	RecordList iTypefaceList;
	};

#endif
