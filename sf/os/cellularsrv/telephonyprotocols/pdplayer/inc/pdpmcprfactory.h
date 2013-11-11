// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Pdp MCPR Factory
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PDPMCPR_FACTORY_H
#define SYMBIAN_PDPMCPR_FACTORY_H

#include <comms-infras/ss_metaconnprov.h>
#include "pdp.hrh"

class CPdpMetaConnectionProviderFactory : public ESock::CMetaConnectionProviderFactoryBase
	{
public:
    enum { iUid = PDPMcprImplUid };
	static CPdpMetaConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CPdpMetaConnectionProviderFactory(TUid aFactoryId, ESock::CMetaConnectionFactoryContainer& aParentContainer);
	ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif
//SYMBIAN_PDPMCPR_FACTORY_H
