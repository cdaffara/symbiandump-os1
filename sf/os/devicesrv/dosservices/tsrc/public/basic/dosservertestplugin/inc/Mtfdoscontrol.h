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
*     Definition of MTFDosServerControl
*
*/


#ifndef __MTFDOSCONTROL_H__
#define __MTFDOSCONTROL_H__

#include <e32base.h>

struct TTFDosServerControlTestCaseState;

class MTFDosControl
    {
public:
    MTFDosControl( void );
    virtual ~MTFDosControl( void );
    
private:
    MTFDosControl( const MTFDosControl& aStub );
    MTFDosControl& operator=( const MTFDosControl& aStub );
    
public:
    virtual void CallDosFunctionL( TTFDosServerControlTestCaseState& aParameter ) = 0;
    virtual void NotifyDosEvent( TInt aEvent, TInt aParameter ) = 0;
    };

MTFDosControl* GetDosServerControl( void );

#endif
