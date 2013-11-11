// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @prototype
*/

#ifndef MDFEXTENDEDSETTINGS_H
#define MDFEXTENDEDSETTINGS_H

class TPuConfig;

/**
Custom interface for setting configuration structures into MDF
*/
class MMDFExtendedSettings
	{
public:
	/**
	Sets an extended configuration structure
	@param aConfig The configuration structure to be used
	@return System wide error code
	*/
	virtual TInt SetExtendedConfig(const TPuConfig& aConfig) = 0;
	};

#endif // MDFEXTENDEDSETTINGS_H
