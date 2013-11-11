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
 @internalTechnology
*/

#ifndef CONNECTIONQUERYFACTORY_H
#define CONNECTIONQUERYFACTORY_H

#include <e32base.h>


namespace ESock
{

class XConnectionQueryBase;


/**
Factory for generating commdb query sets
@internalComponent
*/
class CConnectionQueryFactory : public CBase
	{
public:
	enum { iUid = 0x10283004 };

	static XConnectionQueryBase* NewL(TAny* aConstructionParams);
	};

} // namespace ESock

#endif
// CONNECTIONQUERYFACTORY_H


