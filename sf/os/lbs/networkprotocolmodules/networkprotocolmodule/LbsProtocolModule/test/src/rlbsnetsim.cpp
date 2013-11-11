// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Stubbed class to support testing of Protocol Module.
// This emulates network simulator interface.
// 
//

#include <e32base.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbspositioninfo.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "lbsnetsimgateway.h"
#include "lbsnetsimgatewayobserver.h"
#include "cnetworkobserver.h"

// Use global variables in namespace NetworkObserverGlobals
// This is not ideal but we have no other way of allowing
// the RLbsNetSim test stub to access a test observer.
using namespace NetworkObserverGlobals;

RLbsNetSim::RLbsNetSim()
	{
	iTestObserver = NULL;
	}
	
TInt RLbsNetSim::ConnectL(MLbsNetSimObserver* aObserver)
	{
	User::LeaveIfNull(aObserver);
	User::LeaveIfNull(G_TestObserver);

	iObserver = aObserver;
	iTestObserver = G_TestObserver;
	return iTestObserver->ConnectL(this);
	}
	
void RLbsNetSim::RegisterLcsMoLrL(const TDesC& aDestination)
	{
	User::LeaveIfNull(iTestObserver);
	if (iTestObserver)
		{
		iTestObserver->RegisterLcsMoLr(aDestination);
		}
	}
	
void RLbsNetSim::ReleaseLcsMoLrL(TInt aReason)
	{
	User::LeaveIfNull(iTestObserver);
	if (iTestObserver)
		{
		iTestObserver->ReleaseLcsMoLr(aReason);
		}
	}
	
void RLbsNetSim::ReleaseLcsLocationNotificationL(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	User::LeaveIfNull(iTestObserver);
	if (iTestObserver)
		{
		iTestObserver->ReleaseLcsLocationNotification(aResponse);
		}
	}
	
void RLbsNetSim::MeasurementReportLocationRequestMoreAssistanceDataL(const TLbsAsistanceDataGroup& aAssistData)
	{
	User::LeaveIfNull(iTestObserver);
	if (iTestObserver)
		{
		iTestObserver->MeasurementReportLocationRequestMoreAssistanceData(aAssistData);
		}
	}

void RLbsNetSim::MeasurementReportLocationL(const TPositionInfoBase& aPosition)
	{
	User::LeaveIfNull(iTestObserver);
	if (iTestObserver)
		{
		iTestObserver->MeasurementReportLocation(aPosition);
		}
	}
	
void RLbsNetSim::MeasurementControlFailureL(TInt aReason)
	{
	User::LeaveIfNull(iTestObserver);
	if (iTestObserver)
		{
		iTestObserver->MeasurementControlFailure(aReason);
		}
	}
	

void RLbsNetSim::Close()
	{
	if (iTestObserver)
		{
		iTestObserver->Close();
		iTestObserver = NULL;
		}
	iObserver = NULL;
	}

RLbsNetSim::~RLbsNetSim()
    {
    Close();
    }

MLbsNetSimObserver* RLbsNetSim::Observer() const
	{
	return iObserver;
	}
