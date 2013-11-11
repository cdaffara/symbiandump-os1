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
* Header PDRREADR.CPP
*
*/


#include "PDRREADR.H"

const int NumResources=34;

String IdentResource[NumResources] =
	{
	"Reset",
	"SetPageSize",
	"PreAmble",
	"PostAmble",
	"SetTextColor",	//  !!
	"BoldOn",
	"BoldOff",
	"ItalicOn",
	"ItalicOff",
	"UnderlineOn",
	"UnderlineOff",
	"StrikethroughOn",
	"StrikethroughOff",
	"NewPage",
	"Portrait",
	"Landscape",
	"SetXPos",
	"SetYPos",
	"IncrementXPos",
	"IncrementYPos",
	"CarriageReturn",
	"SetGraphicsColor",	//  !!
	"BitmapStart",
	"BitmapEnd",
	"ScanLine",
	"EndScanLine",	//  !!
	"Resource1",
	"Resource2",
	"Resource3",
	"Resource4",
	"Resource5",
	"Resource6",
	"Resource7",
	"Resource8"
	};

const int NumDisplayModeValues = 11;

String IdentDisplayModeValue[NumDisplayModeValues] =
	{
	"None",
	"Gray2",
	"Gray4",
	"Gray16",
	"Gray256",
	"Color16",
	"Color256",
	"Color64K",
	"Color16M",
	"Rgb",
	"Color4K"
	};

EXPORT_C PdrReader::PdrReader()
 :	Reader(),
	iPdrModelStore(),
	iPdrStoreFile(NULL),
	iResources(NULL),
	iTranslates(NULL),
	iCodeSection(NULL),
	iFontInfo(NULL),
	iIndex(Normal),
	iFontHeight(NULL),
	iTypefaceFonts(NULL),
	iModel(NULL)
	{
	}

EXPORT_C boolean PdrReader::Read(const String& aFilename)
	{
	boolean state = Open(aFilename);

	while (!_EOF() && state)
		{
		if (IdentComp(IdentResources))
			state = ReadResources();
		else if (IdentComp(IdentTranslates))
			state = ReadTranslates();
		else if (IdentComp(IdentFontInfo))
			state = ReadFontInfo();
		else if (IdentComp(IdentTypefaceFonts))
			state = ReadTypefaceFonts();
//		else if (IdentComp(IdentExtraInfo))
//			state = ReadExtraInfo();
		else if (IdentComp(IdentModel))
			state = ReadModel();
		else if (IdentComp(IdentPdrStoreFile))
			state = ReadPdrStoreFile();
		else
			{
			Error("Resource identifier expected");
			state = efalse;
			}
		if (state)
			state = NewLine();
		}
	return state;
	}

EXPORT_C PdrReader::~PdrReader()
	{
	}

boolean PdrReader::ReadResources()
	{
	boolean state = etrue;
	iResources = PdrResources::New();
	state = IdentCopy(iResources->iLabel);
	if (state)
		state = NewLine();
	while (!IdentComp(IdentEndResources) && !_EOF() && state)
		{
   	    int i; // DEF102183: Graphics tools fail to build using MS VC8.
		for (i = 0; (i < NumResources) && !IdentComp(IdentResource[i]); i++)
			{	//	Tries to match resources identifier
			}
		if (i < NumResources)
			{
			PdrResource *resource = PdrResource::New();
			state = Command(resource->iString);
			resource->iId = i;
			if (state)
				{
				iResources->AddResource(resource);
				state = NewLine();
				}
			else
				resource->Delete();
			}
		else
			{
			state = efalse;
			Error("Resources identifier expected");
			}
		}
	if (state)
		{
		iPdrModelStore.AddResources(iResources);
		cout << "Resources read\n";
		}
	else 
		iResources->Delete();
	return state;
	}

boolean PdrReader::ReadTranslates()
	{
	int num;
	boolean state = etrue;
	iTranslates = PdrTranslates::New();
	state = IdentCopy(iTranslates->iLabel);
	if (state)
		state = NewLine();
	while (!IdentComp(IdentEndTranslates) && !_EOF() && state)
		{
		if (iLex->iType == ELexNumber)
			{
			PdrTranslation *translation = PdrTranslation::New();
			Number(num);
			translation->iFrom = uint16(num);
			char ch;
			state = Operator(ch);
			if (state)
				{
				state = (ch == ':');
				if (state)
					{
					if (iLex->iType == ELexNumber)
						{
						state = Number(num);
						translation->iTo += char(num);
						}
					else 
						{
						state = Command(translation->iTo);
						}
					if (state)
						state = NewLine();
					}
				else
					{
					Error("Operator ':' expected");
					}
				}
			if (state)
				iTranslates->AddTranslation(translation);
			else
				translation->Delete();
			}
		}
	if (state)
		{
		iPdrModelStore.AddTranslates(iTranslates);
		cout << "Translates read\n";
		}
	else 
		iTranslates->Delete();
	return state;
	}

boolean PdrReader::ReadCodeSection(int aCode)
	{
	boolean state = etrue;
	int code;
	int num;
	iCodeSection = WidthsCodeSection::New();
	char ch = 0;
	state = Number(num);
	if ((num<aCode) && state)
		{
		Error("CodeSection out of sequence");
		state = efalse;
		}
	if (state)
		{
		iCodeSection->iStart = uint16(num);
		state = Operator(ch);
		if (state)
			state = (ch == ':');
		if (state)
			{
			state = Number(num);
			iCodeSection->iEnd = uint16(num);
			state = NewLine();
			}
		else
			{
			state = efalse;
			Error("Operator ':' expected");
			}
		}
	else
		state = efalse;
	while (!IdentComp(IdentEndCodeSection) && !_EOF() && state)
		{
		if (iLex->iType != ELexNL)
			{
			state = Number(code);
			if ((code != iCodeSection->iStart + iCodeSection->NumWidths()) && state)
				{
				state = efalse;
				Error("Width out of sequence");
				}
			if (state)
				state = Operator(ch);
			if (state)
				state = (ch == ':');
			if (state)
				{
				Width *width = Width::New();
				if (Number(num))
					{
					width->iWidthInPixels = (uint16) num;
					iCodeSection->AddWidth(width);
					}
				else
					{
					state = efalse;
					width->Delete();
					}
				}
			}
		else
			state = NewLine();
		}
	if (state)
		{
		num = ((iCodeSection->iEnd + 1) - iCodeSection->iStart);
		if ((num != iCodeSection->NumWidths()) && (iCodeSection->NumWidths() != 1))
			{
			Error("Wrong number of widths in codesection");
			state = efalse;
			}
		}
	if (state)
		{
		iFontInfo->AddCodeSection(iCodeSection);
//		cout << "Codesection read\n";
		}
	else 
		iCodeSection->Delete();
	return state;
	}

boolean PdrReader::ReadFontInfo()
	{
	boolean state = etrue;
	iFontInfo = FontInfo::New();
	int num;
	state = IdentCopy(iFontInfo->iLabel);
	if (state)
		state = NewLine();
	while (!IdentComp(IdentEndFontInfo) && !_EOF() && state)
		{
		if (iLex->iType == ELexIdent)
			{
			if (IdentComp(IdentCodeSection))
				{
				int code = 0,size = iFontInfo->NumCodeSections();
				if (size)
					code = iFontInfo->CodeSectionList(size - 1)->iEnd + 1;
				state = ReadCodeSection(code);
				}
			else if (IdentComp(IdentAscent))
				{
				if (Number(num))
					iFontInfo->iAscentInPixels = uint16(num);
				else
					state = efalse;
				}
			else if (IdentComp(IdentMaxNormalCharWidth))
				{
				state = Number(num);
				if (state)
					iFontInfo->iMaxNormalCharWidthInPixels = uint16(num);
				}
			else
				{
				Error("Unrecognised fontinfo identifier");
				state = efalse;
				}
			}
		else
			{
			Error("Fontinfo identifier expected");
			state = efalse;
			}
		if (state)
			state = NewLine();
		}	  
	if (state)
		{
		iFontInfo->iMaxCharWidthInPixels = 0;
		for (int i = 0; i < iFontInfo->NumCodeSections(); i++)
			{
			WidthsCodeSection* codesection = iFontInfo->CodeSectionList(i);
			for (int j = 0; j < codesection->NumWidths(); j++)
				{
				int width = codesection->WidthList(j)->iWidthInPixels;
				if (width > iFontInfo->iMaxCharWidthInPixels)
					iFontInfo->iMaxCharWidthInPixels = (uint16) width;
				}
			}
		}
	if (state)
		{
		iPdrModelStore.AddFontInfo(iFontInfo);
		cout << "Fontinfo read\n";
		}
	else
		iFontInfo->Delete();
	return state;
	}

boolean PdrReader::ReadStyle()
	{
	boolean state = etrue;
	Record *fontinfo;
	PdrStyle *style;
	String label;
	if (!iTypefaceFonts->iIsScalable)
		style = &iFontHeight->iStyle[iIndex];
	else
		style = &iTypefaceFonts->iScalableFontHeight.iStyle[iIndex];
	style->iIsAvailable = etrue;
	state = IdentCopy(label);
	if (state)
		{
		fontinfo = iPdrModelStore.FindFontInfo(label);
		if (fontinfo)
			style->iFontInfo = fontinfo;
		else
			{
			Error("Fontinfo not found");
			state = efalse;
			}
		}
	return state;
	}

boolean PdrReader::ReadFontHeight()
	{
	boolean state = etrue;
	int num;
	if (iTypefaceFonts->iIsScalable == etrue)
		{
		state = efalse;
		Error("Scalablefontheight already defined");
		}
	else
		iFontHeight = PdrFontHeight::New();
	if (state)
		state = NewLine();

	while (!IdentComp(IdentEndFontHeight) && !_EOF() && state)
		{
		if (iLex->iType == ELexIdent)
			{
			if (IdentComp(IdentHeight))
				{
				if (Number(num))
					iFontHeight->iHeightInTwips = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentWidthScale))
				{
				if (Number(num))
					iFontHeight->iWidthScale = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentNormal))
				{
				iIndex = Normal;
				state = ReadStyle();
				}
			else if (IdentComp(IdentBold))
				{
				iIndex = Bold;
				state = ReadStyle();
				}
			else if (IdentComp(IdentItalic))
				{
				iIndex = Italic;
				state = ReadStyle();
				}
			else if (IdentComp(IdentBoldItalic))
				{
				iIndex = BoldItalic;
				state = ReadStyle();
				}
			else if (IdentComp(IdentCommand))
				{
				state = Command(iFontHeight->iCommandString);
				}
			else
				{
				Error("Unrecognised fontheight identifier");
				state = efalse;
				}
			}
		else
			{
			Error("Fontheight identifier expected");
			state = efalse;
			}
		if (state)
			state = NewLine();
		}
	if (state)
		{
		iTypefaceFonts->AddFontHeight(iFontHeight);
		cout << "Fontheight read\n";
		}
	else
		iFontHeight->Delete();
	return state;
	}

boolean PdrReader::ReadScalableFontHeight()
	{
	boolean state = etrue;
	int num;
	if (iTypefaceFonts->NumFontHeights())
		{
		state = efalse;
		Error("Non-scalable fontheights already defined");
		}
	else if (iTypefaceFonts->iIsScalable == etrue)
		{
		state = efalse;
		Error("Scalablefontheight already defined");
		}
	iTypefaceFonts->iIsScalable=etrue;
	if (state)
		state = NewLine();

	while (!IdentComp(IdentEndScalableFontHeight) && !_EOF() && state)
		{
		if (iLex->iType == ELexIdent)
			{
			if (IdentComp(IdentHeightMin))
				{
				if (Number(num))
					iTypefaceFonts->iScalableFontHeight.iHeightMinInTwips = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentHeightMax))
				{
				if (Number(num))
					iTypefaceFonts->iScalableFontHeight.iHeightMaxInTwips = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentHeightDelta))
				{
				if (Number(num))
					iTypefaceFonts->iScalableFontHeight.iHeightDeltaInTwips = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentNormal))
				{
				iIndex = Normal;
				state = ReadStyle();
				}
			else if (IdentComp(IdentBold))
				{
				iIndex = Bold;
				state = ReadStyle();
				}
			else if (IdentComp(IdentItalic))
				{
				iIndex = Italic;
				state = ReadStyle();
				}
			else if (IdentComp(IdentBoldItalic))
				{
				iIndex = BoldItalic;
				state = ReadStyle();
				}
			else if (IdentComp(IdentCommand))
				{
				state = Command(iTypefaceFonts->iScalableFontHeight.iCommandString);
				}
			else
				{
				Error("Unrecognised scalablefontheight identifier");
				state = efalse;
				}
			}
		else
			{
			Error("Scalablefontheight identifier expected");
			state = efalse;
			}
		if (state)
			state = NewLine();
		}	  
	if (state)
		{
		cout << "Scalablefontheight read\n";
		}
	else
		{
		if (!iTypefaceFonts->iIsScalable)
			iFontHeight->Delete();
		}
	return state;
	}

boolean PdrReader::ReadTypefaceFonts()
	{
	boolean state = etrue;
	Record* translates;
	String label;
	iTypefaceFonts = TypefaceFonts::New();
	state = IdentCopy(iTypefaceFonts->iLabel);
	if (state)
		state = NewLine();
	while (!IdentComp(IdentEndTypefaceFonts) && !_EOF() && state)
		{
		if (iLex->iType == ELexIdent)
			{
			if (IdentComp(IdentTypefaceName))
				{
				if (StringCopy(iTypefaceFonts->iTypeface.iName))
					while (iLex->iType != ELexNL)
					{	if (IdentComp(IdentProportional))
							iTypefaceFonts->iTypeface.iFlags = boolean(iTypefaceFonts->iTypeface.iFlags | Proportional);
						else if (IdentComp(IdentSerif))
							iTypefaceFonts->iTypeface.iFlags = boolean(iTypefaceFonts->iTypeface.iFlags | Serif);
						else if (IdentComp(IdentSymbol))
							iTypefaceFonts->iTypeface.iFlags = boolean(iTypefaceFonts->iTypeface.iFlags | Symbol);
						else
							{ 
							Error("Typefacefonts identifier or newline expected");
							state = efalse;
							}
					}
				else
					state = efalse;
				}
			else if (IdentComp(IdentTypefaceTranslates))
				{
				state = IdentCopy(label);
				if (state)
					{
					translates = iPdrModelStore.FindTranslates(label);
					if (translates)
						iTypefaceFonts->iTranslates = translates;
					else
						{
						Error("Translates not found");
						state = efalse;
						}
					}
				}
			else if (IdentComp(IdentFontHeight))
				{
				state = ReadFontHeight();
				}
			else if (IdentComp(IdentScalableFontHeight))
				{
				state = ReadScalableFontHeight();
				}
			else
				{
				Error("Unrecognised typefacefonts identifier");
				state = efalse;
				}
			}
		else
			{
			Error("Typefacefonts identifier expected");
			state = efalse;
			}
		if (state)
			state = NewLine();
		}	  
	if (state)
		{
		iPdrModelStore.AddTypefaceFonts(iTypefaceFonts);
		cout << "Typefacefonts read\n";
		}
	else
		iTypefaceFonts->Delete();
	return state;
	}

boolean PdrReader::ReadModel()
	{
	boolean state = etrue;
	int num;
	iModel = PrinterModelHeader::New();
	Record* resources;
//	Record* extrainfo;
	String label;
	state = IdentCopy(iModel->iLabel);
	if (state)
		state = NewLine();
	while (!IdentComp(IdentEndModel) && !_EOF() && state)
		{
		if (iLex->iType == ELexIdent)
			{
			if	(IdentComp(IdentModelName))
				{
				if (StringCopy(iModel->iEntry.iName))
					while (iLex->iType != ELexNL)
						{
						if (IdentComp(IdentRequiresPrinterPort))
							iModel->iEntry.iRequiresPrinterPort = etrue;
						else
							{
							Error("Model identifier or newline expected");
							state = efalse;
							}
						}
				else
					state = efalse;
				}
			else if (IdentComp(IdentModelUid))
				state = Number(iModel->iEntry.iUid);
			else if (IdentComp(IdentModelFlags))
				{
				state = Number(num);
				iModel->iInfo.iFlags = num;
				}
			else if (IdentComp(IdentModelResources))
				{
				state = IdentCopy(label);
				if (state)
					{
					resources = iPdrModelStore.FindResources(label);
					if (resources)
						iModel->iInfo.iResources = resources;
					else
						{
						Error("Resources not found");
						state = efalse;
						}
					}
				}
/*			else if (IdentComp(IdentSpareRecord))
				{
				state = IdentCopy(label);
				if (state)
					{
					extrainfo = iPdrModelStore.FindExtraInfo(label);
					if (extrainfo)
						iModel->iInfo.iSpareRecord = extrainfo;
					else
						{
						Error("Spare record not found");
						state = efalse;
						}
					}
				}
*/
			else if (IdentComp(IdentKPixelWidth))
				{
				if (Number(num))
					iModel->iInfo.iKPixelWidthInTwips = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentKPixelHeight))
				{
				if (Number(num))
					iModel->iInfo.iKPixelHeightInTwips = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentPortraitOffset))
				{
				if (Number(num))
					{
					iModel->iInfo.iPortraitOffsetInPixels.iX = num;
					if (Number(num))
						iModel->iInfo.iPortraitOffsetInPixels.iY = num;
					else
						state = efalse;
					}
				else
					state = efalse;
				}
			else if (IdentComp(IdentLandscapeOffset))
				{
				if (Number(num))
					{
					iModel->iInfo.iLandscapeOffsetInPixels.iX = num;
					if (Number(num))
						iModel->iInfo.iLandscapeOffsetInPixels.iY = num;
					else
						state = efalse;
					}
				else
					state = efalse;
				}
			else if (IdentComp(IdentMinMarginLeft))
				{
				if (Number(num))
					iModel->iInfo.iMinMarginsInPixels.iLeft = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentMinMarginRight))
				{
				if (Number(num))
					iModel->iInfo.iMinMarginsInPixels.iRight = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentMinMarginTop))
				{
				if (Number(num))
					iModel->iInfo.iMinMarginsInPixels.iTop = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentMinMarginBottom))
				{
				if (Number(num))
					iModel->iInfo.iMinMarginsInPixels.iBottom = num;
				else
					state = efalse;
				}
			else if (IdentComp(IdentDisplayMode))
				{
        	    int i; // DEF102183: Graphics tools fail to build using MS VC8.
				for (i = 0; (i < NumDisplayModeValues) && !IdentComp(IdentDisplayModeValue[i]); i++)
					{	// Tries to match display mode identifier
					}
				if (i < NumDisplayModeValues)
					{
					iModel->iInfo.iDisplayMode = i;
					}
				else
					{
					state = efalse;
					Error("Display mode identifier expected");
					}
				}
			else if (IdentComp(IdentTypefaceFontss))
				{
				state = NewLine();
				while (!IdentComp(IdentEndTypefaceFontss) && !_EOF() && state)
					{
					TypefaceFontsEntry* typefacefontsentry = NULL;
					if (iLex->iType == ELexIdent)
						{
						state = IdentCopy(label);
						Record* typefacefonts = iPdrModelStore.FindTypefaceFonts(label);
						if (typefacefonts)
							{
							typefacefontsentry = TypefaceFontsEntry::New(typefacefonts);
							state = etrue;
							}
						else
							{
							Error("Typefacefonts not found");
							state = efalse;
							}
						}
					if (state)
						{
						while ((iLex->iType != ELexNL) && !_EOF() && state)
							{
							if (IdentComp(IdentNotInPortrait))
								{
								typefacefontsentry->iNotInPortrait = etrue;
								}
							else if (IdentComp(IdentNotInLandscape))
								{
								typefacefontsentry->iNotInLandscape = etrue;
								}
							else
								{
								Error("Typefacefontsentry identifier or newline expected");
								state = efalse;
								}
							}
						if (state)
							iModel->iInfo.AddTypefaceFontsEntry(typefacefontsentry);
						else
							typefacefontsentry->Delete();
						}
					if (state)
						state = NewLine();
					}
				}
			else
				{
				Error("unrecognised model identifier");
				state = efalse;
				}
			}
		else
			{
			Error("Model identifier expected");
			state = efalse;
			}
		if (state)
			state = NewLine();
		}
	if (state)
		{
		iPdrModelStore.AddModel(iModel);
		cout << "Model read\n";
		}
	else
		iModel->Delete();
	return state;
	}

boolean PdrReader::ReadPdrStoreFile()
	{
	boolean state = etrue;
	if (iPdrStoreFile)
		{
		state = efalse;
		Error("Pdrstorefile already read");
		}
	else
		{
		iPdrStoreFile = PdrStoreFile::New();
		String label;
		Record* model;
		state = NewLine();
		while (!IdentComp(IdentEndPdrStoreFile) && !_EOF() && state)
			{
			if (IdentComp(IdentPDLName))
				{
				state = StringCopy(iPdrStoreFile->iPDLName);
				}
			else if (IdentComp(IdentPDLUid))
				{
				state = Number(iPdrStoreFile->iPDLUid);
				}
			else if (IdentComp(IdentModels))
				{
				state = NewLine();
				while (!IdentComp(IdentEndModels) && !_EOF() && state)
					{
					state = IdentCopy(label);
					if (state)
						{
						model = iPdrModelStore.FindModel(label);
						if (model)
							{
							iPdrStoreFile->AddModel((PrinterModelHeader*)model);
							}
						else
							{
							Error("model not found");
							state = efalse;
							}
						}
					if (state)
						state = NewLine();
					}
				}
			else
				{
				Error("Pdrstorefile identifier expected");
				state = efalse;
				}
			if (state)
				state = NewLine();
			}
		if (state)
			{
			iPdrModelStore.AddPdrStoreFile(iPdrStoreFile);
			cout << "Pdrstorefile read\n";
			}	
		else
			iPdrStoreFile->Delete();
		}
	return state;
	}

EXPORT_C boolean PdrReader::Store(const String& aFilename)
	{
	boolean state = etrue;
	if (!iPdrStoreFile)
		{
		state = efalse;
		Error("No pdrstore file record");
		}
	else
		state = iPdrModelStore.Store(aFilename);
	return state;
	}

boolean PdrReader::Command(String& aCommand)
	{
	boolean state = etrue;
	String string;
	state = StringCopy(string);
	int length = string.Length();
	for (int i = 0; i < length; i++)
		{
		char ch = string[i];
		if (ch == '<')	// Read control character
			{
			ch = 0;
			for (i = i + 1; (i < length) && (string[i] != '>'); i++)
				ch = char((ch * 10) + (string[i] - '0'));
			}
		aCommand += ch;
		}
	return state;
	}
/*
boolean PdrReader::ReadExtraInfo()
	{
	boolean state = etrue;
	String label;
	PdrExtraInfo* extrainfo = new PdrExtraInfo();
	state = IdentCopy(extrainfo->iLabel);
	if (state)
		state = NewLine();
	while (!IdentComp(IdentEndExtraInfo) && !_EOF() && state)
		{
		String* string = new String;
		state = Command(*string);
		if (state)
			extrainfo->iInfo.Add(string);
		else
			delete string;
		state = NewLine();
		}
	if (state)
		{
		iPdrModelStore.AddExtraInfo(extrainfo);
		cout << "Extra info read\n";
		}
	else
		delete extrainfo;
	return state;
	}
*/
