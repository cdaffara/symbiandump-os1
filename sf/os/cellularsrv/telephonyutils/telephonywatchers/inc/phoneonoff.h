// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __PHONEONOFF_H__
#define	__PHONEONOFF_H__

// System includes
#include <e32base.h>
#include <e32property.h>


//
// ------> MPhoneOnOffObserver (header)
//
class MPhoneOnOffObserver
/**
@internalComponent
*/
{
//
public: // FROM MPhoneOnOffObserver
//
	IMPORT_C virtual void PhoneIsOff();
	IMPORT_C virtual void PhoneIsOn();
};




//
// ------> CPhoneOnOff (header)
//
class CPhoneOnOff : public CActive
/**
@internalComponent
*/
	{
//
public: // CONSTRUCT / DESTRUCT
//
	CPhoneOnOff(MPhoneOnOffObserver& aPhoneWatcher);
	~CPhoneOnOff();
	void ConstructL();

//
public: // ISSUE REQUEST
//
	void IssueRequestL();

//
private: // FROM CActive
//
	void RunL();
	void DoCancel();

//
private: // MEMBER DATA
//
	RProperty iPhonePowerProperty;
	TBool iWaitingForPhoneOff;
	MPhoneOnOffObserver& iPropertyPhoneWatcher;
	};


#endif
