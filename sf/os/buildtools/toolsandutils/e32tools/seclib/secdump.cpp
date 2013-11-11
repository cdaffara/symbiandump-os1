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
// e32tools/seclib/secdump.cpp
// Exercise seclib
// 
//

#include "h_utl.h"
#include "h_ver.h"

#include <e32def.h>
#include <e32def_private.h>
#include "seclib.h"

#include <stdio.h>

int main(int argc, char *argv[])
	{
	Print(EAlways,"secdump");
  	Print(EAlways," V%02d.%02d (Build %03d)\n",MajorVersion,MinorVersion,Build);
  	Print(EAlways,Copyright);

	if (argc < 1  || !argv[1])
		{
		Print(EAlways, "Usage: %s filename\n\n", argv[0]);
		Print(EAlways, "Dumps the capabilites in the file filename\n\n");
		return KErrNotFound;
		}

	SBinarySecurityInfo info;

	TInt err = GetSecurityInfo(argv[1], info);

	if (!err)
		{
		for (TInt count = 0; count < KCapabilitySetMaxSize; ++count)
			{
			Print(EAlways, "Byte #%d\t0x%02x\n", count, info.iCapabilities[count]);
			}

		Print(EAlways, "Secure ID:\t0x%08x.\n", info.iSecureId);

		Print(EAlways, "Vendor ID:\t0x%08x.\n", info.iVendorId);

		Print(EAlways, "Input file is%s an E32Image file.\n", (info.iE32Image ? "" : " not"));
		}
	else
		{
		Print(EAlways, "ERROR: %s failed with %d.\n", argv[0], err);
		}

	return err;
	}
