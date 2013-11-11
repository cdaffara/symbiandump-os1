// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

inline CRfcommStateFactory::CRfcommStateFactory()
	{
	}

inline CRfcommStateFactory::~CRfcommStateFactory()
	{
	// Destroy all the state objects
	iStates.DeleteAll();
	}
	
inline TRfcommState* CRfcommStateFactory::GetState(const TRfcommStates aState)
	/**
	   Return the state singleton.
	**/
	{
	__ASSERT_DEBUG(aState != ERfcommMaxState, Panic(ERfCommMuxerStateOutOfBounds));
	return iStates[aState];
	}
