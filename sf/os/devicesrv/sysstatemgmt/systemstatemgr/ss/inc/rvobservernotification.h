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

#ifndef __RVOBSERVERNOTIFICATION_H__
#define __RVOBSERVERNOTIFICATION_H__


class MRvObserverNotification
	{
public:
	virtual void NotifyCompletion( const TInt aCompletion ) = 0;
	virtual void NotifyCancellation( TInt aCompletion ) = 0;
	};
	
	
#endif // __RVOBSERVERNOTIFICATION_H__