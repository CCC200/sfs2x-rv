class Error {
public:

	Error() {}

	static void parseError(IOCommand e) {
		//Variables
		std::string reason = e.data.front();
		bool fatal;
		fatal = (e.data.back() == "true");
		//Execute
		execError(reason, fatal);
	}
	static void execError(std::string r, bool f) {
		//Fatal error, terminate session
		if (f) {
			intercept::sqf::hint_c("FATAL ERROR: " + r);
			intercept::sqf::end_mission("LOSER");
			Global::Terminate = true;
		}
		//Non-fatal, continue
		else {
			intercept::sqf::hint("ERROR: " + r);
		}
	}
};
