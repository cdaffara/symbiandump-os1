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

#ifndef __LBSAGPSREQUESTHANDLER_H__
#define __LBSAGPSREQUESTHANDLER_H__

#include <e32base.h>
#include "cagpsrequestbus.h"
#include "crequesthandler.h"

class CPositionerQ;

class CAgpsRequestHandler: public CRequestHandler
	{
	public:		
		static CAgpsRequestHandler* NewL(CPositionerQ* aPositionerQ);
		~CAgpsRequestHandler();

		void IssueStatus(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::ENoPowerAdvice);
		void IssueCancel(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
		void IssueMergedRequestL();

	private:
		void ConstructL();
		CAgpsRequestHandler(CPositionerQ* aPositionerQ);
		
		TInt MergeRequest(const TRequestParameters* aRequestParameters,
							const TBool aTracking,
							TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::ENoPowerAdvice);

		TInt MergeMethod(TLbsNetPosRequestMethodInt& aNewReq);
		void ResetMergedRequest();
		TInt ModeToIndex(TInt aMode);
		
	private:	
		CAgpsRequestBus* iAGPSRequestBus;
		TPositionModuleInfoExtended::TDeviceGpsModeCapabilities iCapabilities;
	};
	
#endif // __LBSREQUESTHANDLER_H__
