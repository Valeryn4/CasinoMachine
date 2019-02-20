#include "GameScene.h"
#include "DrumNode.h"

#include <ui/UIButton.h>

USING_NS_CC;



bool GameScene::initGame()
{
	director = Director::getInstance();
	origin = director->getVisibleOrigin();
	visibleSize = director->getVisibleSize();
	desingSize = director->getOpenGLView()->getDesignResolutionSize();
	scaleFactorX = desingSize.width / visibleSize.width;
	scaleFactorY = desingSize.height / visibleSize.height;

	if (!initMachine())
		return false;

	if (!initParts())
		return false;

	if (!initBG())
		return false;

	if (!initDrum())
		return false;

	if (!initReelShadow())
		return false;

	if (!initBetButton())
		return false;

	if (!initSpinButton())
		return false;

	if (!initLabelWinner())
		return false;

	this->scheduleUpdate();

	return true;
}

GameScene * GameScene::create(IClient * client_)
{
	GameScene * ret = new (std::nothrow) GameScene();

	if (ret && client_ && ret->init(client_))
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}


void GameScene::update(float dt)
{
	static float sec = 0;
	static int second = 1;

	sec += dt;

	/*

	*/
	if (sec >= 1.0f)
	{
		sec = 0;
		second++;
	}

	/*
		каждые пять секунду запрашиваем состояние аппарата. Вдруг сервер остановил вращение
	*/
	if (second % 6 == 0)
	{
		client->getMachine();
		second = 1;
	}

	

}

bool GameScene::initMachine()
{
	machine = std::make_unique<Machine>();
	return true;
}

bool GameScene::init(IClient * client_)
{
	client = client_;
	CCASSERT(client != nullptr, "client is nullptr!");

	this->addChild(client);


	/*
		обработка ответа от сервера, если пришли
		данные со спрайтами
	*/
	client->onLoadContentFiles = [this](const IClient::BinnaryMap &fileMap)
	{
		auto res = initContent(fileMap);
		CCASSERT(res, "InitClient failed!");

		res = initGame();
		CCASSERT(res, "InitLoadFile failed!");

	};

	client->loadContentFiles();

	return true;
}


/*
	Инициализируем контент игры
*/
bool GameScene::initContent(const IClient::BinnaryMap &fileMap)
{
	CCASSERT(client != nullptr, "client is nullptr!");

	//Кол бэк функция, которая обновляет данные автомата
	client->onUpdateMachine = [this](const Machine &new_machine) -> void
	{
		(*machine) = new_machine;
		updateStatus();
		CCLOG("Machine update! TotalBet: %i", machine->getTotalBet());
	};

	//действия при разрыве связи
	client->onDisconected = [this]() -> void
	{
		pause();
		if (!client->connect(1000))
			CCASSERT(true, "client connect failed");
	};

	//действия при подключении
	client->onConnected = [this]() -> void
	{
		resume();
	};

	
	client->onAddBet = [this]() -> void
	{
		CCLOG("Plus Bet");
	};

	client->onDelBet = [this]() -> void
	{
		CCLOG("Minus Bet");
	};


	//Кэширует спрайты в SpriteFrameCache
	auto cachedSpriteFrame = [this](const uint8_t *data, size_t len, const std::string &spriteName) -> SpriteFrame*
	{
		auto img = new (std::nothrow) Image();
		if (!img)
			return nullptr;

		img->autorelease();
		img->initWithImageData(data, len);
		cacheImage.pushBack(img);

		auto tex = new (std::nothrow) Texture2D();
		if (!tex)
			return nullptr;

		tex->autorelease();
		tex->initWithImage(img);
		cacheTexture.pushBack(tex);

		auto spriteFrame = SpriteFrame::createWithTexture(tex, Rect({0,0}, tex->getContentSize()));
		if (!spriteFrame)
			return nullptr;

		SpriteFrameCache::getInstance()->addSpriteFrame(spriteFrame, spriteName);

		return spriteFrame;
	};

	for (auto&&[key, data] : fileMap)
	{
		auto res = cachedSpriteFrame(data.data(), data.size(), key);
		CCASSERT(res != nullptr, "cached sprite frame failed! res == nullptr");
	}

	return true;
}

bool GameScene::initParts()
{
	return true;
}

bool GameScene::initBG()
{
	backgroud = Sprite::createWithSpriteFrameName(GameSprite::getSpriteName(PartsID::BACK));
	backgroud->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	backgroud->setPosition(origin + (visibleSize / 2.f));
	backgroud->setScale(scaleFactorX, scaleFactorY);
	this->addChild(backgroud);

	return true;
}

bool GameScene::initDrum()
{
	

	float bottomPos = backgroud->getContentSize().height - 482.f;

	drum = DrumNode::create();
	drum->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	drum->setPosition({ backgroud->getContentSize().width / 2.f, bottomPos }); //середина, низ
	drum->onStopped = CC_CALLBACK_0(GameScene::onStoppedDrum, this);

	backgroud->addChild(drum);

	//MASK Делаем маску, что бы скрыть полоски спрайтов
	//Дублируем бэкграунд, вырезаем центр, вставляем по верх Drum. Что бы задние спрайты у нас спрятались.

	auto maskBackground = Sprite::createWithSpriteFrameName(GameSprite::getSpriteName(PartsID::BACK));
	maskBackground->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	auto mask = Sprite::createWithSpriteFrameName(GameSprite::getSpriteName(PartsID::REEL_SHADOW));
	mask->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	mask->setPosition({ maskBackground->getContentSize().width / 2.f, bottomPos });

	auto cliper = ClippingNode::create(mask);
	cliper->setInverted(true);
	cliper->addChild(maskBackground);
	backgroud->addChild(cliper);

	backSprite = cliper;



	return true;
}

bool GameScene::initReelShadow()
{
	

	float bottomPos = backgroud->getContentSize().height - 482.f;

	reelShadowSprite = Sprite::createWithSpriteFrameName(GameSprite::getSpriteName(PartsID::REEL_SHADOW));
	reelShadowSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	reelShadowSprite->setPosition({ backgroud->getContentSize().width / 2.f, bottomPos });
	backgroud->addChild(reelShadowSprite);


	
	return true;
}

bool GameScene::initSpinButton()
{

	float bottomPos = backgroud->getContentSize().height - 608.f - 10.f;
	float rightPos = 720.f;

	auto spinName = GameSprite::getSpriteName(PartsID::SPIN);
	auto spinPressName = GameSprite::getSpriteName(PartsID::SPIN_PRESS);
	auto button = ui::Button::create(spinName, spinPressName, spinPressName, ui::Widget::TextureResType::PLIST);
	
	button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	button->setPosition({ rightPos, bottomPos });

	button->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
	{
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			onSpin();
			break;
		case ui::Widget::TouchEventType::ENDED:
			break;
		default:
			break;
		}
	});

	backgroud->addChild(button);
	spinButton = button;


	/*
		Обработка событий. При получение от клиента колбэка start или stop.
		Задаю именно эти функции тут, а не раньше, из за 
		lusButton->setEnabled(false);
		minusButton->setEnabled(false);
	*/

	using SlotID = DrumNode::SlotID;
	client->onSpinStop = [this]() -> void
	{
		drum->stopRotate({
				(SlotID)machine->getSlot(0, 2),
				(SlotID)machine->getSlot(1, 2),
				(SlotID)machine->getSlot(2, 2),
				(SlotID)machine->getSlot(3, 2),
				(SlotID)machine->getSlot(4, 2)
			});

	
		CCLOG("onSpinStop");
	};

	client->onSpinStart = [this]() -> void
	{
		
		drum->resetWinnerLine();
		drum->startRotate();

		plusButton->setEnabled(false);
		minusButton->setEnabled(false);
		

		waitStartSpin = false;
		winnerLabel->setString("Good LUCK!");
		CCLOG("onSpinStart");

	};

	return true;
}

bool GameScene::initBetButton()
{
	float leftPos = 113.f;
	float bottomPos = backgroud->getContentSize().height - 608.f - 10.f;
	float rightPos = 326.f;

	auto plusName = GameSprite::getSpriteName(PartsID::PLUS);
	auto plusPressName = GameSprite::getSpriteName(PartsID::PLUS_PRESS);
	plusButton = ui::Button::create(plusName, plusPressName, plusPressName, ui::Widget::TextureResType::PLIST);

	plusButton->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	plusButton->setPosition({ rightPos, bottomPos });
	plusButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
	{
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			addBet();
			break;
		case ui::Widget::TouchEventType::ENDED:
			break;
		default:
			break;
		}
	});

	backgroud->addChild(plusButton);
	


	auto minusName = GameSprite::getSpriteName(PartsID::MINUS);
	auto minusPressName = GameSprite::getSpriteName(PartsID::MINUS_PRESS);
	minusButton = ui::Button::create(minusName, minusPressName, minusPressName, ui::Widget::TextureResType::PLIST);

	minusButton->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	minusButton->setPosition({ leftPos, bottomPos });
	minusButton->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
	{
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			delBet();
			break;
		case ui::Widget::TouchEventType::ENDED:
			break;
		default:
			break;
		}
	});

	backgroud->addChild(minusButton);

	auto widthText = plusButton->getBoundingBox().getMinX() - minusButton->getBoundingBox().getMaxX();
	auto heightText = minusButton->getContentSize().height - 10.f;

	totalBetLabel = Label::createWithBMFont("fonts/myriad.fnt", "0", cocos2d::TextHAlignment::CENTER);
	totalBetLabel->setContentSize({ widthText, heightText });
	totalBetLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	totalBetLabel->setPosition(minusButton->getBoundingBox().getMaxX() + (widthText / 2), bottomPos + 10.f + (heightText / 2));
	backgroud->addChild(totalBetLabel);

	return true;
}

bool GameScene::initLabelWinner()
{
	float posX = totalBetLabel->getPositionX() + 250;
	winnerLabel = Label::createWithBMFont("fonts/myriad.fnt", "0", cocos2d::TextHAlignment::CENTER);
	winnerLabel->setContentSize(totalBetLabel->getContentSize());

	winnerLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	winnerLabel->setPosition(posX, totalBetLabel->getPositionY());

	backgroud->addChild(winnerLabel);

	client->onWinner = [this](unsigned int win)
	{
		waitWinner = false;
		CCLOG("Winner!");
	};

	return true;
}

void GameScene::updateStatus()
{
	totalBetLabel->setString(std::to_string(machine->getTotalBet()));
}

void GameScene::pause()
{
}

void GameScene::resume()
{
}

void GameScene::onSpin()
{
	using MStatus = Machine::Status;
	auto status = machine->getStatus();
	switch (status)
	{
	case MStatus::Spin :
		{
			CCLOG("Spin stop");

			client->spinStop();

			spinButton->setEnabled(false);

			/*замораживаем кнопку на пару секунду, после старта */
			auto delay = DelayTime::create(5.0f);
			auto enableButton = CallFunc::create([this]()
			{
				spinButton->setEnabled(true);
			});
			auto seq = Sequence::create(delay, enableButton, nullptr);
			spinButton->runAction(seq);
			
		}
		break;
	case MStatus::Stop :
		if (waitStartSpin || machine->getTotalBet() <= 0 || drum->isRotate())
			break;
		{
			waitStartSpin = true;
			waitWinner = true;

			spinButton->setEnabled(false);

			/*замораживаем кнопку на секунду после стопа или до ответа от сервера*/
			auto delay = DelayTime::create(1.0f);
			auto enableButton = CallFunc::create([this]()
			{
				spinButton->setEnabled(true);
			});
			auto seq = Sequence::create(delay, enableButton, nullptr);
			spinButton->runAction(seq);
			CCLOG("Spin start");

			client->spinStart();
		}
		break;
	default:
		break;
	}
}

void GameScene::addBet()
{
	client->addBet();
}

void GameScene::delBet()
{
	client->delBet();
}

/*
	ответ от барабана, что он остановился.
	Чертем линии комбинаций
*/
void GameScene::onStoppedDrum()
{
	winnerLabel->setString(std::to_string(machine->getWinner()));
	if (machine->getWinner() > 0)
		CCLOG("WINNER %i ", machine->getWinner());


	plusButton->setEnabled(true);
	minusButton->setEnabled(true);
	spinButton->setEnabled(true);
	drum->drawWinnerLine(machine->getResult());

	CCLOG("MATRIX:");
	for (int row = 0; row < 3; row++)
	{
		int slot[5] = {};
		for (int col = 0; col < 5; col++)
		{
			auto slotID = int(machine->getSlot(col, row));
			slot[col] = slotID + 1;
		}

		CCLOG(" %i  %i  %i  %i  %i ", slot[0], slot[1], slot[2], slot[3], slot[4]);
	}
	
	CCLOG("END MATRIX");
}
