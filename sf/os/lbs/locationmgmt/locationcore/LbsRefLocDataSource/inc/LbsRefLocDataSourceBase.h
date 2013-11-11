// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LbsLocSourceGpsBase.h
// 
//
/** @file
    @publishedPartner
	@prototype
 */

#ifndef LBSREFLOCSOURCEGPSBASE_H
#define LBSREFLOCSOURCEGPSBASE_H

#include <lbs/lbsadmin.h>
#include <lbs/lbsloccommon.h>
#include <lbspositioninfo.h>
#include <lbs/lbsassistancedatabase.h>
#include <lbsareainfo.h>

class MLbsRefLocationObserver;

/**
 Defines the interface which needs to be implemented by the ECOM plugins which can return a 
 reference position. The interface allows for a request to a reference position and for cancelling that 
 request. The requests are asynchronous, the result being given through an observer interface. So when 
 the implementation manages to obtain the reference position it returns it through the calls in 
 MLbsRefLocationObserver. If there is an error retrieving the reference position then this 
 error should be reported through the call-back. The interface allows also cancelling of the request 
 made. If no request is outstanding nothing should be done. If a second request is made while one 
 is already active then the second request can just be ignored (since the request doesn't really have 
 any parameters and all it's doing is retrieving something that is stored, there is no need to 
 pass through the second request when the first one is still outstanding).
 
 The implementer of the interface needs to call the BaseConstructL() method as part of the construction 
 procedure to ensure the object is build fully.
 
 @see MLbsRefLocationObserver
 */
class CLbsRefLocationSourceBase : public CBase
	{
public: 
  	IMPORT_C static CLbsRefLocationSourceBase* NewL(MLbsRefLocationObserver& aObserver, TUid aEcomModuleId);
	IMPORT_C ~CLbsRefLocationSourceBase();
	IMPORT_C TVersion Version() const;
	
	/**
	 Method used to set the address of the original requester of the reference location.
	 
	 @param aAddress The adress of the originator of the request
	*/
	virtual void SetRequestorAddress(const TDesC& aAddress) = 0;
	/**
	 Interface definition. This method is used to make a request for reference position information.
	 
	 Making a request while another one is outstanding should not affect anything and can be ignored.
	 */
	virtual void RequestRefPositionInfo() = 0; 
	/**
	 Interface definition. This method is used to cancel a previous request for reference position information.
	 */
	virtual void CancelRefPositionInfoRequest() = 0;
	
protected:
	IMPORT_C CLbsRefLocationSourceBase(MLbsRefLocationObserver& aObserver);
	IMPORT_C void BaseConstructL();

private:
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	
protected:
	/** Observer which receives call-backs with the ref position information */
    MLbsRefLocationObserver& iObserver;
	
private:
	/** reserved for future use */
	TAny* iReserved;
	/** the ID key used for destroying the ECom interface */
	TUid  iDtor_ID_Key;
	/** The version of the interface */
	TVersion iVersion;
	};

/**
 Observer interface for callbacks with the results to a request for reference position.   
 
 @see MLbsRefLocationObserver
 */
class MLbsRefLocationObserver
    {
public:
    IMPORT_C virtual TVersion Version() const;
    /**
     Callback used by the implementation of CLbsRefLocationSourceBase to report the 
     reference location. The ownership of the pointer parameters is passed to the 
	 observer class.
     
     @param aError The error code associated with the outcome, if any, otherwise KErrNone
     @param aFirstPosition The first reference position. Ownership is transfered to the 
		MLbsRefLocationObserver which should ensure the object gets deleted.
     @param aFirstCellId The cell ID corresponding to the first reference position. Ownership is transfered to the 
		MLbsRefLocationObserver which should ensure the object gets deleted.
     @param aSecondPosition The second position. Ownership is transfered to the 
		MLbsRefLocationObserver which should ensure the object gets deleted.
     @param aSecondCellId The cell ID corresponding to the second position. Ownership is transfered to the 
		MLbsRefLocationObserver which should ensure the object gets deleted.
     */
    virtual void HandleRefPositionInformation(TInt aError, const TPositionInfoBase* aFirstPosition, const TPositionAreaInfoBase* aFistCellId, 
                const TPositionInfoBase* aSecondPosition, const TPositionAreaInfoBase* aSecondCellId) = 0;
private:
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
				
    };

#endif //LBSREFLOCSOURCEGPSBASE_H
