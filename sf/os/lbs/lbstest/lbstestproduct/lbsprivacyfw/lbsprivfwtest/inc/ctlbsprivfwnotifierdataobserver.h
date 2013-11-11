/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file ctlbsprivfwnrhobserverao.h
*/
#ifndef __CT_LBS_PRIVFW_NOTIFIER_DATA_OBSERVER_H__
#define __CT_LBS_PRIVFW_NOTIFIER_DATA_OBSERVER_H__

#include <e32base.h>
#include <lbs.h>

#include <lbs/test/lbsprivfwproxy.h>
#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwverifyrequest.h"

    
class MNotifierDataObserver
    {
    public:
    	virtual void ProcessNotifierData(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData) = 0;
    	virtual void ProcessRequestComplete(TInt aRequestId, TInt aCompletionCode) = 0;
        virtual void ProcessNotiferCtrlCloseDown() = 0;
    };
    

class CT_LbsPrivFwNotifierDataObserver: public CActive, public MPrivFwProxyObserver
	{
	public:
		// destructor
		~CT_LbsPrivFwNotifierDataObserver();
		// constructor
		static CT_LbsPrivFwNotifierDataObserver* NewL(MNotifierDataObserver* aTestStepObserver);
		void Send();
		void Start();

		void SetLogger(CTestExecuteLogger& aLogger){ iLogger = aLogger; };
		CTestExecuteLogger& Logger() {return iLogger;};

	protected:
		// Redeclares the base class CActive::DoCancel(). 
		virtual void DoCancel();
		// Redeclares the base class CActive::RunL(). 
		virtual void RunL();
		// Redeclares the base class CActive::RunError(). 
		virtual TInt RunError(TInt aError);
		void ConstructL(MNotifierDataObserver* aTestStepObserver);
	    CT_LbsPrivFwNotifierDataObserver();
	    void SelfComplete();
 
		virtual void NotifierDataAvailable(TInt aRequestId, TLbsTestRequestInfo& aPrivNotifierData);
		virtual void NotifierRequestComplete(TInt aRequestId, TInt aCompletionCode);
	    virtual void NotiferCtrlCloseDown();
	private:
	
		TPrivacyHandlerType		iPrivacyHandlerType;
		
		TBool iRequestComplete;
		TInt iCompletionCode;
		MNotifierDataObserver* iTestStepObserver;
		TInt iRequestId; 
		TLbsTestRequestInfo iPrivNotifierData;
		CTestExecuteLogger iLogger;

	};

#endif // __CT_LBS_PRIVFW_NOTIFIER_DATA_OBSERVER_H__
