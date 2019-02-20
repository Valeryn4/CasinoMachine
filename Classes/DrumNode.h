#pragma once

#include <cocos2d.h>
#include <array>

#include "GameSprite.h"
#include "Machine.h"

/*
	������ ��������
	�� �������� �� ��� �������� ������ ����.
	��� �� ������ �� ������������, � ������ ���������� ��������

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


	using SlotID = GameSprite::Symbols; //ID �����/�������
	using SlotSpriteMap = std::unordered_map<SlotID, cocos2d::Sprite*>; //����� ������
	using DrumSpritesArray = std::array<SlotSpriteMap, size_t(DrumID::COUNT)>; //������ ���������. �� ������ ������� ���������� ���� ����� ��������

	using DrumSlot = std::array<SlotID, size_t(DrumID::COUNT)>; //� ����� ��������, ����� ���� ������ ����������.

	/* ������ �������� */
	void startRotate();
	/* ���������� �������� �� ������������ ���������� */
	void stopRotate(const DrumSlot &slots);
	/* �������� �� �������� */
	bool isRotate();


	void update(float delta);

	/* ������ ����� ���������� */
	void drawWinnerLine(const Machine::ListLinesResult &list);
	/* ������� ����� ���������� */
	void resetWinnerLine();

	/* �������� � ���, ��� ������� ����������� */
	std::function<void()> onStopped = nullptr;

private:
	/* ���������� �� ���� ��� ���������� �������*/
	void animationRotateNex(DrumID drum);

	DrumSpritesArray drumSprites; //�������
	DrumSlot drumSlot; //����� � ���������
	DrumSlot drumSlotTop; //����� �������� ��������

	DrumSlot drumSlotTarget; //����� � ���������

	std::array<bool, size_t(DrumID::COUNT)> drumRotatedStatus = {};
	std::array<bool, size_t(DrumID::COUNT)> drumRotate = {};
	bool started = false;

	float delayStart = 0.3f; //����� ������
	float delayTime = 0.1f;
	
	cocos2d::DrawNode *drawLineNode;
	std::vector<cocos2d::DrawNode*> drawNodeList;
	cocos2d::Size spriteSize;
};