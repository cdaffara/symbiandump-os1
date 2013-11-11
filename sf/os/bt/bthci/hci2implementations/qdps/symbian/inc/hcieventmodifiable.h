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
 @internalComponent
*/

#ifndef HCIEVENTMODIFIABLE_H
#define HCIEVENTMODIFIABLE_H

#include <bluetooth/hci/event.h>

enum THCIEventModifiablePanic
	{
	EIllegalConstructorUse = 0
	};

/**
This class represents a QDP that is relevant for hardware that is perfect,
and so the Command Queue can operate normally without any special considerations.
*/
class THCIEventModifiable : public THCIEventBase
	{
public:
    /**
	 * Setter for the error code.
	 *
	 * Although this error code is not defined for all events in the HCI specification, the error code
	 * is in the base class so that the command queue can use the event error in its decisions without casting
	 * to a derived class.
	 * Those events that do not contain an error code in the specification should return with THCIErrorCode::EOK.
	 *
	 * @param aErrorCode The value the error code should be set to
	 */

	void SetErrorCode(THCIErrorCode aErrorCode);
	
	TPtrC8& EventData();

private:
	/* explicit constructor */
	THCIEventModifiable();
	};

#endif // HCIEVENTMODIFIABLE_H
