// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SUSADAPTIONSERVER_H__
#define __SUSADAPTIONSERVER_H__

#include <e32base.h>
#include <e32std.h> 
#include "ssmadaptation.h"
#include "susstateadaptation.h"
#include "sussimadaptation.h"
#include "susmiscadaptation.h"
#include "susemergencycallrfadaptation.h"
#include "susrtcadaptation.h"
#include "susadaptationpluginloader.h"
#include "susadaptionsession.h"

class CStateAdaptationRef;

/**
 @internalComponent
 @released
 */
class CSsmAdaptationServer : public CPolicyServer
	{	
public:
	static CSsmAdaptationServer* NewLC();
	static CSsmAdaptationServer* NewLC(const TDesC& aServerName);

	~CSsmAdaptationServer();

	//State Adaptation services 
	void DoRequestCoopSysStateChangeL(const RMessage2& aMessage);
	void DoRequestCoopSysSelfTestL(const RMessage2& aMessage);
	void DoRequestCoopSysPerformRestartActionsL(const RMessage2& aMessage);
	void DoRequestCoopSysPerformShutdownActionsL(const RMessage2& aMessage);
	void DoRequestCoopSysPerformRfsActionsL(const RMessage2& aMessage);
	void DoRequestCoopSysCancelL(const RMessage2& aMessage);
	void DoNotifyCoopSysEventL(const RMessage2& aMessage);
	void DoNotifyCoopSysEventCancelL(const RMessage2& aMessage);
	void DoGetLastCoopSysEventL(const RMessage2& aMessage);
	//SIM Adaptation services
	void DoGetSimOwnedL(const RMessage2& aMessage);
	void DoGetSimCancelL(const RMessage2& aMessage);
	void DoNotifySimEventL(const RMessage2& aMessage);
	void DoNotifySimCancelL(const RMessage2& aMessage);
	void DoGetLastSimEventL(const RMessage2& aMessage);
	// RTC Adaptation services
	void DoValidateRtcL(const RMessage2& aMessage);
	void DoSetWakeupAlarmL(const RMessage2& aMessage);
	void DoUnsetWakeupAlarmL(const RMessage2& aMessage);
	void DoRtcCancelL(const RMessage2& aMessage);
	//Misc Adaptation services
	void DoSecurityStateChangeL(const RMessage2& aMessage);
	void DoGetGlobalStartupModeL(const RMessage2& aMessage);
	void DoPrepareSimLanguagesL(const RMessage2& aMessage);
	void DoMiscAdaptationCancelL(const RMessage2& aMessage);
	void DoMiscAdaptationHiddenResetL(const RMessage2& aMessage);
	void DoGetSimLanguagesArrayL(const RMessage2& aMessage);

	//Emergency call Rf services
	void DoActivateRfForEmergencyCallL(const RMessage2& aMessage);
	void DoDeactivateRfForEmergencyCallL(const RMessage2& aMessage);
	void DoEmergencyCallRfCancelL(const RMessage2& aMessage);
	TInt SetAsPriorityClientL(CSsmAdaptationSession* aSession);
	void RemovePriorityClient(CSsmAdaptationSession* aSession);	
#ifdef _DEBUG
	void UnloadAdaptationsAndObservers();
#endif
	
#ifdef  TESTSUSADAPTATIONPLUGINLOAD_OOM
	friend class CTestSusLoadAdaptationServer;
#endif

private:
//From Policy Server
	CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);
	CSsmAdaptationServer();
	void ConstructL(const TDesC& aServerName);
	//from CServer2
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	void LoadStateAdaptationL();
	void LoadStateAdaptationObserversL();
	void LoadSimAdaptationL();
	void LoadSimAdaptationObserversL();
	void LoadRtcAdaptationL();
	void LoadMiscAdaptationL();
	void LoadEmergencyCallRfAdaptationL();
private:
	CStateAdaptationRequests* iStateAdaptation;
	CStateAdaptationObservers* iStateAdaptationObservers;
	CSimAdaptationRequests* iSimAdaptation;
	CSimAdaptationObservers* iSimAdaptationObservers;
	CEmergencyCallRfAdaptation* iEmergencyCallRfAdaptation;
	CMiscAdaptation* iMiscAdaptation;
	CRtcAdaptation* iRtcAdaptation;
	CSusAdaptationPluginLoader* iSusAdaptationPluginLoader;
	CSsmAdaptationSession* iPriorityClientSession;
	};

#endif //__SUSADAPTIONSERVER_H__
