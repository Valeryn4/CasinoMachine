#pragma once

#include <unordered_map>
#include <string>

#include <cocos2d.h>

/*
	Простой класс, что бы получать имя спрайта по енуму. Я бы обьеденил это с 
	machine. Но тот класс используется еще и в сервере, без cocos2d
	
*/
class GameSprite
{
public:
	enum class Symbols
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

	enum class Parts
	{
		BACK = 0,
		MINUS,
		MINUS_PRESS,
		PLUS,
		PLUS_PRESS,
		REEL_SHADOW,
		SPIN,
		SPIN_PRESS,

		COUNT,
		UNKNOW = COUNT
	};

	/*
		Получаем имя спрайта по Enum
	*/
	static std::string getSpriteName(Symbols id)
	{
		CCASSERT(id != Symbols::UNKNOW, "Symbols id is UNKNOW!");
		return "Symbols/" + symbols.at(id);
	}

	/*
		Получаем имя спрайта по Enum
	*/
	static std::string getSpriteName(Parts id)
	{
		CCASSERT(id != Parts::UNKNOW, "Parts id is UNKNOW!");
		return "Parts/" + parts.at(id);
	}

	using SymbolNameMap = std::unordered_map<Symbols, std::string>;
	using PartNameMap = std::unordered_map<Parts, std::string>;

private:
	static SymbolNameMap symbols;
	static PartNameMap parts;
};