#pragma once

#include <cocos2d.h>
#include <ui/UIButton.h>
#include <memory>

#include "GameSprite.h"

#include "Client.h"

class Machine;
class DrumNode;

/*
	������� ������ �����.
*/
class GameScene : public cocos2d::Scene
{

public:
	/*
		������� ������������� � ����� cocos2d
		
		� ���� ������� ���������� ������ �� ������� �������
		��� ��������� �������-������ ����. 
		������ ������� �������� ��� �������� ���. ���� ������ �� �������� - 
		���� ��������� ������ ������ ��������
	*/
	bool init(IClient *client);

	/*
		���������������� ���������� ������� � �������.

	*/
	bool initContent(const IClient::BinnaryMap &);

	/*
		����������� ��������� ������� � ����� cocos2d
	*/
	static GameScene* create(IClient *client_);

	void update(float dt);
private:
	/*
		������������� ����� ����
		� ���� ������ ���������������� ��������� ����������
	*/
	bool initGame();
	/*������������� * */

	bool initMachine();	//�������� (�� ����� ����� �������)
	bool initParts(); //�� ���������
	bool initBG();  //�������� � ��� ������� node
	bool initDrum(); //������������� ��������
	bool initReelShadow(); //����
	bool initSpinButton(); //������ spin
	bool initBetButton(); //������ ���� � �����, ��� �� labelBet
	bool initLabelWinner(); //���� WIN

	/*
		���������� ��������� �������� � �����
		�� ����� ��������� totalBet
	*/
	void updateStatus();
	//�� ���������
	void pause();
	//�� ���������
	void resume();

	/*
		��������� ������� ������ SPIN
	*/
	void onSpin();

	/* ��������� ������ ���� � ����� */
	void addBet();
	void delBet();

	/* ��������� �������� */
	void onStoppedDrum();

	IClient *client = nullptr;

	/*

		����� ���� �� ������� ������� Node/Ref ��� Machine. 
		�� ����� �������� �������� ��������������� ������������ ����������� � ���������
	*/
	std::unique_ptr<Machine> machine = nullptr; 

	/*
		�������� �������� � ��������.
		� ������������ ������ �� �������, ��� ����� ���� Image � Texture2D, 
		����� �� ���������� ��� �������� SpriteFrame. �� ������ ������ ����������� � ���������
		������, �.�. �� ������ �� Ref ���������.
	*/
	cocos2d::Vector<cocos2d::Image*> cacheImage;
	cocos2d::Vector<cocos2d::Texture2D *> cacheTexture;
	
	//������ ������� ���������
	using PartsID = GameSprite::Parts;

	DrumNode *drum;

	cocos2d::Point origin = {};
	cocos2d::Size visibleSize = {};
	cocos2d::Size desingSize = {};
	float scaleFactorX = 1.0f;
	float scaleFactorY = 1.0f;
	
	cocos2d::Director *director = nullptr;

	cocos2d::ui::Button *spinButton = nullptr;
	cocos2d::ui::Button *plusButton = nullptr;
	cocos2d::ui::Button *minusButton = nullptr;

	cocos2d::Sprite *backgroud = nullptr;
	cocos2d::Sprite *reelShadowSprite = nullptr;
	cocos2d::Node *backSprite = nullptr;

	cocos2d::Label *totalBetLabel = nullptr;
	cocos2d::Label *winnerLabel = nullptr;

	bool winner = false;
	bool waitStartSpin = false;
	bool waitStopSpin = false;
	bool waitMachine = false;
	bool waitWinner = false;
};