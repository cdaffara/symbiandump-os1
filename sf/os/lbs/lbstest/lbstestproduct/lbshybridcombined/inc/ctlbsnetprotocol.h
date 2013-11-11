// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CT_LBS_NET_PROTOCOL_H__
#define __CT_LBS_NET_PROTOCOL_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsadmin.h>

// LBS test includes.
#include <lbs/test/lbsnetprotocolproxy.h>
#include "ctlbstestactivemanager.h"

class CT_LbsNetProtocol : public CActive
	{
public:
	enum TPlannedPositionOriginator
		{
		EPositionOriginatorUnkown,
		EPositionOriginatorModule,
		EPositionOriginatorNetwork
		};
public:
	~CT_LbsNetProtocol();

	static CT_LbsNetProtocol* NewL(MTestFlowObserver& aObserver, CTestStep& aStep);

	void StartNetworkL();
	void SignalClientsFinished();
	
	TPlannedPositionOriginator PlannedPositionOriginator();
	void SetPlannedPositionOriginator(TInt aOriginator);
	void SetNetworkMethod(TInt aMethod);
	
private:
	CT_LbsNetProtocol(MTestFlowObserver& aObserver, CTestStep& aStep);
	void ConstructL();

private:
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void Process_RequestSelfLocationL();
	TLbsNetPosRequestMethod RequestNetworkMethod();
	void CheckQualityL(TLbsNetPosRequestOptionsAssistance* opts);
	void DoClientStepL(TInt aIndex);
	void DecideWhatNetworkDoesntReceive();

private:
	enum TChosenNetworkMethod
		{
		ENetworkMethodNotSet,
		ETerminalBasedNetworkMethod,
		ETerminalBasedTerminalAssistedNetworkMethod,
		ETerminalAssistedNetworkMethod,
		ETerminalAssistedTerminalBasedNetworkMethod
		};
	enum TNetworkProtocolProxyStep
		{
		ENetworkProtocolProxyStepInitial, // = 0
		ENetworkProtocolProxyStepSessionNrhRequestSent, // = 1
		ENetworkProtocolProxyStepSessionWaitingForFirstMeasurmentsTimeOut, // = 2
		ENetworkProtocolProxyStepSessionWaitingForFirstMeasurments, // = 3
		ENetworkProtocolProxyStepSessionMeasurmentsReceived, // = 4
		ENetworkProtocolProxyStepSessionPositionSent, // = 5
		ENetworkProtocolProxyStepNoSessionRunning // = 6
		};
	
	MTestFlowObserver& iObserver;
	CTestStep& iStep;
	
	TLbsNetSessionId iSessionId;
	/** Used so the network requests the method specified in the ini file */
	TChosenNetworkMethod iNetworkMethod;
	/** Remembers what is the next thing the network should expect/send */
	TNetworkProtocolProxyStep iNetworkProtocolProxyStep;
	/** The session ID where the count starts from the beggining of the test*/
	TInt iRelativeSessionId;
	/** The Network Protocol Proxy */
	CNetProtocolProxy* iProxy;
	TBool iClientsFinished;

	/** Used so the network proxy knows if it should return the positions 
	 	or should let the module calculate it in hybrid case */
	TPlannedPositionOriginator iPlannedPositionOriginator;
	/** Used so the network proxy knows what to expect*/
	TBool iNetworkExpectsMeasurments;
	TBool iNetworkExpectsPositions;

	TTime iAlpha2StartTime;
	};

#endif //__CT_LBS_NET_PROTOCOL_H__
