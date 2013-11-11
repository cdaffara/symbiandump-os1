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



#if (!defined __T_MM_DATA_MDA_AUDIO_INPUT_STREAM_H__)
#define __T_MM_DATA_MDA_AUDIO_INPUT_STREAM_H__

//	User Includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <mdaaudioinputstream.h>

/**
 * Test Active Notification class
 *
 */


class CT_MMDataMdaAudioInputStream : public CDataWrapperBase, private MMdaAudioInputStreamCallback
	{
public:
	/**
	* Public destructor
	*/
	~CT_MMDataMdaAudioInputStream();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_MMDataMdaAudioInputStream*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MMDataMdaAudioInputStream();

	void	ConstructL();

private:
	inline void				DoCmdDestructor();
	inline void				DoCmdBitRateL(const TDesC& aSection);
	inline void				DoCmdCustomInterface();
	inline void				DoCmdDataTypeL(const TDesC& aSection);
	inline void				DoCmdGain(const TDesC& aSection);
	inline void				DoCmdGetBalanceL(const TDesC& aSection);
	inline void				DoCmdGetBytes();
	inline void				DoCmdGetSupportedBitRatesL();
	inline void				DoCmdMaxGain();
	inline void				DoCmdNewL(const TDesC& aSection);
	inline void				DoCmdOpen(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void				DoCmdPosition();
	inline void				DoCmdReadL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void				DoCmdSetAudioPropertiesL(const TDesC& aSection);
	inline void				DoCmdSetBalanceL(const TDesC& aSection);
	inline void				DoCmdSetBitRateL(const TDesC& aSection);
	inline void				DoCmdSetDataTypeL(const TDesC& aSection);
	inline void				DoCmdSetGain(const TDesC& aSection);
	inline void				DoCmdSetPriority(const TDesC& aSection);
	inline void				DoCmdStop();

	//	MMdaAudioInputStreamCallback implementation
	virtual void			MaiscOpenComplete(TInt aError);
	virtual void			MaiscBufferCopied(TInt aError, const TDesC8& aBuffer);
	virtual void			MaiscRecordComplete(TInt aError);

	//	Helper methods
	void					ReadStreamAudioDataL();
	void					DestroyData();

private:
	CMdaAudioInputStream*	iInputStream;
	HBufC8*					iBuffer;
	TPtr8*					iBufferPtr;
	TInt					iBufferSize;
	TBool					iStreamOpened;
	RFile					iFile;
	TBool					iFileOpened;
	TBool					iIsAllDataRead;
	RFs						iFs;
	TInt					iAsyncErrorIndex;
	};

#endif /* __T_MM_DATA_MDA_AUDIO_INPUT_STREAM_H__ */
