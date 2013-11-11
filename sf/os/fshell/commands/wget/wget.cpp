// wget.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

// TODO: byte-range support

#include <commdbconnpref.h>
#include <httpstringconstants.h>
#include <httperr.h>
#include <mhttpdatasupplier.h>
#include <rhttpheaders.h>
#include <thttphdrfielditer.h>
#include <bautils.h>
#include "wget.h"

using namespace IoUtils;

_LIT8(KHttpProtocol,	"HTTP/TCP");
_LIT(KDateFormat,		"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3");
const TInt KMaxHeaderNameLen = 32;
const TInt KMaxHeaderValueLen = 128;

CCommandBase* CCmdWget::NewLC()
	{
	CCmdWget* self = new(ELeave) CCmdWget();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdWget::~CCmdWget()
	{
	delete iUrl;
	delete iUsername;
	delete iPassword;
	if (iSessionIsOpen)
		{
		iSession.Close(); // closes iTransaction also
		}
	if (iConnection.SubSessionHandle() > 0)
		{
		iConnection.Close();
		}
	if (iSocketServ.Handle() > 0)
		{
		iSocketServ.Close();
		}
	if (iLocalFile.SubSessionHandle() > 0)
		{
		iLocalFile.Flush();
		iLocalFile.Close();
		}
	}

CCmdWget::CCmdWget() : CCommandBase(CCommandBase::EManualComplete)
	{
	}

const TDesC& CCmdWget::Name() const
	{
	_LIT(KName, "wget");
	return KName;
	}

void CCmdWget::DoRunL()
	{
	// sanity check parameters
	if (!iUrl || iUrl->Des().Length() > KMaxName)
		{
		if (iVerbose)
			{
			PrintError(KErrArgument, _L("Url not specified or is too large."));
			}
		User::Leave(KErrArgument);
		}		
	if ((iUsername && iUsername->Des().Length() > KMaxName) || (iPassword && iPassword->Des().Length() > KMaxName))
		{
		if (iVerbose)
			{
			PrintError(KErrArgument, _L("Username or Password is too large."));
			}
		User::Leave(KErrArgument);
		}

	if (iUploadFilename.Length())
		{
		iPostData = ETrue; // Keep this member variable because lots of code assumes it
		}

	LaunchConnectionL(); // start a connection - KErrNone returned if successfully started or a compatible connection already exists
		
	if (iPostData)
		{
		// http post
		PrepareUploadFileL();
		}
	else
		{
		// http get
		PrepareDownloadFileL(); // prep. the local file to receive the downloaded data
		}
	User::LeaveIfError(iLocalFile.Size(iLocalFileSize));
	ConfigureHTTPL(); // setup http session & configure with connection parameters
	LaunchHTTPTransactionL(); // launch a http transaction
	}

void CCmdWget::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArg1, "url");
	aArguments.AppendStringL(iUrl, KArg1);
	}

void CCmdWget::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptContinue, "continue");
	aOptions.AppendBoolL(iContinue, KOptContinue);

	_LIT(KOptDestinationFilename, "downloadfile");
	aOptions.AppendFileNameL(iDestinationFilename, KOptDestinationFilename);

	_LIT(KOptSourceFilename, "uploadfile");
	aOptions.AppendFileNameL(iUploadFilename, KOptSourceFilename);

	_LIT(KOptIapId, "iap");
	aOptions.AppendIntL(iIapId, KOptIapId);

	_LIT(KOptUsername, "username");
	aOptions.AppendStringL(iUsername, KOptUsername);

	_LIT(KOptPassword, "password");
	aOptions.AppendStringL(iPassword, KOptPassword);
	
	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);
	}

//
// CCmdWget::PrepareDownloadFileL
// determine the destination file & path and open a handle to a local file
//
void CCmdWget::PrepareDownloadFileL()
	{
	ASSERT(iUrl);
	ASSERT(!iPostData);
	TFileName fname;
	RFs& fs = FsL(); 
	fname.Zero();
	if (iDestinationFilename.Length() <= 0)
		{
		// use current path
		fname.Append(Env().Pwd());
		
		// use the same name as the file we're intending to download
		TChar dirMarker('/');
		TInt mark = iUrl->Des().LocateReverse(dirMarker);
		if (mark <= 0)
			{
			User::Leave(KErrNotFound);
			}
		fname.Append(iUrl->Des().Mid(++mark)); // increment mark to step over the '/' 
		}
	else 
		{
		fname.Copy(iDestinationFilename); // user has specified a local file name
		}
	if (BaflUtils::FileExists(fs, fname))
		{
		if (iContinue)
			{
			// open the file & append any new data to what's already in there
			User::LeaveIfError(iLocalFile.Open(fs, fname, EFileShareExclusive));
			}
		else
			{
			// open the file & replace old data with new data
			User::LeaveIfError(iLocalFile.Replace(fs, fname, EFileShareExclusive));
			}
		}
	else
		{
		// create the file for the first time
		User::LeaveIfError(iLocalFile.Create(fs, fname, EFileShareExclusive));
		}
	}

//
// CCmdWget::PrepareUploadFileL
// prep. the local file containing data for upload to the web server
//
void CCmdWget::PrepareUploadFileL()
	{
	ASSERT(iUrl);
	ASSERT(iPostData);
	
	User::LeaveIfError(iLocalFile.Open(FsL(), iUploadFilename, EFileRead | EFileShareReadersOnly));
		
	
	}

//
// CCmdWget::LaunchConnectionL
// establish a connection to the network
//
void CCmdWget::LaunchConnectionL()
	{
	User::LeaveIfError(iSocketServ.Connect());
	User::LeaveIfError(iConnection.Open(iSocketServ));
	
	TInt connError = KErrNone;	
	TCommDbConnPref prefs;
	if (iIapId > 0)
		{
		prefs.SetIapId(iIapId);
		prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
		connError = iConnection.Start(prefs);
		}
	else
		{
		connError = iConnection.Start();
		}
	if (iVerbose && (connError != KErrNone))
		{
		PrintError(connError, _L("Unable to establish network connection"));
		User::Leave(connError);
		}	
	}

//
// CCmdWget::ConfigureHTTPL
//
void CCmdWget::ConfigureHTTPL()
	{
	iSession.OpenL(KHttpProtocol);
	iSessionIsOpen = ETrue;
	iSession.SetSessionEventCallback(this);
	RStringPool pool = iSession.StringPool();
	RHTTPConnectionInfo conInf = iSession.ConnectionInfo();
	conInf.SetPropertyL(pool.StringF(HTTP::EHttpSocketServ, RHTTPSession::GetTable()), THTTPHdrVal(iSocketServ.Handle()));
	conInf.SetPropertyL(pool.StringF(HTTP::EHttpSocketConnection, RHTTPSession::GetTable()), THTTPHdrVal(reinterpret_cast<TInt> (&iConnection)));
	InstallAuthenticationL(iSession);
	}

//
// CCmdWget::LaunchHTTPTransactionL
// launches a new http transaction
//
void CCmdWget::LaunchHTTPTransactionL()
	{
	// create the transaction
	TBuf8<KMaxName> url8;
	url8.Copy(iUrl->Des());
	TUriParser8 uri;
	User::LeaveIfError(uri.Parse(url8));
	RStringPool pool = iSession.StringPool();
	RStringF method = pool.StringF(HTTP::EGET,RHTTPSession::GetTable());
	if (iPostData)
		{
		method = pool.StringF(HTTP::EPOST,RHTTPSession::GetTable());
		}
	iTransaction = iSession.OpenTransactionL(uri, *this, method);
	
	// some transaction configuration is required for posting data
	if (iPostData)
		{
		RHTTPRequest req = iTransaction.Request();
		req.SetBody(*this); // ccmdwget to supply the data for posting
		THTTPHdrVal length(OverallDataSize());
		THTTPHdrVal type(pool.StringF(HTTP::ETextAny, RHTTPSession::GetTable()));
		RHTTPHeaders hdr = req.GetHeaderCollection();
		hdr.SetFieldL(pool.StringF(HTTP::EContentLength, RHTTPSession::GetTable()), length);
		hdr.SetFieldL(pool.StringF(HTTP::EContentType, RHTTPSession::GetTable()), type);
		}
	
	// launch it
	iTransaction.SubmitL();	
	}

//
// CCmdWget::GetCredentialsL
// request from the underlying HTTP session to provide it with some authentication credentials (if present)
//
TBool CCmdWget::GetCredentialsL(const TUriC8& /*aURI*/, RString aRealm, RStringF /*aAuthenticationType*/, RString& aUsername, RString& aPassword)
	{
	TBool present = EFalse;
	RStringPool pl = aRealm.Pool();
	if (iUsername)
		{
		present = ETrue;
		TBuf8<KMaxName> user;
		user.Copy(iUsername->Des());
		aUsername = pl.OpenStringL(user);
		}
	if (iPassword)
		{
		present = ETrue;
		TBuf8<KMaxName> pwd;
		pwd.Copy(iPassword->Des());
		aPassword = pl.OpenStringL(pwd);
		}
	return present;
	}
	
//
// CCmdWget::MHFSessionRunL
// handles any up-calls from the http session
//
void CCmdWget::MHFSessionRunL(const THTTPSessionEvent& aEvent)
	{
	switch (aEvent.iStatus)
		{
		case THTTPSessionEvent::EConnect:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Connect\r\n"));
				}
			}
		break;
		
		case THTTPSessionEvent::EConnectedOK:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Connected OK\r\n"));
				}
			}
		break;
		
		case THTTPSessionEvent::EDisconnect:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Disconnect\r\n"));
				}
			}
		break;
		
		case THTTPSessionEvent::EConnectedWithReducedCapabilities:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Connected with reduced capabilities\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EDisconnected:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Disconnected\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EAuthenticatedOK:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Authenticated OK\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EAuthenticationFailure:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Authentication failure\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EConnectionTimedOut:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Connection timed out\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::ENotConnected:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Not connected\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EExceptionInfo:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Exception info\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::ERedirected:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Redirected\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EAlreadyConnecting:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Already connecting\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EAlreadyConnected:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Already connected\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EAlreadyDisconnecting:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Already disconnecting\r\n"));
				}
			
			}
		break;

		case THTTPSessionEvent::EAlreadyDisconnected:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Already disconnected\r\n"));
				}
			
			}
		break;

		default:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Session Event unrecgonised\r\n"));
				}
			}
		break;
		};
	}

//
// CCmdWget::MHFSessionRunError
// handles an error related up-calls 
TInt CCmdWget::MHFSessionRunError(TInt aError, const THTTPSessionEvent& /*aEvent*/)
	{
	if (iVerbose)
		{
		PrintError(aError, _L("HTTP Session Error"));
		}
	return KErrNone;
	}
	
//
// CCmdWget::MHFRunL
// handles any up-calls regarding a current transaction
//
void CCmdWget::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
	{
	if (aTransaction.Id() != iTransaction.Id())
		{
		if (iVerbose)
			{
			PrintError(KErrUnknown, _L("HTTP Transaction Id is invalid"));
			}
		User::Leave(KErrUnknown);
		}
	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			if (iVerbose)
				{
				DumpRespHeadersL(aTransaction);
				}
			}
		break;
		
		case THTTPEvent::EGotResponseBodyData:
			{
			RHTTPResponse response = aTransaction.Response();
			if (iVerbose)
				{
				Printf(_L("HTTP <%d> received\r\n"), response.StatusCode());
				}
			switch (response.StatusCode())
				{
				// informational codes (1xx)
				case HTTPStatus::EContinue:
				case HTTPStatus::ESwitchingProtocols:
				break;
				
				// successful codes (2xx)
				case HTTPStatus::EOk:
					{
					MHTTPDataSupplier* body = aTransaction.Response().Body();
					TPtrC8 data;
					TBool last = body->GetNextDataPart(data);
					if (!iPostData)
						{
						User::LeaveIfError(iLocalFile.Write(data));
						if (last)
							{
							iLocalFile.Flush();
							}
						if (iVerbose)
							{
							Printf(_L("bytes: %d\r\n"), data.Length());
							}
						}
					else
						{
						TBuf<512> woop;
						woop.Copy(data);
						Stdout().Write(woop);
						}
					body->ReleaseData();
					}
				break;
				
				case HTTPStatus::ECreated:
				case HTTPStatus::EAccepted:
				case HTTPStatus::ENonAuthoritativeInfo:
				case HTTPStatus::ENoContent:
				case HTTPStatus::EResetContent:
				break;
				
				case HTTPStatus::EPartialContent:
					{
					// TODO - byte range support
					}
				break;
				
				// Redirection codes (3xx)
				case HTTPStatus::EMultipleChoices:
				case HTTPStatus::EMovedPermanently:
				case HTTPStatus::EFound:
				case HTTPStatus::ESeeOther:
				case HTTPStatus::ENotModified:
				case HTTPStatus::EUseProxy:
				case HTTPStatus::EUnused:
				case HTTPStatus::ETemporaryRedirect:
					if (iVerbose)
						{
						Printf(_L("Redirecting\r\n"));
						}
					
				break;
				
				// Client error codes (4xx)
				case HTTPStatus::EBadRequest:
				case HTTPStatus::EUnauthorized:
				case HTTPStatus::EPaymentRequired:
				case HTTPStatus::EForbidden:
				case HTTPStatus::ENotFound:
				case HTTPStatus::EMethodNotAllowed:
				case HTTPStatus::ENotAcceptable:
				case HTTPStatus::EProxyAuthenticationRequired:
				case HTTPStatus::ERequestTimeout:
				case HTTPStatus::EConflict:
				case HTTPStatus::EGone:
				case HTTPStatus::ELengthRequired:
				case HTTPStatus::EPreconditionFailed:
				case HTTPStatus::ERequestEntityTooLarge:
				case HTTPStatus::ERequestURITooLong:
				case HTTPStatus::EUnsupportedMediaType:
				case HTTPStatus::ERequestedRangeNotSatisfiable:
				case HTTPStatus::EExpectationFailed:
					{
					if (iVerbose)
						{
						PrintError(KErrGeneral, _L("HTTP client error"));
						}
					Complete(KErrGeneral);
					}
				break;
				
				// Server error codes (5xx)
				case HTTPStatus::EInternalServerError:
				case HTTPStatus::ENotImplemented:
				case HTTPStatus::EBadGateway:
				case HTTPStatus::EServiceUnavailable:
				case HTTPStatus::EGatewayTimeout:
				case HTTPStatus::EHTTPVersionNotSupported:
					{
					if (iVerbose)
						{
						PrintError(KErrGeneral, _L("HTTP Remote Server Error"));
						}
					Complete(KErrGeneral);
					}
				break;

				default:
					Complete(KErrGeneral);
				break;
				};
			}
		break;
		
		case THTTPEvent::EResponseComplete:
			{
			if (iVerbose)
				{
				Printf(_L("HTTP Response complete.\r\n"));
				}
			}
		break;
		
		case THTTPEvent::ESucceeded:
			{
			if (iVerbose)
				{
				Printf(_L("Download transaction complete.\r\n"));
				}
			Complete(KErrNone);
			}
		break;
		
		case THTTPEvent::EFailed:
			{
			if (iVerbose)
				{
				Printf(_L("Download transaction failed. Aborting\r\n"));
				}
			Complete(KErrGeneral);			
			}
		break;
		
		case THTTPEvent::ERedirectedPermanently:
		case THTTPEvent::ERedirectedTemporarily:
			{
			if (iVerbose)
				{
				Printf(_L("Redirecting\r\n"));
				}
			}
		break;
		
		case THTTPEvent::ERedirectRequiresConfirmation:
			{
			if (iVerbose)
				{
				Printf(_L("Redirect requires confirmation. Aborting\r\n"));
				}
			Complete(KErrAbort);
			}
		break;
		
		default:
			{
			if (iVerbose)
				{
				Printf(_L("Transaction result: <%d>\r\n"), aEvent.iStatus);
				}
			if (aEvent.iStatus < 0)
				{
				Complete(aEvent.iStatus);
				}
			}
		break;
		};
	}
	
//
// CCmdWget::MHFRunError
// handles any error related up-calls from an underlying transaction in progress
//
TInt CCmdWget::MHFRunError(TInt aError, RHTTPTransaction /* aTransaction */, const THTTPEvent& /* aEvent */)
	{
	if (iVerbose)
		{
		PrintError(aError, _L("HTTP Transaction error"));
		}
	return KErrNone;
	}
	
//
// CCmdWget::DumpRespHeadersL
// dump the transaction response headers
//
void CCmdWget::DumpRespHeadersL(RHTTPTransaction& aTransaction)
	{
	RHTTPResponse resp = aTransaction.Response();
	RStringPool strP = aTransaction.Session().StringPool();
	RHTTPHeaders hdr = resp.GetHeaderCollection();
	THTTPHdrFieldIter it = hdr.Fields();

	TBuf<KMaxHeaderNameLen>  fieldName16;
	TBuf<KMaxHeaderValueLen> fieldVal16;

	while (it.AtEnd() == EFalse)
		{
		RStringTokenF fieldName = it();
		RStringF fieldNameStr = strP.StringF(fieldName);
		THTTPHdrVal fieldVal;
		if (hdr.GetField(fieldNameStr,0,fieldVal) == KErrNone)
			{
			const TDesC8& fieldNameDesC = fieldNameStr.DesC();
			fieldName16.Copy(fieldNameDesC.Left(KMaxHeaderNameLen));
			switch (fieldVal.Type())
				{
			case THTTPHdrVal::KTIntVal:
				Printf(_L("%S: %d\r\n"), &fieldName16, fieldVal.Int());
				break;
			case THTTPHdrVal::KStrFVal:
				{
				RStringF fieldValStr = strP.StringF(fieldVal.StrF());
				const TDesC8& fieldValDesC = fieldValStr.DesC();
				fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
				Printf(_L("%S: %S\r\n"), &fieldName16, &fieldVal16);
				}
				break;
			case THTTPHdrVal::KStrVal:
				{
				RString fieldValStr = strP.String(fieldVal.Str());
				const TDesC8& fieldValDesC = fieldValStr.DesC();
				fieldVal16.Copy(fieldValDesC.Left(KMaxHeaderValueLen));
				Printf(_L("%S: %S\r\n"), &fieldName16, &fieldVal16);
				}
				break;
			case THTTPHdrVal::KDateVal:
				{
				TDateTime date = fieldVal.DateTime();
				TBuf<40> dateTimeString;
				TTime t(date);
				t.FormatL(dateTimeString,KDateFormat);
				Printf(_L("%S: %S\r\n"), &fieldName16, &dateTimeString);
				} 
				break;
			default:
				Printf(_L("%S: <unrecognised value type>\r\n"), &fieldName16);
				break;
				};

			// Display realm for WWW-Authenticate header
			RStringF wwwAuth = strP.StringF(HTTP::EWWWAuthenticate,RHTTPSession::GetTable());
			if (fieldNameStr == wwwAuth)
				{
				// check the auth scheme is 'basic'
				RStringF basic = strP.StringF(HTTP::EBasic,RHTTPSession::GetTable());
				RStringF realm = strP.StringF(HTTP::ERealm,RHTTPSession::GetTable());
				THTTPHdrVal realmVal;
				if ((fieldVal.StrF() == basic) && 
					(!hdr.GetParam(wwwAuth, realm, realmVal)))
					{
					RStringF realmValStr = strP.StringF(realmVal.StrF());
					fieldVal16.Copy(realmValStr.DesC());
					Printf(_L("Realm is: %S\r\n"), &fieldVal16);
					}
				}
			}
		++it;
		}	
	}

//
// MHTTPDataSupplier methods
//

//
// CCmdWget::GetNextDataPart
// reads a chunk of data from the source file
// 
TBool CCmdWget::GetNextDataPart(TPtrC8& aDataPart)
	{
	ASSERT(iPostData);
	ASSERT(iLocalFile.SubSessionHandle() > 0);
		
	TBool result = EFalse;
	
	// calc read length
	TInt readlength = KDefaultChunkSize;
	if ((iLocalFileSize - iLocalBytesRead) < readlength)
		{
		readlength = iLocalFileSize - iLocalBytesRead;
		}
	
	// if necessary, return previous chunk (per the SDK's MHTTPDataSupplier documentation)
	if (!iReleaseDataCalled && (iPrevData.Length() > 0))
		{
		iReleaseDataCalled = EFalse;
		aDataPart.Set(iPrevData);
		return result;
		}
	
	// read another chunk of the data & pass it back
	TInt error = iLocalFile.Read(iPrevData, readlength);
	if (error != KErrNone)
		{
		if (iVerbose)
			{
			PrintError(error, _L("Unable to read data from local file."));
			}
		Complete(error);
		return result;
		}
	aDataPart.Set(iPrevData);
	iReleaseDataCalled = EFalse;
	iLocalBytesRead += readlength;
	if (iLocalBytesRead >= iLocalFileSize)
		{
		ASSERT((iLocalBytesRead == iLocalFileSize)); // else our math has gone wrong somewhere 
		result = ETrue;
		}
	return result;
	}

//
// CCmdWget::ReleaseData
// tell the http transaction we have more data available
//
void CCmdWget::ReleaseData()
	{
	ASSERT(iPostData);
	iReleaseDataCalled = ETrue;
	if ((iLocalFileSize - iLocalBytesRead) > 0)
		{
		// more data to come
		TRAPD(error, iTransaction.NotifyNewRequestBodyPartL());
		if (error != KErrNone)
			{
			if (iVerbose)
				{
				PrintError(error, _L("Unable to release data."));
				}
			Complete(error);
			}
		}
	}

//
// CCmdWget::OverallDataSize
// return the total size of the data we're to upload to the server
//
TInt CCmdWget::OverallDataSize()
	{
	ASSERT(iPostData);
	ASSERT(iLocalFile.SubSessionHandle() > 0);
	return iLocalFileSize;
	}

//
// CCmdWget::Reset
// Reset the data supply to the beginning of the local file
// 
TInt CCmdWget::Reset()
	{
	ASSERT(iPostData);
	ASSERT(iLocalFile.SubSessionHandle() > 0);
	TInt pos = 0;
	TInt error = iLocalFile.Seek(ESeekStart, pos); // not interested in pos
	return error;
	}


EXE_BOILER_PLATE(CCmdWget)
