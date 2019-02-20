#include "GameSprite.h"

using Parts = GameSprite::Parts;
using Symbols = GameSprite::Symbols;

USING_NS_CC;

GameSprite::SymbolNameMap GameSprite::symbols = 
{
	{ Symbols::SLOT_1, "1.png" },
	{ Symbols::SLOT_2, "2.png" },
	{ Symbols::SLOT_3, "3.png" },
	{ Symbols::SLOT_4, "4.png" },
	{ Symbols::SLOT_5, "5.png" },
	{ Symbols::SLOT_6, "6.png" },
	{ Symbols::SLOT_7, "7.png" },
	{ Symbols::SLOT_8, "8.png" },
	{ Symbols::SLOT_9, "9.png" },
	{ Symbols::SLOT_10, "10.png" }
};

GameSprite::PartNameMap GameSprite::parts =
{
	{ Parts::BACK,			"back.png" },
	{ Parts::MINUS,			"minus.png" },
	{ Parts::MINUS_PRESS,	"minus_press.png" },
	{ Parts::PLUS,			"plus.png" },
	{ Parts::PLUS_PRESS,	"plus_press.png" },
	{ Parts::REEL_SHADOW,	"reel_shadow.png" },
	{ Parts::SPIN,			"spin.png" },
	{ Parts::SPIN_PRESS,	"spin_press.png" }
};
