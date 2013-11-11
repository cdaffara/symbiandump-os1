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

#ifndef HCICOMMANDBASEEXTENSION_H
#define HCICOMMANDBASEEXTENSION_H

#include <e32base.h>
#include <bluetooth/hci/completingeventqueryhelper.h>
#include <bluetooth/hci/command.h>


/**
This class provides access to the Event Completion query methods
*/

NONSHARABLE_CLASS(CHCICommandBaseExtension) : public CBase
	{
	
public:
	static CHCICommandBaseExtension* NewL();
	~CHCICommandBaseExtension();
	CHCICompletingEventQueryHelper* CompletingEventQueryHelper();
	void SetCompletingEventQueryHelper(CHCICompletingEventQueryHelper*);
	
private:
	CHCICommandBaseExtension();
	CHCICompletingEventQueryHelper* iCompletingEventQueryHelper;
	
	// Reserved for use when the class needs to be extended in a binary compatible way.
	TAny* iReserved;

	};
	
#endif // HCICOMMANDBASEEXTENSION_H
