#include "GameScene.h"
#include <string>
#include "SimpleAudioEngine.h"
#include <algorithm>
#include "GameOver.h"
#pragma execution_character_set("utf-8")
#define database UserDefault::getInstance()
USING_NS_CC;

using namespace std;
using namespace CocosDenshion;

Scene* GameScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//  the origin position is in the leftmost and downmost
	if (!Layer::init())
	{
		return false;
	}
	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	// background
	visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto background = Sprite::create("background.png");
	background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height/2));
	this->addChild(background, 0);
	//----------------------------------------------
	// 显示分数和母鸡数量
	eggsNum = Label::createWithTTF("EggsNum: 0", "fonts/arial.TTF", 40);
	eggsNum->setColor(Color3B(25, 25, 25));
	eggsNum->setPosition(150, 600);
	this->addChild(eggsNum, 3);
	hensNum = Label::createWithTTF("HensNum: 8", "fonts/arial.TTF", 40);
	hensNum->setColor(Color3B(25, 25, 25));
	hensNum->setPosition(150, 550);
	this->addChild(hensNum, 3);

	database->setIntegerForKey("scoreInt", 0);

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;

	// functions
	addFarmer();//初始化农夫
	addEagle();// 初始化老鹰
	addHens(0);//初始化母鸡20只
	preloadMusic(); // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(GameScene::update), 0.04f, kRepeatForever, 0);
	schedule(schedule_selector(GameScene::lay), 2.5f, kRepeatForever, 0);

	//从贴图中以像素单位切割，小人的动画切割
	auto texture1 = Director::getInstance()->getTextureCache()->addImage("walk.png");
	walkFrame.reserve(5);
	for (int i = 0; i < 4; ++i) {
		auto frame = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0 + 137 * i, 292, 137, 146)));
		walkFrame.pushBack(frame);
	}
	walkFrame.pushBack(frame0);
	return true;
}

//创建母鸡角色
void GameScene::addHens(int num) {
	//随机生成母鸡
	for (unsigned j = 0; j < 6; j++) {
		auto texture = Director::getInstance()->getTextureCache()->addImage("hens.png");
		auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 120, 130)));
		hen = Sprite::createWithSpriteFrame(frame0);
		float x = random(origin.x + visibleSize.width/3, origin.x + visibleSize.width*3/4);
		float y = random(origin.y + visibleSize.height/3, origin.y + visibleSize.height*2/3);
		hen->setPosition(x, y);
		addChild(hen, 3);
		hens.push_back(hen);
	}
}

// 创建农夫
void GameScene::addFarmer() {
	//  use to change the direction of the player
	dir = true; 
    //使用第一帧创建精灵
	auto texture = Director::getInstance()->getTextureCache()->addImage("walk.png");
	frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 292, 137, 146)));
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	player->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 7));
	this->addChild(player, 4);
	
}
//增加老鹰，老鹰的总数最多为5，使用队列
void GameScene::addEagle() {
	auto newEagle = Sprite::create("eagle2.png");
	newEagle->setPosition(Vec2(origin.x + visibleSize.width * 12/13, origin.y + visibleSize.height * 12/ 13));
	this->addChild(newEagle, 3);
	eagles.push_back(newEagle);
}
// 随机数生成鸡蛋
void GameScene::lay(float a) {
	if (hensNum != 0) {
		Sprite* mons = Sprite::create("egg1.png");
		eggs.push_back(mons);
		auto m = mons;
		float x = random(origin.x + 200, origin.x + 800);
		float y = random(origin.y + 150, origin.y + 450);
		m->setPosition(x, y);
		addChild(m, 2);
	}
}


void GameScene::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.wav");
	audio->preloadEffect("music/explore.wav");
	audio->preloadEffect("music/fire.wav");
	audio->preloadEffect("music/游戏结束.wav");
}
//播放背景音乐
void GameScene::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.wav", true);
}

//小人动画移动，四个方向，长按时小人会出边界，一步一步则不会出边界
// setFlipX用于调整小人的左右方向
void GameScene::movePlayer(char c) {
	auto pos = player->getPosition();
	switch (c) {
	case 'D':
		if (dir == false) {
			dir = true;
			player->setFlipX(false);
		}
		if (pos.x < visibleSize.width - 50.0f)
			player->runAction(MoveBy::create(1.0f, Vec2(+15.0f,0)));
		break;
	case 'A':
		if (dir == true) {
			dir = false;
			player->setFlipX(true);
		}
		if (pos.x > 60.0f)
			player->runAction(MoveBy::create(1.0f, Vec2(-15.0f,0)));
		break;
	case 'W':
		if (pos.y < visibleSize.height - 80.0f)
			player->runAction(MoveBy::create(1.0f, Vec2(0, +15.0f)));
		break;
	case 'S':
		if (pos.y > 60.0f)
			player->runAction(MoveBy::create(1.0f, Vec2(0, -15.0f)));
		break;
	}
	//  行走的动画
	auto animation = Animation::createWithSpriteFrames(walkFrame, 0.3f);
	auto animate = Animate::create(animation);
	player->runAction(Repeat::create(animate, 1));
}

//发射子弹
void GameScene::fire() {
	auto bullet = Sprite::create("bullet.png");

	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	auto pos = player->getPosition();
    auto temp = 0;
	if (dir == true)
		temp = 27.0f;
	else  temp = -27.0f;
	bullet->setPosition(Vec2(pos.x+ temp, pos.y));
	addChild(bullet, 1);
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);

	// 移除飞出屏幕外的子弹
	bullet->runAction(MoveBy::create(1.5f, Vec2(0, visibleSize.height - pos.y)));
	auto seq = Sequence::create(MoveBy::create(1.0f, Vec2(0, visibleSize.height - pos.y)), CallFunc::create([this, bullet]() {
		bullet->removeFromParentAndCleanup(true);
		this->bullets.remove(bullet); }), nullptr);
	bullet->runAction(seq);
}
// 切割爆炸动画帧
void GameScene::explosion() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(2, 0, 140, 140))));
	explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(210, 0, 140, 140))));
	explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(400, 0, 140, 140))));
	explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(590, 0, 140, 140))));
	explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(780, 0, 140, 140))));
	explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(2, 190, 140, 140))));
	explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(210, 190, 140, 140))));
	explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(400, 190, 140, 140))));
}

//  每0.04s更新一次
void GameScene::update(float f) {
	if (isMove)
		this->movePlayer(movekey);
	//  鸡蛋数目和母鸡数目
	char str[15];
	sprintf(str, "HensNum: %d", hens.size());
	hensNum->setString(str);
	sprintf(str, "EggsNum: %d", clickeggs.size());
	eggsNum->setString(str);
	database->setIntegerForKey("scoreInt", clickeggs.size());

	//  当母鸡的数目为0时，游戏结束
	if (hens.size() == 0) {
		SimpleAudioEngine::getInstance()->playEffect("music/gameover.wav", false);
		unschedule(schedule_selector(GameScene::update));
		this->getEventDispatcher()->removeAllEventListeners();
		Director::getInstance()->replaceScene(TransitionFade::create(1, GameOver::createScene()));
		return;
	}

	// 老鹰的数目最多是5，如果已经有老鹰在抓母鸡的话，其他的老鹰慢慢飞行
	// 每一定时间后， 队头的老鹰飞下来抓母鸡，抓完母鸡后飞回天空
	static int count = 0;
	count++;
	auto eagleNum = 0;
	for (Sprite* e : eagles) {
		if (e != NULL) {
			eagleNum++;
		}
	}
	//  每隔一定时间，如果老鹰的数目<5的话就会增加老鹰
	if (count == 60) {
		for (Sprite* e : eagles) {
			if (e != NULL) {
				e->setPosition(e->getPosition() + Vec2(-40, 0));
			}
		}
		if (eagleNum < 5) {
			addEagle();
		}	
		count = 0;
	}
	//  当老鹰飞出屏幕的时候，将老鹰从队列中移除
	for (auto it = eagles.begin(); it != eagles.end(); ++it) {
		auto pos = (*it)->getPosition();
		if (pos.x < 0) {
			(*it)->removeFromParentAndCleanup(true);
			it = eagles.erase(it);
		}
	}
	static int eagleAttackCount = 0;
	eagleAttackCount++;
	// 队头的老鹰
	auto eagleIt = eagles.begin();
	for (eagleIt = eagles.begin(); eagleIt != eagles.end(); ++eagleIt) {
		if ((*eagleIt) != NULL)
			break;
	}
	if (eagles.size() == 0) {
		*eagleIt = NULL;
	}

	if ((*eagleIt) != NULL) {
		// 队头的母鸡
		auto henIt = hens.begin();
		for (henIt = hens.begin(); henIt != hens.end(); henIt++) {
			if ((*henIt) != NULL)
				break;
		}
		if (hens.size() == 0) 
			(*henIt) = NULL;
		if ((*henIt) != NULL) {
			auto henPos = (*henIt)->getPosition();
			if (eagleAttackCount == 250) {
				//  老鹰飞向母鸡
				(*eagleIt)->runAction(MoveTo::create(0.88f, henPos));
				eagleAttackCount = 0;
			}
			//  老鹰到达母鸡的位置后，母鸡被抓并且老鹰飞回天空
			if ((*eagleIt)->getPosition().getDistance(henPos) < 45) {
				(*henIt)->runAction(FadeOut::create(1.0f));
				(*henIt)->removeFromParentAndCleanup(true);
				henIt = hens.erase(henIt);
				(*eagleIt)->runAction(MoveTo::create(0.5f, Vec2(visibleSize.width / 13, visibleSize.height * 12 / 13)));
			}
		}
	}
	//  此处是保证天空中至少有一直老鹰
	if (eagles.size() > 1) {
		EventCustom e("meet");
		_eventDispatcher->dispatchEvent(&e);
	}
}


// 自定义碰撞事件
void GameScene::meet(EventCustom * event) {
	// 判断子弹是否打中老鹰并执行对应操作（若打中则老鹰爆炸身亡）
	bool flag = false;
	auto it = bullets.begin();
	while (it != bullets.end()) {
		flag = false;
		for (auto it1 = eagles.begin(); it1 != eagles.end(); ++it1) {
			if ((*it) != NULL && (*it1) != NULL && (*it)->getPosition().getDistance((*it1)->getPosition()) < 25) {
				flag = true;
				auto curMeet = *it1;
				(*it)->removeFromParentAndCleanup(true);
				it = bullets.erase(it);
				auto animate = Animate::create(Animation::createWithSpriteFrames(explore, 0.03f));
				auto seq = Sequence::create(animate, CallFunc::create([this, curMeet]() {
					curMeet->removeFromParentAndCleanup(true);
				}), nullptr);
				(*it1)->runAction(seq);
				it1 = eagles.erase(it1);
				auto audio = SimpleAudioEngine::getInstance();
				audio->playEffect("music/explore.wav", false);
				break;
			}
		}
		if (flag == false) {
			it++;
		}
	}
}

//  添加自定义监听器
void GameScene::addCustomListener() {
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(GameScene::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

//  添加键盘事件监听器
void GameScene::addKeyboardListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	dispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}
void GameScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_W:
		movekey = 'W';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_S:
		movekey = 'S';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
	case EventKeyboard::KeyCode::KEY_ENTER:
		fire();
		break;
	}
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
	case EventKeyboard::KeyCode::KEY_W:
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case EventKeyboard::KeyCode::KEY_S:
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
		isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void GameScene::addTouchListener() {
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// 鼠标点击鸡蛋，鸡蛋飞回鸡蛋篮
bool GameScene::onTouchBegan(Touch *touch, Event *unused_event) {
	//获取点击位置
	auto location = touch->getLocation();
	auto fadeOut = FadeOut::create(2);
	auto delay = DelayTime::create(1);
	auto seq = Sequence::create(delay, fadeOut, NULL);
	//比较鸡蛋位置与点击位置是否一致，若一致鸡蛋moveto篮子，鸡蛋数加一，鸡蛋消失
	auto newp = Vec2(origin.x + 55, origin.y + 362);
	bool flag = false;
	auto it = eggs.begin();
	while (it != eggs.end()) {
		flag = false;
		if ((*it) != NULL && (*it)->getPosition().getDistance(location) < 10) {
			(*it)->runAction(seq);
			(*it)->runAction(MoveTo::create(0.5, newp));
			clickeggs.push_back(*it);
			it = eggs.erase(it);
			flag = true;
		}
		if (flag == false) {
			it++;
		}
	}

	return true;
}
void GameScene::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
}
