#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <intercept.hpp>
#include <shlobj_core.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <math.h>
#include "IOCommands.h"
#include "IOCommand.h"
#include "Chat.h"
#include "EntityTypes.h"
#include "Entity.h"
#include "Vehicle.h"
#include "Global.h"
#include "ChatDialogue.h"
#include "Error.h"
#include "Player.h"

using namespace intercept;

//Client execution
void ExecuteOut(IOCommand IO) {
    //Execute function
    if (IO.cmd == IOCommands::CMD_STUB) {
        sqf::system_chat(IO.cmd + " -> " + IO.data.front());
    }
    else if (IO.cmd == IOCommands::CMD_HANDLECHAT) {
        Global::chat.sendChat(IO.data.front(), IO.data.back());
    }
    else if (IO.cmd == IOCommands::CMD_ERROR) {
        Error::parseError(IO);
    }
    else if (IO.cmd == IOCommands::CMD_FINISHLOGIN) {
        sqf::system_chat("Login successful! Welcome, " + IO.data.front());
    }
    else if (IO.cmd == IOCommands::CMD_JOINEDROOM) {
        if (Player::getInitialPos()) {
            sqf::system_chat(IO.data.front() + " world joined!");
            //Create chat bindings
            ChatDialogue::EnableKeybinds();
            //Initialize global chat
            Global::chat.createChat("Global");
        }
    }
    else if (IO.cmd == IOCommands::CMD_FINISHSPAWN) {
        Player::isPlayerSpawned = true;
        sqf::enable_simulation(sqf::player(), true);
    }
    else if (IO.cmd == IOCommands::CMD_SPAWNUNIT) {
        Global::addUnit(IO.data);
    }
    else if (IO.cmd == IOCommands::CMD_DESPAWNUNIT) {
        if (Global::removeUnit(IO.data.front()) == false) {
            Error::execError("Unit " + IO.data.front() + " does not exist!", false);
        }
    }
    else if (IO.cmd == IOCommands::CMD_UPDATEUSER || IO.cmd == IOCommands::CMD_UPDATEAI) {
        if (Global::updateUnit(IO.data) == false) {
            Error::execError("Unit " + IO.data.front() + " is not spawned!", false);
        }
    }
}

//Intercept setup
int intercept::api_version() {
    return INTERCEPT_SDK_API_VERSION;
}

void intercept::register_interfaces() {}

void intercept::pre_init() {}

//SQF Functions
game_value exit_destroy_thread() {
    Global::Terminate = true;
    return NULL;
}

game_value execute_thread() {
    if(Global::Terminate == false && Global::outIO.size() > 0) {
        client::invoker_lock thread_lock;
        for (IOCommand IO : Global::outIO) {
            ExecuteOut(IO);
        }
        Global::outIO.clear();
    }
    return NULL;
}

types::registered_sqf_function _exit_destroy_thread;
types::registered_sqf_function _open_server_chat;
types::registered_sqf_function _parse_chat_input;
types::registered_sqf_function _execute_thread;

void intercept::pre_start() {
    _exit_destroy_thread = client::host::register_sqf_command(
        "exit_destroy_thread",
        "Destroys active threads",
        userFunctionWrapper<exit_destroy_thread>,
        types::GameDataType::NOTHING
    );
    _open_server_chat = client::host::register_sqf_command(
        "open_server_chat",
        "Opens server chat prompt",
        userFunctionWrapper<ChatDialogue::openChat>,
        types::GameDataType::NOTHING
    );
    _parse_chat_input = client::host::register_sqf_command(
        "parse_chat_input",
        "Parses input from chat box",
        userFunctionWrapper<ChatDialogue::parseChat>,
        types::GameDataType::NOTHING
    );
    _execute_thread = client::host::register_sqf_command(
        "execute_thread",
        "Uses execution thread",
        userFunctionWrapper<execute_thread>,
        types::GameDataType::NOTHING
    );
}

//Monitor
void MonitorIn() {
    //Define path
    std::string path_in = getenv("appdata");
    path_in += "\\ZeroIP\\SFS2X-RV_Client\\in";
    std::string path_in_file = path_in + "\\io";
    //Error var
    bool isMonitorError = false;
    std::string err_string;
    //Handler
    while (true) {
        //Parse data
        size_t global_size = Global::inIO.size();
        //Write data
        if (global_size > 0) {
            std::ofstream io_in(path_in_file);
            std::string build_string;
            for (int i = 0; i < global_size; i++) {
                build_string += Global::inIO.at(i).toString();
                if (i < global_size - 1) {
                    build_string += "\n";
                }
            }
            if (io_in.is_open()) {
                io_in.write(build_string.c_str(), build_string.size());
                io_in.close();
                Global::inIO.erase(Global::inIO.begin(), Global::inIO.begin()+global_size); //Remove from global IO
            }
            else if (io_in.badbit) {
                //File in use, skip cycle
            }
            else {
                isMonitorError = true;
                err_string = "Failed to open IO in file";
            }
        }
        if (isMonitorError) {
            Error::execError(err_string, true);
        }
        //Global exit
        if (Global::Terminate) {
            ExitThread(GetLastError());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void MonitorOut() {
    //Define path
    std::string path_out = getenv("appdata");
    path_out += "\\ZeroIP\\SFS2X-RV_Client\\out";
    std::string path_out_file = path_out + "\\io";
    //Define data
    std::string out_string;
    std::vector<std::string> out_array;
    std::vector<std::string> out_memory;
    bool doExecute;
    //Error var
    bool isMonitorError = false;
    std::string err_string;
    //Handler
    while (true) {
        //Reset data
        out_array.clear();
        doExecute = true;
        //Catch data
        std::ifstream io_out(path_out_file);
        if (io_out.is_open()) {
            while (getline(io_out, out_string)) {
                out_array.push_back(out_string);
            }
            io_out.close();
        }
        else if (io_out.badbit) {
            //File being written to, skip cycle
            doExecute = false;
        }
        else {
            if (io_out.eofbit) {
                err_string = "IO out failure: eofbit";
            }
            else if (io_out.failbit) {
                err_string = "IO out failure: failbit";
            }
            isMonitorError = true;
            doExecute = false;
        }
        //Compare to memory before parse
        if (out_memory.size() > 0 && out_memory == out_array) {
            doExecute = false;
        }
        if (doExecute) {
            //Parse
            for (int i = 0; i < out_array.size(); i++) {
                std::vector<std::string> cmd = IOCommands::parseData(out_array.at(i), ",");
                IOCommand IO;
                IO.build(cmd.front(), IOCommands::parseData(cmd.back()));
                Global::addIO(IOCommands::IO_OUT, IO);
            }
            //Save to memory
            out_memory = out_array;
        }
        if (isMonitorError) {
            Error::execError(err_string, true);
        }
        //Global exit
        if (Global::Terminate) {
            ExitThread(GetLastError());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void MonitorInput() {
    //Handler
    while (true) {
        if (Player::isPlayerSpawned) {
            Player::updatePlayerPos();
        }
        //Global exit
        if (Global::Terminate) {
            ExitThread(GetLastError());
        }
        //Wait for next loop
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

//Execution start
void intercept::post_init() {
    //Lock player movement until finished joining
    sqf::enable_simulation(sqf::player(), false);
    //Create groups
    EntityTypes::GROUP_UNITS = sqf::create_group(sqf::civilian());
    //Start monitors
    std::thread monitor_out_thread(MonitorOut);
    monitor_out_thread.detach();
    std::thread monitor_in_thread(MonitorIn);
    monitor_in_thread.detach();
    std::thread monitor_input_thread(MonitorInput);
    monitor_input_thread.detach();
    //Initialize execution loop
    types::game_value script_exec;
    __SQF(
        _exec = 0 spawn
        {
            while {true} do {
                execute_thread;
                sleep 0.05;
            };
        };
        _exec;
    ).capture(0, script_exec);
    Global::script_handle = script_exec;
    //Signal for connection
    IOCommand connectSignal;
    connectSignal.build_single(IOCommands::CMD_STARTCONNECT, sqf::profile_name());
    Global::addIO(IOCommands::IO_IN, connectSignal);
}
