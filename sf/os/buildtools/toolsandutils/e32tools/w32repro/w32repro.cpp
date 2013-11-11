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
// Based on PREPRO.CPP and ROMBUILD.CPP
// 
//

#define WIN32_LEAN_AND_MEAN
#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union
#include <windows.h>
#include <winbase.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "h_utl.h"
#include "h_ver.h"

// need to pretend we've done e32image.h, to avoid clashes with winnt.h
#define __E32IMAGE_H__
struct E32ImageFile
	{
	};
class CBytePair;
#include "r_rom.h"

#define READ_BUFFER_SIZE 0x1000		// allows for exuberant T_REPRO without delays in it
#define WRITE_BUFFER_SIZE 0x1000

TRomLoad TheRomHeader;
TUint ImageDataSize=0;
TUint FileSize=0;
TText PortNumber='1';
TUint BaudRate=115200;
TBool Kick=EFalse;
TBool UseHex=EFalse;
TBool Verbose=EFalse;
TBool RawImage=EFalse;
TText* BootstrapName=NULL;

const TUint KReproWrapperSize = 0x100;	// REPRO protocol assumes a wrapper size of 256 bytes

HANDLE comPort;
TUint32 BytesWritten;

TText *processCommandLine(int argc, char *argv[])
//
// Process the command line arguments, printing a helpful message if none are supplied
//
	{

	char HelpText[] = 
		"* Syntax: W32REPRO [options] filename[.bin]\n"
		"* \n"
		"* Option: -P<n>        port number, defaults to COM1,\n"
		"* Option: -K           kick the other end, to force another repro attempt\n"
		"* Option: -B<rate>     baud rate, minimum 9600, defaults to 115200\n"
		"* Option: -RAW         raw image with no header\n"
		"* Option: -BOOT <file> bootstrap with <file> transmitted at 9600 baud\n"
		"* Option: -HEX         use base 16 (for use with ReproC)\n"
		"* Option: -V           display raw protocol messages\n"
		"* \n"
		"* All messages from W32REPRO begin with '*', every thing else comes from the\n"
		"* machine being reprogrammed.\n";

	TText *filename=NULL;
	if (argc == 1)
		{
		cout << HelpText;
		return NULL;
		}
	for (int i=1; i<argc; i++)
		{
		strupr(argv[i]);
		if ((argv[i][0] == '-') || (argv[i][0] == '/'))
			{
			if (strcmp(argv[i],"-RAW")==0)
				{
				RawImage=ETrue;
				}
			else if (strcmp(argv[i],"-HEX")==0)
				{
				UseHex=ETrue;
				}
			else if (strcmp(argv[i],"-BOOT")==0)
				{
				if (++i==argc)
					{
					cout << "**** Missing argument for -BOOT\n*\n";
					cout << HelpText;
					return NULL;
					}
				BootstrapName=(TText*)argv[i];
				}
			else if (argv[i][1] == 'P')
				{
				PortNumber=argv[i][2];
				}
			else if (argv[i][1] == 'K')
				{
				Kick=ETrue;
				}
			else if (argv[i][1] == 'V')
				{
				Verbose=ETrue;
				}
			else if (argv[i][1] == 'B')
				{
				TInt rate=atoi(argv[i]+2);
				if (rate>=9600)
					{
					BaudRate=rate;
					}
				else
					{
					cout << "**** Invalid baud rate: " << argv[i] << "\n*\n";
					cout << HelpText;
					return NULL;
					}
				}
			else if (argv[i][1] == '?')
				{
				cout << HelpText;
				return NULL;
				}
			else
				{
				cout << "**** Unrecognised argument " << argv[i] << "\n*\n";
				cout << HelpText;
				return NULL;
				}
			}
		else // Must be the image filename
			filename=(TText *)argv[i];
		}
	if (filename==NULL)
		{
		cout << "**** Missing image filename\n*\n";
		cout << HelpText;
		}
	return filename;
	}

TInt openComPort()
//
// Open the com port and configure it
//
	{
	char port[5]="COM1";
	port[3]=PortNumber;
	comPort=CreateFile(port,GENERIC_READ+GENERIC_WRITE,0,0,OPEN_EXISTING,0,NULL);
	if (comPort==INVALID_HANDLE_VALUE)
		return Print(EError,"* Cannot open %s\n",port);

	DCB settings;
	if (!GetCommState(comPort,&settings))
		return Print(EError,"* Cannot read settings for %s\n",port);

	if (!SetupComm(comPort,READ_BUFFER_SIZE,WRITE_BUFFER_SIZE))
		return Print(EError,"* Cannot set buffer sizes for %s\n",port);

	settings.fBinary=TRUE;
	settings.fParity=FALSE;
	settings.fAbortOnError=TRUE;	// overrides EV_ERR
	settings.BaudRate=BaudRate;
	settings.ByteSize=8;
	settings.Parity=NOPARITY;
	settings.StopBits=ONESTOPBIT;
	settings.fRtsControl=RTS_CONTROL_ENABLE;
	settings.fDtrControl=DTR_CONTROL_ENABLE;
	settings.fOutxCtsFlow=FALSE;
	settings.fOutxDsrFlow=FALSE;
	settings.fDsrSensitivity=FALSE;
	settings.fOutX=FALSE;		// no XON/XOFF for transmission
	settings.fInX=FALSE;		// no XON/XOFF for reception
	settings.fNull=FALSE;		// don't discard null bytes

	settings.EvtChar='\001';	// REPRO command separator

	if (!SetCommState(comPort,&settings))
		return Print(EError,"* Cannot configure %s\n",port);
	
	if (!SetCommMask(comPort,EV_RXFLAG+EV_ERR))
		return Print(EError,"* Cannot set CommMask for %s\n",port);

	COMMTIMEOUTS timeouts = {
		//20,0,0,	// allow up to 20 milliseconds between characters, i.e. buffer them properly
		MAXDWORD,0,0,	// return immediately
		0,0	// no write timeouts
		};
	if (!SetCommTimeouts(comPort,&timeouts))
		return Print(EError,"* Cannot set timeouts for %s\n",port);

	if (!SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST))
		Print(EError,"* Failed to raise priority of this thread err=%d\n",GetLastError());
	if (!SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS))
		Print(EError,"* Failed to raise priority of this process err=%d\n",GetLastError());

	Print(EScreen,"* Using %s at %d baud\n\n",port,BaudRate);
	return KErrNone;
	}

BOOL WriteToComPort(char* data, DWORD length, char* comment)
	{
	if (Verbose)
		{
		if (comment==NULL)
			Print(EScreen, "* TX=%*s\n", length, data);
		else
			Print(EScreen, "* TX <%d bytes of %s>\n", length, comment);
		}
	return WriteFile(comPort, data, length, &BytesWritten, NULL);
	}

TInt Bootstrap9600()
	{
	DCB settings;
	if (!GetCommState(comPort,&settings))
		return Print(EError,"* Cannot read COM settings\n");

	settings.BaudRate=9600;
	if (!SetCommState(comPort,&settings))
		return Print(EError,"* Cannot reconfigure to 9600 baud\n");
	
	FILE* bootstrapFile=fopen((const char*)BootstrapName,"rb");
	if (bootstrapFile==NULL)
		return Print(EError,"* Cannot open bootstrap file %s for input (errno=%d)\n",BootstrapName,errno);

	Print(EScreen,"* Sending bootstrap %s at 9600 baud\n",BootstrapName,BaudRate);

	char bootdata[WRITE_BUFFER_SIZE];
	TUint32 imageBytes=0;

	while (!feof(bootstrapFile))
		{
		imageBytes=fread(bootdata,1,WRITE_BUFFER_SIZE,bootstrapFile);
		if (imageBytes==0 && feof(bootstrapFile))
			break;
		if (imageBytes!=WRITE_BUFFER_SIZE && ferror(bootstrapFile))
			{
			return Print(ESevereError,"* Read only %d bytes of bootstrap err=%d\n",imageBytes,ferror(bootstrapFile));
			}
		if (!WriteToComPort(bootdata,imageBytes,"bootstrap data"))
			return Print(ESevereError,"* Wrote only %d of %d bytes of bootstrap err=%d\n",
				BytesWritten,imageBytes,GetLastError());
		}
	fclose(bootstrapFile);

	settings.BaudRate=BaudRate;
	if (!SetCommState(comPort,&settings))
		return Print(EError,"* Cannot reconfigure to %d baud\n",BaudRate);
	
	Print(EScreen,"* Bootstrap downloaded\n\n");

	return KErrNone;
	}

TInt main(int argc, char *argv[])
	{
	TInt err=KErrNone;

	Print(EScreen,"\n* W32REPRO - Win32 version of PREPRO");
  	Print(EScreen," V%02d.%02d (Build %03d)\n",MajorVersion,MinorVersion,Build);
  	Print(EScreen,"* %s",Copyright);

	TText *imageFileName = processCommandLine(argc, argv);
	if (imageFileName==NULL)
		return KErrGeneral;

	FILE* romFile=fopen((const char*)imageFileName,"rb");
	if (romFile==NULL)
		return Print(EError,"* Cannot open ROM Image file %s for input (errno=%d)\n",imageFileName,errno);

	if (RawImage)
		TheRomHeader.wrapSize=0;
	else
		{
		if (fread(&TheRomHeader,sizeof(TheRomHeader),1,romFile)!=1)
			return Print(EError,"* Cannot read ROM Image header\n");
		if (TheRomHeader.wrapSize!=KRomWrapperSize)
			return Print(EError,"* Incorrect ROM header - wrong wrapper size\n");
		}
	if (fseek(romFile,0,SEEK_END)!=0)
		return Print(EError,"* Cannot seek in ROM Image file\n");
	FileSize=ftell(romFile);
	ImageDataSize=FileSize-TheRomHeader.wrapSize;

	Print(EAlways,"\n* ROM Image %s - 0x%06x bytes\n",imageFileName,ImageDataSize);

	err=openComPort();
	if (err!=KErrNone)
		return err;

	if (BootstrapName != NULL)
		{
		err=Bootstrap9600();
		if (err!=KErrNone)
			return err;
		}

	char romdata[WRITE_BUFFER_SIZE];
	if (Kick)
		{
		memset(romdata,'!',64);		// string of non-numeric characters, won't harm old REPRO
		WriteToComPort(romdata,64,NULL);
		}
	//
	// Wait around for REPRO on the other end to send us commands
	//
	char command[READ_BUFFER_SIZE+1];
	char* cp=command;
	TInt length=READ_BUFFER_SIZE;
	TUint expectedOffset=0;
	TInt done=0;
	while (!done)
		{
		TUint32 bytesRead=0,imageBytes=0,offset=0;

		TUint32 event;
		if (!WaitCommEvent(comPort,&event,NULL))
			{
			if (GetLastError()!=ERROR_OPERATION_ABORTED)
				Print(EAlways,"\n* Unexpected WaitCommEvent failure %d event %x\n",GetLastError(),event);
			TUint32 commError;
			if (!ClearCommError(comPort,&commError,NULL))
				{
				Print(ESevereError,"\n* Failed to clear CommError - give up now!\n");
				return KErrGeneral;
				}
			if (commError!=CE_OVERRUN)
				Print(EAlways,"\n* Unexpected comms error %x\n",commError);
			}
		if (!ReadFile(comPort,cp,length,&bytesRead,NULL))
			{
			if (GetLastError()!=ERROR_OPERATION_ABORTED)
				Print(EAlways,"\n* Unexpected ReadFile failure %d bytes %d\n",GetLastError(),bytesRead);
			}
		if (bytesRead==0)
			continue;

		char* next;
		char* end = cp+bytesRead;
		*end='\0';	// stick a terminator on the end, just in case

		for (cp=(char*)command; (next=(char*)memchr(cp,'\001',end-cp))!=NULL ;cp=next+1)
			{
			*next='\0';	// drop the terminator
			if (Verbose)
				Print(EScreen, " * RX=%s\n", cp);
			switch (cp[0])
				{
			case 'D':	// disconnect after successful REPRO
				Print(EScreen,"* Disconnect\n");
				done=1;
				break;
			case 'M':	// print message
				Print(EScreen,"%s",cp+1);
				break;
			case 'P':	// panic
				Print(ESevereError,"%s",cp+1);
				break;
			case 'R':	// request for data from the image at specified address
				if (end-next>1)
					break;	// must be the last command in the buffer
				offset=strtoul(cp+1,NULL,UseHex?16:10)-KReproWrapperSize; // REPRO assumes wrapSize=256	
				if ((offset&4095)!=0)
					{
					Print(ESevereError,"* Image offset %x not a multiple of 4k (%s)\n", offset,cp);
					break;
					}
				if (offset>expectedOffset)
					{
					Print(ESevereError,"* Image offset %x should have been %x\n", offset,expectedOffset);
					break;
					}
				Print(EScreen,"%x       \r",offset);		// in case we lost the message
				expectedOffset=offset+WRITE_BUFFER_SIZE;	// what we expect next time
				offset+=TheRomHeader.wrapSize;	// offset into the file
				if (fseek(romFile,offset,SEEK_SET)!=0)
					{
					Print(ESevereError,"* Can't seek to file offset %x", offset);
					break;
					}

				memset(romdata,0xff,WRITE_BUFFER_SIZE);
				imageBytes=fread(romdata,1,WRITE_BUFFER_SIZE,romFile);
				if (imageBytes!=WRITE_BUFFER_SIZE && offset+imageBytes!=FileSize)
					{
					Print(ESevereError,"* Read only %d bytes of image data err=%d\n",imageBytes,ferror(romFile));
					break;
					}
				if (!WriteToComPort(romdata,WRITE_BUFFER_SIZE,"image data"))
					Print(ESevereError,"* Wrote only %d bytes of image data err=%x\n",BytesWritten,GetLastError());
				break;
			case 'S':	// request for the size of the image
				if (end-next>1)
					break;	// must be the last command in the buffer
				if (next-cp==1)
					{
					sprintf((char*)romdata,"%010d\n",ImageDataSize+KReproWrapperSize);
					if (!WriteToComPort(romdata,strlen(romdata),NULL) 
							|| BytesWritten!=strlen(romdata))
						Print(ESevereError,"* Failed to write file size\n");
					expectedOffset=0;	// because we are starting again
					break;
					}
				// otherwise fall through
			default:
				Print(EAlways,"\n* Unrecognised command >%s<\n", cp);
				}
			}
		if (cp<end)	// copy trailing characters back to the start of the buffer
			memmove(command,cp,end-cp);
		cp=command+(end-cp);
		length=command+READ_BUFFER_SIZE-cp;
		}
	return KErrNone;
	}
