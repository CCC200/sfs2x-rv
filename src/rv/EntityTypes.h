class EntityTypes {
public:
	EntityTypes() {}

	inline static const int TYPE_NULL = 0;
	inline static const int TYPE_PLAYER = 1;
	inline static const int TYPE_AI = 2;
	inline static const int TYPE_VEHICLE = 3;
	inline static const int TYPE_GLOBAL_UNIT = 4;

	inline static intercept::types::group GROUP_UNITS;

	inline static const std::string ANIM_PRONE = "AmovPpneMstpSnonWnonDnon";

};
