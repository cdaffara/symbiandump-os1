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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_INTERFACETABLE_H
#define SYMBIAN_INTERFACETABLE_H

#include <elements/interfacetable_internal.h>

/**
Helper macros.
Not intended to be used directly but only via helper macros (like IMPLEMENTERBASE_x) defined below.

@publishedPartner
@released
*/

#define ITFHIERARCHY_1(TCLIENT,TIF1) NetInterfaces::TIfStaticFetcher<NetInterfaces::TIfStaticFetcherFirstInHierarchy<NetInterfaces::TInterfaceControl>,TCLIENT,TIF1>
#define ITFHIERARCHY_2(TCLIENT,TIF1,TIF2) NetInterfaces::TIfStaticFetcher<ITFHIERARCHY_1(TCLIENT,TIF1),TCLIENT,TIF2>
#define ITFHIERARCHY_3(TCLIENT,TIF1,TIF2,TIF3) NetInterfaces::TIfStaticFetcher<ITFHIERARCHY_2(TCLIENT,TIF1,TIF2),TCLIENT,TIF3>
#define ITFHIERARCHY_4(TCLIENT,TIF1,TIF2,TIF3,TIF4) NetInterfaces::TIfStaticFetcher<ITFHIERARCHY_3(TCLIENT,TIF1,TIF2,TIF3),TCLIENT,TIF4>
//and so on if needed at all

#define ITFHIERARCHY_LINK_1(TCLIENT,TLINK,TIF1) NetInterfaces::TIfStaticFetcherLinkBase<TLINK::TIfStaticFetcherNearestInHierarchy,TCLIENT,TIF1>
#define ITFHIERARCHY_LINK_2(TCLIENT,TLINK,TIF1,TIF2) NetInterfaces::TIfStaticFetcher<ITFHIERARCHY_LINK_1(TCLIENT,TLINK,TIF1),TCLIENT,TIF2>
#define ITFHIERARCHY_LINK_3(TCLIENT,TLINK,TIF1,TIF2,TIF3) NetInterfaces::TIfStaticFetcher<ITFHIERARCHY_LINK_2(TCLIENT,TLINK,TIF1,TIF2),TCLIENT,TIF3>
#define ITFHIERARCHY_LINK_4(TCLIENT,TLINK,TIF1,TIF2,TIF3,TIF4) NetInterfaces::TIfStaticFetcher<ITFHIERARCHY_LINK_3(TCLIENT,TLINK,TIF1,TIF2,TIF3),TCLIENT,TIF4>
#define ITFHIERARCHY_LINK_5(TCLIENT,TLINK,TIF1,TIF2,TIF3,TIF4,TIF5) NetInterfaces::TIfStaticFetcher<ITFHIERARCHY_LINK_4(TCLIENT,TLINK,TIF1,TIF2,TIF3,TIF4),TCLIENT,TIF5>

//and so on if needed at all


/**
Helper macros usage example.

Used by the implementer. An example usage has been ilustrated below:

NONSHARABLE_CLASS(CIPNetworkConnectionProvider) : public CConnectionProviderBase, public MConnectionControlClient, public AMobilityProtocolReq, public AAvailabilityProtocolReq,
                                                  ITFHIERARCHY_2(CIPNetworkConnectionProvider,MMobilityProtocolReq,MAvailabilityProtocolReq)
	{
public:
	typedef ITFHIERARCHY_2(CIPNetworkConnectionProvider,MMobilityProtocolReq,MAvailabilityProtocolReq) TIfStaticFetcherNearestInHierarchy;

protected:
	CIPNetworkConnectionProvider(CConnectionProviderFactoryBase& aFactory) :
		CConnectionProviderBase(aFactory),
		TIfStaticFetcherNearestInHierarchy(this)
			{
			}
	...
public:
	void ReturnInterfacePtrL(MMobilityProtocolReq*& aInterface);
	void ReturnInterfacePtrL(MAvailabilityProtocolReq*& aInterface);
	...
	};
*/

#endif
	//SYMBIAN_INTERFACETABLE_H



