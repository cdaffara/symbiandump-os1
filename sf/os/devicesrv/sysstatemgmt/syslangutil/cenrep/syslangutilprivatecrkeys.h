/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Definitions of private CenRep keys of SysLangUtil component.
*
*/

#ifndef SYSLANGUTILPRIVATECRKEYS_H
#define SYSLANGUTILPRIVATECRKEYS_H

#include <e32cmn.h>

// ===========================================================================
// SysLangUtil Stored Values API
// ===========================================================================
//
static const TUid KCRUidSysLangUtil = { 0x100059CB }; // SysLangUtil DLL UID

/**
* The selected SIM Language.
* Possible values: 0 (default, no selection) or one of TLanguage values.
* Used by automatic language selection to store the last selected language
* code.
*/
const TUint32 KSysLangUtilSimLanguage = 0x1;

#endif // SYSLANGUTILPRIVATECRKEYS_H
