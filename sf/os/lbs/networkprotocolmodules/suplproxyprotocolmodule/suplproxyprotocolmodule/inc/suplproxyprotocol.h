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

#ifndef __SUPLPROXYPROTOCOL_H__
#define __SUPLPROXYPROTOCOL_H__

#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>

class CCancelSuplProxyPrivacyRequestInfo;
class TLbsNetSessionId;
class CSuplProxyPrivacyRequestInfo;


class MPosProtocolResponseObserver
	{
public:
	virtual void HandleRrlpLocationResponse(TInt aReason, const TPositionInfoBase& aPosInfo) = 0;
	virtual void HandleSetAssistanceDataMask(TInt aReason, TLbsAsistanceDataGroup aDataRequestMask) = 0;
	};

/** 
Network privacy protocol mixin definition.
*/
class MSuplProxyPrivacyProtocol
	{
public:

	virtual void CancelVerifyLocationRequest(CCancelSuplProxyPrivacyRequestInfo* aCancelInfo) =0;
	virtual void GetNextSessionId(TLbsNetSessionId& aSessionId) =0;
	virtual void NotifyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo) =0;
	virtual void NotifyVerificationTimeout(CSuplProxyPrivacyRequestInfo* aRequestInfo) =0;
	virtual void VerifyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo) =0;
	virtual void PrivacyLocationRequest(CSuplProxyPrivacyRequestInfo* aRequestInfo) =0;
	virtual void CompletePrivacyRequest(TUint32 aRequestId, TInt aReason) =0;
	virtual void PrivacySessionDisconnected(const CSession2* aSession,TInt aReason) =0;
	virtual void EndPrivacySession(TInt aRequestId, TInt aReason) =0;
	};

/** 
Pos protocol mixin definition.
*/
class MSuplProxyPosProtocol
    {
public:
	// a location request has been received from the network (via GetPositionL() interface)
	virtual TInt NetworkGetPosition(const RMessage2& aMessage, TInt aRequestId) = 0;
	// a location request has been received from the network (via GetSuplInfoL() interface)
	virtual TInt RrlplLocationRequest(const TLbsNetPosRequestQuality& aQuality, const TLbsNetPosRequestMethod& aPosMethod, TInt aRequestId) = 0;
	// location request from network has been cancelled (via CancelGetPosition() interface)
	virtual TInt CancelNetworkGetPosition(const RMessage2& aMessage, TInt aRequestId) = 0;
	// Network has created a new SUPL session (MOLR / MTLR)
	virtual void NotifySubSessionOpen(MPosProtocolResponseObserver* aObserver, TInt aRequestId) = 0;
    // SUPL FW is reusing an inactive session[SessionEnd()] for a new session by reinitializing it.
	virtual void  NotifySubSessionReInit(MPosProtocolResponseObserver* aObserver, TInt aRequestId) = 0;
	// Notification that the POS message plugin is closing the subsession handle with the SPPM.  
	// Happens when the SUPL FW deletes the POS session object
	virtual TInt NotifySubSessionClose(TInt aRequestId) = 0;
	// Notification that the SUPL Session (MOLR / MTLR) is ending through SUPL_END from the network
	virtual TInt NotifySessionEnd(TInt aRequestId) = 0;
	// Gets the positioning capabilities from LBS subsystem
	virtual void GetCurrentPosCapabilities(TLbsNetPosCapabilities& aPosCaps) = 0;
	// Subsession asking for the assistance data mask to be used to complete GetSuplInfoL 
	virtual TInt GetAssistanceDataMask(TInt aRequestId) = 0;
	// Pass assistance data received from network to LBS subsystem 
	virtual void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, const RLbsAssistanceDataBuilderSet& aBuilderSet, TInt aReason, TInt aRequestId) = 0;
	// Pass positioning method requested by the network
	virtual void SetPosMethod(const TLbsNetPosRequestMethod& aMethod, TInt aRequestId) = 0;
	
    };

#endif // __SUPLPROXYPROTOCOL_H__
