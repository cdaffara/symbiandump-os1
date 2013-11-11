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
* Description:  Declares document class for application.
*
*/


#ifndef __AUDIOCLIENTSTUBDOCUMENT_h__
#define __AUDIOCLIENTSTUBDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CaudioclientstubAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CaudioclientstubDocument application class.
 * An instance of class CaudioclientstubDocument is the Document part of the
 * AVKON application framework for the audioclientstub example application.
 */
class CaudioclientstubDocument : public CAknDocument
    {
public:
    // Constructors and destructor

    /**
     * NewL.
     * Two-phased constructor.
     * Construct a CaudioclientstubDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CaudioclientstubDocument.
     */
    static CaudioclientstubDocument* NewL(CEikApplication& aApp);

    /**
     * NewLC.
     * Two-phased constructor.
     * Construct a CaudioclientstubDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CaudioclientstubDocument.
     */
    static CaudioclientstubDocument* NewLC(CEikApplication& aApp);

    /**
     * ~CaudioclientstubDocument
     * Virtual Destructor.
     */
    virtual ~CaudioclientstubDocument();

public:
    // Functions from base classes

    /**
     * CreateAppUiL
     * From CEikDocument, CreateAppUiL.
     * Create a CaudioclientstubAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CaudioclientstubDocument.
     * C++ default constructor.
     * @param aApp Application creating this document.
     */
    CaudioclientstubDocument(CEikApplication& aApp);

    };

#endif // __AUDIOCLIENTSTUBDOCUMENT_h__
// End of File
