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
// PDP Connection Provider factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#include "PDPCPRFactory.h"
#include "PDPCPR.h"


//-=========================================================
//
// CPDPConnectionProviderFactory methods
//
//-=========================================================
CPDPConnectionProviderFactory* CPDPConnectionProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CPDPConnectionProviderFactory(TUid::Uid(CPDPConnectionProviderFactory::iUid), *reinterpret_cast<ESock::CConnectionFactoryContainer*>(aParentContainer));
    }

CPDPConnectionProviderFactory::CPDPConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer)
:	ESock::CConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    }

ESock::ACommsFactoryNodeId* CPDPConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& /* aQuery */)
    {
	ESock::CConnectionProviderBase* provider = CPDPConnectionProvider::NewL(*this);
	
	
	return provider;
    }






