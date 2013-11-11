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
*    This file contains the inline functions defined in DosServer.h
*
*/


//
// ---------------------------------------------------------
// CDosServer::ContainerIx
// ---------------------------------------------------------
//  
inline CObjectConIx* CDosServer::ContainerIx() const
{ 
	return iContainerIx; 
}

//
// ---------------------------------------------------------
// CDosServer::EventManager
// ---------------------------------------------------------
//  
inline CDosEventManager* CDosServer::EventManager() const
{ 
	return iEventManager; 
}

//
// ---------------------------------------------------------
// CDosServer::DosFactory
// ---------------------------------------------------------
//  
inline CDosFactoryBase* CDosServer::DosFactory() const
{ 
	return iDosFactory; 
}


//
// CDosServer::TSignal class
//

//
// ---------------------------------------------------------
// CDosEvent::TSignal Constructor
// ---------------------------------------------------------
//  

inline CDosServer::TSignal::TSignal()
{
}

//
// ---------------------------------------------------------
// CDosEvent::TSignal Constructor
// ---------------------------------------------------------
//  
inline CDosServer::TSignal::TSignal(TRequestStatus& aStatus)
    : iStatus(&aStatus), iId(RThread().Id())
{
	aStatus=KRequestPending; 
}

//
// ---------------------------------------------------------
// CDosEvent::TSignal::Get
// ---------------------------------------------------------
//  
inline TPtrC CDosServer::TSignal::Get() const
{
	return TPtrC((const TText*)this,sizeof(*this)/sizeof(TText));
}

//
// ---------------------------------------------------------
// CDosEvent::TSignal::Set
// ---------------------------------------------------------
//  
inline TInt CDosServer::TSignal::Set(const TDesC& aData)
{
    return aData.Size()!=sizeof(*this) ? 
           KErrGeneral :
           (Mem::Copy(this,aData.Ptr(),sizeof(*this)),KErrNone);
}
