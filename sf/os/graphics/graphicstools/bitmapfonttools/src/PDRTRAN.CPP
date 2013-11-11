/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Header PDRTRAN.CPP
*
*/


#include "PDRREADR.H"

bool OutputUnicode = false;

// Input and output filenames entered in command line
int main(int argc, char *argv[])  
	{
	cout << "\nPDRTRAN V";
	cout << KPdrtranVersion << "\n";
	cout << "Copyright (c) 1998-2004 Symbian Software Ltd.\n";

	if(argc!=3)
		{
		cout << "Usage:	PDRTRAN srcfile [srcfile2 ..] destfile\n";
		cout << "where srcfile is the file containing printer models,\n";
		cout << "typeface information and character width tables, and\n";
		cout << "destfile is the pdr store file.\n";
		return -1;
		}
	
	PdrReader reader;

	for (int i = 1; i < argc - 1; i++)
		{
		if (!reader.Read(argv[i]))
			{
			cerr << "Problem encountered in file " << argv[i] << "\n";
			return -1;
			}
		}
	if (!reader.Store(argv[argc - 1]))
		{
		cerr << "Problem writing pdr file " << argv[argc - 1] << "\n";
		return -1;
		}
	cout << argv[argc-1] << " created\n";
	return 0;
	}
