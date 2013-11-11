/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef CREMCONACTIVE_H
#define CREMCONACTIVE_H

#include <e32base.h>

class MRemConObserver;

class CRemConActive : public CActive
	{
public:
	static CRemConActive* NewL( MRemConObserver& aObserver );
	~CRemConActive();

public:
	void SetActive();
	
private: // from CActive
	void RunL();
	void DoCancel();

private:
	CRemConActive( MRemConObserver& aObserver );
	void ConstructL();

private: // unowned
	MRemConObserver& iObserver;
	};

#endif // CREMCONACTIVE_H
