class Chat {
public:

	Chat() {}

	void createChat(std::string type) {
		intercept::types::game_value chat_capture;
		//This seems to cause tons of warnings if I do it through intercept functions
		__SQF(
			_logicCenter = createCenter sideLogic;
			_logicGroup = createGroup _logicCenter;
			_pos = getPos player;
			_logic = _logicGroup createUnit["Logic", _pos, [], 0, "NONE"];
			_chat = radioChannelCreate [[211, 213, 0, 0.8], "Server Chat " + _this, _this, [player, _logic], true];
			_array = [_logic, _chat];
			_array;
		).capture(type, chat_capture);
		_chat = chat_capture.get(0).value();
		_chat_channel = chat_capture.get(1).value();
	}

	void sendChat(std::string user, std::string message) {
		intercept::sqf::custom_chat(_chat, (uint32_t) _chat_channel, user + ": " + convertToBannedCharacters(message));
	}

private:
	intercept::types::object _chat;
	float _chat_channel = 0.0f;
	std::vector<std::string> banned_chars {"_comma_", "_colon_", "_scolon_"};
	std::vector<std::string> banned_replace {",", ":", ";"};

	std::string convertToBannedCharacters(std::string s) {
		std::string c = s;
		for (int i = 0; i < c.size(); i++) {
			for (int i2 = 0; i2 < 3; i2++) {
				size_t pos = c.find(banned_chars[i2]);
				if (pos != std::string::npos) {
					c.replace(pos, banned_chars[i2].size(), banned_replace[i2]);
				}
			}
		}
		return c;
	}

};
