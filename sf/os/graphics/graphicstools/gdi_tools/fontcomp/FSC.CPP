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

FscRead::FscRead(fstream& aFile,FontCompiler& aFontCompiler,Fxf* aFxf):
	FontRead(aFile,aFontCompiler,aFxf)
	{}

int FscRead::DoCom(int aSecondPass)
/*
Takes a command and takes appropriate action
Returns -ve if an error occured
        0 for OKAY
        +ve number for new current character number
*/
    {
    int ret=0,num=0,neg=0;
	int i=0;
    for (;i<iInputBufLen;i++)
        {
		char chr=iInputBuf[i];
        if (num==0 && chr=='-')
            neg=1;
        else if (chr>='0' && chr<='9')
            num=num*10+chr-'0';
        }
    if (neg)
        num= -num;
    if (iInputBuf[1]=='c')
        return(num);
    if (aSecondPass)
        ret=0;
    else switch(iInputBuf[1])
        {
        case 'd':
            iFxf->descent=num;
            break;
        case 'a':
            iFxf->nominal_ascent=num;
            break;
        case 'm':
            iFxf->max_info_width=num;
            break;
        case 'h':
            if ((iFxf->cell_height=num)>MAX_HEIGHT)
                ret=Parameter;
            if (iFxf->nominal_ascent==0)
                iFxf->nominal_ascent=iFxf->cell_height-iFxf->descent;
            break;
        case 'n':       /* Font data label name */
			{
			int pos=0;
			while(iInputBuf[pos]!=' ')
				pos++;
			while(iInputBuf[pos]==' ')
				pos++;
			if(iInputBufLen-pos>FONT_NAME_LEN)
		        return(FileFormat);
			memcpy(iFxf->name,&iInputBuf[pos],iInputBufLen-pos);
			(iFxf->name)[iInputBufLen-pos]=0;
			}
            break;
        case 't':       /* Font data typeface */
			{
			int pos=0;
			while(iInputBuf[pos]!=' ')
				pos++;
			while(iInputBuf[pos]==' ')
				pos++;
			if(iInputBufLen-pos>FONT_NAME_LEN)
		        return(FileFormat);
			memcpy(iFxf->typeface,&iInputBuf[pos],iInputBufLen-pos);
			(iFxf->typeface)[iInputBufLen-pos]=0;
			}
            break;
        case 'b':
			iFxf->iBold=1;
            break;
        case 'i':
			iFxf->iItalic=1;
            break;
        case 'f':
			// not a lot
            break;
        case 'v':
			iFxf->iUid=num;
            break;
        case 'o':
			iOverHang=num;
            break;
        case 'u':
			iUnderHang=num;
            break;
        default:
            ret=FileFormat;
            break;
        }
    return(ret);
    }

char* FscRead::ScanLine(int& aLen)
/* Scan the input line for the first '0' or '1' character and return a descriptor pointing to it,
returns a zero length descriptor if not found.
*/
    {
	aLen=0;
	if((iInputBuf[0]!='0' && iInputBuf[0]!='1') || iInputBufLen==0)
		return(NULL);
	while(iInputBuf[aLen]=='0' || iInputBuf[aLen]=='1')
		aLen++;
	return(iInputBuf);
    }


int FscRead::ReadLine()
	{
	int pos=0;
	while(iFileBuf[iFileBufPos+pos]!='\n' && iFileBufPos+pos<iFileBufLen)
		pos++;
	if(iFileBufPos+pos==iFileBufLen) return(1);
	memcpy(iInputBuf,&iFileBuf[iFileBufPos],pos);
	iInputBufLen=pos-1;
	iFileBufPos+=pos+1;
	if(iFileBufPos>=iFileBufLen) return(1);
	return(0);
	}

FontRead::FontRead(fstream& aFile,FontCompiler& aFontCompiler,Fxf* aFxf):
	iInputFile(aFile),
	iFontCompiler(&aFontCompiler),
	iFxf(aFxf)
	{
	}

int FscRead::ReadFont()
	{
	iInputFile.seekg(0,ios::end);
	iFileBufLen=iInputFile.tellg();
	iInputFile.seekg(0);
	iFileBufPos=0;
	iFileBuf=new char[iFileBufLen];
	iInputFile.read(iFileBuf,iFileBufLen);
	int ret=Pass1();
	if(ret) return(ret);
	return(Pass2());
	}

int FscRead::Pass1()
    {
    int n_row=0;            	// counts row in character picture
    int ret=0;
    int specChr=0;
	int isCharBody=0;
	int lastLine=0;
	int widthofi=0;
	int widthofM=0;

	iFxf->MaxChrWidth=0;
	iFxf->cell_height=0;
	iFxf->UlinePos=0;
	iFxf->UlineThickness=0;
	iFxf->nominal_ascent=0;
	iFxf->descent=0;
	iFxf->chr_seg=0;
	iFxf->FirstChr=0;
	iFxf->n_chars=0;
	iFxf->max_info_width=0;
	iFxf->flags=0;
	iFxf->special=0;
	iFxf->ByteWid=0;
	iFxf->UseWords=0;
	iFxf->iBold=0;
	iFxf->iItalic=0;
	iFxf->iProportional=0;
	iFxf->iSerif=0;
	iFxf->iSymbol=0;
	iFxf->iUid=0;

	iUnderHang=0;
    iOverHang=0;
    iChar=new FcmCharHead[MAX_CHARS];
    for(int letter=0;letter<MAX_CHARS;letter++)
        iFxf->chr[letter]=NULL;
//**************************************************
// First pass. Read header info & character widths *
//**************************************************
    do
        {
	    int width=0;         	// width of current character picture

		lastLine=ReadLine();
		isCharBody=0;
        if (iInputBufLen>0 && iInputBuf[0]=='*')
            {
            if ((ret=DoCom(0))<0)
                  return(FileFormat);
            else if (ret)
                {
                for(;iFxf->n_chars<ret;iFxf->n_chars++)
                    iFxf->chr[iFxf->n_chars]=NULL;
                specChr=iFxf->n_chars;
                }
            }
        else
            {
			int len=0;
		    char* ptr=ScanLine(len);
            if (len)
                {
				isCharBody=1;
                if (iFxf->FirstChr<0)
                    iFxf->FirstChr=iFxf->n_chars;
                if (n_row==0)
                    {
                    for (width=0;width<len && (ptr[width]=='0' || ptr[width]=='1');width++);
                    if (iFxf->n_chars>255)
                        return(FileFormat);
                    iFxf->chr[iFxf->n_chars]=iChar;
                    iFxf->n_chars++;
                    iChar->xOffset= -iUnderHang;
                    iChar->width=width;
                    iChar->ByteWid=((iChar->width+15)>>3)&(~1);
                    iChar->move=width-iUnderHang-iOverHang;
					if(iFxf->n_chars=='i')
						widthofi=iChar->move;
					else if(iFxf->n_chars=='M')
						widthofM=iChar->move;
                    iUnderHang=0;
                    iOverHang=0;
                    if (width>iFxf->MaxChrWidth)
                        iFxf->MaxChrWidth=width;
                    }
                n_row++;
                }
            }
        if ((n_row!=0 && !isCharBody) || (lastLine && isCharBody))
            {
            if (n_row>iFxf->cell_height)
                return(FileFormat);
            iChar->height=n_row;
            iChar->yOffset=iFxf->cell_height-iFxf->descent;
            specChr++;
            n_row=0;
            width=0;
            iChar++;
            }
        } while(!lastLine);
    if (iFxf->cell_height==0)
        return(FileFormat);
	if(widthofi && widthofM)
		iFxf->iProportional=(widthofi!=widthofM);
	return(NoError);
	}

int FscRead::Pass2()
/******************************************************/
/* Second pass. Read in actual picture data this time */
/******************************************************/
	{
	unsigned int bit;            	// Used to set bits in pic
	int ret,chNum;
	int n_row;            	// counts row in character picture
	int offset=0;
	int isCharBody;
	int lastLine;

	iFileBufPos=0;
	n_row=0;
	chNum=0;
	do	{
		lastLine=ReadLine();
		isCharBody=0;
        if (iInputBufLen>0 && iInputBuf[0]=='*')
            {
            if ((ret=DoCom(1))>0)
                {
                if (ret<chNum)
                    return(FileFormat);
                chNum=ret;
                }
            }
		else
			{
			int len=0;
			char* ptr=ScanLine(len);
			if (len)
                {
				isCharBody=1;
                if (n_row==0)
                    {
                    iChar=iFxf->chr[chNum];
                    chNum++;
                    }
				unsigned short int* pDest=(unsigned short int*)(iFontCompiler->FontStore()+offset+iChar->ByteWid*n_row);
                bit=1;
                *pDest=0;
                for (int width=0;width<len && (ptr[width]=='0' || ptr[width]=='1');width++)
                    {
                    if (ptr[width]=='1')
                        *pDest|=bit;
                    if (bit==0x8000)
                        {
                        bit=1;
                        pDest++;
                        *pDest=0;
                        }
                    else
                        bit<<=1;
                    }
                n_row++;
                }
            }
        if ((n_row!=0 &&  !isCharBody) || (lastLine && isCharBody))
            {
            iChar->offset=offset;
			offset+=iChar->ByteWid*n_row;
            n_row=0;
            }
        } while(!lastLine);
	return(NoError);
    }
