/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



#ifndef CACCDBIMPORTER_H
#define CACCDBIMPORTER_H

//  INCLUDES
#include <e32base.h>
#include <d32dbms.h> // RDbStoreDatabase
#include <f32file.h> // RFs
#include <s32file.h> // CFileStore

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  This class is responsible for providing the 
*  functions for importing the data to database.
*
*  @lib
*  @since S60 S60_version
*/
class CAccDBImporter : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CAccDBImporter* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CAccDBImporter();

    public:     // New functions

        /**
        * Runs the import functions
        * @since S60 S60_version
        * @param void
        * @return void
        */
        void DoImportL();
            
    public:     // Functions from base classes
    
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccDBImporter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Create DB tables.
        */
        void CreateTablesL();
        
        /**
        * Create DB tables.
        */
        void DoFindDatsL();
        
        /**
        * Import GenericID Integer data to DB.
        */
        void ImportGIDIntsL();
        
        /**
        * Import Capability Rules data to DB.
        */
        void ImportCapRulesL();

    public:     // Data
      
    protected:  // Data
  
    private:    // Data
         
        // For use of iFileStore
        RFs iFsSession;
    
        TBuf<100> iFileName;
        
        // For database operations
        RDbNamedDatabase iDb;
    
        // Database session
        RDbs iDbServer;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#endif      // CACCDBIMPORTER_H   
            
// End of File
