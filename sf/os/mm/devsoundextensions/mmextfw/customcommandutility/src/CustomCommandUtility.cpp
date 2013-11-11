/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Custom command utility implementation
*
*/



#include <mdaaudiosampleplayer.h>
#include <mdaaudiosampleeditor.h>
#include <midiclientutility.h>
#include <DrmAudioSamplePlayer.h>
#include <CustomCommandUtility.h>
#include <videoplayer.h>
#include <videorecorder.h>

class CConvertCustomCommandUtility : public CCustomCommandUtility
	{
public:
	CConvertCustomCommandUtility(CMdaAudioConvertUtility& aUtility) :
		iUtility(aUtility)
		{
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom)
		{
		return iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2)
		{
		return iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
		}

private:
	CMdaAudioConvertUtility& iUtility;

	};

class CPlayerCustomCommandUtility : public CCustomCommandUtility
	{
public:
	CPlayerCustomCommandUtility(CMdaAudioPlayerUtility& aUtility) :
		iUtility(aUtility)
		{
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom)
		{
		return iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2)
		{
		return iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
		}

private:
	CMdaAudioPlayerUtility& iUtility;

	};

class CRecorderCustomCommandUtility : public CCustomCommandUtility
	{
public:
	CRecorderCustomCommandUtility(CMdaAudioRecorderUtility& aUtility) :
		iUtility(aUtility)
		{
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom)
		{
		return iUtility.RecordControllerCustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2)
		{
		return iUtility.RecordControllerCustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.RecordControllerCustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.RecordControllerCustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
		}

private:
	CMdaAudioRecorderUtility& iUtility;

	};

class CRecorderPlayCustomCommandUtility : public CCustomCommandUtility
	{
public:
	CRecorderPlayCustomCommandUtility(CMdaAudioRecorderUtility& aUtility) :
		iUtility(aUtility)
		{
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom)
		{
		return iUtility.PlayControllerCustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2)
		{
		return iUtility.PlayControllerCustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.PlayControllerCustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.PlayControllerCustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
		}

private:
	CMdaAudioRecorderUtility& iUtility;

	};

class CMidiCustomCommandUtility : public CCustomCommandUtility
	{
public:
	CMidiCustomCommandUtility(CMidiClientUtility& aUtility) :
		iUtility(aUtility)
		{
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom)
		{
		iUtility.CustomCommandSyncL(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
		return KErrNone;
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2)
		{
		iUtility.CustomCommandSyncL(aDestination, aFunction, aDataTo1, aDataTo2);
		return KErrNone;
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
		}

private:
	CMidiClientUtility& iUtility;
	};

class CDrmCustomCommandUtility : public CCustomCommandUtility
	{
public:
	CDrmCustomCommandUtility(CDrmPlayerUtility& aUtility) :
		iUtility(aUtility)
		{
		}

    virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom)
		{
		iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
		return KErrNone;
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2)
		{
		iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
		return KErrNone;
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
		}

private:
	CDrmPlayerUtility& iUtility;
	};

class CVideoRecorderCustomCommandUtility : public CCustomCommandUtility
	{
public:
	CVideoRecorderCustomCommandUtility(CVideoRecorderUtility& aUtility) :
		iUtility(aUtility)
		{
		}

    virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom)
		{
		iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
		return KErrNone;
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2)
		{
		iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
		return KErrNone;
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
		}

private:
	CVideoRecorderUtility& iUtility;
	};

class CVideoPlayerCustomCommandUtility : public CCustomCommandUtility
	{
public:
	CVideoPlayerCustomCommandUtility(CVideoPlayerUtility& aUtility) :
		iUtility(aUtility)
		{
		}

    virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2,
	                                     TDes8&                       aDataFrom)
		{
		iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
		return KErrNone;
		}

	virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
	                                     TInt                         aFunction,
	                               const TDesC8&                      aDataTo1,
	                               const TDesC8&                      aDataTo2)
		{
		iUtility.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
		return KErrNone;
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TDes8&                      aDataFrom,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
		}

	virtual void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
	                                      TInt                        aFunction,
	                                const TDesC8&                     aDataTo1,
	                                const TDesC8&                     aDataTo2,
	                                      TRequestStatus&             aStatus)
		{
		iUtility.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
		}

private:
	CVideoPlayerUtility& iUtility;
	};


EXPORT_C CCustomCommandUtility* CCustomCommandUtility::NewL(CMdaAudioConvertUtility& aUtility)
	{
	CCustomCommandUtility* customCommand;

	customCommand = new(ELeave) CConvertCustomCommandUtility(aUtility);

	return customCommand;
	}

EXPORT_C CCustomCommandUtility* CCustomCommandUtility::NewL(CMdaAudioPlayerUtility& aUtility)
	{
	CCustomCommandUtility* customCommand;

	customCommand = new(ELeave) CPlayerCustomCommandUtility(aUtility);

	return customCommand;
	}

EXPORT_C CCustomCommandUtility* CCustomCommandUtility::NewL(CMdaAudioRecorderUtility& aUtility,
                                                            TBool       aRecordStream)
	{
	CCustomCommandUtility* customCommand;

	if( !aRecordStream ) //Playing
		{
		customCommand = new(ELeave) CRecorderPlayCustomCommandUtility(aUtility);
		}
	else //Recording
		{
		customCommand = new(ELeave) CRecorderCustomCommandUtility(aUtility);
		}

	return customCommand;
	}

EXPORT_C CCustomCommandUtility* CCustomCommandUtility::NewL(CMidiClientUtility& aUtility)
	{
	CCustomCommandUtility* customCommand;

	customCommand = new(ELeave) CMidiCustomCommandUtility(aUtility);

	return customCommand;
	}

EXPORT_C CCustomCommandUtility* CCustomCommandUtility::NewL(CDrmPlayerUtility& aUtility)
	{
	CCustomCommandUtility* customCommand;

	customCommand = new(ELeave) CDrmCustomCommandUtility(aUtility);

	return customCommand;
	}

EXPORT_C CCustomCommandUtility* CCustomCommandUtility::NewL(CVideoPlayerUtility& aUtility)
	{
	CCustomCommandUtility* customCommand;

	customCommand = new(ELeave) CVideoPlayerCustomCommandUtility(aUtility);

	return customCommand;
	}

EXPORT_C CCustomCommandUtility* CCustomCommandUtility::NewL(CVideoRecorderUtility& aUtility)
	{
	CCustomCommandUtility* customCommand;

	customCommand = new(ELeave) CVideoRecorderCustomCommandUtility(aUtility);

	return customCommand;
	}	
// ========================== OTHER EXPORTED FUNCTIONS =========================



// End of File
