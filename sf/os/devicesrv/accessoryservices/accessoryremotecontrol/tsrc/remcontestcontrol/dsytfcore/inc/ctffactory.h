/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CTFFactory class
*
*/


#ifndef __CTFFACTORY_H__
#define __CTFFACTORY_H__

#include <e32base.h>
#include "ctfstubmoduleinterface.h"

class MTFStubControl;
class MTFStubTestCase;
class CTFATestSuite;
class CTFStubControl;
class CTFStub;
class CTFTestServer;

/**
* CTFFactory is a singleton class stored in thread-local storage of the test framework.
* The entry point calls BuildTestSuiteL when the test framework DLL is loaded.
* BuildTestSuiteL loads the plug-in modules and fetches the test cases from them.
*/
class CTFFactory : public CBase
    {
    public:
        /**
        * Creates a new CTFFactory
        */
        static CTFFactory* NewL( void );

        /**
        * Destructor
        */
        ~CTFFactory( void );
    
    private:
        /**
        * C++ constructor
        */
        CTFFactory( void );

        /**
        * Symbian constructor
        */
        void ConstructL( void );

        /**
        * Copy constructor is hidden
        */
        CTFFactory( const CTFFactory& aFactory );

        /**
        * Assignment operator is hidden
        */
        CTFFactory& operator=( const CTFFactory& aFactory );

    public:
        /**
        * Gets the stub that has the given plug-in module stub type identifier
        */
        IMPORT_C static CTFStub* Stub( TInt aType );

        /**
        * Calls a stub in remote process asynchronously.
        */
        IMPORT_C static void CallRemoteStubL( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4, TRequestStatus& aStatus );

        /**
        * Calls a stub in remote process synchronously.
        */
        IMPORT_C static void CallRemoteStubL( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4 );

        /**
        * Calls a stub in local process. This can be used to generate a RMessage object to the stub.
        */
        IMPORT_C static void CallLocalStubL( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4 );

    public:
        /**
        * Returns the stub controller interface.
        */
        IMPORT_C MTFStubControl* StubControl( void );

        /**
        * Loads the plug-in modules and uses them to create test cases.
        * @param aRootSuite The root test suite
        */
        void BuildTestSuiteL( CTFATestSuite* aRootSuite );

        /**
        * Gets the stub that has the given plug-in module stub type identifier
        */
        CTFStub* InternalStub( TInt aType );

    private:
        /**
        * Loads the plug-in modules and uses them to create test cases.
        */
        void BuildSuiteFromModulesL( CTFATestSuite* aRootSuite );

        /**
        * Initializes the test cases of given test suite. This is called
        * for each test suite obtained from plug-in modules.
        */
        void InitializeTestCasesL( CTFATestSuite* aSuite );

        /**
        * Thread function for the test server
        * @param aData Pointer to this factory
        * @return Symbian error code
        */
        static TInt ServerThreadFunction( TAny* aData );
        static void ServerThreadFunctionL( CTFFactory* aFactory );

        /**
        * Thread function for calls to the local the test server
        * @param aData Pointer to this factory
        * @return Symbian error code
        */
        static TInt LocalThreadFunction( TAny* aData );
        static void LocalThreadFunctionL( struct TTFThreadParam* aData );
    
    protected:
        CTFStubControl* iStubControl;
        RImplInfoPtrArray iPluginInfos ;
        RPointerArray<CTFStubModuleInterface> iPlugins;
        CTFTestServer* iTestServer;
    };

#endif
