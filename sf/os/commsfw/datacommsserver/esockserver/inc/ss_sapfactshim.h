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
// SS_SAPFACTSHIMProv.H
// 
//

/**
 @file 
 @internalComponent 
*/

#if !defined(SS_SAPFACTSHIM_H)
#define SS_SAPFACTSHIM_H

#include <e32def.h>
#include <comms-infras/metadata.h>
#include <es_ini.h>
#include <comms-infras/ss_subconnflow.h>
#include <ss_fact.h>

class CServProviderBase;
namespace ESock
{
    
class XProviderFactoryQueryProvider : public Factories::MFactoryQuery
	{
public:
	XProviderFactoryQueryProvider(CServProviderBase& aSSP ) :
		iSSP( aSSP )
		{
		}

protected:
	CServProviderBase& iSSP;

public:
	virtual TMatchResult Match(Factories::TFactoryObjectInfo& aProviderInfo);
	};

class CTransportFlowShimFactory : public CSubConnectionFlowFactoryBase
/**
Factory for creation CTransportFlowShim objects.

@internalComponent
*/
	{
public:
    enum { iUid = 0x101F7482 }; //Same as ESockSrv
	static CTransportFlowShimFactory* NewL(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer);
	~CTransportFlowShimFactory();
protected:
	CTransportFlowShimFactory(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer);
	// from CSubConnectionFlowFactoryBase
	CSubConnectionFlowBase* DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery);
	ACommsFactoryNodeId* DoFindOrCreateObjectL(TFactoryQueryBase& aQuery);
	};

// ==============================================================================================

} //namespace ESock
#endif	// SS_SAPFACTSHIM_H

