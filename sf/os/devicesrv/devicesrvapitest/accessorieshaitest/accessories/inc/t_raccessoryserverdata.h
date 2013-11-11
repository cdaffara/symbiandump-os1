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

 
#ifndef T_ACCESSORYSERVER_DATA_H
#define T_ACCESSORYSERVER_DATA_H

//User Includes
#include "datawrapperbase.h"

//System includes
#include <accessoryserver.h>

/**
 * Test Active Notification class
 *
 */
class CT_RAccessoryServerData : public CDataWrapperBase
	{
public:
	~CT_RAccessoryServerData();
	static	CT_RAccessoryServerData* NewL();

	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	void DoCmdConnect();
	void DoCmdDisconnect();
	
private:
	
	/**
	Accessory server client handle
	*/
	RAccessoryServer iServer;

	
	};

#endif // T_ACCESSORYSERVER_DATA_H