class Entity {
public:

	int type;
	std::string name;
	intercept::types::object obj;
	intercept::types::vector3 pos;
	intercept::types::vector3 dir;
	intercept::types::vector3 up;
	intercept::types::vector3 vel;
	std::string anim;
	std::string stance = "AUTO";

	Entity() {
		type = EntityTypes::TYPE_NULL;
		name = "NULL";
	}

	void despawn() {
		intercept::sqf::delete_vehicle(obj);
	}

	void create(std::vector<std::string> data) {
		type = std::stoi(data.at(0));
		name = data.at(1);
		pos = unflatten(data.at(2));
		obj = spawn();
		//Disable collision with player
		auto player = intercept::sqf::player();
		intercept::sqf::disable_collision_with(player, obj);
		intercept::sqf::disable_collision_with(obj, player);
		//Control AI behavior
		//ANIM, LIGHTS, MOVE, PATH, TEAMSWITCH, WEAPONAIM still enabled
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::AIMINGERROR);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::AUTOCOMBAT);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::AUTOTARGET);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::CHECKVISIBLE);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::COVER);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::MINEDETECTION);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::NVG);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::RADIOPROTOCOL);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::SUPPRESSION);
		intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::TARGET);
		intercept::sqf::allow_damage(obj, false);
		intercept::sqf::do_stop(obj);
		intercept::sqf::switch_move(obj, "");
	}

	void update(std::vector<std::string> data) {
		if (type == EntityTypes::TYPE_PLAYER) {
			update_player(data);
		}
		else if (type == EntityTypes::TYPE_AI) {
			update_ai(data);
		}
	}

private:

	intercept::types::object spawn() {
		return intercept::sqf::create_unit(EntityTypes::GROUP_UNITS, "C_man_polo_1_F", pos);
	}

	bool compare(intercept::types::vector3 current, intercept::types::vector3 update) {
		if (update.x != current.x || update.y != current.y || update.z != current.z) {
			return true;
		}
		return false;
	}

	intercept::types::vector3 unflatten(std::string in) {
		std::vector<std::string> build_string = IOCommands::parseData(in, ":");
		intercept::types::vector3 build_vec;
		build_vec.x = std::stof(build_string.at(0));
		build_vec.y = std::stof(build_string.at(1));
		build_vec.z = std::stof(build_string.at(2));
		return build_vec;
	}

	float magnitude(intercept::types::vector3 v1, intercept::types::vector3 v2) {
		float m = sqrt( (float) pow(v2.x-v1.x, 2) + (float) pow(v2.y-v1.y, 2) + (float) pow(v2.z-v1.z, 2) );
		return m;
	}

	void update_player(std::vector<std::string> data) {
		//Pull data from command
		intercept::types::vector3 u_pos = unflatten(data.at(1));
		intercept::types::vector3 u_dir = unflatten(data.at(2));
		intercept::types::vector3 u_up = unflatten(data.at(3));
		intercept::types::vector3 u_vel = unflatten(data.at(4));
		std::string u_anim = data.at(5);
		//Check real position for rubberbanding
		auto real_pos = intercept::sqf::get_pos_atl(obj);
		float diff = magnitude(real_pos, pos);
		//Compare and execute
		if (compare(pos, u_pos)) { //Entity has moved
			pos = u_pos;
			dir = u_dir;
			up = u_up;
			vel = u_vel;
			anim = u_anim;
			//Set unit in motion
			intercept::sqf::set_vector_dir_and_up(obj, dir, up);
			intercept::sqf::set_velocity(obj, vel);
			if (diff >= 3) { //Desync, rubberband unit
				intercept::sqf::set_pos_atl(obj, pos);
			}
			intercept::sqf::enable_ai(obj, intercept::sqf::ai_behaviour_types::MOVE);
			intercept::sqf::move_to(obj, pos);
			//Wait for stillness and disable movement
			__SQF(
				_move_end = _this spawn
				{
					waitUntil { moveToCompleted _this; };
					if (speed _this == 0) then { _this disableAI "MOVE"; };
				};
				_move_end;
			).capture(obj);
			//Play animation
			intercept::sqf::play_move_now(obj, anim);
			//Set stance
			if (anim == EntityTypes::ANIM_PRONE) {
				stance = "DOWN";
				intercept::sqf::set_unit_pos(obj, stance);
			}
			else if(stance == "DOWN") {
				stance = "AUTO";
				intercept::sqf::set_unit_pos(obj, stance);
			}
		}
		else if (compare(dir, u_dir) || compare(up, u_up)) { //Entity has turned but not moved
			pos = u_pos;
			dir = u_dir;
			up = u_up;
			anim = u_anim;
			if (diff >= 1) { //Desync, rubberband unit
				intercept::sqf::set_pos_atl(obj, pos);
			}
			intercept::sqf::disable_ai(obj, intercept::sqf::ai_behaviour_types::MOVE);
			intercept::sqf::play_move_now(obj, anim);
			intercept::sqf::set_vector_dir_and_up(obj, dir, up);
			
		}
		else if(anim != u_anim) { //No movement, just animation
			anim = u_anim;
			intercept::sqf::play_move_now(obj, anim);
		}
	}

	void update_ai(std::vector<std::string> data) {
		//Pull data
		intercept::types::vector3 u_pos = unflatten(data.at(1));
		//Compare and execute
		if (compare(pos, u_pos)) {
			pos = u_pos;
			intercept::sqf::move_to(obj, pos);
		}
	}

};
