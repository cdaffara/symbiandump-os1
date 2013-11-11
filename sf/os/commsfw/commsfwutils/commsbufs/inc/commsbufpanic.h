// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Comms buf panic
//

#ifndef __COMMSBUFPANIC_H__
#define __COMMSBUFPANIC_H__

#include <e32base.h>
#include <comms-infras/mbufpanic.h> 
class CommsBuf
	{
	public:
	static void Panic (TMBufPanic aPanic)
		{
		_LIT(mbuf,"MBuf");
		User::Panic(mbuf, aPanic);		
		}
	};


#endif // __COMMSBUFPANIC_H__
