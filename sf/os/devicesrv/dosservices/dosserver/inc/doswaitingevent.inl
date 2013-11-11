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
*    File containing inline functions for classes defined in DosWaitingEvent.h
*
*/


//
// ---------------------------------------------------------
// CDosWaitingEvent::HasParameter
// ---------------------------------------------------------
//  
inline TBool CDosWaitingEvent::HasParameter() const
{
	return iParameter ? ETrue : EFalse;
}

//
// ---------------------------------------------------------
// RDosEventRcv::Parameter
// ---------------------------------------------------------
//  
inline const TDesC8& CDosWaitingEvent::Parameter() const
{
	return *iParameter; 
}

//
// ---------------------------------------------------------
// RDosEventRcv::IsQueued
// ---------------------------------------------------------
//  
inline TBool CDosWaitingEvent::IsQueued() const
{ 
	return iQueueCount>0; 
}
