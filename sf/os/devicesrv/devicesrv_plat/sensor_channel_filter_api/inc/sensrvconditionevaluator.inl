/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Control interface for Condition Evaluator plug-ins.
*
*/


#include <ecom/ecom.h>

/**
* Length of match string: "0x00000000"
*/ 
const TInt KSensrvMatchStringLen(10);

/**
* Hexadecimal prefix
*/
_LIT8( KSensrvHexPrefix, "0x" );

// -----------------------------------------------------------------------------
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
//
inline CSensrvConditionEvaluator::~CSensrvConditionEvaluator()
    {
    REComSession::DestroyedImplementation( iDestructorIdKey );
    }

// -----------------------------------------------------------------------------
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
inline CSensrvConditionEvaluator* CSensrvConditionEvaluator::NewL( TSensrvChannelInfo& aChannelInfo )
    {    
    // Convert aMatch to descriptor
    HBufC8* matchString = HBufC8::NewLC(KSensrvMatchStringLen);
    matchString->Des().Append(KSensrvHexPrefix);
    matchString->Des().AppendNum(aChannelInfo.iChannelType, EHex);

    // Set resolve parameters to find correct implementation
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(*matchString);
    resolverParams.SetWildcardMatch(EFalse);

    // Create instance using ECom
    TAny* interface = NULL;
    TRAPD( err, interface = REComSession::CreateImplementationL( TUid::Uid(KSensrvConditionEvaluatorInterfaceUid), 
                                        _FOFF( CSensrvConditionEvaluator, iDestructorIdKey ),
                                        &aChannelInfo,
                                        resolverParams ) );
                                        
    if( KErrNone != err )
        {
        //If condition plug-in doesn't exist, 
        //CreateImplementationL will leave with KEComErrNoInterfaceIdentified.
        if( err == KErrNoMemory )
            {
            User::Leave( KErrNoMemory );
            }
        else
            {   
            User::Leave( KErrNotSupported );
            }
        }

    CSensrvConditionEvaluator* service = reinterpret_cast<CSensrvConditionEvaluator*>( interface );    
    
    CleanupStack::PopAndDestroy(matchString);

    return service;
    }
            
// End of File


