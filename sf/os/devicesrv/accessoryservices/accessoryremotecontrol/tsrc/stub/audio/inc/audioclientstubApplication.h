/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares main application class.
*
*/


#ifndef __AUDIOCLIENTSTUBAPPLICATION_H__
#define __AUDIOCLIENTSTUBAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "audioclientstub.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidaudioclientstubApp =
    {
    _UID3
    };

// CLASS DECLARATION

/**
 * CaudioclientstubApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CaudioclientstubApplication is the application part of the
 * AVKON application framework for the audioclientstub example application.
 */
class CaudioclientstubApplication : public CAknApplication
    {
public:
    // Functions from base classes

    /**
     * From CApaApplication, AppDllUid.
     * @return Application's UID (KUidaudioclientstubApp).
     */
    TUid AppDllUid() const;

protected:
    // Functions from base classes

    /**
     * From CApaApplication, CreateDocumentL.
     * Creates CaudioclientstubDocument document object. The returned
     * pointer in not owned by the CaudioclientstubApplication object.
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();
    };

#endif // __AUDIOCLIENTSTUBAPPLICATION_H__
// End of File
