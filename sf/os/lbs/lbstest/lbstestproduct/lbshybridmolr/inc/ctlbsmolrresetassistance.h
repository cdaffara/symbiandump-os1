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
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_MOLR_RESET_ASSISTANCE_H__
#define __CT_LBS_MOLR_RESET_ASSISTANCE_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsadmin.h>

// LBS test includes.
#include "ctlbshybridmolrstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/test/posserverwatch.h>

// Literals used
_LIT(KLbsMolrResetAssistance, "LbsMolrResetAssistance");

class CT_LbsMolrResetAssistance :	public	CT_LbsHybridMOLRStep, public MPosServerObserver
	{
public:
	~CT_LbsMolrResetAssistance();

	static CT_LbsMolrResetAssistance* New(CT_LbsHybridMOLRServer& aParent);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
protected:
	CT_LbsMolrResetAssistance(CT_LbsHybridMOLRServer& aParent);
	void ConstructL();

private:
	TLbsNetPosRequestMethod RequestNetworkMethod();
	void DoNetworkProtocolProxyStepL();
	void DecideWhatNetworkDoesntReceive();
	TBool SendResetAssistanceDataL();

private:
	enum TState
		{
		EInitializing,
		ERefLocReceived,
		EGpsLocReceived
		};
	enum TChosenNetworkMethod
		{
		ENetworkMethodInvalid,
		ETerminalBasedNetworkMethod,
		ETerminalBasedTerminalAssistedNetworkMethod,
		ETerminalAssistedNetworkMethod,
		ETerminalAssistedTerminalBasedNetworkMethod,
		EAutonomousNetworkMethod,
		ENetworkMethodNotSet
		};
	enum TNetworkProtocolProxyStep
		{
		ENetworkProtocolProxyStepInitial,
		ENetworkProtocolProxyStepAfterFirstNPUD,
		ENetworkProtocolProxyStepAfterSecondNPUD,
		ENetworkProtocolProxyStepFinal
		};
	enum TPlannedPositionOriginator
		{
		EPositionOriginatorUnkown,
		EPositionOriginatorModule,
		EPositionOriginatorNetwork
		};
	enum TNetworkStep
		{
		EInitial,
		EBeforeSession,
		EBeginningOfSession,
		EAfterAssistanceData,
		EAfterMeasurements,
		EAfterPosition,
		EAfterSessionClose
		};
	
	TState iState;
	TLbsNetSessionId iSessionId;
	/** Used so the network requests the method specified in the ini file */
	TChosenNetworkMethod iNetworkMethod;
	/** Remembers what is the next thing the netwrok should expect/send */
	TNetworkProtocolProxyStep iNetworkProtocolProxyStep;
	CNetProtocolProxy* iProxy;
	/** Used so the test checks the right request is sent to the network */
	CLbsAdmin::TGpsMode iAdminGpsMode;

	/** Used so the network proxy knows if it should return the positions 
	 	or should let the module calculate it in hybrid case */
	TPlannedPositionOriginator iPlannedPositionOriginator;
	/** Used so the network proxy knows what to expect*/
	TBool iNetworkExpectsMeasurments;
	TBool iNetworkExpectsPositions;
	TBool iGPSModeNotSupported;

	TNetworkStep iStepForSendingReset;
	TBool iWarmUpModule;
	TLbsAsistanceDataGroup iResetMask;

	TBool iModuleWarmedUp;
	TBool iExpectExtraRequestForAssistanceData;
	};

#endif //__CT_LBS_MOLR_RESET_ASSISTANCE_H__

