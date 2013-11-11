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
*
*/



#ifndef CMMPACKETCONTEXTLIST_H
#define CMMPACKETCONTEXTLIST_H

// INCLUDES
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include <etelpckt.h>
#include <ctsy/serviceapi/mmtsy_defaults.h> // KMmMaxNumberOfContexts
#include "Cmmpacketservicetsy.h"
#include "cmmtsyobjectlist.h"
#include "cmmmessagemanagerbase.h"

// CONSTANTS
const TInt KMaxNumberOfNifs = 9;

// FORWARD DECLARATIONS
class CMmPacketServiceTsy;
class CMmPacketContextTsy;

// CLASS DECLARATION

/**
 *  Contains information list for Packet Context Tsy objects.
 *  This class provides the methods for the Packet Context List object
 *  (CMmPacketContextList class) to keep track of opened contexts. When a 
 *  Context object is opened it should be put on the Service's Context List.
 */
NONSHARABLE_CLASS( CMmPacketContextList ) : public CMmTsyObjectList
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aMmPacketService Pointer to the Packet Service object
         */ 
        static CMmPacketContextList* NewL
            (
            CMmPacketServiceTsy* const aPacketService
            );

        /**
         * Default destructor.
         */        
        virtual ~CMmPacketContextList();

        /**
         * Adds context to the context info list.
         *
         * @param aContext pointer of context to be added
         * @param aProxyId of the context
         * @param aContextName name of current context
         * @return TInt success or error code
         */   
        TInt virtual AddObject( CMmPacketContextTsy* const aContext,
            const TInt aChannelId,
            const TUint8 aObjectId,
            const TInfoName& aContextName );

        /**
         * Removes context from the context info list.
         *
         * @param aContext context to be removed from context info list
         * @return TInt success or error code
         */                
        TInt virtual RemoveObject( CMmPacketContextTsy* const aContext );

        /**
         * Returns context defined by aIndex from context info list.
         *
         * @param aIndex of the  context to be retrieved
         * @return CMmPacketContextTsy pointer.
         */  
        virtual CMmPacketContextTsy* PacketContextByIndex( 
        									const TInt aIndex ) const;
            
        /**
         * Returns the context that has name given in parameter aName.
         *
         * @param aName of the  context to be retrieved
         * @return CMmPacketContextTsy pointer.
         */         
        virtual CMmPacketContextTsy* PacketContextByName( 
        						const TInfoName* const aName ) const;

        /**
         * Returns number of created contexts.
         *
         * @return TInt number of context in the list
         */ 
        virtual TInt NumberOfContexts() const;

        /**
         * Sets proxy id and generates name for context.
         *
         * @param aProxyId of the context
         * @return aNewName newly created name of this context
         * @return TInt number of context in the list
         */ 
        TInt GenerateNewContextName( TDes& aNewName, 
            TUint8& aProxyId );

        /**
         * Returns context status from the context info list if aIndex is 
         * valid.
         *          
         *
         * @param aIndex of the  context to be retrieved
         * @return aStatus reference to context status data.
         */ 
        TInt GetContextStatusByIndex( const TInt aIndex,
            RPacketContext::TContextStatus& aStatus ) const;

        /**
         * Returns context name from the context info list if aIndex is valid.
         *          
         *
         * @param aIndex of the  context to be retrieved
         * @return aName name of the context
         */ 
        TInt GetContextNameByIndex( const TInt aIndex,
            								TName& aName ) const;

        /**
         * Enumerate network interfaces.
         *          
         *
         * @return aCount count of nifs in return
         */
        void EnumerateNifs( TInt* aCount );

        /**
         * Get network interface information.
         *          
         *
         * @param aIndex of the  context to be retrieved
         * @return aNifInfoV2 information of the wanted NIF
         */
        TInt GetNifInfo( const TInt* aIndex, 
        					RPacketService::TNifInfoV2* aNifInfoV2 );

        /**
         * Number of contexts in network interface.
         *          
         *
         * @param aExistingContextName name of the context
         * @return aCount count of contexts in NIF
         */
        void EnumerateContextsInNif
            (
            const TInfoName aExistingContextName, 
            TInt* aCount
            );

        /**
         * Gets name of context in network interface.
         *          
         *
         * @param aContextNameInNif name of one of the context(s) belonging to
         * this NIF.
         * @return aContextName name of contexts in return
         */
        TInt GetContextNameInNif( 
        		const RPacketService::TContextNameInNif* aContextNameInNif,
            	TDes* aContextName );

    private: // New functions

        /**
         * C++ default constructor.
         */
        CMmPacketContextList();

        /**
         * Second phase constructor of CMmPacketContextList.
         */
        void ConstructL();

        /**
         * Initialize used lists and attributes.
         */
        void InitAttributesAndListsL();


    private: // Data
        
        /**
         * Context Info List contains TMmContextInfo elements.
         */
        struct TMmContextInfo
                : public CBase
            {
            TUint8 iChannelId;
            TUint8 iObjectId;
            TInfoName iContextName;
            CMmPacketContextTsy* iObject;
            };

        /**
         * Proxy Id List contains TMmProxyIdList elements.
         */
        struct TMmProxyIdList
                : public CBase
            {
            TUint8 iChannelId;
            TUint8 iObjectId;
            TBool iIsFree;
            };

        /**
         * A pointer to multimode packet service object.
         * Not own.         
         */
        CMmPacketServiceTsy* iPacketService;

        /**
         * A context info list.
         */

        TMmContextInfo iContextInfoArray[KMmMaxNumberOfContexts];

        /**
         * A proxy id list.
         */

        TMmProxyIdList iProxyIdList[KMmMaxNumberOfContexts];

        /**
         * Number of created contexts in context info list.
         */
        TInt iNumberOfCreatedContexts;

        /**
         * Number used to generate individual context name.
         */
        TUint8 iIndividualNameForContext;

    };

#endif // CMMPACKETCONTEXTLIST_H

//  End of file
