class Vehicle {
public:

	int type;
	std::string name;
	std::string vclass;
	intercept::types::object obj;
	intercept::types::vector3 pos;
	intercept::types::vector3 dir;
	intercept::types::vector3 up;
	intercept::types::vector3 vel;

	Vehicle() {
		type = EntityTypes::TYPE_NULL;
		name = "NULL";
	}

	void despawn() {
		intercept::sqf::delete_vehicle(obj);
	}

	void create(std::vector<std::string> data) {
		type = EntityTypes::TYPE_VEHICLE;
		name = data.at(0);
		vclass = data.at(1);
		pos = intercept::sqf::get_pos_atl(intercept::sqf::player()); //temp
		obj = spawn();
	}

private:

	intercept::types::object spawn() {
		return intercept::sqf::create_vehicle(vclass, pos);
	}

};
