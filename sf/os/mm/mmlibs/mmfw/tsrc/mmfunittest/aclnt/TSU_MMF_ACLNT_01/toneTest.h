// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _FONTTEST_H_
#define _FONTTEST_H_

// taken from tmdatest.h


class TLitPtrC8
	{
public:
	inline const TDesC8* operator&() const;
	inline operator const TDesC8&() const;
	inline const TDesC8& operator()() const;
	inline operator const __TRefDesC8() const;
public:
	TUint iTypeLength;
	const TUint8 *iPtr;
	};

// Template class TLitC8
inline const TDesC8* TLitPtrC8::operator&() const
	{return REINTERPRET_CAST(const TDesC8*,this);}
inline const TDesC8& TLitPtrC8::operator()() const
	{return *operator&();}
inline TLitPtrC8::operator const TDesC8&() const
	{return *operator&();}
inline TLitPtrC8::operator const __TRefDesC8() const
	{return *operator&();}

#define KNumBuf 5
#define _LIT8DATA(name,data) const static TLitPtrC8 name={(sizeof(data) | (1<<28)),REINTERPRET_CAST(const TUint8*,data)}

// Signature definitions
#define SEQ_SIG1 0x5153
#define SEQ_SIG2 0x434E	
#define SEQ_SIG	SEQ_SIG1,SEQ_SIG2, // Signature = "SQNC" ASCII

// Commands
#define SEQ_CMD_RET         -1
#define SEQ_CMD_STARTLOOP   -2,
#define SEQ_CMD_ENDLOOP     -3,

// Useful values
// Note frequencies
#define SEQ_FREQ_A0	     440
#define SEQ_FREQ_AsBf0   466  // 466.1637615 A#, Bb
#define SEQ_FREQ_B0	     494  // 493.8833013
#define SEQ_FREQ_C0	     523  // 523.2511306
#define SEQ_FREQ_CsDf0	 554  // 554.365262 C#, Db, etc.
#define SEQ_FREQ_D0	     587  // 587.3295358
#define SEQ_FREQ_DsEf0   622  // 622.2539674
#define SEQ_FREQ_E0      659  // 659.2551138
#define SEQ_FREQ_F0      698  // 698.4564629
#define SEQ_FREQ_FsGf0   740  // 739.9888454
#define SEQ_FREQ_G0      784  // 783.990872
#define SEQ_FREQ_GsAf0   831  // 830.6093952
#define SEQ_FREQ_A1      880
#define SEQ_FREQ_AsBf1   932  // 932.327523
#define SEQ_FREQ_B1      988  // 987.7666025
#define SEQ_FREQ_C1      1047 // 1046.502261
#define SEQ_FREQ_CsDf1   1109 // 1108.730524
#define SEQ_FREQ_D1      1175 // 1174.659072
#define SEQ_FREQ_DsEf1   1245 // 1244.507935
#define SEQ_FREQ_E1      1319 // 1318.510228
#define SEQ_FREQ_F1      1397 // 1396.912926
#define SEQ_FREQ_FsGf1   1480 // 1479.977691
#define SEQ_FREQ_G1      1568 // 1567.981744
#define SEQ_FREQ_GsAf1   1661 // 1661.21879
#define SEQ_FREQ_A2      1760
#define SEQ_FREQ_AsBf2   1865 // 1864.655046
#define SEQ_FREQ_B2      1976 // 1975.533205
#define SEQ_FREQ_C2      2093 // 2093.004522
#define SEQ_FREQ_CsDf2   2217 // 2217.461048
#define SEQ_FREQ_D2      2349 // 2349.318143
#define SEQ_FREQ_DsEf2   2489 // 2489.01587
#define SEQ_FREQ_E2      2673 // 2637.020455
#define SEQ_FREQ_F2      2794 // 2793.825851
#define SEQ_FREQ_FsGf2   2960 // 2959.955382
#define SEQ_FREQ_G2      3136 // 3135.963488
#define SEQ_FREQ_GsAf2   3322 // 3322.437581
#define SEQ_FREQ_A3      3520
#define SEQ_FREQ_AsBf3   3729 // 3729.310092

// Volume defines
#define SEQ_VOL_F        32767 // Forte = Loud
#define SEQ_VOL_MF	     24575 // Mezzo forte = Medium loud
#define SEQ_VOL_MP		 16383 // Mezzo piano = Medium soft
#define SEQ_VOL_P         8191 // Piano = Soft
	
// Macros for defining sequences
#define SEQ_DATA(A) SEQ_SIG A SEQ_CMD_RET
#define SEQ_VAL(N) N,
#define SEQ_LOOP(NUMBEROFTIMES,DATA) SEQ_CMD_STARTLOOP SEQ_VAL(NUMBEROFTIMES) DATA SEQ_CMD_ENDLOOP
#define SEQ_TONE(DUR,F1,V1) SEQ_VAL(DUR) SEQ_VAL(F1) SEQ_VAL(V1) SEQ_VAL(0) SEQ_VAL(0)
#define SEQ_2TONES(DUR,F1,V1,F2,V2) SEQ_VAL(DUR) SEQ_VAL(F1) SEQ_VAL(V1) SEQ_VAL(F2) SEQ_VAL(V2)
#define SEQ_SILENCE(DUR) SEQ_VAL(DUR) SEQ_VAL(0) SEQ_VAL(0) SEQ_VAL(0) SEQ_VAL(0)
// DUR = number samples@8kHz hence 8000 = 1 second
// F1/F2 = frequency in Hz - can use note definitions above
// V1/V1 = volume in range 0-32767 - can use volume definitions above
// When playing two tones, sum of volumes should not be greater than 32767
// e.g. can use MF+P or MP+MP

// end of rip from tmdatest.h
//
const TInt16 KFixedSequenceTestSequenceData[]=
    {
	SEQ_DATA
		(
		SEQ_LOOP
			(
			2, // Repeat twice
			SEQ_TONE(100,SEQ_FREQ_A0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_AsBf0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_B0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_C0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_CsDf0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_D0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_DsEf0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_E0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_F0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_FsGf0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_GsAf0,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_A1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_AsBf1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_B1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_C1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_CsDf1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_D1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_DsEf1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_E1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_F1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_FsGf1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_GsAf1,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_A2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_AsBf2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_B2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_C2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_CsDf2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_D2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_DsEf2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_E2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_F2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_FsGf2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_GsAf2,SEQ_VOL_F)
			SEQ_TONE(100,SEQ_FREQ_A3,SEQ_VOL_F)
			SEQ_SILENCE(6000)
			)
		SEQ_SILENCE(6000)
		)	
    };

_LIT8DATA(KFixedSequenceData,KFixedSequenceTestSequenceData);

#endif //_FONTTEST_H_
