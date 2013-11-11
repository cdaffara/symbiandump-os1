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
// PDP SubConnection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PDPSCPR_FACTORY_H
#define SYMBIAN_PDPSCPR_FACTORY_H

#include <comms-infras/ss_subconnprov.h>
#include "pdp.hrh"

class CPDPSubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
	{
public:
    enum { iUid = PDPScprImplUid };
	static CPDPSubConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CPDPSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif
//SYMBIAN_PDPSCPR_FACTORY_H
