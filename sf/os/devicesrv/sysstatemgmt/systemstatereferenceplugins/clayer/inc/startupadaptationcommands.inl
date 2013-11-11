/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface   : Domain, Startup Adaptation API
* Implementation for inline functions of classes defined in
* startupadaptationcommands.h.
*
*/



#ifndef __STARTUPADAPTATIONCOMMANDS_INL__
#define __STARTUPADAPTATIONCOMMANDS_INL__

/*
* @publishedPartner
* @released
*/

// ============================ MEMBER FUNCTIONS ===============================

namespace StartupAdaptation
    {
    inline RLanguageListResponse::RLanguageListResponse()
      : iErrorCode( KErrNone )
        {
        }

    inline void RLanguageListResponse::Close()
        {
        iLanguages.Close();
        }
    }

#endif	// __STARTUPADAPTATIONCOMMANDS_INL__
