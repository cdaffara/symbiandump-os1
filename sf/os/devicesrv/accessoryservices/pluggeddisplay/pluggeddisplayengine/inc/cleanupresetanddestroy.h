/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// NAMESCAPE DEFINITION
namespace
    {
    // CLASS DECLARATION
    template <class T> class CleanupResetAndDestroy
    	{
        public:
            
            // Push cleanup item into cleanup stack
    	    inline static void PushL(T& aRef)
    	        {
    	        CleanupStack::PushL(
    	            TCleanupItem( &ResetAndDestroy, &aRef ) );
    	        };
        
        private:
        
            // Cleanup item action
    	    static void ResetAndDestroy(TAny *aPtr)
        	    {
        	    static_cast<T*>( aPtr )->ResetAndDestroy();
        	    };
    	};
    
    // Local function which pushes RPointerArray<T&> instance into cleanup stack
    template <class T> inline void CleanupResetAndDestroyPushL( T& aRef )
        {
            CleanupResetAndDestroy<T>::PushL( aRef );
        };
    }
    