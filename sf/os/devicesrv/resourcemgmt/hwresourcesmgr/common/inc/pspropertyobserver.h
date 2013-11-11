// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology 
*/


#ifndef PSPOPERTYOBSERVER_H
#define PSPOPERTYOBSERVER_H

#include <e32base.h>
#include <e32property.h>


/**
* Callback interface for property change notification.
*/
class MPsPropertyObserver
    {
public:
    /**
    * Notifies changes in the property value
    *
    * @param aKey key for which value changed.
    * @param aValue new value.
    * @param aError KErrNone if successful. otherwise on of system-wide error.
    */
    virtual void PsPropertyChanged(TInt aKey, TInt aValue, TInt aError) =0;
    };

/**
*  This class observes changes in PS-Property value.
*
*/
NONSHARABLE_CLASS(CPsPropertyObserver) : public CActive
    {
public:
    IMPORT_C static CPsPropertyObserver* NewL(const TUid& aGroup, TInt aKey, MPsPropertyObserver& aObserver);
    IMPORT_C static TInt Get(const TUid& aGroup, TInt aKey, TInt& value);
    IMPORT_C TInt Get(TInt& value);
    void Start();
    ~CPsPropertyObserver();
        
private:
    CPsPropertyObserver(TUid aGroup, TInt aKey, MPsPropertyObserver& aObserver);
    void ConstructL(const TUid& aGroup);
    
protected:
    void RunL();
    void DoCancel();
    
private:
    RProperty iProperty;
    TUid iGroup;
    TInt iKey;
    TInt iValue;
    MPsPropertyObserver& iObserver;
    };

#endif
