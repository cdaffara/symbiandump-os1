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

/**
 @file
 @InternalComponent
*/

#ifndef AGPSPOSITIONER_H
#define AGPSPOSITIONER_H

#include <lbs/epos_cpositioner.h>
#include "mpositioner.h"
#include "cadaptationpositioner.h"
#include "lbsnetinternalapi.h"
#include "tagpsrequestparameters.h"
#include "lbsnetregstatusint.h"

class CPositionerQ;

class CAgpsPositioner :public CAdaptationPositioner
	{
	public:
		static CAgpsPositioner* NewL(TAny* aConstructionParameters);
		virtual ~CAgpsPositioner();
	
	private:
		virtual void ConstructL(TAny* aConstructionParameters);
	
	protected:
		CAgpsPositioner();
		void StartTrackingL(const TTimeIntervalMicroSeconds& aInterval);
		void NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus);
		void SetModuleId(TPositionInfoBase* aPosInfo);
		const TRequestParameters* PositionRequestParams();
		void ModeUpdate(TUint aMode);
		virtual void SetAGPSMode();
		
	protected:
		TBool iTerminalAssisted;
		TUint iAGPSMode;
		RLbsNetworkRegistrationStatus iNetRegStatus;
		TAgpsRequestParameters iRequestParameters;
	};

#endif

// End of File
