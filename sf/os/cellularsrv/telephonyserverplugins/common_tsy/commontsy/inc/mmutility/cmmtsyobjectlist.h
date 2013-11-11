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



#ifndef CMMTSYOBJECTLIST_H
#define CMMTSYOBJECTLIST_H

//  INCLUDES
#include <e32base.h>
#include <et_phone.h>

// FORWARD DECLARATIONS
class CMmPhoneTsy;

// CLASS DECLARATION

/**
* Base class for CMmCallList and CMmLineList classes.
*/
NONSHARABLE_CLASS( CMmTsyObjectList ) : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CMmTsyObjectList();

        // New functions
        
        /**
         * Returns the object that has the given index in container
         *          
         * 
         * @param aIndex index of the object which is to be returned.
         * @return CTelObject* Object to be returned
         */
        virtual CBase* GetObjectByIndex( const TInt aIndex ) const;

        /**
         * This method adds a TSY object to the container
         *          
         * 
         * @param aObject object to be added to the list.
         * @return TInt status success/failure
         */
        virtual TInt AddObject( CTelObject* aObject );

        /**
         * Removes the given object from the container
         *          
         * 
         * @param aObject object to be removed from the list.
         * @return TInt status success/failure
         */
        virtual TInt RemoveObject( CTelObject* aObject );

        /**
         * Removes all TSY objects from the container
         *          
         * 
         */
        virtual void RemoveAll();

        /**
         * Return number of calls in the call container
         *          
         * 
         * @return TInt number of objects in the container
         */
        virtual TInt GetNumberOfObjects();

    protected:  // New functions

        /**
         * Create Call Container
         *          
         * 
         * @return TInt status success/failure
         */  
        TInt CreateContainerL();

        /**
         * Delete Call Container
         *          
         * 
         */  
        void DeleteContainer();
    
    protected:  // Data
    
        /**
         * A pointer to the multimode phone object
         * Not own.
         */                  
        CMmPhoneTsy* iMmPhone;

        /**
         * Container containing all the objects
         * Own.         
         */         
        CArrayPtrFlat<CTelObject>* iObjectContainer;    

    };

#endif // CMMTSYOBJECTLIST_H 

//  End of file

