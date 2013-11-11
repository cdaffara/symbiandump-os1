/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message handling 
*
*/


#ifndef CRESTRICTEDAUDIOOUTPUTMESSAGEHANDLER_H
#define CRESTRICTEDAUDIOOUTPUTMESSAGEHANDLER_H

#include <mmfcontroller.h>
#include <RestrictedAudioOutputMessageTypes.h>


class CRestrictedAudioOutput;

/**
 * CRestrictedAudioOutputMessageHandler class documentation.
 *
 * @lib RestrictedAudioOutputMessageHandler.lib
 *
 * @since S60 5.0 
 */
class CRestrictedAudioOutputMessageHandler: public CMMFObject
	{
    public:  
    
  	/**
     * Constructs, and returns a pointer to, a new 
     * CRestrictedAudioOutputMessageHandler object.
     * Leaves on failure..
     * @param 
     * @return CRestrictedAudioOutputMessageHandler* A pointer to newly created object.
     */                
    IMPORT_C static CRestrictedAudioOutputMessageHandler* NewL(TAny* customInterface);

		
   /**
    * Destructor.
    */
    virtual ~CRestrictedAudioOutputMessageHandler();
    

    /**
     * Returns class identifier to caller.
     * 
     *
     * @since S60 5.0
     * @param 
     * @return const TUid identifier
     */ 	
	IMPORT_C static TUid Uid();

	/**
	 * Handles Incoming Messages
	 * @param TMMFMessage& aMessage: Handles message package from caller
   	 * @return 
   	 */
	virtual void HandleRequest(TMMFMessage& aMessage);        

	private:

		/**
		* C++ default constructor.
		*/
		CRestrictedAudioOutputMessageHandler(CRestrictedAudioOutput* aAudioOutput);

		/**
        * Internally processes incoming messages
        * @since Series 60 5.0
        * @param TMMFMessage& aMessage
        * @return void
        */
		void DoHandleRequestL(TMMFMessage& aMessage);

	
		/**
        * Internally, handles processing of message request for a commit
        * @since Series 60 5.0
        * @param TMMFMessage& aMessage
        * @return void
        */
		void DoCommitL(TMMFMessage& aMessage);
		
        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();


    private:    // Data
        CRestrictedAudioOutput*	iAudioOutput;
		TMMFMessage*		 	iMessage;

		TInt iCount;


	};

#endif      // CRESTRICTEDAUDIOOUTPUTMESSAGEHANDLER_H
            
// End of File
