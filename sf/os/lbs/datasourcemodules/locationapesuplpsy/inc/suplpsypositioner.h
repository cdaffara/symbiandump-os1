/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SUPL PSY Positioner header file
*
*/



#ifndef SUPLPSYPOSITIONER_H
#define SUPLPSYPOSITIONER_H

//  INCLUDES
#include <lbs/epos_cpositioner.h> // CPositioner
#include "suplpsyrequestcompletelistener.h"

// FORWARD DECLARATIONS
class CSuplPsyRequestManager;

// CLASS DECLARATION

/**
*  This class implements S60 Positioning Plug-in API for SUPL PSY
*  
*  This class provides the interface to Location Framework as specified by
*  Positioning Plug-in API. Each client connection to SUPL PSY makes an 
*  instance of this class.
*
*  @lib nlaapesuplpsy.dll
*  @since S60 3.1
*/
class CSuplPsyPositioner : public CPositioner, private MSuplPsyRequestCompleteListener
    {
    public:  // Constructors and destructor
        
        /**
        * Two - phased constructor.
        */
        static CSuplPsyPositioner* NewL( TAny* aConstructionParameters );
        
        /**
        * Destructor.
        */
        ~CSuplPsyPositioner();
        

    public: // Functions from base classes

        /**
        * From CPositioner See Positioning Plug-in API
        * @since S60 3.1
        */
        void NotifyPositionUpdate( 
            TPositionInfoBase& aPosInfo,  
            TRequestStatus& aStatus ); 


        /**
        * From CPositioner See Positioning Plug-in API.
        * @since S60 3.1
        */
        void CancelNotifyPositionUpdate();
        

    private: //From base class
        /**
        * From MSuplPsyRequestCompleteListener
        */
        TBool RequestComplete( 
            TInt aErr,
            const HPositionGenericInfo& aPosInfo );

    private:

        /**
        * C++default constructor.
        */
        CSuplPsyPositioner();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TAny* aConstructionParameters );

        /**
        * Clear position info data
        * @return KErrNotSupport if the class tyep is not supported
        */
        TInt ClearPositionInfoData( 
            TPositionInfoBase& aPosInfo );

        /**
        * Fill position info data
        * @return KErrPositionBufferOverflow if there is not enough buffer allocated
        */
        TInt FillPositionInfoData( 
            const HPositionGenericInfo& aSrcPosInfo,
            TPositionInfoBase& aDesPosInfo );
            
    private:    // Data
        // Pointer to SUPL PSY singleton component
        CSuplPsyRequestManager* iRequestManager;
        
        // Pointer to request status
        TRequestStatus* iRequestStatus;
        
        // Pointer to position info
        TPositionInfoBase* iPositionInfo;

        // Allowed accuracy
        TInt        iAllowedAccr;
    };

#endif      // SUPLPSYPOSITIONER_H   
            
// End of File
