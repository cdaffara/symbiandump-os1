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

#ifndef __CT_LBS_MOLR_GPS_OPTIONS_H__
#define __CT_LBS_MOLR_GPS_OPTIONS_H__

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
_LIT(KLbsMolrGpsOptions, "LbsMolrGpsOptions");

class CT_LbsMolrGpsOptions :	public	CT_LbsHybridMOLRStep, public MPosServerObserver
	{
public:
	~CT_LbsMolrGpsOptions();

	static CT_LbsMolrGpsOptions* New(CT_LbsHybridMOLRServer& aParent);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

	// MPosServerObserver
	void OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
protected:
	CT_LbsMolrGpsOptions(CT_LbsHybridMOLRServer& aParent);
	void ConstructL();

private:
	TLbsNetPosRequestMethod RequestNetworkMethod();
	void CheckQualityL(TLbsNetPosRequestOptionsAssistance* opts);
	void DoNetworkProtocolProxyStepL();
	void DecideWhatNetworkDoesntReceive();

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
		ENetworkMethodNotSet,
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
	
	TState iState;
	TLbsNetSessionId iSessionId;
	/** Used so the network requests the method specified in the ini file */
	TChosenNetworkMethod iNetworkMethod;
	/** Specified in .ini file. When true the test will expect
        that that the measuremnts should be delivered immediately and not
        when the alpha2 (maxfix) time expires */ 
	TBool iImmediateMeasurements;
	
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
	};

#endif //__CT_LBS_MOLR_GPS_OPTIONS_H__

