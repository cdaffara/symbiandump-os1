/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@file
@internalComponent
@released
*/

#ifndef PRIVACYHANDLEROBSERVER_H 
#define PRIVACYHANDLEROBSERVER_H

#include "lbsnetinternalapi.h"

/**
 * Used for feeding back responses from the Privacy Uniformity Layer 
 * to the Privacy and Location Handler
 */
class  MLbsSessionObserver;
class MPrivacyHandlerObserver
    {
public:
	virtual void SetServerObserver(MLbsSessionObserver* aNrhPrivacyServer) = 0;
    virtual void OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
							TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
                            TInt aResponseReason) = 0;
    virtual void OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId) = 0;
    };
    

#endif // PRIVACYHANDLEROBSERVER_H
