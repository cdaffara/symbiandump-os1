/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Header definition of server plugin wrapper class implementig S60 custom interface.
*
*/


#ifndef CIEXTNSERVERPLUGINWRAPPER_H
#define CIEXTNSERVERPLUGINWRAPPER_H

#include <e32base.h>
#include <MCustomInterface.h>

// Forward declarations
class MCustomInterface;
class MyMCustomInterface;

class CIExtnServerPluginWrapper : public CBase, 
                                  public MCustomInterface
    {
    public:  // Constructors and destructor

        /**
        * Constructs, and returns a pointer to, a new CIExtnCustomInterfaceS60 object.
        * Leaves on failure..
        * @param MCustomInterface& aInterfaceRef A reference to MCustomInterface.
        * @return CIExtnCustomInterfaceS60* A pointer to newly created object.
        */
        static CIExtnServerPluginWrapper* NewL(MCustomInterface& aInterfaceRef);

        /**
        * Destructor.
        */
        ~CIExtnServerPluginWrapper();
        
    public: 
        /**
        Obtain a custom interface
        @param aInterfaceId  The UID of the required custom interface
        @return a pointer to the custom interface
         */
        TAny* CustomInterface(TUid aInterfaceId);
        
    public:
        MCustomInterface* GetInterface();
        void Release();
    private:

        /**
        * Default Constructor 
        */
        CIExtnServerPluginWrapper(MCustomInterface& aInterfaceRef);

        /**
        * Second phase constructor.
        */
        void ConstructL();

    private:
        MCustomInterface* iCustomInterfaceRef;
        MyMCustomInterface* iCustomInterfaceStub;

    private:
        TUid iDtor_ID_Key;
    };

#endif // CIEXTNSERVERPLUGINWRAPPER_H
