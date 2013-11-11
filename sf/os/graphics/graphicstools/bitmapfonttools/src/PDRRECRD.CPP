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
* Header PDRRECRD.CPP
*
*/


#include "PDRRECRD.H"

EXPORT_C PdrResource* PdrResource::New()
/** Creates a new instance of this class.

@return Reference to a new object. */
	{
	return new PdrResource();
	}

EXPORT_C void PdrResource::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void PdrResource::Externalize(ostream& out)
	{
	uint8 id = (uint8) iId;
	out.write((char*) &id, sizeof(id));
	iString.Externalize(out);
	}

EXPORT_C PdrResources* PdrResources::New()
/** Creates a new instance of this class.

@return Reference to a new object. */
	{
	return new PdrResources();
	}

EXPORT_C void PdrResources::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void PdrResources::Externalize(ostream& out)
	{
	iStreamId = out.tellp();
	iPdrResourceList.Externalize(out);
	}

EXPORT_C void PdrResources::AddResource(PdrResource* aResource)
/** Adds a printer resource to the object.

@param aResource Printer resource. */
	{
	iPdrResourceList.Add(aResource);
	}

PdrResources::~PdrResources()
/** Default destructor. */
	{
	iPdrResourceList.Destroy();
	}

EXPORT_C PdrTranslation* PdrTranslation::New()
/** Creates a new instance of this class.

@return Reference to a new object. */
	{
	return new PdrTranslation();
	}

EXPORT_C void PdrTranslation::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void PdrTranslation::Externalize(ostream& out)
	{
	out.write((char*) &iFrom, sizeof(iFrom));
	iTo.Externalize(out);
	}

EXPORT_C PdrTranslates* PdrTranslates::New()
/** Creates a new instance of this class.

@return Reference to a new object. */
	{
	return new PdrTranslates();
	}

EXPORT_C void PdrTranslates::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void PdrTranslates::Externalize(ostream& out)
	{
	iStreamId = out.tellp();
	iPdrTranslationList.Externalize(out);
	}

EXPORT_C void PdrTranslates::AddTranslation(PdrTranslation* aTranslation)
/** Adds a printer translation to the list.

@param aTranslation Reference to a translation. */
	{
	iPdrTranslationList.Add(aTranslation);
	}

PdrTranslates::~PdrTranslates()
/** This function is internal, and is not intended for use. */
	{
	iPdrTranslationList.Destroy();
	}

EXPORT_C Width* Width::New()
/** Creates a new instance of this object.

@return Reference to a new object. */
	{
	return new Width();
	}

EXPORT_C void Width::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void Width::Externalize(ostream& out)
	{
	out.write ((char*) &iWidthInPixels, sizeof(iWidthInPixels));
	}

WidthsCodeSection::WidthsCodeSection()
 :	iWidthList()
/** This function is internal only, and is not intended for use. */
	{
	}

EXPORT_C WidthsCodeSection* WidthsCodeSection::New()
/** Creates a new instance of this class.

@return Reference to a new object. */
	{
	return new WidthsCodeSection();
	}
	
EXPORT_C void WidthsCodeSection::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void WidthsCodeSection::Externalize(ostream& out)
	{
	out.write((char*) &iStart, sizeof(iStart));
	out.write((char*) &iEnd, sizeof(iEnd));
	iWidthList.Externalize(out);
	}
	
EXPORT_C void WidthsCodeSection::AddWidth(Width* aWidth)
/** Adds a new font width to the list.

@param aWidth Font width. */
	{
	iWidthList.Add(aWidth);
	}

EXPORT_C Width* WidthsCodeSection::WidthList(int i)
/** Returns a font width from the list at the position specified.

@param i Position of font width in list.
@return Reference to a font width. */
	{
	return iWidthList[i];
	}

EXPORT_C int WidthsCodeSection::NumWidths()
/** Returns the number of font widths in the list.

@return Number of widths. */
	{
	return iWidthList.Size();
	}

WidthsCodeSection::~WidthsCodeSection()
/** Default destructor. */
	{
	iWidthList.Destroy();
	}

EXPORT_C FontInfo* FontInfo::New()
/** Creates a new instance of this class.

@return Reference to a new instance of this class. */
	{
	return new FontInfo();
	}

EXPORT_C void FontInfo::Delete()
/** Deletes the current FontInfo object. */
	{
	delete this;
	}

void FontInfo::Externalize(ostream& out)
	{
	iStreamId = out.tellp();
	out.write((char*) &iAscentInPixels, sizeof(iAscentInPixels));
	out.write((char*) &iMaxCharWidthInPixels, sizeof(iMaxCharWidthInPixels));
	out.write((char*) &iMaxNormalCharWidthInPixels, sizeof(iMaxNormalCharWidthInPixels));
	iCodeSectionList.Externalize(out);
	}

EXPORT_C void FontInfo::AddCodeSection(WidthsCodeSection* aCodeSection)
/** Adds a section of information about the font to an internally-stored array.

@param aCodeSection Font information. */
	{
	iCodeSectionList.Add(aCodeSection);
	}	

EXPORT_C WidthsCodeSection* FontInfo::CodeSectionList(int i)
/** Returns a section of font information from a specified position in the internally-stored 
array.

@param i Postion of information in the array.
@return Reference to section of font information. */
	{
	return iCodeSectionList[i];
	}

EXPORT_C int FontInfo::NumCodeSections()
/** Returns the number of sections of font information in the internally-stored 
array.

@return Current number of font information sections. */
	{
	return iCodeSectionList.Size();
	}

FontInfo::~FontInfo()
/** This function is internal, and not intended for use. */
	{
	iCodeSectionList.Destroy();
	}

EXPORT_C PdrStyle* PdrStyle::New()
/** Creates a new instance of this class

@return Reference to a new object. */
	{
	return new PdrStyle();
	}

EXPORT_C void PdrStyle::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void PdrStyle::Externalize(ostream& out)
	{
	out.write((char*) &iIsAvailable, sizeof(iIsAvailable));
	streamoff streamid = 0;
	if (iIsAvailable)
		streamid = iFontInfo->iStreamId;
	::ExternalizeStreamOff(out, streamid);
	}

PdrStyle::PdrStyle()
 :	iIsAvailable(efalse)
/** Default constructor. */
	{
	}

EXPORT_C PdrFontHeight* PdrFontHeight::New()
/** Returns a new instance of this class.

@return Reference to new object. */
	{
	return new PdrFontHeight();
	}

EXPORT_C void PdrFontHeight::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void PdrFontHeight::Externalize(ostream& out)
	{
	iCommandString.Externalize(out);
	out.write((char*) &iHeightInTwips, sizeof(iHeightInTwips));
	out.write((char*) &iWidthScale, sizeof(iWidthScale));
	for (int style = Normal;style <= BoldItalic; style++)
		iStyle[style].Externalize(out);
	}

PdrFontHeight::PdrFontHeight():
	iWidthScale(1)
/** Default constructor. */
	{
	}

PdrScalableFontHeight::PdrScalableFontHeight()
/** Default constructor. */
	{
	}

void PdrScalableFontHeight::Externalize(ostream& out)
	{
	iCommandString.Externalize(out);
	out.write((char*) &iHeightMinInTwips, sizeof(iHeightMinInTwips));
	out.write((char*) &iHeightMaxInTwips, sizeof(iHeightMaxInTwips));
	out.write((char*) &iHeightDeltaInTwips, sizeof(iHeightDeltaInTwips));
	for (int style = Normal; style <= BoldItalic; style++)
		iStyle[style].Externalize(out);
	}

EXPORT_C TypefaceFonts* TypefaceFonts::New()
/** Creates a new instance of this class.

@return Reference to a new object. */
	{
	return new TypefaceFonts();
	}

EXPORT_C void TypefaceFonts::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void TypefaceFonts::Externalize(ostream& out)
	{
	iStreamId = out.tellp();
	iTypeface.Externalize(out);
	out.write((char*) &iIsScalable, sizeof(iIsScalable));
	if (iIsScalable)
		iScalableFontHeight.Externalize(out);
	else
		iFontHeightList.Externalize(out);
	::ExternalizeStreamOff(out, iTranslates->iStreamId);
	}

EXPORT_C void TypefaceFonts::AddFontHeight(PdrFontHeight* aFontHeight)
	{
	iFontHeightList.Add(aFontHeight);
	}

EXPORT_C int TypefaceFonts::NumFontHeights()
/** Returns the number of font height descriptions currently held in the list.

@return Number of descriptions. */
	{
	return iFontHeightList.Size();
	}

PdrFontHeight* TypefaceFonts::FontHeightList(int i)
/** Returns the font height description from the position specified in the list.

@param i Position of description.
@return Font height description. */
	{
	return iFontHeightList[i];
	}

TypefaceFonts::TypefaceFonts():
	iIsScalable(efalse)
/** This function is internal only, and is not intended for use. */
	{
	}

TypefaceFonts::~TypefaceFonts()
/** This function is internal only, and is not intended for use. */
	{
	iFontHeightList.Destroy();
	}

EXPORT_C TypefaceFontsEntry* TypefaceFontsEntry::New(Record* aTypefaceFonts)
	{
	return new TypefaceFontsEntry(aTypefaceFonts);
	}

EXPORT_C void TypefaceFontsEntry::Delete()
/** Deletes the current object. */
	{
	delete this;
	}
	
void TypefaceFontsEntry::Externalize(ostream& out)
	{
	::ExternalizeStreamOff(out, iTypefaceFonts->iStreamId);
	out.write((char*) &iNotInPortrait, sizeof(iNotInPortrait));
	out.write((char*) &iNotInLandscape, sizeof(iNotInLandscape));
	}

TypefaceFontsEntry::TypefaceFontsEntry(Record* aTypefaceFonts)
 :	iTypefaceFonts(aTypefaceFonts), iNotInPortrait(efalse), iNotInLandscape(efalse)
	{
	}

TypefaceFontsEntry::~TypefaceFontsEntry()
/** Default destructor. */
	{
	}

void Margins::Externalize(ostream& out)
	{
	out.write((char*) &iLeft, sizeof(iLeft));
	out.write((char*) &iRight, sizeof(iRight));
	out.write((char*) &iTop, sizeof(iTop));
	out.write((char*) &iBottom, sizeof(iBottom));
	}

/*
PdrExtraInfo::~PdrExtraInfo()
	{
	iInfo.Destroy();
	}

void PdrExtraInfo::Externalize(ostream& out)
	{
	iInfo.Externalize(out);
	}
*/

void PdrModelInfo::Externalize(ostream& out)
	{
	iStreamId=out.tellp();
	out.write((char*) &KPdrtranVersion, sizeof(KPdrtranVersion));
	out.write((char*) &iFlags, sizeof(iFlags));
	out.write((char*) &iKPixelWidthInTwips, sizeof(iKPixelWidthInTwips));
	out.write((char*) &iKPixelHeightInTwips, sizeof(iKPixelHeightInTwips));
	iPortraitOffsetInPixels.Externalize(out);
	iLandscapeOffsetInPixels.Externalize(out);
	iMinMarginsInPixels.Externalize(out);
	out.write((char*) &iDisplayMode, sizeof(iDisplayMode));
	iTypefaceFontsEntryList.Externalize(out);
	::ExternalizeStreamOff(out, iResources->iStreamId);
	streamoff streamid = 0;
	if (iSpareRecord)
		streamid = iSpareRecord->iStreamId;
	::ExternalizeStreamOff(out, streamid);
	}
 
EXPORT_C void PdrModelInfo::AddTypefaceFontsEntry(TypefaceFontsEntry* aTypefaceFontsEntry)
/** Adds an entry containing information about typeface fonts to an internally-stored 
array.

@param aTypefaceFontsEntry Typeface font information. */
	{
	iTypefaceFontsEntryList.Add(aTypefaceFontsEntry);
	}

int PdrModelInfo::NumTypefaceFontsEntries()
/** Returns the number of entries containing information about typeface fonts in 
the internally-stored array.

@return Number of entries. */
	{
	return iTypefaceFontsEntryList.Size();
	}

TypefaceFontsEntry* PdrModelInfo::TypefaceFontsEntryList(int i)
/** Returns the typeface font entry stored the internal array from the position 
specified.

@param i Position in array.
@return Typeface font entry. */
	{
	return iTypefaceFontsEntryList[i];
	}

PdrModelInfo::PdrModelInfo()
 :	iStreamId(0),
	iFlags(0),
	iKPixelWidthInTwips(0),
	iKPixelHeightInTwips(0),
	iPortraitOffsetInPixels(),
	iLandscapeOffsetInPixels(),
	iMinMarginsInPixels(),
	iDisplayMode(0),
	iTypefaceFontsEntryList(),
	iResources(NULL),
	iSpareRecord(NULL)	// !! Not used yet
/** Default constructor. */
	{
	}

PdrModelInfo::~PdrModelInfo()
/** Default destructor. */
	{
	iTypefaceFontsEntryList.Destroy();
	}

PrinterModelEntry::PrinterModelEntry()
 :	iName(), iRequiresPrinterPort(efalse), iUid(KNullUid)
/** Default constructor. */
	{
	}

void PrinterModelEntry::Externalize(ostream& out)
	{
	iName.Externalize(out);
	out.write((char*) &iRequiresPrinterPort, sizeof(iRequiresPrinterPort));
	out.write((char*) &iUid, sizeof(iUid));
	}

EXPORT_C PrinterModelHeader* PrinterModelHeader::New()
/** Creates a new instance of this class.

@return Reference to a new object. */
	{
	return new PrinterModelHeader();
	}

EXPORT_C void PrinterModelHeader::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

void PrinterModelHeader::Externalize(ostream& out)
	{
	iEntry.Externalize(out);
	::ExternalizeStreamOff(out, iInfo.iStreamId);
	}

void PrinterModelHeader::ExternalizeData(ostream& out)
	{
	iInfo.Externalize(out);
	}

boolean PrinterModelHeader::IsPdrModel()
/** Determines whether printer information is available for this printer model.

@return ETrue: printer description is available. */
	{
	return iInfo.NumTypefaceFontsEntries() || iInfo.iResources;
	}

EXPORT_C PdrStoreFile* PdrStoreFile::New()
/** Creates a new instance of this class.

@return Reference to a new object. */
	{
	return new PdrStoreFile();
	}

EXPORT_C void PdrStoreFile::Delete()
/** Deletes the current object. */
	{
	delete this;
	}

EXPORT_C void PdrStoreFile::AddModel(PrinterModelHeader* aModel)
/** Adds a printer model.

@param aModel Printer model. */
	{
	iModelList.Add(aModel);
	if (aModel->IsPdrModel())
		{
		for (int i = 0; i < aModel->iInfo.NumTypefaceFontsEntries(); i++)
			AddTypefaceFonts((TypefaceFonts*) aModel->iInfo.TypefaceFontsEntryList(i)->iTypefaceFonts);
		iResourcesList.Add(aModel->iInfo.iResources);
//		if (aModel->iInfo.iSpareRecord)
//			iExtraInfoList.Add(aModel->iInfo.iSpareRecord);
		}
	}

void PdrStoreFile::Externalize(ostream& out)
	{
	ExternalizeHeader(out);
	ExternalizeComponents(out);
	}

void PdrStoreFile::ExternalizeHeader(ostream& out)
	{
	out.write((char*) &KStoreWriteOnceLayoutUid, sizeof(KStoreWriteOnceLayoutUid));
	out.write((char*) &KPdrStoreFileUid, sizeof(KPdrStoreFileUid));
	out.write((char*) &KNullUid, sizeof(KNullUid));
	out.write((char*) &KPdrStoreFileChecksum, sizeof(KPdrStoreFileChecksum));
	::ExternalizeStreamOff(out, iStreamId);
	iStreamId = out.tellp();
	iPDLName.Externalize(out);
	out.write((char*) &iPDLUid, sizeof(iPDLUid));
	iModelList.Externalize(out);
	}

void PdrStoreFile::AddTypefaceFonts(TypefaceFonts* aTypefaceFonts)
	{
	iTypefaceFontsList.Add(aTypefaceFonts);
	iTranslatesList.Add(aTypefaceFonts->iTranslates);
	if (aTypefaceFonts->iIsScalable)
		{
		for (int style = Normal; style <= BoldItalic; style++)
			{
			if (aTypefaceFonts->iScalableFontHeight.iStyle[style].iIsAvailable)
				iFontInfoList.Add(aTypefaceFonts->iScalableFontHeight.iStyle[style].iFontInfo);
			}
		}
	else
		{
		for (int j = 0; j < aTypefaceFonts->NumFontHeights(); j++)
			{
			PdrFontHeight* fontheight = aTypefaceFonts->FontHeightList(j);
			for (int style = Normal; style <= BoldItalic; style++)
				{
				if (fontheight->iStyle[style].iIsAvailable)
					iFontInfoList.Add(fontheight->iStyle[style].iFontInfo);
				}
			}
		}
	}

void PdrStoreFile::ExternalizeComponents(ostream& out)
	{
	for (int i = 0; i < iModelList.Size(); i++)
		{
		PrinterModelHeader* model = (PrinterModelHeader*) iModelList[i];
		if (model->IsPdrModel())
			model->ExternalizeData(out);
		}
	iTypefaceFontsList.Externalize(out);
	iFontInfoList.Externalize(out);
	iResourcesList.Externalize(out);
	iTranslatesList.Externalize(out);
//	iExtraInfoList.Externalize(out);
	}

PdrStoreFile::PdrStoreFile()
 :	Record(),
	iPDLName(),
	iPDLUid(KNullUid),
	iModelList(),
	iTypefaceFontsList(),
	iFontInfoList(),
	iResourcesList(),
	iTranslatesList()
/** This function is internal only, and is not intended for use. */
	{
	}

EXPORT_C boolean PdrModelStore::Store(const String& aFilename)
/** Externalizes printer information from the store to the specfied external file.

@param aFilename Filename.
@return ETrue: store successful. */
	{
	boolean state = efalse;
	ofstream fout;
	String string = aFilename;
	fout.open(string.Text(), ios::binary);
	if (!fout.fail())
		{
		iPdrStoreFile->Externalize(fout);
		fout.close();
		fout.open(string.Text(), ios::binary | ios::trunc);
		iPdrStoreFile->Externalize(fout);
		fout.close();
		state = etrue;
		}
	return state;
	}

EXPORT_C void PdrModelStore::AddPdrStoreFile(PdrStoreFile* aPdrStoreFile)
	{
	iPdrStoreFile = aPdrStoreFile;
	}

EXPORT_C void PdrModelStore::AddModel(PrinterModelHeader *aModel)
/** Adds a printer model to the store.

@param aModel Printer model. */
	{
	iModelList.Add(aModel);
	}

EXPORT_C Record *PdrModelStore::FindModel(String& aLabel)
	{
	return iModelList.LabelToRecord(aLabel);
	}

EXPORT_C void PdrModelStore::AddTypefaceFonts(TypefaceFonts *aTypefaceFonts)
/** Adds typeface information to the store.

@param aTypefaceFonts Typeface fonts. */
	{
	iTypefaceFontsList.Add(aTypefaceFonts);
	}

EXPORT_C Record *PdrModelStore::FindTypefaceFonts(String& aLabel)
	{
	return iTypefaceFontsList.LabelToRecord(aLabel);
	}

EXPORT_C void PdrModelStore::AddFontInfo(FontInfo *aFontInfo)
/** Adds font information to the store.

@param aFontInfo Font information. */
	{
	iFontInfoList.Add(aFontInfo);
	}

EXPORT_C Record *PdrModelStore::FindFontInfo(String& aLabel)
	{
	return iFontInfoList.LabelToRecord(aLabel);
	}

EXPORT_C void PdrModelStore::AddResources(PdrResources *aResources)
	{
	iResourcesList.Add(aResources);
	}

EXPORT_C Record *PdrModelStore::FindResources(String& aLabel)
	{
	return iResourcesList.LabelToRecord(aLabel);
	}

EXPORT_C void PdrModelStore::AddTranslates(PdrTranslates *aTranslates)
/** Adds printer translations to the store.

@param aTranslates Translation list. */
	{
	iTranslatesList.Add(aTranslates);
	}

EXPORT_C Record *PdrModelStore::FindTranslates(String& aLabel)
	{
	return iTranslatesList.LabelToRecord(aLabel); 
	}

	/*
void PdrModelStore::AddExtraInfo(PdrExtraInfo *aInfo)
	{
	iExtraInfoList.Add(aInfo);
	}

Record* PdrModelStore::FindExtraInfo(String& aLabel)
	{
	return iExtraInfoList.LabelToRecord(aLabel);
	}
*/

PdrModelStore::PdrModelStore()
 :	iPdrStoreFile(NULL),
	iModelList(),
	iTypefaceFontsList(),
	iFontInfoList(),
	iResourcesList()//,
//	iExtraInfoList()
/** Default constructor. */
	{
	}

PdrModelStore::~PdrModelStore()
/** Default destructor. */
	{
	delete iPdrStoreFile;
	iModelList.Destroy();
	iTypefaceFontsList.Destroy();
	iFontInfoList.Destroy();
	iResourcesList.Destroy();
	iTranslatesList.Destroy();
//	iExtraInfoList.Destroy();
	}
