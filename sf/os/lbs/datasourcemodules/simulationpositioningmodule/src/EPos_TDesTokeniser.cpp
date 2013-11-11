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
#include "EPos_TDesTokeniser.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TDesTokeniser::TDesTokeniser(const TDesC8& aTokenString) :
    iLexer(aTokenString)
    {
    }

// ----------------------------------------------------------------------------
// TDesTokeniser::HasMoreTokens
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool TDesTokeniser::HasMoreTokens()
    {
    return (!iLexer.Eos());
    }

// ----------------------------------------------------------------------------
// TDesTokeniser::SkipToken
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void TDesTokeniser::SkipToken(const TChar& aSeparator)
    {
    while (HasMoreTokens() && iLexer.Get() != aSeparator) {}
    }

// ----------------------------------------------------------------------------
// TDesTokeniser::SkipToken
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void TDesTokeniser::SkipToken(const TInt aNrOfTimes)
    {
    for(TInt i=0; i<aNrOfTimes; i++)
        {
        SkipToken();
        }
    }

// ----------------------------------------------------------------------------
// TDesTokeniser::NextToken
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TPtrC8 TDesTokeniser::NextToken(const TChar& aSeparator)
    {
    iLexer.SkipSpace();
    iLexer.Mark();
    while (HasMoreTokens() && iLexer.Peek() != aSeparator)
        {
        iLexer.Inc();
        }
    TPtrC8  retVal = iLexer.MarkedToken();


    if (HasMoreTokens())
        {
        iLexer.Inc();
        }
    return retVal;
    }

//  End of File
