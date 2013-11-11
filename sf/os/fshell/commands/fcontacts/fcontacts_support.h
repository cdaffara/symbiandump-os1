// fcontacts_support.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef FCONTACTS_SUPPORT_H
#define FCONTACTS_SUPPORT_H

#include <cntdef.hrh>

#ifndef KUidContactCardValue
// v9.1 didn't have these (the uids were used and understood, just these variables didn't exist)

#define KUidContactCardValue			0x10001309
#define KUidContactGroupValue			0x1000130A
#define KUidContactOwnCardValue			0x10004F95
#define KUidContactICCEntryValue		0x101F4A6F
#define KUidContactTemplateValue		0x1000130B
#define KUidContactCardTemplateValue	0x10004FF1

#define KUidContactFieldAssistantValue		0x10274DA4
#define KUidContactFieldAnniversaryValue	0x10274DA5
#define KUidContactFieldSpouseValue		0x10274DA6
#define KUidContactFieldChildrenValue		0x10274DA7
#define KUidContactFieldClassValue		0x10274DA8

#define KIntContactFieldVCardMapAssistant		0x10274DAA
#define KIntContactFieldVCardMapAssistantTel		0x10274DAB
#define KIntContactFieldVCardMapAnniversary		0x10274DAC
#define KIntContactFieldVCardMapSpouse			0x10274DAD

#endif // KUidContactCardValue

#ifndef KIntContactFieldVCardMapChildren
#define KIntContactFieldVCardMapChildren		0x10274DAE
#define KIntContactFieldVCardMapClass			0x10274DAF

// v9.1 didn't have these at all, but I think we only use them for sync support and 9.1 will happily just store them for us
const TUid KUidContactFieldVCardMapClass={KIntContactFieldVCardMapClass};
const TUid KUidContactFieldClass = { KUidContactFieldClassValue };

#endif // KIntContactFieldVCardMapChildren

#endif // FCONTACTS_SUPPORT_H
