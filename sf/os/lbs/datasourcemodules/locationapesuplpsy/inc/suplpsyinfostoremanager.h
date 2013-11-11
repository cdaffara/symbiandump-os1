/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of CSuplPsyInfoStoreManager.
*
*/

#ifndef SUPLPSYINFOSTOREMANAGER_H
#define SUPLPSYINFOSTOREMANAGER_H

// INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include <f32file.h>
#include <lbs.h>

#include "suplpsycellidinfo.h"


/**
*  Class for performing Cenrep & file store operations
*  @lib nlacidsuplpsy.dll
*  @since S60 v3.1
*/

class CSuplPsyInfoStoreManager : public CBase
    {
   
    public:  // Constructors and destructor
        
   	   /**
        * Two phase constructor
        * @since S60  v3.1
        */
   		static CSuplPsyInfoStoreManager* NewL();
   		
       /**
        * Destructor
        * @since S60  v3.1
        */
        ~CSuplPsyInfoStoreManager();
        
       /**
        * Reads stored Location Information from file.
        * @since S60  v3.1
        */
        TInt ReadSuplPsyLocationInfo( HPositionGenericInfo& aPosInfo );
        
       /**
        * Writes Location Information to file.
        * @since S60  v3.1
        */
        TInt WriteSuplPsyLocationInfo( const HPositionGenericInfo& aPosInfo );
        
       /**
        * Reads stored Cell Id Information from SUPL PSY CR Key.
        * @since S60  v3.1
        */
        TInt ReadSuplPsyCellIdInfo( TCellIdInfo& aCellIdInfo );
        
       /**
        * Writes Cell Id Information to SUPL PSY CR Key.
        * @since S60  v3.1
        */
        TInt WriteSuplPsyCellIdInfo( const TCellIdInfo& aCellIdInfo );
       
       
       /**
        * Reads Cell Id Information from SUPL FW CR key.
        * @since S60  v3.1
        */
        TInt ReadSuplFwCellIdInfo( TCellIdInfo& aCellIdInfo );
        	     	
     	
     private:
     
       /**
        * Constructor
        * @since S60  v3.1
        */
        CSuplPsyInfoStoreManager();

       /**
        * 2nd phase constructor
        */
        void ConstructL();

	     
    private:    // Data
  
      	CRepository* iRepoSuplPsy;
      	CRepository* iRepoSuplFw;
      	
      	RFs             iFs;
        RFile           iFile;
    };

#endif      // SUPLPSYINFOSTOREMANAGER_H
            
// End of File
