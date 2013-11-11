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

#ifndef CAGPSPOSITIONER_H
#define CAGPSPOSITIONER_H

#include <e32base.h>
#include "cresponsehandler.h"
#include <lbssatellite.h>
#include "cagpsdatabus.h"
#include "cfinalnetdatabus.h"
#include "crefdatabus.h"

class CAGPSDataBus;
class CFinalNetDataBus;
class CPositionerQ;

class CAgpsResponseHandler : public CResponseHandler 
	{
    public:
        static CAgpsResponseHandler* NewL(CPositionerQ* aPositionerQ, CRequestHandler* aRequestHandler);
        ~CAgpsResponseHandler();

	public:
		TBool GetLastPosition(TPositionInfoBase& aPos, TTime aMaxAge, TBool aAllowPartial);
		
		// from MDataBusObserver
		void DataUpdate(TPositionInfo& aPosition, TInt aStatus, TInt aError, TTime aActualTime, TUint aAttributes, TUint aMode = 0);

	private:
        CAgpsResponseHandler(CPositionerQ* aPositionerQ, CRequestHandler* aRequestHandler);
        void ConstructL();
        
		TInt ReadPositionFromLocDataBus(TPositionInfo& aPosInfo, TTime& aActualTime);
		TInt ReadFinal(TPositionInfo& aPosInfo, TTime& aActualTime);
		
		void Subscribe();
		
		enum TPositionChoice
			{
			EAGPSPos = 0,
			ERefPos	 = 1,
			EFNPPos	 = 2,
			ENone = 3,
			};	
	private:
		CAgpsDataBus* iAGPSDataBus;
		CFinalNetDataBus* iFinalNetDataBus;
		CRefDataBus* iRefDataBus;
		CPositionerQ* iPositionerQ;
		CRequestHandler* iRequestHandler;
		RLbsPositionUpdateRequests::TChannelIdentifer iChannel;
	};

#endif
