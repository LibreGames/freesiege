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
#include "combinaisonscreen.h"
#include "board.h"
#include "param.h"

#define COMBI_NAME_NORMAL_H 36
#define COMBI_NAME_SELECTED_H 50
#define COMBI_NAME_COLOR {0,1,0,0}
#define COMBI_SPACING 40
#define COMBI_MAIN_Y 20
#define COMBI_MAIN_X 300
#define COMBI_COMBI_Y 200
#define COMBI_COMBI_X 300
#define COMBI_COMBI_DELAY 40

#define COMBI_RIGHT_X 300
#define COMBI_BOTTOM_Y 300

CombinaisonScreen::CombinaisonScreen(const SpriteCollection *spr_coll,const CombinaisonCollection *cmb_coll,const std::string &ttf_path,TextureIds ids) {
	this->spr_coll=spr_coll;
	
	//font creation
	TTF_Font *normal_font=TTF_OpenFont(ttf_path.c_str(),COMBI_NAME_NORMAL_H);
	TTF_Font *selected_font=TTF_OpenFont(ttf_path.c_str(),COMBI_NAME_SELECTED_H);
	if (!normal_font || !selected_font) {
		std::cerr<<"font "<<ttf_path<<" creation failed..."<<std::endl;
		return;
	}
	
	//initialize combinaisons sprites
	SDL_Color name_color=COMBI_NAME_COLOR;
	for (CombinaisonCollection::Names::const_iterator name=cmb_coll->names.begin(); name!=cmb_coll->names.end(); name++) {
		CombinaisonSprite *combinaison_sprite=new CombinaisonSprite;

		SDL_Surface *name_normal_surf=TTF_RenderText_Solid(normal_font,(*name)->c_str(),name_color);
		SDL_Surface *name_selected_surf=TTF_RenderText_Solid(selected_font,(*name)->c_str(),name_color);
		combinaison_sprite->name_normal_sprite=new Sprite(name_normal_surf,*ids++);
		combinaison_sprite->name_selected_sprite=new Sprite(name_selected_surf,*ids++);
		SDL_FreeSurface(name_normal_surf);
		SDL_FreeSurface(name_selected_surf);

		combinaison_sprite->game_sprite=spr_coll->get_sprite(**name);

		combinaison_sprites[*name]=combinaison_sprite;
	}

	//push corresponding combinaisons
	for (CombinaisonCollection::Coll::const_iterator combi=cmb_coll->coll.begin(); combi!=cmb_coll->coll.end(); combi++)
	   combinaison_sprites[combi->second]->combinaisons.push_back(combi->first);	

	for (CombinaisonSprites::const_iterator sprite=combinaison_sprites.begin(); sprite!=combinaison_sprites.end(); sprite++)
		sprite->second->current_combinaison=sprite->second->combinaisons.begin();

	//delete font
	TTF_CloseFont(normal_font);
	TTF_CloseFont(selected_font);

	//element sprite
	fire=spr_coll->get_sprite("fire");
	wood=spr_coll->get_sprite("wood");
	steel=spr_coll->get_sprite("steel");
	magic=spr_coll->get_sprite("magic");
	any=spr_coll->get_sprite("any");
}

CombinaisonScreen::~CombinaisonScreen() {
	for (CombinaisonSprites::iterator iter=combinaison_sprites.begin(); iter!=combinaison_sprites.end(); iter++) {
		delete iter->second->name_normal_sprite;
		delete iter->second->name_selected_sprite;
		delete iter->second;
	}
}

void CombinaisonScreen::display_combinaisons(SDL_Surface *screen) {
	Uint32 ticks=SDL_GetTicks();
	CombinaisonSprites::const_iterator current=combinaison_sprites.begin();
	
	const Sprite *back=spr_coll->get_sprite("back_patterns");
	const Sprite *back_selected=spr_coll->get_sprite("back_pattern_selected");

	SDL_Event event;
	int frame_count=0;
	while (true) {

		//background
		fill_rect_opengl(0,0,SCREEN_W,SCREEN_H,1,1,0,1);
		back->draw(0,0);
		
		//menu
		int y=(SCREEN_H - combinaison_sprites.size()*COMBI_SPACING)/2, y_current;
		for (CombinaisonSprites::const_iterator iter=combinaison_sprites.begin(); iter!=combinaison_sprites.end(); iter++) {
			if (iter!=current) iter->second->name_normal_sprite->draw((COMBI_RIGHT_X-iter->second->name_normal_sprite->w)/2,y);
			else {
				y_current=y;
			}
			y+=COMBI_SPACING;
		}
		back_selected->draw((COMBI_RIGHT_X-back_selected->w)/2,y_current-(back_selected->h-COMBI_NAME_NORMAL_H)/2);
		current->second->name_selected_sprite->draw((COMBI_RIGHT_X-current->second->name_selected_sprite->w)/2,y_current-(COMBI_NAME_SELECTED_H-COMBI_NAME_NORMAL_H)/2);


		//main game sprite
		current->second->game_sprite->draw((COMBI_RIGHT_X+(SCREEN_W-COMBI_RIGHT_X-current->second->game_sprite->w)/2),(COMBI_BOTTOM_Y-current->second->game_sprite->h)/2);

		//combinaison
		const Combinaison *current_combinaison=*current->second->current_combinaison;
		std::string::const_iterator element=static_cast<const std::string*>(current_combinaison)->begin();
		const Sprite *current_element;
		int x_el = COMBI_RIGHT_X + (SCREEN_W - COMBI_RIGHT_X - (BOARD_W-1)*(ELEM_W+SPACING) - ELEM_W)/2;
		int y_el = COMBI_BOTTOM_Y + (SCREEN_H - COMBI_BOTTOM_Y - 2*ELEM_H - SPACING)/2;
		for (int i=0; i<2; i++) {
			for (int j=0; j<BOARD_W; j++) {
				switch (*element++) {
				case WOOD:
					current_element=wood;
					break;
				case STEEL:
					current_element=steel;
					break;
				case MAGIC:
					current_element=magic;
					break;
				case FIRE:
					current_element=fire;
					break;
				default:
					current_element=any;
					break;
				}
				current_element->draw(x_el+j*(ELEM_W+SPACING),y_el+i*(ELEM_H+SPACING));
			}
		}

		//swap screens
		SDL_GL_SwapBuffers();
		SDL_Flip(screen);

		//swap displayed combinaison
		if (frame_count>=COMBI_COMBI_DELAY) {
			frame_count=0;
			current->second->current_combinaison++;
			if (current->second->current_combinaison==current->second->combinaisons.end()) current->second->current_combinaison=current->second->combinaisons.begin();
		}

		//handle keys
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_SPACE:
					frame_count=0;
					current->second->current_combinaison++;
					if (current->second->current_combinaison==current->second->combinaisons.end()) current->second->current_combinaison=current->second->combinaisons.begin();
					break;
				case SDLK_UP:
					if (current==combinaison_sprites.begin()) current=combinaison_sprites.end();
					current--;
					break;
				case SDLK_DOWN:
					current++;
					if (current==combinaison_sprites.end()) current=combinaison_sprites.begin();
					break;
				case SDLK_RETURN://FIXME debug
					std::cout<<current->second->combinaisons.size()<<" "<<*current_combinaison<<std::endl;
					break;
				case SDLK_ESCAPE:
					return;
				default:
					break;
				}
				break;
			case SDL_QUIT:			
				return;
				break;
			default:
				break;
			}
		}


		while (ticks>(SDL_GetTicks()-1000/FPS)) SDL_Delay(3);
		ticks=SDL_GetTicks();
		frame_count++;
	}
}
