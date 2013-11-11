// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __SMSUACT_H__
#define __SMSUACT_H__

#include <e32base.h>

class CSmsuTimeout;


/**
 *  Base class for SMS active objects.
 *  
 *  @publishedAll
 *  @released 
 */
class CSmsuActiveBase : public CActive
	{
	friend class CSmsuTimeout;
	
public:
	IMPORT_C ~CSmsuActiveBase();

protected:
	IMPORT_C CSmsuActiveBase(TInt aPriority);

	IMPORT_C virtual void Complete(TInt aStatus);

	IMPORT_C void   ConstructTimeoutL();

	IMPORT_C void   CompleteMyself(TInt aStatus, TBool aSetActive = ETrue);
	IMPORT_C void   CompleteMyselfAfterTimeout(TInt aStatus);

	IMPORT_C void   TimedSetActive(const TTimeIntervalMicroSeconds32& aTimeIntervalMicroSeconds32);
	IMPORT_C void   TimedSetActiveCancel();
	IMPORT_C TBool  TimedOut() const;

	IMPORT_C void   Queue(TRequestStatus& aStatus);

	IMPORT_C TInt   RunError(TInt aError); //< Calls complete

	/**
	 *  Implements derived-class specified RunL() behaviour.
	 *  
	 *  This is called from RunL(). 
	 */
    virtual void DoRunL()=0;

	/**
	 *  Implements derived-class specified Complete() behaviour.
	 *  
	 *  This is called from Complete().
	 *  
	 *  @param Active object status word 
	 */
	inline virtual void DoComplete(TInt&) {};

    IMPORT_C void RunL();

protected:
	/**
	 *  Timer, to be activated by TimedSetActive().
	 */
	CSmsuTimeout* iSmsuTimeout;

	/**
	 *  Observer, set by Queue().
	 */
	TRequestStatus* iReport;
	};

#endif // __SMSUACT_H__
