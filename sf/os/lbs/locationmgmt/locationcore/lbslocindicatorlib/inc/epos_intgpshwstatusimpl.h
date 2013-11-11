/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSPOSITIONINGSTATUSIMPL_H
#define CPOSPOSITIONINGSTATUSIMPL_H

#include <e32base.h>
#include <e32property.h>
#include <lbs/epos_intgpshwstatus.h>
/**
 @file epos_intgpshwstatusimpl.h
 @released
 @internalComponent
 */
NONSHARABLE_CLASS( CPosIntGpsHwStatusImpl ) : public CActive
    {
public:
    static CPosIntGpsHwStatusImpl* NewL();
    static CPosIntGpsHwStatusImpl* NewL(MPosIntGpsHwStatusObserver& aObserver);
    static CPosIntGpsHwStatusImpl* NewLC();
    static CPosIntGpsHwStatusImpl* NewLC(MPosIntGpsHwStatusObserver& aObserver);
    virtual ~CPosIntGpsHwStatusImpl();
    void SetStatusL( CPosIntGpsHwStatus::TIntGpsHwStatus aStatus );
    TInt GetStatus(CPosIntGpsHwStatus::TIntGpsHwStatus& aStatus);

private:
    CPosIntGpsHwStatusImpl();
    CPosIntGpsHwStatusImpl( const CPosIntGpsHwStatusImpl& );
    CPosIntGpsHwStatusImpl& operator=( const CPosIntGpsHwStatusImpl& );
    void ConstructL();
    void ConstructL(MPosIntGpsHwStatusObserver& aObserver);
    
    void CreatePSKeyL();

    // From CActive
    virtual void RunL();
    virtual void DoCancel();
    
private: 
	// Hardware status Property
    RProperty iProperty;
    
    // Pointer to status observer - NOT OWNED
    MPosIntGpsHwStatusObserver* iObserver;
    
    // Used to determine if this instance has just updated the hardware status, 
    // if it has the next notification is consumed
    CPosIntGpsHwStatus::TIntGpsHwStatus iLastSet;
    
    
    //TBool iLastSet;
    };


#endif // CPOSPOSITIONINGSTATUSIMPL_H
