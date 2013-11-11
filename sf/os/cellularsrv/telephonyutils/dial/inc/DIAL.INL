// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Clears the following flags:
// KPhoneNumberMobile
// KPhoneNumberUsePulseDial
// KPhoneNumberPbxUsePulseDial
// KPhoneNumberWaitForDialingTone
// KPhoneNumberWaitForProceedTone
// KReturnPhoneNumberWithPlusFormat
// 
//

/**
 @publishedAll
 @deprecated 9.1
*/
inline void TDialLocation::ClearDialLocationFlags (TUint aFlags)
    {
    iFlags &= ~aFlags;
    }

/**
True if the KReturnPhoneNumberWithPlusFormat flag is set

@publishedAll
@deprecated 9.1
*/
inline TBool TDialLocation::IsReturnPhoneNumberWithPlusFormat() const
    {
    return(iFlags & KReturnPhoneNumberWithPlusFormat);
    }
