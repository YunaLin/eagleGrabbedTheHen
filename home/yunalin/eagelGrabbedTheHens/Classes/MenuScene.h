#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "ui/CocosGUI.h"
#include "GameScene.h"
#include "network/HttpClient.h"
#include <string>
#include <vector>
using std::vector;
using std::string;
using namespace cocos2d::ui;
using namespace cocos2d::network;

#include "cocos2d.h"

class MenuScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
//	void jumpToGame(Ref* sender);

    CREATE_FUNC(MenuScene);
	//-----------------------------------------------fang
	void LoginButtonCallback(Ref* sender);
	void LoginCompleted(HttpClient* sender, HttpResponse* response);
	//-----------------------------------------------fang
private:
	float visibleHeight;
	float visibleWidth;
	TextField * textField;
	TextField* result;
};

#endif // __MENU_SEBCE_H__
