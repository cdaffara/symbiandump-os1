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

EffRead::EffRead(fstream& aFile,FontCompiler& aFontCompiler,Fxf* aFxf,short int* aMapSpace):
	FontRead(aFile,aFontCompiler,aFxf),
	iMapSpace(aMapSpace)
	{}

int EffRead::ReadFont()
    {
	int count=0;
	int version=0;
	char* outbuffer[(MAX_CHAR_WID+7)>>3];
	int bitpos,len1,len2;
	int ReadSize,widToGo;
	int widthofi=0;
	int widthofM=0;
	FcmCharHead* pFxf;
	LetterTableData lettertabledata;
	LetterData letterdata;
	letterdata.blx = 0;
	letterdata.bly = 0;
	letterdata.rWid = 0;
	letterdata.rHgt = 0;
	int FileSize,FontNameOffs,VariousDataOffs,LetterOffs,LetterOffsBase,seekpos;
	unsigned short int* pDest;
	unsigned short int* pSrc;
	EffVariousData VariousData;

	char* inBuffer=new char[((MAX_CHAR_WID+7)>>3)*MAX_HEIGHT];
	if(!inBuffer) return(NoMemory);
	iInputFile.read((char*)&FileSize,4);
	iInputFile.read((char*)&FontNameOffs,4);
	iInputFile.read((char*)&VariousDataOffs,4);
	iInputFile.read((char*)&VariousDataOffs,4);
	iInputFile.read((char*)&LetterOffs,4);
	iInputFile.seekg(FontNameOffs);

	char tmpName[FONT_NAME_LEN+1];
	iInputFile.read(tmpName,FONT_NAME_LEN);
	tmpName[FONT_NAME_LEN]=0;
	strcpy(iFxf->name,tmpName);
	*(iFxf->typeface)=0;

	iInputFile.seekg(VariousDataOffs);
	iInputFile.read((char*)&VariousData,sizeof(VariousData));
	version=VariousData.version;
	if(version==0) version=1;
	if(version!=1 && version!=2) return(FileFormat);
	pFxf=new FcmCharHead[MAX_CHARS];
	for(count=0;count<sizeof(FcmCharHead)*MAX_CHARS;count++)
		((char*)pFxf)[count]=0;
	int offset=0;
	int first=1;
	LetterOffsBase=LetterOffs;
	iFxf->descent= -VariousData.bly;
	iFxf->UlinePos= -VariousData.UnderLinePos-VariousData.UnderLineThickness;
	iFxf->UlineThickness=VariousData.UnderLineThickness;
	iFxf->nominal_ascent=(VariousData.Hgt/*+VariousData.LineSpacing*/)-iFxf->descent;
	iFxf->MaxChrWidth=0;
	iFxf->cell_height=VariousData.Hgt;
	for(unsigned int chNum=0;chNum<MAX_CHARS;chNum++)
		{
		if(iMapSpace)
			iInputFile.seekg((*(iMapSpace+chNum)<<2)+LetterOffsBase);
		else
			iInputFile.seekg(LetterOffs);
		iInputFile.read((char*)&lettertabledata,sizeof(LetterTableData));
		if(version==1)
			{
			lettertabledata.width>>=4;
			iInputFile.read((char*)&letterdata,sizeof(LetterData));
			LetterOffs+=sizeof(LetterData);
			}
		LetterOffs+=sizeof(LetterTableData);
		iFxf->chr[chNum]=0;
// char 0 in EFF fonts have a garbage offset so ignore them.
//		if(letterdata.offset)
		if(lettertabledata.offset && chNum>0)
			{
			seekpos=LetterOffsBase+lettertabledata.offset;
			iInputFile.seekg(seekpos);
			if(version==2)
				iInputFile.read((char*)&letterdata,sizeof(LetterData));
			if(first)
				{
				iFxf->FirstChr=chNum;
				first=0;
				}
			iFxf->n_chars=chNum+1;
			iFxf->chr[chNum]=pFxf;
			pFxf->offset=offset;
			pFxf->xOffset=letterdata.blx;
			pFxf->yOffset=letterdata.rHgt+letterdata.bly;
			pFxf->width=letterdata.rWid;
			if(chNum=='i')
				widthofi=pFxf->width;
			if(chNum=='M')
				widthofM=pFxf->width;
			pFxf->ByteWid=((pFxf->width+15)>>3)&(~1);
			pFxf->height=letterdata.rHgt;
			pFxf->move=lettertabledata.width;
			int chWid=letterdata.rWid;
			if(chWid>iFxf->MaxChrWidth)
			    iFxf->MaxChrWidth=chWid;
			ReadSize=(letterdata.rWid*letterdata.rHgt+7)>>3;
			iInputFile.read(inBuffer,ReadSize);
			bitpos=0;
			pSrc=(unsigned short int*)inBuffer;
			for(int i=0;i<letterdata.rHgt;i++)
			    {
			    pDest=(unsigned short int*)outbuffer;
			    for(int j=0;j<pFxf->ByteWid;j+=2)
			        {
			        len1=16-bitpos;
			        *pDest=(unsigned short int)((*pSrc)>>bitpos);
			        widToGo=letterdata.rWid-(j<<3);
			        if (len1>widToGo)
			            {
			            len1=widToGo;
			            len2=0;
			            }
			        else
			            {
			            pSrc++;
			            if ((len1+bitpos)>widToGo)
			                len2=widToGo-len1;
			            else
			                len2=bitpos;
			            if (len2)
			                *pDest|= (*pSrc)<<(16-bitpos);
			            }
			        *pDest&=0xFFFF>>(16-len2-len1);
			        bitpos=(bitpos+len1+len2)&0xF;
			        pDest++;
			        }
				memcpy(iFontCompiler->FontStore()+offset+pFxf->ByteWid*(letterdata.rHgt-i-1),outbuffer,pFxf->ByteWid);
			    }
			offset+=pFxf->ByteWid*letterdata.rHgt;
			}
        pFxf++;
        }
	if(widthofi && widthofM)
		iFxf->iProportional=(widthofi!=widthofM);
	return(NoError);
    }

