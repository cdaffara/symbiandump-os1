// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <string.h>
#include <io.h>

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <iomanip>
#else //!__MSVCDOTNET__
#include <iomanip.h>
#endif //__MSVCDOTNET__

#include "h_utl.h"
#include "h_ver.h"

// ROMMASK.EXE cl parameters
char *gRomImage=NULL;
char *gOutputRom=NULL;
int	gRomSize=0;
char gFormat='P';
const int KMaxSections=8;
int gNumberOfSections=0;
int gSectionSize[KMaxSections];
int gHeader=ETrue;
int gVerbose=EFalse;


TInt MaskPlain(TInt aRomSize,ifstream& aPsionImageFile,ofstream& aMaskRomImageFile)
//
// Create a plain mask rom image file
//
	{

	const char KMaskRomFillCharacter='\377';
	const TInt KBufferSize=1024;
	char empty[KBufferSize];
	for (TInt y=0;y<KBufferSize;y++)
		empty[y]=KMaskRomFillCharacter;
	char buffer[KBufferSize];
	TUint checksum32=0;
	TUint checksum8=0;
	for (TInt x=0;x<aRomSize;x+=KBufferSize)
		{
		memcpy(buffer,empty,KBufferSize);
		aPsionImageFile.read(buffer,KBufferSize);
		aMaskRomImageFile.write(buffer,KBufferSize);
		TInt z;
		for (z=0;z<KBufferSize;z+=4)
			checksum32+=*(TUint32*)&buffer[z];
		for (z=0;z<KBufferSize;z++)
			checksum8+=(TUint8)buffer[z];
		}

	if (gVerbose)
		{
		cout << dec;
		cout << aRomSize << " byte PLAIN\r\n";
		cout << "32 bit checksum 0x";
		cout << setw(8);
		cout << hex;
		cout << setfill('0');
		cout << checksum32 << "\r\n";
		cout << " 8 bit checksum 0x";
		cout << setw(8);
		cout << hex;
		cout << setfill('0');
		cout << checksum8 << "\r\n";
		}
	return(KErrNone);
	}

TInt MaskMotorola(TInt /*aRomSize*/,ifstream& /*aPsionImageFile*/,ofstream& /*aMaskRomImageFile*/)
//
// Create a motorola s-record mask rom image file
//
	{
	return(KErrNone);
	}

TInt MaskRom(TInt aRomSize,ifstream& aPsionImageFile,const char* aMaskRomImageFileName,const char* aMaskRomFormat)
	{
	ofstream MaskRomImageFile;

	TInt r;
	if (!MaskRomImageFile)
		{
		cout << "Error:  Cannot create mask rom image file (" << aMaskRomImageFileName << ")\r\n";
		r=KErrArgument;
		}
	else
		{
		if (gVerbose)
			{
			cout << "\r\nMask ROM image file: ";
			cout << aMaskRomImageFileName << "\r\n"; 
			}
		switch (aMaskRomFormat[0])
			{
		case 'P':
			MaskRomImageFile.open(aMaskRomImageFileName, ios::in | ios::binary);
			r=MaskPlain(aRomSize,aPsionImageFile,MaskRomImageFile);
			break;
		case 'M':
			MaskRomImageFile.open(aMaskRomImageFileName, ios::in);
			r=MaskMotorola(aRomSize,aPsionImageFile,MaskRomImageFile);
			break;
		default:
			cerr << "Error:  Rom format not recognised\r\n";
			r=KErrGeneral;
			break;
			}
		MaskRomImageFile.close();
		}
	return r;
	}


TInt getIntegerArg(int argc, char *argv[], int param)
	{

	int error=KErrNone;
	if (param>=argc)
		error=KErrArgument;
	if (error==KErrNone)
		{
		int val;
//		if (!isNumber(argv[param]))
//			return KErrArgument;

#ifdef __TOOLS2__
istringstream s(argv[param]);
#else
istrstream s(argv[param], strlen(argv[param]));
#endif

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
		s >> setbase(0);
#endif //__MSVCDOTNET__

		s>>val;
		if (!s.fail())
			return val;
		}
	cerr << "Error:  Integer argument required for ";
	cerr << argv[param-1];
	cerr << "\r\n";
	return -1;
	}

char *getStringArg(int argc, char *argv[], int param)
	{

	if (param>=argc)
		{
		cerr << "Error:  String argument required for ";
		cerr << argv[param-1];
		cerr << "\r\n";
		return NULL;
		}
	return argv[param];
	}

char getCharArg(int argc, char *argv[], int param)
	{
	
	char *p=getStringArg(argc,argv,param);
	if (p!=NULL)
		return p[0];
	return '\000';
	}

TInt processCommandLine(int argc, char *argv[])
	{

	if (argc==1)
		return KErrArgument;
	int param=1;
	while (param<argc)
		{
		switch (argv[param][1])
			{
		case 'r':
		case 'R':
			// rom name
			param++;
			gRomImage=getStringArg(argc, argv, param);
			break;
		case 's':
		case 'S':
			if (argv[param][2]=='i')
				{
				// rom size
				param++;
				gRomSize=getIntegerArg(argc, argv, param);
				if (gRomSize==-1)
					return KErrGeneral;
				}
			else
				{
				// section
				if (gNumberOfSections>=KMaxSections)
					{
					cerr << "Error:  Too many sections\r\n";
					return KErrGeneral;
					}
				param++;
				gSectionSize[gNumberOfSections]=getIntegerArg(argc, argv, param);
				if (gSectionSize[gNumberOfSections]==-1)
					return KErrGeneral;
				if (gSectionSize[gNumberOfSections]==0)
					{
					cerr << "Error:  Section is zero bytes long\r\n";
					return KErrGeneral;
					}
				if (gSectionSize[gNumberOfSections]>64)
					{
					cerr << "Error: Section too big\r\n";
					return KErrGeneral;
					}
				gNumberOfSections++;
				}
				break;
			case 'f':
			case 'F':
				param++;
				gRomSize=getCharArg(argc, argv, param);
				break;
			case 'l':
			case 'L':
				cerr << "Error:  Use -verbose instead of -log";
				break;
			case 'v':
			case 'V':
				gVerbose=ETrue;
				break;
			case 'o':
			case 'O':
				param++;
				gOutputRom=getStringArg(argc, argv, param);
				break;
			case 'n':
			case 'N':
				// -no-header
				gHeader=EFalse;
				break;
			default:
				cout << "Error:  Unrecognised switch '"<<argv[param]<<"'\r\n";
				return KErrArgument;
			}
		param++;
		}
	if (gRomImage==NULL)
		{
		cerr << "Error:  No rom image specified\r\n";
		return KErrArgument;
		}
	if (gOutputRom==NULL)
		{
		cerr << "Error:  No output rom file specified\r\n";
		return KErrArgument;
		}
	if (gRomSize>64)
		{
		cerr << "Error:  Rom too big\r\n";
		return KErrGeneral;
		}
	if (gRomSize==0)
		{
		cerr << "Error: No rom size specified\r\n";
		return KErrArgument;
		}
	if (gFormat!='P' && gFormat!='M')
		{
		cerr << "Error:  Invalid mask rom format specified\r\n";
		return KErrArgument;
		}

	return KErrNone;
	}

TInt Align1M(TInt aVal)
	{
	return (aVal+0xfffff) & 0x7ff00000;
	}

TInt main(int argc, char *argv[])
	{

	const TInt KPsionImageFileHeaderSize=0x100;

	cout << "\r\nROMMASK - Rom masker V" << MajorVersion << "." << MinorVersion << "(Build " << Build << ")\r\n";
	cout << Copyright;

	char HelpText[] = 
		"Syntax: ROMMASK -romimage <psion img file> -output <rom img name>\r\n"
		"                [-verbose] [-size <total rom size>]\r\n"
		"                [-no-header] [-format <format>] [-section <size>]*\r\n"
		"Format: MOTOROLA (ascii s-record format)\r\n"
		"        PLAIN (plain binary format) default\r\n";
	int r=processCommandLine(argc, argv);
	if (r==KErrArgument)
		{
		cout << HelpText;
		return(KErrArgument);
		}

// Open the psion image file

	ifstream PsionImageFile;
	char*& PsionImageFileName=gRomImage;

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
	PsionImageFile.open(PsionImageFileName, ios::in | ios::binary);
#else //!__MSVCDOTNET__
	PsionImageFile.open(PsionImageFileName, ios::nocreate | ios::binary);
#endif //__MSVCDOTNET__

	if (!PsionImageFile)
		{
		cerr << "Error:  Cannot open psion image file (" << PsionImageFileName << ")\r\n";
		return(KErrArgument);
		}
	
	gRomSize*=1024*1024; // in Mb
	// resolve sections to cover whole rom
	int size=0;
	int i;
	for (i=0; i<gNumberOfSections; i++)
		{
		gSectionSize[i]*=1024*1024; // in Mb
		size+=gSectionSize[i];
		}
	if (size<gRomSize)
		gSectionSize[gNumberOfSections++]=gRomSize-size;
	if (size>gRomSize)
		{
		cerr << "Error:  Sections too big for rom";
		return KErrGeneral;
		}

// Create the mask rom image file

	ofstream MaskRomImageFile;
	char*& MaskRomImageFileName=gOutputRom;

 	MaskRomImageFile.open(MaskRomImageFileName);
	if (!MaskRomImageFile)
		{
		cerr << "Error:  Cannot create mask rom image file (" << MaskRomImageFileName << ")\r\n";
		PsionImageFile.close();
		return(KErrArgument);
		}

	if (gHeader)
		{
		PsionImageFile.ignore(KPsionImageFileHeaderSize);
		int count=PsionImageFile.gcount();
		if (count!=KPsionImageFileHeaderSize)
			{
			cerr << "Error:  Corrupt Psion image file\r\n";
			return(KErrGeneral);
			}
		}

	r=KErrNone;
	for (i=0; i<gNumberOfSections; i++)
		{
		r=MaskRom(gSectionSize[i],PsionImageFile,MaskRomImageFileName,&gFormat);
		if (r!=KErrNone)
			{
			cerr << "Error:  An error occured while processing Rom image\r\n";
			return r;
			}
		// next section
		char* ptr=MaskRomImageFileName;
		while (*++ptr!=0)
			;
		*--ptr=(char)(i+'2');
		}

	PsionImageFile.close();
	return(r);
	}
