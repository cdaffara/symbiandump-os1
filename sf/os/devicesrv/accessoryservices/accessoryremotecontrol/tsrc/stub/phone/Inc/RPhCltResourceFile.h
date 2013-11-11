/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It is phoneclient resource file loader.
*
*/


#ifndef RPHCLTRESOURCEFILE_H
#define RPHCLTRESOURCEFILE_H

//  INCLUDES
#include    <e32base.h>


// FORWARD DECLARATIONS
class CCoeEnv;
class RConeResourceLoader;


// CLASS DECLARATION

/**
*  It is PhoneClient resource file loader.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class RPhCltResourceFile
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        */
        IMPORT_C RPhCltResourceFile();
              

    public: // New functions

        /**
        * Opens resource file.
        *
        * Note: Panics if you try to reopen.
        *
        * @param aEnv Coe env.
        */
        IMPORT_C void OpenL( CCoeEnv& aEnv );

        /**
        * Closes resource file.
        */
        IMPORT_C void Close();


    public: // Functions from base classes
        
        // Prohibit copy constructor if not deriving from CBase.
        RPhCltResourceFile( const RPhCltResourceFile& );
        // Prohibit assigment operator if not deriving from CBase.
        RPhCltResourceFile& operator= ( const RPhCltResourceFile& );


    private:    // Data
        
        // Resource file loader.
        RConeResourceLoader* iLoader;

    };

#endif      // RPHCLTRESOURCEFILE_H
            
// End of File
