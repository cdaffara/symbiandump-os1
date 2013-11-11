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
// Psion3a WVE to Series5 sound file convertor
// 
//

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <iostream>
#include <fstream>
using namespace std;
#else  //!__MSVCDOTNET__
#include <iostream.h>
#include <fstream.h>
#ifdef __CW32__
using std::streampos;
#endif //__CW32__
#endif //__MSVCDOTNET__

#include <string.h>

#if defined(__VC32__) && !defined(__MSVCDOTNET__)
#pragma warning( disable : 4710 )	// 'fn': function not inlined
#endif // old MSVC

#define KUidRecordApp 0x1000007e

#define KUid1 0x10000037
#define KUid2 0x1000006d
#define KUid3 KUidRecordApp
#define KUidCheckSum 0x5508accf

#define KUidAppStream 0x10000089
#define KUidSampleStream 0x10000052


int ErrorNotWveFile()
	{
	cout << "The specified input file is not a Series 3a WVE file" << endl ;
	return(0);
	}

int ErrorNotFound()
	{
	cout << "Unable to open input file" << endl ;
	return(0);
	}

int ReadInt32(ifstream& inStream)
	{
    char ch[4];
    inStream.read(&ch[0],4);
    return *(int*)&ch[0];
	}

int ReadInt16(ifstream& inStream)
	{
    char ch[4];
    inStream.read(&ch[0],2);
    ch[2]=0;
    ch[3]=0;
    return *(int*)&ch[0];
	}

int WriteWord(ofstream& outStream,unsigned int aWord)
	{
    outStream.write((char*)&aWord,4);
	return 0;
	}

int WriteByte(ofstream& outStream,const char aByte)
	{
    outStream.write((char*)&aByte,1);
	return 0;
	}

int WriteText(ofstream& outStream,const char* aText)
	{
	char ch;
	while((ch=*aText++)!=0)
		outStream << ch;
	return 0;
	}

void ShowHelp()
	{
	cout << "WveConv 0.04";
	cout << endl ;
	cout << "Converts a Psion 3a WVE file to an EPOC sound file" << endl ;
	cout << endl ;
	cout << "Usage: WVECONV S3AFILE.WVE [EPOCFILE]" << endl ;
	cout << endl ;
	cout << "If [EPOCFILE] is omitted, the input filename" << endl ;
    cout << "without the extension will be used" << endl ;
	}

int ConvertFile(const char* inputFile,const char* outputFile)
	{
	ifstream inStream(inputFile,ios::in|ios::binary);
	ofstream outStream(outputFile,ios::out|ios::binary);

	if(!inStream.is_open())
		return(ErrorNotFound());

	const char* appName="Record.app";
	char header[16];
	
	inStream.read(&header[0],16);

	if(strcmp(header,"ALawSoundFile**")!=0)
		return(ErrorNotWveFile());

	int compressedLength=0;
	int trailingSilence=0;
	int compressorUid=0;
	int sampleLength=0;
	int repeatCount=0;
    int versionNumber=0;
//
// extract the sample length, repeat count and trailing silence...
//
	versionNumber=ReadInt16(inStream);
	sampleLength=ReadInt32(inStream);
	trailingSilence=ReadInt16(inStream);
	repeatCount=ReadInt16(inStream);
    if(repeatCount==0) // 0 and 1 are the same on Series 3 
        repeatCount=1; 

	trailingSilence*=31250;

	compressedLength=sampleLength;

	cout << "Converting " << inputFile << " to " << outputFile << endl << endl;
    cout << "Version number  :" << versionNumber << endl ;
	cout << "Sample length   :" << sampleLength << " bytes" << endl ;
	cout << "Repeat count    :" << repeatCount << endl ;
	cout << "Trailing silence:" << trailingSilence << " uS" << endl ;

	inStream.seekg((streampos)0x20);
//
// Write out the header...
//
	unsigned int rootstreamid=0x14;
	unsigned int appstreamid=0x25;
	unsigned int samplestreamid=0x34;

// checked uid header

	WriteWord(outStream,KUid1);
	WriteWord(outStream,KUid2);
	WriteWord(outStream,KUidRecordApp);
	WriteWord(outStream,KUidCheckSum);

// root stream id

	WriteWord(outStream,rootstreamid);

//stream dictionary @ 0x14 root stream

	WriteByte(outStream,4);		// two entries in dictionary
	WriteWord(outStream,KUidSampleStream);	// sample stream
	WriteWord(outStream,samplestreamid);
	WriteWord(outStream,KUidAppStream);	// appid stream
	WriteWord(outStream,appstreamid);

// record app identifier stream @ 0x25

	WriteWord(outStream,KUidRecordApp);
	WriteByte(outStream,42);
	WriteText(outStream,appName);

//  sample header @ 0x34

	WriteWord(outStream,sampleLength);
	WriteWord(outStream,compressorUid);
	WriteWord(outStream,repeatCount-1); // repeats are zero based on Series 5 
	WriteWord(outStream,trailingSilence);
	WriteWord(outStream,compressedLength);
//
// Copy the sample data...
//
	streampos newPos=0x20; // start of sample data in 3a file...
	inStream.seekg(newPos);

	char buffer[256];
	int count;
	int actualLength=0;
	do
		{
		inStream.read(&buffer[0],256);
		if((count=inStream.gcount())!=0)
			{
			outStream.write(&buffer[0],count);
			actualLength+=count;
			}
		} while(count);
//
// should check actualLength==sampleLength...but what the heck
//
	outStream.close();
	inStream.close();
	return 0;
	}

int main(int aNumArgs,char* aArgs[])
	{
	if(aNumArgs<=1 || aArgs[1][0]=='?' || aArgs[1][0]=='/')
		{
		ShowHelp();
		return 0;
		}
	char inputFile[255];
	char outputFile[255];

	strcpy(inputFile,aArgs[1]);

	if(aNumArgs==3)
		strcpy(outputFile,aArgs[2]);

	if(aNumArgs==2 || outputFile[0]==0)
		{
		strcpy(outputFile,inputFile);
//
// remove the extension
//
		int len=strlen(outputFile);
		for(;;)
			{
			if(--len<0)
				break;
			if(outputFile[len]=='.')
				{
				outputFile[len]=0;
				break;
				}
			}
		}
	return(ConvertFile(inputFile,outputFile));
	}

