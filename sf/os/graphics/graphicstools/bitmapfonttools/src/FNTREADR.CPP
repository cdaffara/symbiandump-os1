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
* Header FNTREADR.CPP
*
*/


#include "FNTREADR.H"

const int KMarkFirstCharacterInTypeface = -1;
const int KBytesAddedForMetricIndex = 2;
const int KBytesForIndexForFillCharacters = 2;
const int KReportRateFrequencyInPercent = 10;

// class CroppedValues

class CroppedValues
	{
public:
	CroppedValues();
public:
	int iTopCrop;
	int iBottomCrop;
	int iLeftCrop;
	int iRightCrop;
	};

CroppedValues::CroppedValues()
 :	iTopCrop(0), iBottomCrop(0), iLeftCrop(0), iRightCrop(0)
	{}

// class FontReader

FontReader::FontReader()
 :	Reader(),
	iFontStore(),
	iFontStoreFile(NULL),
	iCharacterMetrics(NULL),
	iCodeSection(NULL),
	iFontBitmap(NULL),
	iTypeface(NULL),
	iReadFileFormat(ESymbianGDFFormat),
	iBitmapWidth(0),
	iBitmapHeight(0),
	iDefaultXMoveInPixels(KUndefinedInteger),
	iDefaultYMoveInPixels(KUndefinedInteger)
	{
	}

boolean FontReader::Read(const String& aFilename)
	{
	iFileName = aFilename;
	boolean state = Open(iFileName.Text());

	while (!_EOF() && state)
		{
		if (IdentComp(IdentBDFFileHeader) || IdentComp(IdentBDFComment))
			{
			state = ReadBDFFontBitmap();
			iReadFileFormat = EBDFFormat;
			}
		else if (IdentComp(IdentTypeface))
			state = ReadTypeface();
		else if (IdentComp(IdentFontStoreFile))
			state = ReadFontStoreFile();
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

boolean FontReader::ReadMetricFromBDFCharacter(CharacterMetrics* aCharacterMetrics, CroppedValues* aCropped=  NULL)
	{
	int xMoveInPixels = 0;
	int yMoveInPixels = 0;
	int bitmapXOffset = 0;
	int bitmapYOffset = 0;

	boolean state = true;

	if (iDefaultXMoveInPixels != KUndefinedInteger)
		xMoveInPixels = iDefaultXMoveInPixels;
	if (iDefaultYMoveInPixels != KUndefinedInteger)
		yMoveInPixels = iDefaultYMoveInPixels;
	
	while (!IdentComp(IdentBDFEndChar) && !_EOF() && state)
		{
		iLexAnal->ReadNextLine(); // Skip to next line
		if (iLex->iType == ELexIdent)
			{
			if (IdentComp(IdentBDFCursorMove))
				{
				state = Number(xMoveInPixels);
				if (state)
					state = Number(yMoveInPixels);
				}
			else if (IdentComp(IdentBDFBitmapSizeAndDisplacement))
				{
				state = Number(iBitmapWidth);
				if (state)
					{
					state = Number(iBitmapHeight);
					state = Number(bitmapXOffset);
					state = Number(bitmapYOffset);
					}
				}
			else if (IdentComp(IdentBDFStartBitmap) && state)
				{
				int line = 0;
				for (line = 0; line < iBitmapHeight; line++)
					{
					iLexAnal->ReadNextLine();	// Skip to next line
					int bits = 0;
					int bitOffset = 0;
					int byteValue = 0;
					for (bits = 0; bits < iBitmapWidth; bits++)
						{
						if (bitOffset == 0)
							{
							Lexical lex;
							strncpy(lex.iText, &iLexAnal->iLine[bits >> 2], 2);
							lex.iText[2] = '\0';
							byteValue = lex.CovertStringToHex();
							bitOffset = 8;
							}

						bitOffset--;
						int bitValue = (byteValue >> bitOffset) & 1;
						iBitArray[bits][line] = bitValue;
						}
					}
				}
			}
		else
			{
			Error("Fontbitmap identifier expected");
			state = efalse;
			}
		}

	if (state)
		state = NewLine();
//
// Make sure that bitmap is fully cropped
//
	int leftCrop = 0;
	int rightCrop = 0;
	int topCrop = 0;
	int bottomCrop = 0;
	
	int line = 0;
	while (BitmapLineEmpty(line) && line < iBitmapHeight)
		{
		topCrop++;;
		line++;
		}
	line = iBitmapHeight - 1;
	while (BitmapLineEmpty(line) && line > topCrop)
		{
		bottomCrop++;
		line--;
		}
	int column = iBitmapWidth - 1;
	while (BitmapColumnEmpty(column) && column >= 0)
		{
		rightCrop++;
		column--;
		}
	column = 0;
	while (BitmapColumnEmpty(column) && column < iBitmapWidth - 1 - rightCrop)
		{
		leftCrop++;
		column++;
		}

	int croppedBitmapHeight = iBitmapHeight - topCrop - bottomCrop;
	int croppedBitmapWidth = iBitmapWidth - leftCrop - rightCrop;
	int croppedLeftAdjust = bitmapXOffset + leftCrop;
	int croppedRightAdjust = (xMoveInPixels - croppedLeftAdjust - croppedBitmapWidth);
	int croppedAscent = croppedBitmapHeight + bitmapYOffset + bottomCrop;

	if(state)
		{
		aCharacterMetrics->iAscentInPixels = (chardim) croppedAscent;
		aCharacterMetrics->iHeightInPixels = (chardim) croppedBitmapHeight;
		aCharacterMetrics->iLeftAdjustInPixels = (chardim) croppedLeftAdjust;
		aCharacterMetrics->iMoveInPixels = (chardim) xMoveInPixels;
		aCharacterMetrics->iRightAdjustInPixels = (chardim) croppedRightAdjust;
		if (aCropped)
			{
			aCropped->iBottomCrop = bottomCrop;
			aCropped->iLeftCrop = leftCrop;
			aCropped->iRightCrop = rightCrop;
			aCropped->iTopCrop = topCrop;
			}
		}
	return state;
	}

boolean FontReader::ReadBDFCharacter(int /*aCode*/)
	{
	boolean state = etrue;
	ObjectList<String*> character;
	iCharacterMetrics = new CharacterMetrics;
	CroppedValues* croppedValues = new CroppedValues;

	state = ReadMetricFromBDFCharacter(iCharacterMetrics, croppedValues);

	if (state)
		{
		BitmapOffset* offset = new BitmapOffset((uint16)iCodeSection->iCharactersBitmap.iByteList.Length());
		
		
		iCodeSection->iCharactersBitmap.iByteList.NewByte();
		iCodeSection->iCharacters.iBitmapOffsetList.Add(offset);

		int index = iFontBitmap->iCharacterMetrics->Index(*iCharacterMetrics);
		if (index == -1)
			{
			Error("Internal Compiler Error");
			state = 0;
			}
		delete iCharacterMetrics;

		if (state)
			{
			iCodeSection->iCharactersBitmap.AddIndex(index);

			int line = croppedValues->iTopCrop;
			boolean repeatLines;
			int countLines = 0;
			const int bottomCrop = croppedValues->iBottomCrop;
			while (line < (iBitmapHeight - bottomCrop))
				{
				if ((line + 1) == (iBitmapHeight - bottomCrop))
					{
					repeatLines = efalse;
					countLines = 1;
					}
				else if (!CompareBitmapLines(line, line + 1))
					{
					repeatLines = efalse;
					for (countLines = 2; ((line + countLines) < (iBitmapHeight - bottomCrop)) && (countLines < KMaxNumberRepeatedLines); countLines++)
						{
						if (CompareBitmapLines(line + countLines - 1, line + countLines))
							break;
						}
					}
				else
					{
					repeatLines = etrue;
					for (countLines = 2; ((line + countLines) < (iBitmapHeight - bottomCrop)) && (countLines < KMaxNumberRepeatedLines); countLines++)
						{
						if (!CompareBitmapLines(line, line + countLines))
							break;
						}
					}
				char bit;
				if (repeatLines)
					bit = 0;
				else
					bit = 1;
				iCodeSection->iCharactersBitmap.iByteList.AddBit(bit);
				for (int digit = 0; digit < 4; digit++)
					{
					bit = char(countLines >> digit);
					iCodeSection->iCharactersBitmap.iByteList.AddBit(bit);
					}
				int lineFromTop;
				for (lineFromTop = line; lineFromTop < (line + countLines); lineFromTop++)
					{
					if ((!repeatLines) || (lineFromTop == line))
						{
						int column;
						for (column = croppedValues->iLeftCrop;
							column < iBitmapWidth - croppedValues->iRightCrop; column++)
							{
							if (iBitArray[column][lineFromTop] == 1)
								bit = 1;
							else
								bit = 0;
							iCodeSection->iCharactersBitmap.iByteList.AddBit(bit);
							}
						}
					}
				line = line+countLines;
				}
			}
		}
	delete croppedValues;
	return state;
	}

boolean FontReader::ReadBDFChars(const int aNumberOfGlyphsInFile, const int aMaxConsecutiveFillChars)
	{
	boolean state = etrue;
	boolean newCodeSection = etrue;
	int currentEncodingValue = 0; // each glyph has an unique encoding value
	int lastEncodingValue = KMarkFirstCharacterInTypeface;
	const int maxCharacterBitmapsize =
		((KMaxBitmapWidth * KMaxBitmapHeight) / KNumberOfBitsInByte) + KBytesAddedForMetricIndex + KBytesForIndexForFillCharacters;

	iCodeSection = NULL;
	for (int numberGlyphsRead = 0; state && numberGlyphsRead < aNumberOfGlyphsInFile && !_EOF(); numberGlyphsRead++)
		{
		if (state && iCodeSection && iCodeSection->iCharactersBitmap.iByteList.Length() + maxCharacterBitmapsize >= KMaxSizeCodeSectionBitmap)
			{
			newCodeSection = etrue;
			}

		state = IdentComp(IdentBDFCharLabel);
		if (!state)
			ErrorIdentifierExpected(IdentBDFCharLabel);
		if (state)
			iLexAnal->ReadNextLine(); // Skip to next line

 		if (IdentComp(IdentBDFChar))
			{
			state = Number(currentEncodingValue);
			}
		else
			{
			state = efalse;
			ErrorIdentifierExpected(IdentBDFChar);
			}

		if (KLowestPermittedCharacterEncoding > currentEncodingValue ||
			KHighestPermittedCharacterEncoding < currentEncodingValue)
			{
			while (!IdentComp(IdentBDFEndChar) && !_EOF() && state) 
				// Skip fill character.
				{
				iLexAnal->ReadNextLine(); // Skip to next line
				}
			iLexAnal->ReadNextLine();
			continue;
			}

		if (!iCodeSection || state && (currentEncodingValue > (lastEncodingValue + 1 + aMaxConsecutiveFillChars)))
			{
			// First character skip to new code section
			newCodeSection = etrue;
			}
		else if (state && !newCodeSection && (currentEncodingValue > lastEncodingValue + 1))
			{
			WriteFillCharacters(currentEncodingValue - (lastEncodingValue + 1));
			}
		else if (state && currentEncodingValue <= lastEncodingValue)
			{
			Error("CodeSection out of sequence");
			state = efalse;
			}

		if (state && newCodeSection)
			{
			if (iCodeSection)
				{
				iCodeSection->iEnd = (uint16) lastEncodingValue;
				iFontBitmap->iCodeSectionList.Add(iCodeSection);
				PrintoutCodeSection(iCodeSection);
				}
			iCodeSection = new BitmapCodeSection;
			iCodeSection->iStart = (uint16) currentEncodingValue;
			}

		if (state)
			state = ReadBDFCharacter(currentEncodingValue);

		newCodeSection = efalse;
		lastEncodingValue = currentEncodingValue;
		}

	if (state)
		{
		iCodeSection->iEnd = (uint16) lastEncodingValue;
		iFontBitmap->iCodeSectionList.Add(iCodeSection);
		PrintoutCodeSection(iCodeSection);
		}

	return state;
	}

void FontReader::PrintoutCodeSection(const BitmapCodeSection* aCodeSection) const
	{
	cout << hex << "Codesection 0x" << aCodeSection->iStart << ": 0x" << aCodeSection->iEnd << " read" << endl;
	cout.flush();
	}

boolean FontReader::ReadBDFFontBitmap()
	{
	boolean state = etrue;
	String label;
	iFontBitmap = new FontBitmap;
	int pointSize;
	int xresolution; // in dots per inch
	int yresolution; // in dots per inch
	int widthBoundingBox = 0; // In pixels
	int heightBoundingBox = 0; // in pixels
	int boundingBoxXOffset = 0; // From origin (cursor position at baseline)
	int boundingBoxYOffset = 0;	// From origin (cursor position at baseline)
	int numberChars = 0;
	String fontLabel;
	int maxNormalCharWidth = KUndefinedInteger;
	uid fontUid = KNullUid;
	int posture = PostureUpright;
	int strokeWeight = StrokeWeightNormal;
	int defaultXMoveInPixels = KUndefinedInteger;
	int defaultYMoveInPixels = KUndefinedInteger;
	int writingDirection = 0;
	int maxConsecutiveFillChars = 0;	// Max permitted "fill" characters with zero size to prevent
										// break between code sections.
	int fontAscent = 0;
	int fontDescent = 0;

	while (!IdentComp(IdentBDFNumChars) && !_EOF() && state)
		{
		iLexAnal->ReadNextLine(); // Skip to next line
		if (iLex->iType == ELexIdent)
			{
			if (IdentComp(IdentBDFFontBitmap))
				{
				state = IdentCopy(fontLabel);
				}
			if (IdentComp(IdentBDFPointSize))
				{
				state = Number(pointSize);
				if (state)
					state = Number(xresolution);
				if (state)
					state = Number(yresolution);
				}
			else if (IdentComp(IdentBDFFontDesignBox))
				{
				state = Number(widthBoundingBox);
				if (state)
					state = Number(heightBoundingBox);
				if (state)
					state = Number(boundingBoxXOffset);
				if (state)
					state = Number(boundingBoxYOffset);
				}
			else if (IdentComp(IdentBDFWritingDirection))
				{
				Number(writingDirection);
				if (writingDirection != 0)
					cout << "Warning: Only left to right writing direction supported by EPOC32";
				}
			else if (IdentComp(IdentBDFCursorMove))
				{
				state = Number(defaultXMoveInPixels);
				if (state)
					state = Number(defaultYMoveInPixels);
				}
			else if (IdentComp(IdentBDFStartProperties))	// Adding additional properties
				{
				int numberOfProperties;
				state = Number(numberOfProperties);
				if (state)
					state = NewLine();
				{for (int properties = 0; properties < numberOfProperties && !_EOF() && state; properties++)
					{
					if (IdentComp(IdentBDFPropertyUid) || IdentComp(IdentUid))
						state = Number(fontUid);
					else if (IdentComp(IdentBDFPropertyBold) || IdentComp(IdentBold))
						state = Number(strokeWeight);
					else if (IdentComp(IdentBDFPropertyItalic) || IdentComp(IdentItalic))
						state = Number(posture);
					else if (IdentComp(IdentBDFPropertyFontAscent))
						state = Number(fontAscent);
					else if (IdentComp(IdentBDFPropertyFontDescent))
						state = Number(fontDescent);
					else if	(IdentComp(IdentBDFPropertyMaxNormalCharWidth) || IdentComp(IdentMaxNormalCharWidth))
						state = Number(maxNormalCharWidth);
					else if (IdentComp(IdentBDFPropertyMaxConsecutiveFillChars) || IdentComp(IdentMaxConsecutiveFillChars))
						state = Number(maxConsecutiveFillChars);

					iLexAnal->ReadNextLine();	// Skip to next line
					}
				}
				if (state)
					{
					state = IdentComp(IdentBDFEndProperties);
					if (!state)
						ErrorIdentifierExpected(IdentBDFEndProperties);
					}
				}

			}
		else
			{
			Error("Identifier expected");
			state = efalse;
			}
		}
//
// Check that maximum size of bitmap glyph not exceeded.
//
	if (widthBoundingBox>KMaxBitmapWidth || widthBoundingBox>KMaxBitmapWidth)
		{
		state = efalse;
		cout << "Error: A font bounding box dimension exceeds maximum permitted in EPOC32";
		}

// Check that have everthing that we need and set some bitmap properties.

	iFontBitmap->iMaxCharWidthInPixels = (chardim) widthBoundingBox;
	if (fontAscent + fontDescent)
		{
		iFontBitmap->iCellHeightInPixels =( chardim) (fontAscent+fontDescent);
		iFontBitmap->iAscentInPixels = (chardim) (fontAscent);
		}
	else
		{	// Old file so use the old calculation
		iFontBitmap->iCellHeightInPixels =( chardim) heightBoundingBox;
		iFontBitmap->iAscentInPixels = (chardim) (heightBoundingBox + boundingBoxYOffset);
		}
	iFontBitmap->iPosture = (boolean) posture;
	iFontBitmap->iStrokeWeight =( boolean) strokeWeight;
	iFontBitmap->iLabel = fontLabel;

	iDefaultXMoveInPixels = defaultXMoveInPixels;
	iDefaultYMoveInPixels = defaultYMoveInPixels;

	if (fontUid != KNullUid)
		iFontBitmap->iUid = fontUid;
	else
		{
		cerr << "Warning: No font bitmap UID specified in properties\n";
		iFontBitmap->iUid = rand();
		}

	if (maxNormalCharWidth != KUndefinedInteger)
		iFontBitmap->iMaxNormalCharWidthInPixels = (chardim) maxNormalCharWidth;
	else
		{
		cerr << "Warning: No Maximum Normal Character Width specified in properties\n";
		iFontBitmap->iMaxNormalCharWidthInPixels = iFontBitmap->iMaxCharWidthInPixels;
		}

	if (state)
		state = Number(numberChars);
	if (state)
		state = NewLine();
	else
		return state;

	// store position of Lex here and then set it back for the reading of the BDF chars!
	int saveLineNo = iLexAnal->iLineNo;
	
	ParseMetricsFromBDF(numberChars, maxConsecutiveFillChars);
	// now reset the lexical
	state = Open(iFileName.Text());
	do
		{
		iLexAnal->Read();
		}
	while(saveLineNo > iLexAnal->iLineNo);

	ReadBDFChars(numberChars, maxConsecutiveFillChars);

	if (state)
		{
		state = IdentComp(IdentBDFEndFontBitmap);
		if (!state)
			Error("ENDFONT identifier expected");
		}

	int globalMove = KUndefinedInteger;
	int monospaced= etrue;
	if (state)
		{
		for (int i = 0; i <iFontBitmap->iCodeSectionList.Size(); i++)
			{
			const int end = iFontBitmap->iCharacterMetrics->iCharacterMetricsList.Size();
			for (int j = 0; j< end; j++)
				{
				int move = iFontBitmap->iCharacterMetrics->iCharacterMetricsList[j]->Metric()->iMoveInPixels;
				if (globalMove == KUndefinedInteger)
					globalMove = move;
				if (move > iFontBitmap->iMaxCharWidthInPixels)
					iFontBitmap->iMaxCharWidthInPixels = (chardim) move;
				if (globalMove!= move)
					monospaced = efalse;
				}
			}
		}
	if (monospaced)
		iFontBitmap->iIsProportional = efalse;
	else
		iFontBitmap->iIsProportional = etrue;

	if (state)
		{
		iFontStore.AddFontBitmap(iFontBitmap);
		cout << "FONT read\n";
		}
	else
		delete iFontBitmap;
	return state;
	}

boolean FontReader::ReadTypeface()
	{
	boolean state = etrue;
	iTypeface = new FntTypeface;
	String label;
	int num;
	state = IdentCopy(iTypeface->iLabel);
	if (state)
		state = NewLine();
	while (!IdentComp(IdentEndTypeface) && !_EOF() && state)
		{
		if (IdentComp(IdentName))
			{
			state = StringCopy(iTypeface->iName);
			while ((iLex->iType != ELexNL) && !_EOF() && state)
				{
				if (IdentComp(IdentTypefaceProportional))
					iTypeface->iFlags = (boolean) (iTypeface->iFlags | Proportional);
				else if (IdentComp(IdentSerif))
					iTypeface->iFlags = (boolean) (iTypeface->iFlags | Serif);
				else if (IdentComp(IdentSymbol))
					iTypeface->iFlags = (boolean) (iTypeface->iFlags | Symbol);
				else
					{ 
					Error("Typeface identifier or newline expected");
					state = efalse;
					}
				}
			}
		else if (IdentComp(IdentFontBitmaps))
			{
			state = NewLine();
			while (!IdentComp(IdentEndFontBitmaps) && !_EOF() && state)
				{
				TypefaceFontBitmap* typefacefontbitmap = NULL;
				if (iLex->iType == ELexIdent)
					{
					state = IdentCopy(label);
					FontBitmap* fontbitmap = (FontBitmap*) iFontStore.FindFontBitmap(label);
					if (fontbitmap)
						{
						typefacefontbitmap = new TypefaceFontBitmap(fontbitmap);
						state = etrue;
						}
					else
						{
						Error("Fontbitmap not found");
						state = efalse;
						}
					}
				else
					{
					uid fontbitmapuid;
					state = Number(fontbitmapuid);
					if (state)
						typefacefontbitmap = new TypefaceFontBitmap(fontbitmapuid);
					}
				if (state)
					{
					while ((iLex->iType != ELexNL) && !_EOF() && state)
						{
						if (IdentComp(IdentWidthFactor))
							{
							state = Number(num);
							typefacefontbitmap->iWidthFactor = (char) num;
							}
						else if (IdentComp(IdentHeightFactor))
							{
							state = Number(num);
							typefacefontbitmap->iHeightFactor =( char) num;
							}
						else
							{ 
							Error("Typefacefontbitmap identifier or newline expected");
							state = efalse;
							}
						}
					if (state)
						iTypeface->iTypefaceFontBitmapList.Add(typefacefontbitmap);
					else
						delete typefacefontbitmap;
					}
				if (state)
					state = NewLine();
				}
			}
		else
			{
			Error("Typeface identifier expected");
			state = efalse;
			}
		if (state)
			state = NewLine();
		}
	if (state)
		{
		iFontStore.AddTypeface(iTypeface);
		cout << "Typeface read\n";
		}	
	else
		delete iTypeface;
	return state;
	}

boolean FontReader::ReadFontStoreFile()
	{
	boolean state = etrue;
	int num;
	if (iFontStoreFile)
		{
		state = efalse;
		Error("Fontstorefile already read");
		}
	else
		{
		iFontStoreFile = new FontStoreFile;
		String label;
		Record* typeface;
		state = NewLine();
		while (!IdentComp(IdentEndFontStoreFile) && !_EOF() && state)
			{
			if (IdentComp(IdentCollectionUid))
				{
				state = Number(iFontStoreFile->iCollectionUid);
				}
			else if (IdentComp(IdentKPixelAspectRatio))
				{
				state = Number(num);
				if (state)
					iFontStoreFile->iKPixelAspectRatio = num;
				}
			else if (IdentComp(IdentCopyrightInfo))
				{
				state = NewLine();
				while (!IdentComp(IdentEndCopyrightInfo) && !_EOF() && state)
					{
					String* string = new String;
					state = StringCopy(*string);
					if (state)
						iFontStoreFile->iCopyrightInfo.Add(string);
					else
						delete string;
					state = NewLine();
					}
				}
			else if (IdentComp(IdentTypefaces))
				{
				state = NewLine();
				while (!IdentComp(IdentEndTypefaces) && !_EOF() && state)
					{
					state =IdentCopy(label);
					if (state)
						{
						typeface = iFontStore.FindTypeface(label);
						if (typeface)
							{
							iFontStoreFile->AddTypeface((FntTypeface*) typeface);
							}
						else
							{
							Error("Typeface not found");
							state = efalse;
							}
						}
					if (state)
						state = NewLine();
					}
				}
			else if (IdentComp(IdentExtraFontBitmaps))
				{
				state = NewLine();
				while (!IdentComp(IdentEndExtraFontBitmaps) && !_EOF() && state)
					{
					state = IdentCopy(label);
					FontBitmap* fontbitmap = (FontBitmap*) iFontStore.FindFontBitmap(label);
					if (fontbitmap)
						{
						iFontStoreFile->AddFontBitmap((FontBitmap*) fontbitmap);
						}
					else
						{
						Error("Fontbitmap not found");
						state = efalse;
						}
					if (state)
						state = NewLine();
					}
				}
			else
				{
				Error("Fontstorefile identifier expected");
				state = efalse;
				}
			if (state)
				state = NewLine();
			}
		if (state)
			{
			iFontStore.AddFontStoreFile(iFontStoreFile);
			cout << "Fontstorefile read\n";
			}
		else
			delete iFontStoreFile;
		}
	return state;
	}

int FontReader::Store(const String& aFilename)
	{
	boolean state = etrue;
	if (!iFontStoreFile)
		{
		state = efalse;
		Error("No fontstore file record");
		}
	else
		state = iFontStore.Store(aFilename);
	return state;
	}

boolean FontReader::CharLine(String& aCharLine)
	{
	boolean state = etrue;
	while (state && (iLex->iType != ELexNL))
		{
		char ch;
		state = Operator(ch);
		if (state)
			{
			if ((ch == '.') || (ch == '*'))
				aCharLine += ch;
			else
				{
				state = efalse;
				Error("Operator '.' or '*' expected");
				}
			}
		}
	return state;
	}

void FontReader::ErrorIdentifierExpected(const String& aIdentifier)
	{
	cerr << "Error: Identifier Expected " << aIdentifier;
	iLexAnal->Report();
	while ((iLex->iType != ELexNL) && (iLex->iType != ELexEOF))
		iLexAnal->Read();
	}

boolean FontReader::CompareBitmapLines(int aLine1, int aLine2)
	{

	int column = 0;
	boolean identical = etrue;
	for (column=0; column < iBitmapWidth; column++)
		{
		if (iBitArray[column][aLine1] != iBitArray[column][aLine2])
			{
			identical = efalse;
			break;
			}
		}

	return identical;
	}

boolean FontReader::BitmapLineEmpty(int aLine)
	{

	int column = 0;
	boolean empty = etrue;
	for (column = 0; column < iBitmapWidth; column++)
		{
		if (iBitArray[column][aLine] != 0)
			{
			empty = efalse;
			break;
			}
		}

	return empty;
	}

boolean FontReader::BitmapColumnEmpty(int aColumn)
	{

	int line = 0;
	boolean empty = etrue;
	for (line = 0; line < iBitmapHeight; line++)
		{
		if (iBitArray[aColumn][line] != 0)
			{
			empty = efalse;
			break;
			}
		}

	return empty;
	}

void FontReader::WriteFillCharacters(int aNumberConsecutive)
	{
	for (int counter = 0; counter < aNumberConsecutive; counter++)
		{
		BitmapOffset* offset = new BitmapOffset(KFillCharacterOffset);
		iCodeSection->iCharacters.iBitmapOffsetList.Add(offset);
		}
	}

boolean FontReader::ParseMetricsFromBDF(int aNumberCharsInFile, int aMaxConsecutiveFillChars)
	{
	boolean state = etrue;
	int character = 0;
	int numberCharactersRead;
	int lastCharacter = KMarkFirstCharacterInTypeface;
	int reportRate = ((aNumberCharsInFile - 1) / KReportRateFrequencyInPercent) + 1;

	CharacterMetrics* metric = 0;
	
	cout << "Analysing character metrics...\n";
	cout.flush();

	for (numberCharactersRead = 0; numberCharactersRead < aNumberCharsInFile && !_EOF() && state; numberCharactersRead++)
		{
		state = IdentComp(IdentBDFCharLabel);
		if (!state)
			ErrorIdentifierExpected(IdentBDFCharLabel);
		if (state)
			iLexAnal->ReadNextLine();	// Skip to next line

		if (IdentComp(IdentBDFChar))
			{
			state = Number(character);
			}
		else
			{
			state = efalse;
			ErrorIdentifierExpected(IdentBDFChar);
			}

		if (character < KLowestPermittedCharacterEncoding || character > KHighestPermittedCharacterEncoding)
			{
			while (!IdentComp(IdentBDFEndChar) && !_EOF() && state) 
				// Skip fill character.
				{
				iLexAnal->ReadNextLine(); // Skip to next line
				}
				iLexAnal->ReadNextLine();
			continue;
			}

		if (character<lastCharacter)
			{
			Error("CodeSection out of sequence");
			state = efalse;
			}

		if ((character > lastCharacter + 1) 
			&& (character <= lastCharacter + 1 + aMaxConsecutiveFillChars) && state)
			{
			// Would result in fill characters being used if not at end of code section!
			metric = new CharacterMetrics;
			metric->iLeftAdjustInPixels = (chardim)0;
			metric->iMoveInPixels = (chardim)0;
			metric->iRightAdjustInPixels = (chardim)0;
			metric->iAscentInPixels = (chardim)0;
			metric->iHeightInPixels = (chardim)0;
			iFontBitmap->iCharacterMetrics->AddOrIncrementMetric(*metric);
			delete metric;
			metric = 0;
			}

		if (state)
			{
			metric = new CharacterMetrics;
			state = ReadMetricFromBDFCharacter(metric);
			iFontBitmap->iCharacterMetrics->AddOrIncrementMetric(*metric);
			delete metric;
			metric = 0;
			}
		
		lastCharacter = character;
		if(numberCharactersRead == ((numberCharactersRead / reportRate) * reportRate))
			{
			cout << "Done " << (numberCharactersRead * 100 / aNumberCharsInFile) << "% \n";
			cout.flush();
			}
		}

	if (state)
		{
		iFontBitmap->iCharacterMetrics->SortMetricsByFrequency();
		}
	return state;
	}
