/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This class is the main interface to the TelephonyAudioRoutingServer. 
*				 It implements the client-side session.
*
*/

                                      
                                                                                 
#ifndef TELEPHONYAUDIOROUTINGSESSION_H                                                
#define TELEPHONYAUDIOROUTINGSESSION_H                                                  
                                                                                 
#include <e32base.h>
#include <e32std.h>     
#include "TelephonyAudioRouting.h" 
#include "MTelephonyAudioRoutingPolicyObserver.h"
#include "TelephonyAudioRoutingClientServer.h"
  
  
class CTelephonyAudioRoutingRequest;
class CTelephonyAudioRoutingManager;

// CLASS DECLARATION

/**
*  Main interface to the TelephonyAudioRoutingServer. 
*  It implements the client-side session.
*
*  @lib TelephonyAudioRoutingSession.lib
*  @since Series 60 3.1
*/                                     
                                                                              
class RTelephonyAudioRoutingSession : public RSessionBase                                   
    { 
                                                                               
	public:                                                                          
                                                                        
    	IMPORT_C RTelephonyAudioRoutingSession();    
    
    public:                                                    
         
        /**
        * Connects a client to the  server.
        * @since Series 60 3.1
        * @param aAudioRouting obj: client obj, aObserver: Client's observer class
        * @return TInt connection success or failure
        */                                                                        
    	IMPORT_C TInt ConnectL(CTelephonyAudioRouting& aAudioRouting, MTelephonyAudioRoutingObserver& aObserver);        
    	
        /**
        * Closes connection to the server.
        * @since Series 60 3.1
        * @param none
        * @return void
        */
        IMPORT_C void Close();                                                      
                                                                       
	    /**
        * Gets the client side version number.
        * @since Series 60 3.1
        * @param none
        * @return TVersion: version
        */
	    IMPORT_C TVersion Version() const;                                                    
                                                                                 
	    /**
        * Send a SetOutput request to server.
        * @since Series 60 3.1
        * @params aOutput: output value to set
        *		  aShowNote: Boolean to indicate note option
        * @return void
        */
	    IMPORT_C void SetOutput(CTelephonyAudioRouting::TAudioOutput aOutput, TBool aShowNote); 

    	/**
        * Get the available audio outputs from the server.
        * @since Series 60 3.1
        * @param none
        * @return void
        */
    	IMPORT_C void GetAvailableOutputsL();
    	
    	/**
        * Cancel a previously send request.
        * @since Series 60 3.1
        * @param aRequest: request to cancel
        * @return void
        */
    	IMPORT_C void CancelRequest(TTelAudRtngServRqst aRequest);
    	
      	/**
        * Send a request to server to receive notification whenever current audio output changes.
        * @since Series 60 3.1
        * @param none
        * @return void
        */
    	IMPORT_C void NotifyIfOutputChanged();
    	
    	/**
        * Send a request to server to receive notification whenever available audio output changes.
        * @since Series 60 3.1
        * @param none
        * @return void
        */
    	IMPORT_C void NotifyIfAvailableOutputsChanged();
    	
    	// Accessor methods:
    	
     	/**
        * Provide caller with iCurrentAudioOutputPkg
        * @since Series 60 3.1
        * @param none
        * @return iCurrentAudioOutputPkg
        */
    	IMPORT_C  TPckgBuf<CTelephonyAudioRouting::TAudioOutput>& CurrentAudioOutputPkg();   	
    	  	
     	/**
        * Provide caller with iPreviousAudioOutputPkg
        * @since Series 60 3.1
        * @param none
        * @return iPreviousAudioOutputPkg
        */
    	IMPORT_C  TPckgBuf<CTelephonyAudioRouting::TAudioOutput>& PreviousAudioOutputPkg();  

     	/**
        * Provide caller with iErrPkg
        * @since Series 60 3.1
        * @param none
        * @return iErrPkg value
        */
    	IMPORT_C  TPckgBuf<TInt>& ErrPkg();
    	
     	/**
        * Provide caller with iAvailableOutputs array
        * @since Series 60 3.1
        * @param none
        * @return: Array of available outputs
        */
    	IMPORT_C RArray<CTelephonyAudioRouting::TAudioOutput>& AvailableOutputs();  
    	
      	/**
        * Sets value for iOkToGetShowNoteMode after successful completion of SetOutput call
        * @since Series 60 3.1
        * @param none
        * @return: none
        */
    	IMPORT_C void SetShowNoteMode();  
    	
    	
      	/**
        * Provides caller with value for iShowNoteModePkg
        * @since Series 60 3.1
        * @param none
        * @return: none
        */
    	IMPORT_C TPckgBuf<TBool>& ShowNoteMode(); 
    	
    	
      	/**
        * Will reset iOkToGetShowNoteMode value to EFalse
        * @since Series 60 3.1
        * @param none
        * @return: none
        */
    	IMPORT_C void ResetShowNoteMode();   
    	
      	
      	/**
        * Provides caller with value for iOkToGetShowNoteMode
        * @since Series 60 3.1
        * @param none
        * @return: none
        */
    	IMPORT_C TBool OkToGetShowNoteMode();  
    	
    	
      	/**
        * Provides caller with default values from server
        * @since Series 60 3.1
        * @param none
        * @return: none
        */    	
    	IMPORT_C void GetDefaultValuesL();    	     	       	    	    	

	private:

   		/**
        * Start asynchronous request handlers.
        * @since Series 60 3.1
        * @param aAudioRouting: client obj, aObserver: client's observer class for callbacks
        * @return void
        */
   		void StartRequestHandlersL( CTelephonyAudioRouting& aAudioRouting, MTelephonyAudioRoutingObserver& aObserver );

 
	private:

 		TPckgBuf<CTelephonyAudioRouting::TAudioOutput> iCurrentAudioOutputPkg;  		
  		TPckgBuf<CTelephonyAudioRouting::TAudioOutput> iPreviousAudioOutputPkg; 
  		TPckgBuf<TInt> iErrPkg;   
   		TPckgBuf<TBool> iShowNoteModePkg; 
   		TPckgBuf<TTelephonyAudioRoutingDefaultParams> iDefaultValuePkg;
 	    RArray<CTelephonyAudioRouting::TAudioOutput> iAvailableOutputs;
      
		RPointerArray<CTelephonyAudioRoutingRequest> iRequests;  
	
		TBool 	iConnected; 
		TBool	iOkToGetShowNoteMode;

                                                                             
    };                                                                           
                                      
                                                                                 
#endif         //TELEPHONYAUDIOROUTINGSESSION_H                                
                                                                                 
                                                                                 
