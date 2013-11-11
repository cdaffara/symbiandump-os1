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



#if (!defined __T_MM_DATA_IMAGEENCODER_H__)
#define __T_MM_DATA_IMAGEENCODER_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <imageconversion.h>

/**
 * Test Active Notification class
 *
 */
class CT_MMDataImageEncoder : public CDataWrapperBase
	{
public:
	~CT_MMDataImageEncoder();

	static	CT_MMDataImageEncoder*	NewL();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	virtual TAny*	GetObject();

	virtual void	SetObjectL(TAny* aObject);

	virtual void	DisownObjectL();

	void DestroyData();

	void RunL(CActive* aActive, TInt aIndex);
    void DoCancel(CActive* aActive, const TInt aIndex);

protected:
	CT_MMDataImageEncoder();

	void	ConstructL();

private:
	inline void				DoCmdDestructor();
	inline void				DoCmdCancel();
	inline void				DoCmdConvertL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void				DoCmdDataNewL(const TDesC& aSection);
	inline void				DoCmdFileNewL(const TDesC& aSection);
	inline void				DoCmdGetFileTypesL(const TDesC& aSection);
	inline void				DoCmdGetImageSubTypesL(const TDesC& aSection);
	inline void				DoCmdGetImageTypesL(const TDesC& aSection);
	inline void				DoCmdImplementationUid();
	inline void				DoCmdSetThumbnail(const TDesC& aSection);

	//	MTPActiveCallback implementation
	inline void				RunActiveL(const TInt aIndex);
	inline void				DoCancelActive(const TInt aIndex);

	//	Helper methods
	CImageEncoder::TOptions	ReadOptions(const TDesC& aSection);
	void					ReadUids(const TDesC& aSection, TUid& aImageType, TUid& aImageSubType, TUid& aEncoderUid);


private:
	RFbsSession					iFbsSession;
	TBool						iFbsSessionConnected;
	CImageEncoder*				iImageEncoder;
	CActiveCallback*			iActive;
	CFbsBitmap*					iBitmap;
	RImageTypeDescriptionArray	iImageTypeArray;
	RImageTypeDescriptionArray	iSubTypeArray;
	RFileExtensionMIMETypeArray	iFileExtensionArray;
	HBufC8*						iBuffer;
	RFile						iFile;
	TBool						iFileOpened;
	RFs							iFs;
	};

#endif /* __T_MM_DATA_IMAGEENCODER_H__ */
