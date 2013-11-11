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




/**
 @file
 @publishedPartner
 @released
*/
#ifndef MLTSYFACTORYBASE_H
#define MLTSYFACTORYBASE_H

//  INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
*  Base class for all LicenseeTSY factory interfaces
*/
class MLtsyFactoryBase
    {
    public:
    
	    /**
    	 * Defines the version number of the interface used by the LTSY.          
    	 */
	    enum TCtsyInterfaceVersion 
			{ 
			EIfVersion1 = 1, 
			EIfVersion2, 
			EIfVersion3, 
			EIfVersion4, 
			}; 
		
    public:
		
		/**
    	 * For getting interface version used in LTSY          
    	 *
    	 *
         * @return TCtsyInterfaceVersion version
    	 */	
		virtual TCtsyInterfaceVersion Version() = 0; 
		
		/**
		 * For releasing created LTSY object
		 *
		 *
		 * @return none
		 */ 
		virtual void Release() = 0 ;          
		
    }; // MLtsyFactoryBase


/**
* Library entry function to LTSY
*
*
* @return MLtsyFactoryBase pointer to LicenseeTsy
*/ 
IMPORT_C MLtsyFactoryBase* LTsyFactoryL();

#endif // MLTSYFACTORYBASE_H

//  End of file
