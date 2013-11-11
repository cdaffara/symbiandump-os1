// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMCONDITIONALCALLBACK_H__
#define __SSMCONDITIONALCALLBACK_H__

#include <e32def.h>
#include <barsc2.h>

/**
Callback used by CSsmCommandListResourceReader when a decision needs to be made
on whether to include a command in a command list or not.

@publishedPartner
@released
*/
class MSsmConditionalCallback
	{
public:
	/**
	 * Called by CSsmCommandListResourceReader to determine if a command should be added to a command list.
	 *
	 * Both the resource ID for the condition information and the resource file are provided.
	 * This allows the conditional information at the resource ID to reference other resources
	 * in the same resource file.
	 * 
	 * @param aResourceFile The resource file which contains the conditional information.
	 * @param aResourceId The resource ID containing the conditional information.
	 *  
	 * @return ETrue if the command should be added to the list, EFalse if the command should not be added to the command list.
	 * 
	 * @publishedPartner
	 * @released
	 */
	virtual TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId) = 0;
	};

#endif
