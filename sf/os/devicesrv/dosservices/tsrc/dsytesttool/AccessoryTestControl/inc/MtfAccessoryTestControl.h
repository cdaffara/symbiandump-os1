/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


#ifndef __MTFACCESSORYTESTCONTROL_H__
#define __MTFACCESSORYTESTCONTROL_H__

#include <e32base.h>

struct TTFAccessoryTestCaseStateControl;

class MTFAccessoryTestControl
    {
public:
    MTFAccessoryTestControl( void );
    virtual ~MTFAccessoryTestControl( void );
    
private:
    MTFAccessoryTestControl( const MTFAccessoryTestControl& aStub );
    MTFAccessoryTestControl& operator=( const MTFAccessoryTestControl& aStub );
    
public:
    virtual void CallAccessoryFunctionL( TTFAccessoryTestCaseStateControl& aParameter ) = 0;        
    
    virtual void CancelOutStandingRequest() = 0;        
    
    virtual void ResetAccessoryServer() = 0;
    
    };

MTFAccessoryTestControl* GetAccessoryTestControl( void );

#endif
