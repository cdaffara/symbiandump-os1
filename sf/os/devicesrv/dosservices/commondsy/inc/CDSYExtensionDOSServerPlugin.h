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
* Description:  Declaration of CCDSYExtensionDOSServerPlugin class
*
*/


#ifndef CDSYEXTENSIONDOSSERVERPLUGIN_H
#define CDSYEXTENSIONDOSSERVERPLUGIN_H

// INCLUDES
#include <dosextensionbase.h>


// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CCDSYDOSServerRequestManager;


/**
*  CCDSYExtensionDOSServerPlugin
*  This class implements the CCDSYExtensionDOSServerPlugin
*/
NONSHARABLE_CLASS( CCDSYExtensionDOSServerPlugin ): public CDosExtensionBase
    {
    public:                 // Constructor and destructor
        /**
        * Destructor.
        */
        ~CCDSYExtensionDOSServerPlugin();

        /**
        * Creates a new extension plug-in.
        * @param aCDSYDOSServerRequestManager   The request manager
        * @return                               The new plug-in
        */
        static CCDSYExtensionDOSServerPlugin* NewL( CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager );
        
    protected:              // From Dos plug-in API
        /**
        * Implements a raw function call. The implementation interprets the parameters and satisfies the requests.
        * @param aFunc                  A unique integer among the extended functions identifying the called function.
        * @param aParameter             A pointer to the function parameter.
        * @param aParLength             The parameter length.
        * @param aParameterModified     ETrue if the contents of aParameter have changed, otherwise EFalse. 
        * @return                       Symbian error code.
        */
        TInt CallFunctionL( TInt aFunc, TAny *aParameter, TInt aParLength, TBool& aParameterModified );

        /**
        * This is the asynchronous version of CallFunction.
        * @param aFunc                  A unique integer among the extended functions identifying the called function.
        * @param aParameter             A pointer to the function parameter.
        * @param aParLength             The parameter length.
        * @param aMessage               The message to be completed. Has to be completed by the implementation of the function.
        * @return                       void 
        */
        void CallFunctionAndCompleteL( TInt aFunc, TAny *aParameter, TInt aParLength, const RMessage2& aMessage );

    private:                // Internal functions
        /**
        * Constructor.
        * @param aCDSYDOSServerRequestManager The request manager
        */
        CCDSYExtensionDOSServerPlugin( CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager );

        /**
        * 2nd phase constructor.
        * @return                       void 
        */
        void ConstructL();

    private:                // Data
        // The request manager is used to send messages to service logic
        CCDSYDOSServerRequestManager& iCDSYDOSServerRequestManager;

        // The extension interface where function calls are delegated to
        MDosExtensionBaseDSY* iDosExtensionBaseDSY;
    };



#endif //#ifndef CDSYEXTENSIONDOSSERVERPLUGIN_H



//  End of File
