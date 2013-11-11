/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* This file contains the class declarations for the plugin service and callback APIs.
*
*/




/**
 @file
 @publishedPartner
 @released
*/


#ifndef HWRMPLUGINSERVICE_H
#define HWRMPLUGINSERVICE_H


// INCLUDES
#include <e32base.h>

/** ECOM Plugin interface UID 
@publishedPartner
@released
*/
const TUid KCHWRMPluginServiceInterfaceUid = { 0x10205028 };
    
/**
*  Callback interface for the HWRM plugins.
*  Used to return data and/or error codes from an asynchronous plugin calls.
*  invoked by the HWRM server.
* 
@publishedPartner
@released
*/
class MHWRMPluginCallback
    {
    public:
        /**
        * Method to return data in response to a message from a HWRM plugin.
        * Related ProcessCommandL call must return before this method can
        * be called.
        *
        * @param aCommandId Command ID for which the response comes
        * @param aTransId   Unique transcation identifier of the original command
        * @param aData      Data returned from call. 
        *                   Data package contents are defined by command.
        * @leave Standard Symbian leaves if there are any problems handling response.
        */
        virtual void ProcessResponseL( const TInt aCommandId,
                                       const TUint8 aTransId, 
                                       TDesC8& aData ) = 0;
        
        /**
        * Method to notify events to a HWRM plugin.
        * Note: No implementation yet.
        *
        * @param aIndId An indication ID
        * @param aData  Data associated with the event ID. 
        *
        */
        virtual void EventL( const TUint32 aIndId,
                             TDesC8& aData ) = 0;
    };


/**
*  Plugin interface class to be implemented by adaptation.
*  Responsible for invoking the adaptation implementation of
*  each command and cancelling the command.
*
@publishedPartner
@released
*/
class CHWRMPluginService : public CBase
    {
    public:  // construction and destruction
    
        /**
        * Constructor method for instance.
        * Uses ECom to find correct instance.
        *
        * @param aMatch Determines the API that the loaded plugin implements.
        * @param aResponseCallback Pointer to plugin callback handler.
        * @leave Standard Symbian leaves.
        */
        inline static CHWRMPluginService* NewL(const TDesC8& aMatch,
                                               MHWRMPluginCallback* aResponseCallback);

        /**
        * Destructor
        */
        inline virtual ~CHWRMPluginService();       


    public:
        /**
        * Method to invoke a particular command in the plugin. 
        * Response to method is returned via separate ProcessResponseL
        * call. Call to ProcessResponseL is done after the call to
        * ProcessCommandL returns. Plugin must be able to handle another
        * ProcessCommandL before ProcessResponseL is called for previous
        * call(s) (i.e. multiple clients are attempting to access resource
        * simultaneously). If ProcessCommandL leaves, no corresponding
        * ProcessResponseL is expected. 
        *
        * @param aCommandId Command ID
        * @param aTransId   Transaction ID
        * @param aData      Data associated with command.
        *                   Data package contents are defined by command.
        *                   Some commands require no data and pass
        *                   empty buffer as aData;
        * @leave KErrGeneral or command specific leaves.
        */
        virtual void ProcessCommandL( const TInt aCommandId,
                                      const TUint8 aTransId,
                                      TDesC8& aData ) = 0;

        /**
        * Method to cancel a particular command. The corresponding
        * ProcessResponseL will not be called for cancelled commands,
        * whether cancel was successful or not. 
        *
        * @param aTransId   Transaction ID
        * @param aCommandId Command ID to optionally double check with the 
        *                   transaction ID
        * @leave Standard Symbian leaves. HWRM will consider
        *            transaction canceled even if this method leaves. 
        */
        virtual void CancelCommandL( const TUint8 aTransId, 
                                     const TInt aCommandId) = 0;
                                     
                                      
        

   protected:
   
        /**
        * Destructor identifier to be used with ECom framework.
        */
        TUid iDestructorIDKey;

        /**
        * Callback pointer to be used with responses to commands.
        * This pointer is not owned by this class.
        */
        MHWRMPluginCallback* iResponseCallback;  // not owned
                                      
    };

#include <hwrm/hwrmpluginservice.inl>


#endif      // HWRMPLUGINSERVICE_H   
            
// End of File
