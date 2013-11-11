/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

 
#ifndef T_RACCESSORYCONNECTION_DATA_H
#define T_RACCESSORYCONNECTION_DATA_H

//User Includes
#include "datawrapperbase.h"

//System includes
#include <accessoryserver.h>
#include <accessoryconnection.h>


/**
 * Test Active Notification class
 *
 */
class CT_RAccessoryConnectionData : public CDataWrapperBase
	{
public:
	static	CT_RAccessoryConnectionData* NewL();

	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	
	void DoCmdCreateSubSession(const TTEFSectionName& aSection);
	void DoCmdCloseSubSession();
	void DoCmdGetAccessoryConnectionStatus();
	void DoCmdUtilityVerifyDeviceTypeCaps(const TTEFSectionName& aSection);
	void LogDeviceTypeCapabilities(const TUint aDeviceTypeCaps);

private:
		
	/**
	Accessory server connection handle (a subsession to accessory server)
	*/
	RAccessoryConnection iConnection;

    /**
	For saving accessory connection states, set by DoCmdGetAccessoryConnectionStatus function
	*/
	TAccPolGenericIDArray iGenericIDarray;
	};

#endif // T_RACCESSORYCONNECTION_DATA_H
