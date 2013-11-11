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
* Description: 
*    File containing the inline functions defined in DosService.h
*
*/


//
// ---------------------------------------------------------
// CDosService::EventManager
// ---------------------------------------------------------
//  
inline CDosEventManager* CDosService::EventManager() const
{ 
	return iEventManager;
}

//
// ---------------------------------------------------------
// CDosService::RequestComplete
// ---------------------------------------------------------
//  
inline void CDosService::RequestComplete(const RMessagePtr2 aMessage, TInt aResult) const
{ 
	aMessage.Complete(aResult); 
}

