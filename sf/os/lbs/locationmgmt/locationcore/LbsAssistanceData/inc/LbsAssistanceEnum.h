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



#ifndef LBSASSISTANCEENUM_H
#define LBSASSISTANCEENUM_H

/**
Satellite Status
@publishedPartner
@released
*/
enum TSatelliteStatus
	{
	/** ENzNNU */
	ENsNNU,
	/** EEsSN */
	EEsSN,
	/** EEsNNu */
	EEsNNU,
	/** ERev2 */
	ERev2,
	/** ERev */
	ERev
	};

/**
Doppler Uncertainty
@publishedPartner
@released
*/	
enum TDopplerUncertainty
	{
	/** EHz12_5 */
	EHz12_5,
	/** EHz25 */
	EHz25,
	/** EHz50 */
	EHz50,
	/** EHz100 */
	EHz100,
	/** Ehz200 */
	EHz200
	};

/**
Code Phase Search Window
@publishedPartner
@released
*/
enum TCodePhaseSearchWindow
	{
	/** EW1023 - This is the value for zero for RRC */
	EW1023 = 0,
	/** EW512 - This is the value for zero for RRLP */
	EW512 = 0,
	/** EW1 */
	EW1 = 1,
	/** EW2 */
	EW2 = 2,
	/** EW3 */
	EW3 = 3,
	/** EW4 */
	EW4 = 4,
	/** EW6 */
	EW6 = 5,
	/** EW8 */
	EW8 = 6,
	/** EW12 */
	EW12 = 7,
	/** EW16 */
	EW16 = 8,
	/** EW24 */
	EW24 = 9,
	/** EW32 */
	EW32 = 10,
	/** EW48 */
	EW48 = 11,
	/** EW64 */
	EW64 = 12,
	/** WE96 */
	EW96 = 13,
	/** EW128 */
	EW128 = 14,
	/** EW192 */
	EW192 = 15,
	};
	
/**
Sfn Tow Uncertainty
@publishedPartner
@released
*/
enum TSfnTowUncertainty
	{
	/** ELessThan10 */
	ELessThan10,
	/** EMoreThan10 */
	EMoreThan10
	};

/**
Utran Gps Drift Rate
@publishedPartner
@released
*/
enum TUtranGpsDriftRate
	{
	/** EUtranGPSDrift0 */
	EUtranGPSDrift0,
	/** EUtranGPSDrift1 */
	EUtranGPSDrift1,
	/** EUtranGPSDrift2 */
	EUtranGPSDrift2,
	/** EUtranGPSDrift5 */
	EUtranGPSDrift5,
	/** EUtranGPSDrift10 */
	EUtranGPSDrift10,
	/** EUtranGPSDrift15 */
	EUtranGPSDrift15,
	/** EUtranGPSDrift25 */
	EUtranGPSDrift25,
	/** EUtranGPSDrift50 */
	EUtranGPSDrift50,
	/** EUtranGPSDriftMinus1 */
	EUtranGPSDriftMinus1,
	/** EUtranGPSDriftMinus2 */
	EUtranGPSDriftMinus2,
	/** EUtranGPSDriftMinus */
	EUtranGPSDriftMinus5,
	/** EUtranGPSDriftMinus10 */
	EUtranGPSDriftMinus10,
	/** EUtranGPSDriftMinus15 */
	EUtranGPSDriftMinus15,
	/** EUtranGPSDriftMinus25 */
	EUtranGPSDriftMinus25,
	/** EUtranGPSDriftMinus50 */
	EUtranGPSDriftMinus50
	};

#endif //LBSASSISTANCEENUM_H

