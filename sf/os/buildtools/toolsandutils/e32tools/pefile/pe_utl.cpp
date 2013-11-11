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

#include <e32std.h>
#include <e32std_private.h>
#include "pe_file.h"
#include <string.h>
#include "e32image.h"

TInt PEFile::CmpSectionName(PIMAGE_SECTION_HEADER apSectionHeader, char *aName)
//
// Returns true if the name of the pe section is the same as aName
//
	{

	return (strnicmp((const char *)apSectionHeader->Name, aName, IMAGE_SIZEOF_SHORT_NAME)==0);
	}

TInt PEFile::VirtualAddressInSection(TUint aVA, PIMAGE_SECTION_HEADER aHeader)
//
// Returns true if the virtual address is in the section
//
	{

	TUint start = iLinkedBase + aHeader->VirtualAddress;
	TUint finish = start + aHeader->Misc.VirtualSize;
	return (aVA>=start) && (aVA<finish);
	}

TUint PEFile::DistanceFromSection(TUint aVA, PIMAGE_SECTION_HEADER aHeader)
//
// Returns the minimum distance from aVA to any address in the specified section
//
	{

	TUint start = iLinkedBase + aHeader->VirtualAddress;
	TUint finish = start + aHeader->Misc.VirtualSize;
	if (aVA>=start)
		{
		if (aVA<finish)
			return 0;
		return aVA - finish + 1;
		}
	return start - aVA;
	}

TInt PEFile::FindSectionByVa(TUint aVA, TUint aTryToBeClever)
	{
	TInt i;
	TInt s = -1;
	if (aTryToBeClever == 0)
		{
		for (i=0; i<KNumberOfSections; i++)
			if (iSectionHeader[i])
				if (VirtualAddressInSection(aVA, iSectionHeader[i]))
					s=i;
		}
	if (aTryToBeClever == 1)
		{
		// Find the minimum distance from the specified address to any section
		TUint dist = KMaxTUint;
		for (i=0; i<KNumberOfSections; i++)
			{
			if (!iSectionHeader[i])
				continue;
			TUint d = DistanceFromSection(aVA, iSectionHeader[i]);
			if (d < dist)
				{
				dist = d;
				s = i;
				}
			else if (d == dist)
				{
				s = -1;	// Ambiguous :-(
				}
			}
		if (dist >= 0x1000u)
			s = -1;		// too far for comfort
		}
	return s;
	}
