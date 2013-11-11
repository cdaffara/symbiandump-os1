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

#include "FONTCOMP.H"

FontCompiler::FontCompiler():
	iInputFile(),
	iOutputFile(),
	iFxf(NULL),
	iFontSpace(NULL),
	iWorkSpace(NULL),
	iMapSpace(NULL),
	iWorkSpaceSize(0),
	iHeaderDataLen(0),
	iHeaderData()
	{}

int FontCompiler::Init(char* infile,char* outfile,char* mapfile)
	{
	if(mapfile)
		{
		iMapSpace=new short int[256];
		if(!iMapSpace) return(NoMemory);

#ifdef __MSVCDOTNET__
		fstream tempfile(mapfile, ios::in | ios::binary);
#else //!__MSVCDOTNET__
		fstream tempfile(mapfile, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__

		if(!tempfile.is_open())
			return(NoFile);
		tempfile.read((char*)iMapSpace,512);
		tempfile.close();
		}
	iOutputFile.open(outfile, ios::out);

#ifdef __MSVCDOTNET__
	iInputFile.open(infile, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	iInputFile.open(infile, ios::in | ios::binary | ios::nocreate);
#endif //__MSVCDOTNET__

	if(!(iOutputFile.is_open() && iInputFile.is_open()))
		return(NoFile);
	iFxf=new Fxf;
	if(!iFxf) return(NoMemory);
	iFxf->iBold=0;
	iFxf->iItalic=0;
	iFxf->iProportional=0;
	iFxf->iSerif=0;
	iFxf->iSymbol=0;
	iFxf->iUid=0;
	iFontSpace=new char[0x100000];
	if(!iFontSpace) return(NoMemory);
	return(NoError);
	}

char* FontCompiler::FontStore() const
	{
	return(iFontSpace);
	}

int FontCompiler::Read(FontType aInputType)
	{
	FontRead *read=NULL;
	switch(aInputType)
		{
		case EFontTypeFsc:
			read=new FscRead(iInputFile,*this,iFxf);
			break;
		case EFontTypeEff:
			read=new EffRead(iInputFile,*this,iFxf,iMapSpace);
			break;
		default:
			return(Parameter);
		}
	if(!read)
		return(NoMemory);
	int error=read->ReadFont();
	delete read;
	return(error);
	}

void FontCompiler::RemoveBlankSpace()
	{
	int count=0;
	const int maxbytewidth=(MAX_CHAR_WID+15)>>3;
	unsigned char buf[maxbytewidth];
	unsigned char zbuf[maxbytewidth];

	for(count=0;count<maxbytewidth;count++)
			zbuf[count]=0;
	for(int chNum=iFxf->FirstChr;chNum<iFxf->n_chars;chNum++)
		{
		FcmCharHead *fChar=iFxf->chr[chNum];
		if(fChar)
		    {
		    int LastNonBlank=0;
		    int TopCount=0;
		    int MinLeftBlank=fChar->width;
		    int MinRightBlank=0;
		    
       	    // DEF102183: Graphics tools fail to build using MS VC8.
		    int row;
		    int x;
		    for(row=0;row<fChar->height;)
			{
				memcpy(buf,iFontSpace+fChar->offset+fChar->ByteWid*row,fChar->ByteWid);
			unsigned char bit=1;
			unsigned char* pb=buf;
			for(x=0;x<MinLeftBlank;x++)
			    {
			    if ((*pb)&bit)
				{
				MinLeftBlank=x;
				break;
				}
			    bit<<=1;
			    if (bit==0)
				{
				bit=1;
				pb++;
				}
			    }
			bit=(unsigned char)(1<<((fChar->width-1)%8));
			pb=&buf[((fChar->width-1)>>3)];
			for(x=fChar->width;x>MinRightBlank;x--)
			    {
			    if ((*pb)&bit)
				{
				MinRightBlank=x;
				break;
				}
			    bit>>=1;
			    if (bit==0)
				{
				bit=unsigned char(0x80);
				pb--;
				}
			    }
			row++;
				if(memcmp(zbuf,buf,fChar->ByteWid))
			    {
			    if (TopCount==0)
				TopCount=row;   /* Count of blank rows at the top */
			    LastNonBlank=row;
			    }
			}
		    if (TopCount==0)
			{
			fChar->height=0;
			fChar->width=0;
			fChar->xOffset=0;
			fChar->yOffset=0;
			}
		    else
			{
			TopCount--;
			fChar->height=LastNonBlank-TopCount;
			fChar->width=MinRightBlank-MinLeftBlank;
			fChar->yOffset-=TopCount;
			fChar->offset+=TopCount*fChar->ByteWid;
			fChar->xOffset+=MinLeftBlank;
			if (MinLeftBlank)
			    {
			    int byte_step=MinLeftBlank/8;
			    int bit_shift=MinLeftBlank%8;
			    unsigned char mask=(unsigned char)(0xFF>>(7-((fChar->width-1)%8)));
			    for(row=0;row<fChar->height;row++)
						{
						memcpy(buf,iFontSpace+fChar->offset+fChar->ByteWid*row,fChar->ByteWid);
						for(x=0;x<(fChar->ByteWid-byte_step);x++)
							buf[x]=(unsigned char)((buf[x+byte_step]>>bit_shift)+
								(buf[x+byte_step+1]<<(8-bit_shift)));
						buf[x-1]&=mask;
						memcpy(iFontSpace+fChar->offset+fChar->ByteWid*row,buf,fChar->ByteWid);
						}
			    }
			}
		    }
		}
	}

