// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__SS_COMMON_H__)
#define __SS_COMMON_H__

#include <elements/mm_node.h>
#include <elements/mm_activities.h>
#include <elements/sd_std.h>
#include <elements/mm_context_internal.h>
#include <cfshared.h>
#include <comms-infras/ss_activities.h>

/** Support for checking that the current thread is the "home thread" */
#if defined(_DEBUG)
#define ESOCK_HOME_THREAD_CHECK_ENABLED
#endif

// Some legacy oddities supported for a transition period have code guarded by this macro. Some day we'll be
// breaking all of this behaviour and so able to drop this
#define SYMBIAN_NETWORKING_LEGACY_COMPATIBILITY_SUPPORT

/**
	Type identifier for the derived sub-session objects. Used e.g. in containers to identify
	what is stored.

	@see CSockSubSession::Type()
*/
class TCFSubSessInfo : public Den::TSubSessInfo
/**
@internalTechnology
*/
	{
public:
	enum TSubSessionType
		{
		EHostResolver = 2, //starting from TSubSessInfo::EAny
		EServiceResolver = 3,
		ENetDatabase = 4,
		EConnection = 5,
		ESubConnection = 6,
		ESocket = 7
		};

	TCFSubSessInfo(TUint8 aType)
	:	TSubSessInfo(aType)
		{
		}
	};

/**
@class TCFPlayerRole

TCFPlayerRole and TPlayerRole can be removed after Players are relieved from knowing their roles
which should hopefully come as part of the "Global Object Broker" work.
*/
class TCFPlayerRole : public Den::TPlayerRole
/**
@internalTechnology
*/
	{
public:
	/** Parallel threads operate the same technologies in the same plane but with some interesting (probably private difference), eg two data threads both
	bound to a control thread, one default and one devoted to realtime. We describe them as "kin" of one another, and they are distinguished by their
	"kindex" ie kin index, which is simply a whole number zero upwards, zero being the default and generally only value. It's up to a control provider to
	know about the parallel threads and specify the kindex when requesting a FC to talk to (eg a SCPR when creating a flow)

	At present there are no parallel threads for CPRs as they have 1:1 with MCPR. It may be that in the future we have to worry about the same TierMgr in two
	threads, eg an authentication protocol being used by both BT & IP, and they're running separate control thread. This will require further thought once
	the use is clearer.
	*/
	static const TInt KDefaultKindex = 0;
    enum TRole
        {
        EDataPlane     = 0x000001,
        ESubConnPlane  = 0x000010,
        EConnPlane     = 0x000100,
        EMetaConnPlane = 0x001000,
        ETierMgrPlane  = 0x010000,
        ETierResolver  = 0x100000,
        EAllPlanes     = EDataPlane | ESubConnPlane | EConnPlane | EMetaConnPlane | ETierMgrPlane,
        EPrePlanarArchitecture = 0x40000000,	// never explicitly set; denotes a legacy CMI and so a worker whose plane FCs can be ignored
		};

public:
	TCFPlayerRole()
	:	Den::TPlayerRole(EUnknown, KDefaultKindex)
		{
		}

	TCFPlayerRole(TUint aRole, TUint16 aKindex = KDefaultKindex)
	:	Den::TPlayerRole(aRole, aKindex)
		{
		}
	};

#define mcfnode_cast Messages::mnode_cast


#endif


