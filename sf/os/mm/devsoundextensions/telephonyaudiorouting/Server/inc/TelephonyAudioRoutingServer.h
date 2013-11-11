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
* Description:   TelephonyAudioRoutingServer implementation
*
*/



#ifndef TELEPHONYAUDIOROUTINGSERVER_H
#define TELEPHONYAUDIOROUTINGSERVER_H

#include <e32base.h>
#include "TelephonyAudioRouting.h"

const TInt KMaxNumberOfSessions = 256; 

enum TTelephonyAudioRoutingPanic
	{
	EPanicGeneral,
	EPanicIllegalFunction,
	EPanicResourceError
	};

void PanicClient(const RMessage2& aMessage,TTelephonyAudioRoutingPanic aPanic);

// CLASS DECLARATION

/**
*  TelephonyAudioRoutingServer class declaration
*
*  @lib TelephonyAudioRoutingServer.lib
*  @since Series 60 3.1
*/
class CTelephonyAudioRoutingServer : public CServer2
	{
		
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CTelephonyAudioRoutingServer* NewL();
        
        /**
        * Destructor.
        */
        ~CTelephonyAudioRoutingServer();
        
	public:
	
		/**
        * Start the TelephonyAudioRoutingServer thread
        * @since Series 60 3.1
        * @param TAny* aParams
        * @return KErrNone is successfull, else appropriate err code
        */
		IMPORT_C static TInt StartThreadL(TAny* aParams);
		
		/**
        * Add a new Session.
        * @since Series 60 3.1
        * @param none
        * @return void
        */
		void AddSession();
		
		/**
        * Remove an existing session.
        * @since Series 60 3.1
        * @param TInt aSessionId Id of the session to remove
        * @return void
        */
		void RemoveSession(TInt aSessionId);
		
		/**
        * Notify the policy session about a request to change output.
        * @since Series 60 3.1
        * @param TInt aSessionId Id of the session to utilize, aMessage contains output value
        * @return void
        */
		void DoSetOutputL (TInt aSessionId, const RMessage2& aMessage);
		
		/**
        * Assign a unique session Id to a new session.
        * @since Series 60 3.1
        * @param none
        * @return TInt session Id
        */
		TInt IdentifyAndAllocateNextFreeSessionIdL();
		
		/**
        * Free the session Id or a deleted session for re-use.
        * @since Series 60 3.1
        * @param TInt aSessionId Id of the session to free
        * @return void
        */
		void FreeSessionId(TInt aSessionId);
		
		/**
        * Used by policy session to indicate the session that requested a SetOutput() that
        * the request is complete, all the other session get OutputChanged() notification
        * @since Series 60 3.1
        * @param aOutput value of modified output, aError result of operation.
        * @return void
        */
		void SetOutputCompleteL(CTelephonyAudioRouting::TAudioOutput aOutput, TInt aError);
		
		/**
        * Used by policy session to notify all sessions that the output has changed.
        * (Policy initiated change in output)
        * @since Series 60 3.1
        * @param aOutput value of modified output
        * @return void
        */
		void OutputChanged(CTelephonyAudioRouting::TAudioOutput aOutput);		
		
		/**
        * Used by policy session to indicated to all other sessions that available outputs have changed.
        * @since Series 60 3.1
        * @param aOutputs contains array of available outputs
        * @return void
        */
		void AvailableOutputsChangedL( const TArray<CTelephonyAudioRouting::TAudioOutput>& aOutputs);
		
		/**
        * Set the Id of the policy session.
        * @since Series 60 3.1
        * @param TInt aSessionId is Id to set
        * @return void
        */
		void SetPolicySessionId(TInt aSessionId);
		
    	/**
        * Provide caller with iCurrentAudioOutput
        * @since Series 60 3.1
        * @param none
        * @return iCurrentAudioOutput
        */
    	CTelephonyAudioRouting::TAudioOutput& CurrentAudioOutput();  			
		
   		/**
        * Provide caller with iPreviousAudioOutput
        * @since Series 60 3.1
        * @param none
        * @return iPreviousAudioOutput
        */
    	CTelephonyAudioRouting::TAudioOutput& PreviousAudioOutput();  
    	
    	/**
        * Provide caller with iShowNoteMode
        * @since Series 60 3.1
        * @param none
        * @return iShowNoteMode
        */
    	TBool& ShowNoteMode(); 
    	
    	/**
        * Allows caller to set iShowNoteMode
        * @since Series 60 3.1
        * @param TBool aShowNoteMode
        * @return void
        */    	
 		void SetShowNoteMode(TBool aShowNoteMode);  	
    	
       	/**
        * Provide caller with iAvailableOutputs array
        * @since Series 60 3.1
        * @param none
        * @return: Array of available outputs
        */
    	RArray<CTelephonyAudioRouting::TAudioOutput>& AvailableOutputs();    				
								
		
	private:
	
		/**
        * C++ default constructor.
        */
        CTelephonyAudioRoutingServer(/*TProcessId& aClientPID*/);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		/**
        * From CServer2
        * Creates a server-side client session object.
        */
		CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
		
		/**
        * determines if audio output is in available outputs array.
        * @since Series 60 3.1
        * @param aOutput is current output value
        * @return TBool ETrue if current output is available, EFalse if not
        */		
		TBool IsAvailableOutput(CTelephonyAudioRouting::TAudioOutput aOutput);
			
	private:
	
		TInt iSessionCount;
		TInt iPolicySessionId;
		RArray<TInt> iSetOutputRequests;
		RArray<TInt> iSessionIdsInUse;
		CTelephonyAudioRouting::TAudioOutput iCurrentAudioOutput;
		CTelephonyAudioRouting::TAudioOutput iPreviousAudioOutput;
		RArray<CTelephonyAudioRouting::TAudioOutput> iAvailableOutputs;
		TBool iShowNoteMode;
		

	};

#endif
