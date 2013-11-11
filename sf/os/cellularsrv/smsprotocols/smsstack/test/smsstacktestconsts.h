// Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SMSSTACKTESTCONSTS_H
#define SMSSTACKTESTCONSTS_H

// Location and name of the RSC file.
_LIT(KSMSUResourceDir, "C:\\private\\101f7989\\sms\\");
_LIT(KSMSUResourceFile, "C:\\private\\101f7989\\sms\\smsu.rsc");

// Here are the section names
_LIT8(KSetupTelNumbers,           "Defaults");
_LIT8(KTelefoneNumber,            "TelephoneNumber");

// Here are the item names
_LIT8(KServiceCenter,               "ServiceCenter");

_LIT(KGmsSmsConfigFileName,"setupgsmsms.txt");
_LIT(KTSmsPrtConfigFileName, "tsmsprt_config.txt");
_LIT(KTSmsPrtConfigExtraFileName, "tsmsprt_config_extra.txt");
_LIT(KTSmsPrtConfigTxRxMaxMsgFileName, "tsmsprt_config_tx_rx_maxmsg.txt");

_LIT8(KServiceCentreAddress,   "ServiceCentreAddress");
_LIT8(KSmspEntryValidParams,  "SmspEntryValidParams");
_LIT8(KSmspEntryScTypeOfNumber,  "SmspEntryScTypeOfNumber");
_LIT8(KSmspEntryScNumberPlan,  "SmspEntryScNumberPlan");

_LIT(KVodafoneSC,"+447785016005"); //Vodafone SC
_LIT(KRegTestNumber,"+447747065548"); //Number used with regtest
_LIT(KTestNumber,"+447747065825"); //Number used with regtest

_LIT(KSoneraSC,"+358405202000"); //Sonera SC
_LIT(KRadiolinjaSC,"+358508771010"); //Radiolinja SC

_LIT(KPekka,"+358408415528");  //Pekka's telephone number
_LIT(KOther,"+358408415582"); // Other test number
_LIT(KLocalNumber, "08408415528"); //Local test number to verify local/international switching
_LIT(KInternationalTestNumber, "+819021710979"); // International number to be used with a status
                                                 // report containing local version of this number

_LIT8(KTestSendAndRecvMsgsWithDifferentTON, "SendAndRecvMsgsWithDifferentTON");
_LIT8(KTestSendAndReceiveIndicatorMsgs, "SendAndReceiveIndicatorMsgs");
_LIT(KConfigFileDir,"sms");
_LIT8(KAlphabet,    "Alphabet");
_LIT8(KTypeOfNumber,    "TypeOfNumber");
_LIT8(KIndicationType,  "IndicationType");
_LIT8(KDCSBits7To4, "DCSBits7To4");
_LIT8(KIndicationState, "IndicationState");

_LIT(KText, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123");

_LIT(KTestCaseNumber, "testNumber"); ///< test number run by SIMTSY

const TUint KSocketMessageSlots = 16;   // Override the default value of 8

#endif // SMSSTACKTESTCONSTS_H
