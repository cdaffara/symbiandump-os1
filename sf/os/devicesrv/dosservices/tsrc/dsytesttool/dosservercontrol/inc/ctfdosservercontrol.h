/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CTFDosServerControl class
*
*/


#ifndef __CTFDOSSERVERCONTROL_H__
#define __CTFDOSSERVERCONTROL_H__

#include <DosSvrServices.h>
#include "ctfstub.h"
#include "mtfdosservercontrol.h"
#include "tfdosservertypes.h"

struct TTFDosServerControlTestCaseState;
class CTFDosServerEventListener;
class RTFDosExtension;

const TInt KTFStubTypeDosServerControl = 560;

NONSHARABLE_CLASS( CTFDosServerControl ): public CTFStub, public MTFDosServerControl
    {
    public:
        /**
        * Constructor
        */
        CTFDosServerControl( void );

        /**
        * Destructor
        */
        virtual ~CTFDosServerControl( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        CTFDosServerControl( const CTFDosServerControl& aStub );

        /**
        * Assignment operator is hidden
        */
        CTFDosServerControl& operator=( const CTFDosServerControl& aStub );
    
    public:
        /**
        * Initializes this stub
        */
        void InitializeL( void );
    
    public:
        /**
        * Calls a DosServer function
        */
        void CallDosFunctionL( TTFDosServerControlTestCaseState& aParameter );

        /**
        * Gets the parameter count for given DosServer method
        */
        TInt ParameterCount( TTFDosFunction aFunction ) const;

        /**
        * Gets the parameter bounds of given DosServer method
        */
        TBool ParameterBounds( TTFDosFunction aFunction, TInt* aMin, TInt* aMax ) const;

        /**
        * Notifies about a DosServer event
        */
        void NotifyDosEvent( TInt aEvent, TInt aParameter );

        /**
        * Closes all DosServer services
        */
        void ResetDosServices( void );
    
    private:
        RDosServer iDosServer;
#ifndef __ACCESSORY_FW
        RDosAudio* iDosAudio;
        RDosAccessory* iDosAccessory;
#endif //__ACCESSORY_FW
#ifndef RD_STARTUP_CHANGE
        RDosSysUtils* iDosSysUtils;
        RDosHelper* iDosHelper;
        RDosMtc* iDosMtc;
        RDosSelfTest* iDosSelfTest;
#endif //RD_STARTUP_CHANGE 
        RDosSae* iDosSae;
#ifndef __ACCESSORY_FW 
        RDosBTAudio* iDosBTAudio;
#endif //__ACCESSORY_FW
        RTFDosExtension* iDosExtension;
        CTFDosServerEventListener* iEventListener;
        TBool iInitialized;
    };

class RTFDosExtension : public RDosExtension
    {
    public:
        /**
        * Calls a synchronous RDosExtension::CallFunction
        */
        TInt CallSyncDosExtFunction( const TInt& aFunc, TAny* aParam, TInt aParLength, TBool aAutoComplete );

        /**
        * Calls an asynchronous RDosExtension::CallFunction
        */
        TInt CallAsyncDosExtFunction( TRequestStatus& aStatus, TInt aFunc, TAny* aParam, TInt aParLength, TBool aAutoComplete );
    };

#endif
