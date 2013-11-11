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
*    This file contains the inline functions defined in DosSession.h
*
*/


//
// ---------------------------------------------------------
// CDosSession::Server
// ---------------------------------------------------------
//  
inline CDosServer& CDosSession::Server() const
{
	return *static_cast<CDosServer*>(const_cast<CServer2*>(CSession2::Server()));
}

//
// ---------------------------------------------------------
// CDosSession::ServiceFromHandle
// ---------------------------------------------------------
//  
inline CDosService* CDosSession::ServiceFromHandle(const TInt aHandle) const
{
	return (CDosService*)iServices->At(aHandle); 
}

