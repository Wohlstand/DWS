#pragma once
#include <Windows.h>
#include <memory>
#include "DWS_Kernel.h"

#pragma comment(lib, "Advapi32.lib")
namespace NULLPTR {
	namespace DWS {
		namespace Utility {
			namespace Registry {
				struct RegInformation {
					enum TYPE
					{
						UNDEFINED = -2,//когда произошла ошибка инициализации
						DIR = -1, //специальный случай. У нас открыта директория
								  //Все возможные типы в порядке "возрастания"
								  NONE = REG_NONE,
								  SZ = REG_SZ,
								  EXPAND_SZ = REG_EXPAND_SZ,
								  BINARY = REG_BINARY,
								  DWORD = REG_DWORD,
								  LINK = REG_LINK,
								  MULTI_SZ = REG_MULTI_SZ,
								  RESOURCE_LIST = REG_RESOURCE_LIST,
								  FULL_RESOURCE_DESCRIPTOR = REG_FULL_RESOURCE_DESCRIPTOR,
								  RESOURCE_REQUIREMENTS_LIST = REG_RESOURCE_REQUIREMENTS_LIST,
								  QWORD = REG_QWORD
					};

					enum BRANCH
					{
						HKLM = (::DWORD) HKEY_LOCAL_MACHINE,
						HKU = (::DWORD) HKEY_USERS,
						HKCU = (::DWORD) HKEY_CURRENT_USER
					};

					struct KeyValue
					{
						String value;
						TYPE type;
					};

					BRANCH	cBranch = HKLM;//текущая ветвь реестра
					String	address = TEXT("");//полный адрес до текущего ключа
					HKEY	nativeHandle = 0; //собстенно хендл
					TYPE	type = UNDEFINED;
					String	fullAddress = TEXT("");
					::DWORD	cb = 0;
				};


				class AbstractRegistryItem
				{
				protected:
					RegInformation info;
				public:
					AbstractRegistryItem();
					AbstractRegistryItem(RegInformation::BRANCH _br, String _path, HKEY _v);

					~AbstractRegistryItem();

					RegInformation::TYPE getType() const;

					virtual bool isValid() const = 0;

					//			virtual void create(RegInformation::TYPE, String _name=TEXT("")) = 0;
					virtual void remove() = 0;
					virtual void setValue(String _v) = 0;
				};

				//todo разнести классы по разным файлам
				struct Directory;

				struct Key :public AbstractRegistryItem
				{
					friend class Directory;
					typedef unsigned __int64 QWORD;//WTF? 
					Key();
					Key(RegInformation::BRANCH _br, String _path, HKEY _v);

					virtual bool isValid() const override;

					void setType(RegInformation::TYPE _t);

					String getValue();

					//			virtual void create(RegInformation::TYPE, String _name = TEXT("")) override;
					virtual void remove() override;
					virtual void setValue(String _v) override;
				};

				struct Directory :public AbstractRegistryItem
				{
					friend struct Key;
					Directory();
					Directory(Key&);
					Directory(RegInformation::BRANCH _br, String _path, HKEY _v);

					Key getKey(String keyName);

					virtual bool isValid() const override;

					Key createKey(String _name = TEXT(""), RegInformation::TYPE _t = RegInformation::NONE);
					Directory createDir(String _name = TEXT(""));
					virtual void remove() override;
					virtual void setValue(String _v) override;
				};

			}
			using namespace Kernel;

			typedef Registry::Key					VReg;//пусть безобразно - зато единообразно
			typedef Registry::Directory				DReg;
			typedef Registry::AbstractRegistryItem	AReg;

			/*
			Класс будет следить за локальным или удаленным реестром.
			*/
			class RegistryManager
			{
			public:
				void init();

				DReg getDir(Registry::RegInformation::BRANCH from, String _path);//открыть директорию по ПОЛНОМУ пути
				VReg getKey(Registry::RegInformation::BRANCH from, String _path);//открыть ключ по ПОЛНОМУ пути

				static String machinePath;//игнорь меня полностью.
			private:

			};


		}
	}
}