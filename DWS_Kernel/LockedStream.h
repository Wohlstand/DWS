#pragma once
#include "Signal.h"
namespace DWSKernel {
	//делаем потоко безопасный вывод
	template< class STREAM >
	class LockedStream
	{
	private:
		std::mutex m_guard;
		STREAM & m_stream;
	public:
		LockedStream(STREAM& _stream) :m_stream(_stream)
		{}
		template < typename T >
		LockedStream& operator << (T cl)
		{
			std::lock_guard<std::mutex> g(m_guard);
			m_stream << cl << std::flush;
			outputGrabber(to_string(cl));
			return *this;
		}
		template < typename T >
		LockedStream& operator << (T cl) const
		{
			std::lock_guard<std::mutex> g(m_guard);
			m_stream << cl << std::flush;
			outputGrabber(to_string(cl));
			return *this;
		}
		template < typename T >
		LockedStream& operator >> (T& cl)
		{
			std::lock_guard<std::mutex> g(m_guard);
			m_stream >> cl;
			return *this;
		}
		template < typename T >
		LockedStream& operator >> (T& cl) const
		{
			std::lock_guard<std::mutex> g(m_guard);
			m_stream >> cl;
			return *this;
		}
		STREAM& getStream()
		{
			return m_stream;
		}
		std::lock_guard<std::mutex> guard()
		{
			return m_guard;
		}
		DWSUtility::Signal<std::function<void(String)> > outputGrabber;
	};
}