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

#ifndef COMMSDATAOBJECTFACTORY_H
#define COMMSDATAOBJECTFACTORY_H

#include <e32base.h>

namespace ESock
{

class XCommsDataObject;


/**
Factory for generating commdb query sets
@internalComponent
*/
class CCommsDataObjectFactory : public CBase
	{
public:
	enum { iUid = 0x10282FF3 };

	static XCommsDataObject* NewL(TAny* aConstructionParams);
	};


} // namespace ESock


#endif
// COMMSDATAOBJECTFACTORY_H


