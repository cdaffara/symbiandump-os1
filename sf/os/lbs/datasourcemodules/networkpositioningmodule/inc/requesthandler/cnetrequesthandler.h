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

#ifndef __LBSNETREQUESTHANDLER_H__
#define __LBSNETREQUESTHANDLER_H__

#include <e32base.h>
#include "cnetrequestbus.h"
#include "crequesthandler.h"

class CNETRequestHandler: public CRequestHandler
	{
	public:		
		static CNETRequestHandler* NewL(CPositionerQ* aPositionerQ);
		~CNETRequestHandler();

		void IssueStatus(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::ENoPowerAdvice);
		void IssueCancel(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
		void IssueMergedRequestL();

	private:
		void ConstructL();
		CNETRequestHandler(CPositionerQ* aPositionerQ);
			
		TInt MergeRequest(const TRequestParameters* aRequestParameters,
							const TBool aTracking,
							TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::ENoPowerAdvice);

		void ResetMergedRequest();

	private:	
		RLbsPositionUpdateRequests::TChannelIdentifer	iChannel;	// the channel identifier for the key of RProperty object
		CNETRequestBus* iNETRequestBus;
	};
	
#endif // __LBSNETREQUESTHANDLER_H__
