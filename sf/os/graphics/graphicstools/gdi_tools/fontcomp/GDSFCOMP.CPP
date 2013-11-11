// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "GDSFCOMP.H"
#include "TOOLSVER.H"

int GdsFontCompiler::WriteFont()
	{
	int index=0;
	index=WriteHeader();
	if(index<0) return(index);
	for(int ch=index;ch<iFxf->n_chars;ch++)
		WriteCharData(ch);
	WriteFooter();
	return(NoError);
	}

int GdsFontCompiler::WriteHeader()
	{
	int mncw=0;
	for(char letter='A';letter<='Z';letter++)
		{
		FcmCharHead* fChar=iFxf->chr[letter];
		if(fChar)
			{
			int letterwidth=fChar->move;
			if(letterwidth>mncw)
				mncw=letterwidth;
			}
		}
	WriteFormattedData("FontBitmap ");
	WriteFormattedData(iFxf->name,1);
	WriteFormattedData("Uid ");
	WriteFormattedData(iFxf->iUid);
	if(iFxf->iBold)
		WriteFormattedData(" Bold");
	if(iFxf->iItalic)
		WriteFormattedData(" Italic");
	if(iFxf->iProportional)
		WriteFormattedData(" Proportional");
	if(iFxf->iSerif)
		WriteFormattedData(" Serif");
	if(iFxf->iSymbol)
		WriteFormattedData(" Symbol");
	WriteNewLine();
	WriteFormattedData("MaxNormalCharWidth ");
	WriteFormattedData(mncw,1);
	WriteFormattedData("CellHeight ");
	WriteFormattedData(iFxf->cell_height,1);
	WriteFormattedData("Ascent ");
	WriteFormattedData(iFxf->nominal_ascent,1);
//	WriteFormattedData("Uline ");
//	WriteFormattedData(iFxf->UlinePos,0);
//	WriteFormattedData(" ");
//	WriteFormattedData(iFxf->UlineThickness,1);
	int index=0;
	while(!iFxf->chr[index])
		index++;
	if(index>=iFxf->n_chars)
		return(FileFormat);
	WriteFormattedData("CodeSection ");
	WriteFormattedData(index,0);
	WriteFormattedData(":");
	WriteFormattedData(iFxf->n_chars-1,1);
	return(index);
	}

void GdsFontCompiler::WriteFooter()
	{
	WriteFormattedData("EndCodeSection",1);
	WriteFormattedData("EndFontBitmap",1);
	WriteNewLine();
	}

void GdsFontCompiler::WriteCharData(int charnum)
	{
	unsigned short int* pSrc;
	unsigned short int bit;
	char buf[10+MAX_CHAR_WID];
	unsigned short int srcBuf[(MAX_CHAR_WID+15)>>4];

	const FcmCharHead* fChar=iFxf->chr[charnum];
	WriteNewLine();
	WriteFormattedData("Char ");
	WriteFormattedData(charnum);
	if(fChar==NULL)
		{
		WriteNewLine();
		WriteFormattedData("EndChar",1);
		return;
		}
	WriteFormattedData(" Adjust ");

	int bitwid=fChar->move+(fChar->xOffset<0?-fChar->xOffset:0);
	const int ohang=fChar->move-fChar->xOffset-fChar->width;
	WriteFormattedData(fChar->xOffset);
	WriteFormattedData(" ");
	WriteFormattedData(ohang);
	bitwid+=ohang;
	if (charnum>31)
		{
		WriteFormattedData("  ! '");
		WriteFormattedData((char*)&charnum);
		WriteFormattedData("'");
		}
	WriteNewLine();
	if (fChar->width!=0)
		{
		WriteBlankLines(iFxf->cell_height-iFxf->descent-fChar->yOffset,fChar->width);
		for(int y=0;y<fChar->height;y++)
			{
			memcpy(srcBuf,iFontSpace+fChar->offset+y*fChar->ByteWid,fChar->ByteWid);
			pSrc=&srcBuf[0];
			bit=1;
			int pb=0;
			for(int i=0;i<fChar->width;i++)
				{
				buf[pb++]=((*pSrc)&bit)?SetPixel:BlankPixel;
				bit<<=1;
				if(!bit)
					{
					bit=1;
					pSrc++;
					}
				}
			buf[pb]=0;
			WriteFormattedData(buf,1);
			}
		WriteBlankLines(iFxf->descent+fChar->yOffset-fChar->height,fChar->width);
		}
	WriteFormattedData("EndChar",1);
	}

void GdsFontCompiler::WriteBlankLines(int num,int width)
	{
	char buf[2+MAX_CHAR_WID+20];
	memset(&buf[0],BlankPixel,width);
	buf[width] ='\0';
	for(int i=0;i<num;i++)
		WriteFormattedData(buf,1);
	}

void GdsFontCompiler::WriteFormattedData(char* aData,int aNewLine)
	{
	if(aData)
		iOutputFile << aData;
	if(aNewLine)
		WriteNewLine();
	}

void GdsFontCompiler::WriteFormattedData(int aNum,int aNewLine)
	{
	char numbuf[16];
	_itoa(aNum,numbuf,10);
	WriteFormattedData(numbuf,aNewLine);
	}

void GdsFontCompiler::WriteNewLine()
	{
	iOutputFile << "\n";
	}

int main(int argc,char *argv[])
    {
	if(argc<3 || argc>5)
		{
		cout << "\n";
		cout << "FONTCOMP Version 0.01(" << version << ")\n";
		cout << "\n";
		cout << "USAGE: FONTCOMP srcfile destfile [/e [mapfile]|/f]\n";
		cout << "Where srcfile is the file to be processed,\n";
		cout << "destfile is the file to be created,\n";
		cout << "/e specifies EFF format (default) and /f\n";
		cout << "specifies FSC format.  If the format is EFF then\n";
		cout << "the optional mapfile may be used for altenative\n";
		cout << "character sets.\n\n";
		return(0);
		}
	FontType type=EFontTypeEff;
	if(argc==4)
		if(argv[3][1]=='f' || argv[3][1]=='F')
			type=EFontTypeFsc;
	char* mapfile=NULL;
	if(argc==5) mapfile=argv[4];
	GdsFontCompiler fontcomp;
	int ret=fontcomp.Init(argv[1],argv[2],mapfile);
	if(ret==NoError)
		ret=fontcomp.Read(type);
	if(ret==NoError)
		fontcomp.RemoveBlankSpace();
	if(ret==NoError)
		ret=fontcomp.WriteFont();
	switch(ret)
		{
		case NoError:
			cout << "Success\n\n";
			break;
		case NoMemory:
			cout << "Out of memory\n\n";
			break;
		case NoFile:
			cout << "File does not exist\n\n";
			break;
		case FileRead:
			cout << "File read error\n\n";
			break;
		case FileWrite:
			cout << "File write error\n\n";
			break;
		case FileFormat:
			cout << "File has wrong format\n\n";
			break;
		case Parameter:
			cout << "Bad parameter\n\n";
			break;
		}
	return(ret);
    }

