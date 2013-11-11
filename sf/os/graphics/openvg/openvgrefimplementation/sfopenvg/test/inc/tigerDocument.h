/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Tiger app document class
*/

#ifndef TIGERDOCUMENT_H
#define TIGERDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
 * Document class that is just used as the container for the application
 * (as required by the Symbian UI application architecture).
 */
class CTigerDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CTigerDocument object.
         */
        static CTigerDocument* NewL(CEikApplication& aApp);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CTigerDocument();

    private:  // Functions from base classes

        /**
         * C++ constructor. Just passes the given application reference to the baseclass.
         */
        CTigerDocument(CEikApplication& aApp);

        /**
         * Second phase constructor. Does nothing.
         */
        void ConstructL();

    private: // New functions

        /**
         * From CEikDocument, creates and returns CTigerAppUi application UI object.
         */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

