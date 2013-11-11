// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/

#ifndef CS_MOBILITY_APIEXT_H
#define CS_MOBILITY_APIEXT_H

#include <es_sock.h>

#include <comms-infras/es_api_ext.h>
#include <comms-infras/api_ext_msg.h>

class RCommsMobilityApiExt : public RCommsApiExtension<EMobilityApiExt>
/**
Comms Mobility API extension for client not running an Active Scheduler.

This class can actually be used by any client, one running an Active Scheduler 
and one that is not. However, if the Active Scheduler is present, 
it is recommended to use CActiveCommsMobilityApiExt which is much simpler.

@see CActiveCommsMobilityApiExt

@publishedAll
@released
*/
	{
public:
	IMPORT_C explicit RCommsMobilityApiExt();
	IMPORT_C void Close();

	IMPORT_C void RegisterForMobilityNotification(TDes8& aResponseEventBuf, TRequestStatus& aStatus);
	IMPORT_C void CancelMobilityNotification();

	IMPORT_C void MigrateToPreferredCarrier();
	IMPORT_C void IgnorePreferredCarrier();
	IMPORT_C void NewCarrierAccepted();
	IMPORT_C void NewCarrierRejected();
	};


class MMobilityProtocolResp
/**
Comms Mobility API extension protocol.
Used with CActiveCommsMobilityApiExt to reqister for mobility response messages.

@publishedAll
@released
*/
	{
public:
	virtual void PreferredCarrierAvailable(TAccessPointInfo aOldAP, TAccessPointInfo aNewAP, TBool aIsUpgrade, TBool aIsSeamless) = 0;
	virtual void NewCarrierActive(TAccessPointInfo aNewAP, TBool aIsSeamless) = 0;
	virtual void Error(TInt aError) = 0;
	};

class CActiveCommsMobilityApiExt : public CActive
/**
Comms Mobility API extension for client running an Active Scheduler.. Whenever an Active 
Scheduler is present, this API is recommended over RCommsMobilityApiExt because of its simplicity.

With few important exceptions, CActiveCommsMobilityApiExt exports very similar APIs to RCommsMobilityApiExt

@see RCommsMobilityApiExt

@publishedAll
@released
*/
	{
public:
	IMPORT_C static CActiveCommsMobilityApiExt* NewL(RCommsSubSession& aExtensionProvider,MMobilityProtocolResp& aProtocol);
	IMPORT_C ~CActiveCommsMobilityApiExt();

	IMPORT_C void MigrateToPreferredCarrier();
	IMPORT_C void IgnorePreferredCarrier();
	IMPORT_C void NewCarrierAccepted();
	IMPORT_C void NewCarrierRejected();

protected:
    void ConstructL(RCommsSubSession& aExtensionProvider);
    explicit CActiveCommsMobilityApiExt(MMobilityProtocolResp& aProtocol);

private:
  	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	MMobilityProtocolResp& iProtocol;
	RCommsMobilityApiExt iExtApi;
	RBuf8 iResponseBuf;
	};

#endif   // CS_MOBILITY_APIEXT_H

