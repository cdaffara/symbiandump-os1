/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* audiostreamfactory.h
* Part of     : ACL Logical layer
*
*/



#ifndef COMPONENTPARAMETERS_H
#define COMPONENTPARAMETERS_H

class MLogicalSettingObserver; 

class TComponentParameters
	{
public:
	TUid iTypeUid;
	TAudioComponentId iInstanceId;
	TAudioContextId iContextId;
	MLogicalSettingObserver* iSettingsObserver;
	};

#endif // COMPONENTPARAMETERS_H
