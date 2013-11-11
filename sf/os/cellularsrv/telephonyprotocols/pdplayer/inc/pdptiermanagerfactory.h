// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_PDPTIERMANAGERFACTORY_H
#define SYMBIAN_PDPTIERMANAGERFACTORY_H

#include <comms-infras/ss_tiermanager.h>
#include "pdp.hrh"

class CPDPTierManagerFactory : public ESock::CTierManagerFactoryBase
	{
public:
    enum { EUid = PDPTierManagerImplUid };
	static CPDPTierManagerFactory* NewL(TAny* aParentContainer);

protected:
	CPDPTierManagerFactory(TUid aTierTypeId, TUid aFactoryUid, ESock::CTierManagerFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif // SYMBIAN_PDPTIERMANAGERFACTORY_H

