/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef FMTXDATA_
#define FMTXDATA_
// DATA TYPES

/**
* Possible FM Tx states
* 
@publishedPartner
@released
*/
enum TFmTxState
    {
    // Unknown HW error condition
    EFmTxStateUnknown   = 0x0, 
    // Transmitter HW is disabled 
    EFmTxStateOff       = 0x1, 
    // Transmitter HW is enabled.  Audio is currently routed to the transmitter.
    EFmTxStateActive    = 0x2, 
    // Transmitter HW is enabled, in standby mode.  Audio is not currently 
    // routed to the transmitter. 
    EFmTxStateInactive  = 0x4, 
	// Transmitter HW is disabled.  This occurs after a 
	// timeout period has elapsed, from EFmTxStateInactive mode.
    EFmTxStatePowerSaveInactivity = 0x8, 
	// Transmitter HW is disabled.  This occurs after a 
	// mutually exclusive accessory has been attached.
	// When accessory is detached, transition to EFmTxStateOff will occur.
    EFmTxStatePowerSaveAccessory = 0x10,     
    // Transmitter HW is enabled, and is scanning for the next clear frequency.			
    EFmTxStateScanning  = 0x20  
    };

/**
* Frequency range & step size (kHz)
@publishedPartner
@released
*/    
struct TFmTxFrequencyRange
	{
	TInt iMinFrequency;
	TInt iMaxFrequency;
	TInt iStepSize;
	};

/**
* Clear frequency search
@publishedPartner
@released
*/
const TInt KClearFrequencyArrayMax = 10;
typedef TFixedArray<TUint32,KClearFrequencyArrayMax> TClearFrequencyArray;

/**
 * RDS Programme Service
@publishedPartner
@released
 */
const TInt KMaxRdsPsLength = 8;
typedef TBuf<KMaxRdsPsLength> TRdsProgrammeService;

#ifndef EXCLUDE_RDS_PROGRAMME_TYPE_DEFS
/**
 * RDS (& RBDS) Programme Types. There are some differences in numbering of programme types
 * between these two standards.
@publishedPartner
@released
*/
typedef TInt TRdsProgrammeType;

/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyNone                = 0;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyNews                = 1;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyCurrentAffairs      = 2;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyInformation         = 3;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtySport               = 4;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyEducation           = 5;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyDrama               = 6;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyCulture             = 7;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyScience             = 8;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyVariedSpeech        = 9;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyPopMusic            = 10;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyRockMusic           = 11;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyEasyListening       = 12;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyLightClassical      = 13;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtySeriousClassical    = 14;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyOtherMusic          = 15;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyWeather             = 16;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyFinance             = 17;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyChildrensProgrammes = 18;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtySocialAffairs       = 19;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyReligion            = 20;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyPhoneIn             = 21;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyTravel              = 22;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyLeisure             = 23;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyJazzMusic           = 24;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyCountryMusic        = 25;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyNationalMusic       = 26;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyOldiesMusic         = 27;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyFolkMusic           = 28;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyDocumentary         = 29;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyAlarmTest           = 30;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRdsPtyAlarm               = 31;

/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyNone               = 0;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyNews               = 1;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyInformation        = 2;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtySports             = 3;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyTalk               = 4;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyRock               = 5;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyClassicRock        = 6;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyAdultHits          = 7;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtySoftRock           = 8;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyTop40              = 9;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyCountry            = 10;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyOldies             = 11;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtySoft               = 12;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyNostalgia          = 13;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyJazz               = 14;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyClassical          = 15;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyRhythmAndBlues     = 16;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtySoftRhythmAndBlues = 17;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyLanguage           = 18;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyReligiousMusic     = 19;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyReligiousTalk      = 20;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyPersonality        = 21;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyPublic             = 22;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyCollege            = 23;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyUnassigned1        = 24;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyUnassigned2        = 25;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyUnassigned3        = 26;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyUnassigned4        = 27;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyUnassigned5        = 28;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyWeather            = 29;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyEmergencyTest      = 30;
/**
*
* @publishedPartner
* @released
*/
const TRdsProgrammeType KRbdsPtyEmergency          = 31;

/**
 * RDS Programme Type Name.
*
* @publishedPartner
* @released
*/
const TUint8 KMaxRdsPtynLength = 8;
typedef TBuf<KMaxRdsPtynLength> TRdsProgrammeTypeName;

#endif // EXCLUDE_RDS_PROGRAMME_TYPE_DEFS

/**
 * RDS Language IDs.
*
* @publishedPartner
* @released
 */
typedef TInt TRdsLanguageIdType;

/**
* FM Tx RDS data
* These data items may set, using the HWRM FM Tx API
*
* @publishedPartner
* @released
*/
struct TFmTxRdsData
    {
    TRdsProgrammeType     iPty;        // Programme Type
    TRdsProgrammeTypeName iPtyn;       // Programme Type Name
    TBool                 iMs;         // Music/Speech flag
    TRdsLanguageIdType    iLanguageId; // Language ID
    };


/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageUnknown 		= 0x00; // Unknown/not applicable 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageAlbanian 		= 0x01; // Albanian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageBreton 		= 0x02; // Breton
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageCatalan 		= 0x03; // Catalan 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageCroatian 		= 0x04; // Croatian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageWelsh 			= 0x05; // Welsh 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageCzech 			= 0x06; // Czech 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageDanish 		= 0x07; // Danish
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageGerman 		= 0x08; // German 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageEnglish 		= 0x09; // English
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSpanish 		= 0x0A; // Spanish
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageEsperanto 		= 0x0B; // Esperanto 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageEstonian 		= 0x0C; // Estonian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageBasque 		= 0x0D; // Basque 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageFaroese 		= 0x0E; // Faroese 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageFrench 		= 0x0F; // French 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageFrisian 		= 0x10; // Frisian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageIrish 			= 0x11; // Irish 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageGaelic 		= 0x12; // Gaelic 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageGalician 		= 0x13; // Galician 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageIcelandic 		= 0x14; // Icelandic 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageItalian 		= 0x15; // Italian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageLappish 		= 0x16; // Lappish 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageLatin 			= 0x17; // Latin 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageLatvian 		= 0x18; // Latvian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageLuxembourgian 	= 0x19; // Luxembourgian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageLithuanian 	= 0x1A; // Lithuanian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageHungarian 		= 0x1B; // Hungarian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageMaltese 		= 0x1C; // Maltese 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageDutch 			= 0x1D; // Dutch 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageNorwegian 		= 0x1E; // Norwegian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageOccitan 		= 0x1F; // Occitan 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguagePolish 		= 0x20; // Polish
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguagePortuguese 	= 0x21; // Portuguese
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageRomanian 		= 0x22; // Romanian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageRomansh 		= 0x23; // Romansh
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSerbian 		= 0x24; // Serbian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSlovak 		= 0x25; // Slovak
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSlovene 		= 0x26; // Slovene 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageFinnish 		= 0x27; // Finnish
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSwedish 		= 0x28; // Swedish
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageTurkish 		= 0x29; // Turkish
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageFlemish 		= 0x2A; // Flemish
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageWalloon 		= 0x2B; // Walloon

/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageBackgroundSound = 0x40; // Background sound/Clean feed

/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageZulu 			= 0x45; // Zulu
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageVietnamese 	= 0x46; // Vietnamese
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageUzbek 			= 0x47; // Uzbek
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageUrdu 			= 0x48; // Urdu
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageUkrainian 		= 0x49; // Ukrainian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageThai 			= 0x4A; // Thai
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageTelugu 		= 0x4B; // Telugu
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageTatar	 		= 0x4C; // Tatar
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageTamil 			= 0x4D; // Tamil
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageTadzhik 		= 0x4E; // Tadzhik
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSwahili 		= 0x4F; // Swahili
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSrananTongo 	= 0x50; // Sranan Tongo
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSomali 		= 0x51; // Somali
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSinhalese 		= 0x52; // Sinhalese
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageShona 			= 0x53; // Shona
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageSerboCroat 	= 0x54; // Serbo-Croat
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageRuthenian 		= 0x55; // Ruthenian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageRussian 		= 0x56; // Russian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageQuechua 		= 0x57; // Quechua
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguagePushtu 		= 0x58; // Pushtu
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguagePunjabi 		= 0x59; // Punjabi
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguagePersian 		= 0x5A; // Persian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguagePapamiento 	= 0x5B; // Papamiento
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageOriya 			= 0x5C; // Oriya
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageNepali 		= 0x5D; // Nepali
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageNdebele 		= 0x5E; // Ndebele
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageMarathi 		= 0x5F; // Marathi
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageMoldavian 		= 0x60; // Moldavian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageMalaysian 		= 0x61; // Malaysian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageMalagasay 		= 0x62; // Malagasay 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageMacedonian 	= 0x63; // Macedonian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageLaotian 		= 0x64; // Laotian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageKorean 		= 0x65; // Korean 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageKhmer 			= 0x66; // Khmer 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageKazakh 		= 0x67; // Kazakh 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageKannada 		= 0x68; // Kannada 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageJapanese 		= 0x69; // Japanese
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageIndonesian 	= 0x6A; // Indonesian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageHindi 			= 0x6B; // Hindi 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageHebrew 		= 0x6C; // Hebrew
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageHausa 			= 0x6D; // Hausa 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageGurani 		= 0x6E; // Gurani
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageGujurati 		= 0x6F; // Gujurati 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageGreek 			= 0x70; // Greek 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageGeorgian 		= 0x71; // Georgian
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageFulani 		= 0x72; // Fulani 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageDari 			= 0x73; // Dari 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageChurash 		= 0x74; // Churash 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageChinese 		= 0x75; // Chinese 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageBurmese 		= 0x76; // Burmese 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageBulgarian 		= 0x77; // Bulgarian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageBengali 		= 0x78; // Bengali 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageBelorussian 	= 0x79; // Belorussian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageBambora 		= 0x7A; // Bambora 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageAzerbaijan 	= 0x7B; // Azerbaijan 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageAssamese 		= 0x7C; // Assamese 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageArmenian 		= 0x7D; // Armenian 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageArabic 		= 0x7E; // Arabic 
/**
*
* @publishedPartner
* @released
*/
const TRdsLanguageIdType KRdsLanguageAmharic 		= 0x7F; // Amharic 

/**
 *  Radiotext+ content type codes
*
* @publishedPartner
* @released
 */
enum TRtPlusContentType
    {
    ERtPlusItemTitle              = 0,
    ERtPlusItemAlbum              = 1,
    ERtPlusItemTrackNumber        = 2,
    ERtPlusItemArtist             = 3,
    ERtPlusItemComposition        = 4,
    ERtPlusItemMovement           = 5,
    ERtPlusItemConductor          = 6,
    ERtPlusItemComposer           = 7,
    ERtPlusItemBand               = 8,
    ERtPlusItemComment            = 9,
    ERtPlusItemGenre              = 10,
    ERtPlusInfoNews               = 11,
    ERtPlusInfoNewsLocal          = 12,
    ERtPlusInfoStockMarket        = 13,
    ERtPlusInfoSport              = 14,
    ERtPlusInfoLottery            = 15,
    ERtPlusInfoHoroscope          = 16,
    ERtPlusInfoDailyDiversion     = 17,
    ERtPlusInfoHealth             = 18,
    ERtPlusInfoEvent              = 19,
    ERtPlusInfoScene              = 20,
    ERtPlusInfoCinema             = 21,
    ERtPlusInfoTV                 = 22,
    ERtPlusInfoDateTime           = 23,
    ERtPlusInfoWeather            = 24,
    ERtPlusInfoAlarm              = 25,
    ERtPlusInfoAdvisertisement    = 26,
    ERtPlusInfoOther              = 27,
    ERtPlusStationNameLong        = 28,
    ERtPlusProgramNow             = 29,
    ERtPlusProgramNext            = 30,
    ERtPlusProgramPart            = 31,
    ERtPlusProgramHost            = 32,
    ERtPlusProgramEditorialStaff  = 33,
    ERtPlusProgramRadio           = 34,
    ERtPlusProgramHomepage        = 35,
    ERtPlusPhoneHotline           = 36,
    ERtPlusPhoneStudio            = 37,
    ERtPlusPhoneOther             = 38,
    ERtPlusSmsStudio              = 39,
    ERtPlusSmsOther               = 40,
    ERtPlusEmailHotline           = 41,
    ERtPlusEmailStudio            = 42,
    ERtPlusEmailOther             = 43,
    ERtPlusMmsOther               = 44,
    ERtPlusChat                   = 45,
    ERtPlusChatCenter             = 46,
    ERtPlusVoteQuestion           = 47,
    ERtPlusVoteCenter             = 48,
    ERtPlusOpenClass49            = 49,
    ERtPlusOpenClass50            = 50,
    ERtPlusOpenClass51            = 51,
    ERtPlusOpenClass52            = 52,
    ERtPlusOpenClass53            = 53,
    ERtPlusOpenClass54            = 54,
    ERtPlusOpenClass55            = 55,
    ERtPlusOpenClass56            = 56,
    ERtPlusOpenClass57            = 57,
    ERtPlusPlace                  = 58,
    ERtPlusAppointment            = 59,
    ERtPlusHotline                = 60,
    ERtPlusIdentifier             = 61,
    ERtPlusPurchase               = 62,
    ERtPlusGetData                = 63,
    ERtPlusCount
    };

/**
* Radiotext+ tag
*
* @publishedPartner
* @released
*/    
struct TRtPlusTag
    {
    TRtPlusContentType iContentType; // 0 to 63
    TInt iStartMarker;               // Start marker 0: means first character in the RT.
    TInt iLengthMarker;              // The length marker is ranging from 1 to 63.
    };

#endif /*FMTXDATA_*/
