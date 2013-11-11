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
*    File containing inline functions for classes defined in DosSvrServices.h
*
*/


//
// ---------------------------------------------------------
// RDosEventRcv::ParameterSize
// ---------------------------------------------------------
//  
inline TInt RDosEventRcv::ParameterSize() const
{ 
	return ( iParamContent ? iParamContent->Size() : 0 ); 
}

//
// ---------------------------------------------------------
// RDosEventRcv::Parameter
// ---------------------------------------------------------
//  
inline HBufC8* RDosEventRcv::Parameter() const
{ 
	return iParamContent;
}

//
// ---------------------------------------------------------
// RDosEventRcv::Queue
// ---------------------------------------------------------
//  
inline TQueueType RDosEventRcv::Queue() const
{
	return iQueue;  
}

//
// ---------------------------------------------------------
// RDosEventRcv::Event
// ---------------------------------------------------------
//  
inline TUint RDosEventRcv::Event() const
{ 
	return iDosEvent; 
}

//
// ---------------------------------------------------------
// CDosEventListenerBase::DosServer
// ---------------------------------------------------------
//  
inline const RDosServer& CDosEventListenerBase::DosServer() const
{
	return iDosServer; 
}

