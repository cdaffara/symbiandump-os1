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

#ifndef CENREPOBSERVER_H
#define CENREPOBSERVER_H

//listen for changes to CenRep keys in repository

#include <e32base.h>

class CCenRepListener;
class MCenRepObserver
	{
public:
	virtual void HandleNotifyInt( TUint32 aId, TInt aNewValue ) = 0;
	virtual void HandleNotifyError( TUint32 aId, TInt aError, CCenRepListener* aHandler ) = 0;
	};
	
class CRepository;
NONSHARABLE_CLASS(CCenRepListener) : public CActive
    {
public:
   	static CCenRepListener* NewL(MCenRepObserver& aCallback, CRepository& aSession, TUint32 aId );
   	~CCenRepListener();
   	
   	void StopListening();
   	void StartListeningL();   	
   	
protected:
	CCenRepListener(MCenRepObserver& aCallback, CRepository& aSession, TUint32 aId);
	void RunL();
	void DoCancel();
	
private:
	MCenRepObserver& iCallback;
	CRepository& iSession;
	TUint32 iId;   	
    };



#endif //CENREPOBSERVER_H
