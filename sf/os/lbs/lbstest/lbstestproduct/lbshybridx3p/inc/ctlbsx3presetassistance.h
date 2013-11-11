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
// This contains the header file for X3P GPS mode options Tests
// 
//

#ifndef __CT_LBS_X3P_RESET_ASSISTANCE_H__
#define __CT_LBS_X3P_RESET_ASSISTANCE_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsadmin.h>

// LBS test includes.
#include "ctlbshybridx3pstep.h"
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/test/transmitserverwatch.h>

// Literals used
_LIT(KLbsX3PResetAssistance, "LbsX3PResetAssistance");

class CT_LbsX3PResetAssistance :	public	CT_LbsHybridX3PStep, public MTransmitServerObserver
	{
public:
	~CT_LbsX3PResetAssistance();

	static CT_LbsX3PResetAssistance* New(CT_LbsHybridX3PServer& aParent);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
	// MTransmitServerObserver
	void OnTransmitRefPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	void OnTransmitPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo);
	
protected:
	CT_LbsX3PResetAssistance(CT_LbsHybridX3PServer& aParent);
	void ConstructL();

private:
	TLbsNetPosRequestMethod RequestNetworkMethod();
	void DecideWhatNetworkReceives();
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
	CNetProtocolProxy* iProxy;	
	RLbsTransmitPositionServer iServer;	
	RLbsTransmitPosition iTransmitter;	
	/** Used so the network requests the method specified in the ini file */
	TChosenNetworkMethod iNetworkMethod;
	/** Used so the network proxy knows if it should return the positions 
	 	or should let the module calculate it in hybrid case */
	TPlannedPositionOriginator iPlannedPositionOriginator;
	/** Used so the network proxy knows what to expect */
	TBool iNetworkExpectsMeasurements;
	TBool iNetworkExpectsPositions;
	/* If network requests a mode that the module does not support it will be rejected */
	TBool iNetRequestRejected;
	/** So we know what to expect when network doesn't specify mode */
	CLbsAdmin::TGpsMode iAdminGpsMode;	

	TNetworkStep iStepForSendingReset;
	TBool iWarmUpModule;
	TLbsAsistanceDataGroup iResetMask;
	};

#endif //__CT_LBS_X3P_RESET_ASSISTANCE_H__

