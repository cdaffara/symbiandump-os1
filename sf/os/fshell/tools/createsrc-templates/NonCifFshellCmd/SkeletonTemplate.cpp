// SkeletonTemplate.cpp
//
// [[COPYRIGHT]]
//

#include <fshell/ioutils.h>
#include <fshell/common.mmh>

using namespace IoUtils;

class CCmdSkeletonTemplate : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSkeletonTemplate();
private:
	CCmdSkeletonTemplate();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual const TDesC& Description() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	//TODO: arguments/options
	};

EXE_BOILER_PLATE(CCmdSkeletonTemplate)

CCommandBase* CCmdSkeletonTemplate::NewLC()
	{
	CCmdSkeletonTemplate* self = new(ELeave) CCmdSkeletonTemplate();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSkeletonTemplate::~CCmdSkeletonTemplate()
	{
	}

CCmdSkeletonTemplate::CCmdSkeletonTemplate()
	{
	}

const TDesC& CCmdSkeletonTemplate::Name() const
	{
	_LIT(KName, "skeletontemplate");	
	return KName;
	}

const TDesC& CCmdSkeletonTemplate::Description() const
	{
	_LIT(KDescription, "[[DESCRIPTION]]");
	return KDescription;
	}

void CCmdSkeletonTemplate::ArgumentsL(RCommandArgumentList& aArguments)
	{
	//TODO: aArguments.AppendStringL(iStringArgument, _L("argument-name"), _L("description goes here"));
	}

void CCmdSkeletonTemplate::OptionsL(RCommandOptionList& aOptions)
	{
	//TODO: aOptions.AppendBoolL(iBoolOption, 'o' _L("long-option-name"), _L("description goes here"));
	}

void CCmdSkeletonTemplate::DoRunL()
	{
	// TODO: Add implementation.
	}
