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



#ifndef CTSYDELEGATES_H
#define CTSYDELEGATES_H

// INCLUDES
#include <e32base.h>
#include "cmmmessagemanagerbase.h"

// CLASS DECLARATION

/**
 *  Class defining of TSY delegates. 
 */
class CTsyDelegates : public CBase
    {
    public:

        /**
         * Destructor.
         */
        virtual ~CTsyDelegates();

        /**
         * Creates object instance.
         *          
         * 
         * @return pointer to tsydelegates object
         */
        static CTsyDelegates* NewL();
         
        /**
         * Register Tsy object into use
         *          
         *
         * @param aTsyObjects Tsy object to be registered
         * @param aTsyObject pointer to regitered Tsy object
         */
        void RegisterTsyObject( 
            CMmMessageManagerBase::TTsyObjects aTsyObjects,
            CBase* aTsyObject );
         
        /**
         * De registers Tsy object 
         *          
         * 
         * @param aTsyObject pointer to regitered Tsy object
         */
        void DeregisterTsyObject( CBase* aTsyObject );
         
        /**
         * Returns specified Tsy object
         *          
         *
         * @param aObject Tsy object type
         * @return pointer to requested Tsy object
         */ 
        CBase* GetTsyObject( CMmMessageManagerBase::TTsyObjects aObject );
         
    private:
    
        /**
         * C++ default constructor.
         */
        CTsyDelegates();

    private: // data

		/** 
         * array with pointers to Tsy objects for completion
         */
        TFixedArray<CBase*, 
            CMmMessageManagerBase::EMaxNumOfTsyObjects> iTsyObjectArray;

    };

#endif  // CTSYDELEGATES_H

//  End of file
