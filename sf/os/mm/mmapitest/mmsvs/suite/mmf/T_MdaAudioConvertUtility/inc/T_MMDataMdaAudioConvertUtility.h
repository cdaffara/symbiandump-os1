/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#if (!defined __T_MM_DATA_MDA_AUDIO_CONVERT_UTILITY_H__)
#define __T_MM_DATA_MDA_AUDIO_CONVERT_UTILITY_H__

//	User Includes
#include "DataWrapperBase.h"
//	EPOC includes
#include <mdaaudiosampleeditor.h>
#include <audio.h>
#include <mmfcontrollerpluginresolver.h>
const TInt KMaxdataFromText = 255;

// Temp fix for conditional building and testing of UseSharedHeap
#if (defined SYMBIAN_SWI_POST_INSTALL_REVOCATION)
#define CMDA_AUDIO_CONVERT_UTILITY_USED_SHARED_HEAP
#endif

/**
CMdaAudioConvertUtility API test commands
@internalComponent
*/
class CT_MMDataMdaAudioConvertUtility:public CDataWrapperBase ,public MMdaObjectStateChangeObserver
	{
public:
	static CT_MMDataMdaAudioConvertUtility*	NewL();

	~CT_MMDataMdaAudioConvertUtility();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_MMDataMdaAudioConvertUtility();

	void	ConstructL();

private:
	inline	void			DoCmdConstructor(const TDesC& aSection);
	inline 	void			DoCmdOpenL(const TDesC& aSection);
	inline 	void			DoCmdConvertL(const TDesC& aSection);
	inline	void			DoCmdSourceDataTypeL(const TDesC& aSection);
	inline	void			DoCmdGetSupportedDestDataTypesL(const TDesC& aSection);
	inline	void 			DoCmdSetDestinationDataTypeL(const TDesC& aSection);
	inline	void 			DoCmdDestinationDataTypeL(const TDesC& aSection);
	inline	void 			DoCmdSourceBitRateL(const TDesC& aSection);
	inline	void 			DoCmdGetSupConversionBitRatesL(const TDesC& aSection);
	inline	void 			DoCmdDestinationBitRateL(const TDesC& aSection);
	inline	void 			DoCmdSetDestinationBitRateL(const TDesC& aSection);
	inline	void 			DoCmdSourceSampleRateL(const TDesC& aSection);
	inline	void 			DoCmdGetSupConvSampleRatesL(const TDesC& aSection);
	inline	void 			DoCmdDestinationSampleRateL(const TDesC& aSection);
	inline	void 			DoCmdSetDestinationSampleRateL(const TDesC& aSection);
	inline	void 			DoCmdSourceFormatL(const TDesC& aSection);
	inline	void 			DoCmdDestinationFormatL(const TDesC& aSection);
	inline	void 			DoCmdSetDestinationFormatL(const TDesC& aSection);
	inline	void 			DoCmdSourceNumberOfChannelsL(const TDesC& aSection);
	inline	void 			DoCmdGetSupConvNrOfChnlsL(const TDesC& aSection);
	inline	void 			DoCmdDestinationNrOfChannelsL(const TDesC& aSection);
	inline	void 			DoCmdSetDestinationNrOfChnlsL(const TDesC& aSection);
	inline	void 			DoCmdState(const TDesC& aSection);
	inline	void 			DoCmdClose();
	inline	void 			DoCmdPlayL();
	inline	void 			DoCmdRecordL();
	inline	void 			DoCmdStop();
	inline	void 			DoCmdPosition(const TDesC& aSection);
	inline	void 			DoCmdSetPosition(const TDesC& aSection);
	inline	void 			DoCmdDuration(const TDesC& aSection);
	inline	void 			DoCmdRecordTimeAvailable(const TDesC& aSection);
	inline	void 			DoCmdCropL();
	inline	void 			DoCmdSetPlayWindow(const TDesC& aSection);
	inline	void 			DoCmdClearPlayWindow();
	inline	void 			DoCmdSetMaxWriteLength(const TDesC& aSection);
	inline	void 			DoCmdCropFromBeginningL();
	inline	void 			DoCmdControllerImplInfoL();
	inline	void 			DoCmdCustomCommandSyncL(const TDesC& aSection);
	inline	void 			DoCmdCustomCommandAsyncL(const TDesC& aSection);
	inline	void 			DoCmdSetThreadPriority(const TDesC& aSection);
	inline	void			DoCmdSetRepeats(const TDesC& aSection);
	inline	void			DoCmdUseSharedHeap();
	inline	void 			DoCmdDestructor();

	// from the MMdaObjectStateChangeObserverInterface
	virtual void 			MoscoStateChangeEvent(CBase *aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

	//	Helper methods
	void					DestroyData();
	TBool 					GetFormatL(const TDesC& aFormatStr, TMdaClipFormat* &aMdaClipFormat, const TDesC& aSection);
	TBool  					GetCodecL(const TDesC& aCodecStr, TMdaPackage* &aMdaPackage);
	const TDesC& 			GetState(const TInt& aState);
	void					GetAudioSettingsL(const TDesC& aAudioSettingsStr, CDataWrapperBase& aTestStep, const TDesC& aSection, TMdaPackage* &aMdaPackage);

	//	MT_MMActiveCallback implementation
 	virtual void			RunL(CActive* aActive,const TInt aAsyncErrorIndex);
 	virtual void			DoCancel(CActive* aActive);
 	inline void				RunCustomCommandAsyncL(const TInt aAsyncErrorIndex);
 	inline void				DoCancelCustomCommandAsync();

private:
	CMdaAudioConvertUtility*	iConvertUtility;
	CMdaServer* 				iServer;
	TUid						iAudioControllerUid;
	TUid						iAudioFormatUid;
	CActiveCallback*			iActiveCustomCommandAsync;
	TBuf8<KMaxdataFromText>		iDataFromText;
	TInt 						iAsyncErrorIndex;
	TInt						iOutstandingRepeats;
	};

#endif /* __T_MM_DATA_MDA_AUDIO_CONVERT_UTILITY_H__ */
