/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of COMASuplPosHandlerBase
*
*/

/**
 @file
 @publishedPartner
 @released
*/

#ifndef C_COMASUPLPOSHANDLER_H
#define C_COMASUPLPOSHANDLER_H


#include <lbs/epos_comasuplposhandlerbase.h>
#include "lbssuplproxyposclient.h"

// Forward Declaration
class COMASuplPosSession;

/**
* Implementation of COMASuplPosHandlerBase
* 
*/
class COMASuplPosHandler : public COMASuplPosHandlerBase
    {
    
    public:  
            
		/**
		* Factory function that instantiates a new object of COMASuplPosHandler
		*/
        static COMASuplPosHandlerBase* NewL();
        
        /**
		* Destructor
		*/
        ~COMASuplPosHandler();        
        
    public: 

        /**
		* From COMASuplPosHandlerBase
		* Initializes the instance of COMASuplPosHandler
		*/
        void InitializeL( TRequestStatus &aStatus );
        
        /**
		* From COMASuplPosHandlerBase
		* Cancels an outstanding request to initialize
		*/
        void CancelInitialize();
        
        /**
		* From COMASuplPosHandlerBase
		* Factory function to create a new instance of COMASuplPosSession
		*/
        COMASuplPosSessionBase* CreateNewSessionL( 
                                        MOMASuplObserver *aSuplObserver);
        
    private:
    
        /**
		* Constructor
		*/
        COMASuplPosHandler();
    
	private:
		/**
		* Session with the SUPL Proxy protocol module server
		*/
		RSuplProxyPosSession iSuplProxyPosSession;
    };


#endif // C_COMASUPLPOSHANDLER_H
