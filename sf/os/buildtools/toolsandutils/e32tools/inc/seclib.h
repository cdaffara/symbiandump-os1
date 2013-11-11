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
// e32tools/inc/seclib.h
// Get the capabilities of an emulator / e32image.
// Image security information structure.
// 
//

/**
 @internalTechnology
 @prototype
*/
struct SBinarySecurityInfo
	{
	TUint32			iSecureId;
	TUint32			iVendorId;
	TUint8			iCapabilities[KCapabilitySetMaxSize];
	TBool			iE32Image;
	};

/**
 * Extracts security information from an image.
 *
 * @internalTechnology
 * @prototype
 */
TInt GetSecurityInfo(const char* aFileName, SBinarySecurityInfo& aInfo);
#ifndef __LINUX__
TInt GetSecurityInfo(const wchar_t* aFileName, SBinarySecurityInfo& aInfo);
#endif
