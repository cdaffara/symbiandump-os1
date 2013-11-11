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

#include <assert.h>
#include "BMCONV.H"

EpocLoader::EpocLoader():
	iPbmBits(NULL)
	{}

EpocLoader::~EpocLoader()
	{
	delete iPbmBits;
	}

int EpocLoader::EpocBitmapCount(char* aFileName, int& aCount, int& isRomFormat)
    {
#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	fstream file(aFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	fstream file(aFileName, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__

	if (file.is_open()==0)
		return Files;

	long int wordbuffer;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	if (wordbuffer==KMultiBitmapRomImageUid)
		{
		// ROM format file - next 4 bytes are the number of bitmaps
		isRomFormat=1;
		}
	else
		{
		if (wordbuffer!=KWriteOnceFileStoreUid)
			return SourceFile;
		isRomFormat=0;
		file.read((char *)&wordbuffer,4);
		if (file.gcount()!=4 || wordbuffer!=KMultiBitmapFileImageUid)
			return SourceFile;
		file.read((char *)&wordbuffer,4);
		if (file.gcount()!=4 || wordbuffer!=0)
			return SourceFile;
		file.read((char *)&wordbuffer,4);
		if (file.gcount()!=4 || wordbuffer!=KMultiBitmapFileImageChecksum)
			return SourceFile;
		file.read((char *)&wordbuffer,4);
		if (file.gcount()!=4)
			return SourceFile;
		file.seekg(wordbuffer,ios::beg);
		}
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	aCount=wordbuffer;
	return NoError;
    }

int EpocLoader::LoadEpocBitmap(char* aFileName,int aIndex)
	{
#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	fstream file(aFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	fstream file(aFileName, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__

	if (file.is_open()==0)
		return Files;

	long int wordbuffer;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	file.close();

	if (wordbuffer==KMultiBitmapRomImageUid)
		return LoadRom(aFileName,aIndex);

	return LoadFile(aFileName,aIndex);
	}

int EpocLoader::LoadFile(char* aFileName,int aIndex)
	{
#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	fstream file(aFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	fstream file(aFileName, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__

	if (file.is_open()==0)
		return Files;

	long int wordbuffer;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4 || wordbuffer!=KWriteOnceFileStoreUid)
		return SourceFile;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4 || wordbuffer!=KMultiBitmapFileImageUid)
		return SourceFile;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4 || wordbuffer!=0)
		return SourceFile;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4 || wordbuffer!=KMultiBitmapFileImageChecksum)
		return SourceFile;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	file.seekg(wordbuffer,ios::beg);
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;

	int numsources=wordbuffer;
	if (aIndex >= numsources)
		return OutOfRange;
	file.seekg(aIndex*4,ios::cur);
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	file.seekg(wordbuffer,ios::beg);
	int ret = DoLoadFile(file);
	file.close();
	return ret;
	}

int EpocLoader::LoadRom(char* aFileName,int aIndex)
	{
#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	fstream file(aFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	fstream file(aFileName, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__

	if (file.is_open()==0)
		return Files;

	long int wordbuffer;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	if (wordbuffer!=KMultiBitmapRomImageUid)
		return SourceFile;
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	if (aIndex>=wordbuffer)
		return OutOfRange;
	file.seekg(aIndex*4,ios::cur);
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	file.seekg(wordbuffer,ios::beg);
	int ret=DoLoadRom(file);
	file.close();
	return ret;
	}

int EpocLoader::DoLoadFile(fstream& aFile)
	{
	long size = sizeof(SEpocBitmapHeader);
	aFile.read((char *)&iPbmHeader,size);
	if (aFile.gcount()!=size)
		return SourceFile;
	iOriginalPbmHeader = iPbmHeader;
	size=iPbmHeader.iBitmapSize-iPbmHeader.iStructSize;

	iPbmBits=new char[size];
	if (iPbmBits==NULL)
		return NoMemory;
	memset(iPbmBits,0xff,size);
	aFile.read(iPbmBits,size);
	aFile.close();
	if (aFile.gcount() != size)
		return SourceFile;

	if (iPbmHeader.iCompression != ENoBitmapCompression)
		{
		return Decompress(size);
		}

	return NoError;
	}

int EpocLoader::DoLoadRom(fstream& aFile)
	{
	Bitmap bmp;
	long size=sizeof(Bitmap);
	aFile.read((char*)&bmp,size);
	if (aFile.gcount() != size)
		return SourceFile;

	size = bmp.iHeader.iBitmapSize - sizeof(SEpocBitmapHeader);
	iPbmBits = new char[size];
	if (iPbmBits == NULL)
		return NoMemory;
	memset(iPbmBits,0xff,size);

	aFile.read(iPbmBits,size);
	if (aFile.gcount() != size)
		return SourceFile;
	iPbmHeader = bmp.iHeader;
	iOriginalPbmHeader = iPbmHeader;

	if (iPbmHeader.iCompression != ENoBitmapCompression)
		{
		return Decompress(size);
		}

	return NoError;
	}

int EpocLoader::Decompress(int aSize)
	{
	int byteWidth = BitmapUtils::ByteWidth(iPbmHeader.iWidthInPixels,iPbmHeader.iBitsPerPixel);
	int expandedsize = byteWidth * iPbmHeader.iHeightInPixels;
	char* newbits = new char[expandedsize];
	if (newbits == NULL)
		return NoMemory;
	memset(newbits,0xff,expandedsize);
	int ret = NoError;
	switch (iPbmHeader.iCompression)
		{
	case EByteRLECompression:
		ret = ExpandByteRLEData(newbits,expandedsize,iPbmBits,aSize);
		break;
	case ETwelveBitRLECompression:
		ret = ExpandTwelveBitRLEData(newbits,expandedsize,iPbmBits,aSize);
		break;
	case ESixteenBitRLECompression:
		ret = ExpandSixteenBitRLEData(newbits,expandedsize,iPbmBits,aSize);
		break;
	case ETwentyFourBitRLECompression:
		ret = ExpandTwentyFourBitRLEData(newbits,expandedsize,iPbmBits,aSize);
		break;
	case EThirtyTwoUBitRLECompression:
		ret = ExpandThirtyTwoUBitRLEData(newbits,expandedsize,iPbmBits,aSize);
		break;
	case EThirtyTwoABitRLECompression:
		ret = ExpandThirtyTwoABitRLEData(newbits,expandedsize,iPbmBits,aSize);
		break;
	default:
		ret = UnknownCompression;
		break;
		}
	delete iPbmBits;
	iPbmBits = newbits;
	iPbmHeader.iCompression = ENoBitmapCompression;
	iPbmHeader.iBitmapSize += expandedsize-aSize;
	return ret;
	}

int EpocLoader::ExpandByteRLEData(char* aDest,int aDestSize,char* aSrce,int aSrceSize)
	{
	char* srcelimit=aSrce+aSrceSize;
	char* destlimit=aDest+aDestSize;
	while(aSrce<srcelimit && aDest<destlimit)
		{
		char count=*aSrce++;
		if (count<0)
			{
			int runLength=-count;
			memcpy(aDest,aSrce,runLength);
			aSrce+=runLength;
			aDest+=runLength;
			}
		else
			{
			char value=*aSrce++;
			while(count>=0)
				{
				*aDest++=value;
				count--;
				}
			}
		}
	if (aSrce!=srcelimit || aDest!=destlimit)
		return DecompressionError;
	return NoError;
	}

int EpocLoader::ExpandTwelveBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes)
	{
	unsigned short* srcePtr = (unsigned short*)aSrce;
	unsigned short* destPtr = (unsigned short*)aDest;
	unsigned short* srcePtrLimit = srcePtr + (aSrceSizeInBytes / 2);
	unsigned short* destPtrLimit = destPtr + (aDestSizeInBytes / 2);

	while (srcePtr < srcePtrLimit && destPtr < destPtrLimit)
		{
		unsigned short value = *srcePtr++;
		int runLength = (value >> 12) + 1;
		value &= 0x0fff;

		for (;runLength > 0;runLength--)
			*destPtr++ = value;
		}

	if (srcePtr != srcePtrLimit || destPtr != destPtrLimit)
		return DecompressionError;

	return NoError;
	}

int EpocLoader::ExpandSixteenBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes)
	{
	char* srcePtrLimit = aSrce + aSrceSizeInBytes;
	unsigned short* destPtr = (unsigned short*)aDest;
	unsigned short* destPtrLimit = destPtr + (aDestSizeInBytes / 2);

	while (aSrce < srcePtrLimit && destPtr < destPtrLimit)
		{
		int runLength = *aSrce++;

		if (runLength >= 0)
			{
			unsigned short value = *((unsigned short*)(aSrce));
			aSrce += 2;
			for (runLength++; runLength > 0; runLength--)
				*destPtr++ = value;
			}
		else
			{
			runLength = -runLength;
			int byteLength = runLength * 2;
			memcpy(destPtr,aSrce,byteLength);
			aSrce += byteLength;
			destPtr += runLength;
			}
		}

	if (aSrce != srcePtrLimit || destPtr != destPtrLimit)
		return DecompressionError;

	return NoError;
	}

int EpocLoader::ExpandTwentyFourBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes)
	{
	char* srcePtrLimit = aSrce + aSrceSizeInBytes;
	char* destPtrLimit = aDest + aDestSizeInBytes;

	while (aSrce < srcePtrLimit && aDest < destPtrLimit)
		{
		int runLength = *aSrce++;

		if (runLength >= 0)
			{
			char component1 = *aSrce++;
			char component2 = *aSrce++;
			char component3 = *aSrce++;
			for (runLength++; runLength > 0; runLength--)
				{
				*aDest++ = component1;
				*aDest++ = component2;
				*aDest++ = component3;
				}
			}
		else
			{
			runLength = -runLength;
			int byteLength = runLength * 3;
			memcpy(aDest,aSrce,byteLength);
			aSrce += byteLength;
			aDest += byteLength;
			}
		}

	if (aSrce != srcePtrLimit || aDest != destPtrLimit)
		return DecompressionError;

	return NoError;
	}

/** The function decodes 24-bit compressed pixel buffer into the 32-bit buffer, where top bytes are unused*/
int EpocLoader::ExpandThirtyTwoUBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes)
	{
	char* srcePtrLimit = aSrce + aSrceSizeInBytes;
	char* destPtrLimit = aDest + aDestSizeInBytes;

	while (aSrce < srcePtrLimit && aDest < destPtrLimit)
		{
		int runLength = *aSrce++;

		if (runLength >= 0)
			{
			char component1 = *aSrce++;
			char component2 = *aSrce++;
			char component3 = *aSrce++;
			for (runLength++; runLength > 0; runLength--)
				{
				*aDest++ = component1;
				*aDest++ = component2;
				*aDest++ = component3;
				aDest++;
				}
			}
		else 
			{
			runLength = -runLength;
			for(int ii = 0; ii < runLength; ii++)
				{
				memcpy(aDest,aSrce,3);
				aSrce += 3;
				aDest += 4;
				}
			}
		}

	if (aSrce != srcePtrLimit || aDest != destPtrLimit)
		return DecompressionError;

	return NoError;
	}

/** The function decodes 32-bit compressed pixel buffer into the 32-bit buffer, where top bytes are alpha channel*/
int EpocLoader::ExpandThirtyTwoABitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes)
	{
	char* srcePtrLimit = aSrce + aSrceSizeInBytes;
	char* destPtrLimit = aDest + aDestSizeInBytes;

	while (aSrce < srcePtrLimit && aDest < destPtrLimit)
		{
		int runLength = *aSrce++;

		if (runLength >= 0)
			{
			char component1 = *aSrce++;
			char component2 = *aSrce++;
			char component3 = *aSrce++;
			char component4 = *aSrce++;
			for (runLength++; runLength > 0; runLength--)
				{
				*aDest++ = component1;
				*aDest++ = component2;
				*aDest++ = component3;
				*aDest++ = component4;
				}
			}
		else 
			{
			runLength = -runLength;
			memcpy(aDest,aSrce,4*runLength);
			aSrce += 4*runLength;
			aDest += 4*runLength;
			}
		}

	if (aSrce != srcePtrLimit || aDest != destPtrLimit)
		return DecompressionError;

	return NoError;
	}

TRgb EpocLoader::GetPixel(int aXCoord,int aYCoord)
	{
	unsigned char col;
	aXCoord%=iPbmHeader.iWidthInPixels;
	aYCoord%=iPbmHeader.iHeightInPixels;
	int byteWidth = BitmapUtils::ByteWidth(iPbmHeader.iWidthInPixels,iPbmHeader.iBitsPerPixel);
	int yOffset = aYCoord * byteWidth;

	switch(iPbmHeader.iBitsPerPixel)
		{
		case 1:
			col = iPbmBits[yOffset + (aXCoord / 8)];
			col >>= (aXCoord&7);
			return TRgb::Gray2(col & 1);
		case 2:
			col = iPbmBits[yOffset + (aXCoord / 4)];
			col = (unsigned char)(col>>(2*(aXCoord%4)));
			return TRgb::Gray4(col & 3);
		case 4:
			col = iPbmBits[yOffset + (aXCoord / 2)];
			if (aXCoord & 1)
				col >>= 4;
			col &= 0xf;
			if (iPbmHeader.iColor==EColorBitmap)
				return TRgb::Color16(col);
			else
				return TRgb::Gray16(col);
		case 8:
			col=iPbmBits[yOffset + aXCoord];
			if (iPbmHeader.iColor==EColorBitmap)
				return TRgb::Color256(col);
			else
				return TRgb::Gray256(col);
		case 12:
		case 16:
			{
			unsigned short* shortPtr = (unsigned short*)&iPbmBits[yOffset + (aXCoord * 2)];
			if (iPbmHeader.iBitsPerPixel == 12)
				return TRgb::Color4K(*shortPtr);
			else
				return TRgb::Color64K(*shortPtr);
			}
		case 24:
			{
			char* pixelPtr = iPbmBits + yOffset + (aXCoord * 3);
			TRgb pixelColor;
			pixelColor.iBlue = *pixelPtr++;
			pixelColor.iGreen = *pixelPtr++;
			pixelColor.iRed = *pixelPtr;
			return pixelColor;
			}
		case 32:
			{
			char* pixelPtr = iPbmBits + yOffset + (aXCoord * 4);
			TRgb pixelColor;
			pixelColor.iBlue = *pixelPtr++;
			pixelColor.iGreen = *pixelPtr++;
			pixelColor.iRed = *pixelPtr++;
			pixelColor.iSpare = *pixelPtr;
			return pixelColor;
			}
		default:
			return TRgb(0);
		}
	}

unsigned char EpocLoader::GetAlpha(int aXCoord,int aYCoord)
	{
	aXCoord%=iPbmHeader.iWidthInPixels;
	aYCoord%=iPbmHeader.iHeightInPixels;
	int byteWidth = BitmapUtils::ByteWidth(iPbmHeader.iWidthInPixels,iPbmHeader.iBitsPerPixel);
	int yOffset = aYCoord * byteWidth;

	assert(iPbmHeader.iBitsPerPixel == 32); // this method must only be called for 32bpp bitmaps!

	char* pixelPtr = iPbmBits + yOffset + (aXCoord * 4);
	pixelPtr += 3; // skip over RGB
	unsigned char col = *pixelPtr;
	return col;	
	}

int EpocLoader::SaveBitmap(char* aFileName)
	{
	TBitmapFileHeader fileheader;
	TBitmapInfoHeader bmpHeader;
	char* bmpBits;

	bmpHeader.biSize = sizeof(TBitmapInfoHeader);
	bmpHeader.biWidth = iPbmHeader.iWidthInPixels;
	bmpHeader.biHeight = iPbmHeader.iHeightInPixels;
	bmpHeader.biPlanes = 1;
	bmpHeader.biBitCount = 24;
	bmpHeader.biCompression = 0;
	bmpHeader.biSizeImage = 0;
	bmpHeader.biXPelsPerMeter = 0;
	bmpHeader.biYPelsPerMeter = 0;
	bmpHeader.biClrUsed = 0;
	bmpHeader.biClrImportant = 0;

	long byteWidth = ((bmpHeader.biWidth * 3) + 3) & ~3;
	long destlength = bmpHeader.biHeight * byteWidth;

	fileheader.bfType = 'B'+('M'<<8);
	fileheader.bfSize = sizeof(TBitmapFileHeader)+sizeof(TBitmapInfoHeader)+destlength;
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = sizeof(TBitmapFileHeader)+sizeof(TBitmapInfoHeader);

	bmpBits = new char[destlength];
	if (bmpBits == NULL)
		return NoMemory;
	memset(bmpBits,0xff,destlength);

	for(long y=0;y<bmpHeader.biHeight;y++)
		{
		char* dest=&bmpBits[(bmpHeader.biHeight-y-1)*byteWidth];
		for(long x=0;x<bmpHeader.biWidth;x++)
			{
			TRgb pixel=GetPixel(x,y);
			*dest++=pixel.iBlue;
			*dest++=pixel.iGreen;
			*dest++=pixel.iRed;
			}
		}

	fstream file(aFileName, ios::out | ios::binary);

	if (file.is_open()==0)
		{
		delete bmpBits;
		return DestFile;
		}

	file.write((char *)&fileheader,sizeof(TBitmapFileHeader));
	file.write((char *)&bmpHeader,sizeof(TBitmapInfoHeader));
	file.write((char *)bmpBits,destlength);
	file.close();

	delete bmpBits;

	if (iPbmHeader.iColor == EColorBitmapAlpha || iPbmHeader.iColor ==EColorBitmapAlphaPM)
		SaveAlpha(aFileName);
	return NoError;
	}


int EpocLoader::SaveAlpha(char* aFileName)
	{
	TBitmapFileHeader fileheader;
	TBitmapInfoHeader alphaHeader;

	alphaHeader.biSize = sizeof(TBitmapInfoHeader);
	alphaHeader.biWidth = iPbmHeader.iWidthInPixels;
	alphaHeader.biHeight = iPbmHeader.iHeightInPixels;
	alphaHeader.biPlanes = 1;
	alphaHeader.biBitCount = 8;
	alphaHeader.biCompression = 0;
	alphaHeader.biSizeImage = 0;
	alphaHeader.biXPelsPerMeter = 0;
	alphaHeader.biYPelsPerMeter = 0;
	alphaHeader.biClrUsed = 256;
	alphaHeader.biClrImportant = 0;

	const long paletteSize = 1024;

	// ensure bytes-per-scanline is a large enough multiple of 4
	long byteWidth = (alphaHeader.biWidth + 3) & ~3; 
	long destlength = alphaHeader.biHeight * byteWidth;
	alphaHeader.biSizeImage = destlength;

	fileheader.bfType = 'B'+('M'<<8);
	fileheader.bfSize = sizeof(TBitmapFileHeader)+sizeof(TBitmapInfoHeader)+paletteSize+destlength;
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = sizeof(TBitmapFileHeader)+sizeof(TBitmapInfoHeader)+paletteSize;

	// we need to output the grayscale palette before the actual alpha data as this is an 8bpp BMP
	char* palette = new char[paletteSize]; // 256 colors x 4bytes/color (r,g,b,unused)
	if (palette == NULL)
		return NoMemory;
	memset(palette,0,paletteSize);
	char* pEnt = palette;
	for (long p=0;p<256;++p)
		{
		unsigned char col = (unsigned char)p;
		*pEnt++=col;
		*pEnt++=col;
		*pEnt++=col;
		pEnt++;
		}

	char* alphaBits = new char[destlength];
	if (alphaBits == NULL)
		{
		delete [] palette;
		return NoMemory;
		}
	memset(alphaBits,0,destlength);

	for(long y=0;y<alphaHeader.biHeight;y++)
		{
		char* dest=&alphaBits[(alphaHeader.biHeight-y-1)*byteWidth];
		for(long x=0;x<alphaHeader.biWidth;x++)
			{
			unsigned char alphaVal=GetAlpha(x,y);
			*dest++=alphaVal;
			}
		}

	int fileNameLen = strlen(aFileName);
	char* alphaFileName = new char[fileNameLen + 7];// -alpha suffix is 6 chars, plus NUL termination
	if (alphaFileName == NULL)
		{
		delete [] palette;
		delete [] alphaBits;
		return NoMemory;
		}
	int dotPos = -1;
	for (int i = 0; i < fileNameLen; ++i)
		if (aFileName[i]=='.')
			dotPos=i;
	int prefixLen = (dotPos>=0?dotPos:fileNameLen);
	memcpy(alphaFileName,aFileName,prefixLen);
	const char* suffix = "-alpha";
	memcpy(alphaFileName+prefixLen,suffix,6);
	if (dotPos>=0)
		memcpy(alphaFileName+prefixLen+6,aFileName+dotPos,fileNameLen-dotPos);
	*(alphaFileName + fileNameLen + 6) = '\0';
	fstream file(alphaFileName, ios::out | ios::binary);
	if (file.is_open()==0)
		{
		delete [] alphaFileName;
		delete [] alphaBits;
		delete [] palette;
		return DestFile;
		}

	file.write((char *)&fileheader,sizeof(TBitmapFileHeader));
	file.write((char *)&alphaHeader,sizeof(TBitmapInfoHeader));
	file.write((char *)palette,paletteSize);
	file.write((char *)alphaBits,destlength);
	file.close();

	delete [] alphaFileName;
	delete [] alphaBits;
	delete [] palette;
	return NoError;
	}

int EpocLoader::DupBitmap(SEpocBitmapHeader*& aPbm)
	{
	char* newPbm = new char[iPbmHeader.iBitmapSize];
	if (newPbm == NULL)
		return NoMemory;
	memcpy(newPbm, &iPbmHeader, sizeof(SEpocBitmapHeader));
	memcpy(newPbm+sizeof(SEpocBitmapHeader), iPbmBits, iPbmHeader.iBitmapSize - sizeof(SEpocBitmapHeader));
	aPbm = (SEpocBitmapHeader*)newPbm;
	return NoError;
	}

/**
Validates an mbm files bitmap information to ensure that the length of each bitmaps matches their location
within the file. Also checks that the number of bitmap offsets available matches the number of bitmaps.

@param aFilename The mbm file to be validated
@return An error code from the Errors enumeration
 */
int EpocLoader::ValidateEpocBitmap(char* aFilename)
	{
#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	fstream file(aFilename, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	fstream file(aFilename, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__
	if (file.is_open()==0)
		return Files;

	long int wordbuffer;
	int isRomFormat;
	
	file.read((char *)&wordbuffer,4);
	if (file.gcount()!=4)
		return SourceFile;
	if (wordbuffer==KMultiBitmapRomImageUid)
		{
		//Validation not implemented for ROM only files.
		}
	else
		{
		if (wordbuffer!=KWriteOnceFileStoreUid)
			return SourceFile;
		isRomFormat=0;
		file.read((char *)&wordbuffer,4);
		if (file.gcount()!=4 || wordbuffer!=KMultiBitmapFileImageUid)
			return SourceFile;
		file.read((char *)&wordbuffer,4);
		if (file.gcount()!=4 || wordbuffer!=0)
			return SourceFile;
		file.read((char *)&wordbuffer,4);
		if (file.gcount()!=4 || wordbuffer!=KMultiBitmapFileImageChecksum)
			return SourceFile;
		file.read((char *)&wordbuffer,4);
		if (file.gcount()!=4)
			return SourceFile;
		//The 5th byte is a pointer to the footer containing the number of bitmaps
		//and the offset from the beginning of the file each begins at.
		int footerPos = wordbuffer;
		
		file.seekg(footerPos,ios::beg);
		
		file.read((char *)&wordbuffer, 4);
		if (file.gcount()!=4)
			return SourceFile;
		
		int numBitmaps = wordbuffer;

		int bmpOffset[numBitmaps+1];
		
		for (int i = 0; i < numBitmaps; i++)
			{
			file.read((char *)&wordbuffer,4);
					if (file.gcount()!=4)
						return SourceFile;
			bmpOffset[i] = wordbuffer;
			}
		
		//The byte at the end of the last bitmap is the location of the footer
		bmpOffset[numBitmaps] = footerPos;
		
		//Check length of a bitmap is reflected in the difference between the offsets
		for (int i = 0 ; i < numBitmaps ; i++)
			{
			file.seekg(bmpOffset[i]);
			
			//Read the length of this bitmap
			file.read((char *)&wordbuffer,4);
				if (file.gcount()!=4)
					return SourceFile;

			//Validate length against offsets of this & next bmp
			if (bmpOffset[i+1]-wordbuffer != bmpOffset[i])
				return SourceFile;
			}
		}
	return NoError;
	}
