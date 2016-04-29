#pragma once
//думаю объяснять не нужно? Если нужно - го в педовики.
template <typename T >
class Singleton
{
private:
	Singleton() {};
	Singleton(T) {};
	Singleton(Singleton&) {}
	Singleton(Singleton&&) {}
public:
	static T& instance()
	{
		static T _local;
		return _local;
	}
};