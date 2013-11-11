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

#include "mobilitymcpr.h"
#include <comms-infras/mobilitymcpractivities.h>
#include <comms-infras/ss_log.h>

#include <comms-infras/ss_nodemessages_availability.h>

#ifdef SYMBIAN_TRACE_ENABLE
	#define KCoreMobileMCprTag KESockMetaConnectionTag
	_LIT8(KCoreMobileMCprSubTag, "coremobilemcpr");
#endif

using namespace MeshMachine;
using namespace ESock;
using namespace MobilityMCprActivities;

//
//CCoreMetaConnectionProvider
EXPORT_C CMobilityMetaConnectionProvider::CMobilityMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory,
                                                                          const TProviderInfo& aProviderInfo,
                                                                          const MeshMachine::TNodeActivityMap& aActivityMap)
:	CCoreMetaConnectionProvider(aFactory,aProviderInfo,aActivityMap),
	iIsHandshakingNow(EFalse)
	{
	LOG_NODE_CREATE(KCoreMobileMCprSubTag, CMobilityMetaConnectionProvider);
	}

EXPORT_C CMobilityMetaConnectionProvider::~CMobilityMetaConnectionProvider()
	{
	LOG_NODE_DESTROY(KCoreMobileMCprSubTag, CMobilityMetaConnectionProvider);
	}

EXPORT_C void CMobilityMetaConnectionProvider::Received(TNodeContextBase& aContext)
    {
    CCoreMetaConnectionProvider::Received(aContext);
	}

TBool CMobilityMetaConnectionProvider::IsHandshakingNow() const
	{
	return iIsHandshakingNow;
	}


