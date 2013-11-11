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



#ifndef TE_LOCATIONINFOCONVERSIONOBSERVER_H
#define TE_LOCATIONINFOCONVERSIONOBSERVER_H
#include <lbs/lbslocationinfoconverter.h>
  
  
  class CTelocationinfoconversionobserver: public CTimer, public MLbsLocationInfoConverterObserver
    
    {
    
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTelocationinfoconversionobserver* NewL( );

        /**
        * Destructor.
        */
        ~CTelocationinfoconversionobserver();
        
        
        void OnConversionComplete( TInt aStatusCode );
                       
        void RunL( );
        
	    void DoCancel( );
	    
	    TInt ConversionResult();

	//	  RTimer iTimer;
        TRequestStatus iWaitStatus;
        
    private:

        /**
        * C++ default constructor.
        */
        CTelocationinfoconversionobserver( );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
        
        TInt iStatusCode;
        
    };

#endif      // TE_LOCATIONINFOCONVERSIONOBSERVER_H

// End of File
