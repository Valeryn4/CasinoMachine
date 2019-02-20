#include "DrumNode.h"
#include "GameSprite.h"

USING_NS_CC;


bool DrumNode::init()
{
	auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(GameSprite::getSpriteName(GameSprite::Parts::REEL_SHADOW));
	auto frameSlot = SpriteFrameCache::getInstance()->getSpriteFrameByName(GameSprite::getSpriteName(GameSprite::Symbols::SLOT_1));
	spriteSize = frameSlot->getOriginalSize();
	this->setContentSize(frame->getRect().size);

	for (int iDrum = 0; iDrum < int(DrumID::COUNT); ++iDrum)
	{
		SlotSpriteMap slotSpriteMap;

		
		for (int iSlot = 0; iSlot < int(SlotID::COUNT); ++iSlot)
		{
			auto slotID = SlotID(iSlot);
			auto sprite = Sprite::createWithSpriteFrameName(GameSprite::getSpriteName(slotID));
			auto[it, res] = slotSpriteMap.emplace(slotID, sprite);

			CCASSERT(res, "Slot sprite map emplace failed!");

			auto height = sprite->getContentSize().height;
			auto width = sprite->getContentSize().width;
			sprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			sprite->setPosition(iDrum * width, iSlot * width);

			this->addChild(sprite);
		}

		drumSprites.at(iDrum) = std::move(slotSpriteMap);
		drumSlotTop.at(iDrum) = SlotID(int(SlotID::COUNT) - 1);
		drumSlot.at(iDrum) = SlotID(1);
	}

	for (auto&& v : drumRotate)
		v = false;

	for (auto&& v : drumSlotTarget)
		v = SlotID::UNKNOW;

	started = false;

	drawLineNode = DrawNode::create();

	this->scheduleUpdate();

	return true;
}

void DrumNode::animationRotateNex(DrumID drum)
{
	int iDrum = (int)drum;

	auto& mapSprite = drumSprites.at(iDrum);

	auto& slotID_top = drumSlotTop.at(iDrum);
	auto& slotID_current = drumSlot.at(iDrum);

	if (int(slotID_current) + 1 == int(SlotID::COUNT))
	{
		slotID_current = SlotID::SLOT_1;
	}
	else
	{
		slotID_current = SlotID(int(slotID_current) + 1);
	}

	for (auto[slotID, slot] : mapSprite)
	{
		auto sprite = slot; //у меня IDE гонит, не видит переменной
		auto pos = sprite->getPosition();
		auto size = sprite->getContentSize();
		
		drumRotatedStatus.at(iDrum) = true;
		auto move = MoveBy::create(delayTime, Point(0, -1 * size.height));
		auto stopFunc = CallFunc::create([this, iDrum]()
		{
			drumRotatedStatus.at(iDrum) = false;
			
		});

		sprite->runAction(Sequence::create(move, stopFunc, nullptr));
	}
}

void DrumNode::startRotate()
{
	for (auto&& v : drumSlotTarget)
		v = SlotID::UNKNOW;
	started = true;
}

void DrumNode::stopRotate(const DrumSlot & slots)
{
	drumSlotTarget = slots;
}

bool DrumNode::isRotate()
{
	for (auto&& v : drumRotate)
		if (v) return true;

	return false;
}

void DrumNode::update(float delta)
{
	static float sec = 0;
	sec += delta;

	bool started_released = false;
	for (int iDrum = 0; iDrum < int(DrumID::COUNT); iDrum++ )
	{
		auto& mapSprite = drumSprites.at(iDrum);

		//Перенос спрайта вверх
		for (auto[slotID, slot] : mapSprite)
		{
			auto sprite = slot; //у меня IDE гонит, не видит переменной
			auto pos = sprite->getPosition();
			auto size = sprite->getContentSize();
			if (pos.y + size.height <= 0)
			{
				auto slotID_top = drumSlotTop.at(iDrum);
				auto slotID_current = drumSlot.at(iDrum);

				auto spriteTop = mapSprite.at(slotID_top);
				auto spriteTopMaxY = spriteTop->getBoundingBox().getMaxY();
				sprite->setPosition(pos.x, spriteTopMaxY);

				drumSlotTop.at(iDrum) = slotID;

			}
		}
	

		if (drumRotate.at(iDrum))
		{
			if (drumRotatedStatus.at(iDrum))
			{
				//No update
			}
			else 
			{
				bool next = true;
				if (drumSlotTarget.at(iDrum) == drumSlot.at(iDrum))
				{
					bool prevNoRotate = false;
					if (iDrum == 0)
					{
						prevNoRotate = true;
					}
					else if (!drumRotate.at(iDrum - 1))
					{
						prevNoRotate = true;
					}
	

					if (prevNoRotate)
					{
						
						drumRotate.at(iDrum) = false;
						CCLOG("STOP ID: %i", int(drumSlotTarget.at(iDrum)));
						drumSlotTarget.at(iDrum) = SlotID::UNKNOW;

						if (iDrum == int(DrumID::COUNT) - 1)
						{
							if (onStopped)
								onStopped();
						}
					}
				}

				if (next)
				{

					animationRotateNex(DrumID(iDrum));
				}
			}
		}	

		if (started && !drumRotate.at(iDrum))
		{
			auto start = CallFunc::create([this, iDrum]() 
			{
				drumRotate.at(iDrum) = true;
				animationRotateNex(DrumID(iDrum));
			});
			auto delay = DelayTime::create(delayStart * iDrum);
			auto seq = Sequence::create(delay, start, nullptr);

			this->runAction(seq);
			started_released = true;
		}

	}

	if (started_released)
		started = false;

	if (sec >= 0.5)
		sec = 0;
	
}

void DrumNode::drawWinnerLine(const Machine::ListLinesResult & list)
{	
	auto width = spriteSize.width;
	auto height = spriteSize.height;

	float r = 0.2f;
	float g = 0.3f;
	for (auto&& res : list)
	{
		auto[line, slot, multi] = res;
		auto points = Machine::getLinePoints(line);

		auto color = Color4F(r += 0.2f, g += 0.1f, 0.6f, 1.0f);

		auto prevPoint = Point(0, (2 - points.at(0)) * height  + (height * 0.5f) );
		for (int i = 0; i < points.size(); ++i)
		{
			auto nexPoint = Point(
				i * width + (width * 0.5f),
				(2 - points.at(i)) * height + (height * 0.5f)
			);

			auto draw = DrawNode::create(5.f);
			draw->drawLine(prevPoint, nexPoint, color);
			drawNodeList.push_back(draw);
			this->addChild(draw);
			prevPoint = nexPoint;
		}


		auto nexPoint = Point(
			this->getContentSize().width,
			(2 - points.back()) * height + (height * 0.5f)
		);

		auto draw = DrawNode::create(5.f);
		draw->drawLine(prevPoint, nexPoint, color);
		drawNodeList.push_back(draw);
		this->addChild(draw);
	}

}

void DrumNode::resetWinnerLine()
{
	for (auto&& node : drawNodeList)
	{
		if (node != nullptr)
			node->removeFromParentAndCleanup(true);
	}

	drawNodeList.clear();
}
