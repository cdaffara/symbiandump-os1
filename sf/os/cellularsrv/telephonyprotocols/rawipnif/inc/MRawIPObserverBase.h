// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the class MRawIPObserverBase. All internal NIF observer mixins
// derive from this class.
// 
//

/**
 @file
*/

#ifndef MRAWIPOBSERVERBASE_H__
#define MRAWIPOBSERVERBASE_H__

class MNifIfNotify;
class CBcaIoController;
class CBinderBase;

class MRawIPObserverBase
	{
public:
	enum TInitialisationState
		{
		EStart,
		EBcaController,
		EInitialised
		};

public:
	virtual void InitialiseL(TInitialisationState aState, TInt aError)=0;
	virtual void ShutDown(TInitialisationState aState, TInt aError=KErrNone)=0;

public:
	virtual CBinderBase* GetBinder()=0;
	};

#endif // MRAWIPOBSERVERBASE_H__
