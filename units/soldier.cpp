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
#include "soldier.h"
#include "battlefield.h"
#include "param.h"
#include "utils.h"

Soldier::Soldier(const SpriteCollection *spr_coll,PLAYER player): Unit(player) {
	w=W;
	h=H;

	float f=factor(0.8);
	if (player==PLAYER_1) {
		x=0;
		dx=f*DX;
	} else {
		x=SCREEN_W-w;
		dx=-f*DX;
	}
	y=BattleField::BaseY-h;

	anim_soldier=spr_coll->get_anim_cycle_iterator("soldier_walk",0.22);
	anim_soldier_fight=spr_coll->get_anim_cycle_iterator("soldier_fight",0.22);
	bit_soldier_dead=spr_coll->get_sprite("soldier_unit_cadaver");

	state=WALK;
	
	this->name="soldier";
	this->life=Life;
	this->collide=true;
}

Soldier::~Soldier() {}

void Soldier::post_message(MessageQueue *mess_queue) {
	switch (state) {
	case STOP:
		if ((player==PLAYER_1 && x>SCREEN_W-BattleField::CastleW-w) || (player==PLAYER_2 && x<BattleField::CastleW)) mess_queue->push(Message(Message::EVENT_DAMAGE_PLAYER,this,this,NO_DELAY,Message::PRIORITY_NORMAL,PlayerDamage));
		state=WALK;
		break;
	case WALK:
		if ((player==PLAYER_1 && x>SCREEN_W-BattleField::CastleW-w) || (player==PLAYER_2 && x<BattleField::CastleW)) mess_queue->push(Message(Message::EVENT_DAMAGE_PLAYER,this,this,NO_DELAY,Message::PRIORITY_NORMAL,PlayerDamage));
		else x=x+dx;
		break;
	case FIGHT:
	case CADAVER:
		break;
	}
}

void Soldier::handle_message(const Message &mess,MessageQueue *mess_queue) {
	if (!dead) {
	switch (state) {
	case STOP:
	case WALK:
		switch (mess.event) {
		case Message::EVENT_DAMAGE_PLAYER:
			//collide=false;
			//state=CADAVER;
			//mess_queue->push(Message(Message::EVENT_DIE,this,this,60,Message::PRIORITY_LOW));
			dead=true;
			break;
		case Message::EVENT_COLLISION:
			if (mess.sender->get_player()!=this->player && mess.sender->get_player()!=PLAYER_NEUTRAL) {
				collide=true;
				state=FIGHT;
				mess_queue->push(Message(Message::EVENT_ATTACK,mess.sender,this,NO_DELAY,Message::PRIORITY_NORMAL,rand()%Damage));
				mess_queue->push(Message(Message::EVENT_ENDFIGHT,this,this,5,Message::PRIORITY_NORMAL));
			}
			break;
		case Message::EVENT_ATTACK:
			life-=mess.data;
			if (life<=0) {
				collide=false;
				state=CADAVER;
				mess_queue->push(Message(Message::EVENT_DIE,this,this,30,Message::PRIORITY_LOWEST));
			}
			break;
		default:
			std::cerr<<"stop/walk state:";
			Unit::handle_message(mess,mess_queue);
		}			
		break;
	case FIGHT:
		switch (mess.event) {
		case Message::EVENT_COLLISION:
			break;
		case Message::EVENT_ENDFIGHT:
			state=STOP;
			break;
		case Message::EVENT_ATTACK:
			life-=mess.data;
			if (life<=0) {
				collide=false;
				state=CADAVER;
				mess_queue->push(Message(Message::EVENT_DIE,this,this,30,Message::PRIORITY_LOWEST));
			}
			break;
		default:
			std::cerr<<"fight state:";
			Unit::handle_message(mess,mess_queue);
		}
		break;		
	case CADAVER:
		switch (mess.event) {
		case Message::EVENT_COLLISION:
		case Message::EVENT_ENDFIGHT:
		case Message::EVENT_ATTACK:
			break;
		case Message::EVENT_DIE:
			dead=true;
			break;
		default:
			std::cerr<<"cadaver state:";
			Unit::handle_message(mess,mess_queue);
		}
	}
	}
}

void Soldier::draw() {
	switch (state) {
	case STOP:
	case WALK:
		if (player==PLAYER_1) anim_soldier.get_next_bitmap()->draw(x,y);
		else anim_soldier.get_next_bitmap()->draw_flip_h(x,y);
		break;
	case FIGHT:
		if (player==PLAYER_1) anim_soldier_fight.get_next_bitmap()->draw(x-8,y-11);
		else anim_soldier_fight.get_next_bitmap()->draw_flip_h(x-8,y-11);
		break;	
	case CADAVER:
		bit_soldier_dead->draw(x-4,BattleField::BaseY-15);
		break;
	}
	
}
