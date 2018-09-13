#pragma once

#include "unistr.h"
#include <vector>
#include <string>

	/*!
	\brief
	Text utility support class.  This class is all static members.  You do not create instances of this class.
	*/
class CTextUtils
{
public:
	/*************************************************************************
	Constants
	*************************************************************************/
	static const std::wstring DefaultWhitespace;      //!< The default set of whitespace
	static const std::wstring DefaultAlphanumerical;  //!< default set of alphanumericals.
	static const std::wstring DefaultWrapDelimiters;  //!< The default set of word-wrap delimiters


												/*************************************************************************
												Methods
												*************************************************************************/
												/*!
												\brief
												return a String containing the the next word in a String.
												This method returns a String object containing the the word, starting at index \a start_idx, of String \a str
												as delimited by the code points specified in string \a delimiters (or the ends of the input string).
												\param str
												String object containing the input data.
												\param start_idx
												index into \a str where the search for the next word is to begin.  Defaults to start of \a str.
												\param delimiters
												String object containing the set of delimiter code points to be used when determining the start and end
												points of a word in string \a str.  Defaults to whitespace.
												\return
												String object containing the next \a delimiters delimited word from \a str, starting at index \a start_idx.
												*/
	static  std::wstring  getNextWord ( const std::wstring& str, std::wstring::size_type start_idx = 0, const std::wstring& delimiters = DefaultWhitespace );


	/*!
	\brief
	Return the index of the first character of the word at \a idx.
	/note
	This currently uses DefaultWhitespace and DefaultAlphanumerical to determine groupings for what constitutes a 'word'.
	\param str
	String containing text.
	\param idx
	Index into \a str where search for start of word is to begin.
	\return
	Index into \a str which marks the begining of the word at index \a idx.
	*/
	static  std::wstring::size_type   getWordStartIdx ( const std::wstring& str, std::wstring::size_type idx );


	/*!
	\brief
	Return the index of the first character of the word after the word at \a idx.
	/note
	This currently uses DefaultWhitespace and DefaultAlphanumerical to determine groupings for what constitutes a 'word'.
	\param str
	String containing text.
	\param idx
	Index into \a str where search is to begin.
	\return
	Index into \a str which marks the begining of the word at after the word at index \a idx.
	If \a idx is within the last word, then the return is the last index in \a str.
	*/
	static  std::wstring::size_type   getNextWordStartIdx ( const std::wstring& str, std::wstring::size_type idx );


	/*!
	\brief
	Trim all characters from the set specified in \a chars from the begining of \a str.
	\param str
	String object to be trimmed.
	\param chars
	String object containing the set of code points to be trimmed.
	*/
	static  void    trimLeadingChars (std::wstring& str, const std::wstring& chars );


	/*!
	\brief
	Trim all characters from the set specified in \a chars from the end of \a str.
	\param str
	String object to be trimmed.
	\param chars
	String object containing the set of code points to be trimmed.
	*/
	static  void    trimTrailingChars (std::wstring& str, const std::wstring& chars );

private:
	/*************************************************************************
	Data
	*************************************************************************/
	static  std::wstring      d_delimiters;           //!< Current set of delimiters.
	static  std::wstring      d_whitespace;           //!< Current set of whitespace.


												/*************************************************************************
												Construction / Destruction
												*************************************************************************/
												/*!
												\brief
												Constructor and Destructor are private.  This class has all static members.
												*/
	CTextUtils ( void );
	~CTextUtils ( void );
};