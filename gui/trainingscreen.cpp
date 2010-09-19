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
#include <cmath>
#include "trainingscreen.h"

#include "board.h"
#include "boardsurvivor.h"
#include "utils.h"
#include "param.h"
#include "plant.h"
#include "options.h"

#define FONT_COLOR { 0x77, 0xd1, 0x00, 0 }
#define SURVIVAL_TIME 3000

TrainingScreen::TrainingScreen(const SpriteCollection *spr_coll,const CombinaisonCollection *cmb_coll,const std::string &ttf_path,TextureIds ids,Background *background, MusicCollection *music_coll) {
    base_speed=150;
	font=TTF_OpenFont(ttf_path.c_str(),80);
	font_huge=TTF_OpenFont(ttf_path.c_str(),120);
	font_tiny=TTF_OpenFont(ttf_path.c_str(),40);
	if (!font || !font_huge || !font_tiny) {
		std::cerr<<"font "<<ttf_path<<" creation failed..."<<std::endl;
		return;
	}

	SDL_Color color=FONT_COLOR;
	SDL_Surface *text_key_help_surf=TTF_RenderText_Solid(font_tiny,"Space to continue ... Esc to quit", color);
	text_key_help=new Sprite(text_key_help_surf,ids[2]);
	skull=spr_coll->get_anim_cycle_iterator("skull",0.1);
	hand=spr_coll->get_anim_cycle_iterator("hand",0.1);
	SDL_FreeSurface(text_key_help_surf);

	score_id=ids[3];
	perfect_id=ids[4];
	go_id=ids[5];
	ko_id=ids[6];

	this->spr_coll=spr_coll;
	this->cmb_coll=cmb_coll;
	this->music_coll=music_coll;
	this->background=background;
}

void TrainingScreen::set_ai_level(MenuScreen::AILEVEL ai_level)
{
    switch(ai_level)
      {
        case 0:
            base_speed=500;
            break;
        case 1:
            base_speed=150;
            break;
        case 2:
            base_speed=90;
            break;
        case 3:
            base_speed=60;
            break;
      }
}

TrainingScreen::~TrainingScreen() {
	delete text_key_help;
	
 	if(TTF_WasInit()) {
 		if(font)
 			TTF_CloseFont(font);
 			font = NULL;
 		if(font_huge)
 			TTF_CloseFont(font_huge);
 			font_huge = NULL;
 		if(font_tiny)
 			TTF_CloseFont(font_tiny);
 			font_tiny = NULL;
 	}
}

void TrainingScreen::display_game(SDL_Surface *screen) {
	int p1_win=0;
	int p2_win=0;

	bool quit_game=false;
	bool paused=false;
	SDL_Event event;
	SDL_Color color=FONT_COLOR;
	Uint32 ticks=SDL_GetTicks();
		
	SDL_Surface *pause_surf=TTF_RenderText_Solid(font,"GAME PAUSED",color);
	Sprite pause_sprite(pause_surf,pause_id);
	SDL_FreeSurface(pause_surf);

	while (!quit_game) {
		//game object init
        int play_ticks=1;
		bool quit=false;
		PLAYER winner=PLAYER_NEUTRAL;
		LifeBar life_bar1(spr_coll,PLAYER_1);
		LifeBar life_bar2(spr_coll,PLAYER_2);
		Foreground foreground(spr_coll);
		BattleField battlefield(spr_coll,&life_bar1,&life_bar2,&foreground);
		Board board1(spr_coll,cmb_coll,&battlefield,PLAYER_1);
		BoardSurvivor board2(spr_coll,cmb_coll,&battlefield,PLAYER_2,base_speed);
		
		//main loop
		while (!quit && !quit_game) {
			//draw
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			background->draw();
			life_bar1.draw();
			life_bar2.draw();
			if(!paused)
				battlefield.refresh();
			battlefield.draw();
			board1.draw();
			board2.draw();
			foreground.draw();
			if(paused) {
				fill_rect_opengl(0,0,SCREEN_W,SCREEN_H,0,0,0,0.7);
				pause_sprite.draw((SCREEN_W-pause_sprite.w)/2,(SCREEN_H-pause_sprite.h)/2);
			}
			SDL_GL_SwapBuffers();	
			SDL_Flip(screen);
			
			if(!paused) {
				//logic
				board1.logic(foreground.p1_flower);
				board2.logic(foreground.p2_flower);
			}
			
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym==SDLK_ESCAPE) quit_game=true;
#ifdef DEBUG_MODE
					else if (event.key.keysym.sym==SDLK_t) life_bar2.damage(100); //DEBUG
					else if (event.key.keysym.sym==SDLK_y) life_bar1.damage(100);
#endif
					else if (event.key.keysym.sym==Options::pause_key) paused=!paused;
					break;
				case SDL_QUIT:			
					quit_game=true;
					break;
				default:
					break;
				}
			}

            if( !paused && (play_ticks > SURVIVAL_TIME) && (battlefield.get_nonplant_unit_count(PLAYER_2) == 0) ) {
				winner=PLAYER_1;
				quit=true;
			}

			if (life_bar1.get_life()<=0) {
				winner=PLAYER_2;
				quit=true;
			} else if (life_bar2.get_life()<=0) {
				winner=PLAYER_1;
				quit=true;
			}

			while (ticks>(SDL_GetTicks()-1000/FPS)) SDL_Delay(3);
			ticks=SDL_GetTicks();
			if(!paused)
				play_ticks++;
            
			if(!Mix_PlayingMusic()) {
				music_coll->play_random_music();
			}
		}

		//final screen
		switch (winner) {
		case PLAYER_1:
			p1_win++;
            board2.hasWin();
			break;
		case PLAYER_2:
			p2_win++;
			break;
		default:
			break;
		}
		
		//render score
		SDL_Surface *score_surf=TTF_RenderText_Solid(font,("Level " + number_as_roman(level)+" cleared!!!").c_str(),color);
		Sprite score_sprite(score_surf,score_id);
		SDL_FreeSurface(score_surf);
		SDL_Surface *go_surf=TTF_RenderText_Solid(font,"GAME OVER!",color);
		Sprite go_sprite(go_surf,go_id);
		SDL_FreeSurface(go_surf);
		SDL_Surface *perfect_surf=TTF_RenderText_Solid(font,"PERFECT!",color);
		Sprite perfect_sprite(perfect_surf,perfect_id);
		SDL_FreeSurface(perfect_surf);
		SDL_Surface *ko_surf=TTF_RenderText_Solid(font,"KO!",color);
		Sprite ko_sprite(ko_surf,ko_id);
		SDL_FreeSurface(ko_surf);

		quit=false;
        bool ko=(life_bar2.get_life() == 0);
        bool perfect=(life_bar1.get_life() == LIFE_FACTOR);
		while (!quit && !quit_game) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//background
			background->draw();
			life_bar1.draw();
			life_bar2.draw();
			battlefield.refresh();
			battlefield.draw();
			board1.draw();
			foreground.draw();
			
			//overlay
			fill_rect_opengl(0,0,SCREEN_W,SCREEN_H,0,0,0,0.7);
			text_key_help->draw((SCREEN_W-text_key_help->w)/2,SCREEN_H-50-text_key_help->h);

			const Sprite* current_skull=skull.get_next_bitmap();
			const Sprite* current_hand=hand.get_next_bitmap();
			if (winner==PLAYER_2) {
                go_sprite.draw((SCREEN_W-go_sprite.w)/2,50);
				current_skull->draw(SCREEN_W/2-current_skull->w/2,(SCREEN_H-current_skull->h)/2);
			} else {
                if (perfect)
                  {
                    if (ko) {
                        current_hand->draw(SCREEN_W/2-current_hand->w/2-160,(SCREEN_H-current_hand->h)/2);
                        current_hand->draw(SCREEN_W/2-current_hand->w/2,(SCREEN_H-current_hand->h)/2);
                        current_hand->draw(SCREEN_W/2-current_hand->w/2+160,(SCREEN_H-current_hand->h)/2);
                        ko_sprite.draw((SCREEN_W-ko_sprite.w)/2,60+score_sprite.h+perfect_sprite.h);
                    }
                    else
                      {
                        current_hand->draw(SCREEN_W/2-current_hand->w-perfect_sprite.w/2,(SCREEN_H-current_hand->h)/2);
                        current_hand->draw(SCREEN_W/2+perfect_sprite.w/2,(SCREEN_H-current_hand->h)/2);
                      }
                    perfect_sprite.draw((SCREEN_W-perfect_sprite.w)/2,60+score_sprite.h);
                  }
                else if (ko)
                  {
                    current_hand->draw(SCREEN_W/2-current_hand->w-ko_sprite.w/2,(SCREEN_H-current_hand->h)/2);
                    current_hand->draw(SCREEN_W/2+ko_sprite.w/2,(SCREEN_H-current_hand->h)/2);
                    ko_sprite.draw((SCREEN_W-ko_sprite.w)/2,60+score_sprite.h);
                  }
                else
                    current_hand->draw(SCREEN_W/2-current_hand->w/2,(SCREEN_H-current_hand->h)/2);
                score_sprite.draw((SCREEN_W-score_sprite.w)/2,50);
			}

			SDL_GL_SwapBuffers();
			SDL_Flip(screen);

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym==SDLK_ESCAPE) quit_game=true; //quit game
					if (event.key.keysym.sym==SDLK_SPACE) quit=true;
					break;
				case SDL_QUIT:			
					quit_game=true;
					break;
				default:
					break;
				}
			}

			SDL_Delay(10);
			while (ticks>(SDL_GetTicks()-1000/FPS)) SDL_Delay(3);
			ticks=SDL_GetTicks();
		}
        if (winner==PLAYER_2) quit_game=true;
	}
}
