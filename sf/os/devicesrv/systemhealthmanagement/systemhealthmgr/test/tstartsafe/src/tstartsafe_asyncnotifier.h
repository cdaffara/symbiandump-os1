/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TSTARTSAFE_ASYNCNOTIFIER_H_
#define TSTARTSAFE_ASYNCNOTIFIER_H_

class MSsTestAsyncNotifier
	{
public:
	virtual void SsTestNotify( TInt aNotify ) = 0;
	};

#endif /*TSTARTSAFE_ASYNCNOTIFIER_H_*/
