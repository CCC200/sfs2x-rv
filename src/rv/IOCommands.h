class IOCommands {
public:

    //Core
    inline static const std::string CMD_STUB = "STUB";
    inline static const std::string CMD_ERROR = "ERROR";
    inline static const std::string CMD_STARTCONNECT = "STARTCONNECT";
    inline static const std::string CMD_FINISHLOGIN = "FINISHLOGIN";
    inline static const std::string CMD_JOINEDROOM = "JOINEDROOM";
    inline static const std::string CMD_FINISHSPAWN = "FINISHSPAWN";

    //Chat
    inline static const std::string CMD_SENDCHAT = "SENDCHAT";
    inline static const std::string CMD_HANDLECHAT = "HANDLECHAT";

    //Entity
    inline static const std::string CMD_SPAWNPOS = "SPAWNPOS";
    inline static const std::string CMD_SENDPOS = "SENDPOS";
    inline static const std::string CMD_SPAWNUNIT = "SPAWNUNIT";
    inline static const std::string CMD_DESPAWNUNIT = "DESPAWNUNIT";
    inline static const std::string CMD_UPDATEUSER = "UPDATEUSER";
    inline static const std::string CMD_UPDATEAI = "UPDATEAI";

    inline static const bool IO_IN = true;
    inline static const bool IO_OUT = false;

    IOCommands() {}

    static std::vector<std::string> parseData(std::string data, std::string delim = ";") {
        std::string s = data;
        size_t pos = 0;
        std::string token;
        std::vector<std::string> out;
        while ((pos = s.find(delim)) != std::string::npos) {
            token = s.substr(0, pos);
            out.push_back(token);
            s.erase(0, pos + delim.length());
        }
        out.push_back(s);
        return out;
    }

};
