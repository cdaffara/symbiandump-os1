/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used as an reference implementation
*
*/


#ifndef CASYSTUBOBSERVER_H
#define CASYSTUBOBSERVER_H

#include <e32property.h>
#include "AccMonConnetionConsts.h"
#include "AsyStubService.h"
#include "AsyConst.h"

/**
*  A pub sub based Active object for reference implementation.
*  Demonstrates asynchronous event handling from connection emulator.
*/
class CASYStubObserver : public CActive
    {
public:
    
    /**
    * Two-phased constructor.
    * @param aService Pointer to the main service object.
    * @return The created object.        
    */
    static CASYStubObserver* NewL( CASYStubService& aService );
    
    /**
    * C++ default constructor.
    * @param aService Pointer to the main service object.
    */
    CASYStubObserver( CASYStubService& aService );
    
    /**
    * Destructor
    */
    ~CASYStubObserver();
    
    /**
    * Set subscribe.
    */
    void IssueRequest();
          
public:   
        
protected:
    
    // From CActive
    void DoCancel();
    
    // From CActive
    void RunL();
    
    // From CActive
    TInt RunError( TInt aError );
    
private:
    
    // Property for Pub/Sub
    RProperty iSubscribe;
    // Pointer to service class
    CASYStubService* iService;
    
    };
#endif // CASYSTUBOBSERVER_H
