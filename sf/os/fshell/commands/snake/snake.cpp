// snake.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <e32math.h>

using namespace IoUtils;

_LIT(KSnakeSeg, "*");
_LIT(KBait, "$");

class CKeyWatcher;

class CCmdSnake : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSnake();
	
	void Up();
	void Down();
	void Left();
	void Right();
	void Exit();
	void OnTimer();
private:
	CCmdSnake();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	
	void ConstructL();
	void InitBoardL();
	void SetBoard(TInt aX, TInt aY, TBool aSet);
	TBool GetBoard(TInt aX, TInt aY);
	void DrawBoardL();
	void InitSnakeL(TInt aLen, TPoint aPos);
	void DrawSnakeL();
	void DrawScore();
	void PlaceBait();
	void Grow();
	
	void Dead();
private:
	RIoConsoleWriteHandle iCons;
	RIoConsoleReadHandle iConsIn;
	TSize iBoardSize;
	RArray<TPoint> iSnake;
	TInt iSnakeHead;
	RArray<TUint32> iBoard;
	TInt iBoardWidthWords;
	TPoint iDirection;
	TPoint iBait;
	TInt iScore;
	TInt iSpeed;
	
	CKeyWatcher* iKeys;
	CPeriodic* iTimer;
	TInt64 iRandomSeed;
	TBool iInCollisionGracePeriod;
	};

class CKeyWatcher : public CActive
	{
public:
	CKeyWatcher(CCmdSnake& aOwner, RIoConsoleReadHandle& aCons);
	~CKeyWatcher();
	void Request();

private:
	virtual void RunL();
	virtual void DoCancel();
private:
	CCmdSnake& iOwner;
	RIoConsoleReadHandle& iCons;
	};


CCommandBase* CCmdSnake::NewLC()
	{
	CCmdSnake* self = new(ELeave) CCmdSnake();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCmdSnake::~CCmdSnake()
	{
	delete iKeys;
	delete iTimer;
	iSnake.Close();
	iBoard.Close();
	}

CCmdSnake::CCmdSnake()
	: CCommandBase(EManualComplete), iSpeed(50000)
	{
	}

const TDesC& CCmdSnake::Name() const
	{
	_LIT(KName, "snake");	
	return KName;
	}
	
TInt OnTimerS(TAny* aSnake)
	{
	((CCmdSnake*)aSnake)->OnTimer();
	return KErrNone;
	}

void CCmdSnake::DoRunL()
	{
	if (!Stdout().AttachedToConsole())
		{
		LeaveIfErr(KErrNotSupported, _L("Stdout must be connected to a console"));
		}
	iCons = Stdout();
	if (!Stdin().AttachedToConsole())
		{
		LeaveIfErr(KErrNotSupported, _L("Stdin must be connected to a console"));
		}
	iConsIn = Stdin();
	iKeys = new(ELeave)CKeyWatcher(*this, iConsIn);
	iCons.SetCursorHeight(0);
	
	User::LeaveIfError(iCons.GetScreenSize(iBoardSize));
	iBoardSize.iHeight--; // space for status bar, plus to ensure that the console doesn't scroll when we draw the bottom line
	iBoardWidthWords = iBoardSize.iWidth / 32;
	if (iBoardSize.iWidth % 32) ++iBoardWidthWords;
	InitBoardL();
	
	DrawBoardL();
	
	TPoint centre;
	centre.iX = iBoardSize.iWidth / 2;
	centre.iY = iBoardSize.iHeight / 2;
	InitSnakeL(10, centre);	
	DrawSnakeL();
	PlaceBait();
	DrawScore();
	
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	iTimer->Start(iSpeed, iSpeed, TCallBack(OnTimerS, this));
	}

void CCmdSnake::ArgumentsL(RCommandArgumentList& /*aArguments*/)
	{
	}

void CCmdSnake::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptSpeed, "speed");
	aOptions.AppendIntL(iSpeed, KOptSpeed);
	}
	
void CCmdSnake::ConstructL()
	{
	BaseConstructL();
	TTime now;
	now.HomeTime();
	iRandomSeed = now.Int64();
	}

void CCmdSnake::InitBoardL()
	{
	// first, zero the whole board
	iBoard.Reset();
	for (TInt y=0; y<iBoardSize.iHeight; ++y)
		{
		for (TInt x=0; x<iBoardWidthWords; ++x)
			{
			iBoard.AppendL(0);
			}
		}
	// draw a box around the edge
	for (TInt i=0; i<iBoardSize.iWidth; ++i)
		{
		SetBoard(i, 0, ETrue);
		SetBoard(i, iBoardSize.iHeight-1, ETrue);
		}
	for (TInt i=0; i<iBoardSize.iHeight; ++i)
		{
		SetBoard(0, i, ETrue);
		SetBoard(iBoardSize.iWidth-1, i, ETrue);
		}
	}
	
void CCmdSnake::SetBoard(TInt aX, TInt aY, TBool aSet)
	{
	TInt word = (aY * iBoardWidthWords) + (aX / 32);
	TInt bit = 1 << (aX % 32);
	if (aSet)
		{
		iBoard[word] |= bit;
		}
	else
		{
		iBoard[word] &= (~bit);
		}
	}
	
TBool CCmdSnake::GetBoard(TInt aX, TInt aY)
	{
	TInt word = (aY * iBoardWidthWords) + (aX / 32);
	TInt bit = 1 << (aX % 32);
	return iBoard[word] & bit;
	}
	
void CCmdSnake::DrawBoardL()
	{
	User::LeaveIfError(iCons.ClearScreen());
	
	RBuf line;
	line.CreateMaxL(iBoardSize.iWidth);
	CleanupClosePushL(line);
	
	for (TInt y=0; y<iBoardSize.iHeight; ++y)
		{
		for (TInt x=0; x<iBoardSize.iWidth; ++x)
			{
			line[x] = GetBoard(x,y) ? '#' : ' ';
			}
		User::LeaveIfError(iCons.SetCursorPosAbs(TPoint(0, y)));
		User::LeaveIfError(iCons.Write(line));
		}
	
	
	CleanupStack::PopAndDestroy();
	}
	
void CCmdSnake::InitSnakeL(TInt aLen, TPoint aPos)
	{
	iSnake.Reset();
	for (TInt i=0; i<aLen; ++i)
		{
		iSnake.AppendL(aPos);
		}
	iSnakeHead = 0;
	}
	
void CCmdSnake::DrawSnakeL()
	{
	for (TInt i=0; i<iSnake.Count(); ++i)
		{
		User::LeaveIfError(iCons.SetCursorPosAbs(iSnake[i]));
		User::LeaveIfError(iCons.Write(KSnakeSeg));
		}
	}

void CCmdSnake::Up()
	{
	if (iDirection.iY != 1)
		{
		iDirection.SetXY(0, -1);
		}
	}
	
void CCmdSnake::Down()
	{
	if (iDirection.iY != -1)
		{
		iDirection.SetXY(0, 1);
		}
	}
	
void CCmdSnake::Left()
	{
	if (iDirection.iX != 1)
		{
		iDirection.SetXY(-1, 0);
		}
	}
	
void CCmdSnake::Right()
	{
	if (iDirection.iX != -1)
		{
		iDirection.SetXY(1, 0);
		}
	}
	
_LIT(KQuitFmt, "Quit; score: %d\n");

void CCmdSnake::Exit()
	{
	iCons.SetCursorPosAbs(TPoint(0, iBoardSize.iHeight));
	iCons.SetCursorHeight(20);
	TBuf<0x20> buf;
	buf.AppendFormat(KQuitFmt, iScore);
	iCons.Write(buf);
	Complete(iScore);
	}
	
_LIT(KDeadFmt, "Game over; score: %d\n");

void CCmdSnake::Dead()
	{
	iKeys->Cancel();
	iCons.SetCursorPosAbs(TPoint(0, iBoardSize.iHeight));
	iCons.SetCursorHeight(20);
	TBuf<0x20> buf;
	buf.AppendFormat(KDeadFmt, iScore);
	iCons.Write(buf);
	Complete(iScore);
	}
	
void CCmdSnake::OnTimer()
	{
	iKeys->Request();
	if (!(iDirection.iX || iDirection.iY)) return;
	TPoint newHead = iSnake[iSnakeHead] + iDirection + iBoardSize;
	newHead.iX %= iBoardSize.iWidth;
	newHead.iY %= iBoardSize.iHeight;
	
	// check for obstacles
	if (GetBoard(newHead.iX, newHead.iY))
		{
		if (iInCollisionGracePeriod)
			{
			Dead();
			}
		else
			{
			iInCollisionGracePeriod = ETrue;
			}
		return;
		}
	iInCollisionGracePeriod = EFalse;
	for (TInt i=0; i<iSnake.Count(); ++i)
		{
		if (newHead == iSnake[i])
			{
			Dead();
			return;
			}
		}
	
	TInt tail = (iSnakeHead+1) % iSnake.Count();
	// overwrite the tail if it's not in the same position as the segment before it
	if (iSnake[tail] != iSnake[(tail+1)%iSnake.Count()])
		{
		iCons.SetCursorPosAbs(iSnake[tail]);
		iCons.Write(_L(" "));
		}
	iCons.SetCursorPosAbs(newHead);
	iCons.Write(KSnakeSeg);
	iSnakeHead = (iSnakeHead+1) % iSnake.Count();
	iSnake[iSnakeHead] = newHead;
	
	if (iSnake[iSnakeHead] == iBait)
		{
		// Yummy!
		iScore++;
		DrawScore();
		PlaceBait();
		Grow();
		}
	}
	
_LIT(KScoreFormat, "Score: %d");
	
void CCmdSnake::DrawScore()
	{
	iCons.SetCursorPosAbs(TPoint(0, iBoardSize.iHeight));
	TBuf<0x20> score;
	score.AppendFormat(KScoreFormat, iScore);
	iCons.Write(score);
	}

void CCmdSnake::PlaceBait()
	{
	TBool ok;
	do
		{
		ok = ETrue;
		iBait.iX = Math::Rand(iRandomSeed) % iBoardSize.iWidth;
		iBait.iY = Math::Rand(iRandomSeed) % iBoardSize.iHeight;
		
		if (GetBoard(iBait.iX, iBait.iY)) ok = EFalse;
		if (ok)
			{
			for (TInt i=0; i<iSnake.Count(); ++i)
				{
				if (iSnake[i] == iBait) ok = EFalse;
				}
			}
		} while (!ok);
	iCons.SetCursorPosAbs(iBait);
	iCons.Write(KBait);
	}

void CCmdSnake::Grow()
	{
	TInt tail = (iSnakeHead+1)% iSnake.Count();
	iSnake.Insert(iSnake[tail], tail);
	if (tail < iSnakeHead) iSnakeHead++;
	}



EXE_BOILER_PLATE(CCmdSnake)


//______________________________________________________________________________
//						CKeyWatcher
CKeyWatcher::CKeyWatcher(CCmdSnake& aOwner, RIoConsoleReadHandle& aCons)
	: CActive(EPriorityHigh), iOwner(aOwner), iCons(aCons)
	{
	CActiveScheduler::Add(this);
	Request();
	}

CKeyWatcher::~CKeyWatcher()
	{
	Cancel();
	}

void CKeyWatcher::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	TKeyCode key = (TKeyCode)iCons.KeyCode();
	switch (key)
		{
	case EKeyUpArrow:
	case '2':
		iOwner.Up();
		break;
	case EKeyDownArrow:
	case '8':
		iOwner.Down();
		break;
	case EKeyLeftArrow:
	case '4':
		iOwner.Left();
		break;
	case EKeyRightArrow:
	case '6':
		iOwner.Right();
		break;
	case EKeyEscape:
	case EKeyBackspace:
		iOwner.Exit();
		break;
	default:
		break;
		}
	}

void CKeyWatcher::DoCancel()
	{
	iCons.WaitForKeyCancel();
	}

void CKeyWatcher::Request()
	{
	if (!IsActive())
		{
		iCons.WaitForKey(iStatus);
		SetActive();
		}
	}
