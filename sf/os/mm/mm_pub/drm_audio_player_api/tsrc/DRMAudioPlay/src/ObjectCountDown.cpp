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

#include "ObjectCountDown.h"

TObjectCountDown::TObjectCountDown () : iObjectCount(0) {}

TInt TObjectCountDown::NotifyCreation()
	{
	return ++iObjectCount;
	}

TInt TObjectCountDown::GetCount()
	{
	return iObjectCount;
	}

TInt TObjectCountDown::NotifyDestruction()
	{
	if (--iObjectCount <= 0)
		{
		CActiveScheduler::Stop();
		}
	return iObjectCount;
	}
