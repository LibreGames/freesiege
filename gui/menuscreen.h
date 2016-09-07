//	This file is part of freesiege program <http://freesiege.sourceforge.net>.
//	Copyright (C) 2007 Pierre Gueth <pierregueth@users.sourceforge.net>
//	                   Joel Schaerer <joelschaerer@users.sourceforge.net>
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef __MENUSCREEN_H
#define __MENUSCREEN_H

#include "spritecollection.h"
#include "menu.h"

class MenuScreen {
public:
	enum SELECTION {QUIT,TWO_PLAYERS,TRAINING,COMBINAISONS,EXHIBITION};
	MenuScreen(const SpriteCollection *spr_coll,const std::string &ttf_path,SDL_Renderer *sdlRenderer);
	~MenuScreen();
	bool display_menu(SDL_Renderer *sdlRenderer,SDL_Window *sdlWindow,SELECTION &selection);

    //Options
    enum AILEVEL {EASY,NORMAL,HARD,EXTREME};
    AILEVEL ai_level;
private:
	Sprite* load_message(SDL_Renderer* sdlRenderer,std::string msg);
	void refresh_key_menu(SDL_Renderer *sdlRenderer);
	bool handle_action(SDL_Renderer *sdlRenderer, SDL_Window *sdlWindow, Menu *current_menu,SELECTION &selection, bool &waiting_key, Sprite* &msgSprite);

	const SpriteCollection *spr_coll;
	std::string ttf_path;
	Menu *key_menu;
	Menu *main_menu;
	Menu *option_menu;
	typedef std::stack<Menu*> Menus;
	Menus menus;
	TextureIds ids;

	TTF_Font *normal_font;
	TTF_Font *selected_font;
};

#endif
