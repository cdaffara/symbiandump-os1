// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MDFPUCONFIG_INL
#define MDFPUCONFIG_INL

/**
Constructor
*/	
inline TPuConfig::TPuConfig()
	{
	}

/**
Obtain the UID identifying the structure.
@return The identifying UID of the structure.
*/	
inline TUid TPuConfig::Uid() const 
	{
	return iUid;
	}

#endif  // MDFPUCONFIG_INL
