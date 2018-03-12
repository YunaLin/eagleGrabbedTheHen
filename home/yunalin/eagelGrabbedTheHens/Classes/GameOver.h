#ifndef __GAMEOVER_H__
#define __GAMEOVER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "network/HttpClient.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocos2d::network;

class GameOver : public cocos2d::Layer {
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(GameOver);
	void submitCallback(Ref* sender);
	void rankCallback(Ref * sender);
	void logoutCallback(Ref * sender);
	void continueCallback(Ref * pSender);
	void onsubmitCompleted(HttpClient * sender, HttpResponse * res);
	void rankCompleted(HttpClient * sender, HttpResponse * response);
	Label* score;
	TextField* topx;
	Label* rank_field;
};

#endif // __THUNDER_H__
