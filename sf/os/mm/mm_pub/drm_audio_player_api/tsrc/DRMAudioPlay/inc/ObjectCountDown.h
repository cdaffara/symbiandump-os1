/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: DRM Player
*
*/

#ifndef __OBJECT_COUNTDOWN_H
#define __OBJECT_COUNTDOWN_H

#include <e32std.h>
#include <e32base.h>

class TObjectCountDown
	{
	private:
		TInt iObjectCount;

	public:
		TObjectCountDown();
		TInt NotifyCreation();
		TInt GetCount();
		TInt NotifyDestruction();
	};

#endif //__OBJECT_COUNTDOWN_H
