// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32base.h>
#include <lbs.h>
#include <lbserrors.h>

#include "EPos_Global.h"

// ================= DEBUG TRACES =======================

#ifdef _DEBUG

#include <lbs/epos_cpostrace.h>
_LIT(KTraceFileName, "EPos_Global.cpp");

CPosTrace* theTrace = NULL;

void Global::SetupTraceL()
    {
    if (!theTrace)
        {
        theTrace = CPosTrace::NewL();
        }
    }

void Global::TearDownTrace()
    {
    delete theTrace;
    theTrace = NULL;
    }

void Global::Trace(const TDesC& aDescription, const TDesC& aFilename, const TInt aLineNumber)
    {
    if (theTrace)
        {
        // to epos log
        TInt err;
        TRAP(err, theTrace->TraceL(aDescription, aFilename, aLineNumber));
        
        // same to epocwind.out
        _LIT(KPosTraceLogFormat , "EPos: Server: %S");
        RDebug::Print(KPosTraceLogFormat, &aDescription);
        }
        
    }
    
#endif

// ================= LOCAL FUNCTIONS =======================

TInt DataSizeL(const RMessage2& aMessage, const TInt aClientBufferParam)
    {
    TInt dataSize = aMessage.GetDesLength(aClientBufferParam);
    if (dataSize < 0)
        {
        aMessage.Panic(KPosClientFault, EPositionBadDescriptor);
        User::Leave(KErrNoMemory);
        }
    return dataSize;
    }

void LeaveIfFalseL(const TBool aCondition, const TInt aReason)
    {
    if (!aCondition)
        {
        User::Leave(aReason);
        }
    }

// ================= MEMBER FUNCTIONS =======================

/**
 * Read and kill the client if it leaves. If the returned error code 
 * is negative, the RMessage must not be completed. Complete after 
 * client is paniced crashes the server with USER 70.
 *
 * @param aMessage Message instance
 * @param aPtr Pointer to read from
 * @param aDes Descriptor to read to
 * @param aOffset offset to start read
 * @return Error code
 */
TInt Global::Read(const RMessage2& aMessage, const TInt aParam, TDes8& aDes, TInt aOffset)
	{
    TInt ret = aMessage.Read(aParam, aDes, aOffset);
    if (ret != KErrNone)
        {
        aMessage.Panic(KPosClientFault, EPositionBadDescriptor);
        }
    return ret;
    }

/**
 * Read and kill the client if it leaves. If the returned error code 
 * is negative, the RMessage must not be completed. Complete after 
 * client is paniced crashes the server with USER 70.
 *
 * @param aMessage Message instance
 * @param aPtr Pointer to read from
 * @param aDes Descriptor to read to
 * @param aOffset offset to start read
 * @return Error code
 */
TInt Global::Read(const RMessage2& aMessage, const TInt aParam, TDes16& aDes, TInt aOffset)
    {
	TInt ret = aMessage.Read(aParam, aDes, aOffset);
    if (ret != KErrNone)
        {
        aMessage.Panic(KPosClientFault, EPositionBadDescriptor);
        }
    return ret;
    }

/**
 * Write and kill the client if it leaves. If the returned error code 
 * is negative, the RMessage must not be completed. Complete after 
 * client is paniced crashes the server with USER 70.
 *
 * @param aMessage Message instance
 * @param aPtr Pointer to write to
 * @param aDes Descriptor to write from
 * @param aOffset offset to start write
 * @return Error code
 */
TInt Global::Write(const RMessage2& aMessage, const TInt aParam, const TDesC8& aDes, TInt aOffset)
	{
	TInt ret = aMessage.Write(aParam, aDes, aOffset);
	if (ret != KErrNone)
        {
        aMessage.Panic(KPosClientFault, EPositionBadDescriptor);
        }
    return ret;
	}

/**
 * Copies an 8-bit buffer from the address space of the client and puts 
 * the result in the returned argument. If the method leaves it the 
 * client may have been paniced and in that case it is forbidden to
 * complete the RMessage. If a client is paniced can be verified by the
 * call chain:
 * aMessage.Client().ExitType()
 *
 * @param aMessage the message from the client.
 * @param aClientBuffer a pointer to an 8-bit descriptor located in the 
 *        address space of the client.
 * @return a copy of the client buffer.
 */
HBufC8* Global::CopyClientBuffer8LC(const RMessage2& aMessage, const TInt aClientBufferParam)
    {
    // Create an empty server side buffer that will contain client data
    TInt dataSize = DataSizeL(aMessage, aClientBufferParam);
    HBufC8* buffer = HBufC8::NewLC(dataSize);

    // Populate server side buffer with client data
    TPtr8 ptrToBuf = buffer->Des();
    User::LeaveIfError(Global::Read(aMessage, aClientBufferParam, ptrToBuf));
    return buffer;
    }

/**
 * Copies a 16-bit buffer from the address space of the client and puts 
 * the result in the returned argument. If the method leaves it the  
 * client may have been paniced and in that case it is forbidden to 
 * complete the RMessage. If a client is paniced can be verified by the 
 * call chain:
 * aMessage.Client().ExitType()
 *
 * @param aMessage the message from the client.
 * @param aClientBuffer a pointer to a 16-bit descriptor located in the 
 *        address space of the client.
 * @return a copy of the client buffer.
 */
HBufC* Global::CopyClientBufferLC(const RMessage2& aMessage, const TInt aClientBufferParam)
    {
    // Create an empty server side buffer that will contain client data
    TInt dataSize = DataSizeL(aMessage, aClientBufferParam);
    HBufC* buffer = HBufC::NewLC(dataSize);

    // Populate server side buffer with client data
    TPtr ptrToBuf = buffer->Des();
    User::LeaveIfError(Global::Read(aMessage, aClientBufferParam, ptrToBuf));
    return buffer;
    }

/**
 * Checks that buffer size from client's message is the same
 * as position data class size specified in the class.
 *
 * @param aClass Position data class.
 * @param aBuffer Buffer containing position data class.
 * Leaves with KErrArgument if buffer is wrong.
 */
void Global::ValidatePositionClassBufferL(const TPositionClassTypeBase& aClass, const TDes8& aBuffer)
    {
DEBUG_TRACE("ValidatePositionClassBufferL in", __LINE__);
    LeaveIfFalseL(aBuffer.MaxSize() >= sizeof(TPositionClassTypeBase), KErrArgument);
    LeaveIfFalseL(aBuffer.MaxSize() >= aClass.PositionClassSize(), KErrArgument);
DEBUG_TRACE("ValidatePositionClassBufferL out", __LINE__);
    }

/**
 * Checks that class type specified in class is supported
 *
 * @param aClass Position data class.
 * @param aTypeExpected Position data class type as expected.
 * @param aResult Leave code if type is wrong. Default is KErrNotSupported.
 */
void Global::ValidatePositionClassTypeL(const TPositionClassTypeBase& aClass, const TInt aTypeExpected, const TInt aResult)
    {
DEBUG_TRACE("ValidatePositionClassTypeL in", __LINE__);
    if (aClass.PositionClassType() != aTypeExpected)
        {
        User::Leave(aResult);
        }
DEBUG_TRACE("ValidatePositionClassTypeL out", __LINE__);
    }

//  End of File  
