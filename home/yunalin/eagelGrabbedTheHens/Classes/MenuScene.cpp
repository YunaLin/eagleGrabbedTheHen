#include "MenuScene.h"
#include "GameScene.h"
#include "GameOver.h"
#include "Global.h"
//-----------------------------------fang1
#include "cocostudio/CocoStudio.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

#include <regex>
using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;
USING_NS_CC;
using namespace cocostudio::timeline;
#define database UserDefault::getInstance()
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace  rapidjson;

#pragma execution_character_set("utf-8")
//----------------------------------fang2

Scene* MenuScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MenuScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{

    if ( !Layer::init() )
    {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	visibleHeight = visibleSize.height;
	visibleWidth = visibleSize.width;

	auto entry = Sprite::create("entry.jpg");
	entry->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(entry, 0);

	auto farmer = Sprite::create("begin.png");
	farmer->setPosition(Vec2(origin.x + visibleSize.width/2 ,origin.y + 100));
	this->addChild(farmer, 1);

/*	auto startImage = Sprite::create("startImage.png");
	startImage->setPosition(Vec2(visibleSize.width*8/9 , visibleSize.height*4/5));
	this->addChild(startImage, 1);

	auto start = MenuItemImage::create("start-0.png", "start-1.png", CC_CALLBACK_1(MenuScene::jumpToGame, this));
	start->setPosition(Vec2(visibleSize.width * 8 / 9, visibleSize.height * 4/5 + 75));
	auto menu = Menu::create(start, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);
	*/

	//------------------------------------------fang1
	auto UserName = Label::createWithTTF("UserName:", "fonts/arial.ttf", 50);
	UserName->setPosition(Vec2(760 + origin.x, visibleHeight / 2 + origin.y + 30));
	UserName->setColor(Color3B(218, 165, 32));
	this->addChild(UserName, 1);

	textField = TextField::create("Player Name", "Arial", 50);
	textField->setColor(Color3B(0, 0, 0));
	//textField->setTextColor(Color3B(218, 165, 32));
	textField->setPosition(Size(760 + origin.x, visibleHeight / 2 + origin.y - 50));
	this->addChild(textField, 2);

	result = TextField::create("", "Arial", 30);
	result->setPosition(Size(760 + origin.x, visibleHeight / 2 + origin.y - 50));
	this->addChild(result, 2);

	if (database->getStringForKey("username") != "") {
		textField->setString(database->getStringForKey("username"));
	}

	auto LoginItem = MenuItemImage::create(
		"login-0.png",
		"login-1.png",
		CC_CALLBACK_1(MenuScene::LoginButtonCallback, this));

	LoginItem->setPosition(Vec2(760 + origin.x, visibleHeight / 2 + origin.y - 150));
	auto menu = Menu::create(LoginItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);
	//----------------------------------------fang2
	return true;
}
/*
void MenuScene::jumpToGame(Ref* sender)
{
	Director::getInstance()->replaceScene(TransitionFade::create(1, GameScene::createScene()));
}
*/
//--------------------------------------------fang1
void MenuScene::LoginButtonCallback(Ref* sender) {
	if (textField->getString() != "") {
		HttpRequest* req = new HttpRequest();
		req->setRequestType(HttpRequest::Type::POST);
		req->setUrl("http://localhost:8080/login");

		string playername = "username=" + textField->getString();
		req->setRequestData(playername.c_str(), playername.size());
		req->setResponseCallback(CC_CALLBACK_2(MenuScene::LoginCompleted, this));
		HttpClient::getInstance()->send(req);
		database->setStringForKey("username", textField->getString());
		req->release();
	}
	//
}



void MenuScene::LoginCompleted(HttpClient* sender, HttpResponse* response) {
	if (response == nullptr) return;
	if (response->isSucceed()) {
		std::vector<char> *resHeader = response->getResponseHeader();
		std::vector<char> *resBody = response->getResponseData();
		string result1 = "";
		auto it = resHeader->begin();
		string ResHeader = "";
		for (it = resHeader->begin(); it != resHeader->end(); it++) {
			ResHeader += (*it);
		}
		string ResBody = "";
		for (it = resBody->begin(); it != resBody->end(); it++) {
			ResBody += (*it);
		}
		result->setString(result1);
		Global::gameSessionId = Global::getSessionIdFromHeader(ResHeader);
		Director::getInstance()->replaceScene(TransitionFade::create(2.0f, GameScene::createScene()));
	}
}
//-------------------------------------------fang2