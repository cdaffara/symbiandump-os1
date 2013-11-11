/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message handler for RA8 decoder custom interface.
*
*/


#ifndef CRA8CUSTOMINTERFACEMSGHDLR_H
#define CRA8CUSTOMINTERFACEMSGHDLR_H

// INCLUDES
#include <mmfcontroller.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CRa8CustomInterface;

// CLASS DECLARATION

/**
*  Handle messages from interface proxy.
*  The class receives the messages sent by the custom interface proxy
*  and makes the appropriate call on its custom interface implementation.
*
*  @lib Ra8CustomInterfaceMsgHdlr.lib
*  @since Series 60 3.0
*/
class CRa8CustomInterfaceMsgHdlr: public CMMFObject
	{
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CRa8CustomInterfaceMsgHdlr* NewL(
                                           TAny* aRa8CustomInterface);
        
        /**
        * Destructor.
        */
		IMPORT_C virtual ~CRa8CustomInterfaceMsgHdlr();

    public: // New functions
        
    public: // Functions from base classes
		IMPORT_C virtual void HandleRequest(TMMFMessage& aMessage);        
		
    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
		CRa8CustomInterfaceMsgHdlr(
		                  CRa8CustomInterface* aRa8CustomInterface);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

		void DoHandleRequestL(TMMFMessage& aMessage);

		void DoFrameNumberL(TMMFMessage& aMessage);

		void DoSetInitStringL(TMMFMessage& aMessage);

    private:    // Data
        // pointer to the actual custom interface implementation
        CRa8CustomInterface* iRa8CustomInterface;

	};

#endif      // CRA8CUSTOMINTERFACEMSGHDLR_H
            
// End of File
