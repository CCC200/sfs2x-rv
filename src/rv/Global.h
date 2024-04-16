class Global {
public:
	inline static bool Terminate = false;
	inline static intercept::types::game_value script_handle;
	inline static std::vector<IOCommand> inIO; //For sending IN
	inline static std::vector<IOCommand> outIO; //For executing OUT
	inline static Chat chat;
	inline static std::vector<Entity> units;
	inline static std::vector<Vehicle> vehicles;

	Global() {}

	static void addIO(bool dir, IOCommand io) {
		if (dir) {
			Global::inIO.push_back(io);
		}
		else {
			Global::outIO.push_back(io);
		}
	}

	//Units
	static void addUnit(std::vector<std::string> data) {
		Entity new_unit;
		new_unit.create(data);
		Global::units.push_back(new_unit);
	}

	static bool removeUnit(std::string id) {
		int u_pos = getEntityPos(id, EntityTypes::TYPE_GLOBAL_UNIT);
		if (u_pos > -1) {
			Global::units.at(u_pos).despawn();
			Global::units.erase(Global::units.begin()+u_pos);
			return true;
		}
		return false;
	}

	static bool updateUnit(std::vector<std::string> data) {
		int u_pos = getEntityPos(data.front(), EntityTypes::TYPE_GLOBAL_UNIT);
		if (u_pos > -1) {
			Global::units.at(u_pos).update(data);
			return true;
		}
		return false;
	}

	//Vehicles
	static void addVehicle(std::vector<std::string> data) {
		Vehicle new_veh;
		new_veh.create(data);
		Global::vehicles.push_back(new_veh);
	}

	static bool removeVehicle(std::string id) {
		int v_pos = getEntityPos(id, EntityTypes::TYPE_VEHICLE);
		if (v_pos > -1) {
			Global::vehicles.at(v_pos).despawn();
			Global::vehicles.erase(Global::vehicles.begin()+v_pos);
			return true;
		}
		return false;
	}

private:

	static int getEntityPos(std::string id, int type) {
		if (type == EntityTypes::TYPE_GLOBAL_UNIT) { //AI or player
			for (int i = 0; i < Global::units.size(); i++) {
				if (Global::units.at(i).name == id) {
					return i;
				}
			}
		}
		else if (type == EntityTypes::TYPE_VEHICLE) {
			for (int i = 0; i < Global::vehicles.size(); i++) {
				if (Global::vehicles.at(i).name == id) {
					return i;
				}
			}
		}
		return -1;
	}

};
