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
#ifndef __BOARD_H
#define __BOARD_H

#include "combinaisoncollection.h"
#include "battlefield.h"
#include "boardabstract.h"

class Board : public BoardAbstract {
public:
	Board(const SpriteCollection *spr_coll,const CombinaisonCollection *com_coll,BattleField *field,PLAYER player);
	~Board();
	void logic(bool flowers);

protected:
	SDL_Scancode key_select;
	SDL_Scancode key_swap;
	SDL_Scancode key_validate;
	SDL_Scancode key_left;
	SDL_Scancode key_right;
	SDL_Scancode key_down;
	SDL_Scancode key_up;

};

#endif
