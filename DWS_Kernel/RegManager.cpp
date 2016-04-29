#include "RegManager.h"
#include "DWS_Utility.h"
namespace DWSUtility
{
	String RegistryManager::machinePath = TEXT(".\\");
	void RegistryManager::init()
	{
		//sergius-dart:Теоретически здесь будет подключение к удаленному реестру, но пока здесь место для вашей рекламы.
		//nummer: Продам гараж.
	}

	DReg RegistryManager::getDir(Registry::RegInformation::BRANCH from, String _path)
	{
		HKEY _result = 0;
		if (ERROR_SUCCESS != RegOpenKeyEx((HKEY)from, _path.c_str(), NULL, KEY_ALL_ACCESS, &_result))
		{
			UC_ERROR << "Not open key :: " << _path << "\r\n";
		}
		return DReg(from, _path, _result);
	}

	VReg RegistryManager::getKey(Registry::RegInformation::BRANCH from, String _path)
	{
		HKEY _result = 0;
		LONG openResult = RegOpenKeyEx((HKEY)from, _path.c_str(), NULL, KEY_READ | KEY_SET_VALUE, &_result);
		if (ERROR_SUCCESS != openResult)
		{
			UC_ERROR << "Not open key :: " << _path << " err = " << openResult << "\r\n";
		}
		return VReg(from, _path, _result);
	}
	namespace Registry 
	{
		//default constructer. Its bad...
		AbstractRegistryItem::AbstractRegistryItem()
		{

		}

		AbstractRegistryItem::AbstractRegistryItem(RegInformation::BRANCH _br, String _path, HKEY _v)
		{
			info.cBranch = _br;
			info.address = info.fullAddress = _path;
			info.nativeHandle = _v; // если он прилетел нормальный, значит с помощью OpenKey открыли. И значит это папка. 
			//получаем тип
			if (!info.nativeHandle) //если не получили ничего, значит у нас значение и нужно сначало получить hande папки, а потом уже  работать
			{
				size_t lastIndex = _path.find_last_of('\\');
				String _dirPath = _path.substr(0, lastIndex);
				LONG openResult = RegOpenKeyEx((HKEY)_br, _dirPath.c_str(), NULL, KEY_READ | KEY_SET_VALUE, &info.nativeHandle);
				if (ERROR_SUCCESS != openResult)
				{
					UC_ERROR << "Not open key :: " << _path << " err = " << openResult << "\r\n";
				}else{
					//значит мы открыли, и теперь правим адрес 
					info.address = _path.substr(lastIndex+1);
				}
			}
			DWORD type = 0;
			LONG resultQuery = RegQueryValueEx(
				(HKEY)info.nativeHandle,
				const_cast<typename String::traits_type::char_type*> (info.address.c_str()),
				NULL,
				&type,
				NULL,
				&info.cb
				);
			if (resultQuery != ERROR_SUCCESS )
			{
				if ( info.nativeHandle )
					info.type = RegInformation::DIR;
				else 
					UC_ERROR << "RegQueryValueEx fail = " << resultQuery; //по дефолту значение UNDEFINED - поэтому ничего не ставим
			} else
				info.type = (RegInformation::TYPE) type;//кастуем
		}

		AbstractRegistryItem::~AbstractRegistryItem()
		{
			//Не должно быть такого, но мало ли...
			if (info.nativeHandle != 0 && info.type == RegInformation::DIR )
			{
				LSTATUS result = RegCloseKey(info.nativeHandle);
				if (ERROR_SUCCESS != result)
					U_WARNING << "Error on close Key. Error = " << result << "\r\n";
			}
		}

		RegInformation::TYPE AbstractRegistryItem::getType() const
		{
			return info.type;
		}

		Key::Key()
		{
		}

		Key::Key(RegInformation::BRANCH _br, String _path, HKEY _v)
			:AbstractRegistryItem(_br, _path, _v)
		{
		}

		bool Key::isValid()
			const
		{
			return info.nativeHandle && info.type > RegInformation::DIR;
		}

		void Key::setType(RegInformation::TYPE _t)
		{
			if (info.type != _t && _t > RegInformation::DIR)
			{
				info.type = _t;
				setValue(TEXT(""));//устанавливаем тип записи и заполняем пустым значением. Если не задали тип - значит папка!
			}
		}

		String Key::getValue()
		{
			String result;
			result.reserve(info.cb / sizeof( typename String::traits_type::char_type ) );
			DWORD type = 0;
			LONG resultQuery = RegQueryValueEx(
				(HKEY)info.nativeHandle,
				const_cast<typename String::traits_type::char_type*> (info.address.c_str()),
				NULL,
				&type,
				(LPBYTE)const_cast<typename String::traits_type::char_type* > (result.c_str() ),//я медленно снимаю с тебя const
				&info.cb
				);
			if (ERROR_SUCCESS != resultQuery)
			{
				UC_ERROR << "Error on get value : " << info.fullAddress << " = " << resultQuery << "\r\n";
			}
			return result;
		}

		void Key::remove()
		{
			//\TODO не удаляет, проверить
			LONG result = RegDeleteKey(info.nativeHandle, info.address.c_str());
			if (ERROR_SUCCESS != result)
			{
				UC_ERROR << "Error on remove registry item : " << info.fullAddress << " error = " << result << "\r\n";
			}
		}

		void Key::setValue(String _v)
		{
			const BYTE* ptr = nullptr;//буфер, который скармливаем 
			DWORD size = 0;//размер получиывшихся данных
			if (_v.length())
			{
				switch (info.type)//\TODO TOO MORE CASE!!!
				{
				case RegInformation::SZ:
				case RegInformation::MULTI_SZ:
				case RegInformation::EXPAND_SZ:
					size = _v.length() * sizeof(String::traits_type::char_type);
					ptr = (const BYTE*) new String::traits_type::char_type[_v.length()];
					std::strncpy((char*)ptr, reinterpret_cast<const char*>(_v.c_str()), size);
					break;
				case RegInformation::DWORD:
					ptr = (const BYTE*)new DWORD(std::stoi(_v));
					size = sizeof(DWORD);
					break;
				case RegInformation::QWORD:
					ptr = (const BYTE*)new QWORD(std::stoi(_v));
					size = sizeof(QWORD);
					break;
				default:
					UC_ERROR << "Not implemented current type set value : " << info.type << "\r\n";
					throw TEXT("No implement current TYPE!");
					return;
				}
			}
			LONG result = RegSetValueEx(
				info.nativeHandle,
				info.address.c_str() ,
				NULL,
				info.type,
				ptr,
				size
				);
			if (ERROR_SUCCESS != result)
			{
				UC_ERROR << "Error on setValue = " << result << "\r\n";
			}
			delete [] ptr;//нагадили, надо убрать за собой
		}

		//default construct. delete me.
		Directory::Directory()
		{
		}

		Directory::Directory(Key& _k)
		{
			_k.setType(RegInformation::NONE);
			info = _k.info;//копирование информации и установка типа в REG_NONE
		}

		Directory::Directory(RegInformation::BRANCH _br, String _path, HKEY _v)
			:AbstractRegistryItem(_br, _path, _v)
		{
		}

		Key Directory::getKey(String keyName)
		{
			return Key(info.cBranch, info.fullAddress + TEXT("\\") + keyName,0);
		}

		bool Directory::isValid()
			const
		{
			return info.nativeHandle;
		}

		Key Directory::createKey( String _name, RegInformation::TYPE _t )
		{
			if (!isValid())
			{
				UC_ERROR << "don't open dir " << info.fullAddress << " -> not create inherit items...\r\n";
				throw "isValid";
			}
			//HKEY created = 0;
			//DWORD resultOpen = 0;
			//LONG resultCreate = RegCreateKeyEx(
			//	info.nativeHandle,
			//	_name.c_str(),
			//	NULL,
			//	NULL,
			//	NULL,
			//	KEY_ALL_ACCESS,
			//	NULL,
			//	&created,
			//	&resultOpen
			//	);
			//if (ERROR_SUCCESS != resultCreate)
			//{
			//	UC_ERROR << "Error on create key " << info.fullAddress << _name << "\r\n";
			//	return Key();
			//}
			//CloseHandle((HANDLE)created);//создали? Теперь откроем через нормальный путь
			Key result(info.cBranch, _name, info.nativeHandle);
			result.setType(_t);
			return result;
		}

		Directory Directory::createDir(String _name)
		{
			if (!isValid())
			{
				UC_ERROR << "don't open dir " << info.fullAddress << " -> not create inherit items...\r\n";
				throw "isValid";
			}
			HKEY created = 0;
			DWORD resultOpen = 0;
			LONG resultCreate = RegCreateKeyEx(
				info.nativeHandle,
				_name.c_str(),
				NULL,
				NULL,
				NULL,
				KEY_ALL_ACCESS,
				NULL,
				&created,
				&resultOpen
				);
			if (ERROR_SUCCESS != resultCreate)
			{
				UC_ERROR << "Error on create key " << info.fullAddress  << "\\"<< _name  << " error code = " <<  resultCreate<< "\r\n";
				return Directory();
			}
			return Directory(info.cBranch, info.fullAddress + TEXT("\\") + _name, created);
		}

		void Directory::remove()
		{
			LONG result = RegDeleteTree((HKEY)info.cBranch, info.address.c_str());
			if (ERROR_SUCCESS != result)
			{
				UC_ERROR << "Error on remove registry item : " << info.fullAddress << " error = " << result << "\r\n";
			}
		}

		void Directory::setValue(String _v)
		{

		}

	}
}
