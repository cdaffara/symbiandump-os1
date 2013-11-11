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
// PDP Connection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PDPCPR_FACTORY_H
#define SYMBIAN_PDPCPR_FACTORY_H

#include <comms-infras/ss_connprov.h>
#include <ss_fact.h>
#include "pdp.hrh"

class CPDPConnectionProviderFactory : public ESock::CConnectionProviderFactoryBase
	{
public:
    enum { iUid = PDPCprImplUid };
	static CPDPConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CPDPConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif //SYMBIAN_PDPCPR_FACTORY_H
