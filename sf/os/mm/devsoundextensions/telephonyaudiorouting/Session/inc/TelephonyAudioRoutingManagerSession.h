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
* Description:   This class is the main interface to the TelephonyAudioRoutingServer. 
*				 It implements the client-side session.
*
*/

                                      
                                                                                 
#ifndef TELEPHONYAUDIOROUTINGMANAGERSESSION_H                                                
#define TELEPHONYAUDIOROUTINGMANAGERSESSION_H                                                  
                                                                                 
#include <e32base.h>
#include <e32std.h>     
#include "MTelephonyAudioRoutingPolicyObserver.h"
#include "TelephonyAudioRoutingClientServer.h"
  

class CTelephonyAudioRoutingPolicyRequest;
class CTelephonyAudioRoutingManager;

// CLASS DECLARATION

/**
*  Main interface to the TelephonyAudioRoutingServer. 
*  It implements the client-side session.
*
*  @lib TelephonyAudioRoutingManagerSession.lib
*  @since Series 60 3.1
*/                                     
                                                                              
class RTelephonyAudioRoutingManagerSession : public RSessionBase                                   
    { 
                                                                               
	public:                                                                          
                                                                        
    	IMPORT_C RTelephonyAudioRoutingManagerSession();    
    
    public:                                                    
         
    	/**
        * Connects a policy client to the server.
        * @since Series 60 3.1
        * @param aAudioRoutingManager: Client, aPolicyObserver: client observer class
        * @return TInt: if connection was successful
        */
    	IMPORT_C TInt Connect(CTelephonyAudioRoutingManager& aAudioRoutingManager, MTelephonyAudioRoutingPolicyObserver& aPolicyObserver);
    	
        /**
        * Closes connection to the server.
        * @since Series 60 3.1
        * @param none
        * @return void
        */
        IMPORT_C void Close();                                                      
                                      
	    /**
        * Used by policy to notify server of audio output changes.
        * @since Series 60 3.1
        * @param aOutput: client obj
        * @return void
        */
	    IMPORT_C void OutputChanged(CTelephonyAudioRouting::TAudioOutput aOutput); 
	    
	    /**
        * Used by policy to notify server that requested output change completed
        * @since Series 60 3.1
        * @param aOutput: client obj, aError: error code for operation
        * @return void
        */
	    IMPORT_C void OutputChangeCompleted(CTelephonyAudioRouting::TAudioOutput aOutput, TInt aError); 
	    	    
	    
    	/**
        * Used by policy session to get notification whenever any of the clients sends
        * a SetOutput request.
        * @since Series 60 3.1
        * @param none
        * @return void
        */
    	IMPORT_C void MonitorOutputChangeRequest();
    	
    	/**
        * Sets policy session Id.
        * @since Series 60 3.1
        * @param none
        * @return void
        */
    	IMPORT_C void SetPolicySessionIdL();
    	
    	/**
        * Cancel a previously send request.
        * @since Series 60 3.1
        * @param aRequest: request to cancel
        * @return void
        */
    	IMPORT_C void CancelRequest(TTelAudRtngServRqst aRequest);
    	
    	/**
        * Used by policy session to notify server if the available audio
        * output change.
        * @since Series 60 3.1
        * @param aOutputs: array of available outputs
        * @return void
        */
    	IMPORT_C void AvailableOutputsChangedL(const TArray<CTelephonyAudioRouting::TAudioOutput>& aOutputs);
    	
     	/**
        * Provide caller with iAudioOutputPkg
        * @since Series 60 3.1
        * @param none
        * @return iAudioOutputPkg
        */
    	IMPORT_C  TPckgBuf<CTelephonyAudioRouting::TAudioOutput>& AudioOutputPkg();    	 
    	 
	private:

   		/**
        * Start asynchronous request handlers.
        * @since Series 60 3.1
        * @param aAudioRoutingManager: client obj, aPolicyObserver: observer obj for callbacks
        * @return void
        */
   		void StartPolicyRequestHandlersL( CTelephonyAudioRoutingManager& aAudioRoutingManager, MTelephonyAudioRoutingPolicyObserver& aPolicyObserver );
     
	private:

  		TPckgBuf<CTelephonyAudioRouting::TAudioOutput> iAudioOutputPkg;      
		CTelephonyAudioRoutingPolicyRequest* iPolicyRequest;	
		TBool iConnected;  
                                                                             
    };                                                                           
                                      
                                                                                 
#endif         //TELEPHONYAUDIOROUTINGSESSION_H                                
                                                                                 
                                                                                 
