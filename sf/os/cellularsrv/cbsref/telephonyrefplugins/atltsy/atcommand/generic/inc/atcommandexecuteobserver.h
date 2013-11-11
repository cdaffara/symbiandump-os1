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
// @file atcommandexecuteobserver.h
// 

#ifndef ATCOMMANDEXECUTEOBSERVER_H_
#define ATCOMMANDEXECUTEOBSERVER_H_

//symstem include

//user include
#include "ltsycommondefine.h"


class MAtCommandExecuteObserver
	{
public:
	virtual void AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus) = 0;
	};

#endif /*ATCOMMANDEXECUTEOBSERVER_H_*/
