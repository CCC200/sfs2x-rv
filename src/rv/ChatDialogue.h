class ChatDialogue {
public:

	static intercept::types::game_value openChat() {
		__SQF(
			createDialog "RscChatUI";
		);
		return NULL;
	}

	static intercept::types::game_value parseChat() {
		//Pull chatbox input and close
		std::string chat_input;
		{
			intercept::client::invoker_lock thread_lock;
			chat_input = intercept::sqf::ctrl_text(1400);
			intercept::sqf::close_dialog(1);
		}
		//Convert incompatible characters
		std::string chat = ChatDialogue::convertFromBannedCharacters(chat_input);
		//Send command to IO
		IOCommand sendChat;
		sendChat.build_single(IOCommands::CMD_SENDCHAT, chat);
		Global::addIO(IOCommands::IO_IN, sendChat);
		return NULL;
	}

	static void EnableKeybinds() {
		__SQF(
			["SFS2X-RV", "OpenChat", "Open Chat",
			{ "" },
			{
				open_server_chat
			},
			[0x35, [false, false, false]]] call CBA_fnc_addKeybind;
		);
	}
private:

	static std::string convertFromBannedCharacters(std::string s) {
		std::string c = s;
		for (int i = 0; i < c.size(); i++) {
			if (c[i] == ',') {
				c.replace(i, 1, "_comma_");
			}
			else if (c[i] == ';') {
				c.replace(i, 1, "_scolon_");
			}
			else if (c[i] == ':') {
				c.replace(i, 1, "_colon_");
			}
		}
		return c;
	}
};