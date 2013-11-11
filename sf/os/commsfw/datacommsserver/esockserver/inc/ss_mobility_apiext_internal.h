// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SS_MOBILITY_APIEXT_INTERNAL_H__
#define __SS_MOBILITY_APIEXT_INTERNAL_H__

#include <comms-infras/ss_mobility_apiext.h>

namespace ESock
{

class TMobilitySubscriptionMsg;
class CCommsMobilityApiExtRespMsg;

class CCommsMobilitySrvIpcResponder : public CCommsMobilitySrvResp
/**
@internalTechnology
*/
	{
public:
	friend class ESock::TMobilitySubscriptionMsg; //the subscription message constructs CCommsMobilitySrvResp

public:
	IMPORT_C virtual void DoPreferredCarrierAvailable(TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless);
	IMPORT_C virtual void DoNewCarrierActive(TAccessPointInfo aNewAP, TBool aIsSeamless);
	virtual void DoComplete(TInt aError);

private:
	IMPORT_C static CCommsMobilitySrvIpcResponder* NewL(Elements::RResponseMsg& aResponseMsg);
	explicit CCommsMobilitySrvIpcResponder(Elements::RResponseMsg& aResponseMsg);
	virtual ~CCommsMobilitySrvIpcResponder();

private:
	ESock::CCommsMobilityApiExtRespMsg* iMsg;
	Elements::RResponseMsg iResponseMsg;
	};

} // namespace ESock

#endif // __SS_MOBILITY_APIEXT_INTERNAL_H__

