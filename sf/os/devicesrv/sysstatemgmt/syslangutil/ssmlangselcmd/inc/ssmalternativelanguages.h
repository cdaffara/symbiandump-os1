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
* Alternative languages definitions.
*
*/

#ifndef SSMTALTERNATIVELANGUAGES_H
#define SSMTALTERNATIVELANGUAGES_H

#include <e32const.h>

/** Pair of alternative languages: languages which can substitute each other. */
class TSsmAlternativeLanguagePair
    {
public:
    TInt iLang1;
    TInt iLang2;
    };

/** Number of alternative language pairs in the alternative language table. */
static const TInt KSsmAlternativeLanguageTableSize( 14 );

/** Alternative language table. */
static const TSsmAlternativeLanguagePair
KAlternativeLanguageTable[KSsmAlternativeLanguageTableSize] =
    {
        { ELangEnglish,     ELangAmerican },
        { ELangEnglish,     ELangEnglish_Apac },
        { ELangEnglish,     ELangEnglish_Taiwan },
        { ELangEnglish,     ELangEnglish_HongKong },
        { ELangEnglish,     ELangEnglish_Prc },
        { ELangEnglish,     ELangEnglish_Japan },
        { ELangEnglish,     ELangEnglish_Thailand },
        { ELangMalay,       ELangMalay_Apac },
        { ELangIndonesian,  327 }, // APAC Indonesian - no enum value available
        { ELangFrench,      ELangCanadianFrench },
        { ELangSpanish,     ELangLatinAmericanSpanish },
        { ELangPortuguese,  ELangBrazilianPortuguese },
        { ELangPrcChinese,  ELangTaiwanChinese },
        { ELangPrcChinese,  ELangHongKongChinese }
    };

#endif // SSMTALTERNATIVELANGUAGES_H
