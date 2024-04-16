class IOCommand {
public:
	std::string cmd;
	std::vector<std::string> data;

	IOCommand() {
		cmd = IOCommands::CMD_STUB;
	}

	void build(std::string n, std::vector<std::string> d) {
		cmd = n;
		data = d;
	}

	void build_single(std::string n, std::string d) {
		cmd = n;
		data.push_back(d);
	}

	std::string toString() {
		std::string build = cmd + ",";
		if (data.size() > 1) {
			for (int i = 0; i < data.size(); i++) {
				build += data.at(i);
				if (i < data.size() - 1) {
					build += ";";
				}
			}
		}
		else {
			build += data.front();
		}
		return build;
	}

};
