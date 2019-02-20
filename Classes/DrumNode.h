#pragma once

#include <cocos2d.h>
#include <array>

#include "GameSprite.h"
#include "Machine.h"

/*
	Обьект барабана
	Он отвечает за все анимации внутри себя.
	Сам он ничего не просчитывает, а только показывает картинку

*/
class DrumNode : public cocos2d::Node
{
public:

	bool init() override;
	
	CREATE_FUNC(DrumNode);

	enum class DrumID : int
	{
		DRUM_1 = 0,
		DRUM_2,
		DRUM_3,
		DRUM_4,
		DRUM_5,

		COUNT,
		UNKNOW = COUNT
	};


	using SlotID = GameSprite::Symbols; //ID слота/спрайта
	using SlotSpriteMap = std::unordered_map<SlotID, cocos2d::Sprite*>; //карта слотов
	using DrumSpritesArray = std::array<SlotSpriteMap, size_t(DrumID::COUNT)>; //список барабанов. На каждый барабан приходится своя карта спрайтов

	using DrumSlot = std::array<SlotID, size_t(DrumID::COUNT)>; //в каком барабане, какой слот сейчас находиться.

	/* начать вращение */
	void startRotate();
	/* остановить вращение на определенной комбинации */
	void stopRotate(const DrumSlot &slots);
	/* проверка на вращение */
	bool isRotate();


	void update(float delta);

	/* рисует линии комбинаций */
	void drawWinnerLine(const Machine::ListLinesResult &list);
	/* стирает линии комбинаций */
	void resetWinnerLine();

	/* сообщает о том, что барабан остановился */
	std::function<void()> onStopped = nullptr;

private:
	/* прокрутить на один шаг конкретный барабан*/
	void animationRotateNex(DrumID drum);

	DrumSpritesArray drumSprites; //спрайты
	DrumSlot drumSlot; //слоты в барабанах
	DrumSlot drumSlotTop; //самые вверхние элементы

	DrumSlot drumSlotTarget; //слоты в барабанах

	std::array<bool, size_t(DrumID::COUNT)> drumRotatedStatus = {};
	std::array<bool, size_t(DrumID::COUNT)> drumRotate = {};
	bool started = false;

	float delayStart = 0.3f; //пауза старта
	float delayTime = 0.1f;
	
	cocos2d::DrawNode *drawLineNode;
	std::vector<cocos2d::DrawNode*> drawNodeList;
	cocos2d::Size spriteSize;
};