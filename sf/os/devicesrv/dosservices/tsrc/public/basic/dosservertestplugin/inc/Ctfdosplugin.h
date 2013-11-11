/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Definition of CTFDosServerPlugin
*
*/


#ifndef __CTFDOSPLUGIN_H__
#define __CTFDOSPLUGIN_H__

// INCLUDE FILES
#include <ctfstubmoduleinterface.h>

// CLASS DECLARATION

/**
*  An implementation of CTFStubModuleInterface.
*/
class CTFDosPlugin : public CTFStubModuleInterface
	{
	public:		// Constructors and destructor

		/**
        * Two-phased constructor.
        * @param aInitParams Initialisation parameters.
		* @return The created object.
        */
		static CTFDosPlugin* NewL( TAny* aInitParams );

		/**
		* Destructor.
		*/
		~CTFDosPlugin( void );

	public:		// Functions from base classes

        /**
        * Gets the stub implementation of this plug-in module
        */
        CTFStub* GetStubL( void );

        /**
        * Builds the test suite of this plug-in module.
        */
        void BuildTestSuiteL( CTFATestSuite* aRootSuite );

        /**
        * Gets the plug-in module specific part of a test case which has the given test case ID
        */
        MTFStubTestCase* GetStubTestCaseL( TInt aTestCaseId );

	private:	// Functions

		/**
        * C++ default constructor.
        * @param aParams Initialisation parameters.
        */
		CTFDosPlugin( TAny* aParams );

	private:	// Data
		
		CTFStubModuleInterface::TInterfaceInitParams* iInitParams;

	};  

#endif	//__CTFDOSSERVERPLUGIN_H__

// End of File
