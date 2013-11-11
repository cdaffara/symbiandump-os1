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
*    File containing inline functions for classes defined in DosEvent.h
*
*/


//
// ---------------------------------------------------------
// CDosEvent Constructor
// ---------------------------------------------------------
//  

inline CDosEvent::CDosEvent() : iListenerList(_FOFF(CDosEventRcvService,iLink)),iListenerIter(iListenerList)
{
}

//
// ---------------------------------------------------------
// CDosEvent::Event
// ---------------------------------------------------------
//  

inline TUint CDosEvent::Event() const
{ 
	return iEventName; 
}

//
// ---------------------------------------------------------
// CDosEvent::ListenerCount
// ---------------------------------------------------------
//  

inline TInt CDosEvent::ListenerCount() const
{ 
	return iListenerCount; 
}
