/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Implementation for the RDosExtension class.
*
*/


#include <DosSvrServices.h>
#include "dosclientserver.h"
#include "dos_debug.h"

//
// ---------------------------------------------------------
// RDosExtension::Open
// ---------------------------------------------------------
//  
EXPORT_C TInt RDosExtension::Open(RDosServer& aServer)
{
    API_TRACE_( "[DOSSERVER] RDosExtension::Open()" );

    return CreateSubSession(aServer,EDosCreateExtensionSubSession, TIpcArgs(TIpcArgs::ENothing, TIpcArgs::ENothing, KAutoComplete));
}

//
// ---------------------------------------------------------
// RDosExtension::CallFunction 
// Synchronous, taking TAny*
// ---------------------------------------------------------
//  
EXPORT_C TInt RDosExtension::CallFunction(TInt aFunc, TAny *aParameter, TInt aParLength, TBool aAutoComplete) 
{
    API_TRACE_4( "[DOSSERVER] RDosExtension::CallFunction(0x%x, 0x%x, 0x%x, 0x%x)", aFunc, aParameter, aParLength, aAutoComplete );

    TExtensionParPckg parInfo;
	TPtr8 parData(static_cast<TUint8*>(aParameter), aParLength, aParLength);

	parInfo().iFunc = aFunc;
	parInfo().iParLength = aParLength;
	parInfo().iAutoComplete = aAutoComplete;

	return CallFunction(parInfo, parData);
}

//
// ---------------------------------------------------------
// RDosExtension::CallFunction 
// Synchronous, taking descriptor
// ---------------------------------------------------------
//  
EXPORT_C TInt RDosExtension::CallFunction(const TExtensionParPckg& aParamInfo, TDes8& aParameter)
{
    API_TRACE_4( "[DOSSERVER] RDosExtension::CallFunction(0x%x, 0x%x, 0x%x, 0x%x)"
        , aParamInfo().iFunc, aParamInfo().iParLength, aParamInfo().iAutoComplete, &aParameter );

    return SendReceive(ECallFunction, TIpcArgs(&aParamInfo, &aParameter, aParamInfo().iAutoComplete));
}

//
// ---------------------------------------------------------
// RDosExtension::CallFunction
// Asynchronous, taking descriptor
// ---------------------------------------------------------
//  
EXPORT_C void RDosExtension::CallFunction(TRequestStatus& aStatus, const TExtensionParPckg& aParamInfo, TDes8& aParameter)
{
    API_TRACE_1( "[DOSSERVER] RDosExtension::CallFunction(0x%x)", aStatus.Int() );
    API_TRACE_4( "[DOSSERVER] RDosExtension::CallFunction(0x%x, 0x%x, 0x%x, 0x%x)"
        ,aParamInfo().iFunc, aParamInfo().iParLength, aParamInfo().iAutoComplete, &aParameter );

    SendReceive(ECallFunction, TIpcArgs(&aParamInfo, &aParameter, aParamInfo().iAutoComplete), aStatus);
}

