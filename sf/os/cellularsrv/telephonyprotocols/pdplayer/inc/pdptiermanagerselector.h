// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef SYMBIAN_PDPTIERMANAGERSELECTOR_H
#define SYMBIAN_PDPTIERMANAGERSELECTOR_H

#include <comms-infras/simpleselectorbase.h>


class TPdpSelectorFactory
/**
@internalComponent
@prototype
*/
	{
public:
	static ESock::MProviderSelector* NewSelectorL(const Meta::SMetaData& aSelectionPreferences);
	};

#endif // SYMBIAN_PDPTIERMANAGERSELECTOR_H

