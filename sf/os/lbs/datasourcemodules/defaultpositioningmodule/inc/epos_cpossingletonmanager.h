/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSSINGLETONMANAGER_H
#define CPOSSINGLETONMANAGER_H

#include <e32base.h>

// Singleton object ID.
enum {
    EPosSigletonObjectIdPsyFixStateManager   = 1,
    EPosSigletonObjectIdPsyListHandlerId     = 2,
    EPosSigletonObjectConstManagerId         = 3, 
    EPosSigletonObjectExtGpsMonitorId        = 4
    };


/**
* The CPosSingletonManager manages all singleton objects of default proxy. 
* Each singleton object must have a unique ID. This ID is not allocated
* by this class. The singleton class itself must guarentee that the ID
* is unique within the DLL.
*
* Usage:
*
* @code
* @endcode
*/
class CPosSingletonManager : public CBase
    {
    public: // Constructors and destructor
              
        /**
        * Destructor.
        */
        ~CPosSingletonManager();

    public: // Functions from base classes
        /**
        * Get the singleton object. This function 
        * returns NULL if the object is not set before.
        *
        * @param aObjectId The ID of the singleton object.
        * @return The pointer to the singleton ojbect. NULL
        * if the object has not been set.
        */
        static CBase* GetObject( TInt aObjectId );
        
        /**
        * Set the singleton object. This function
        * panics in debug mode if the object has been set.
        *
        * @param aObject The pointer to the object. NULL will
        * clear the object.
        * @param aObjectId The id of the object. 
        */
        static void SetObjectL( 
            CBase* aObject,
            TInt aObjectId );
            
        /**
        * Release the singleton object. 
        *
        * @param aObjectId The id of the object. 
        */
        static void ReleaseObject(
            TInt aObjectId );
        
    private:
        /**
        * Get instance of singleton manager.
        */
        static CPosSingletonManager* GetInstanceL();
        
        /**
        * Get instance of the singleton manager. This function
        * returns NULL if the singleton manager has not be 
        * constructed.
        */
        static CPosSingletonManager* GetInstance();

        /**
        * C++ default constructor.
        */
        CPosSingletonManager();

        /**
        * Get a object
        */
        CBase* GetAObject( TInt aObjectId );
        
        /**
        * Release a object
        */
        void ReleaseAObject( TInt aObjectId );
        
        /**
        * Set a object
        */
        void SetAObjectL(
            CBase* aObject,
            TInt aObjectId );
        
        /**
        * Check if the singleton manager object shall be cleard.
        * When there is no sigleton object stored, then this object
        * shall be cleared
        */
        void ClearIfNeeded();

        // By default, prohibit copy constructor
        CPosSingletonManager(const CPosSingletonManager&);
        // Prohibit assigment operator
        CPosSingletonManager& operator= (const CPosSingletonManager&);

    private: // Data
        //Struct defination for singleton object
        struct TSingletonOb
            {
            CBase* iObject;
            TInt iObjectId;
            };
        
        //Objects array    
        RArray< TSingletonOb > iObjectsArray;
    };

#endif      // CPOSSINGLETONMANAGER_H  
            
// End of File
