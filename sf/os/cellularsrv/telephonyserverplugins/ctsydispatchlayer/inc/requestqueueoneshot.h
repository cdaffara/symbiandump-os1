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

/**
 @file
 @internalAll 
*/

#ifndef REQUESTQUEUEONESHOT_H
#define REQUESTQUEUEONESHOT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CMmDataPackage;
class MDispatcherCallback;

// CLASS DECLARATION

/**
 * CRequestQueueOneShot holds a queue of requests which have been completed 
 * synchronously by the Licensee LTSY but  needs completion 
 * asynchronously back up to the Common TSY.
 */
class CRequestQueueOneShot : public CAsyncOneShot 
	{
public:

	virtual ~CRequestQueueOneShot();

	static CRequestQueueOneShot* NewL();
	static CRequestQueueOneShot* NewLC();

	/**
	 * Class which encapsulates data returned from the Licensee LTSY for 
	 * requests which are dealt with synchronously.  The data held in this 
	 * object is returned to the Common TSY.
	 */
	class TIpcDataPackage
		{
	public:
		TIpcDataPackage();
		~TIpcDataPackage();
	public:
		TInt iIpc;
		CMmDataPackage* iDataPackage; 	// When this field is assigned to an object,
										// this TIpcDataPackage object takes ownership
										// of it.
		TInt iResultCode;
		TBool iCleanupOnly; // Indicates to the dispatcher that it should not 
							// complete the request because the TSY is currently 
							// closing down. 
		MDispatcherCallback* iDispatcherCallback;
		TSglQueLink iLink;
		}; // class TIpcDataPackage 
	
	void QueueRequest(TIpcDataPackage& aIpcDataPackage);
	
protected:

	virtual void DoCancel();
	virtual void RunL();
	
private:

	CRequestQueueOneShot();
	void ConstructL();

private:
	
	TSglQue<TIpcDataPackage> iIpcQueue;

	}; // class CRequestQueueOneShot 

#endif // REQUESTQUEUEONESHOT_H
