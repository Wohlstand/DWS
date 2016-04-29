#include "HostsEditor.h"
#include "DWS_Utility.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>

#define HOSTS_PATH_WIN \
		STR SysDir[50]; \
		GetSystemDirectory(SysDir, 50); \
		String _hosts_location = SysDir + String(TEXT("\\drivers\\etc\\hosts")); 

namespace DWSUtility
{

	template < typename T >
	typename T::const_iterator skipSpace(typename T::const_iterator _iter, T& _src)
	{
		do
		{
			if (!isspace(*_iter))//как встретили что-то отличное от пробелом - выходим!
				break;
		} while (++_iter != _src.end());//kill me? may be....
		return _iter;
	}

	template < typename T >
	typename T::const_iterator doSpace(typename T::const_iterator _iter, T& _src)
	{
		do
		{
			if (isspace(*_iter))//как встретили пробел - выходим!
				break;
		} while (++_iter != _src.end());//kill me? may be....
		return _iter;
	}

	HostsEditor::HostsEditor()
	{

	}


	HostsEditor::~HostsEditor()
	{
	}

	void HostsEditor::read()
	{
		//\TODO наверное переделать, или забить.
		//\TODO СДЕЛАТЬ ЗАПИСЫВАЕМЫМ!!!
		HOSTS_PATH_WIN
		m_file.open(_hosts_location.c_str(), std::fstream::in);
#pragma message("Open file!!!")
		parse();
		for (HRef _i : m_all)
		{
			wcout() << _i->getLine() << " : "<< _i->dump();
		}
	}

	void HostsEditor::write()
	{
		size_t currentLine = 0;
		std::wstringstream output;

		for (HRef _i : m_all)
		{
			//Мы здесь считаем что руками ничего сильно не трогали и позиции у нас не нарушены. Поэтому просто пишем!
			
			//Вначале надо заполнить все пустые строки!
			for ( 
					;
					currentLine < _i->getLine(); 
					++currentLine
				)
			{
				output << "\r\n";
			}

			//вызываем write!
			_i->write(output);
		}
		//последний \r\n
		output << "\r\n";

		wcout() << "------------------\r\n";
		wcout() << output.str();
		wcout() << "------------------\r\n";
		m_file.close();
		HOSTS_PATH_WIN
		m_file.open(_hosts_location.c_str(), std::fstream::out);
		m_file.write(output.str().c_str(), output.str().length());
	}

	HRef HostsEditor::add(String _host, String _ip, String _comment,HRef _prepend)
	{
		auto insertIter = m_all.end();
		size_t line = m_all.back()->getLine() + 1;;
		if (_prepend)
		{
			for (auto _i = m_all.begin(); _i != m_all.end(); ++_i)
				if (_prepend == *_i)
				{
					insertIter = _i;//save iterator on find
					line = _i->get()->getLine() + 1;
					break;
				}
			if (insertIter == m_all.end())
				U_WARNING << TEXT(" May be not found inserted!\r\n");
		}

		auto _comIter = m_all.insert(
			insertIter, 
			HRef(
				new HostsRecord(_host, _ip, line)
				)
			);
		if (_comment.length() != 0)
		{
			m_all.insert(
				++_comIter,//вставляем следующим
				HRef(
					new HostsComment(TEXT("#") + _comment, line)
					)
				);
			_comIter--;//исправляем итератор, чтобы он указывал на начало записи!
		}
		return *_comIter;
	}

	HRef HostsEditor::findHost(String _host) const
	{
		for (HRef _i : m_all)
		{
			HostsRecord* _p = dynamic_cast<HostsRecord*>(_i.get());
			if (_p)
			{
				if (_p->checkHost(_host))
					return _i;
			}
		}
		return HRef(0);
	}

	void HostsEditor::parse()
	{
		if (!m_file.is_open())
		{
			U_WARNING << TEXT("Hosts file not open!\r\n");
			throw std::runtime_error("FileNotOpen");
		}
		size_t currentLine = 0;
		String preparsedString;//не бейте меня сильно копытами. Но буду читать ASCII строки!
		bool isComment = false;

		while (!m_file.eof())
		{
			std::getline(m_file,preparsedString);//получаю строку

			String record;
			String comment;

			size_t recordEnd = preparsedString.find('#');
			if (std::wstring::npos != recordEnd)
			{
				record = preparsedString.substr(0, recordEnd);
			}
			comment = preparsedString.substr(recordEnd == std::wstring::npos ? 0 : recordEnd);

			if (record.length() != 0)
				m_all.push_back(
					HRef(
						new HostsRecord(record, currentLine)
						)
					);
			//если коммент есть - то добавляем его
			if (comment.length() != 0)
				m_all.push_back(
					HRef(
						new HostsComment(comment, currentLine)
						)
					);
			++currentLine;
		}
	}

	HostsComment::HostsComment(String _str, int _line):
		HostsElement(_line),
		m_comment( _str )
	{
	}

	String HostsComment::getComment() const
	{
		return m_comment;
	}

	void HostsComment::write( std::wostream &_stream) const
	{
		_stream << m_comment ;
	}

	size_t HostsElement::getLine() const
	{
		return m_line;
	}

	HostsElement::HostsElement(size_t line)
		:m_line(line)
	{
	}

	HostsRecord::HostsRecord(String _str, size_t _line):
		HostsElement(_line)
	{
		auto startIp = skipSpace<String>(_str.begin(), _str);
		auto endIp = doSpace<String>(startIp, _str);
		m_ip = String(startIp, endIp);

		auto startAddress = skipSpace<String>(endIp, _str);
		auto endAddress = doSpace<String>(startAddress, _str);
		m_host = String(startAddress, endAddress);
	}

	HostsRecord::HostsRecord(String _host, String _ip, size_t line):
		HostsElement(line),
		m_host(_host),
		m_ip(_ip)
	{
	}

	bool HostsRecord::checkHost(String _host)
	{
		return m_host == _host;
	}

	String HostsRecord::getIp() const
	{
		return m_ip;
	}

	String HostsRecord::getHost() const
	{
		return m_host;
	}

	void HostsRecord::write( std::wostream & _stream) const
	{
		//\TODO доделать форматирование IP
		_stream << m_ip <<  " " << m_host << " ";
	}
	bool HostsRecord::isIpv4() const
	{
		//sergius-dart:IMPLEMENT ME
		return false;
	}
}