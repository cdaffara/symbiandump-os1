// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/


#ifndef COMPLETINGEVENTQUERYHEPLER_H
#define COMPLETINGEVENTQUERYHEPLER_H

#include <e32base.h>
#include <bluetooth/hci/completingeventquery.h>


NONSHARABLE_CLASS(CHCICompletingEventQueryHelper) : public CBase, public MHCICompletingEventQuery
	{
public:
// Interface to ascertain completing event expected
	virtual TBool MhceqCompletingEventExpected();
	
	CHCICompletingEventQueryHelper(TBool aExpectsToComplete);
	~CHCICompletingEventQueryHelper();
	static CHCICompletingEventQueryHelper* NewL(TBool aExpectsToComplete);
	void SetExpectsCompletingEvent(TBool aExpectCompletingEvent);
		
private:
	TBool iExpectCompletingEvent;
	
	};

#endif // COMPLETINGEVENTQUERYHEPLER_H
