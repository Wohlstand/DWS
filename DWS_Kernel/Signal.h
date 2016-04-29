#pragma once
#include <functional>
#include <tuple>
//тут секс который не будет понятен сразу. Ты серъезно хочешь сюда посмотреть?
namespace DWSUtility
{
	//дело твое конечно. Эта структура не используется.
	template<typename T>
	struct function_traits;
	//и частичная специализация шаблона тоже не используется ( спс stackoverflow за код )
	template<typename R, typename ...Args>
	struct function_traits<std::function<R(Args...)>>
	{
		static const size_t nargs = sizeof...(Args);

		typedef R result_type;

		template <size_t i>
		struct arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
		};


	};
	//вот это мы и будем использовать
	template < typename T >
	struct Signal;
	//частичная специализация шаблона для std::function. Других специализаций не будет!!!
	template < typename ... Args >
	struct Signal<std::function<void(Args...) > >
	{
		//определяем типы, которые будем использовать.
		typedef void RET;
		typedef  std::function<RET(Args...)> func_type;
		size_t operator += (func_type _slot)
		{
			m_slots.push_back(_slot);
			return m_slots.size();
		}
		//список всех return
		void operator () (Args... _a)
		{
			//так как у нас функция вовращает void - мы не можем сделать список "возвратов". Так что просто запускаем колбеки и валим отсюда.
			for (func_type _f : m_slots)
			{
				try {
					_f(_a...);
				}
				catch (...)
				{
					//У нас произошла ошибка при обработе слота!!! 
#pragma message("Nada napisat' obrabotky oshibok")
				}
			}
		}
	private:
		std::list< func_type > m_slots;//список подсоединенных функций
	};

	//а вот тут начинается жесткое порево. У нас теперь есть возвращаемое значение! 
	template <typename R, typename ... Args >
	struct Signal<std::function<R(Args...) > >
	{
		typedef R RET;
		typedef  std::function<RET(Args...)> func_type;
		unsigned int operator += (func_type _slot)
		{
			m_slots.push_back(_slot);
			return m_slots.size();
		}
		//список всех return
		std::list<RET> operator () (Args... _a)
		{
			//и мы тупо собираем в список эти значения и валим отсюда.
			std::list<RET> result;
			for (func_type _f : m_slots)
			{
				try {
					result.push_back(_f(_a...));
				}
				catch (...)
				{
					//У нас произошла ошибка при обработе слота!!! 
#pragma message("Nada napisat' obrabotky oshibok")
				}
			}
			return result;
		}
	private:
		std::list< func_type > m_slots;
	};

}