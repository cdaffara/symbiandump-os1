// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Limitations:
// Leavescan will attempt to avoid comments, strings and char literals which it
// finds within the code. It is also capable of recognising escaped quotes,
// but complex combinations of escaping e.g. "\\\"\"\'" may fool the parser and
// result in false positives, or cause parsing to terminate prematurely (resulting
// in possible unreported problems.
// Leavescan can only parse what it sees, and is therefore oblivious to any 
// non-trivial attempt to call a leaving function (i.e. by function pointer)
// Macros can also fool the parser (as is), but leavscan will now indicate
// whether the leaving invocation may, in fact, be a macro.
// Multiple files are now supported on the command-line, along with some basic
// options such as '-v' which will dump the version. Invoking with no 
// parameters will dump the version & help, so that automated build systems can
// check the version and cater for enhanced behaviour.
// Local Version:
// $Id: leavescan2.cpp,v 1.6 2003/02/19 12:59:00 AndrewHa Exp $
// 
//


#include <fstream>
#include <iostream>
#include <sstream>   
#include <string>
#include <assert.h>
#include <list>

#define FOREVER for(;;)

//static const char Kversion[] = "0.06"; // Version updated 07 March 08
static const char Kversion[] = "0.07";  // suprrot L class, fix some defects.Version updated 07/2008
// Shorthand
typedef unsigned int uint;

// Set of match types. E_Last must be at the end of the list.
enum TMatchTypes { 
  E_LCleanedUp=0,
  E_LString,
  E_LManaged,
  E_LData,
  E_Ell , 
  E_Trap, 
  E_BlockComment, 
  E_LineComment, 
  E_Eleave, 
  E_UserEleave, 
  E_String,
  E_Quote,
  E_OpenBrace,
  E_CloseBrace,
  E_OrLeave,
  E_Last };

// Set of return values of GetFunc().
enum TReturnTypes {
  E_False = 0,
  E_True,
  E_Continue };
  
enum TPPState { 
   PPSTATE_NORMAL=0,
   PPSTATE_ESCAPED_LINE ,
   PPSTATE_SINGLE_COMMENT,
   PPSTATE_MUL_COMMENT,
   PPSTATE_STRING_LITERAL,
   PPSTATE_SHARP_COMMENT
  };

enum TClassMemberTypes { 
    E_NA= 0, 
    E_MemberData, 
    E_MemberFunctionDecl, 
    E_MemberFunctionDef, 
    E_Enum
    };

using namespace std;
struct ClassMeta {
	 string className ;
	 int startPos;
	 int endPos;
};

class TLeaveScanModel
{
public:
  bool  Scan(ifstream& aStream);
  void  SetFileName(char* aName);
  char* FileName();
  int   Verbose;
  bool OnlyPrePrecess;

private:	
  void  FileToText(ifstream& aStream, string& aText);
  enum  TReturnTypes  GetFunc(const string& aFile, string &name, uint &start, uint &end);
  bool  CheckFunc(const string& aFile, string& aName, uint start, uint end, bool& aIsLeaver);
  bool  MoveToEndOfTrapHarness(const string& aStr, uint& aIndex, uint aLen);
  void  MoveToEndOfComment(const string& aStr, uint& aIndex, uint aLen);
  void  MoveToEndOfLine(const string& aStr, uint& aIndex, uint aLen);
  void  MoveToEndOfString(const string& aStr, uint& aIndex, uint aLen, char aQuote);
  uint  GetRealStartPosition(const string& aFile, uint aStart, uint aIsvalidDoublecolon);
  void  Peek(const string aStr, uint aPos);
  uint  SmartFind(const string& aStr, const string& aTarget, uint aStart);
  uint  SmartFindEndOfString(const string& aStr, uint aPos, char aQuote);
  bool  IsInCommentScopeOrKeyword(const string& aFile, string &aName, uint aStartposition, uint aStartbraceposn);
  bool  IsFunctionNameStringNotAMacro(const string& aFile, uint aStartbraceposn, uint aTempParenPos, uint aNstart);
  bool  IsSemiColonAndAssignmentOccurs(const string& aFile, uint aStartbraceposn, uint aTempParenPos);
  bool  CheckValidFuncName(const string& aFile, uint aStart);
  bool  CheckForTemplateFunc(const string& aFile, uint nstart, uint nend);
  uint  FindFirstMatchType(uint aMatchSet[]);
  string PreProcess(const string& aFile);
  string CombineLine(const string& aFile);
  list<ClassMeta> GetClass(const string& aFile);
  int GetLeftParenthesis(const string& aText, uint pos);
  int GetRightParenthesis(const string& aText, uint pos);
  int GetLeftTmpltParentBack(const string& aText,uint pos);
  int GetRightTmpltParent(const string& aText,uint pos);
  bool IsClassKeyword(const string& aText, uint aBegin, uint aEnd);
  int GetLeftBracket(const string& aText,uint pos);
  int GetRightBracket(const string& aText,uint pos);
  void CheckClass(const string& aText , const string& aClassName, uint start,uint end);
  void CheckInClassFunc(const string& aText , const string& aClassName, const string& aFuncName, uint start,uint end);
  bool IsLClassCtor(const string& aName);
  bool IsInClassScope(uint pos);
  string GetFunctNameFromFuncHead(const string& aFuncHead);
  string GetFunctionParam(const string& aText, uint aStart);
  string GetFunctionHead(const string& aText, uint aStart);
  string GetErrorLineHead(const string& aText,uint aStart);
  string GetClassName(uint aStart);
  void  ReportErrorLineAndString(uint aStart);
  bool  AppendCommentToErrorString(uint aEnd);
  bool  IdentifyMacro(const string& str, const uint aPos);
  void  Test1();
  void  Test2L();
  void  Test3();
  void  Test4();
  
  inline bool IsNewLine(const char& aCurChar) {return ((aCurChar)=='\n'||(aCurChar)=='\r')?true:false;}
  inline bool IsLastChar(uint aCurPos, uint aStringLength) { return ((aCurPos+1)==aStringLength)?true:false;}
  inline bool HasEnoughChar(uint aCurPos,uint aStringLength,uint aWanted) {return (aStringLength-aCurPos-1>=aWanted)?true:false;}
  inline string TrimString(const string& aStr)
  {
	string result = aStr;
	result.erase(result.find_last_not_of(" \t\n\r")+1);
	result.erase(0,result.find_first_not_of(" \t\n\r"));
	return result;
  }
private:
  string	iText;
  string	iBody;
  string	iErrorString;
  list<ClassMeta> iClassList;
  int	        iPositionOfError;
  char*         iFileName;
  
};

void pversion()
{
  cout << "Leavescan version: " << Kversion <<"(build time:"<<__DATE__<<" "<<__TIME__<<")\n";
}

void pusage()
{
  pversion();
  cout << "syntax: \n\tleavescan [-h|-n|-v|-N] <iFilename.cpp> [<iFilename.cpp> ...]\n\n";
  cout << "\t-h: This help.\n";
  cout << "\t-n: Noisy output - provides diagnostics (if available).\n";
  cout << "\t-N: Very noisy output - provides diagnostics (if available).\n";
  cout << "\t-v: Displays version (for build & automation systems).\n";
  cout << "\t-E: Only do pre-process\n\n";

}

int main(int argc, char** argv)
{
  int noisy = 0;
  bool onlyPreProcess =false;
  if (argc < 2)
    {
      pusage();
      return 1;
    }
  
  for (int clparam = 1; clparam < argc; clparam++)
    {
      if (argv[clparam][0] == '-')
	{
	  switch(argv[clparam][1])
	    {
	    case 'n':
	      noisy = 1;
	      break;
	    case 'N':
	      noisy = 2;
	      break;
	    case 'h':
	      pusage();
	      break;
	    case 'v':
	      pversion();
	      break;
	    case 'E':
	    	{
	    		onlyPreProcess = true;
	    	}
	      break;
	    default:
	      pusage();
	      break;
	    }
	} //if
      else
	{
	  // invoked once per file
	  TLeaveScanModel model;
	  model.SetFileName(argv[clparam]);
	  model.Verbose = noisy;
	  if(onlyPreProcess)
	  {
		  model.OnlyPrePrecess = true;
	  }
	  else
	  {
		  model.OnlyPrePrecess = false;
	  }
	  ifstream file(model.FileName());
	  (void) model.Scan(file);
	  file.close();
	  //	  return 0;
	}
    }
  return 0;
}
  
void TLeaveScanModel::SetFileName(char *aName)
{
  iFileName = aName;
}

char* TLeaveScanModel::FileName()
{
  return iFileName;
}

//
// Main processing function which converts the file to a basic::string
// and then searches through for methods, analysing them as they are
// located.
bool TLeaveScanModel::Scan(ifstream& aStream)
{
  FileToText(aStream, iText);
  string oldText(iText);
  iText = PreProcess(iText);
  if(OnlyPrePrecess)
	  {
	  	cout<<iText;
	  	return false;
	  }
  if(!iText.length())
    {
      cout << "File not found or empty.\n";
      return false;
    }
  
  uint start  = 0;
  uint end    = 0;
  bool isLeaver;
  enum TReturnTypes getFuncReturnTypes;
  //init class list
  iClassList = GetClass(iText);
  //check function
  while (start < iText.length())
    {
      string name;
	  getFuncReturnTypes = E_Continue;
	  while(getFuncReturnTypes == E_Continue)
	  {
		  getFuncReturnTypes = GetFunc(iText, name, start, end);
	  }
      if (!getFuncReturnTypes)
		break;
      
      bool clean = CheckFunc(iText, name, start, end, isLeaver);
      bool isLCFunc =false;
      if(name[name.length()-1] == 'C' && name[name.length()-2] == 'L')
      {
    	  isLCFunc = true;
      }
      if (isLeaver) // was a leaving container(safe) and no leavers found
      {
    	  
    	  if (clean)
    	  {
    		  iPositionOfError=0;
    		  iErrorString = "Warning - " + name + " appears to contain no leavers.";
    		  if(!IsInClassScope(start))
			  {
				  ReportErrorLineAndString(start);	  
			  }
    	  }
    	  else if(isLCFunc)
    	  {
    	      
			  uint lcleanedupPos = iErrorString.find("LCleanedup");
			  if(lcleanedupPos!=string::npos)
				{
    	    	  uint callPos = iErrorString.find(" calls ");
				  uint lcleanedupPosInFuncName = name.find("LCleanedup");
				  if(callPos!=string::npos && lcleanedupPosInFuncName!=string::npos && lcleanedupPos<callPos)
				  {;}
				  else
				  {
					iErrorString = "LCleanedup class is used in a function suffixed LC";
    	    		if(!IsInClassScope(start))
					{
						ReportErrorLineAndString(start);	  
					}
				  }
				}
    	  }

    	  // any leavers info can be supressed here since container was leaver
      }
      else // wasn't a leaving container 
      {
    	  if (!clean) // was dirty, so report
    		  if(!IsInClassScope(start))
			  {
				  ReportErrorLineAndString(start);	  
			  }
      }
      start = end;
    }
  //check class 
  list<ClassMeta>::iterator iter; 
  for(iter=iClassList.begin();iter!=iClassList.end();iter++) 
  { 
	  CheckClass(iText,iter->className,iter->startPos,iter->endPos);
  } 
  return true;
}

//
// Spool a file into one large basic::string
//
void TLeaveScanModel::FileToText(ifstream& aStream, string& aText)
{
  char data[1024];
  int len;
  do	
    {
      aStream.read(data,1024);
      len = aStream.gcount();
      aText.append(data, len);
    }
  while(len);
}

// To get the actual position of starting parenthesis for a function definition
//
uint TLeaveScanModel::GetRealStartPosition(const string& aFile,uint aStart, uint aIsvaliddoublecolon)
{
	uint realstartpos = 0;

	// The opening parenthesis which comes fist after opening brace while moving
 	// in backward direction is a valid candidate for function name.
		
	uint firstcurlybrace = SmartFind(aFile, "{", aStart + 1 ); // advance start

	if ((firstcurlybrace == string::npos) || (firstcurlybrace >= aFile.length())) 
	{
		return realstartpos;
	}

	// This variable will store the first paranthesis position
	uint tempParenPos = aFile.find_last_of('(',firstcurlybrace);

	if (tempParenPos == string::npos)
	{
		// No '(' found yet
		return realstartpos;
	}

	if (aIsvaliddoublecolon == 1)
	{ 
		tempParenPos = aStart;	
	}
	
	string whiteSpaceChars = " \t\n\r*&";
	
	// ignore all tab and spaces between function name and opening parenthesis
	uint nend = aFile.find_last_not_of(whiteSpaceChars, --tempParenPos);
	uint pos = aFile.find_last_of(whiteSpaceChars, nend);
	
	if (pos == string::npos)
	{
		return realstartpos;
	}
	else
	{
		return pos;
	}
}
/*
This function checks the following :
1) Whether the name is in comment scope, name is checked for both C-style comments(or block comments) and C++-style comments(Line Comments)
2) The function name is not a keyword having same program construct as function name(i.e. is not among 'while', 'for','if' and 'for'

@internalComponent
@released
@param aFile -  Buffer storing the whole source file contents(input)
@param aName -  String that is to be checked whether it lies in the scope of the comment(input)
@param aStartposition - Starting offset of name in file.(input)
@param aStartbraceposn - Starting offset of function block (input)
@param true - if name is in comment scope(output)
@param false - if name is not in comment scope


Following patterns are handled in this function:
-----------------------------------------------
NOTE: in order to remove compile warning, i use "|" stand for "/" in this section of comment

Pattern 1: String is in the scope of both c-style and C++-style comments
		
	|*
	|| name1
	* |

	
Pattern 2: String is in the scope of both c-style comments

	|*
		name2()
	* |
			
Pattern 3: String is in comment scope and the comment scope llies in a string
			
	printf(" |*  * | || ")
	

Pattern 4: String is in the scope of C++-style comments but C-style comments is also available in souurce file	

	|*

	* |

	|| function() 	
		
		

Pattern 5: String is in the scope of C-style comments but C+++-style comments is also available in souurce file	
		
	|| function() 	
	|*

	* |


Pattern 6: The string is a keyword and the program construct for the keyword is same as for the functions
	for()
	{ } 


Pattern 7: Both C-style and C++-style comments are avialable in file and the way of specifying comments 
          is complex.(i.e. it is difficult to identify whether it is a C-Style comment or C++-style comment
	  ||*******
	   fun() 
	  |******
		**** | 
	  {}

*/
bool TLeaveScanModel::IsInCommentScopeOrKeyword(const string& aFile, string &aName, uint aStartposition,uint aStartbraceposn)
{
	uint blockCommentStartPosn = aFile.rfind("/*",aStartposition);  // In backward direction
	uint lineCommentStartPosn = aFile.rfind("//",aStartposition);
	uint blockCommentEndPosn = aFile.find("*/", aStartposition); // In forward direction
	uint semicolon = aFile.rfind(";",aStartposition);
	uint isvalidlocationforBlock;
	uint isvalidlocationforLine ;

	if( (0 == strcmp(aName.c_str(),"if")) || (0 == strcmp(aName.c_str(),"while")) ||
		(0 == strcmp(aName.c_str(),"for"))|| (0 == strcmp(aName.c_str(),"switch")) ||
		(0 == strcmp(aName.c_str(),"defined")))
	{
		return true;
	}
	// There is no comment of any type either block comment or line comment 
	if(blockCommentStartPosn == string::npos && lineCommentStartPosn == string::npos)
	{
		return false;
	}
	// A semiclon exists and semicolon is followed by both (1) the end of block comment scope and (2) the end of line comment scope
	else if((semicolon != string::npos)&& 
		((semicolon > blockCommentEndPosn) && (blockCommentStartPosn < blockCommentEndPosn))  &&
		(semicolon > lineCommentStartPosn))
	{
		return false;
	}
	else if(lineCommentStartPosn == string::npos)
	{
		// Valid Block comment, verify that the function name by applying smartfind
		isvalidlocationforBlock = SmartFind(aFile, aName, blockCommentStartPosn); 
		if( (isvalidlocationforBlock == string::npos) || // There is no occurrence of name
			((isvalidlocationforBlock > aStartbraceposn) && (isvalidlocationforBlock != string::npos))) // The name exists but that is inside function block
		{
			return true; 
		}
		else
		{
			return false; 
		}
	}
	else if(blockCommentStartPosn == string::npos)
	{
		isvalidlocationforLine = SmartFind(aFile,aName, lineCommentStartPosn); 
		if(	(isvalidlocationforLine == string::npos) || // There is no occurrence of name
			((isvalidlocationforLine > aStartbraceposn) && (isvalidlocationforLine != string::npos) ) ) // The name exists but that is inside function block
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		isvalidlocationforBlock = SmartFind(aFile,aName, blockCommentStartPosn); 
		isvalidlocationforLine = SmartFind(aFile,aName, lineCommentStartPosn);
		if((isvalidlocationforLine == string::npos) )
		{
		  return true;
		}
		else if(( (isvalidlocationforLine > aStartbraceposn) && (isvalidlocationforLine != string::npos)) )
		{
		 return true; 
		}
		else if( (0 == isvalidlocationforBlock) || ((isvalidlocationforBlock > aStartbraceposn) && (isvalidlocationforBlock != string::npos)) )
		{
		 return true; 
		}
		else if(( blockCommentStartPosn <  aStartbraceposn) && (blockCommentEndPosn < aStartbraceposn ) && (blockCommentStartPosn < blockCommentEndPosn) && ((isvalidlocationforBlock == string::npos)))
		{
			return true; 
		}
		else
		{
			return false; 
		}
	}
}

/*
This function checks whether the function name is a macro or not

@internalComponent
@released
@param aFile - Buffer storing the whole source file contents
@param aStartbraceposn - Starting offset of function block (input)
@param aParenPos - End position of function signature
@param aStartposnofname - Starting offset of string that is to be considered as a function name
@param true - If function name is not a macro
@param false - If function name is a macro  

Following patterns are handled in this function:
-----------------------------------------------

Pattern 1: Function is invoked in macro

	#if defined macro()
	#endif
	struct s1
	{

	}

Pattern 2: Function is invoked in macro but the macro itself is in comment scope
	void fun()
	// #if macro()
	// #endif
	{
		struct s1
		{
		
		}
	}
  
Pattern 3: There is parameterized Constructor e.g.

	Foobar(CFileMan* aFileMan) : iFileMan(aFileMan), iCurrentStep(0) {}


Pattern 4: Function is defined on the preprocessor statement e.g 

	#define __KTRACE_ALL(a,p) {if((DEBUGMASK&(a))==(a))p;}

Pattern 5: All the functions name in comment scope will be ignored

   void func() 
	//	#if comments() 
	{
	 
	}

Pattern 6: All the functions name (e.g. comments) in comment scope will be ignored
  
   #if defined()
   // comments() 
   struct{
 	}

*/

bool TLeaveScanModel::IsFunctionNameStringNotAMacro(const string& aFile, uint aStartbraceposn, uint aParenPos, uint aStartposnofname)
{
	uint startofword = 0;
	uint endofword = 0;
	uint tempParenPos = 0;
	bool posn = true;
	string whiteSpaceChars = " \t\n\r";
	uint tempColonPosn = SmartFind(aFile,":",aParenPos); 
	string  colonString(":");
	bool isColonInCommentScope = IsInCommentScopeOrKeyword(aFile,colonString,aParenPos,aStartbraceposn);
	if(	(tempColonPosn != string::npos)      &&
		(false == isColonInCommentScope)     &&
		(tempColonPosn < aStartbraceposn) )
	{
		return true; // Not a macro
	}
	// The function ignores all the strings which are in the scope of block comments.
	while( startofword < aStartbraceposn && endofword < aStartbraceposn && posn == true)
	{
		if(tempParenPos == 0)
		{
			tempParenPos = aParenPos; // First word
		}
		else
		{
			tempParenPos = endofword; // Next word
		}
		startofword = aFile.find_first_not_of(whiteSpaceChars, tempParenPos+1);
		if(startofword != aStartbraceposn) // No string between function signature and function start block
		{
			// get the posn of word end boundary
			endofword = aFile.find_first_of(whiteSpaceChars, startofword);
			string tmpString = aFile.substr(startofword,endofword-startofword);
			string &aName = tmpString;
			//is throw?
			if(0==aName.find("throw"))
			{
				return true;
			}
			if(	(0 == strcmp(aName.c_str(),"const")) ||
				(0 == strncmp(aName.c_str(),"//",2)) ||
				(0 == strncmp(aName.c_str(),"*/",2)) ||
				(0 == strncmp(aName.c_str(),"/*",2)) )
			{
				// do nothing
			}
			else
			{
				// If string is in comment scope then it is fine i.e. the  function returns true;
				posn = IsInCommentScopeOrKeyword(aFile,aName,startofword,aStartbraceposn);
			}
		}
		else
		{
			//	Pattern 1: only spaces are there between ')' and '{' e.g. void fun() {}
			posn = true;
		}
	}
	if(posn == false)
	{
		return false;
	}
	uint endOfLine=aFile.rfind('\n',aStartposnofname);  // Get the new line position while backtracking 
    uint aIndex=(endOfLine==string::npos ? 0 : endOfLine);
	uint preprocessorDirective = aFile.rfind("#",aStartposnofname); // Get the first char of the line starting with #
	if(preprocessorDirective != string::npos && aIndex <= preprocessorDirective)
	{
		return false; // // Yes, the first char is #, so the name on this line can not be a valid function name
	}
	// Check whether it is a macro 
	return true;
}



/*
This function checks whether any semocolon (';') or assignment(=) is encountered 
when the backtracking is done to look for function name. 
  
@internalComponent
@released
@param aFile - Buffer storing the whole source file contents
@param aStartbraceposn - Starting offset of function block (input)
@param aTempParenPos - End position of function signature
@param true - if either assignment expression '=' or ':' occurs between function signature and starting curly brace of function block.
@param false - if neither assignment expression '=' nor ':' occurs between function signature and starting curly brace of function block.


Following patterns are handled in this function:
-----------------------------------------------

Pattern 1: The semicolon is encountered while backtracking in the search of function name

	_LT();
	struct s1
	{

	}

Pattern 2: The semicolon is encountered while backtracking in the search of function name

  fun();
  a = 2;
  struct str1
  {
  
  }

Pattern 3: The assignment is encountered while backtracking in the search of function name

  voidmain()
  a[2] = 
  {
	1,2,3
  }
 
*/

bool TLeaveScanModel::IsSemiColonAndAssignmentOccurs(const string& aFile,uint aStartbraceposn ,uint aTempParenPos)
{
	string  semicolon(";");
	string  assignment("=");
	uint tempSemicolonPosn = SmartFind(aFile,semicolon,aTempParenPos-1); 
	//uint tempAssignMentPosn = SmartFind(aFile,assignment,aTempParenPos-1); 
	bool isSemicolonInCommentScope = IsInCommentScopeOrKeyword(aFile,semicolon,aTempParenPos,aStartbraceposn);
	bool isAssignmentInCommentScope = IsInCommentScopeOrKeyword(aFile,assignment,aTempParenPos,aStartbraceposn);
	if( (tempSemicolonPosn != string::npos)   &&
		((false == isSemicolonInCommentScope) || (false == isAssignmentInCommentScope)) &&
		(tempSemicolonPosn < aStartbraceposn) 
	  )
	{
		return false;
	}
	return true;
}

/*
This function checks whether the function definition is terminated by semi colon

If the function definition is terminated with semicolon, then it is
more likely to be a class/struct declaration rather than a function definition.

@internalComponent
@released
@param aFile - Buffer storing the whole source file contents
@param aStart - Starting offset of the function name string
@return true -  If closing brace corresponding to function names is followed by semicolon 
@return false - If closing brace corresponding to function names is not by semicolon

Following patterns are handled in this function:
-----------------------------------------------

Pattern 1: NONSHARABLE_CLASS/NONSHARABLE_STRUCT modifier is used

	NONSHARABLE_CLASS(...)
	{

	}; 

Pattern 2: function definition is terminated by semicolon 

	aaa(...)
	{

	} // comment
	; 

Pattern 3: Class is defined in macro

	#define MACRO(arg) class name { };

*/

bool TLeaveScanModel::CheckValidFuncName(const string &aFile, uint aStart)
{
  bool returnValue=false;
  unsigned long uiNumberOfCurlyBraces = 1;
  unsigned int TempPosn = aStart;
  unsigned int open_brace = 0;
  unsigned int close_brace = 0;
  // Continue till than at least any of open curly or close curly brace is remaining
  while( !(open_brace == string::npos) || !(close_brace == string::npos) )
  {
		  open_brace = SmartFind(aFile, "{", TempPosn+1);
		  close_brace = SmartFind(aFile, "}", TempPosn+1);

		  if(open_brace < close_brace) // Handle which curly comes first
		  {
			  if((open_brace != string::npos)&& open_brace < aFile.size())
			  {
				uiNumberOfCurlyBraces++; // Update the curly counter
				TempPosn = open_brace;   // move forward to the value from where the search of next curly to be started
			  }
		  }
		  else // close brace comes first
		  {
			  if((close_brace != string::npos)&& close_brace < aFile.size())
			  {
				uiNumberOfCurlyBraces--;
				TempPosn = close_brace; // move forward to the value from where the search of next curly to be started
			  }
			  if(uiNumberOfCurlyBraces == 0) // Match to the corresponding close curly
			  {
				break;
			  }
		  }
  }
  if(0 == uiNumberOfCurlyBraces)
  {
	  string whiteSpaceChars = " \t\n\r";
	  unsigned int braceFollowsSemicolon1 = 0;
	  unsigned int braceFollowsSemicolon2 = 0;

	  braceFollowsSemicolon1 = SmartFind(aFile, ";", close_brace); // check if any semocolon exists after }
	  braceFollowsSemicolon2 = aFile.find_first_not_of(whiteSpaceChars,close_brace+1); // Get the next character after close_brace

	  if( (braceFollowsSemicolon1 == braceFollowsSemicolon2) && (braceFollowsSemicolon1 !=  string::npos) &&   (braceFollowsSemicolon2 !=  string::npos))
	  {
	   returnValue = true;
	  }
  }

  return returnValue;
}
/*
This function checks whether the function name is specified with template class definition

@internalComponent
@released
@param nstart - starting offset of the function name string 
@param nend - end offset of the function name string
@return true -  If function name is specified with template class functtion definition
@return false - If function name is not specified with template class functtion definition

Following patterns are handled in this function:
-----------------------------------------------

Pattern 1: 

  
template <class T, class S, class L, class R>
GLDEF_C void TestTDes<T,S,L,R>::Test1()
{
	T a(_TL("ABc"));
}
*/

bool TLeaveScanModel::CheckForTemplateFunc(const string& aFile,uint nstart,uint nend)
{
	bool returnValue = false;
	uint OpennAnglebracket = aFile.find_first_of("<", nstart); // advance start
	if(nend == OpennAnglebracket ) // The first invalid char name is <
	{
		uint OpennParen = aFile.find_first_of("(", nstart); // advance start
		uint CloseAnglebracket = SmartFind(aFile, ">::", nstart );
		if( (CloseAnglebracket < OpennParen) && (CloseAnglebracket > OpennAnglebracket))
		{
			returnValue = true;
		}
	}
	return returnValue;
}

// Seek out mathod functions and return their name and start/end bounds in the
// string file.
//
enum TReturnTypes TLeaveScanModel::GetFunc(const string& aFile, string &aName, uint &start, uint &end)
{
  uint nstart = 0;//, cstart = 0;
  uint initStart = start;
  int realnstart = string::npos;
  uint safety = 1000000;
  string validNameChars = ":~abcdefghilkjmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890";
  uint prevstart = string::npos;
  FOREVER// do // forever 
    {
	if(
		(start == string::npos) ||
		(prevstart == start && prevstart != string::npos)
	 )
	{
		return E_False;
	}
	prevstart = start;

      if (safety-- == 0)
	{
	  cout << "ERROR: GetFunc context failure in " << iFileName << "\n";
	  exit(6);
	}

	// move start along to 'real' double-colon
	uint startColon = SmartFind(aFile, "::", start);
	uint parenPostn = SmartFind(aFile, "(", start + 1);
     
	if (startColon == string::npos || startColon >= aFile.length() )
	{
		// Function Definition do not have any :: and function body contains
  		nstart = start; 
		// For 1st function, the value of start will be zero and for the
		// second function onwards, it will start after the first function ends
	}
	else
	{
		uint Realstartpost  = GetRealStartPosition(aFile,parenPostn,0);
		// If no real function name exists after double colon
	//	if (Realstartpost == 0)
	//		return E_False;
		uint Realstartopnparen = SmartFind(aFile, "(", Realstartpost + 1); 

		if(startColon < Realstartopnparen)
		{
			// If the double colon is used for function name then get function name which exists after ::
			nstart = startColon;
			//Get funcName before :: for "La :: La"
			//step1 ':: L...'
			uint fileLen = aFile.length();
			int tmpnstart=nstart+2;
			bool step1 = false;
			while (tmpnstart<fileLen)
			{
				char curChar = aFile[tmpnstart];
				if (curChar==' ' || curChar =='\n' || curChar=='\r' ||curChar=='\t')
				{
					tmpnstart++;
				}
				else if(curChar=='L')
				{
					step1=true;
					break;
				}
				else
				{
					step1 = false;
					break;
				}
			}
			if(step1)
			{
				//step2 L...(<>)? :: L... -->skip all the blank
				bool step2 = true;
				tmpnstart = nstart-1;
				while (tmpnstart>=0)
				{
					char curChar = aFile[tmpnstart];
					if (curChar==' ' || curChar =='\n' || curChar=='\r' ||curChar=='\t')
					{
						tmpnstart--;
					}
					else
					{
						break;
					}
				}
				if(tmpnstart<=0)
				{
					tmpnstart = 0;
					step2 = false;
				}
				else
				{
					char curChar = aFile[tmpnstart];
					if (curChar=='>')
					{
						tmpnstart = GetLeftTmpltParentBack(aFile,tmpnstart-1);
						if(tmpnstart==string::npos)
						{
							//bad pattern
							step2 =false;
						}
						else
						{
							tmpnstart --;
						}
					}
					if(tmpnstart==string::npos)
					{
						//bad pattern
						step2 =false;
					}
					else
					{
						
						while (tmpnstart>=0)
						{
							curChar = aFile[tmpnstart];
							if (curChar==' ' || curChar =='\n' || curChar=='\r' ||curChar=='\t')
							{
								tmpnstart--;
							}
							else
							{
								break;
							}
						}
						if(tmpnstart <=0) 
						{
							tmpnstart=0;
							step2=false;
						}
						while (tmpnstart>=0)
						{
							curChar = aFile[tmpnstart];
							if (curChar=='_' || (curChar>='a' && curChar<='z') || (curChar>='A' && curChar<='Z') ||(curChar>='0' && curChar<='9'))
							{
								tmpnstart--;
							}
							else
							{
								break;
							}
						}
						tmpnstart++;
						if (tmpnstart<0)
						{
							tmpnstart=0;
							step2=false;
						}
						if ( step2 && aFile[tmpnstart]=='L')
						{
							realnstart = tmpnstart;
						}
					}

				}
			}
		}
		else
		{
			// start looking for function name after '('
			nstart = aFile.find_last_of(validNameChars,parenPostn-1);
			if(nstart == string::npos)
				return E_False;
		}
	}
    
	uint semiPos = SmartFind(aFile, ";", nstart + 1); 

	if (semiPos == string::npos || semiPos >= aFile.length())
		return E_False;

	uint parenPos = SmartFind(aFile, "(", nstart + 1);
    if (parenPos == string::npos || parenPos >= aFile.length()) 
		return E_False;

	start = SmartFind(aFile, "{", nstart + 1); // advance start
	if (start == string::npos || start >= aFile.length())
		return E_False;

	uint nstart2 = SmartFind(aFile, "::", nstart + 1); // next fn
	// Either (1) Second instance of Double collon is not there and first occurrence of double colon is valid
	// or (2) second occurrence of double colon is inside the function and first occurrence of double colon is valid
	
	if((nstart2 == string::npos && startColon < parenPos && startColon < start)  || ( nstart2 > start && startColon < parenPos && startColon < start ))
	{
		// This variable will store the first paranthesis position in a temporary variable
		// uint TempparenPos = parenPos;
		uint Realstartpost  = GetRealStartPosition(aFile,parenPos,1 /*To indicate that a valid double colon exists*/);
	
		nstart = Realstartpost+1; // reset
		
		break;
	}
	// Either Second occurrence of Double-colon is invalid  
	// or Second occurrence of Double-colon exists inside the function 
	// while first occurrence of double colon is valid
	else if (nstart2 == string::npos || nstart2 >= aFile.length() || (nstart2 != string::npos && nstart2 > start && startColon < parenPos && startColon < start))
	{
		// The opening parenthesis which comes fist after opening brace while moving
 		// in backward direction is a valid candidate for function name.
		
		// This variable will store the first paranthesis position
		uint tempParenPos = aFile.find_last_of('(',start);
		
		
		//void foo() throw () {;}
		if(tempParenPos != string::npos)
		{
 			uint throwPos = aFile.find("throw",initStart);
			if(throwPos!=string::npos && throwPos>=initStart && throwPos<start)
			{
					tempParenPos = aFile.find_last_of('(',throwPos);
					if(tempParenPos==string::npos || tempParenPos<initStart)
					{
						tempParenPos = aFile.find_last_of('(',throwPos);
					}
			}
		}

		if (tempParenPos == string::npos)
		{
			//FIX this pattern
			//class A{};
			//void foo(){fooL();}
			if(start!=string::npos && semiPos!=string::npos && parenPos!=string::npos&&start<semiPos&&semiPos<parenPos)
			{
				start=semiPos+1;
				return E_Continue;
			}
			//FIX this pattern
			//class B;
			//class B{};
			//void foo(){fooL();}
			else if(start!=string::npos && semiPos!=string::npos && parenPos!=string::npos&&start<parenPos&&semiPos<start)
			{
				int tmpRightBracket = GetRightBracket(aFile,start+1);
				int tmpNextLeftBracket = GetLeftBracket(aFile,tmpRightBracket);
				int tmpSemiPos = aFile.find(";",tmpRightBracket);
				if(tmpRightBracket!=string::npos&&tmpNextLeftBracket!=string::npos&&tmpSemiPos!=string::npos)
				{
					if(tmpRightBracket<tmpSemiPos && tmpSemiPos<tmpNextLeftBracket)
					{
						//skip the content before B{};
						start = tmpSemiPos;
						return E_Continue;
					}
				}
			}
			//pattern:
			//class temp
			//{
			//LData func();
			//};
			//LData func() 
			//{
			//	foo();
			//}
			if(start!=string::npos && semiPos!=string::npos && parenPos!=string::npos&&start<semiPos&&semiPos>parenPos)
			{
				int tmpRightBracket = GetRightBracket(aFile,semiPos);
				if(tmpRightBracket!=string::npos)
				{
					start = tmpRightBracket;
					return E_Continue;
				}
			}
			
			// No '(' found yet
			return E_False;
		}
		

		string whiteSpaceChars = " \t\n\r*&";

		// ignore all tab and spaces and pointer between function name and opening parenthesis
		uint nend = aFile.find_last_not_of(whiteSpaceChars, --tempParenPos);
		uint currPos = aFile.find_last_of(whiteSpaceChars, nend);
	
		uint startOfBlock = SmartFind(aFile, "{", tempParenPos ); // advance start

		if (startOfBlock == string::npos || startOfBlock >= aFile.length())
 			return E_False;

		if (start < tempParenPos || nstart2 < tempParenPos)
		{
			start = startOfBlock;
		}

	 	if ((currPos != string::npos) && (currPos <  tempParenPos)) // fn1 before fn2
			nstart = currPos+1; // reset
	 	break;
	}

	if (semiPos < start) // semi came before brace = prototype
		start = semiPos; 
	else
	if (nstart < nstart2 // fn1 before fn2
	    && nstart2 < parenPos) // fn2 before paren 
		start = nstart2; // reset
	else
		break;
  } //while (true);
  
  // nstart should be left pointing to first of double colons
  // start should point to curly brace

  assert(aFile[start] == '{');
  
  // locate the end of the method name
  nstart = aFile.find_first_of(validNameChars,nstart);
  if(nstart == string::npos)
	  return E_False;

  int nend = aFile.find_first_not_of(validNameChars, nstart);
  if(true == CheckForTemplateFunc(aFile,nstart,nend))
  {
	nend = aFile.find_first_of("(",nstart);
  }
  
  if(realnstart!=string::npos && realnstart<nstart)
  {
	aName = aFile.substr(realnstart, nend - realnstart);
  }
  else
  {
	aName = aFile.substr(nstart, nend - nstart);
  }
  //operator
  
  if(aName.find("operator")!=string::npos)
  {
	  nend = aFile.find_first_of("(",nstart);
	  while(nend>=0)
	  {
		  char tmpChar = aFile[nend];
		  if(tmpChar == ' ' || tmpChar == '\n' ||tmpChar == '\t' ||tmpChar == 'r')
		  {
			  nend--;
		  }
		  else
		  {
			  break;
		  }
	  }
	  aName = aFile.substr(nstart, nend - nstart);
  }
int typeEnd = aFile.find_last_of(validNameChars,nstart-1);
int typeStart= aFile.find_last_of(" \t\n\r*&",typeEnd)+1;
string type=TrimString(aFile.substr(typeStart, typeEnd - typeStart+1));
  uint closeparenposn = aFile.find_first_of(")", nstart); // advance start

  if(
	 (false == IsFunctionNameStringNotAMacro(aFile,start,closeparenposn,nstart)) ||
	 (false == IsSemiColonAndAssignmentOccurs(aFile,start,closeparenposn))
	)
  {
	
	start = SmartFind(aFile, "}", start); 
	return E_Continue;
  }
  
  // rewind nstart until non valid char found
  safety = 1000000;
  while (validNameChars.find_first_of(aFile[--nstart]) != string::npos)
    {
      if (safety-- == 0)
	{
	  cout << "ERROR: GetFunc context failure in " << iFileName << "\n";
	  exit(7);
	}
    }
  
  nstart++;   // skip forward to acceptable character

  // cut name out
  if(realnstart!=string::npos && realnstart<nstart)
  {
	aName = aFile.substr(realnstart, nend - realnstart);
  }
  else
  {
	aName = aFile.substr(nstart, nend - nstart);
  }
  
  // truncate the name with zero

  if(realnstart!=string::npos && realnstart<nstart)
  {
	  aName[nend-realnstart] = 0;
  }
  else
  {
	  aName[nend-nstart] = 0;
  }
  aName = TrimString(aName);
  uint NameSpaceinFunctionName = SmartFind(aName, "::",0);
  // Continue processing when Either function name is in comment scope
  // Or close curly corresponding to function name is followed by semicolon and pattern
  // for scope resolution operator does not exists in function name
  if(((true == IsInCommentScopeOrKeyword(aFile,aName,nstart,start))) || 
	  ( (string::npos == NameSpaceinFunctionName) &&  ((true == CheckValidFuncName(aFile,start))))) 
  {
	  // Yes it is in comment scope
	  start = SmartFind(aFile, "}", start); 
	  return E_Continue;
  }

  if (Verbose)
    cout << "Processing method " << iFileName << ": " << aName << "()\n";
  
  end = start+1;  // start was a curly brace, so place end inside the brace
  int braceCount = 1;   // count the one we've just stepped over & prime loop
  //uint sanity = 0;
  uint lpos = end;
  uint match[E_Last];
  uint braceDepth = 1;

  // keep progressing 'end' until braces match, end finishes up at brace+1
  while (braceCount > 0)
    {
      if (Verbose > 1)
	cout << ".";

      // reset all types (in case we don't use all of them)
      int counter;
      for (counter = E_LCleanedUp; counter < E_Last; counter++)
	match[counter] = string::npos;

      match[E_OpenBrace]    = aFile.find("{",           lpos);
      match[E_CloseBrace]   = aFile.find("}",           lpos);
      match[E_BlockComment] = aFile.find("/*",          lpos);
      match[E_LineComment]  = aFile.find("//",          lpos);
      match[E_String]       = aFile.find("\"",          lpos);
      match[E_Quote]        = aFile.find("\'",          lpos);

      // Check for at least one match-type
      bool nothingFound = true;
      for(counter = E_LCleanedUp; counter < E_Last; counter++)
	{
	  if(match[counter] != string::npos) // if found an item
	    nothingFound = false;
	}

      if(nothingFound) 
	{
	  if (Verbose > 1)
	    cout << "\n";
	  cout << "ERROR: " << iFileName << ":" << aName << " failed brace check.\n";
	  exit(1);
	}
      
      // Find the match-type which occurs first and set
      // lps to its position
      uint lowestMatchType = FindFirstMatchType(match);
      lpos                 = match[lowestMatchType];
      
      switch(lowestMatchType)
	{
	case E_BlockComment:
	  MoveToEndOfComment(aFile, lpos, string::npos); // lpos modified
	  break;

	case E_LineComment:
	  MoveToEndOfLine(aFile, lpos, string::npos); // lpos modified
	  break;

	case E_OpenBrace:
	  braceCount++;
	  if (braceCount > (int)braceDepth)
	    braceDepth = braceCount;
	  lpos++;
	  break;
	  
	case E_CloseBrace:
	  braceCount--;
	  lpos++;
	  break;

	case E_String:
	  // we're sat on a quote, so advance
	  lpos++;
	  //	  Peek(file,lpos);
	  lpos = SmartFindEndOfString(aFile, lpos, '\"');
	  if (lpos != string::npos)
	    lpos++; // skip closing quote
	  break;
	  
	case E_Quote:
	  lpos++;
	  lpos = SmartFindEndOfString(aFile, lpos, '\'');
	  if (lpos != string::npos)
	    lpos++; // skip closing quote
	  break;

	default:
	  if (Verbose > 1)
	    cout << "\n";
	  cout << "ERROR: " << iFileName << ":" << aName << " bad match type.\n";
	  exit(1);
	} //endsw
      
      if (lpos == string::npos || lpos == 0 || lpos > aFile.length())
	{
	  if (Verbose > 1)
	    cout << "\n";
	  cout << "ERROR: " << iFileName << ":" << aName << " failed brace check (EOF).\n";
	  exit(1);
	}
      
    } //endwhile

  // advance 'end'
  end = lpos;

  if (braceDepth > 10)
    {
      if (Verbose > 1)
	cout << "\n";
      cout << "WARNING: " << iFileName << ":" << aName << " unexpectedly large brace depth.\n";
    }
  return E_True;
}

// Diagnostic function for displaying the contents of a string
// starting at 'pos'
//
void TLeaveScanModel::Peek(const string aStr, uint aPos)
{
#define PEEKBUFFSIZE 20
  static char peekBuf[PEEKBUFFSIZE];
  int posn = aPos;
  int remaining = aStr.length() - posn;
  
  if (remaining > (PEEKBUFFSIZE - 1))
    remaining = PEEKBUFFSIZE;
  
  for (int i = 0; i < remaining; i++)
    peekBuf[i] = aStr[i+posn];
  
  peekBuf[remaining] = 0;
  
  cout << "-------- Peek -----------\n" << peekBuf << "\n";
  cout << "-------------------------\n";
#undef PEEKBUFFSIZE
}

// Given an array of match types, work out which one actually comes first
// in the string. Effectively this function returns the match type with the
// lowest 'pos' value.
//
uint TLeaveScanModel::FindFirstMatchType(uint aMatchSet[])
{
  // load indexOfLowestMatch with the item which comes first in the fn
  int lowestMatch = 0;
  for(int i = E_LCleanedUp; i < E_Last; i++) 
    {
      if((aMatchSet[lowestMatch] == string::npos) // if didn't find one
	 ||(aMatchSet[lowestMatch] > aMatchSet[i] && aMatchSet[i] != string::npos))
	lowestMatch = i;
    }
  //  cout << "Lowest match found: " << (int)lowestMatch << "\n";
  return lowestMatch;
}

//
// Given a string, see if it looks like a macro (e.g. is capitalised)
//
bool  TLeaveScanModel::IdentifyMacro(const string& str, const uint aPos)
{
  string validNameChars = ":~abcdefghilkjmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890";
  string LowerNameChars = "abcdefghilkjmnopqrstuvwxyz";

  // backwind until not valid
  int start = str.find_last_not_of(validNameChars, aPos);

  // check for any lower case between start and pos
  int lower = str.find_first_of(LowerNameChars, start);

  bool result = false;
  if (lower < (int)aPos)
    result = false; // lower case == not macro
  else
    result = true;
  return result;
}

// Given a method function bound, search thorough it looking for what appear to be
// leaving functions (unless it's a leaver itself).
//
// Returns false if error was detected signifying that caller should dump the 
// error string.
//
bool TLeaveScanModel::CheckFunc(const string& aFile, string &aName, 
				uint start, uint end, bool& aIsLeaver)
{
  // entry criteria: name points to the container function name (after double-colon) which
  // is now null-terminated.
  // end is the bound for the end of the function (closing curly brace)
  string extraTypes="CDX";
  // Look for container name ending in L, or LC, LD, or LX
  bool isLCFunc =false;
  aName = TrimString(aName);
  
  string className="";
    className = GetClassName( start);
    if(className!="")
    {
    	  aName = className+"::"+aName;
    }
	
  if (Verbose > 1)
    {
      cout << "Evaluating code:\n";
      Peek(aFile, start);
    }

  if (aName[aName.length()-1] == 'L' || 
      (aName[aName.length()-2] == 'L' && string::npos != extraTypes.find(aName[aName.length()-1]) ) )
    //return true; // happy - we can exit since this container is marked as a leaver
    aIsLeaver = true; // container is marked Leaver - used to return here
  else if(IsLClassCtor(aName))
  {
	  aIsLeaver = true;
  }
  
  else
    aIsLeaver = false; // container not marked Leaver
  
  if (aName[aName.length()-1] == 'C' && aName[aName.length()-2] == 'L' )
  {
	 isLCFunc =true;
  }
  //CheckFuncDecl
  //CheckFuncParam
  int classNamePos = aFile.rfind(aName, start);
  if(classNamePos == string::npos)
  {
	  uint coloncolonPos =  aName.find("::");
	  if(coloncolonPos != string::npos)
	  {
		  string shortFuncName = aName.substr(coloncolonPos+2);
		  classNamePos = aFile.rfind(shortFuncName,start);
	  }
  }
  string funcParam = GetFunctionParam(aFile, classNamePos-1);
  if(funcParam != "")
  {
  	if(funcParam.find("LString")!=string::npos||(funcParam.find("LData")!=string::npos))
	{
  		if(!aIsLeaver)
  		{
  			cout<<GetErrorLineHead(aFile,classNamePos)<<aName + " uses LString/LData class as parameter"<<endl;	
  		}
	}
  }
  //CheckFuncHead
  string funcHead = GetFunctionHead(aFile, classNamePos-1);
  if(funcHead != "")
  {
	 if(funcHead.find("LString")!=string::npos||(funcHead.find("LData")!=string::npos))
	 {
	  	if(!aIsLeaver) 
	  	{
	  		cout<<GetErrorLineHead(aFile,classNamePos)<<aName + " returns LString/LData class"<<endl;	
	  	}
	 }
  }
  // remainder of function exists to deal with non-leaving container
  bool ok      = true;
  uint bodyLen = end - start;
  iBody        = aFile.substr(start, bodyLen); // bound the string to end

  uint lpos = 0; 
  uint match[E_Last]; // create some search-result slots
  string extraTypesAndSpace=" CDX";

  bool keepProcessing = true; // keep going unless we run out of data or bail
  uint sanity = 1000000;

  while (lpos < bodyLen && keepProcessing == true) 
    {
      if (sanity-- == 0)
	{
	  cout << "\nERROR: " << iFileName << ": " << aName << " context failure.\n";
	  exit(4);
	}
      if (Verbose > 1)
	cout << "?";
      // Scans for a whole set of items from position lpos.
      // string::npos returned if not found

      // reset all types (in case we don't use all of them)
      int counter;
      for (counter = E_LCleanedUp; counter < E_Last; counter++)
	match[counter] = string::npos;

      match[E_Ell]          = iBody.find("L",           lpos); 
      match[E_Trap]         = iBody.find("TRAP",        lpos); 
      match[E_BlockComment] = iBody.find("/*",          lpos);
      match[E_LineComment]  = iBody.find("//",          lpos);
      match[E_Eleave]       = iBody.find("ELeave",      lpos);
      match[E_UserEleave]   = iBody.find("User::Leave", lpos);
      match[E_String]       = iBody.find("\"",          lpos);
      match[E_Quote]        = iBody.find("\'",          lpos);
      match[E_OrLeave]		= iBody.find("OR_LEAVE",	lpos);
      match[E_LCleanedUp]	= iBody.find("LCleanedup",	lpos);
      match[E_LString]		= iBody.find("LString",		lpos);
      match[E_LManaged]		= iBody.find("LManaged",	lpos);
      match[E_LData]		= iBody.find("LData",	    lpos);
      
      // Check for at least one match-type
      bool doBreak = true;
      for(counter = E_LCleanedUp; counter < E_Last; counter++)
	{
	  if(match[counter] != string::npos) // if found an item
	    doBreak = false; 
	}

      if(doBreak) 
	break; // found nothing of interest in this method function
      
      // Find the match-type which occurs first and set
      // lps to its position
      uint lowestMatchType = FindFirstMatchType(match);
      lpos                 = match[lowestMatchType];
      
      switch(lowestMatchType) // what type was it?
	{
	case E_BlockComment:
	  MoveToEndOfComment(iBody, lpos, bodyLen); // lpos modified
	  break;

	case E_LineComment:
	  MoveToEndOfLine(iBody, lpos, bodyLen); // lpos modified
	  break;

	case E_Trap:
	  if (! MoveToEndOfTrapHarness(iBody, lpos, bodyLen)) // lpos modified
	    {
              iErrorString     = aName + " structure Fault-Unclosed bracket after TRAP harness.";
              iPositionOfError = lpos;
	      keepProcessing   = false; // bail
	    }
	  break;

	case E_Eleave:
	  ok               = false;
	  iErrorString     = aName + " calls new(ELeave).";
	  iPositionOfError = match[E_Eleave];
	  keepProcessing   = false; // bail
	  break;
	case E_OrLeave:
		ok               = false;
		iErrorString     = aName + " calls OR_LEAVE.";
		iPositionOfError = match[E_OrLeave];
		keepProcessing   = false; // bail
		break;
	case E_LCleanedUp:
		ok               = false;
		iErrorString     = aName + " uses LCleanedup* class.";
		iPositionOfError = match[E_LCleanedUp];
		keepProcessing   = false; // bail
		break;
	case E_LString:
		ok               = false;
		iErrorString     = aName + " uses LString* class.";
		iPositionOfError = match[E_LString];
		keepProcessing   = false; // bail
		break;
	case E_LManaged:
		ok               = false;
		iErrorString     = aName + " uses LManaged* class.";
		iPositionOfError = match[E_LManaged];
		keepProcessing   = false; // bail
		break;
	
	case E_LData:
			ok               = false;
			iErrorString     = aName + " uses LData* class.";
			iPositionOfError = match[E_LData];
			keepProcessing   = false; // bail
			break;
			
	case E_UserEleave:
	  ok               = false;
	  iErrorString     = aName + " calls User::Leave() or User::LeaveIfError().";
	  iPositionOfError = match[E_UserEleave];
	  AppendCommentToErrorString(bodyLen);
	  keepProcessing   = false; // bail
	  break;
	  
	
	case E_Ell:
	  // Found an L, but is there an underbar before it, 
	  // AND (one of 'CDX ' AND a brace OR just a brace) _L[CDX ](
	  if ((iBody[lpos - 1] != '_') &&
	      (((string::npos != extraTypesAndSpace.find(iBody[lpos + 1])) 
		&& (iBody[lpos + 2] == '(')) || (iBody[lpos + 1] == '(')) ) 
	    {
	      if (IdentifyMacro(iBody, lpos))
		{
		  ok = false;
		  iErrorString = aName + " may employ a macro.";
		  iPositionOfError = match[E_Ell];
		  if(!AppendCommentToErrorString(bodyLen))
		    keepProcessing = false; // keep going if possible
		  lpos++;
		  break; // don't bail - false alarm
		}
	      else
		{
		  if (Verbose > 1)
		    {
		      cout << "Leaver found: \n";
		      Peek(iBody, lpos);
		    }
		}
	      ok = false;
	      iErrorString = aName + " calls a function that can leave.";
	      iPositionOfError = match[E_Ell];
	      if(!AppendCommentToErrorString(bodyLen))
		keepProcessing = false; // keep going if possible
	    }
	  lpos++; // move on over the L
	  break;
	  
	case E_String:
	  lpos++;
	  lpos = SmartFindEndOfString(iBody, lpos, '\"');
	  if (lpos == string::npos)
	    keepProcessing = false;
	  else
	    lpos++; // skip closing quote
	  break;
	  
	case E_Quote:
	  lpos++;
	  lpos = SmartFindEndOfString(iBody, lpos, '\'');
	  if (lpos == string::npos)
	    keepProcessing = false;
	  else
	    lpos++; // skip closing quote
	  break;

	default:
	  cout << "\nERROR: " << iFileName << ": " << aName << " unexpected match type.\n";
	  exit(1);

	} // endsw
	//LCleanedup and LManaged cann't be TRAPed
	/*
	if (match[E_LCleanedUp]==string::npos && match[E_LManaged]==string::npos)
	{
	}
	if(match[E_LCleanedUp]!=string::npos && match[E_LManaged]==string::npos)
	{
		ok               = false;
		iErrorString     = aName + " uses LCleanedup class.";
		iPositionOfError = match[E_LCleanedUp];
		keepProcessing   = false; // bail
		break;
	}
	else if(match[E_LManaged]!=string::npos && match[E_LCleanedUp]==string::npos)
	{
		ok               = false;
		iErrorString     = aName + " uses LManaged* class.";
		iPositionOfError = match[E_LManaged];
		keepProcessing   = false; // bail
		break;
	}
	else if(match[E_LCleanedUp]<match[E_LManaged])
	{
		ok               = false;
		iErrorString     = aName + " uses LCleanedup class.";
		iPositionOfError = match[E_LCleanedUp];
		keepProcessing   = false; // bail
		break;
	}
	else if(match[E_LCleanedUp]>match[E_LManaged])
	{
		ok               = false;
		iErrorString     = aName + " uses LManaged* class.";
		iPositionOfError = match[E_LManaged];
		keepProcessing   = false; // bail
		break;
	}
	*/
    //check:An LCleanedup class is used in the same function body as the Classic cleanup stack API
    if(match[E_LCleanedUp]!=string::npos)
      {
    	uint newLCPos = iBody.find("NewLC");
		uint pushLPos = string::npos; //Cleanedup::PushL()-->find Cleanedup
		uint bodyLen = iBody.length();
		for(uint tmpPos = 0;tmpPos<bodyLen;)
		{
			pushLPos = iBody.find("CleanupStack",tmpPos);
			if (pushLPos == string::npos)
			{
				break;
			}
			if(pushLPos>0)
			{
				if(iBody[pushLPos-1]=='L')
				{
					tmpPos++;
					pushLPos = string::npos;
					continue;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		
		//uint new
    	uint lcleanedUpPos = match[E_LCleanedUp];
    	if(newLCPos!=string::npos && pushLPos!=string::npos)
    	{
    		if(newLCPos<pushLPos)
    		{
    			cout<<GetErrorLineHead(aFile,start+newLCPos)<<aName<<" uses NewLC method with LCleanedup* class.\n";
    			break;
    		}
    		else
    		{
    			cout<<GetErrorLineHead(aFile,start+pushLPos)<<aName<<" uses classic CleanupStack method with LCleanedup* class.\n";
    			break;
    		}
    	}
    	else if(newLCPos!=string::npos)
    	{
    		cout<<GetErrorLineHead(aFile,start+newLCPos)<<aName<<" uses NewLC method with LCleanedup* class.\n";
    		break;
    	}
    	else if(pushLPos!=string::npos)
    	{
    		cout<<GetErrorLineHead(aFile,start+pushLPos)<<aName<<" uses classic CleanupStack with LCleanedup* class.\n";
    		break;
    	}
    	
      }
    } // endwhile
  
  if (Verbose > 1)
    cout << "END\n";

  return ok;
}

//
// Move an index forward to the start of a close-comment
//
void TLeaveScanModel::MoveToEndOfComment(const string& aStr, uint& aIndex, uint aLen)
{
  uint endOfComment=aStr.find("*/",aIndex);
  aIndex=(endOfComment == string::npos ? aLen : endOfComment+2);
}

//
// Move an index forward to the EOL (\n)
//
void TLeaveScanModel::MoveToEndOfLine(const string& aStr, uint& aIndex, uint aLen)
{
  uint endOfLine=aStr.find('\n',aIndex);
  aIndex=(endOfLine==string::npos ? aLen : endOfLine);
}

//
// Move an index forward to a closing quote (specified by x)
//
void TLeaveScanModel::MoveToEndOfString(const string& aStr, uint& aIndex, uint aLen, char aQuote)
{
  uint endOfLine=aStr.find(aQuote,aIndex);
  aIndex=(endOfLine==string::npos ? aLen : endOfLine);
}

//
// Move to end of a trap harness
//
bool TLeaveScanModel::MoveToEndOfTrapHarness(const string& aStr, uint& aIndex, uint aLen)
{
  uint bracketLevel=1;
  uint closeBracket;
  uint openBracket=aStr.find('(',aIndex);
  bool cleanExit=true;
  uint newIndex;
  for(newIndex = openBracket; bracketLevel;)
    {
      closeBracket=aStr.find(')',newIndex+1);
      openBracket=aStr.find('(',newIndex+1);
      if (openBracket<closeBracket && openBracket!=string::npos)
	{
	  bracketLevel++;
	  newIndex=openBracket;
	}
      else if ((closeBracket<openBracket && closeBracket!=string::npos) 
	       || (closeBracket!=string::npos && openBracket==string::npos)) 
	{
	  bracketLevel--;
	  newIndex=closeBracket;
	}
      else //bad structure
	{
	  cleanExit=false;
	  break;
	}
    }
  aIndex=(cleanExit ? newIndex : aLen);
  return cleanExit;
}

//
// Report the current error - note works on class member iText
//
void TLeaveScanModel::ReportErrorLineAndString(uint aStart)
{
  int positionInBody=aStart+iPositionOfError;
  int positionOfNextLine=iText.find('\n',positionInBody+1);
  if(positionOfNextLine==string::npos)
  positionOfNextLine=iText.length()-1; //the first char could be '\n'
  int positionOfLine=-1;
  int lineNum;
  for(lineNum=0 ;positionOfLine<positionOfNextLine;lineNum++)
    positionOfLine=iText.find('\n',positionOfLine+1);
 
  cout << iFileName <<"("<< lineNum << ") : " << iErrorString << "\n";
}

//calculate filename ,fileline
string TLeaveScanModel::GetErrorLineHead(const string& aText, uint aPos)
{
	int lineNum=1;
	char curChar=0;
	//i did not count '\r'
	for(int curPos=0;curPos<=aPos;curPos++)
	{
		curChar = aText[curPos];
		if(curChar=='\n')
		{
			lineNum++;
		}
	}
	stringstream   sstrm;
	sstrm<<iFileName << string("(")<<lineNum<<string(") : ");
	return sstrm.str();
}
//Get a class name from class list
//may return wrong name if there are inner classes
string TLeaveScanModel::GetClassName(uint aStart)
{
	list<ClassMeta>::iterator iter; 
	for(iter=iClassList.begin();iter!=iClassList.end();iter++) 
	{ 
		if(iter->startPos<=aStart && iter->endPos >= aStart)
		{
			return iter->className;
		}
	}
	return "";
}
//
// Add to the current error string - note works on class member iBody
//
bool TLeaveScanModel::AppendCommentToErrorString(uint aEnd)
{
  bool qualified = true;
  uint commentPosition   = iBody.find("//",iPositionOfError+1);
  uint endOfLinePosition = iBody.find('\n',iPositionOfError+1);
  if((commentPosition < endOfLinePosition) && (commentPosition != string::npos))
    iErrorString+=" QUALIFIED WITH-> "+iBody.substr(commentPosition,endOfLinePosition-commentPosition);
  else if(endOfLinePosition==string::npos && commentPosition!=string::npos)
    iErrorString+=" QUALIFIED WITH-> "+iBody.substr(commentPosition,aEnd-commentPosition);
  else //no comment
    {
      iErrorString+=" UnQualified.";
      qualified = false;
    }
  return qualified;
}

//
// Find the end of a string, but cope with escaped quotes
//
uint TLeaveScanModel::SmartFindEndOfString(const string& aStr, uint aPos, char aQuote)
{
  FOREVER//while(true)
    {
       MoveToEndOfString(aStr, aPos, string::npos, aQuote);
      if (aPos == string::npos) // end of fn
	  return aPos;

      if (aStr[aPos - 1] != '\\')
	return aPos;

      // looks like we have an escaped quote
      if (aStr[aPos - 2] == '\\')
	{ // escape was itself quoted, so string end is real
	  return aPos;	
	}
      else
	{ // quote is escaped - carry on
	  if (Verbose > 1)
	    cout << "\n";
	  cout << "WARNING: " << iFileName << ": detected quoted string.\n";
	  aPos++;
	  continue;
	}
    } //while
}

//
// Find a target string from a given start position, but ignore targets buried
// within comments, or strings.
//
uint  TLeaveScanModel::SmartFind(const string& aStr, const string& aTarget, uint aStart)
{
  uint found,cstart;
  FOREVER//while(1)
    {
      //      cout << "Searching for target from: " << aStart << "\n";
      // record found target 
      found = aStr.find(aTarget, aStart);
      if (found == string::npos)
	  return(string::npos);

      // cout << "Target candidate found at: " << found << "\n";
      // look for earlier comment
      cstart = aStr.find("//", aStart);

      if (cstart < found) // earlier
	{
	  aStart = cstart; // reset start to after comment
	  // cout << "Earlier comment found at: " << start << "\n";
	  MoveToEndOfLine(aStr, aStart, string::npos);
	  // cout << "Start advanced to: " << aStart << "\n";
	  if (aStart == string::npos)
	    return(aStart);
	  else
	    continue; // start search again
	}
      
      // do the same for block comments
      cstart = aStr.find("/*", aStart);
      if (cstart < found)
	{
	  aStart = cstart;
	  // cout << "Earlier block found at: " << aStart << "\n";
	  MoveToEndOfComment(aStr, aStart, string::npos);
	  // cout << "Start advanced to: " << aStart << "\n";
	  if (aStart == string::npos)
	    return(aStart);
	  else
	    continue;
	}
      
      cstart = aStr.find("\"", aStart);

      if (cstart < found)
	{
	  aStart = cstart + 1;
	  MoveToEndOfString(aStr, aStart, string::npos, '\"');
	  if (aStart == string::npos)
	    return(aStart);
	  else
	    {
	      aStart++;
	      continue;
	    }
	}

      cstart = aStr.find("\'", aStart);
      if (cstart < found)
	{
	  aStart = cstart + 1;
	  MoveToEndOfString(aStr, aStart, string::npos, '\'');
	  if (aStart == string::npos)
	    return(aStart);
	  else
	    {
	      aStart++;
	      continue;
	    }
	}
      return(found); // quit loop
    } // endwhile
}


/*
 * Decide the "class" is keyword
 * e.g
 *  class -> true
 *  Aclass ->false
 *  classA ->false
 */
bool TLeaveScanModel::IsClassKeyword(const string& aText, uint aBegin, uint aEnd)
{
	bool tmpBefore =false;
	bool tmpAfter =false;
	int len = aText.size();
	if(aBegin==0)
	{
		tmpBefore = true;
	}
	else
	{
		char tmpChar = aText[aBegin-1];
		if(tmpChar==' ' || tmpChar=='\n' || tmpChar=='\r' || tmpChar=='\t' || tmpChar=='{'|| tmpChar=='}'|| tmpChar==';')
		{
			tmpBefore = true;
		}
	}
	if(aEnd==len-1)
	{
		tmpAfter = true;
	}
	else
	{
		char tmpChar = aText[aEnd+1];
		if(tmpChar==' ' || tmpChar=='\n' || tmpChar=='\r' || tmpChar=='{' || tmpChar=='('|| tmpChar==':')
		{
			tmpAfter = true;
		}
	}
	return tmpAfter&&tmpBefore;
}
/**
 * combine escaped lines to one line.
 */
string TLeaveScanModel::CombineLine(const string& aText)
{

	if(aText.size()<0) return string("");
	int curPos = 0;
	int curState = PPSTATE_NORMAL;
	int firstPos = 0;
	int stringSize = aText.size();
	int newlineCount=0;
	string newText("");
	while(curPos< stringSize)
	{
		//remove "\"
		switch(curState)
		{
		case PPSTATE_NORMAL:
		{
			char curChar=aText[curPos];
			if(curChar=='\\')
			{
				// '\\\n" | "\\\r"
				if(stringSize>(curPos+1)&&IsNewLine(aText[curPos+1]))
				{
					curState= PPSTATE_ESCAPED_LINE;				
					newText.append(aText.substr(firstPos,curPos-firstPos));
					curPos+=1;
					newlineCount+=1;
					
				}
				else if(IsLastChar(curPos,stringSize))
				{
					curPos++;
					firstPos++;
				}
				else
				{
					curPos++;
				}
			}
			else if(IsNewLine(curChar))
			{
				if (newlineCount>0)
				{
					newText.append(aText.substr(firstPos,curPos-firstPos));
					while(newlineCount>= 0)
					{
						newText.append("\n");
						newlineCount--;
					}
					newlineCount=0;
					curPos++;
					firstPos=curPos;
				}
				else
				{
					curPos++;
				}
			}
			else
			{
				curPos++;
			}
		}
			break;
		case PPSTATE_ESCAPED_LINE:
			if(stringSize-curPos>1)
			{
				if(
					(aText[curPos]=='\n'&&aText[curPos+1]=='\r')
					||
					(aText[curPos]=='\r'&&aText[curPos+1]=='\n')
					)
				{
					curPos+=2;
					firstPos=curPos;
					curState=PPSTATE_NORMAL;
				}
				else if(IsNewLine(aText[curPos]))
				{
					curPos+=1;
					firstPos=curPos;
					curState=PPSTATE_NORMAL;
				}
			}
			else
			{
				//EOF
				return newText;
			}
		
		}
	}
	newText.append(aText.substr(firstPos,curPos-firstPos));
	return newText;
}

/**
 * C Preprocess
 * a.remove single line comment
 * b.remove mul-lines comment
 * c.remove #command
 * d.struct,NONSHARABLE_CLASS,NONSHARABLE_STRUCT --> class
 * e.string literal -> blank string
 */
string TLeaveScanModel::PreProcess(const string& aText)
{
	string afterCombine = CombineLine(aText);
	//cout<<"before:"<<endl<<afterCombine<<endl;
	if(afterCombine.size()<0) return string("");
	
	int curPos = 0;
	int curState = PPSTATE_NORMAL;
	int firstPos = 0;
	int stringSize = aText.size();
	int newlineCount=0;
	int blankCount=2;
	string newText("");
	while(curPos< stringSize)
	{
		switch(curState)
		{
			case PPSTATE_NORMAL:
            {
				char curChar = afterCombine[curPos];
				if(curChar=='/')
				{
					if(HasEnoughChar(curPos,stringSize,1))
					{
						if('/'==afterCombine[curPos+1])
						{
							curState=PPSTATE_SINGLE_COMMENT;
							newText.append(afterCombine.substr(firstPos,curPos-firstPos));
							curPos+=2;
						}
						else if('*'==afterCombine[curPos+1])
						{
							curState=PPSTATE_MUL_COMMENT;
							newText.append(afterCombine.substr(firstPos,curPos-firstPos));
							curPos+=2;
						}
						else
						{
							curPos++;
						}
					}
					else
					{
						curPos++;
					}
				}
				else if(curChar=='"')
				{
					if(curPos>0&&(afterCombine[curPos-1]!='\\'&&afterCombine[curPos-1]!='\''))
					{
						curState=PPSTATE_STRING_LITERAL;
						newText.append(afterCombine.substr(firstPos,curPos-firstPos+1));
						
					}
					curPos+=1;
				}
				else if(curChar=='#')
				{
					//if # is the first non-blank char of line, comment this line
					bool checkSharp=true;
					for(int i=curPos-1;i>=0;i--)
					{
						char tmpChar = afterCombine[i];
						if(tmpChar==' '||tmpChar=='\t'||tmpChar=='\f')
						{
							continue;
						}
						else if(tmpChar=='\n' || tmpChar=='\t')
						{
							break;
						
						}
						else
						{
							checkSharp=false;
							break;
						}
					}
					if(checkSharp)
					{
						curState=PPSTATE_SHARP_COMMENT;
						newText.append(afterCombine.substr(firstPos,curPos-firstPos));
					}
					curPos+=1;
				}
				//struct,NONSHARABLE_CLASS,NONSHARABLE_STRUCT --> class
				else if(HasEnoughChar(curPos,stringSize,5))
				{
					
					if(curChar=='s'&&afterCombine[curPos+1]=='t'&&afterCombine[curPos+2]=='r'&&afterCombine[curPos+3]=='u'&&afterCombine[curPos+4]=='c'&&afterCombine[curPos+5]=='t'&&IsClassKeyword(aText,curPos,curPos+5))
					{
						newText.append(afterCombine.substr(firstPos,curPos-firstPos));
						newText.append("class ");
						curPos+=6;
						firstPos=curPos;
					}
					else if(curChar=='N'&&HasEnoughChar(curPos,stringSize,11)&&afterCombine[curPos+1]=='O'&&afterCombine[curPos+2]=='N'&&afterCombine[curPos+3]=='S'&&afterCombine[curPos+4]=='H'&&afterCombine[curPos+5]=='A'&&afterCombine[curPos+6]=='R'&&afterCombine[curPos+7]=='A'&&afterCombine[curPos+8]=='B'&&afterCombine[curPos+9]=='L'&&afterCombine[curPos+10]=='E'&&afterCombine[curPos+11]=='_')
					{
						if(HasEnoughChar(curPos,stringSize,5))
						{
							if(afterCombine[curPos+12]=='C'&&afterCombine[curPos+13]=='L'&&afterCombine[curPos+14]=='A'&&afterCombine[curPos+15]=='S'&&afterCombine[curPos+16]=='S'&&IsClassKeyword(aText,curPos,curPos+16))
							{
								newText.append(afterCombine.substr(firstPos,curPos-firstPos));
								newText.append("class");
								for(int i=0;i<12;i++)
								{
									newText.append(" ");
								}
								curPos+=17;
								firstPos=curPos;
								break;
							}
						}
						if(HasEnoughChar(curPos,stringSize,6))
						{
							if(afterCombine[curPos+12]=='S'&&afterCombine[curPos+13]=='T'&&afterCombine[curPos+14]=='R'&&afterCombine[curPos+15]=='U'&&afterCombine[curPos+16]=='C'&&afterCombine[curPos+17]=='T'&&IsClassKeyword(aText,curPos,curPos+17))
							{
								newText.append(afterCombine.substr(firstPos,curPos-firstPos));
								newText.append("class");
								for(int i=0;i<13;i++)
								{
									newText.append(" ");
								}
								curPos+=18;
								firstPos=curPos;
								break;
							}
						}
						curPos++;
						
					}//end of nonsharable
					else
					{
						curPos++;
					}
				}
				
				else
				{
					curPos++;
				}
				break;
            }
			case PPSTATE_SHARP_COMMENT:
			case PPSTATE_SINGLE_COMMENT:
			{
				while(curPos< stringSize)
				{
					if(IsNewLine(afterCombine[curPos]))
					{
						if(stringSize-curPos>1)
						{
										if(
											(afterCombine[curPos]=='\n'&&afterCombine[curPos+1]=='\r')
											||
											(afterCombine[curPos]=='\r'&&afterCombine[curPos+1]=='\n')
											)
										{
											curPos+=2;									
										}
										else 
										{
											curPos+=1;
										}
										firstPos=curPos;
										curState=PPSTATE_NORMAL;
										newText.append("\n");
										break;
						}
						else
						{					
							newText.append("\n");
							return newText;
						}
					}
					else
					{
						curPos++;
					}
				}
				firstPos=curPos;
			
				break;
			}
			case PPSTATE_MUL_COMMENT:
			{
				blankCount=2;
				while(curPos< stringSize)
				{
					if(!HasEnoughChar(curPos,stringSize,1))
					{
						return newText;
					}
					if(afterCombine[curPos]=='*' && afterCombine[curPos+1]=='/')
					{
						curPos+=2;
						firstPos=curPos;
						curState=PPSTATE_NORMAL;
						blankCount+=2;
						break;
					}
					else if(IsNewLine(afterCombine[curPos]))
					{
						if(HasEnoughChar(curPos,stringSize,1))
						{
							if(
									(afterCombine[curPos]=='\n'&&afterCombine[curPos+1]=='\r')
									||
									(afterCombine[curPos]=='\r'&&afterCombine[curPos+1]=='\n')
								)
							{
										curPos+=2;				
							}
							else
							{
								curPos+=1;
							}
						}
						else 
						{
									curPos+=1;
						}					
						while (blankCount-->0)
						{
							newText.append(" ");
						}
						newText.append("\n");				
						blankCount=0;
					}
					else
					{
						curPos++;
						blankCount++;
					}
				}
				firstPos=curPos;
				while (blankCount-->0)
				{
						newText.append(" ");
				}
				blankCount=0;
			
				break;
			}
			case PPSTATE_STRING_LITERAL:
				blankCount=0;
				while(curPos< stringSize)
								{
									
									if((afterCombine[curPos]=='"' )&& (afterCombine[curPos-1]!='\\'))
									{
										curPos+=1;
										firstPos=curPos;
										while (blankCount-->0)
										{
											newText.append(" ");
										}
										newText.append("\"");
										curState=PPSTATE_NORMAL;
										break;
									}
									else
									{
										if(curPos>1 && (afterCombine[curPos]=='"' )&& (afterCombine[curPos-1]=='\\')&&(afterCombine[curPos-2]=='\\'))
										{
											curPos+=1;
										    firstPos=curPos;
											while (blankCount-->0)
											{
													newText.append(" ");
											}
											newText.append("\"");
											curState=PPSTATE_NORMAL;
											break;
										}
										else
										{
											curPos++;
											blankCount++;
										}
									}
								}
				break;
		}
	}
	newText.append(afterCombine.substr(firstPos,curPos-firstPos));
	return newText;
}
/*
 * Get next matched '{'
 */
int TLeaveScanModel::GetLeftBracket(const string& aText,uint pos)
{
	int len =aText.size();
	if(pos>=len)
	{
		return string::npos;
	}
	while(pos<len)
	{
		if(aText[pos]=='{')
		{
			return pos;
		}
		pos++;
	}
	return string::npos;
}
/*
 * Get next matched '}'
 */
int TLeaveScanModel::GetRightBracket(const string& aText,uint pos)
{
	int len = aText.size();
	if(pos>=len)
	{
		return string::npos;
	}
	int parentCount = 1;
	while(pos<len)
		{
			if(aText[pos]=='{')
			{
				parentCount++;
			}
			else if (aText[pos]=='}' )
			{
				parentCount--;
				if(parentCount==0)
				{
					return pos;
				}
			}
			pos++;
		}
		return string::npos;
	
}
/*
 Get next '('
 */
int TLeaveScanModel::GetLeftParenthesis(const string& aText,uint pos)
{
	int len =aText.size();
	if(pos>=len)
	{
		return string::npos;
	}
	while(pos<len)
	{
		char curChar = aText[pos];
		if(curChar=='(')
		{
			return pos;
		}
		pos++;
	}
	return string::npos;
}

/*
 * Get next matched ')'
 */
int TLeaveScanModel::GetRightParenthesis(const string& aText,uint pos)
{
	int len = aText.size();
		if(pos>=len)
		{
			return string::npos;
		}
	int parentCount = 1;
	while(pos<len)
		{
			char curChar = aText[pos];
			if(curChar=='(')
			{
				parentCount++;
			}
			else if (curChar==')' )
			{
				parentCount--;
				if(parentCount==0)
				{
					return pos;
				}
			}
			pos++;
		}
		return string::npos;
}

/*
 Get next '<' by pair
 */
int TLeaveScanModel::GetLeftTmpltParentBack(const string& aText,uint pos)
{
	int count=1;
	if (pos<=0)
	{
		return string::npos;
	}
	while(pos>0)
	{
		char curChar = aText[pos];
		if(curChar=='<')
		{
			count--;
			if (count <= 0)
				return pos;
		}
		else if(curChar=='>')
		{
			count++;
		}
		pos--;
	}
	return string::npos;
}

/*
 * Get next matched '>'
 */
int TLeaveScanModel::GetRightTmpltParent(const string& aText,uint pos)
{
	int len = aText.size();
		if(pos>=len)
		{
			return string::npos;
		}
	int parentCount = 1;
	while(pos<len)
		{
			char curChar = aText[pos];
			if(curChar=='<')
			{
				parentCount++;
			}
			else if (curChar=='>' )
			{
				parentCount--;
				if(parentCount==0)
				{
					return pos;
				}
			}
			pos++;
		}
		return string::npos;
}

/*
 *Get the start and end position of a class
 */
list<ClassMeta> TLeaveScanModel::GetClass(const string& aFile)
{
	int len= aFile.size();
	int startPos=0;
	int nextClassPos=-1;
	list<ClassMeta> classList;
	//struct ,NONSHARABLE_CLASS,NONSHARABLE_STRUCT should be parsed to 'class' in pp stage
	while((nextClassPos = aFile.find("class",startPos))!=string::npos)
	{
		

		char classSuffix = aFile[nextClassPos+5]; //the char after class
		if(classSuffix!=' '&& classSuffix!='\n' && classSuffix!='\r' && classSuffix!='\t'&& classSuffix!=':'&&classSuffix!='{')
		{
			nextClassPos++;
			startPos=nextClassPos;
			continue;
		}
		/*no use*/
		/*
		if(nextClassPos>0)
		{
			char classPrfix = aFile[nextClassPos-1];
			if (classPrfix =='_' || (classPrfix>='a' && classPrfix<='z') || (classPrfix>='A' && classPrfix<='Z') || (classPrfix>='0' && classPrfix<='9'))
			{
				nextClassPos++;
				startPos=nextClassPos;
				continue;
			}
		}
		*/
		if(nextClassPos >0 )
		{
			char classPrefix = aFile[nextClassPos-1];
			if(classPrefix!=' '&& classPrefix!='\n' && classPrefix!='\r' && classPrefix!='\t'&& classPrefix!='{'&& classPrefix!='>'&& classPrefix!='}')
			{
				nextClassPos++;
				startPos=nextClassPos;
				continue;
			}
		}
		
		//pattern: template< ...class...>
		int checkTemplate = nextClassPos-1;
		bool isTemplateClass = false;
		while(checkTemplate >= 0)
		{
			char checkTempChar = aFile[checkTemplate];
			if(checkTempChar==' '|| checkTempChar=='\n' || checkTempChar=='\r' ||checkTempChar=='\t')
			{
				checkTemplate --;
			}
			else if(checkTempChar=='>') //template<> class
			{
				break;
			}
			else if(checkTempChar=='<')
			{
				isTemplateClass =true;
				break;
			}
			else if(checkTempChar==',') //template<typename a, class T>
			{
				isTemplateClass =true;
				break;	
			}
			else
			{
				break;
			}
		}
		if(isTemplateClass)
		{
			startPos = nextClassPos+1;
			continue;
		}

		int classStart = GetLeftBracket(aFile,nextClassPos);
		if(classStart==-1)
		{
			break;  //there is no more class-body, so we can break dircetly
		}
		//skip this pattern
		//class a;
		uint semiPos = aFile.find(";",nextClassPos);
		//such as this pattern: 
		//class a;
		//class b{};
		if(semiPos!=string::npos&&semiPos<classStart)
		{
			startPos = semiPos;
			continue;
		}

		uint classEnd = GetRightBracket(aFile,classStart+1);
		if(classEnd == -1)
		{
			classEnd = len-1;
		}
		
		//get class name
		int curClassNamePos = classStart-1;
		char curClassChar = 0;
		//find pattern:
		//class a:publlc b{...
		uint colonPos = aFile.find(":",nextClassPos);
		while(colonPos!=string::npos && colonPos<curClassNamePos)
		{
			if(colonPos != string::npos && colonPos < classStart)
			{
				uint secondColonPos = aFile.find(":",colonPos+1);
				if(secondColonPos != string::npos && secondColonPos!=(colonPos+1))
				{
					curClassNamePos = colonPos-1;
					break;
				}
				else if(secondColonPos == string::npos)
				{
					curClassNamePos = colonPos-1;
					break;
				}
				//find ::
				else
				{
					colonPos = aFile.find(":",colonPos+2);
				}
			}
		}
		//skip white spaces
		while(curClassNamePos >=  nextClassPos)
		{
			curClassChar = aFile[curClassNamePos];
			if(curClassChar ==' ' || curClassChar =='\t' || curClassChar =='\n' ||curClassChar=='\r' ||curClassChar==')')
			{
				curClassNamePos--;
			}
			else
			{
				break;
			}
		}
		int classNameEnd = curClassNamePos;
		while(curClassNamePos>=nextClassPos)
		{
			curClassChar = aFile[curClassNamePos];
			if(curClassChar ==' ' || curClassChar =='\t' || curClassChar =='\n' ||curClassChar=='\r'||curClassChar==':'||curClassChar=='(')
			{
				break;
			}
			else
			{
				curClassNamePos--;
			}
		}
		int classNameStart = curClassNamePos+1; //cur is a blank
		string curClassName = TrimString(aFile.substr(classNameStart,classNameEnd-classNameStart+1));
		if (curClassName == "class")
		{
			curClassName = "anonymous-class";
		}
		ClassMeta curClass;
		curClass.startPos=classStart;
		curClass.endPos=classEnd;
		curClass.className = curClassName;
		classList.push_back(curClass);
		startPos=nextClassPos+1;
	}
	return classList;
}

/*
 * parse the class and check it
 * */
void TLeaveScanModel::CheckClass(const string& aText ,const string& aClassName , uint start,uint end)
{
	start=start+1;
	int curPos = start;
	end=end-1;
	if(end<=start) return;
	//int colonCount=0;
	int curStart= curPos;
	int curEnd = curStart;
	int curType = E_NA;
	int curLine = 0; // wrong!!!
	int len = aText.size();
	string functionHead="";
	string functionArgu="";
	string functionBody="";
	string memberDef="";
	
	for(curPos = start;curPos<=end;curPos++)
	{
		char curChar = aText[curPos];
		switch(curChar)
		{
		case ':':
			if (aText[curPos+1]==':')//  a '::'
			{
				//skip second :
				curPos+=1;
			}
			//such as ctor_init_list
			else if(curType == E_MemberFunctionDecl || curType==E_MemberFunctionDef)
			{
				//nothing
			}
			else //such as "public:"  ->ignore
			{
				curStart = curPos+1;
			}
			break;
		case '{':
			if(curType ==E_MemberFunctionDecl )
			{
				curType = E_MemberFunctionDef;
			}
			else
			{
				curType = E_Enum;
			}		
			curEnd = GetRightBracket(aText,curPos+1);
			functionBody = aText.substr(curPos,curEnd-curPos+1);
			//check in class function
			if(E_MemberFunctionDef==curType)
			{
				CheckInClassFunc(aText,aClassName,functionHead,curPos,curEnd);
			}
			curPos = curEnd;
			curStart =curPos;
			curType = E_NA;
			break;
		case '(':
			curType = E_MemberFunctionDecl;
			curEnd = GetRightParenthesis(aText,curPos+1);		
			functionHead = aText.substr(curStart,curPos-curStart);
			functionArgu = aText.substr(curPos,curEnd-curPos+1);
			curPos = curEnd;
			curStart =curPos;
			
			break;
		case ';':
			if(curType == E_NA)
			{
				curType = E_MemberData;
			}
			//check point
			if (curType == E_MemberData)
			{
				memberDef = aText.substr(curStart,curPos-curStart);
				if(memberDef.find("LCleanedup")!=string::npos)
				{
					iErrorString = "class "+aClassName+" defines a data member with LCleanedup* type.\n";
					cout<<GetErrorLineHead(aText, curPos)<<iErrorString;	
				}
			}
			else if(curType == E_MemberFunctionDecl)
			{
				string funcName = GetFunctNameFromFuncHead(functionHead);
				string extraTypes="CDX";
				bool isLeaver = false;
				if (funcName[funcName.length()-1] == 'L' || 
				      (funcName[funcName.length()-2] == 'L' && string::npos != extraTypes.find(funcName[funcName.length()-1]) ) )
				{
					isLeaver = true; // container is marked Leaver - used to return here
				}
				else if(IsLClassCtor(aClassName+"::"+funcName))
				{
					isLeaver = true;
				}
				else
				    isLeaver = false; // container not marked Leaver
				  
				
				if(functionHead.find("LString")!=string::npos ||functionHead.find("LData")!=string::npos)
				{
					iErrorString = aClassName+"::"+funcName+" returns LString/LData class.\n";
					if(!isLeaver) cout<<GetErrorLineHead(aText, curPos)<<iErrorString;
				}
				if(functionArgu.find("LString")!=string::npos ||functionArgu.find("LData")!=string::npos)
				{
					iErrorString = aClassName+"::"+funcName+"  uses LString/LData class as parameter.\n";
					if(!isLeaver) cout<<GetErrorLineHead(aText, curPos)<<iErrorString;
				}
			}
			//set up next check
			curStart = curPos;
			curType =E_NA;
			break;
		case 'c':
			//check if 'class' and skip the inner class
			if (HasEnoughChar(curPos,len,5))
			{
				if ( aText[curPos+1]=='l' && aText[curPos+2]=='a'  && aText[curPos+3]=='s'  && aText[curPos+4]=='s' )
				{
					char delimiter = aText[curPos+5];
					//exclude pattern: void classa ()
					if (!((delimiter >='0' && delimiter <='9') || (delimiter>='a' && delimiter<='z') || (delimiter>='A' && delimiter<='Z') ||(delimiter=='_')))
					{
						delimiter = aText[curPos-1];
						//exclude pattern: void aclass()
						if(curPos>0 && !((delimiter >='0' && delimiter <='9') || (delimiter>='a' && delimiter<='z') || (delimiter>='A' && delimiter<='Z') ||(delimiter=='_')))
						{
							//pattern: template<class T> Ltemp(T x){}
							uint templatePos = aText.find("template",curStart);
							if(templatePos != string::npos && templatePos < curPos)
							{
								continue;
							}
							else
							{
								int tmpLeft = GetLeftBracket(aText,curPos+4);
								int tempRight = GetRightBracket(aText,tmpLeft+1);
								curPos = tempRight;
								curStart = curPos;
								curType = E_NA;
							}
						}
					}
					
				}			
			}
			//ignore
			break;
		default:
			//curPos++;
			break;
		}
		//curPos++;
	}
}
/*
 * Is a ctor of L class
 * */
bool TLeaveScanModel::IsLClassCtor(const string& aName)
{
	
	//if(aName[0]=='L'&&aName.find("::")!=string::npos)%
	//{
	//	return true;
	//}
	uint colonPos = aName.find("::");
	if(colonPos == string::npos)
	{
		return false;
	}
	string s1 = aName.substr(0,colonPos);
	string s2 = aName.substr(colonPos+2);
	s1.erase(s1.find_last_not_of(" \t\n\r")+1);
	s1.erase(0,s1.find_first_not_of(" \t\n\r"));

	s2.erase(s2.find_last_not_of(" \t\n\r")+1);
	s2.erase(0,s2.find_first_not_of(" \t\n\r"));
	

	uint tmpltPos = s1.find('<');
	if(tmpltPos!=string::npos)
	{
		s1 = s1.substr(0,tmpltPos);
	}
	if(s1[0]!='L')
	{
		return false;
	}
	if(s1.find(s2)!=string::npos)
	{
		uint s1Len = s1.length();
		uint s2Len = s2.length();
		//s1=LAL
		//s2=LA
		if(s2Len>0 && s1Len > s2Len)
		{
			char diffChar = s1[s2Len];
			if( (diffChar>='a' && diffChar<='z') || (diffChar>='A' && diffChar<='Z') 
				||(diffChar>='0' && diffChar<='9') || (diffChar=='_'))
			{
				return false;
			}
		}
		return true;
	}
	if(s2.find("operator")==0)
	{
		return true;
	}
	if(s2.find("new")==0)
	{
		return true;
	}

	return false;
}
/*
* Is sometiing defined in class scope?
*/
bool TLeaveScanModel::IsInClassScope(uint pos)
{
	if (pos == string::npos)
	{
		return false;
	}
	list<ClassMeta>::iterator iter; 
	for(iter=iClassList.begin();iter!=iClassList.end();iter++) 
	{ 
		if(iter->startPos<=pos && iter->endPos >= pos)
		{
			return true;
		}
	}
	return false;

}
/*
 * Try to get the function name from a function head
 * */
string TLeaveScanModel::GetFunctNameFromFuncHead(const string& aFuncHead)
{
	string funcName;
	int len = aFuncHead.length();
	int curPos=len-1;
	char curChar;
	int nameStart = -1;
	int nameEnd;
	//skip white spaces
	while(curPos>=0)
	{
		curChar = aFuncHead[curPos];
		if(curChar==' ' || curChar=='\t' ||curChar=='\n'||curChar=='\r')
		{
			curPos--;
		}
		else
		{
			break;
		}	
	}
	if(curChar == '>')
	{
		int templateBalance = 1;
		curPos--;
		while(curPos>=0)
		{
			curChar = aFuncHead[curPos];
			
			if(curChar=='>')
			{
				templateBalance++;
			}
			else if(curChar =='<')
			{
				templateBalance--;
			}
			curPos--;
			if(templateBalance==0)
			{
				break;
			}
			
		}
	}
	if(curPos<=0) return "";
	
	nameEnd = curPos;
	if(aFuncHead.find("operator")!=string::npos)
	{
		int tmpNameStart = aFuncHead.find("operator");
		char delimiter = aFuncHead[tmpNameStart+8];
		if (!((delimiter >='0' && delimiter <='9') || (delimiter>='a' && delimiter<='z') || (delimiter>='A' && delimiter<='Z') ||(delimiter=='_')))
		{
			if(tmpNameStart>0)
			{
				delimiter = aFuncHead[tmpNameStart-1];
				if (!((delimiter >='0' && delimiter <='9') || (delimiter>='a' && delimiter<='z') || (delimiter>='A' && delimiter<='Z') ||(delimiter=='_')))
				{
					nameStart = tmpNameStart;
				}
			}
			if(tmpNameStart==0)
			{
				nameStart = tmpNameStart;
			}
		}				
	}
	if(nameStart == -1)
	{
		while(curPos>=0)
		{
			curChar = aFuncHead[curPos];
			if(curChar==' ' || curChar=='\t' ||curChar=='\n'||curChar=='\r'||curChar=='>')
			{
				nameStart = curPos+1;
				if(nameStart<0)
				{
					nameStart = 0;
				}
				break;
				
			}
			else
			{
				curPos--;
			}	
			
		}
		if(curPos<0)
		{
			nameStart = 0;
		}
	}
	funcName = aFuncHead.substr(nameStart,nameEnd-nameStart+1);
	return funcName;

	
}
/*
 * Get the Param string of a function decl
 * aStart usually is the position of '{'
 * */
string TLeaveScanModel::GetFunctionParam(const string& aText, uint aStart)
{
	int curPos = aStart;
	if(curPos == string::npos)
	{
			return "";
	}
	int leftParenthesis  = GetLeftParenthesis(aText, curPos);
	int rightParenthesis = GetRightParenthesis(aText, leftParenthesis+1);
	
	if(leftParenthesis==string::npos || rightParenthesis==string::npos)
	{
		return "";
	}
	else if(rightParenthesis>leftParenthesis) 
	{
		return aText.substr(leftParenthesis+1, rightParenthesis-leftParenthesis-1);
	}
	return "";
	
	
}

/*Get the string from last member decl to aStart*/
string TLeaveScanModel::GetFunctionHead(const string& aText, uint aStart)
{
	int curPos = aStart;
	char curChar;
	if(curPos==string::npos || curPos<=0)
	{
		return "";
	}
	while(curPos >= 0)
	{
		curChar = aText[curPos];
		if(curChar == '{' || curChar == '}' || curChar == ';')
		{
			break;
		}
		curPos--;
	}
	if(curPos<0)
	{
		curPos = 0;
	}
        
	int tempPos = string::npos;
	if( (aText.rfind("public",aStart)!=string::npos)||(aText.rfind("protected",aStart))!=string::npos ||(aText.rfind("private",aStart))!=string::npos)
	{
		tempPos = aText.rfind(':',aStart);
                 
	}
        if(tempPos>curPos)
	{
		curPos = tempPos;
	}
	return aText.substr(curPos,aStart-curPos);

}
/*Chech the function decl/define in a class*/
void TLeaveScanModel::CheckInClassFunc(const string& aText , const string& aClassName, const string& aFuncName, uint start,uint end)
{
	bool isLeaver;
	bool clean;
	string funcName = TrimString(GetFunctNameFromFuncHead(aFuncName));
	string qualifiedFuncName = aClassName+"::"+funcName;
	clean = CheckFunc(aText, funcName, start, end, isLeaver);
	bool isLCFunc =false;
	if(funcName[funcName.length()-1] == 'C' && funcName[funcName.length()-2] == 'L')
	{
	    isLCFunc = true;
	}
	if (isLeaver) // was a leaving container(safe) and no leavers found
	{
	
	    if (clean)
	    {
	    	iPositionOfError=0;
	    	iErrorString = "Warning - " + funcName + " appears to contain no leavers.";
	        ReportErrorLineAndString(start);	  
	    }
	    else if(isLCFunc)
    	  {
    	      
			  uint lcleanedupPos = iErrorString.find("LCleanedup");
			  if(lcleanedupPos!=string::npos)
				{
    	    	  uint callPos = iErrorString.find(" calls ");
				  uint lcleanedupPosInFuncName = funcName.find("LCleanedup");
				  if(callPos!=string::npos && lcleanedupPosInFuncName!=string::npos && lcleanedupPos<callPos)
				  {;}
				  else
				  {
					iErrorString = "LCleanedup class is used in a function suffixed LC";
    	    		ReportErrorLineAndString(start);
				  }
				}
    	  }

	    // any leavers info can be supressed here since container was leaver
	  }
	  else // wasn't a leaving container 
	  {
	    if (!clean) // was dirty, so report
	    	ReportErrorLineAndString(start);
	  }
};

//
// Some test functions (for when leavescan parses its own source code)
//
void TLeaveScanModel::Test1()
{
  Test2L();
}

void TLeaveScanModel::Test2L()
{
  Test1();
}

#define TEST2L Test2L
void TLeaveScanModel::Test3()
{
  TEST2L();
}

void TLeaveScanModel::Test4()
{
  Test2L(); // qualify me
};



// warnings back to default levels
//#pragma warning (pop) 

