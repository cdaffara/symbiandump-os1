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



#if (!defined __T_MM_DATA_IMAGEDECODER_H__)
#define __T_MM_DATA_IMAGEDECODER_H__

//	EPOC includes
#include <imageconversion.h>

// User include
#include "DataWrapperBase.h"

/**
 * Test Active Notification class
 *
 */
class CT_MMDataImageDecoder : public CDataWrapperBase
	{
private:
	enum TState
		{
		EState_Unknown,
		EState_Convert,
		};

public:
	~CT_MMDataImageDecoder();

	static	CT_MMDataImageDecoder*	NewL();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	virtual TAny*	GetObject();

	virtual void	SetObjectL(TAny* aObject);

	virtual void	DisownObjectL();

	void DestroyData();

	void RunL(CActive* aActive, TInt aIndex);
	void DoCancel(CActive* aActive, TInt aIndex);

protected:
	CT_MMDataImageDecoder();

	void	ConstructL();

private:
	inline void				DoCmdDestructor();
	inline void				DoCmdCancel();
	inline void				DoCmdContinueConvert(const TInt aAsyncErrorIndex);
	inline void				DoCmdContinueProcessingHeaderL();
	inline void				DoCmdConvertL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void				DoCmdDataNewL(const TDesC& aSection);
	inline void				DoCmdFileNewL(const TDesC& aSection);
	inline void				DoCmdFrameCommentL(const TDesC& aSection);
	inline void				DoCmdFrameCount(const TDesC& aSection);
	inline void				DoCmdFrameData(const TDesC& aSection);
	inline void				DoCmdFrameInfo(const TDesC& aSection);
	inline void				DoCmdFrameInfoStringsL(const TDesC& aSection);
	inline void				DoCmdFrameInfoStringsLCApi(const TDesC& aSection);
	inline void				DoCmdGetFileTypesL(const TDesC& aSection);
	inline void				DoCmdGetImageSubTypesL(const TDesC& aSection);
	inline void				DoCmdGetImageTypesL(const TDesC& aSection);
	inline void				DoCmdGetImplementationInformationL(const TDesC& aSection);
	inline void				DoCmdGetMimeTypeDataL(const TDesC& aSection);
	inline void				DoCmdGetMimeTypeFileL(const TDesC& aSection);
	inline void				DoCmdImageCommentL(const TDesC& aSection);
	inline void				DoCmdImageType(const TDesC& aSection);
	inline void				DoCmdImplementationUid();
	inline void				DoCmdIsImageHeaderProcessingComplete(const TDesC& aSection);
	inline void				DoCmdNumberOfFrameComments(const TDesC& aSection);
	inline void				DoCmdNumberOfImageComments(const TDesC& aSection);
	inline void				DoCmdReducedSize(const TDesC& aSection);
	inline void				DoCmdReductionFactor(const TDesC& aSection);
	inline void				DoCmdSetAgentProperty(const TDesC& aSection);
	inline void				DoCmdSetImageTypeL(const TDesC& aSection);

	//	MTPActiveCallback implementation
	inline void				RunActiveL(TInt aIndex);
	inline void				DoCancelActive();

	//	Helper methods
	CImageDecoder::TOptions	ReadOptions(const TDesC& aSection);
	void					ReadUids(const TDesC& aSection, TUid& aImageType, TUid& aImageSubType, TUid& aEncoderUid);


private:
	CImageDecoder*					iImageDecoder;
	RFbsSession						iFbsSession;
	TBool							iFbsSessionConnected;
	TState							iState;
	CActiveCallback*				iActive;
	CFbsBitmap*						iBitmap;
	CFbsBitmap*						iBitmapMask;
	CFrameInfoStrings*				iFrameInfoStrings;
	RImageTypeDescriptionArray		iImageTypeArray;
	RImageTypeDescriptionArray		iSubTypeArray;
	RFileExtensionMIMETypeArray		iFileExtensionArray;
	CImplementationInformationType*	iImplementationInformationType;
	RFile							iFile;
	TBool							iFileOpened;
	TFrameInfo						iFrameInfo;
	TPtrC							iBitmapFile;
	TBool							iHasBitmapFile;
	TBool                           iAsyncConvertFailed;
	HBufC8*							iDataSource;
	RFs								iFs;
	};

#endif /* __T_MM_DATA_IMAGEDECODER_H__ */
