// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for etel driver input class
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef RETELDRIVERINPUT_H
#define RETELDRIVERINPUT_H

#include <e32def.h>
#include <e32std.h>
#include <networking/umtsnifcontrolif.h>	// definition of TContextId

#include "eteldrivernmspace.h"	// definition of EtelDriver::TEtelInput
#include "PDPFSM.h"

class CEtelDriverFactory;
class CPdpFsmInterface;
using namespace SpudMan;
/** 
	Definition of an input interface for SPUD eTel driver 

@internalComponent
*/
class REtelDriverInput
	{
public:
	REtelDriverInput();
	virtual ~REtelDriverInput();

	virtual void OpenL (CPdpFsmInterface& aPdpFsmInterface);
	virtual void Close();
	virtual void CreatePdpL (TContextId aPdpId,SpudMan::TPdpContextType aContextType);
	virtual void DeletePdp (TContextId aPdpId);
	virtual void Input(TContextId aPdpId, EtelDriver::TEtelInput aOperation);
	virtual void CancelPdp (TContextId aPdpId);
	virtual void StartPdpNotifications(TContextId aPdpId);
	virtual void CancelPdpNotifications (TContextId aPdpId);
	virtual void CancelAllPdps();
	virtual void CancelAllPdpNotifications();

protected:

private:
	// disable copy of an object
	inline REtelDriverInput(REtelDriverInput&);
	inline REtelDriverInput& operator= (REtelDriverInput& aInput);
	
private:
	/** etel driver factory */
	CEtelDriverFactory*	iDriverFactory;
	};
	

// inlines
/** copy c'tor
	not supported for this class
*/
inline REtelDriverInput::REtelDriverInput(REtelDriverInput&)
	{ 
	// not supported
	__ASSERT_ALWAYS(EFalse, User::Panic(EtelDriver::KTxtSpudTel, KErrNotSupported)); 
	}
	
/** assignment operator
	not supported for this class
*/
inline REtelDriverInput& REtelDriverInput::operator= (REtelDriverInput& aInput)
	{ 
	// not supported
	__ASSERT_ALWAYS(EFalse, User::Panic(EtelDriver::KTxtSpudTel,KErrNotSupported));
	return aInput; 
	}

#endif
// RETELDRIVERINPUT_H
