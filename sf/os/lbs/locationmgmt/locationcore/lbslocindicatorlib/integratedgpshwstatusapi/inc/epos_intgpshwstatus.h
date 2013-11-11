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



#ifndef CPOSPOSITIONINGSTATUS_H
#define CPOSPOSITIONINGSTATUS_H

#include <e32base.h>
#include <e32property.h>

//Forward declaration 
class CPosIntGpsHwStatusImpl;
class MPosIntGpsHwStatusObserver;


/**
 The CPosIntGpsHwStatus class is used for updating and checking the current status of integrated GPS HW.
 
 Users who wish to update the HW status and do not wish to receive callbacks should instantiate the interface with the NewL or NewLC methods.
 Doing so will require WriteDeviceData.
 Updating HW status is usually performed by the HW integration layer.
 
 Users who wish to be notified of changes to the HW status should use the overloaded NewL or NewLC functions that take MPosIntGpsHwStatusObserver as an argument.
 Doing so will not require any capabilities.
 Checking HW status is usually performed by the UI, and applications.
 
 Multiple applications may be notified of HW status updates but only one should change the HW status. 
 
 @publishedPartner
 @released
 @see MPosIntGpsHwStatusObserver
 */
class CPosIntGpsHwStatus
    {
public:
    /** Integrated GPS HW states */
    enum TIntGpsHwStatus
        {
        /** GPS HW state is unknown. */
        EStatusUnknown = 0,
        /** GPS HW is off. */
        EStatusOff,
        /** GPS HW is powered. */
        EStatusOn,
        /** GPS HW is used and is receiving location fixes. */
        EStatusActive
        };

public:
    IMPORT_C static CPosIntGpsHwStatus* NewL();
    IMPORT_C static CPosIntGpsHwStatus* NewL(MPosIntGpsHwStatusObserver& aObserver);
    IMPORT_C static CPosIntGpsHwStatus* NewLC();
    IMPORT_C static CPosIntGpsHwStatus* NewLC(MPosIntGpsHwStatusObserver& aObserver);
    virtual ~CPosIntGpsHwStatus();
    
    IMPORT_C void SetStatusL(TIntGpsHwStatus aStatus);
    IMPORT_C void GetStatusL(TIntGpsHwStatus& aStatus);

private:
    //C++ constructor
    CPosIntGpsHwStatus();

    //Prevent this class being derived
    CPosIntGpsHwStatus( const CPosIntGpsHwStatus& );
    CPosIntGpsHwStatus& operator=( const CPosIntGpsHwStatus& );
    
    //Second phase constructor
    void ConstructL();
    
    //Second phase constructor
    void ConstructL(MPosIntGpsHwStatusObserver& aObserver);
    
private: // data
    CPosIntGpsHwStatusImpl* iImpl;
    };

    
/**
The MPosIntGpsHwStatusObserver class provides a call-back type of interface that must be
used together with the CPosIntGpsHwStatus class. A class using the CPosIntGpsHwStatus interface must 
derive from MPosIntGpsHwStatusObserver and implement appropriate virtual methods in order
to receive notifications about changes to hardware status.

MPosIntGpsHwStatusObserver is used by the UI, and applications.

@publishedPartner
@released
@see CPosIntGpsHwStatus
 */
class MPosIntGpsHwStatusObserver
	{
public:
	IMPORT_C virtual TVersion Version() const;
	/**
	 * Notify observer of a GPS hardware status event
	 * @param aStatus The GPS hardware status.
	 * @param aError KErrNone if successful. Other possible error values defined by RProperty::Get()
	 */
    virtual void OnStatusUpdateEvent(CPosIntGpsHwStatus::TIntGpsHwStatus aStatus, TInt aError) = 0;

protected:					
	/**
	Currently not implemented.
	Should not be used. Provides for future expansion of the observer interface. 
	*/  
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	};
	
	
#endif // CPOSPOSITIONINGSTATUS_H
