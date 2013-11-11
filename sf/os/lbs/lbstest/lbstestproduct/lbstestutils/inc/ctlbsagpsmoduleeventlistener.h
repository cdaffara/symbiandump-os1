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
 @test
*/

#ifndef __CT_LBS_AGPS_MODULE_EVENT_LISTENER_H__
#define __CT_LBS_AGPS_MODULE_EVENT_LISTENER_H__

#include <lbs/test/tlbsutils.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <e32property.h>

class MT_AgpsModuleEventHandler
	{
public:
	virtual void OnSetGpsOptions(const TLbsGpsOptions& aGpsOptions) = 0;
	};

class CT_AgpsModuleEventListener : public CActive
/**
 * This class listens to events published by the AGPS Module.
 * 
 * @internalComponent
*/
	{
public:
	IMPORT_C static CT_AgpsModuleEventListener* NewL(MT_AgpsModuleEventHandler& aHandler);
	IMPORT_C ~CT_AgpsModuleEventListener();
	
private:
	CT_AgpsModuleEventListener(MT_AgpsModuleEventHandler& aHandler);
	void ConstructL();
	
	void RunL();
	void DoCancel();

private:
	/** The call back class */
	MT_AgpsModuleEventHandler& iHandler;
	/** The property that will subscribe to the AGPS Module mode changes key */
	RProperty iModeChangesProperty;
	};

#endif
