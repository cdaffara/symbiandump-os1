
// MmfStdGlblAudioEffect.h

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

#ifndef MMFSTDGLBLAUDIOEFFECT_H
#define MMFSTDGLBLAUDIOEFFECT_H

#include <mmf/common/mmfglblaudioeffect.h>

/**
@publishedPartner
@released
@file
*/

/**
 Global Equaliser effect
 This provides access to the global equaliser
 */
class CMmfGlobalEqEffect : public CMmfGlobalAudioEffect
	{
public:
	/**
	Factory function
	@param Observer to use - can be NULL
	@return Freshly created object
	@leave KErrNotSupported
	       There is no low-level implementation for this class
	*/
	IMPORT_C static CMmfGlobalEqEffect* NewL(MMmfGlobalAudioEffectObserver* aObserver);
	
	/**
	Maximum number of bands an equaliser can support
	*/
	static const TInt KMaxBands = 10;
	
	/**
	Struct to form basis of package buffer to ExtractValuesL() and SetByValuesL()
	*/
	class TValues
		{
	public:
		TInt  iNumBands;			 		//*< Bands on current equaliser
		TReal iCurrentValue[KMaxBands]; //*< current setting
		TInt  iNominalFreq[KMaxBands];	//*< Nominal freq for each band, used in dialogs
		TReal iMin;				 		//*< Min possible value on each band
		TReal iMax;				 		//*< Max possible value on each band
		TVolumeValue iValueType; 		//*< How to interpet iCurrentValue, iMin and iMax
		};

private:
	CMmfGlobalEqEffect();
	void ConstructL(MMmfGlobalAudioEffectObserver* aObserver);
	};


/**
 Global Megaboost effect
 This provides access to the global megaboost.
 Note there is currently no TValues struct defined for this - client apps must use
 the preset interface.
 */
class CMmfGlobalMegaEffect : public CMmfGlobalAudioEffect
	{
public:
	/**
	Factory function
	@param Observer to use - can be NULL
	@return Freshly created object
	@leave KErrNotSupported
	       There is no low-level implementation for this class
	*/
	IMPORT_C static CMmfGlobalMegaEffect* NewL(MMmfGlobalAudioEffectObserver* aObserver);

private:
	CMmfGlobalMegaEffect();
	void ConstructL(MMmfGlobalAudioEffectObserver* aObserver);
	};



#endif // MMFSTDGLBLAUDIOEFFECT_H
