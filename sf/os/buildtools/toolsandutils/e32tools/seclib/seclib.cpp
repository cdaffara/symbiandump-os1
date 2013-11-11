// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// e32tools/seclib/seclib.cpp
// Get the capabilities of an emulator / e32image.
// 
//

#include <e32cmn.h>
#include <e32ldr.h>
#include "seclib.h"

#include "e32image.h"
#include "h_utl.h"

#define __E32DEBUG_H__
#include "setcap.h"


GLDEF_C TInt GetSecurityInfo(const char* aFileName, SBinarySecurityInfo& aInfo)
	{
	// Check we have a filename
	if (!aFileName)
		{
		return KErrNotFound;
		}

	// Zero the capabilities
	memset(aInfo.iCapabilities, 0, KCapabilitySetMaxSize);

	// We don't want to update the image's capabilities
	CapabilitySet = EFalse;

	// Try opening the file as an E32Image
	E32ImageFile f;
	TInt r = f.Open(aFileName);

	if (r==KErrCorrupt || r==KErrNotSupported || r==1)
		{
#ifndef __LINUX__
		// Try emulator format
		aInfo.iE32Image = EFalse;

		HANDLE file = CreateFileA(aFileName, 
		                          GENERIC_READ|GENERIC_WRITE, 
								  FILE_SHARE_READ|FILE_SHARE_WRITE, 
								  NULL, 
								  OPEN_EXISTING, 
								  0, 
								  NULL);
		if (file == INVALID_HANDLE_VALUE)
			{
			return GetLastError();
			}

		r = SetCap(file);
#endif
		}
	else
		{
		// Extract caps from E32Image
		aInfo.iE32Image = ETrue;

		r = SetCap(f.iOrigHdr);
		}

	if (r == KErrNone)
		{
		aInfo.iSecureId = SecureId.iId;
		aInfo.iVendorId = VendorId.iId;
		*(SCapabilitySet*)(aInfo.iCapabilities) = Capability;
		}

	return r;
	}


#ifndef __LINUX__
GLDEF_C TInt GetSecurityInfo(const wchar_t* aFileName, SBinarySecurityInfo& aInfo)
	{
	// Check we have a filename
	if (!aFileName)
		{
		return KErrNotFound;
		}

	// Zero the capabilities
	memset(aInfo.iCapabilities, 0, KCapabilitySetMaxSize);

	// We don't want to update the image's capabilities
	CapabilitySet = EFalse;

	// Try opening the file as an E32Image
	E32ImageFile f;
	TInt r = f.Open(aFileName);

	if (r==KErrCorrupt || r==KErrNotSupported)
		{
		// Try emulator format
		aInfo.iE32Image = EFalse;

		HANDLE file = CreateFileW(aFileName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (file == INVALID_HANDLE_VALUE)
			{
			return GetLastError();
			}

		r = SetCap(file);
		}
	else
		{
		// Extract caps from E32Image
		aInfo.iE32Image = ETrue;

		r = SetCap(f.iOrigHdr);
		}

	if (r == KErrNone)
		{
		aInfo.iSecureId = SecureId.iId;
		aInfo.iVendorId = VendorId.iId;
		*(SCapabilitySet*)(aInfo.iCapabilities) = Capability;
		}

	return r;
	}
#endif
