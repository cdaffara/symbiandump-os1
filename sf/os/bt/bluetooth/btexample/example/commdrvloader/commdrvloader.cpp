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
//

#include <e32std.h>


TInt E32Main()
/**
Load the wins device drivers so the Bluetooth HCI layer can use them
*/
	{
#ifdef __WINS__
	TInt err;
	_LIT(COMM_PDD_NAME, "ECDRV");
	_LIT(COMM_LDD_NAME, "ECOMM");
	err = User::LoadLogicalDevice(COMM_LDD_NAME);
	if(err)
		{
		return err;
		}
		
	err = User::LoadPhysicalDevice(COMM_PDD_NAME);
	if(err)
		{
		return err;
		}
		
	return KErrNone;
#else
	return KErrNotSupported;
#endif	
	}
