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
* Header PDRRECRD.H
*
*/


#ifndef __PDRRECRD_H__
#define __PDRRECRD_H__

#include "LEXICAL.H"
#include "RECORD.H"
#include "UID.H"

/** 
Index of text styles used by printer description records. 
@publishedAll
@released
*/
enum StyleIndex
	{
	/** Normal text. */
	Normal,
	/** Bold text. */
	Bold,
	/** Italic text. */
	Italic,
	/** Bold italic text. */
	BoldItalic
	};

class PdrResource
/** 
This class encapsulates a printer resource. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static PdrResource* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
public:
	/** Resource identifier. */
	int32 iId;
	/** String containing the resource. */
	String iString;
	};

class PdrResources : public Record
/** 
This class holds printer resources. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static PdrResources* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
	IMPORT_C void AddResource(PdrResource* aResource);
private:
	~PdrResources();
private:
	ObjectList<PdrResource*> iPdrResourceList;
	};

class PdrTranslation
/** 
This class holds a translation between printer descriptions. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static PdrTranslation* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
public:
	/** Printer identifier to translate from. */
	uint16 iFrom;
	/** Printer identifier to translate to. */
	String iTo;	//  !! Think about Unicode version
	};

class PdrTranslates : public Record
/** 
This class encapsulates a list of printer description translations. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static PdrTranslates* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
	IMPORT_C void AddTranslation(PdrTranslation* aTranslation);
private:
	~PdrTranslates();
private:
	ObjectList<PdrTranslation*> iPdrTranslationList;
	};

class Width
/** 
This class is used to hold font widths. It is used by printer descriptions. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static Width* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
public:
	/** Font width in pixels. */
	uint16 iWidthInPixels;
	};

class WidthsCodeSection
/** 
This class encapsulates a list of font widths used by printer descriptions. 
@publishedAll
@released
*/
	{
private:
	WidthsCodeSection();
public:
	IMPORT_C static WidthsCodeSection* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
	IMPORT_C void AddWidth(Width* aWidth);
	IMPORT_C Width* WidthList(int i);
	IMPORT_C int NumWidths();
	~WidthsCodeSection();
public:
	/** Start of list. */
	uint16 iStart;
	/** End of list. */
	uint16 iEnd;
private:
	ObjectList<Width*> iWidthList;
	};	

class FontInfo : public Record
/** 
This class contains information about fonts. It is used by a printer description. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static FontInfo* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
	IMPORT_C void AddCodeSection(WidthsCodeSection* aCodeSection);
	IMPORT_C WidthsCodeSection* CodeSectionList(int i);
	IMPORT_C int NumCodeSections();
private:
	~FontInfo();
public:
	/** Font ascent in pixels. */
	uint16 iAscentInPixels;
	/** Maximum font character width in pixels. */
	uint16 iMaxCharWidthInPixels;
	/** Normal font character width in pixels. */
	uint16 iMaxNormalCharWidthInPixels;
private:
	ObjectList<WidthsCodeSection*> iCodeSectionList;
	};
 
class PdrStyle
/** 
This class contains information about a font style used in a printer descriptions. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static PdrStyle* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
	PdrStyle();
public:
	/** Determines whether the font is available for use. */
	boolean iIsAvailable;
	/** Reference to a record containing information about the font. */
	Record *iFontInfo;
	};

class PdrFontHeight
/** 
This class stores information for use in printer descriptions about font heights 
and relative scaling. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static PdrFontHeight* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
	PdrFontHeight();
public:
	/** Command string associated with font height. */
	String iCommandString;
	/** Font height in twips. */
	int32 iHeightInTwips;
	/** Width scaling. */
	int32 iWidthScale;
	/** Array containing space for four styles. */
	PdrStyle iStyle[BoldItalic + 1];	// Array for four styles
	};

class PdrScalableFontHeight
/** 
This class contains information on scalable fonts used by printer descriptions. 
@publishedAll
@released
*/
	{
public:
	PdrScalableFontHeight();
	void Externalize(ostream& out);
public:
	/** Contains a printer command string associated with this font. */
	String iCommandString;
	/** Minimum height in twips. */
	int32 iHeightMinInTwips;
	/** Maximum height in twips. */
	int32 iHeightMaxInTwips;
	/** Scale delta in twips. */
	int32 iHeightDeltaInTwips;
	/** Array for four styles. */
	PdrStyle iStyle[BoldItalic + 1];	// Array for four styles
	};

class TypefaceFonts : public Record
/** 
This class encapsulates a list of font heights for printer descriptions. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static TypefaceFonts* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
	IMPORT_C void AddFontHeight(PdrFontHeight* aFontHeight);
	IMPORT_C int NumFontHeights();
	PdrFontHeight* FontHeightList(int i);
private:
	TypefaceFonts();
	~TypefaceFonts();
public:
	Typeface iTypeface;
	/** Determines whether the font height is scalable. */
	boolean iIsScalable;
private:
	ObjectList<PdrFontHeight*> iFontHeightList;
public:
	/** Describes a scalable font height. */
	PdrScalableFontHeight iScalableFontHeight;
	Record *iTranslates;
	};

class TypefaceFontsEntry
/** 
This class encapsulates information about a printer font. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static TypefaceFontsEntry* New(Record* aTypefaceFonts);
	IMPORT_C void Delete();
	void Externalize(ostream& out);
private:
	TypefaceFontsEntry(Record* aTypefaceFonts);
public:
	~TypefaceFontsEntry();
public:
	/** Contains the font record. */
	Record* iTypefaceFonts;
	/** Determines whether the font can be used in portrait view. */
	boolean iNotInPortrait;
	/** Determines whether the font can be used in landscape view. */
	boolean iNotInLandscape;
	};

class Margins
/** 
This class contains the margins used by a printer description. 
@publishedAll
@released
*/
	{
public:
	void Externalize(ostream& out);
public:
	/** Left margin. */
	int32 iLeft;
	/** Right margin. */
	int32 iRight;
	/** Top margin. */
	int32 iTop;
	/** Bottom margin. */
	int32 iBottom;
	};
/*
class PdrExtraInfo : public Record
	{
public:
	~PdrExtraInfo();
	void Externalize(ostream& out);
public:
	ObjectList<String*> iInfo;
	};
*/
class PdrModelInfo
/** 
This class contains information about a printer model used by a printer description. 
@publishedAll
@released
*/
	{
public:
	void Externalize(ostream& out);
	IMPORT_C void AddTypefaceFontsEntry(TypefaceFontsEntry* aTypefaceFontsEntry);
	int NumTypefaceFontsEntries();
	TypefaceFontsEntry* TypefaceFontsEntryList(int i);
	PdrModelInfo();
	~PdrModelInfo();
public:
	streampos iStreamId;
	/** Flags associated with model. */
	int32 iFlags;
	/** Pixel width in twips. */
	int32 iKPixelWidthInTwips;
	/** Pixel height in twips. */
	int32 iKPixelHeightInTwips;
	/** Portrait printng offset in twips. */
	Point iPortraitOffsetInPixels;
	/** Landscape printing offset in twips. */
	Point iLandscapeOffsetInPixels;
	/** Minimum margin in pixels. */
	Margins iMinMarginsInPixels;
	/** Display mode. */
	int32 iDisplayMode;
private:
	ObjectList<TypefaceFontsEntry*> iTypefaceFontsEntryList;
public:
	Record *iResources;
	Record *iSpareRecord;
	};
 
class PrinterModelEntry
/** 
This class contains information on a printer model. 
@publishedAll
@released
*/
	{
public:
	PrinterModelEntry();
	void Externalize(ostream& out);
public:
	/** Printer name. */
	String iName;
	/** Whether a printer port is required. */
	boolean iRequiresPrinterPort;
	uid iUid;
	};

class PrinterModelHeader : public Record
/** 
This class encapsulates information about a printer model. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static PrinterModelHeader* New();
	IMPORT_C void Delete();
	void Externalize(ostream& out);
	void ExternalizeData(ostream& out);
	boolean IsPdrModel();
public:
	/** Printer model entry. */
	PrinterModelEntry iEntry;
	/** Printer model information. */
	PdrModelInfo iInfo;
	};

class PdrStoreFile : public Record
/** 
This class stores printer description files. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static PdrStoreFile* New();
	IMPORT_C void Delete();
	IMPORT_C void AddModel(PrinterModelHeader *aModel);
	void Externalize(ostream& out);
private:
	PdrStoreFile();
private:
	void AddTypefaceFonts(TypefaceFonts* aTypefaceFonts);
	void ExternalizeHeader(ostream& out);
	void ExternalizeComponents(ostream& out);
public:
	/** Printer descriptor. */
	String iPDLName;
	uid iPDLUid;
private:
	RecordList iModelList;
	RecordList iTypefaceFontsList;
	RecordList iFontInfoList;
	RecordList iResourcesList; 
	RecordList iTranslatesList;
//	RecordList iExtraInfoList;
	};

class PdrModelStore
/** 
This class is used to store information about printer models, and externalize 
it to external files. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C boolean Store(const String& aFilename);
	IMPORT_C void AddPdrStoreFile(PdrStoreFile* aPdrStoreFile);
	IMPORT_C void AddModel(PrinterModelHeader *aModel);
	IMPORT_C Record* FindModel(String& aLabel);
	IMPORT_C void AddTypefaceFonts(TypefaceFonts *aTypefaceFonts);
	IMPORT_C Record* FindTypefaceFonts(String& aLabel);
	IMPORT_C void AddFontInfo(FontInfo *aFontInfo);
	IMPORT_C Record* FindFontInfo(String& aLabel);
	IMPORT_C void AddResources(PdrResources *aResources);
	IMPORT_C Record* FindResources(String& aLabel);
	IMPORT_C void AddTranslates(PdrTranslates *aTranslates);
	IMPORT_C Record* FindTranslates(String& aLabel);
//	void AddExtraInfo(PdrExtraInfo *aInfo);
//	Record* FindExtraInfo(String& aLabel);
	PdrModelStore();
	~PdrModelStore();
private:
	/** This attribute is internal, and is not intended for use. */
	PdrStoreFile* iPdrStoreFile;
	RecordList iModelList;
	RecordList iTypefaceFontsList;
	RecordList iFontInfoList;
	RecordList iResourcesList;
	RecordList iTranslatesList;
//	RecordList iExtraInfoList;
	};

#endif
