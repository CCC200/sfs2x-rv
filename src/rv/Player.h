class Player {
public:
	Player() {}

	inline static intercept::types::object obj;
	inline static intercept::types::vector3 pos;
	inline static intercept::types::vector3 dir;
	inline static intercept::types::vector3 up;
	inline static intercept::types::vector3 vel;
	inline static std::string anim;

	inline static bool isPlayerSpawned = false;
	
	static bool getInitialPos() {
		//Get data
		obj = intercept::sqf::player();
		pos = intercept::sqf::get_pos_atl(obj);
		//Add to IO
		IOCommand spawn;
		spawn.build_single(IOCommands::CMD_SPAWNPOS, flatten(pos));
		Global::addIO(IOCommands::IO_IN, spawn);
		return true;
	}

	static void updatePlayerPos() {
		intercept::types::vector3 update_pos;
		intercept::types::vector3 update_dir;
		intercept::types::vector3 update_up;
		intercept::types::vector3 update_vel;
		intercept::types::game_value update_anim_value;
		std::string update_anim;
		//Get data
		{
			intercept::client::invoker_lock thread_lock;
			__SQF(
				_anim = configName(configFile >> getText(configFile >> "CfgVehicles" >> typeOf _this >> "moves") >> "States" >> (animationState _this));
				_anim;
			).capture(obj, update_anim_value);
			update_anim = (intercept::types::sqf_return_string) update_anim_value;
			update_pos = intercept::sqf::get_pos_atl(obj);
			update_dir = intercept::sqf::vector_dir(obj);
			update_up = intercept::sqf::vector_up(obj);
			update_vel = intercept::sqf::velocity(obj);
		}
		//Check for changes
		bool do_update = false;
		if (anim != update_anim) {
			anim = update_anim;
			do_update = true;
		}
		if (compare(pos, update_pos)) {
			pos = update_pos;
			vel = update_vel; //Velocity can't move without position
			do_update = true;
		}
		if (compare(dir, update_dir) || compare(up, update_up)) {
			dir = update_dir;
			up = update_up;
			do_update = true;
		}
		//Add to IO
		if (do_update) {
			std::vector<std::string> data {flatten(pos), flatten(dir), flatten(up), flatten(vel), anim};
			IOCommand update;
			update.build(IOCommands::CMD_SENDPOS, data);
			Global::addIO(IOCommands::IO_IN, update);
		}
	}

private:

	static bool compare(intercept::types::vector3 current, intercept::types::vector3 update) {
		if (update.x != current.x || update.y != current.y || update.z != current.z) {
			return true;
		}
		return false;
	}

	static std::string flatten(intercept::types::vector3 v) {
		return std::to_string(v.x) + ":" + std::to_string(v.y) + ":" + std::to_string(v.z);
	}
};