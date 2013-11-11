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
* Description:
*
*/


#ifndef __MREMCONCONTROL_H__
#define __MREMCONCONTROL_H__

#include <e32base.h>
#include "remcontypes.h"

class MRemConControl
    {
public:
    MRemConControl( void );
    virtual ~MRemConControl( void );
    
private:
    MRemConControl( const MRemConControl& aStub );
    MRemConControl& operator=( const MRemConControl& aStub );
    
public:
    
    virtual void NotifyKeyEvent( TInt aEvent, TInt aParameter ) = 0;
    
    virtual void CompleteKeyEventResponse ( TInt aEvent, TInt aParam, TInt aError) = 0; 
    
    };

MRemConControl* GetRemConControl( void );

#endif //__MREMCONCONTROL_H__
