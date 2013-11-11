/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observer class for remote control keys
*
*/


#ifndef AUDIOCLIENTSTUBREMCONOBSERVER_H
#define AUDIOCLIENTSTUBREMCONOBSERVER_H

//  INCLUDES
#include <remconcoreapitargetobserver.h>

// FORWARD DECLARATIONS
class CRemConInterfaceSelector;  
class CRemConCoreApiTarget;


// CLASS DECLARATION

/**
*/
class CAudioClientStubRemConObserver : public CBase, public MRemConCoreApiTargetObserver
    {
    public:  // Constructor and destructor
        
        /**
        * Two-phased constructor.
        */
        static CAudioClientStubRemConObserver* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CAudioClientStubRemConObserver();

    protected: // Functions from base classes

        /**
        *
        * A command has been received. 
        * @param aOperationId The operation ID of the command.
        * @param aButtonAct The button action associated with the command.
        */
        void MrccatoCommand( TRemConCoreApiOperationId aOperationId, 
                             TRemConCoreApiButtonAction aButtonAct);


    private: // Functions
        /**
        * C++ default constructor.
        * Not accessible. 
        */
        CAudioClientStubRemConObserver();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        
    private: // Data

		/**
		* Remote Controller interface selector.
		* Own.
		*/
        CRemConInterfaceSelector* iInterfaceSelector;  
        
        /** 
        * Remote Controller target. 
        * Not own.
        */
        CRemConCoreApiTarget* iCoreTarget;

    };

#endif // 

// End of File
