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



#ifndef CMmMBMSContextList_H
#define CMmMBMSContextList_H

// INCLUDES
#include "cmmtsyobjectlist.h"
#include "cmmmbmscontexttsy.h"



// CLASS DECLARATION

/**
 *  Contains information list for MBMS Context Tsy objects.
 *  This class provides the methods for the MBMS Context List object
 *  (CMmMBMSContextList class) to keep track of opened contexts. 
 */
NONSHARABLE_CLASS( CMmMBMSContextList ) : public CMmTsyObjectList
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aMmPacketService Pointer to the Packet Service object
         */ 
        static CMmMBMSContextList* NewL();

        /**
         * Default destructor.
         */        
        virtual ~CMmMBMSContextList();

        /**
         * Adds context to the context info list.
         *
         * @param aContext pointer of context to be added
         * @return KErrNone if success or error code
         */   
        virtual TInt AddContext( CMmMBMSContextTsy* aContext );

        /**
         * Removes context from the context info list.
         *
         * @param aContext context to be removed from context info list
         * @return TInt KErrNone if success or error code
         */                
        virtual TInt RemoveContext( CMmMBMSContextTsy* aContext );

        /**
         * Returns context defined by aIndex from context info list.
         *
         * @param aIndex of the  context to be retrieved
         * @return a pointer to a CMmMBMSContextTsy object that represent the context, or NULL if no such context exists.
         */  
        virtual CMmMBMSContextTsy* ContextByIndex( TInt aIndex ) const;
            
        /**
         * Returns the context that has name given in parameter aName.
         *
         * @param aName of the  context to be retrieved
         * @return a pointer to a CMmMBMSContextTsy object that represent the context, or NULL if no such context exists.
         */         
        virtual CMmMBMSContextTsy* ContextByName( 
        						const TInfoName& aName );

        /**
         * Returns number of created contexts.
         *
         * @return The number of context in the list
         */ 
        virtual TInt NumberOfContexts();

        /**
         * Sets proxy id and generates name for context.
         *
         * @param aNewName newly created name of this context
         * @param aProxyId of the context
         * @param aMaxContexts the maximal number of allowed contexts
         * @return The number of context in the list
         */ 
        TInt GenerateNewContextName( TDes& aNewName, 
            TUint8& aProxyId,
            TInt aMaxContexts );

 		/**
         * Returns boolen whether one of the MBMS context's status is receiving
         *          
         * @return TBool indicating whether one of the MBMS context is in receiving state
         */ 
		virtual TBool IsMbmsContextReceiving();

    private: // New functions

        /**
         * C++ default constructor.
         */
        CMmMBMSContextList();

        /**
         * Second phase constructor of CMmMBMSContextList.
         */
        void ConstructL();
        

    public:
        

        };

#endif // CMmMBMSContextList_H

//  End of file
