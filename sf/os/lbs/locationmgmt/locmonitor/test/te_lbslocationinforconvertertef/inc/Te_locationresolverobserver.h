/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test module to implement Advanced trigger supervision cases
*
*/



#ifndef TE_BASICLOCATIONINFOCONVERSIONOBSERVER_H
#define TE_BASICLOCATIONINFOCONVERSIONOBSERVER_H
#include <lbs/lbslocationresolver.h>
  
  
  class CTeLocationResolverObserver: public CTimer, public MLbsLocationResolverObserver
    
    {
    
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTeLocationResolverObserver* NewL( );

        /**
        * Destructor.
        */
        ~CTeLocationResolverObserver();
        
        void OnResolveLocationComplete(TInt aStatus, const TLocality& aLocality);
                       
        void RunL( );
        
	    void DoCancel( );
	    
	    TInt ConversionResult();
	    
	    TInt ConversionLocality(TLocality& aLocality);


	//	  RTimer iTimer;
        TRequestStatus iWaitStatus;
        
    private:

        /**
        * C++ default constructor.
        */
        CTeLocationResolverObserver( );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
        
        TInt iStatusCode;
        TLocality iLocality;
        TBool iFromCache;
        
    };

#endif      // TE_LOCATIONINFOCONVERSIONOBSERVER_H

// End of File
