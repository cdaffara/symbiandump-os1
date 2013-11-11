// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMPROPERTYOBSERVER_H__
#define __SSMPROPERTYOBSERVER_H__

#include <e32base.h>
#include <e32property.h>
#include <ssm/ssmutility.h>

/**
 * Provide an abstract class for observing RProperty value changes from the SSM Utility Server.
 * 
 * @publishedPartner
 * @released
 */
class CSsmPropertyObserver : public CActive, public MSsmUtility
{
public:
	
	IMPORT_C CSsmPropertyObserver(TUid aCategory, TUint aKey);
	
	/**
	 * Function that is called everytime the property for this observer changes.
	 * 
	 * This is also called once when the observation is first started using StartL().
	 * 
	 * @param aValue The new value for the observed property.
	 * 
	 * @return ETrue if this observer should continue to monitor, EFalse otherwise.
	 * 
	 * @note If EFalse is returned from this object then this object is not destroyed.
	 * 
	 * @publishedPartner
	 * @released
	 */
	virtual TBool PropertyChangedL(TInt aValue) = 0; 
	
	// From MSsmUtility
	IMPORT_C virtual void InitializeL();
	IMPORT_C virtual void StartL();
	IMPORT_C virtual void Release();
	
protected:
	void doRunL();

	IMPORT_C virtual ~CSsmPropertyObserver();
	
	// From CActive
	IMPORT_C virtual void RunL();
	IMPORT_C virtual void DoCancel();
	IMPORT_C virtual TInt RunError(TInt aError);
	
protected:
	const TUid iCategory;
	const TUint iKey;
	
	RProperty iProperty;
	
	TInt iReserved[2];
};

#endif // __SSMPROPERTYOBSERVER_H__
