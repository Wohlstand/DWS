#pragma once
#include "DWS_Kernel.h"
#include <fstream>

namespace NULLPTR {
	namespace DWS {
		namespace Utility {
			class HostsElement;
			typedef std::shared_ptr<HostsElement> HRef;
			/*
			Специальная обертка для удобного редактирования hosts. Написать сохранение всех комментариев.
			*/
			class HostsEditor
			{
				std::wfstream m_file;
				String m_thisSignature;//наша подпись (опционально)
				std::list<HRef> m_all;//парсим сюда
			public:
				HostsEditor();
				virtual ~HostsEditor();

				void read();
				void write();

				/*
				Добавляет элемент в список.
				_host - кого
				_ip - куда
				_prepend - перед какого вставить ( 0 последний )
				*/
				HRef add(String _host, String _ip = TEXT("0.0.0.0"), String _comment = TEXT(""), HRef _prepend = 0);
				/*
				Ищем хост. Если находим - возвращаем его "элемент"( без комментария )
				При ошибке возвращается невалидный PTR.
				*/
				HRef findHost(String _host) const;


			protected:
				//use bison/flex ? Ну я подумаю, но пока КОСТЫЛИ И ВЕЛОСИПЕДЫ! 
				void parse();
			};

			class HostsElement
			{
				friend class HostsEditor;
			public:
				size_t getLine() const;
			protected:
				//позиция в файле
				size_t m_line;
				HostsElement(size_t line);

				virtual void write(std::wostream &_stream) const = 0; //пишем в поток текущий элемент ) С соблюдением всех отступов. 
			public:
				virtual String dump() const = 0;
			};

			class HostsComment : public HostsElement
			{
				String m_comment;
			public:
				HostsComment(String _str, int _line);

				String getComment() const;

			protected:
				virtual void write(std::wostream & _stream) const override;
			public:
				virtual String dump() const override
				{
					return TEXT("HostsComment ") + m_comment + TEXT("\r\n");
				}
			};

			class HostsRecord : public HostsElement
			{
				String m_ip;
				String m_host;
			public:
				HostsRecord(String _str, size_t _line);
				HostsRecord(String _host, String _ip, size_t line = 0);

				bool checkHost(String _host);
				String getIp() const;
				String getHost() const;
			protected:
				virtual void write(std::wostream & _stream) const override;
			public:
				virtual String dump() const override
				{
					return TEXT("HostsRecord ") + m_ip + TEXT(" = ") + m_host + TEXT("\r\n");
				}
				bool isIpv4() const;
			};

		}
	}
}