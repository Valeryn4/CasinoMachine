#pragma once

#include <cocos2d.h>
#include <ui/UIButton.h>
#include <memory>

#include "GameSprite.h"

#include "Client.h"

class Machine;
class DrumNode;

/*
	Главная игрока сцена.
*/
class GameScene : public cocos2d::Scene
{

public:
	/*
		базовая инициализация в стиле cocos2d
		
		в этой функции происходит запрос на сторону сервера
		для получение контент-файлов игры. 
		сейчас никакой проверки или ожидания нет. Если данные не получены - 
		игра останется висеть черным экрамном
	*/
	bool init(IClient *client);

	/*
		инициализируется полученный контент с сервера.

	*/
	bool initContent(const IClient::BinnaryMap &);

	/*
		стандартная фабричная функция в стиле cocos2d
	*/
	static GameScene* create(IClient *client_);

	void update(float dt);
private:
	/*
		Инициализация самой игры
		в этой фунции инициализируются остальные компоненты
	*/
	bool initGame();
	/*Инициализация * */

	bool initMachine();	//аппарата (по факту нашей матрицы)
	bool initParts(); //не актуально
	bool initBG();  //бэграунд и наш ведущий node
	bool initDrum(); //инициализация барабана
	bool initReelShadow(); //тень
	bool initSpinButton(); //кнопка spin
	bool initBetButton(); //кнопки плюс и минус, тут же labelBet
	bool initLabelWinner(); //поле WIN

	/*
		обновление состояния аппарата и полей
		по факту обновляет totalBet
	*/
	void updateStatus();
	//не актуально
	void pause();
	//не актуально
	void resume();

	/*
		обработка нажатия кнопки SPIN
	*/
	void onSpin();

	/* обработка кнопок плюс и минус */
	void addBet();
	void delBet();

	/* остановка барабана */
	void onStoppedDrum();

	IClient *client = nullptr;

	/*

		можно было бы сделать обертку Node/Ref для Machine. 
		Но тогда повторно придется реализовавывать конструкторы копирования и операторы
	*/
	std::unique_ptr<Machine> machine = nullptr; 

	/*
		Кэшируем текстуры и картинки.
		В документации ничего не сказано, как ведет себя Image и Texture2D, 
		когда их используют для создания SpriteFrame. На всякий случай закешировал в кокосовый
		вектор, т.к. он следит за Ref счетчиком.
	*/
	cocos2d::Vector<cocos2d::Image*> cacheImage;
	cocos2d::Vector<cocos2d::Texture2D *> cacheTexture;
	
	//просто упрости написание
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