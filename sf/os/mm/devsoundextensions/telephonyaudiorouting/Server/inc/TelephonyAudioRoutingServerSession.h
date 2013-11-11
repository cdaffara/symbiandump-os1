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
* Description:   Server-side session implementation
*
*/



#ifndef TELEPHONYAUDIOROUTINGSERVERSESSION_H
#define TELEPHONYAUDIOROUTINGSERVERSESSION_H

#include "TelephonyAudioRoutingServer.h"
#include "TelephonyAudioRoutingClientServer.h"
#include "TelephonyAudioRouting.h"

class CTelephonyAudioRoutingServer;

/**
*  Server-side session implementation
*  
*
*  @lib TelephonyAudioRoutingServer.lib
*  @since Series 60 3.1
*/
class CTelephonyAudioRoutingServerSession : public CSession2
	{
		
	public:
	
		/**
        * C++ default constructor.
        */
		CTelephonyAudioRoutingServerSession(TInt aSessionId);
		
		/**
        * Returns the session ID.
        * @since Series 60 3.1
        * @return sessionId
        */
		TInt SessionId();
		
		/**
        * Notifies the client-side session that the SetOutput request is complete.
        * @since Series 60 3.1
        * @param aOutput: client obj, aError: Error code of operation, aShowNote: for show note display
        * @return void
        */
		void SetOutputComplete(CTelephonyAudioRouting::TAudioOutput aOutput, TInt aError, TBool aShowNote);
		
		/**
        * Notifies the client-side session that available audio outputs have changed.
        * @since Series 60 3.1
        * @param aOutputs: array of available outputs
        * @return void
        */
		void AvailableOutputsChanged( const TArray<CTelephonyAudioRouting::TAudioOutput>& aOutputs);
		
		/**
        * Notifies the client-side session that the current audio output has changed.
        * @since Series 60 3.1
        * @param aMessage: server side message containing output info
        * @return void
        */
		void OutputChangedL(const RMessage2& aMessage);
		
		/**
        * After output has been changed, this method notifies all other clients
        * @since Series 60 3.1
        * @params aOutput: client obj, TBool aShowNote
        * @return void
        */
		void OutputChanged(CTelephonyAudioRouting::TAudioOutput aOutput, TBool aShowNote);		
		
		/**
        * Notification that user has requested to change the audio output.
        * @since Series 60 3.1
        * @param aOutput: client obj
        * @return void
        */
		void OutputChangeRequested(CTelephonyAudioRouting::TAudioOutput aOutput);

 	private:

		~CTelephonyAudioRoutingServerSession();
		
		/**
        * Provides ref to server
        * @since Series 60 3.1
        * @param void
        * @return ref to CTelephonyAudioRoutingServer
        */
		inline CTelephonyAudioRoutingServer& Server();
		
		/**
        * From CSession2
        * Handles the servicing of a client request that has been passed to the server.
        */
		void ServiceL(const RMessage2& aMessage);
		
		/**
        * Handles an error from ServiceL()
        * @since Series 60 3.1
        * @param aMessage: server side message, aError: error code
        * @return void
        */
		void ServiceError(const RMessage2& aMessage,TInt aError);
		
		/**
        * Send a request to audio policy to set audio output.
        * @since Series 60 3.1
        * @param aMessage: server side message for policy
        * @return void
        */
		void DoSetOutputL (const RMessage2& aMessage);
		
		/**
        * Returns the number of available outputs to client-side session.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void GetNoOfAvailableOutputs (const RMessage2& aMessage);
		
		/**
        * Returns the available audio outputs to client-side session.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void GetAvailableOutputsL (const RMessage2& aMessage);
		
		/**
        * Add a message to message queue. Complete the message when output changes.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void NotifyIfOutputChangedL (const RMessage2& aMessage);
		
		/**
        * Add a message to message queue. Complete the message when available audio 
		* outputs change.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void NotifyIfAvailOutputsChangedL (const RMessage2& aMessage);
		
		/**
        * Used by policy session to listen to any SetOutput request from other sessions.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void MonitorOutputChangeL (const RMessage2& aMessage);
		
		/**
        * Set the policy session Id.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void SetPolicySessionId(const RMessage2& aMessage);
		
		/**
        * Used by policy session to indicate that the SetOutput request is complete.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void OutputChangeCompleteL(const RMessage2& aMessage);
		
		/**
        * Notify the client-side session that available outputs have changed.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void DoAvailableOutputsChangedL (const RMessage2& aMessage);
		
		/**
        * Cancel the outstanding request.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void CancelRequestL(const RMessage2& aMessage);
		
		/**
        * Session initialization.
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void InitializeL(const RMessage2& aMessage);
		
		/**
        * Gets default values form server for the client
        * @since Series 60 3.1
        * @param aMessage: server side message
        * @return void
        */
		void GetDefaultValuesL(const RMessage2& aMessage);	

	private:
	
		const TInt iSessionId;
		RArray<RMessage2> iMessageArray;
   		RArray<CTelephonyAudioRouting::TAudioOutput> iSessionAvailableOutputs;		
  		TBool iPolicyFlag;
  		TBool iConnected;
  		
  	
  	};
	
#endif 		// TELEPHONYAUDIOROUTINGSERVERSESSION_H
