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

#ifndef PANHELPERREMOTESDP_H
#define PANHELPERREMOTESDP_H

#include <e32base.h>
#include <e32std.h>
#include <btsdp.h>
#include <es_sock.h>
#include <bt_sock.h>
#include "panroles.h"
#include "panhelper.h"

namespace PanHelper
{
	
NONSHARABLE_CLASS(CPanHelperRemoteSdpQuerier) : public CBase, private MSdpAgentNotifier, private MSdpAttributeValueVisitor
/**
Perform SDP queries on remote devices
@internalTechnology
*/
	{

public:
	static CPanHelperRemoteSdpQuerier* NewL();
	virtual ~CPanHelperRemoteSdpQuerier();
	
	void QueryL(const TBTDevAddr& aAddr, TDes8& aSupportedRemoteRoles, TRequestStatus& aStatus);
	void CancelQuery();
private:
	CPanHelperRemoteSdpQuerier();
	TInt ServiceName(TBluetoothPanRole aRole, TDes8& aServiceName);
	TInt ServiceDescription(TBluetoothPanRole aRole, TDes8& aServiceDescription);
	TInt NetworkType(TBluetoothPanRole aRole, TPanNapNetworkType& aNetworkType);
//	TInt PanSecurityMode(TBluetoothPanRole aRole, TPanSecurityMode& aSecurityMode);
	TUint8 LoadFactor(TBluetoothPanRole aRole);

	// MSdpAgentNotifier methods
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError);

	// MSdpAttributeValueVisitor methods
	virtual void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType);
    virtual void StartListL(CSdpAttrValueList &aList);
    virtual void EndListL();

public:

private:		
	CSdpAgent* 						iSdpAgent;			//< For doing the SDP queries on the remote device
	PanHelper::TPanDeviceRolesList* iSupportedRemoteRoles;
	TRequestStatus* 				iClientStatus;
	TBool							iMatchesFound;
	};
	
}
#endif //PANHELPERREMOTESDP_H
