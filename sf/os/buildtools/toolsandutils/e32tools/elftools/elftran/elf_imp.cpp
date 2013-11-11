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

#include <stdlib.h>
#include <string.h>
#include <e32std.h>
#include <e32std_private.h>
#include "h_utl.h"
#include "e32ldfmt.h"
#include "elftran.h"
#include "elffile.h"

char* E32ImageFile_ELF::CreateImportSection(ELFFile& aElfFile, TInt& aSize)
//
// get the ELFFile to do it for us.
//
	{
	TInt size;
	char * newSection = aElfFile.CreateImportSection(size);
	aSize = size;
	return newSection;
	}



