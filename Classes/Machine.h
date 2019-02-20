#pragma once

#include <array>
#include <unordered_map>
#include <tuple>
#include <string>
#include <optional>
#include <list>

#include <nlohmann/json.hpp>

/*
	Класс игрового аппарата.
	Хранит в себе матрицу результатов, а так же умеет подсчитывать успешные комбинации.
	Используется как на сервере, так и на клиенте.
	
	По факту имеем набор примитивных методов.
	Имеет сереиализацию и десереализацию в Json и обратно.

	Так же хранит в себе карту комбинаций, множителей и прочее.
*/
class Machine
{
public:
	//Тип слота
	enum class Slot : int
	{
		SLOT_1 = 0,
		SLOT_2,
		SLOT_3,
		SLOT_4,
		SLOT_5,
		SLOT_6,
		SLOT_7,
		SLOT_8,
		SLOT_9,
		SLOT_10,
		
		COUNT,
		UNKNOW = COUNT
	};

	//множители
	enum class Multi : int
	{
		None = 0,
		X3 = 3,
		X4 = 4,
		X5 = 5
	};

	//линии (Можно было линиям задать банально тип инт или отдельную структуру)
	enum class Line : int
	{
		LINE_1 = 0,
		LINE_2,
		LINE_3,
		LINE_4,
		LINE_5,
		LINE_6,
		LINE_7,
		LINE_8,
		LINE_9,
		LINE_10,
		LINE_11,
		LINE_12,
		LINE_13,
		LINE_14,
		LINE_15,
		LINE_16,
		LINE_17,
		LINE_18,
		LINE_19,
		LINE_20,

		COUNT,
		UNKNOW = COUNT
	};

	enum class Status
	{
		Spin,
		Stop
	};

	static constexpr int COLS = 5;
	static constexpr int ROWS = 3;

	/*
		Таблица выплат.

		Можно было бы не усложнять этот участок кода. 
		Т.к. в Т.З. все слоты возвращают одинаковое количество очков
		но вдруг мы заходим каждому типу слота задать свое количество очков?

		BetMap - таблица выплат по отношению к Multi
		SlotMap - таблица выплат конкретного слота
	*/
	using BetMap = std::unordered_map<Multi, int>;
	using SlotMap = std::unordered_map<Slot, BetMap>;

	using LineResult = std::tuple<Line, Slot, Multi>; //тут можно было в принципе и структуру сделать
	using ListLinesResult = std::list<LineResult>; //лис результатов

	using LinePoints = std::array<int, COLS>; //последовательность позиций линии с лева на право
	using LinePointsMap = std::unordered_map<Line, LinePoints>; //карта всех возможных позиций

	using SlotMylti = std::tuple<Slot, Multi>; //просто пара слота и множителя

	using Matrix = std::array<Slot, COLS * ROWS>; //Матрица результатов (результаты я храню в одномерном массиве.)

	using ResultBet = std::tuple<Line, int>;
	using ListResultBet = std::list<ResultBet>;

	inline Machine() :
		totalBet(0),
		winner(0),
		status(Status::Stop)
	{};

	inline Machine(const Matrix &matrix_, int totalBet_, int winner_, Status status_) :
		matrix(matrix_),
		totalBet(totalBet_),
		winner(winner_),
		status(status_)
	{}

	inline Machine(const Machine &copy) :
		matrix(copy.matrix),
		winner(copy.winner),
		totalBet(copy.totalBet),
		status(copy.status)
	{}
	//move конструктор тут в принципе и не нужен. Все равно копирование будет происходить.
	inline Machine(Machine &&move) :
		matrix(std::move(move.matrix)),
		winner(move.winner),
		totalBet(move.totalBet),
		status(move.status)
	{
		move.winner = 0;
		move.totalBet = 0;
		move.status = Status::Stop;
	}

	//получить слот из матрицы
	inline Slot getSlot(int colum, int row)
	{
		return matrix.at(row * COLS + colum);
	}
	//задать слот из матрицы
	inline void setSlot(int colum, int row, Slot slot)
	{
		matrix.at(row * COLS + colum) = slot;
	}

	//возвращает таблицу ставок конкретного типа слота
	static BetMap getBet(Slot slot)
	{
		return slotMap.at(slot);
	}
	//возвращает ставку конкретного типа слота и множителя
	static int getBet(Slot slot, Multi multi)
	{
		return slotMap.at(slot).at(multi);
	}
	
	//получить список успешных комбинаций
	ListLinesResult getResult();

	//подсчитать и получить результат. Вычисляет Winner, обнуляет totalBet
	int resultWinner();

	//лист выплат за конкретную линюю
	static ListResultBet getListResultBet(const ListLinesResult &listLineResult, int totalBet_);

	//выплата
	static int getResultBetFromResultList(const ListLinesResult &listLineResult, int totalBet_);

	//проверить, есть ли такая комбинация
	bool chekLine(Line line);
	//проверить и получить комбинацию. Если нету, вернет Multi::None
	SlotMylti getLine(Line line);

	static nlohmann::json machineToJSON(const Machine &machine);
	static Machine machineFromJSON(const nlohmann::json &j);

	//получить позиции той или иной линии
	static LinePoints getLinePoints(Line line)
	{
		return linePointsMap.at(line);
	}
	
	//получить и задать ставку
	inline int getTotalBet() { return totalBet; }
	inline void setTotalBet(int Bet) { totalBet = Bet; }

	//получить и задать выигрыш
	inline int getWinner() { return winner; }
	inline void setWinner(int win) 
	{
		if (win < 0)
			win = 0;
		winner = win; 
	}

	inline Machine& operator=(const Machine &copy)
	{
		totalBet = copy.totalBet;
		matrix = copy.matrix;
		winner = copy.winner;
		status = copy.status;
		return *this;
	}

	//Вращает барабан (по факту генерирует случайное число от 0-9.
	static Matrix getRandomMatrix();
	inline void randomize()
	{
		matrix = getRandomMatrix();
	}

	inline Matrix getMatrix() const 
	{ return matrix; }
	inline void setMatrix(const Matrix &matrix_) 
	{ matrix = matrix_; }

	inline Status getStatus() { return status; }
	inline void setStatus(Status status_) { status = status_; }
private:
	//глобальная карта слотов
	static SlotMap slotMap;
	//глобальная карта комбинаций линий.
	static LinePointsMap linePointsMap;

	Matrix matrix = {};
	int totalBet = 0;
	int winner = 0;
	Status status = Status::Spin;
};
