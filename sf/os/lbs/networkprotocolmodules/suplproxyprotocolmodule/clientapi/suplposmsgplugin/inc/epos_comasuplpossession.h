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
* Description:  Implementation of COMASuplPosSessionBase
*
*/


#ifndef C_COMASUPLPOSSESSION_H
#define C_COMASUPLPOSSESSION_H


#include <lbs/epos_comasuplpossessionbase.h>
#include <lbs/epos_tomasuplallowedcapabilities.h>
#include "lbssuplproxyposclient.h"
#include "lbsrequesthandler.h"

/**
* Implementation of the COMASuplPosSessionBase
*/
class COMASuplPosSession :	public COMASuplPosSessionBase,
							public MRequestHandlerObserver
    {
    
    public:  // Constructors and destructor
            
        /**
         * Factory function that instantiates a new object of COMASuplPosSession
         *
         */
        static COMASuplPosSessionBase* NewL( MOMASuplObserver* aSuplObserver, RSuplProxyPosSession& aSuplProxyPosSession );
        
        /**
         * Destructor
         *
         */
        ~COMASuplPosSession();        
        
    public: 

    	/*
    	 * From MRequestHandlerObserver
    	 * Handle GetPositionL() response
    	 */
    	void OnGetPositionComplete(TInt aResult);
    	/*
    	 * From MRequestHandlerObserver
    	 * Handle GetSuplInfoL() response
    	 */
    	void OnGetSuplInfoComplete(TInt aResult);    	
        /**
         * From COMASuplPosSessionBase
         * Initializes the instance of COMASuplPosSession
         */
        void InitializeL( const TInt aRequestID, TRequestStatus &aStatus );
        
        /**
         * From COMASuplPosSessionBase
         * Cancels an outstanding request to initialize
         */
        void CancelInitialize();
        
        /**
         * From COMASuplPosSessionBase
         * Handles SUPL POS payload
         */
        void HandleSuplPosMessageL( const COMASuplPosPayload *aPosPayload );
        
        /**
         * From COMASuplPosSessionBase
         * Provides different POS parameters to the SUPL FW
         */
        void GetSuplInfoL( COMASuplInfoRequestList *aInfoRequestList,
    						    TRequestStatus &aStatus );
    						  
        /**
         * From COMASuplPosSessionBase
         * Cancels an outstanding request to GetSuplInfoL
         */
        void CancelSuplInfoRequest();
        
        /**
         * From COMASuplPosSessionBase
         * Provides position estimates to the SUPL FW
         */
        void GetPositionL( TRequestStatus &aStatus, 
                                COMASuplPosition* aPosition );
        
        /**
         * From COMASuplPosSessionBase
         * Cancels an outstanding request to GetPositionL
         */
        void CancelGetPosition();
        
        /**
         * From COMASuplPosSessionBase
         * Terminates a SUPL POS Session
         */
        void SessionEnd();

        /**
        * From COMASuplPosSessionBase
        */ 
        void SetSLPAddressUsed(const TDesC& aServerAddress); 
        
        /**
        * From COMASuplPosSessionBase
        */ 
        void SetPosMethodAndAllowedCapabilities( TOMASuplAllowedCapabilities aAllowedCapabilities, TOMASuplPositioningMethod aPosMethod);

          
    private:
    
        /**
         * Creates an IPC connection with the SUPL gateway, forwards 
         *				   the request and terminates the connection
         */
        COMASuplPosSession( MOMASuplObserver* aSuplObserver, RSuplProxyPosSession& aSuplProxyPosSession );
        
        /*
         * 2 stage construction
         */
        void ConstructL();
        
        /**
         * Cancels an outstanding request
         */
        void CancelRequest();
    

    private:
    
        /**
         * State information of an instance of COMASuplPosSession
         */
        enum TPosSessionState 
            {
            EIdle, 
            EGetSuplInfo, 
            EGetPosition,
            };
        
        /**
         * Contains the client request status
         */        
        TRequestStatus*             iClientStatus;
        
        /**
         * Contains the request ID 
         */
        TInt                        iRequestID;
        
        /**
         * A flag field to determine if the instance of COMASuplPosSession
         * was initialized
         */        
        TBool                       iIsInitialized;
        
        /**
         * Data field to hold the state information
         */
        TPosSessionState            iState;
        
        /**
		* Data field to hold the payload information. The instance of 
		* COMASuplPosSession instantiates this object and also takes 
		* ownership of the same.
		*/       
        COMASuplPosPayload*         iPosPayload;
		
		/**
		* Session with the Supl Proxy Protocol Module
		*/
		RSuplProxyPosSession& 		iSuplProxyPosSession;
		
		/**
		* SubSession with the Supl Proxy Protocol Module
		*/		
		RSuplProxyPosSubSession     iSuplProxyPosSubSession;	
		
		/**
		 * Processes async requests and calls us back when they are complete
		 */
		CSuplPosRequestHandler*		iRequestHandler;
    };


#endif // C_COMASUPLPOSSESSION_H
