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

#include "BMCONV.H"

BitmapLoader::BitmapLoader():
	iNumBmpColors(0),
	iBmpColors(NULL),
	iBmpBits(NULL),
	iAlphaBits(NULL)
	{}

BitmapLoader::~BitmapLoader()
	{
	delete [] iBmpColors;
	delete [] iBmpBits;
	delete [] iAlphaBits;
	}

int BitmapLoader::LoadBitmap(char* aFileName,int aBpp,TBitmapColor aColor,SEpocBitmapHeader*& aPbm)
	{
	char sig[2];

#if defined(__MSVCDOTNET__) || defined(__LINUX__) || defined(__TOOLS2__)
	fstream file(aFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	fstream file(aFileName, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__

	if (file.is_open()==0)
		return Files;
	file.read(sig,2);
	file.close();
	if (file.gcount()!=2)
		return SourceFile ;
	if (sig[0]!='B'||sig[1]!='M')
		return SourceFile;

	int ret = DoLoad(aFileName);
	if (!ret && aColor==EColorBitmapAlpha)
		{
		int fileNameLen = strlen(aFileName);
		char* alphaFileName = new char[fileNameLen + 7];// -alpha suffix is 6 chars, plus NUL termination
		if (alphaFileName == NULL)
			return NoMemory;
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
		ret = DoLoadAlpha(alphaFileName); // overlay alpha data from separate file
		delete [] alphaFileName;
		}
	if (!ret)
		ret = DoConvert(aBpp,aColor,aPbm);
	return ret;
	}

int BitmapLoader::DoLoad(char* aFileName)
	{
#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	fstream file(aFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	fstream file(aFileName, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__
	if (file.is_open()==0)
		return Files;
	TBitmapFileHeader bmpfile;
	long size=sizeof(TBitmapFileHeader);
	file.read((char *)&bmpfile,size);
	if (file.gcount()!=size)
		return SourceFile;
	size=sizeof(TBitmapInfoHeader);
	file.read((char *)&iBmpHeader,size);
	if (file.gcount()!=size)
		return SourceFile;
	if (iBmpHeader.biCompression != 0)
		return UnknownCompression;
	size=bmpfile.bfSize-sizeof(TBitmapInfoHeader)-sizeof(TBitmapFileHeader);
	long bitcount=iBmpHeader.biBitCount;
	long colors=iBmpHeader.biClrUsed;
	if (colors==0)
		{
		if (bitcount==24)
			iNumBmpColors=0;
		else if (bitcount==32)
			iNumBmpColors=0;//See MSDN - BITMAPFILEHEADER and BITMAPINFOHEADER structures.
                            //If biCompression is 0 - we don't have TRgbQuad array! 
		else
			iNumBmpColors=1<<bitcount;
		}
	else
		iNumBmpColors=colors;
	if (iNumBmpColors > 256)
		return SourceFile;
	if (iNumBmpColors>0)
		{
		iBmpColors = new TRgbQuad[iNumBmpColors];
		if (iBmpColors == NULL)
			return NoMemory;
		memset(iBmpColors,0,iNumBmpColors*sizeof(TRgbQuad));
		}
	size-=iNumBmpColors*sizeof(TRgbQuad);
	iBmpBits = new char[size];
	if (iBmpBits == NULL)
		return NoMemory;
	memset(iBmpBits,0xff,size);
	// DEF102183: Graphics tools fail to run after building with MS VC8.
	if(iBmpColors != NULL)
	    {
		file.read((char *)iBmpColors,iNumBmpColors*sizeof(TRgbQuad));
		if (file.gcount()!=(int)(iNumBmpColors*sizeof(TRgbQuad)))
			return SourceFile;
	    }
	file.read(iBmpBits,size);
	file.close();
	if (file.gcount()!=size)
		return SourceFile;
	return NoError;
	}

int BitmapLoader::DoLoadAlpha(char* aAlphaFileName)
	{
#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	fstream file(aAlphaFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	fstream file(aAlphaFileName, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__
	if (file.is_open()==0)
		return AlphaFiles;
	TBitmapFileHeader alphaBmpfile;
	long size=sizeof(TBitmapFileHeader);
	file.read((char *)&alphaBmpfile,size);
	if (file.gcount()!=size)
		return SourceFile;
	size=sizeof(TBitmapInfoHeader);
	TBitmapInfoHeader alphaBmpInfo;
	file.read((char *)&alphaBmpInfo,size);
	if (file.gcount()!=size)
		return SourceFile;
	if (alphaBmpInfo.biCompression != 0)
		return UnknownCompression;
	if (alphaBmpInfo.biWidth != iBmpHeader.biWidth || alphaBmpInfo.biHeight != iBmpHeader.biHeight)
		return AlphaDimensions;
	if (alphaBmpInfo.biBitCount != 8)
		return AlphaBpp;
	size=alphaBmpfile.bfSize-sizeof(TBitmapInfoHeader)-sizeof(TBitmapFileHeader);
	long numBmpColors=alphaBmpInfo.biClrUsed;
	if (numBmpColors == 0)
		numBmpColors = 256;
	if (numBmpColors != 256)
		return SourceFile;
	size-=numBmpColors*sizeof(TRgbQuad);
	iAlphaBits = new char[size];
	if (iAlphaBits == NULL)
		{
		return NoMemory;
		}
	memset(iAlphaBits,0xff,size);
	char* bmpColors = new char[numBmpColors*sizeof(TRgbQuad)];
	file.read((char *)bmpColors,numBmpColors*sizeof(TRgbQuad));
	delete [] bmpColors; // we aren't interested in the palette data for the 8bpp grayscale alpha bmp
	if (file.gcount()!=(int)(numBmpColors*sizeof(TRgbQuad)))
		return SourceFile;
	file.read(iAlphaBits,size);
	file.close();
	if (file.gcount()!=size)
		return SourceFile;
	return NoError;
	}

TRgb BitmapLoader::GetBmpPixel(long aXCoord,long aYCoord)
	{
	TRgb darkgray(128,128,128);
	TRgb darkgrayex(127,127,127);
	TRgb lightgray(192,192,192);
	TRgb lightgrayex(187,187,187);
	unsigned char col;
	TRgb color;

	switch(iBmpHeader.biBitCount)
		{
		case 1:
			col=iBmpBits[(iBmpHeader.biHeight-aYCoord-1)*(((iBmpHeader.biWidth+31)>>5)<<2)+(aXCoord>>3)];
			col&=(0x80>>(aXCoord%8));
			if (iBmpColors)
				{
				TRgbQuad rgbq;
				if (col)
					rgbq = iBmpColors[1];
				else
					rgbq = iBmpColors[0];
				color = TRgb(rgbq.iRed,rgbq.iGreen,rgbq.iBlue);
				}
			else
				{
				if (col)
					color = TRgb(0x00ffffff);
				else
					color = TRgb(0);
				}
			break;
		case 4:
			col=iBmpBits[(iBmpHeader.biHeight-aYCoord-1)*(((iBmpHeader.biWidth+7)>>3)<<2)+(aXCoord>>1)];
			if (aXCoord%2==0)
				col=(unsigned char)(col>>4);
			col&=0x0f;
			if (iBmpColors)
				{
				TRgbQuad rgbq = iBmpColors[col];
				color = TRgb(rgbq.iRed,rgbq.iGreen,rgbq.iBlue);
				}
			else
				{
				col *= 17;
				color = TRgb(col,col,col);
				}
			break;
		case 8:
			col=iBmpBits[(iBmpHeader.biHeight-aYCoord-1)*((iBmpHeader.biWidth+3)&~3)+aXCoord];
			if (iBmpColors)
				{
				TRgbQuad rgbq = iBmpColors[col];
				color = TRgb(rgbq.iRed,rgbq.iGreen,rgbq.iBlue);
				}
			else
				color = TRgb(col,col,col);
			break;
		case 16:
			{
			unsigned short int* wordptr = (unsigned short int*)&iBmpBits[(iBmpHeader.biHeight-aYCoord-1)*(((iBmpHeader.biWidth+1)&~1)<<1)+(aXCoord<<1)];
			color = TRgb((*wordptr&0x7c)>>10,(*wordptr&0x3e)>>5,(*wordptr&0x1f));
			}
			break;
		case 24:
			{
			TRgbTriple rgbcol = *((TRgbTriple *)&(iBmpBits[(iBmpHeader.biHeight-aYCoord-1)*((3*iBmpHeader.biWidth+3)&~3)+aXCoord+(aXCoord<<1)]));
			color = TRgb(rgbcol.rgbtRed,rgbcol.rgbtGreen,rgbcol.rgbtBlue);
			}
			break;
		case 32:
			{
			unsigned long int* dwordptr = (unsigned long int*)&iBmpBits[(iBmpHeader.biHeight-aYCoord-1)*((iBmpHeader.biWidth)<<2)+(aXCoord<<2)];
			color = TRgb((*dwordptr&0xff0000)>>16,(*dwordptr&0xff00)>>8,*dwordptr&0xff);
			}
			break;
		default:
			break;
		}
	if (color == darkgray)
		color = darkgrayex;
	else if (color == lightgray)
		color = lightgrayex;
	return color;
	}

unsigned char BitmapLoader::GetAlphaPixel(long aXCoord,long aYCoord)
	{
	return iAlphaBits[(iBmpHeader.biHeight-aYCoord-1)*((iBmpHeader.biWidth+3)&~3)+aXCoord];
	}

int BitmapLoader::DoConvert(int aBpp,TBitmapColor aColor,SEpocBitmapHeader*& aPbm)
	{
	bool useAlpha = (aColor==EColorBitmapAlpha || aColor==EColorBitmapAlphaPM);
	long desttwipswidth = 0;
	long desttwipsheight = 0;

	long bytewidth = BitmapUtils::ByteWidth(iBmpHeader.biWidth,aBpp);
	long destlength = iBmpHeader.biHeight * bytewidth;

	if (iBmpHeader.biXPelsPerMeter>0)
		desttwipswidth = iBmpHeader.biWidth*1440000/254/iBmpHeader.biXPelsPerMeter;
	if (iBmpHeader.biYPelsPerMeter>0)
		desttwipsheight = iBmpHeader.biHeight*1440000/254/iBmpHeader.biYPelsPerMeter;

	aPbm = (SEpocBitmapHeader*)new char[sizeof(SEpocBitmapHeader) + destlength];
	if (aPbm == NULL)
		return NoMemory;
	memset(aPbm,0,sizeof(SEpocBitmapHeader));

	// aBitmap->iByteWidth = bytewidth;
	// aBitmap->iDataOffset = sizeof(Bitmap);

	aPbm->iBitmapSize = sizeof(SEpocBitmapHeader) + destlength;
	aPbm->iStructSize = sizeof(SEpocBitmapHeader);
	aPbm->iWidthInPixels = iBmpHeader.biWidth;
	aPbm->iHeightInPixels = iBmpHeader.biHeight;
	aPbm->iWidthInTwips = desttwipswidth;
	aPbm->iHeightInTwips = desttwipsheight;
	aPbm->iBitsPerPixel = aBpp;
	aPbm->iColor = aColor;
	aPbm->iPaletteEntries = 0;
	aPbm->iCompression = ENoBitmapCompression;

	char* pbmBits = ((char*)aPbm) + sizeof(SEpocBitmapHeader);
	memset(pbmBits,0xff,destlength);

	long col = 0;
	char* pixadd = 0;

	switch(aBpp)
		{
	case 1:
		{
		for(long ycrd=0;ycrd<iBmpHeader.biHeight;ycrd++)
			for(long xcrd=0;xcrd<iBmpHeader.biWidth;xcrd++)
				{
				TRgb color=GetBmpPixel(xcrd,ycrd);
				col=color.Gray2();
				pixadd=&(pbmBits[ycrd*bytewidth+(xcrd>>3)]);
				(*pixadd)&=~(1<<((xcrd&7)));
				(*pixadd)|=(unsigned char)(col<<(xcrd&7));
				}
		}
		break;
	case 2:
		{
		for(long ycrd=0;ycrd<iBmpHeader.biHeight;ycrd++)
			for(long xcrd=0;xcrd<iBmpHeader.biWidth;xcrd++)
				{
				TRgb color=GetBmpPixel(xcrd,ycrd);
				col=color.Gray4();
				pixadd=&(pbmBits[ycrd*bytewidth+(xcrd>>2)]);
				(*pixadd)&=~(0x3<<(2*(xcrd%4)));
				(*pixadd)|=(unsigned char)(col<<(2*(xcrd%4)));
				}
		}
		break;
	case 4:
		{
		for(long ycrd=0;ycrd<iBmpHeader.biHeight;ycrd++)
			for(long xcrd=0;xcrd<iBmpHeader.biWidth;xcrd++)
				{
				TRgb color=GetBmpPixel(xcrd,ycrd);
				if (aColor == EMonochromeBitmap)
					col = color.Gray16();
				else
					col = color.Color16();
				pixadd=&(pbmBits[ycrd*bytewidth+(xcrd>>1)]);
				if (xcrd%2!=0)
					*pixadd=(unsigned char)((unsigned char)((col<<4)|(*pixadd&0x0f)));
				else
					*pixadd=(unsigned char)((unsigned char)(col|(*pixadd&0xf0)));
				}
		}
		break;
	case 8:
		{
		for(long ycrd=0;ycrd<iBmpHeader.biHeight;ycrd++)
			for(long xcrd=0;xcrd<iBmpHeader.biWidth;xcrd++)
				{
				TRgb color=GetBmpPixel(xcrd,ycrd);
				if (aColor == EMonochromeBitmap)
					col = color.Gray256();
				else
					col = color.Color256();
				pixadd=&(pbmBits[ycrd*((iBmpHeader.biWidth+3)&~3)+xcrd]);
				*pixadd=(unsigned char)col;
				}
		}
		break;
	case 12:
		{
		for(long ycrd=0;ycrd<iBmpHeader.biHeight;ycrd++)
			for(long xcrd=0;xcrd<iBmpHeader.biWidth;xcrd++)
				{
				TRgb color=GetBmpPixel(xcrd,ycrd);
				col=color.Color4K();
				pixadd=&(pbmBits[ycrd*bytewidth+(xcrd<<1)]);
				unsigned short* wordadd=(unsigned short*)pixadd;
				*wordadd=(unsigned short)col;
				}
		}
		break;
	case 16:
		{
		for(long ycrd=0;ycrd<iBmpHeader.biHeight;ycrd++)
			for(long xcrd=0;xcrd<iBmpHeader.biWidth;xcrd++)
				{
				TRgb color=GetBmpPixel(xcrd,ycrd);
				col=color.Color64K();
				pixadd=&(pbmBits[ycrd*bytewidth+(xcrd<<1)]);
				unsigned short* wordadd=(unsigned short*)pixadd;
				*wordadd=(unsigned short)col;
				}
		}
		break;
	case 24:
		{
		for(long ycrd=0;ycrd<iBmpHeader.biHeight;ycrd++)
			{
			unsigned char* bytePtr = (unsigned char*)&pbmBits[ycrd*bytewidth];
			for(long xcrd=0;xcrd<iBmpHeader.biWidth;xcrd++)
				{
				TRgb col = GetBmpPixel(xcrd,ycrd);
				*bytePtr++ = (unsigned char) col.iBlue;
				*bytePtr++ = (unsigned char) col.iGreen;
				*bytePtr++ = (unsigned char) col.iRed;
				}
			}
		}
		break;
	case 32:
		{
		for(long ycrd=0;ycrd<iBmpHeader.biHeight;ycrd++)
			{
			unsigned char* bytePtr = (unsigned char*)&pbmBits[ycrd*bytewidth];
			for(long xcrd=0;xcrd<iBmpHeader.biWidth;xcrd++)
				{
				TRgb col = GetBmpPixel(xcrd,ycrd);
				unsigned char alpha = useAlpha?GetAlphaPixel(xcrd,ycrd):(unsigned char)0;
				*bytePtr++ = (unsigned char) col.iBlue;
				*bytePtr++ = (unsigned char) col.iGreen;
				*bytePtr++ = (unsigned char) col.iRed;
				*bytePtr++ = alpha;	
				}
			}
		}
		break;
		};

	return NoError;
	}

