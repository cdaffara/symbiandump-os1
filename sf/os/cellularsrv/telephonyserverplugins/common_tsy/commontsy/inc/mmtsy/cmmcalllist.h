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



#ifndef CMMCALLLIST_H
#define CMMCALLLIST_H

// INCLUDES
#include <etelmm.h>
#include "cmmtsyobjectlist.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmCallTsy;

// CLASS DECLARATION

/**
 * This class provides the means for the Phone object (CMmPhoneTsy class) to 
 * keep track of opened calls from all Lines. When ever a Call object is 
 * opened it should be put on the Phone's CallList. Exception: Call objects 
 * for incoming calls -> TSY does not want visibility of these objects to 
 * clients.
 */
NONSHARABLE_CLASS( CMmCallList ) : public CMmTsyObjectList
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param CMmPhoneTsy* aMmPhone: pointer to the Phone
         * @return created CallList object 
         */
        static CMmCallList* NewL( CMmPhoneTsy* aMmPhone );

        /**
         * Destructor.
         */
        virtual ~CMmCallList();

    public: // New functions

        /**
         * Returns the Call that has the given index in container
         *          
         *
         * @param const TInt aIndex: index of the Call which is to be returned.
         * @return Call object
         */
        virtual CMmCallTsy* GetMmCallByIndex( const TInt aIndex );

        /**
         * Returns the Call that has the given index in container
         *          
         *
         * @param const TInt aIndex: index of the Call which is to be returned.
         * @return Call object
         */
        virtual CMmCallTsy* GetMmCallByIndexAndLine( 
                    const TInt aIndex,
                    const TName* aLineName );

        /**
         * Returns the Call by ID
         *          
         *
         * @param const TInt aCallId: Call ID
         * @return Call object
         */
        virtual CMmCallTsy* GetMmCallById( const TInt aCallId );

        /**
         * Returns the Call by name
         *          
         *
         * @param const TName* aName: Name of the Call to be returned
         * @return Call object
         */
        virtual CMmCallTsy* GetMmCallByName( const TName* aName );

        /**
         * Returns the Call that has the given mode
         *          
         *
         * @param const  RMobileCall::TMobileService aCallMode: Call mode
         * @return Call object
         */
        virtual CMmCallTsy* GetMmCallByMode( const RMobilePhone::TMobileService aCallMode );

        /**        
         * Returns the Call that has the given status
         *          
         *
         * @param RMobileCall::TMobileCallStatus aStatus: Call status
         * @return Call object
         */
        virtual CMmCallTsy* GetMmCallByStatus( RMobileCall::TMobileCallStatus aStatus );

        /**
         * Removes the Call that has given ID, from the Call container
         *          
         *
         * @param const TInt aMmCallId: Call ID
         * @return result success/failure
         */
        virtual TInt RemoveMmCallById( const TInt aMmCallId );

        /**
         * Removes Calls from Call list opened from a given Line
         *          
         *
         * @param const TName* aName: Line name
         */
        virtual void RemoveCallsByLine( const TName* aLineName );

        /**
         * This method adds a TSY object to the container
         *          
         *
         * @param CTelObject* aObject: object to be added to the list.
         * @return status success/failure
         */        
        virtual TInt AddObject( CTelObject* aObject );
        
    private: // New functions

        /**
         * C++ default constructor.
         */
        CMmCallList();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    };

#endif // CMMCALLLIST_H

//  End of file
