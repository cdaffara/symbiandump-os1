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
* Description:  Declares UI class for application.
*
*/


#ifndef __AUDIOCLIENTSTUBAPPUI_h__
#define __AUDIOCLIENTSTUBAPPUI_h__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CaudioclientstubAppView;
class CTspClientMapper;
class CAudioClientStubRemConObserver;

// CLASS DECLARATION
/**
 * CaudioclientstubAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CaudioclientstubAppUi : public CAknAppUi
    {
public:
    // Constructors and destructor

    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CaudioclientstubAppUi.
     * C++ default constructor. This needs to be public due to
     * the way the framework constructs the AppUi
     */
    CaudioclientstubAppUi();

    /**
     * ~CaudioclientstubAppUi.
     * Virtual Destructor.
     */
    virtual ~CaudioclientstubAppUi();

private:
    // Functions from base classes

    /**
     * From CEikAppUi, HandleCommandL.
     * Takes care of command handling.
     * @param aCommand Command to be handled.
     */
    void HandleCommandL(TInt aCommand);

    /**
     *  HandleStatusPaneSizeChange.
     *  Called by the framework when the application status pane
     *  size is changed.
     */
    void HandleStatusPaneSizeChange();

private:
    // Data

    /**
     * The application view
     * Owned by CaudioclientstubAppUi
     */
    CaudioclientstubAppView* iAppView;

    CTspClientMapper* iTspClientMapper;
    CAudioClientStubRemConObserver* iRemConObserver;
    };

#endif // __AUDIOCLIENTSTUBAPPUI_h__
// End of File
