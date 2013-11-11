// play.h
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __PLAY_H__
#define __PLAY_H__

#include <fshell/ioutils.h>
#include <MdaAudioSampleEditor.h>

using namespace IoUtils;

class CMmfPlayer;

class CCmdPlay	: public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdPlay();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	CCmdPlay();
	void ConstructL();
private:
	CMmfPlayer* iPlayer;
	TFileName2 iFile;
	TInt iPriority;
	TInt iPreference;
	TInt iVolume;
	TBool iVerbose;
	TBool iMaxVolume;
	};


class CConsoleKeystrokeNotifier;

class CMmfPlayer : public CActive, public MMdaObjectStateChangeObserver
	{
public:
	static CMmfPlayer* NewL(RIoConsoleWriteHandle& aStdOut);
	virtual ~CMmfPlayer();
	
	TInt GetMaxVolume();
	TInt Error();
	
	void GetReady(const TDesC& aFileName, TBool aVerbose);
	void SetPriority(TInt aPriority, TInt iPreference);
	void SetVolume(TInt aVolume);
	
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();
	
	virtual void MoscoStateChangeEvent(CBase *aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);
	
	void KeyStroke(TKeyCode aCode);
private:
	CMmfPlayer(RIoConsoleWriteHandle& aStdOut);
	void ConstructL();
	void Message(TRefByValue<const TDesC> aFmt, ...);
	void ProcessL();
	void Error(TInt aError);
	void Stop();
private:
	RIoConsoleWriteHandle& iStdOut;
	TBuf<0x200> iMsgBuf;
	
	const TDesC* iFileName;
	
	enum TState
		{
		EInitial,
		EOpening,
		EPlaying,
		};
	TState iState;
	
	CMdaAudioRecorderUtility* iUtil;
	TInt iVolume;
	TBool iVerbose;
	TInt iError;
	};

class TTruncateOverflow : public TDes16Overflow
	{
	virtual void Overflow(TDes16& /*aDes*/) {};
	};
	
#endif // __PLAY_H__
