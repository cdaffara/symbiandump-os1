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
 @internalComponent
*/

#if !defined(__SS_MSGS_H__)
#define __SS_MSGS_H__

#include <elements/sd_msgs.h>
//#include <elements/msgintercept_macro.h>

namespace ESock
{
	class RTierThreadMap;
}

/**
@class TWorkerCookiesMsg
*/
class TWorkerCookiesMsg : public Den::TWorkerMsg
	{
public:
	enum {KCode = Den::TWorkerMsg::EWorkerCookies};//< Message code
	inline TWorkerCookiesMsg (
		const Messages::TNodeId& aDataPlaneFC,
		const Messages::TNodeId& aSubConnPlaneFC,
		const Messages::TNodeId& aConnPlaneFC,
		const Messages::TNodeId& aMetaConnPlaneFC,
		const Messages::TNodeId& aTierManagerFC
	)
	:	Den::TWorkerMsg(KCode)
		{
		TParams* params=reinterpret_cast<TParams*>(ParamBlock());
		Mem::FillZ(params, sizeof(TParams));
		params->iDataPlaneFC = aDataPlaneFC;
		params->iSubConnPlaneFC = aSubConnPlaneFC;
		params->iConnPlaneFC = aConnPlaneFC;
		params->iMetaConnPlaneFC = aMetaConnPlaneFC;
		params->iTierManagerFC = aTierManagerFC;
		}

	inline const Messages::TNodeId& DataPlaneFC() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iDataPlaneFC;
		}
	inline const Messages::TNodeId& SubConnPlaneFC() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iSubConnPlaneFC;
		}
	inline const Messages::TNodeId& ConnPlaneFC() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iConnPlaneFC;
		}
	inline const Messages::TNodeId& MetaConnPlaneFC() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iMetaConnPlaneFC;
		}
	inline const Messages::TNodeId& TierManagerFC() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iTierManagerFC;
		}
private:
	struct TParams /** Message parameters */
		{
		// Cookies for plane factory containers (only filled by threads containing a Player)
		Messages::TNodeId iTierManagerFC;
		Messages::TNodeId iMetaConnPlaneFC;
		Messages::TNodeId iConnPlaneFC;
		Messages::TNodeId iSubConnPlaneFC;
		Messages::TNodeId iDataPlaneFC;
		};

	__ASSERT_COMPILE(sizeof(TParams) <= CommsFW::KCFMessageParamBlockSize);
	};

/**
@class TWorkerLoadTierMappings
@see TWorkerMsg::ELoadTierMappings
*/
class TWorkerLoadTierMappings : public Den::TWorkerMsg
	{
public:
	enum {KCode = TWorkerMsg::ELoadTierMappings};/**Message code*/
	inline TWorkerLoadTierMappings()
	:	Den::TWorkerMsg(KCode)
		{
		}
	};

/**
@class TWorkerTierMappingsLoaded
@see TWorkerMsg::ETierMappingsLoaded
*/
class TWorkerTierMappingsLoaded : public Den::TWorkerMsg
	{
public:
	enum {KCode = TWorkerMsg::ETierMappingsLoaded};/**Message code*/

	inline TWorkerTierMappingsLoaded(ESock::RTierThreadMap* aTierMap)
	:	Den::TWorkerMsg(KCode)
		{
		TParams* params=reinterpret_cast<TParams*>(ParamBlock());
		params->iTierMap = aTierMap;
		}

	inline const ESock::RTierThreadMap* TierMap() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iTierMap;
		}

private:
	struct TParams /** Message parameters */
		{
		ESock::RTierThreadMap* iTierMap;
		};
	};


class TBrokerIntroductionMsg : public Den::TWorkerMsg
	{
public:
	enum {KCode = Den::TWorkerMsg::EBrokerIntroduction};

	inline TBrokerIntroductionMsg (const Messages::TNodeId& aBrokerId)
		: Den::TWorkerMsg(KCode)
		{
		TParams* params=reinterpret_cast<TParams*>(ParamBlock());
		Mem::FillZ(params, sizeof(TParams));
		params->iBrokerId = aBrokerId;
		}

	inline const Messages::TNodeId& BrokerId() const
		{
		const TParams* params = reinterpret_cast<const TParams*>(ParamBlock());
		return params->iBrokerId;
		}

private:
	// Message parameters
	struct TParams
		{
		Messages::TNodeId iBrokerId;
		};

	__ASSERT_COMPILE(sizeof(TParams) <= CommsFW::KCFMessageParamBlockSize);
	};

#endif

