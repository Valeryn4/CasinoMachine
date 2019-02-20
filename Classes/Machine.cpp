#include "Machine.h"
#include <nlohmann/json.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>

using Slot = Machine::Slot;
using Line = Machine::Line;
using Multi = Machine::Multi;


/*
	Карта выплат по конкретном слоту за конкретное количество совпадений
*/
Machine::SlotMap Machine::slotMap =
{
	{ Slot::SLOT_1, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_2, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_3, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_4, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_5, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_6, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_7, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_8, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_9, { {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } },
	{ Slot::SLOT_10,{ {Multi::None, 0}, {Multi::X3, 5}, {Multi::X4, 10}, {Multi::X5, 15} } }
};


/*
	Карта комбинаций
*/
Machine::LinePointsMap Machine::linePointsMap = 
{
	{ Line::LINE_1, {1, 1, 1, 1, 1}},
	{ Line::LINE_2, {0, 0, 0, 0, 0}},
	{ Line::LINE_3, {2, 2, 2, 2, 2}},
	{ Line::LINE_4, {0, 1, 2, 1, 0}},
	{ Line::LINE_5, {2, 1, 0, 1, 2}},
	{ Line::LINE_6, {0, 0, 1, 0, 0}},
	{ Line::LINE_7, {2, 2, 1, 2, 2}},
	{ Line::LINE_8, {1, 2, 2, 2, 1}},
	{ Line::LINE_9, {1, 0, 0, 0, 1}},
	{ Line::LINE_10, {1, 0, 1, 0, 1}},
	{ Line::LINE_11, {1, 2, 1, 2, 1}},
	{ Line::LINE_12, {0, 1, 0, 1, 0}},
	{ Line::LINE_13, {2, 1, 2, 1, 2}},
	{ Line::LINE_14, {1, 1, 0, 1, 1}},
	{ Line::LINE_15, {1, 1, 2, 1, 1}},
	{ Line::LINE_16, {0, 1, 1, 1, 0}},
	{ Line::LINE_17, {2, 1, 1, 1, 2}},
	{ Line::LINE_18, {0, 2, 0, 2, 0}},
	{ Line::LINE_19, {2, 0, 2, 0, 2}},
	{ Line::LINE_20, {0, 2, 2, 2, 0}},
};

Machine::ListLinesResult Machine::getResult()
{
	ListLinesResult results;
	for (int i = 0; i < int(Line::COUNT); ++i)
	{
		auto[slot_, multi_] = getLine(Line(i));
		if (multi_ != Multi::None)
		{
			results.emplace_back(Line(i), slot_, multi_);
		}
	}

	return results;
}

int Machine::resultWinner()
{
	auto resultLine = getResult();
	auto resutlBet = getResultBetFromResultList(resultLine, totalBet);
	//setTotalBet(0);
	setWinner(resutlBet);
	return resutlBet;
}

Machine::ListResultBet Machine::getListResultBet(const ListLinesResult & listLineResult, int totalBet_)
{
	if (listLineResult.empty())
		return {};
	if (totalBet_ == 0)
		return {};

	int countLines = listLineResult.size();
	ListResultBet result;
	int bet = totalBet_ / countLines;
	for (auto &&v : listLineResult)
	{
		auto[line_, slot_, multi_] = v;
		int multi = slotMap.at(slot_).at(multi_);
		int res_bet = bet * multi;
		result.emplace_back(line_, res_bet);
	}
	
	return result;
}

int Machine::getResultBetFromResultList(const ListLinesResult & listLineResult, int totalBet_)
{
	auto res = getListResultBet(listLineResult, totalBet_);
	auto sum = std::accumulate(res.begin(), res.end(), 0, [](int a, const ResultBet &b) 
	{
		return a + std::get<1>(b);
	});
	return sum;
}



bool Machine::chekLine(Line line)
{
	auto[slot, multi] = getLine(line);
	return multi != Multi::None;
}


Machine::SlotMylti Machine::getLine(Line line)
{

	auto points = linePointsMap.at(line);

	Multi multi = Multi::None;
	Slot slot = getSlot(0, points.at(0));
	
	for (int i = 0; i < COLS; ++i)
	{

		if (getSlot(i, points.at(i)) != slot)
			break;

		if (i == 2)
			multi = Multi::X3;
		else if (i == 3)
			multi = Multi::X4;
		else if (i == 4)
			multi = Multi::X5;
	}
	

	return SlotMylti{ slot, multi };
}

nlohmann::json Machine::machineToJSON(const Machine & machine)
{
	using json = nlohmann::json;

	
	std::vector<int> matrixInt(ROWS * COLS, 0);
	for (int i = 0; i < machine.matrix.size(); ++i)
		matrixInt[i] = (int)machine.matrix.at(i);

	//auto j_matrix = json(matrixInt);
	json machineJson;
	
	machineJson["Matrix"] = matrixInt;
	machineJson["TotalBet"] = machine.totalBet;
	machineJson["Win"] = machine.winner;
	machineJson["Status"] = (int)machine.status;

	return machineJson;
}

Machine Machine::machineFromJSON(const nlohmann::json &j)
{

	std::vector<int> matrixInt = j["Matrix"];
	Matrix matrix;
	for (int i = 0; i < matrixInt.size(); ++i)
		matrix.at(i) = (Slot)matrixInt.at(i);

	return Machine(
		matrix, 
		j["TotalBet"].get<int>(),
		j["Win"].get<int>(),
		(Status)j["Status"].get<int>()
		);
}

Machine::Matrix Machine::getRandomMatrix()
{
	Matrix matrix = {};
	auto randSlot = []() -> std::tuple<Slot, Slot, Slot>
	{
		int count = int(Slot::COUNT);
		int slot = rand() % count;
		int slot_nex = 0;
		int slot_prev = 0;
		if (slot + 1 == count)
		{
			slot_nex = 0;
		}
		else
		{
			slot_nex = slot + 1;
		}

	    if (slot - 1 < 0)
		{
			slot_prev = count - 1;
		}
		else
		{
			slot_prev = slot - 1;
		}

		return { Slot(slot_nex), Slot(slot), Slot(slot_prev) };
	};

	auto randomColum = [&matrix, &randSlot](int colum) -> void
	{
		auto[row_0, row_1, row_2] = randSlot();
		matrix.at(colum) = row_0;
		matrix.at(1 * COLS + colum) = row_1;
		matrix.at(2 * COLS + colum) = row_2;
	};

	for (int i = 0; i < COLS; ++i)
		randomColum(i);

	return matrix;
}

