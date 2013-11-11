/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Definition of CTFDosControl
*
*/


#ifndef __CTFDOSCONTROL_H__
#define __CTFDOSCONTROL_H__

#include <DosSvrServices.h>
#include "dosclishareddata.h"
#include <ctfstub.h>
#include "mtfdoscontrol.h"
#include "tfdostypes.h"
#include <f32file.h>
#include <e32property.h>
class RTFDosExtension;

struct TTFDosControlTestCaseState;
class CTFEventListener;
class CSdDisk;

const TInt KTFStubTypeDosServerControl = 560;

class CTFDosControl : public CTFStub, public MTFDosControl
    {
public:
    CTFDosControl( void );
    virtual ~CTFDosControl( void );
    
private:
    CTFDosControl( const CTFDosControl& aStub );
    CTFDosControl& operator=( const CTFDosControl& aStub );
    
public:
    void InitializeL( void );
    
public:
    void CallDosFunctionL( TTFDosServerControlTestCaseState& aParameter );
    TInt ParameterCount( TTFDosFunction aFunction ) const;
    TBool ParameterBounds( TTFDosFunction aFunction, TInt* aMin, TInt* aMax ) const;
    void NotifyDosEvent( TInt aEvent, TInt aParameter );

private:
    void ResetDosServices( void );

    
private:
    RDosServer iDosServer;
    
    RDosSharedData* iDosSharedData;
   	CSdDisk* iSdDisk;
    
    RTFDosExtension* iDosExtension;
    CTFEventListener* iEventListener;
    TBool iInitialized;
    TReal64 iFreeDiskSpace;

    RFs iFs;
	RFile iFile;
	
	TInt iIndex;
    
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
