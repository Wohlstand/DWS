#pragma once
#include <Windows.h>
#include <memory>
#include "DWS_Kernel.h"

#pragma comment(lib, "Advapi32.lib")
namespace DWSUtility
{
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

			BRANCH	cBranch		= HKLM;//текущая ветвь реестра
			String	address		= TEXT("");//полный адрес до текущего ключа
			HKEY	nativeHandle= 0; //собстенно хендл
			TYPE	type		= UNDEFINED;
			String	fullAddress = TEXT("");
			::DWORD	cb			= 0;
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

		struct Directory:public AbstractRegistryItem
		{
			friend struct Key;
			Directory();
			Directory(Key&);
			Directory(RegInformation::BRANCH _br, String _path, HKEY _v);
			
			Key getKey(String keyName );

			virtual bool isValid() const override;

			Key createKey(String _name = TEXT(""), RegInformation::TYPE _t = RegInformation::NONE);
			Directory createDir(String _name = TEXT(""));
			virtual void remove() override;
			virtual void setValue(String _v) override;
		};

	}
	using namespace DWSKernel;

	typedef Registry::Key					VReg;//пусть безобразно - зато единообразно
	typedef Registry::Directory				DReg;
	typedef Registry::AbstractRegistryItem	AReg;


//	/*
//	Типичное значение реестра в вакууме умеет лишь пару вещей:
//	Создаваться (func create )
//	Удаляться (func remove )
//	Получать текущий тип( func getType )
//
//	Остальное - удел наследников.
//	*/
//	class ARegistryValue
//	{
//		friend class RegistryManager;
//	public:
//		//тип ключа
//		enum TYPE
//		{
//			UNDEFINED = -2,//когда произошла ошибка инициализации
//			DIR = -1, //специальный случай. У нас открыта директория
//					  //Все возможные типы в порядке "возрастания"
//			NONE = REG_NONE,
//			SZ = REG_SZ,
//			EXPAND_SZ = REG_EXPAND_SZ,
//			BINARY = REG_BINARY,
//			DWORD = REG_DWORD,
//			LINK = REG_LINK,
//			MULTI_SZ = REG_MULTI_SZ,
//			RESOURCE_LIST = REG_RESOURCE_LIST,
//			FULL_RESOURCE_DESCRIPTOR = REG_FULL_RESOURCE_DESCRIPTOR,
//			RESOURCE_REQUIREMENTS_LIST = REG_RESOURCE_REQUIREMENTS_LIST,
//			QWORD = REG_QWORD
//		};
//
//		enum BRANCH
//		{
//			HKLM = (::DWORD) HKEY_LOCAL_MACHINE,
//			HKU = (::DWORD) HKEY_USERS,
//			HKCU = (::DWORD) HKEY_CURRENT_USER
//		};
//
//
//	protected:
//#pragma message("Check casts on x64!") //Nummer: C-Style cast's working :D . X64 cast - reinterpret_cast<TYPE> 
//		BRANCH m_cBranch;//текущая ветвь реестра
//
//		String m_address;//полный адрес до текущего ключа
//		HKEY  m_nativeHandle;//собстенно хендл
//
//		static bool createFullPath;//флаг о том чтобы руками создавать весь путь - если его нет.
//
//		ARegistryValue(BRANCH, String, HKEY);
//	public:
//		~ARegistryValue();
//#pragma message("write?")
//		//void setType(TYPE _t);//пересоздаст ключ в нужном формате. Если возможно - сохранит то что было внутри.
//		virtual TYPE getType() const; //возвращает тип текущего.
//		virtual AReg create(TYPE _t = DIR);//создает ТЕКУЩИЙ элемент. Только если раньше был UNDEFINED
//		virtual void remove(); //удаление собственно ключа...
//		VReg toVReg();
//		DReg toDReg();
//	};
//
//	//Работа с "директорией"
//	class DIRRegistry : public ARegistryValue
//	{
//	public:
//		DIRRegistry(BRANCH, String, HKEY);
//		DReg create(String _name, TYPE _t = DIR);//создание элемента внутри!
//
//		VReg getKey(String _name); //получить доступ к элементу внутри
//		DReg getDir(String _name); //внутри ещё и папка? открыть её полностью!
//
//		/*
//		А надо ли подниматься "вверх" по директориям? Такого функционала пока реализовано НЕ будет!
//		*/
//
//		
//		void setDefault(String _value);//задание default значения
//
//		//impl для наших виртуальных функций
//		virtual void remove() override;
//		virtual AReg create(TYPE _t = DIR) override;//создает ТЕКУЩИЙ элемент. Только если раньше был UNDEFINED
//	};
//
//	//Работа с ключами
//	class ValueRegistry : public ARegistryValue
//	{
//	public:
//		ValueRegistry(BRANCH, String, HKEY);
//		//перегружай сколько хочешь для нужного типа
//		VReg setValue(::DWORD _value);
//		VReg setValue(String _value);
//		
//		//а вот по возвращаемому значению перегрузить нельзя - поэтому вот так
//		/*String getStringValue() const;
//		::DWORD getDwordValue() const;*/
//		String getValue() const; // получаем всегда строку!!!!
//
//		//impl для наших виртуальных функций
//		virtual void remove() override;
//		virtual AReg create(TYPE _t = DIR) override;//создает ТЕКУЩИЙ элемент. Только если раньше был UNDEFINED
//	};
//
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