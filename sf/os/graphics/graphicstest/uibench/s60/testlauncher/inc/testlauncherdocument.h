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

#ifndef __HELLOWORLDBASICDOCUMENT_H__
#define __HELLOWORLDBASICDOCUMENT_H__


#include <akndoc.h>


class CHelloWorldBasicAppUi;
class CEikApplication;


/**
* CHelloWorldBasicDocument application class.
* An instance of class CHelloWorldBasicDocument is the Document part of the
* AVKON application framework for the HelloWorldBasic example application.
*/
class CHelloWorldBasicDocument : public CAknDocument
    {
    public:
        /**
        * Construct a CHelloWorldBasicDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CHelloWorldBasicDocument.
        */
        static CHelloWorldBasicDocument* NewL(CEikApplication& aApp);

        /**
        * Construct a CHelloWorldBasicDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CHelloWorldBasicDocument.
        */
        static CHelloWorldBasicDocument* NewLC(CEikApplication& aApp);

        ~CHelloWorldBasicDocument();

    public:
        /**
        * From CEikDocument
        * Create a CHelloWorldBasicAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private:
        void ConstructL();
        CHelloWorldBasicDocument(CEikApplication& aApp);

    };

#endif // __HELLOWORLDBASICDOCUMENT_H__

// End of File

