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
// *** STUB CLASS HEADER FOR UNIT TESTS ***
// 
//

#ifndef LBSNETSIMTESTGATEWAY_H
#define LBSNETSIMTESTGATEWAY_H
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>

class CNetworkObserver;

// forwards
class MLbsNetSimObserver;

class RLbsNetSim
	{
	public:
		// Constructor
		RLbsNetSim();
		~RLbsNetSim();
		
		// Server methods
		TInt ConnectL(MLbsNetSimObserver* aObserver);
		void Close();
		
		// Methods
		void RegisterLcsMoLrL(const TDesC& aData);
		void ReleaseLcsMoLrL(TInt aReason);
		void ReleaseLcsLocationNotificationL(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
		void MeasurementReportLocationL(const TPositionInfoBase& aPosition);
		void MeasurementReportLocationRequestMoreAssistanceDataL(const TLbsAsistanceDataGroup& aFilter);
		void MeasurementControlFailureL(TInt aReason);

		// stub method
		MLbsNetSimObserver* Observer() const;

	private:
		MLbsNetSimObserver*		iObserver;
		CNetworkObserver* iTestObserver;
	};

#endif // LBSNETSIMTESTGATEWAY_H

