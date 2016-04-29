#pragma once

#include "DWS_Kernel.h"

namespace DWSKernel
{
	//Наследуйтесь от него! 
	DllExport class BaseFixer
	{
	private:
		static std::list < BaseFixer* > m_all;
		//hide constructors!
		BaseFixer() {}
		BaseFixer(BaseFixer&) {}
		BaseFixer(BaseFixer&&) {}
	protected:
		BaseFixer(BaseFixer* _cl);

	protected:
		//имя класса
		String m_className;
		//применено уже?
		bool m_applyed;
	public:
		//функция для инициализации ресурсов. Вызывается в другом потоке! Проверка фикса!
		virtual void init();
		//функция применяет текущий "фиксер"
		virtual bool exec() = 0;
		//функция для проверки - применено ли?
		virtual bool isApply()
		{
			return m_applyed;
		}
		String getName() const
		{
			return m_className;
		}
		//функции для управление всеми "фиксерами"
	public:
		//получить весь список "фиксеров"
		DllExport static std::list < BaseFixer* >& getAllFixers();
		//получить "фиксер" по имени
		DllExport static BaseFixer* getFixerFromName(String _name);

	};
}

//Создаем класс и подключаем все нужные namespaces. И сразу создаем Instance;
#define CreateFixer(CLName) \
using namespace DWSKernel; \
using namespace DWSUtility; \
class CLName :public DWSKernel::BaseFixer \
{ \
public: \
	CLName() :BaseFixer(this) \
	{ \
		m_className = to_string(typeid(*this).name()); \
	} \
	virtual void init() override; \
	virtual bool exec() override; \
}; \
namespace { \
CLName& _T = Singleton<CLName>::instance(); \
};