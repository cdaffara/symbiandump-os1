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
* Header PDRREADR.H
*
*/


#ifndef __PDRREADR_H__
#define __PDRREADR_H__

#include "LEXICAL.H"
#include "PDRRECRD.H"
#include "READER.H"
/**
Resource identifiers
@internalComponent
*/
const String IdentResources("Resources");
const String IdentTranslates("Translates");
const String IdentFontInfo("FontInfo");
const String IdentTypefaceFonts("TypefaceFonts");
const String IdentModel("Model");
const String IdentPdrStoreFile("PdrStoreFile");
//const String IdentExtraInfo("ExtraInfo");
/**
Resources identifiers
@internalComponent
*/
const String IdentEndResources("EndResources");

/**
Translates identifiers
@internalComponent
*/
const String IdentEndTranslates("EndTranslates");
/**
Codesection identifiers
@internalComponent
*/
const String IdentEndCodeSection("EndCodeSection");

/**
Fontinfo identifiers
@internalComponent
*/
const String IdentEndFontInfo("EndFontInfo");
const String IdentAscent("Ascent");
const String IdentMaxNormalCharWidth("MaxNormalCharWidth");
const String IdentCodeSection("CodeSection");

/**
Fontheight identifiers
@internalComponent
*/
const String IdentEndFontHeight("EndFontHeight");
const String IdentHeight("Height");
const String IdentWidthScale("WidthScale");
const String IdentNormal("Normal");
const String IdentBold("Bold");
const String IdentItalic("Italic");
const String IdentBoldItalic("BoldItalic");
const String IdentCommand("Command");

/**
Scalable fontheight identifiers
@internalComponent
*/
const String IdentEndScalableFontHeight("EndScalableFontHeight");
const String IdentHeightMin("HeightMin");
const String IdentHeightMax("HeightMax");
const String IdentHeightDelta("HeightDelta");
    // uses FontHeight IdentNormal etc
/**
Typefacefonts identifiers
@internalComponent
*/
const String IdentEndTypefaceFonts("EndTypefaceFonts");
const String IdentTypefaceTranslates("Translates");
const String IdentTypefaceName("Name");
const String IdentProportional("Proportional");
const String IdentSerif("Serif");
const String IdentSymbol("Symbol");
const String IdentFontHeight("FontHeight");
const String IdentScalableFontHeight("ScalableFontHeight");

// Extra info identifiers
//const String IdentEndExtraInfo("EndExtraInfo");
/**
Model identifiers
@internalComponent
*/
const String IdentEndModel("EndModel");
const String IdentModelName("Name");
const String IdentRequiresPrinterPort("RequiresPrinterPort");
const String IdentModelFlags("Flags");
const String IdentModelUid("Uid");
const String IdentModelResources("Resources");
const String IdentKPixelWidth("KPixelWidth");
const String IdentKPixelHeight("KPixelHeight");
const String IdentPortraitOffset("PortraitOffset");
const String IdentLandscapeOffset("LandscapeOffset");
const String IdentMinMarginLeft("MinMarginLeft");
const String IdentMinMarginRight("MinMarginRight");
const String IdentMinMarginTop("MinMarginTop");
const String IdentMinMarginBottom("MinMarginBottom");
const String IdentDisplayMode("DisplayMode");
const String IdentTypefaceFontss("TypefaceFontss");
const String IdentSpareRecord("SpareRecord");

/**
Typefacefontsentry identifiers
@internalComponent
*/
const String IdentEndTypefaceFontss("EndTypefaceFontss");
const String IdentNotInPortrait("NotInPortrait");
const String IdentNotInLandscape("NotInLandscape");

/**
Pdrstorefile identifiers
@internalComponent
*/
const String IdentEndPdrStoreFile("EndPdrStoreFile");
const String IdentPDLName("PDLName");
const String IdentPDLUid("PDLUid");
const String IdentModels("Models");
const String IdentEndModels("EndModels");

class PdrReader : public Reader
/**
@internalComponent
*/
	{
public:
	IMPORT_C PdrReader();
	IMPORT_C boolean Read(const String& aFilename);
	IMPORT_C ~PdrReader();
	boolean ReadResources();
	boolean ReadTranslates();
	boolean ReadCodeSection(int aCode);
	boolean ReadFontInfo();
	boolean ReadStyle();
	boolean ReadFontHeight();
	boolean ReadScalableFontHeight();
	boolean ReadTypefaceFonts();
	boolean ReadModel();
	boolean ReadPdrStoreFile();
	IMPORT_C boolean Store(const String& aFilename);
	boolean Command(String& aCommand);
//	boolean ReadExtraInfo();
protected:
	PdrModelStore iPdrModelStore;
	PdrStoreFile* iPdrStoreFile;
	PdrResources* iResources;
	PdrTranslates* iTranslates;
	WidthsCodeSection* iCodeSection;
	FontInfo* iFontInfo;
	StyleIndex iIndex;
	PdrFontHeight* iFontHeight;
	TypefaceFonts* iTypefaceFonts;
	PrinterModelHeader* iModel;
	};

#endif
