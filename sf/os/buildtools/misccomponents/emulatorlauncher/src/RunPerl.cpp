// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RunPerl.cpp : Defines the entry point for the console application.
// 
//


#include <stdlib.h>
#include <string.h>

void main(int argc, char* argv[])
	{

	char** args = new char*[argc+3];
	int index = 0;

	char* p = argv[0];
	int pl = strlen(p);
	if((pl >= 4) &&
		(*(p+pl-4)=='.') && 
		(*(p+pl-3)=='e' || *(p+pl-3)=='E') &&
		(*(p+pl-2)=='x' || *(p+pl-2)=='X') &&
		(*(p+pl-1)=='e' || *(p+pl-1)=='E'))
		*(p+pl-4)='\0';
	char* cmd = new char[strlen(p)+4];
	strcpy(cmd,p);
	strcat(cmd,".pl");

	args[index++] = "perl";
	args[index++] = "-S";
	args[index++] = cmd;

	for(int i=1; i<argc; i++)
		{
		args[index++] = argv[i];
		}

	args[index] = NULL;

	int sz = 0;

	for(i=0; args[i]; i++)
		{
		sz += strlen(args[i]) + 3;
		}
		
	char *s = new char[sz];
	strcpy(s,args[0]);
	strcat(s," ");
	strcat(s,args[1]);
	strcat(s," ");
	strcat(s,args[2]);

	for(i=3; args[i]; i++)
		{
		strcat(s," \"");
		strcat(s,args[i]);
		strcat(s,"\"");
		}

	int r = system(s);

	delete[] s;
	delete[] args;
	delete[] cmd;

	exit (r);
	}

