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



#if (!defined __T_MM_DATA_MDA_AUDIO_OUTPUT_STREAM_H__)
#define __T_MM_DATA_MDA_AUDIO_OUTPUT_STREAM_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <mdaaudiooutputstream.h>

/**
 * Test Active Notification class
 *
 */
class CT_MMDataMdaAudioOutputStream : public CDataWrapperBase, private MMdaAudioOutputStreamCallback, private MMMFAudioResourceNotificationCallback
	{
public:
	~CT_MMDataMdaAudioOutputStream();

	static	CT_MMDataMdaAudioOutputStream*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_MMDataMdaAudioOutputStream();

	void	ConstructL();

private:
	inline void				DoCmdDestructor();
	inline void				DoCmdCancelRegisterAudioResourceNotification();
	inline void				DoCmdCustomInterface();
	inline void				DoCmdSetBalanceL(const TDesC& aSection);
	inline void				DoCmdGetBalanceL(const TDesC& aSection);
	inline void				DoCmdGetBytes(const TDesC& aSection);
	inline void				DoCmdNewL(const TDesC& aSection);
	inline void				DoCmdOpen(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void				DoCmdPosition();
	inline void				DoCmdRegisterAudioResourceNotification();
	inline void				DoCmdSetAudioPropertiesL(const TDesC& aSection);
	inline void				DoCmdSetDataTypeL(const TDesC& aSection);
	inline void				DoCmdDataTypeL(const TDesC& aSection);
	inline void				DoCmdSetPriority(const TDesC& aSection);
	inline void				DoCmdSetVolume(const TDesC& aSection);
	inline void				DoCmdMaxVolume();
	inline void				DoCmdVolume(const TDesC& aSection);
	inline void				DoCmdStop();
	inline void				DoCmdWillResumePlay();
	inline void				DoCmdWriteL(const TDesC& aSection, const TInt aAsyncErrorIndex);

	//	MMdaAudioOutputStreamCallback implementation
	virtual void			MaoscOpenComplete(TInt aError);
	virtual void			MaoscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void			MaoscPlayComplete(TInt aError);

	//	MMMFAudioResourceNotificationCallback implementation
	virtual void			MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData);

	//	Helper methods
	TBool					WriteStreamAudioDataL();
	void					DestroyData();


private:
	CMdaAudioOutputStream*	iOutputStream;
	HBufC8*					iBuffer;
	TPtr8*					iBufferPtr;
	TInt					iBufferSize;
	TBool					iStreamOpened;
	RFile					iFile;
	TBool					iFileOpened;
	TBool					iIsAllDataWritten;
	RFs						iFs;
	TInt					iAsyncErrorIndex;
	};

#endif /* __T_MM_DATA_MDA_AUDIO_OUTPUT_STREAM_H__ */
