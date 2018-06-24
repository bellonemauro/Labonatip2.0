/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

//modified version from here https://stackoverflow.com/questions/12978973/unknown-ouput-with-qdebugstream-and-qtextedit  

#ifndef QDEBUGSTREAM_H
#define QDEBUGSTREAM_H

#include <iostream>
#include <streambuf>
#include <string>

#include "qtextedit.h"

  
class QDebugStream : public std::basic_streambuf<char>
{
public:
	 QDebugStream(std::ostream &stream, QTextEdit* text_edit) : m_stream(stream)
	 {
	  log_window = text_edit;
	  m_old_buf = stream.rdbuf();
	  stream.rdbuf(this);
	  to_terminal = false;
	  to_GUI = false;
	 }

	 ~QDebugStream()
	 {
	  // output anything that is left
		 if (!m_string.empty()) {
			 if (to_GUI) { 
				 log_window->append(m_string.c_str()); 
			 }
			 if (to_terminal) { 
				 printf("%s", m_string.c_str()); printf("\n");
			 }
		 }
	  m_stream.rdbuf(m_old_buf);
	 }

	void copyOutToTerminal(bool _to_terminal) { to_terminal = _to_terminal; }
	void redirectOutInGUI(bool _to_GUI) { to_GUI = _to_GUI; }

protected:
	virtual int_type overflow(int_type v)
	{
	  if (v == '\n')
	  {
	   if (to_GUI) { 
		   log_window->append(m_string.c_str());
	   }
	   if (to_terminal) { 
		   printf("%s", m_string.c_str()); printf("\n"); 
	   }
	   m_string.erase(m_string.begin(), m_string.end());
	  }
	  else
	   m_string += v;

	  return v;
	}

	virtual std::streamsize xsputn(const char *p, std::streamsize n)
	{
	  m_string.append(p, p + n);
  
	  int pos = 0;
	  while (pos != std::string::npos)
	  {
	   pos = int (m_string.find('\n'));
	   if (pos != std::string::npos)
	   {
		std::string tmp(m_string.begin(), m_string.begin() + pos);
		if (to_GUI) { log_window->append(tmp.c_str()); }
		if (to_terminal) { printf("%s", tmp.c_str()); printf("\n"); }

		if (m_string.size()>1) m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
	   }
	  }

	  return n;
	 }

private:
	std::ostream &m_stream;
	std::streambuf *m_old_buf;
	std::string m_string;
	bool to_terminal;  //<! if true the output will also go to the terminal
	bool to_GUI;  //<! if true the output will also go to the terminal

	QTextEdit* log_window;
};

#endif