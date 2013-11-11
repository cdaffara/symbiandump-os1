/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ctlbsclientnotifyposupdao.h
*/
#if (!defined __CT_LBS_PRIVFW_PRIVACY_OBSERVER_AO_H__)
#define __CT_LBS_PRIVFW_PRIVACY_OBSERVER_AO_H__

#include <e32base.h>
#include <lbs.h>

#include <lbs/test/lbsprivfwproxy.h>

#include "epos_cposgsmprivacyrequestinfo.h"
#include "epos_mposverificationobserver.h"

class MApiDataObserver
    {
    public:
        virtual void ProcessApiData(TInt aRequestId, TInt aCompletionCode) = 0;
    };

class CT_LbsPrivFwApiDataObserver: public CActive, public MPosVerificationObserver
	{
public:
	// destructor
	~CT_LbsPrivFwApiDataObserver();

	// constructor
	static CT_LbsPrivFwApiDataObserver* NewL(MApiDataObserver* aTestStepObserver);
	
	void Start();
	
protected:
//	CActive:
	// Redeclares the base class CActive::DoCancel(). 
	virtual void DoCancel();
	// Redeclares the base class CActive::RunL(). 
	virtual void RunL();
	// Redeclares the base class CActive::RunError(). 
	virtual TInt RunError(TInt aError);
	void ConstructL(MApiDataObserver* aTestStepObserver);
    CT_LbsPrivFwApiDataObserver();
    void SelfComplete();
    TUint FindArrayIndex(TUint aReqId);

    //From MPosVerificationObserver
    virtual void HandleVerifyComplete(TInt aRequestId, TInt aCompletionCode);

private:
	MApiDataObserver* iTestStepObserver;
	TInt iRequestId;
	TInt iCompletionCode;
	};

#endif // __CT_LBS_PRIVFW_PRIVACY_OBSERVER_AO_H__
