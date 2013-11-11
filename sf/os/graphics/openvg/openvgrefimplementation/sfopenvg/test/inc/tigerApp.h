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
* Tiger app main class declaration
*/

#ifndef TIGERAPP_H
#define TIGERAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
/** UID of the application. */
const TUid KUidTiger = { 0xA000024A };

// CLASS DECLARATION

/**
 * Application class. Provides factory method to create a concrete document object.
 */
class CTigerApp : public CAknApplication
    {
    private: // Functions from base classes

        /**
         * From CApaApplication, creates and returns CTigerDocument document object.
         * @return Pointer to the created document object.
         */
        CApaDocument* CreateDocumentL();

        /**
         * From CApaApplication, returns application's UID (KUidTiger).
         * @return Value of KUidTiger.
         */
        TUid AppDllUid() const;
    };

// OTHER EXPORTED FUNCTIONS

/**
 * Factory method used by the E32Main method to create a new application instance.
 */
LOCAL_C CApaApplication* NewApplication();

/**
 * Entry point to the EXE application. Creates new application instance and
 * runs it by giving it as parameter to EikStart::RunApplication() method.
 */
GLDEF_C TInt E32Main();

#endif

// End of File
