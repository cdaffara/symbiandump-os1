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

#ifndef __BMCONV_H__
#define __BMCONV_H__

#include <string.h>

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <fstream>
#include <iostream>
using namespace std;
#else //!__MSVCDOTNET__
#include <fstream.h>
#include <iostream.h>
#endif //__MSVCDOTNET__

#include <stdlib.h>

#ifdef __LINUX__
    #define GNUPACK __attribute__((packed))
    #define OPTCHAR '-'
    /* on Linux the '/' optchar cannot be supported as filenames can start with '/'. */
    #define ALTERNATE_OPTCHAR '-'
#else
    #define GNUPACK
    #define OPTCHAR '-' 
    #define ALTERNATE_OPTCHAR '/'
#endif


/**
@internalComponent
*/
const long int KWriteOnceFileStoreUid=268435511;
const long int KCBitwiseBitmapUid=268435520;
const long int KMultiBitmapRomImageUid=268435521;
const long int KMultiBitmapFileImageUid=268435522;
const long int KMultiBitmapFileImageChecksum=1194943545;
/**
@internalComponent
synchronized with enum SEpocBitmapHeader::TColor in \epoc32\include\bitmap.h
*/
enum TBitmapColor
	{
	EMonochromeBitmap = 0,
	EColorBitmap = 1,
	EColorBitmapAlpha =2 ,
    EColorBitmapAlphaPM = 3,	
	EColorBitmapUndefined = 8	
	};

/** 
Defines the types of file compression. 
@internalComponent
*/
enum TBitmapfileCompression
	{
	/** Bitmap file is not compressed. */
	ENoBitmapCompression=0,
	/** File is compressed using run-length encoding compression. */
	EByteRLECompression,
	/** File is compressed using twelve bit run-length encoding compression. */
	ETwelveBitRLECompression,
	/** File is compressed using sixteen bit run-length encoding compression. */
	ESixteenBitRLECompression,
	/** File is compressed using twenty four bit run-length encoding compression. */
	ETwentyFourBitRLECompression,
	/** File is compressed using unsigned thirty two bit run-length encoding compression. */
	EThirtyTwoUBitRLECompression,
	/** File is compressed using unsigned thirty two bit run-length encoding compression and includes an alpha channel. */
	EThirtyTwoABitRLECompression,
	/** Insert new compression types here */
	ERLECompressionLast = 255
	};
/**
@internalComponent
*/
enum TStoreType
	{
	ENoStore,
	EFileStore,
	ERomStore,
	ECompressedRomStore
	};
/**
@internalComponent
*/
enum Errors
	{
	NoCompression=-1,
	NoError=0,
	NoMemory,
	Arg,
	Files,
	SourceFile,
	DestFile,
	CommandFile,
	OutOfRange,
	TooManyArgs,
	UnknownCompression,
	CompressionError,
	DecompressionError,
	Bpp,
	PaletteFile,
	PaletteSupportNotImplemented,
	AlphaFiles,
	AlphaDimensions,
	AlphaBpp
	};
/**
@internalComponent
*/
struct SEpocBitmapHeader
/** Contains information about the bitmap when streaming bitmaps to stores. */
	{
	/** The size of the bitmap data, in bytes. */
	long iBitmapSize;
	/** The size of the structure in which the bitmap data is stored. */
	long iStructSize;
	long iWidthInPixels;
	long iHeightInPixels;
	long iWidthInTwips;
	long iHeightInTwips;
	/** The bitmaps number of bits per pixel */
	long iBitsPerPixel;
	/** Whether or not the bitmap is colour. A true value means that the bitmap 
	is colour. */
	TBitmapColor iColor;
	/** The number of entries in the bitmap's palette. */
	long iPaletteEntries;	// always 0 in current implementations
	/** The type of compression used to store the bitmap. */
	TBitmapfileCompression iCompression;
	};

#ifndef __LINUX__ 
#include <pshpack2.h>
#endif 
/**
@internalComponent
*/
struct TRgb24bit
	{
	unsigned char iBlue;
	unsigned char iGreen;
	unsigned char iRed;
	} GNUPACK;
#ifndef __LINUX__ 
#include <poppack.h>
#endif 

#ifndef __LINUX__ 
#include <pshpack2.h>
#endif 
/**
@internalComponent
*/
struct TBitmapFileHeader
	{
	unsigned short bfType;
	unsigned long bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long bfOffBits;
	} GNUPACK;
#ifndef __LINUX__ 
#include <poppack.h>
#endif 
/**
@internalComponent
*/
struct TBitmapInfoHeader
	{
	unsigned long biSize;
	long biWidth;
	long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	long biXPelsPerMeter;
	long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
	};

#ifndef __LINUX__ 
#include <pshpack2.h>
#endif 
/**
@internalComponent
*/
struct TRgbTriple
	{
	unsigned char rgbtBlue;
	unsigned char rgbtGreen;
	unsigned char rgbtRed;
	} GNUPACK;
#ifndef __LINUX__ 
#include <poppack.h>
#endif 
/**
@internalComponent
*/
struct TRgbQuad
	{
	unsigned char iBlue;
	unsigned char iGreen;
	unsigned char iRed;
	unsigned char iReserved;
	};

class TRgb
/** 
24-bit RGB colour value with 8 bits each for red, green and blue.

All Graphics drawing functions are specified in terms of a 32-bit TRgb colour 
containing the three colour values plus 8 unused bits. For hardware which 
does not support 24-bit colour, a mapping from TRgb to display colours is 
performed. 

The supported display modes are enumerated in the TDisplayMode type. In each 
display mode a unique index can represent each physical colours supported, 
and which can be mapped onto a full RGB value. The mappings are as follows: 

16-colour displays use the EGA colour set: black, white, and then both light 
and dark versions of grey, red, green, blue, cyan, magenta and yellow

256-colour displays support 216 colours made up of 6x6x6 RGB values, each 
containing all possible multiples of 51 for R,G,B values. Additionally, all 
remaining 10 shades of pure red, green, blue and grey are represented, by 
adding all remaining multiples of 17. This use of 256 colours is sometimes 
known as the Netscape colour cube.

4096-colour displays effectively support RGB values with 4 bits per primary 
colour

64k-colour displays effectively support RGB values with 5 bits allocated to 
red, 6 to green and 5 to blue

16 million-colour displays support true colour with 8 bits allocated to each 
primary colour

@see TDisplayMode
@see DynamicPalette 
@internalComponent
*/
	{
public:
	TRgb();
	TRgb(long unsigned int val);
	TRgb(int aRed,int aGreen,int aBlue);
	TRgb &operator=(const TRgb& aColor);
	int operator==(const TRgb& aColor);
	int Difference(const TRgb& col) const;
	int Gray2() const;
	int Gray4() const;
	int Gray16() const;
	int Gray256() const;
	int Color16() const;
	int Color256() const;
	int Color4K() const;
	int Color64K() const;
	long int Color16M() const;
	static TRgb Gray2(int aGray2);
	static TRgb Gray4(int aGray4);
	static TRgb Gray16(int aGray16);
	static TRgb Gray256(int aGray256);
	static TRgb Color16(int aColor16);
	static TRgb Color256(int aColor256);
	static TRgb Color4K(int aColor64K);
	static TRgb Color64K(int aColor64K);
	static TRgb Color16M(long int aColor16M);
public:
	unsigned char iRed;
	unsigned char iGreen;
	unsigned char iBlue;
	unsigned char iSpare;
	};

class Bitmap
/**
@internalComponent
*/
	{
public:
	int iUid;
	int iDispMode;
	void* iHeap;
	void* iPile;
	int iByteWidth;
	SEpocBitmapHeader iHeader;
	int iChunk;
	int iDataOffset; // offset from "this"
	};

class BitmapCompiler
/**
Historical note - Pbm is an abbreviation for "Symbian Bitmap"
@internalComponent
*/
	{
public:
	BitmapCompiler(char* aSourcefilenames[],int aNumSources);
	~BitmapCompiler();
	int Compile(TStoreType aSt,int aCompress,char* aDestfilename,char* aHeaderFilename,char* aPaletteFilename);
private:
	int AllocatePbmSourcesArray();
	int LoadSourcefiles();
	int LoadPbmFile(char* aPbmFilename);
	int RomImage(bool aCompress);
	int FileImage(int aCompress);
	int WriteRomheader();
	int WriteRombitmap(SEpocBitmapHeader* aPbm);
	int WriteFileheader();
	int WriteFilebitmap(SEpocBitmapHeader* aPbm);
	int WriteHeadStream();
	int CreateHeader(char* aHeaderFilename);
	char* UnadornedName(char* aName);	// returns pointer to static buffer
	int LoadPalette(char* aPaletteFilename);
	void CalculateInversePalette();
	void WritePalette();
	int CompressBitmap(SEpocBitmapHeader*& aPbm);
	int CompressByteData(char*& aDest,char* aSrce,int aSize);
	int WriteCompressedByteData(char*& aDest,char* aData,int aLength, const char* aDestEnd);
	int WriteCompressedByteValues(char*& aDest,char aValue,int aLength, const char* aDestEnd);
	int CompressTwelveBitData(char*& aDest,char* aSrce,int aSizeInBytes);
	int WriteCompressedTwelveBitData(unsigned short*& aDest,unsigned short aData,int aLength);
	int CompressSixteenBitData(char*& aDest,char* aSrce,int aSize);
	int WriteCompressedSixteenBitData(char*& aDest,unsigned short* aData,int aLength, const char* aDestEnd);
	int WriteCompressedSixteenBitValues(char*& aDest,unsigned short aValue,int aLength, const char* aDestEnd);
	int CompressTwentyFourBitData(char*& aDest,char* aSrce,int aSizeInBytes);
	int WriteCompressedTwentyFourBitData(char*& aDest,char* aData,int aLength, const char* aDestEnd);
	int WriteCompressedTwentyFourBitValues(char*& aDest,char aComponent1,char aComponent2,char aComponent3,int aLength, const char* aDestEnd);
	int CompressThirtyTwoUBitData(char*& aDest,char* aSrce,int aSizeInBytes);
	int WriteCompressedThirtyTwoUBitData(char*& aDest,char* aData, int aLength, const char* aDestEnd);
	int WriteCompressedThirtyTwoUBitValues(char*& aDest,char aComponent1, char aComponent2,char aComponent3, int aLength, const char* aDestEnd);
	int TrueColorPointerCompare(char* aColorPointer,char aComponent1,char aComponent2,char aComponent3);
	int CompressThirtyTwoABitData(char*& aDest,char* aSrce,int aSizeInBytes);
	int WriteCompressedThirtyTwoABitData(char*& aDest,char* aData,int aLength,const char* aDestEnd);
	int WriteCompressedThirtyTwoABitValues(char*& aDest,char aComponent1,char aComponent2,char aComponent3,char aComponent4,int aLength,const char* aDestEnd);
	int ColorAlphaPointerCompare(char* aColorPointer,char aComponent1,char aComponent2,char aComponent3,char aComponent4);
	int ReadHexString(char aHexBuf[10],char*& aDataPtr,char* aDataPtrLimit);
	int HexToInt(char aHighNibble,char aLowNibble);
	int HexToInt(char aNibble);
	static void CopyTail(void* aDst, void* aSrc, int aFullSize, int aSkipped);
private:
	fstream iDestFile;
	char** iSourcefilenames;
	SEpocBitmapHeader** iPbmSources;
	int iNumSources;
	int iDefaultPalette;
	TRgb iPalette[256];
	char iInversePalette[4096];
	};

class BitmapLoader
/**
@internalComponent
*/
	{
public:
	BitmapLoader();
	~BitmapLoader();
	int LoadBitmap(char* aFileName,int aBpp,TBitmapColor aColor,SEpocBitmapHeader*& aPbm);
private:
	int DoLoad(char* aFileName);
	int DoLoadAlpha(char* aAlphaFileName);
	int DoConvert(int aBpp,TBitmapColor aColor,SEpocBitmapHeader*& aPbm);
	TRgb GetBmpPixel(long aXCoord,long aYCoord);
	unsigned char GetAlphaPixel(long aXCoord,long aYCoord);
private:
	TBitmapInfoHeader iBmpHeader;
	int iNumBmpColors;
	TRgbQuad* iBmpColors;
	char* iBmpBits;
	char* iAlphaBits;
	};

class EpocLoader
/**
@internalComponent
*/
	{
public:
	EpocLoader();
	~EpocLoader();
	int EpocBitmapCount(char* aFilename,int& aCount,int& isRomFormat);
	int LoadEpocBitmap(char* aFilename,int aIndex);
	int SaveBitmap(char* aFilename);
	int SaveAlpha(char* aFilename);
	int DupBitmap(SEpocBitmapHeader*& aPbm);
	int ValidateEpocBitmap(char* aFilename);
	SEpocBitmapHeader Header() const { return iOriginalPbmHeader;};
private:
	int LoadFile(char* aFilename,int aIndex);
	int LoadRom(char* aFilename,int aIndex);
	TRgb GetPixel(int aXCoord,int aYCoord);
	unsigned char GetAlpha(int aXCoord,int aYCoord);
	int DoLoadFile(fstream& aFile);
	int DoLoadRom(fstream& aFile);
	int ExpandByteRLEData(char* aDest,int aDestSize,char* aSrce,int aSrceSize);
	int ExpandTwelveBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes);
	int ExpandSixteenBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes);
	int ExpandTwentyFourBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes);
	int ExpandThirtyTwoUBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes);
	int ExpandThirtyTwoABitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes);
	int Decompress(int aSize);
private:
	SEpocBitmapHeader iOriginalPbmHeader;
	SEpocBitmapHeader iPbmHeader;
	char* iPbmBits;
	};

class BitmapUtils
/**
@internalComponent
*/
	{
public:
	static int ByteWidth(int aPixelWidth,int aBitsPerPixel);
	};

#endif
