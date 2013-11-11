 // Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 // All rights reserved.
 // This component and the accompanying materials are made available
 // under the terms of "Eclipse Public License v1.0"
 // which accompanies this distribution, and is available
 // at the URL "http://www.eclipse.org/legal/epl-v10.html".
 //
 // Initial Contributors:
 // Nokia Corporation - initial contribution.
 //
 // Contributors:
 //
 // Description:
 //

#ifndef __HELLOWORLDBASICAPPLICATION_H__
#define __HELLOWORLDBASICAPPLICATION_H__


#include <aknapp.h>

/**
* CHelloWorldBasicApplication application class.
* Provides factory to create concrete document object.
* An instance of CHelloWorldBasicApplication is the application part of the
* AVKON application framework for the HelloWorldBasic example application.
*/
class CHelloWorldBasicApplication : public CAknApplication
    {
    public:
        /**
        * From CApaApplication
        * @return Application's UID (KUidHelloWorldBasicApp).
        * TUid AppDllUid() const;
        */
        TUid AppDllUid() const;

    protected:
        /**
        * From CApaApplication
        * Creates CHelloWorldBasicDocument document object. The returned
        * pointer in not owned by the CHelloWorldBasicApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __HELLOWORLDBASICAPPLICATION_H__
