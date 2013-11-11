// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



inline TInt CPlayerCallbackHandler::ID() 
	{ 
	return iID; 
	}

inline TInt CStateCallbackHandler::ID() 
	{ 
	return iID; 
	}

inline TInt CToneCallbackHandler::ID() 
	{
	return iID; 
	}
	
inline CActiveListener::CActiveListener()
	:CActive(CActive::EPriorityIdle) 
	{ 
	CActiveScheduler::Add(this); 
	}

inline void CActiveListener::InitialiseActiveListener()
	{ 
	iStatus = KRequestPending; 
	SetActive(); 
	}

//



