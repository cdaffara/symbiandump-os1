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
// Description:
// MAllCallReleaseCompleteObserver


#ifndef ALLCALLRELEASECOMPLETEOBSERVER_H_
#define ALLCALLRELEASECOMPLETEOBSERVER_H_

//system include
#include <e32std.h>
//user include


class MAllCallReleaseCompleteObserver
	{
public:
	
	/**
	 * All call was released by ATH
	 */
	virtual void ReleaseAllCallComplete(TInt aError) = 0;
	};

#endif /*ALLCALLRELEASECOMPLETENOTIFY_H_*/
