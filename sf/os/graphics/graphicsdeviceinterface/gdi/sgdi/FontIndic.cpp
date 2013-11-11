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

/**
 @file
 @internalComponent
*/

#include "FontIndic.h"
#include <gdi.h>
#include "GlyphSel.h"
#include "ShapeInfo.h"
#include "gdiinline.inl"

// State machine for determining syllable boundaries.
// Key: D = Dependent vowel. V = Virama. N = Nukta
// I = Independent Vowel. S = Candrabindu/Anusvara/Visarga
// C = Consonant. J = zero-width joiner. X = Other
// A = Addak (double the following constant)
enum TSyllableState
	{
	EReject = -1,
	// Begin + I: add and move to IndependentVowel
	// Begin + C: add and move to Consonant
	// Begin + V/N/D/S/J/X: add and close syllable
	EBegin = 0,
	// Consonant + D: add and move to ConsonantVowel
	// Consonant + N: add
	// Consonant + V: add and move to Virama
	// Consonant + S: add and close syllable
	// Consonant + C/I/J/X: close syllable
	EConsonant = 1,
	// Consonant2 + D: add and move to ConsonantVowel
	// Consonant2 + N: add
	// Consonant2 + V: add and move to Virama2
	// Consonant2 + S: add and close syllable
	// Consonant2 + C/I/J/X: close syllable
	EConsonant2 = 2,
	// Consonant3 + D: add and move to ConsonantVowel
	// Consonant3 + N: add
	// Consonant3 + V: add and move to Virama3
	// Consonant3 + S: add and close syllable
	// Consonant3 + C/I/J/X: close syllable
	EConsonant3 = 3,
	// Consonant + D: add and move to ConsonantVowel
	// Consonant + N: add
	// Consonant + V: add and close syllable
	// Consonant + S: add and close syllable
	// Consonant + C/I/J/X: close syllable
	EConsonant4 = 4,
	// IndependentVowel + S: add
	// IndependentVowel + D/V/N/I/C/J/X: close syllable
	EIndependentVowel = 5,
	// Virama + C: add and move to Consonant
	// Virama + J: add and close syllable
	// Virama + N/V/D/I/S/X: close syllable
	EVirama = 6,
	// Virama2 + C: add and move to Consonant3
	// Virama2 + J: add and close syllable
	// Virama2 + N/V/D/I/S/X: close syllable
	EVirama2 = 7,
	// Virama3 + C: add and move to Consonant4
	// Virama3 + J: add and close syllable
	// Virama3 + N/V/D/I/S/X: close syllable
	EVirama3 = 8,
	// Virama4 + J: add and close syllable
	// Virama4 + CN/V/D/I/S/X: close syllable
	EVirama4 = 9,
	// ConsonantVowel + S: add
	// ConsonantVowel + D/V/N/I/C/J/X: close syllable
	EConsonantVowel = 10,
	// NuktaConsonant + D/V: add
	// NuktaConsonant + S/N/I/C/J/X: close syllable
	ENuktaConsonant = 11,
	// NuktaConsonant + D/V: add
	// NuktaConsonant + S/N/I/C/J/X: close syllable
	ENuktaConsonant2 = 12,
	// NuktaConsonant + D/V: add
	// NuktaConsonant + S/N/I/C/J/X: close syllable
	ENuktaConsonant3 = 13,
	// NuktaConsonant + D: add
	// NuktaConsonant + V/S/N/I/C/J/X: close syllable
	ENuktaConsonant4 = 14,
	// IndependentVowelA + CANDRA E: add
	// IndependentVowelA + anything else: close syllable
	EIndependentVowelA = 15,
	// ZeroWidthJoinerC + V: add
	// ZeroWidthJoinerC + C/S/N/I/J/X/D: close syllable
	EZeroWidthJoinerC = 16,
	// ZeroWidthJoinerA + CANDRA E: add
	// ZeroWidthJoinerA +anything else: close syllable
	EZeroWidthJoinerA = 17,
	// ZeroWidthJoinerV + C/N: add
	// ZeroWidthJoinerV + anything else: close syllable
	EZeroWidthJoinerV = 18,
	// ZeroWidthJoinerV2 + C/N: add
	// ZeroWidthJoinerV2 + anything else: close syllable
	EZeroWidthJoinerV2 = 19,
	// ZeroWidthJoinerV3 + C/N: add
	// ZeroWidthJoinerV3 + anything else: close syllable
	EZeroWidthJoinerV3 =20,
	// EKannadaRa + C/N/V/ZWJ(C): add
	EKannadaRa = 21,
	// ESplitVowel1 + SV2: add and close syllable
	// ESPlitVowel1 + anything else: close syllable
	ESplitVowel1 = 22,
	//EGurBearerA + EIndicCodeGurDependentVowelA: add and move to EIndependentVowel
	//EGurBearerA + modifier: add and close syllable
	//EGurBearerA + anything else: close syllable
	EGurBearerA = 23,
	//EGurBearerU + EIndicCodeGurDependentVowelU: add and move to EIndependentVowel
	//EGurBearerU + anything else: close syllable
	EGurBearerU = 24,
	//EGurBearerI + EIndicCodeGurDependentVowelI: add and move to EIndependentVowel
	//EGurBearerI + anything else: close syllable	
	EGurBearerI = 25,
	
	EConsonantMalaChillu = 26,
	EConsonantMalaChillu2 = 27,
	EConsonantMalaChillu3 = 28,
	EViramaMalaChillu = 29,
	EViramaMalaChillu2 = 30,
	EViramaMalaChillu3 = 31,
	EConsonantMalaChilluNA = 32,
	EConsonantMalaChilluNA2 = 33,
	EConsonantMalaChilluNA3 = 34,
	EViramaMalaChilluNA = 35,
	EViramaMalaChilluNA2 = 36,
	EViramaMalaChilluNA3 = 37,
	EZeroWidthJoinerMalaChilluNA = 38,
	EZeroWidthJoinerMalaChilluNA2 = 39,
	EZeroWidthJoinerMalaChilluNA3 = 40,
	// Add and close acceptances go here.
	EAccept = 41
	};

/** Defines the transitions for a state machine for determining syllable
boundaries. The machine starts in state 0. On receiving a character of type t
while in state s, moves to state KStateResponse[s][t]. */
const TSyllableState KStateResponse[42][24] =
	{
	// EBegin -- nothing added yet
	{EAccept, EAccept, EIndependentVowel, EConsonant, EAccept, EAccept, EAccept, EAccept, ENuktaConsonant, EIndependentVowelA, EAccept, EKannadaRa, EAccept, EAccept, EAccept, EGurBearerA, EGurBearerU, EGurBearerI, EAccept, EAccept, EAccept, EConsonantMalaChillu,EConsonantMalaChilluNA,EConsonant},
	// EConsonant -- seen a consonant either at the start or after virama
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama, ENuktaConsonant, EReject, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// EConsonant2 -- seen a consonant after virama
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama2, ENuktaConsonant2, EReject, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// EConsonant3 -- seen a consonant after virama2 
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama3, ENuktaConsonant3, EReject, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// EConsonant4 -- seen a consonant after virama3
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama4, ENuktaConsonant4, EReject, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// EIndependentVowel -- possibly nasalized. I haven't bothered to distinguish between
	// nasalization and visarga.
	{EReject, EAccept, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EConsonantVowel, EReject, EReject, EReject, EReject, EReject, EReject, EReject,EReject,EReject},
	// EVirama -- just seen a virama in a legal context. Must be consonant or ZWJ next
	{EReject, EReject, EReject, EConsonant2, EReject, EReject, EReject, EZeroWidthJoinerV, ENuktaConsonant2, EReject, EReject, EConsonant2, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EConsonantMalaChillu2,EConsonantMalaChilluNA2,EConsonant2},
	// EVirama2 -- just seen a virama in a legal context. Must be consonant2
	{EReject, EReject, EReject, EConsonant3, EReject, EReject, EReject, EZeroWidthJoinerV2, ENuktaConsonant3, EReject, EReject, EConsonant3, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EConsonantMalaChillu3,EConsonantMalaChilluNA3,EConsonant3},
	// EVirama3 -- just seen a virama in a legal context. Must be consonant3
	{EReject, EReject, EReject, EConsonant4, EReject, EReject, EReject, EZeroWidthJoinerV3, ENuktaConsonant4, EReject, EReject, EConsonant4, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EConsonant4,EConsonant4,EConsonant4},
	// EVirama4 -- just seen a virama in a legal context. Must be consonant4
	{EReject, EReject, EReject, EReject, EReject, EReject, EReject, EAccept, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject,EReject,EReject},
	// EConsonantVowel -- Just seen a consonant with matra. Can only nasalize or visarg
	{EReject, EAccept, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject,EReject,EReject},
	// ENuktaConsonant -- Just seen a consonant with Nukta. Can only accept a dependant vowel or a Virama at this stage.
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama, EReject, EReject, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// ENuktaConsonant2 -- Just seen a consonant with Nukta after a virama.
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama2, EReject, EReject, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// ENuktaConsonant3 -- Just seen a consonant with Nukta after Virama2.
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama3, EReject, EReject, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// ENuktaConsonant4 -- Just seen a consonant with Nukta after Virama3.
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama4, EReject, EReject, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// EIndependentVowelA -- Special case Devanagari independent vowel
	{EReject, EAccept, EReject, EReject, EReject, EReject, EReject, EZeroWidthJoinerA, EReject, EReject, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// EZeroWidthJoinerC -- Just seen a ZWJ after a consonant
	{EReject, EReject, EReject, EReject, EReject, EVirama, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject,EReject,EReject},
	// EZeroWidthJoinerA -- Just seen a ZWJ after the dependent Vowel A
	{EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EAccept, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject,EReject,EReject},
	// EZeroWidthJoinerV -- Just seen the ZWJ after the first pair of consonant + virama
	{EReject, EAccept, EReject, EConsonant2, EReject, EReject, EReject, EReject, ENuktaConsonant2, EReject, EReject, EConsonant2, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EConsonantMalaChillu2,EConsonantMalaChilluNA2,EConsonant2},
	// EZeroWidthJoinerV2 -- Just seen the ZWJ after consonant + virama + consonant + virama
	{EReject, EAccept, EReject, EConsonant3, EReject, EReject, EReject, EReject, ENuktaConsonant3, EReject, EReject, EConsonant3, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EConsonantMalaChillu3,EConsonantMalaChilluNA3,EConsonant3},
	// EZeroWidthJoinerV3 -- Just seen the ZWJ after consonant + virama + consonant + virama + consonant + virama
	{EReject, EAccept, EReject, EConsonant4, EReject, EReject, EReject, EReject, ENuktaConsonant4, EReject, EReject, EConsonant4, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EConsonant4,EConsonant4,EConsonant4},
	// EKannadaRa -- Special case consonant
	{EReject, EAccept, EReject, EReject, EConsonantVowel, EVirama, ENuktaConsonant, EZeroWidthJoinerC, EReject, EReject, EConsonantVowel, EReject, ESplitVowel1, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EConsonantVowel, EReject,EReject,EReject},
	// ESplitVowel1 -- Just seen a consonant with first part of a split vowel. Can only nasalize, visarg or add second part of splt vowel
	{EReject, EAccept, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EConsonantVowel, EConsonantVowel, EReject, EReject, EReject, EReject, EReject, EReject, EReject,EReject,EReject},
	// EGurBearerA -- Just seen a dependent Vowel EIndicCodeGurDependentVowelA and modifier
	{EReject, EAccept, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EIndependentVowel, EReject, EReject, EReject,EReject,EReject},
	// EGurBearerU -- Just seen a dependent Vowel EIndicCodeGurDependentVowelU
	{EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EIndependentVowel, EReject, EReject,EReject,EReject},
	// EGurBearerI -- Just seen a dependent Vowel EIndicCodeGurDependentVowelI
	{EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EIndependentVowel, EReject,EReject,EReject},
	
	{EReject,EAccept,EReject,EReject,EConsonantVowel,EViramaMalaChillu,EReject,EReject,EReject,EReject,EReject,EReject,ESplitVowel1,EConsonantVowel,EConsonantVowel,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject},
	{EReject,EAccept,EReject,EReject,EConsonantVowel,EViramaMalaChillu2,EReject,EReject,EReject,EReject,EReject,EReject,ESplitVowel1,EConsonantVowel,EConsonantVowel,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject},
	{EReject,EAccept,EReject,EReject,EConsonantVowel,EViramaMalaChillu3,EReject,EReject,EReject,EReject,EReject,EReject,ESplitVowel1,EConsonantVowel,EConsonantVowel,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject},
	{EReject,EReject,EReject,EConsonant2,EReject,EReject,EReject,EAccept,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonantMalaChillu2,EConsonantMalaChilluNA2,EConsonant2},
	{EReject,EReject,EReject,EConsonant3,EReject,EReject,EReject,EAccept,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonantMalaChillu3,EConsonantMalaChilluNA3,EConsonant3},
	{EReject,EReject,EReject,EConsonant4,EReject,EReject,EReject,EAccept,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonant4,EConsonant4,EConsonant4},
	{EReject,EAccept,EReject,EReject,EConsonantVowel,EViramaMalaChilluNA,EReject,EReject,EReject,EReject,EReject,EReject,ESplitVowel1,EConsonantVowel,EConsonantVowel,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject},
	{EReject,EAccept,EReject,EReject,EConsonantVowel,EViramaMalaChilluNA2,EReject,EReject,EReject,EReject,EReject,EReject,ESplitVowel1,EConsonantVowel,EConsonantVowel,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject},
	{EReject,EAccept,EReject,EReject,EConsonantVowel,EViramaMalaChilluNA3,EReject,EReject,EReject,EReject,EReject,EReject,ESplitVowel1,EConsonantVowel,EConsonantVowel,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject},
	{EReject,EReject,EReject,EConsonant2,EReject,EReject,EReject,EZeroWidthJoinerMalaChilluNA,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonantMalaChillu2,EConsonantMalaChilluNA2,EConsonant2},
	{EReject,EReject,EReject,EConsonant3,EReject,EReject,EReject,EZeroWidthJoinerMalaChilluNA2,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonantMalaChillu3,EConsonantMalaChilluNA3,EConsonant3},
	{EReject,EReject,EReject,EConsonant4,EReject,EReject,EReject,EZeroWidthJoinerMalaChilluNA3,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonant4,EConsonant4,EConsonant4},
	{EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonant2},
	{EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonant3},
	{EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EReject,EConsonant4},
	
	// EAccept -- this one is added but anything further is rejected
	{EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject, EReject}
	};

TIndicCodeType CodeType(TInt aCode, const TIndicCodeMapping aCodeMap[], TInt aMapLen)
	{
	TInt start = 0;
	TInt end = aMapLen - 1;
	while (start != end)
		{
		const TInt mid = (start + end + 1) >> 1;
		if (aCodeMap[mid].iFirstCodeInRange <= aCode)
			start = mid;
		else
			end = mid - 1;
		}
	return static_cast<TIndicCodeType>(aCodeMap[start].iCodeType);
	}

const TText16* SyllableEnd(const TText16* aCurrentPosition,
	const TText16* aEnd, const TIndicCodeMapping aCodeMap[], TInt aMapLen)
	{
	TSyllableState state = EBegin;
	while (aCurrentPosition != aEnd)
		{
		const TIndicCodeType ct = CodeType(*aCurrentPosition, aCodeMap, aMapLen);
		state = KStateResponse[state][ct];
		if (state == EReject)
			return aCurrentPosition;
		++aCurrentPosition;
		}
	return aCurrentPosition;
	}

/**
Common function for all Indic scripts. 
Performs the shaping operation on Indic script text up to KMaxTextToShape.
*/	
TInt IndicShapingProcess(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo, 
						const TIndicCodeMapping aCodeMap[], TInt aMapLen, const TUint aHighChar, 
						const TUint aLowChar, const TUint32 aScript, const TUint32 aLang)
	{
	const TText16* startCharacter = aGss.iParam.iText.Ptr();
	const TText16* endCharacter = startCharacter + aGss.iParam.iText.Length();
	const TText16* currentCharacter = aGss.iText.CurrentPosition();
	TInt currentPosition = currentCharacter - startCharacter;

	// Do some new shaping
	
	// First close the existing RShapeInfo
	if(aShapeInfo.IsOpen())
		aShapeInfo.Close();
	
	// Find the end of Indic text whilst limiting the amount of text to be shaped to 
	// KMaxTextToShape. When finding the end of end position of the text to be shaped,
	// include numerals, punctuations and other control characters.
	const TText16* endIndic = currentCharacter;
	TChar endIndicChar = *endIndic;
	while((endIndic <= endCharacter || endIndic - currentCharacter <= KMaxTextToShape) &&
		 ((*endIndic >= aLowChar && *endIndic <= aHighChar) || 
		 	((endIndicChar.IsDigit() || endIndicChar.IsPunctuation()) && !QuoteOrBracketPair(*endIndic))))
		{
		if (endIndic == endCharacter || endIndic - currentCharacter > KMaxTextToShape)
			break;
		endIndic = SyllableEnd(endIndic, endCharacter, aCodeMap, aMapLen);	
		endIndicChar = *endIndic;
		}
	
	TInt endPosForShaping = endIndic - startCharacter;
	
	//Now shape from current position to the end of Hindi
	return aShapeInfo.Open(aGss.iFont, aGss.iParam.iText,
						currentPosition, endPosForShaping, aScript, aLang);
	}

/**
Common function for all Indic scripts. Adds the shaped glyphs to the glyph cluster.
This function is called regardless of whether shaping was done or skipped in this 
iteration of the Default Process
*/
TBool IndicPostShapingProcess(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo, 
							TInt aCurrPos, TInt aSyllEndPos, const TText16* aSyllableEnd)
	{
	// Now get all the shaped values
	// Get the number of glyphs, glyphs, positions and indices after shaping.
	TInt numGlyphs = aShapeInfo.GlyphCount();
	const TInt32* glyph = aShapeInfo.Glyphs();
	const RShapeInfo::TPoint16* position = aShapeInfo.GlyphPositions();
	const TInt16* indices = aShapeInfo.Indices();
	TPoint origin;
	
	// Shift the indices and positions until the current poisition is reached. This done so that the 
	// origin can be set back to the original co-ordinates at the start of this Hindi text
	while (*indices < aCurrPos)
		{
		position++;
		indices++;
		}
	// Set the origin back by subtracting the current position co-ordinates from the current pen position	
	origin.iX = aGss.iParam.iPen.iX - position->iX;
	origin.iY = aGss.iParam.iPen.iY - position->iY;
	
	// No re-initialise the indices and positions to start appending the glyphs to clusters from the beginning
	indices = aShapeInfo.Indices();
	position = aShapeInfo.GlyphPositions();
	
	// Now do some post shaping syllable chopping
	for (TInt i = 0; i < numGlyphs; i++, glyph++, position++, indices++)
		{
		if(*indices >= aSyllEndPos)
			break;
		if(*indices >= aCurrPos && *indices < aSyllEndPos)
			{
			aGss.iParam.iPen.iX = origin.iX + position->iX;
			aGss.iParam.iPen.iY = origin.iY + position->iY;
			
			//Just skip the non-existing glyph and continue handling other glyphs if AppendGlyphToCluster return false.
			// The "continue" statement is for avoiding the potential risk of handling the non-existing glyph.
			if (!aGss.AppendGlyphToCluster(0x80000000UL | *glyph))
				continue;
			}
		}
	
	// We'll do pen advance ourselves, thanks.
	aGss.iPen = TGlyphSelectionState::EPenAdvance_No;
	aGss.iParam.iPen.iX = origin.iX + position->iX;
	aGss.iParam.iPen.iY = origin.iY + position->iY;
	aGss.iClusterState = TGlyphSelectionState::EGClusterComplete;
	aGss.iText.SetCurrentPosition(aSyllableEnd);
	return ETrue;
	}

/**
Common function for all Indic scripts.
Performs shaping and Rasterizing operation on Indic script text 
*/
TBool DoProcess(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo, const TIndicScriptProcessorParameters& aScriptSpecificParameters){
	const TText16* startCharacter = aGss.iParam.iText.Ptr();
	const TText16* endCharacter = startCharacter + aGss.iParam.iText.Length();
	const TText16* currentCharacter = aGss.iText.CurrentPosition();
	TInt currentPosition = currentCharacter - startCharacter;
		
	// Get the logical end of the syllable from the input text. We will use this later to chop
	// syllables from the glyph values we get.
	const TText16* syllableEnd = SyllableEnd(currentCharacter, endCharacter, aScriptSpecificParameters.iCodeMap,
			aScriptSpecificParameters.iMapLen);
	TInt syllableEndPos = syllableEnd - startCharacter;
			
	TInt ret = KErrNone;
	if(currentPosition >= aShapeInfo.EndOfShapedText())	
		ret = IndicShapingProcess(aGss, aShapeInfo, aScriptSpecificParameters.iCodeMap, aScriptSpecificParameters.iMapLen, 
				aScriptSpecificParameters.iUCHigh, aScriptSpecificParameters.iUCLow, aScriptSpecificParameters.iScriptCode, aScriptSpecificParameters.iLanguageCode);
		
	// If an RShapeInfo could not be opened, call the default process
	if (ret != KErrNone)
		return GlyphSelector_Default::Process(aGss, aShapeInfo);	
	
	// Now call the post shaping process to add glyphs to the glyph cluster
	return IndicPostShapingProcess(aGss, aShapeInfo, currentPosition, syllableEndPos, syllableEnd);
}

/*************************************************** D E V A N A G A R I ***************************************************/
TBool GlyphSelector_Devanagari::Process(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo)
	{
	return DoProcess(aGss, aShapeInfo, KIndicDevParameters);
	}
	
/***************************************************** K A N N A D A ******************************************************/
TBool GlyphSelector_Kannada::Process(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo)
	{
	return DoProcess(aGss, aShapeInfo, KIndicKanParameters);
	}

/***************************************************** G U J A R A T I ******************************************************/
TBool GlyphSelector_Gujarati::Process(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo)
	{
	return DoProcess(aGss, aShapeInfo, KIndicGujParameters);
	}

/***************************************************** B E N G A L I ******************************************************/
TBool GlyphSelector_Bengali::Process(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo)
	{
	return DoProcess(aGss, aShapeInfo, KIndicBenParameters);
	}

/***************************************************** T A M I L ******************************************************/
TBool GlyphSelector_Tamil::Process(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo)
	{
	return DoProcess(aGss, aShapeInfo, KIndicTamParameters);
	}

/***************************************************** T E L U G U ******************************************************/
TBool GlyphSelector_Telugu::Process(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo)
	{
	return DoProcess(aGss, aShapeInfo, KIndicTelParameters);
	}

/***************************************************** G U R M U K H I ******************************************************/
TBool GlyphSelector_Gurmukhi::Process(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo)\
	{
	return DoProcess(aGss, aShapeInfo, KIndicGurParameters);
	}

/***************************************************** M A L A Y A L A M ******************************************************/
TBool GlyphSelector_Malayalam::Process(TGlyphSelectionState& aGss, RShapeInfo& aShapeInfo)
	{
	return DoProcess(aGss, aShapeInfo, KIndicMalParameters);
	}

