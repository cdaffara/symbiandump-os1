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

#include <e32base.h>
#include "cresponsehandler.h"
#include "cnetdatabus.h"

class CNETDataBus;
class CPositionerQ;

class CNETResponseHandler : public CResponseHandler 
	{
    public:
        static CNETResponseHandler* NewL(CPositionerQ* aPositionerQ, CRequestHandler* aRequestHandler);
        ~CNETResponseHandler();

	public:
		TBool GetLastPosition(TPositionInfoBase& aPos, TTime aMaxAge, TBool aAllowPartial);
		
		// from MDataBusObserver
		void DataUpdate(TPositionInfo& aPosition, TInt aStatus, TInt aError, TTime aActualTime, TUint aAttributes, TUint aMode = 0);

	private:
        CNETResponseHandler(CPositionerQ* aPositionerQ, CRequestHandler* aRequestHandler);
        void ConstructL();
        
		TInt ReadPositionFromLocDataBus(TPositionInfo& aPosInfo, TTime& aActualTime);
		TInt ReadFinal(TPositionInfo& aPosInfo, TTime& aActualTime);
		
		void Subscribe();
	
	private:
		CNETDataBus* iNETDataBus;
		CPositionerQ* iPositionerQ;
		CRequestHandler* iRequestHandler;
		RLbsPositionUpdateRequests::TChannelIdentifer iChannel;
	};
