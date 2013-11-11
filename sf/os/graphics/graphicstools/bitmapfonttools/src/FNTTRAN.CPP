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
* Header FNTTRAN.CPP
*
*/


#include "FNTREADR.H"

bool OutputUnicode = true;

// Input and output filenames entered in command line
int main(int argc, char *argv[])
	{
	cout << "\nFNTTRAN V";
	cout << KFnttranVersion << "\n";
	cout << "Copyright (c) 1998-2005 Symbian Software Ltd.\n";

	if(argc<3)
		{
		cout << "Usage:	FNTTRAN srcfile [srcfile2 ...] destfile\n";
		cout << "where srcfile is the file containing typeface information,\n";
		cout << "and the font bitmaps, and destfile is the font store file.\n";
		return -1;
		}
	
	FontReader reader;

	for (int i = 1; i < (argc - 1); i++)
		{
		if (!reader.Read(argv[i]))
			{
			cerr << "Problem encountered in file " << argv[i] << "\n";
			return -1;
			}
		}
	if (!reader.Store(argv[argc-1]))
		{
		cerr << "Problem writing font file " << argv[argc - 1] << "\n";
		return -1;
		}
	cout << argv[argc - 1] << " created\n";	
	return 0;
	}
