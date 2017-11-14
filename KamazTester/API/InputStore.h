#ifndef  __INPUTSTORE_H__
#define  __INPUTSTORE_H__

#include <vector>
#include <exception>

/*
	Позволяет хранить несколько значений входных параметров,
	выдавая по каждому запросу очередное значение
*/
template <class T>
class InputStore
{
public:

	//Очищает все
	void Reset()
	{
		_currentStage = 0;
		_inputs.clear();
	}

	// Возвращает очередное значение
	T& GetNext()
	{
		if (_currentStage >= _inputs.size())
			throw std::exception("Too many gets");

		return _inputs[_currentStage++];
	}

	// Добавляет новое значение
	void AddInput(T input)
	{
		_inputs.push_back(input);
	}

private:
	std::vector<T> _inputs;
	int _currentStage;
};

#endif 

