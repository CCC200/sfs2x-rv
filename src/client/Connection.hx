package;

import openfl.events.Event;
import openfl.events.EventDispatcher;
import com.smartfoxserver.v2.SmartFox;
import com.smartfoxserver.v2.core.SFSEvent;
import com.smartfoxserver.v2.requests.*;
import com.smartfoxserver.v2.requests.mmo.SetUserPositionRequest;
import com.smartfoxserver.v2.entities.*;
import com.smartfoxserver.v2.entities.data.*;
import com.smartfoxserver.v2.entities.variables.*;
import com.smartfoxserver.v2.util.ConfigData;
import IOCommand;
import IODataError;
import ConnectEvent;

class Connection extends EventDispatcher
{
    public static inline var POSITION_DATA:String = "pd";
    
    public var sfs:SmartFox;

    private var username:String;

    public function new()
    {
        super();
        sfs = new SmartFox();
        Vec3D.useFloatCoordinates = true;
        //Listeners
        sfs.addEventListener(SFSEvent.CONNECTION, onConnection);
        sfs.addEventListener(SFSEvent.CONNECTION_LOST, onConnectionLost);
        sfs.addEventListener(SFSEvent.PUBLIC_MESSAGE, onPublicMessage);
        sfs.addEventListener(SFSEvent.PROXIMITY_LIST_UPDATE, onProximityUpdate);
        sfs.addEventListener(SFSEvent.USER_VARIABLES_UPDATE, onUserVariablesUpdate);
    }

    public function connect(n:String):Void {
        username = n;
        passConsole("Connecting to server...");
        var cfg:ConfigData = new ConfigData();
        cfg.host = "ec2-3-137-172-172.us-east-2.compute.amazonaws.com";
        cfg.port = 9933;
        cfg.zone = "RV-Test";
        sfs.connectWithConfig(cfg);
    }

    public function sendChat(m:String):Void {
        sfs.send(new PublicMessageRequest(m));
    }

    public function sendPosition(_data:Array<Dynamic>):Void {
        //Parse data
        var pos_data:ISFSObject = new SFSObject();
        pos_data.putFloatArray("pos", _data[0]);
        pos_data.putFloatArray("dir", _data[1]);
        pos_data.putFloatArray("up", _data[2]);
        pos_data.putFloatArray("vel", _data[3]);
        pos_data.putUtfString("anim", _data[4]);
        //Send to server
        sfs.send(new ExtensionRequest("EUU", pos_data));
    }

    public function sendPosition_initial(_data:Array<Dynamic>):Void {
        //Parse position
        var pos:Vec3D = new Vec3D(_data[0][0], _data[0][1], _data[0][2]);
        //Send position request
        sfs.send(new SetUserPositionRequest(pos));
        //Create event
        var spawned:IOCommand = new IOCommand();
        spawned.build(IOCommand.CMD_FINISHSPAWN, IOCommand.DATA_NULL);
        passIO(spawned);
    }

    private function passIO(io:IOCommand):Void {
        var outEvent:ConnectEvent = new ConnectEvent(ConnectEvent.PASS_IO);
        outEvent.data = io;
        dispatchEvent(outEvent);
    }

    private function passConsole(m:String):Void {
        var outEvent:ConnectEvent = new ConnectEvent(ConnectEvent.PASS_CONSOLE);
        outEvent.message = m;
        dispatchEvent(outEvent);
    }

    private function updateUserPosition(n:String, uv:UserVariable):Void {
        //Parse position data
        var pos_data:ISFSObject = uv.getSFSObjectValue();
        var u_pos:Array<Float> = pos_data.getFloatArray("pos");
        var u_dir:Array<Float> = pos_data.getFloatArray("dir");
        var u_up:Array<Float> = pos_data.getFloatArray("up");
        var u_vel:Array<Float> = pos_data.getFloatArray("vel");
        var u_anim:String = pos_data.getUtfString("anim");
        //Create data
        var io_pos:IOCommand = new IOCommand();
        io_pos.build(IOCommand.CMD_UPDATEUSER, [n, u_pos, u_dir, u_up, u_vel, u_anim]);
        passIO(io_pos);
    }

    //Listeners
    private function onConnection(e:SFSEvent):Void {
        //Remove listener
        sfs.removeEventListener(SFSEvent.CONNECTION, onConnection);
        //Login
        if(e.params.success) {
            passConsole("Connected! Logging in...");
            sfs.addEventListener(SFSEvent.LOGIN, onLogin);
            sfs.addEventListener(SFSEvent.LOGIN_ERROR, onLoginError);
            sfs.send(new LoginRequest(username, "", "RV-Test"));
        } else {
            //Create error
            var connectError:IODataError = new IODataError("Unable to connect to server", true);
            var outEvent:ConnectEvent = new ConnectEvent(ConnectEvent.CONNECTION_REFUSED);
            outEvent.error = connectError;
            dispatchEvent(outEvent);
        }
    }

    private function onConnectionLost(e:SFSEvent):Void {
        //Remove listener
        sfs.removeEventListener(SFSEvent.CONNECTION_LOST, onConnectionLost);
        //Create error
        var disconnectError:IODataError = new IODataError(e.params.reason, true);
        var outEvent:ConnectEvent = new ConnectEvent(ConnectEvent.CONNECTION_LOST);
        outEvent.error = disconnectError;
        dispatchEvent(outEvent);
    }

    private function onLogin(e:SFSEvent):Void {
        //Remove listeners
        sfs.removeEventListener(SFSEvent.LOGIN, onLogin);
        sfs.removeEventListener(SFSEvent.LOGIN_ERROR, onLoginError);
        //Create data
        var u:User = e.params.user;
        var loginSuccess:IOCommand = new IOCommand();
        loginSuccess.build(IOCommand.CMD_FINISHLOGIN, [u.name]);
        var outEvent:ConnectEvent = new ConnectEvent(ConnectEvent.LOGIN_SUCCESS);
        outEvent.data = loginSuccess;
        dispatchEvent(outEvent);
        //Join test room
        sfs.addEventListener(SFSEvent.ROOM_JOIN, onRoomJoin);
        sfs.addEventListener(SFSEvent.ROOM_JOIN_ERROR, onRoomJoinError);
        sfs.send(new JoinRoomRequest("VR"));
    }

    private function onLoginError(e:SFSEvent):Void {
        //Remove listeners
        sfs.removeEventListener(SFSEvent.LOGIN, onLogin);
        sfs.removeEventListener(SFSEvent.LOGIN_ERROR, onLoginError);
        //Create error
        var loginError:IODataError = new IODataError(e.params.errorMessage, true);
        var outEvent:ConnectEvent = new ConnectEvent(ConnectEvent.LOGIN_ERROR);
        outEvent.error = loginError;
        dispatchEvent(outEvent);
    }

    private function onRoomJoin(e:SFSEvent):Void {
        //Remove listeners
        sfs.removeEventListener(SFSEvent.ROOM_JOIN, onRoomJoin);
        sfs.removeEventListener(SFSEvent.ROOM_JOIN_ERROR, onRoomJoinError);
        //Create data
        var r:Room = e.params.room;
        var roomJoin:IOCommand = new IOCommand();
        roomJoin.build(IOCommand.CMD_JOINEDROOM, [r.name]);
        passIO(roomJoin);
    }

    private function onRoomJoinError(e:SFSEvent):Void {
        //Remove listeners
        sfs.removeEventListener(SFSEvent.ROOM_JOIN, onRoomJoin);
        sfs.removeEventListener(SFSEvent.ROOM_JOIN_ERROR, onRoomJoinError);
        //Create error
        var roomError:IODataError = new IODataError(e.params.errorMessage, false);
        var outEvent:ConnectEvent = new ConnectEvent(ConnectEvent.ROOM_ERROR);
        outEvent.error = roomError;
        dispatchEvent(outEvent);
    }

    private function onPublicMessage(e:SFSEvent):Void {
        //Create data
        var sender:User = e.params.sender;
        var handleChat:IOCommand = new IOCommand();
        handleChat.build(IOCommand.CMD_HANDLECHAT, [sender.name, e.params.message]);
        passIO(handleChat);
    }

    private function onProximityUpdate(e:SFSEvent):Void {
        var added_users:Array<Dynamic> = e.params.addedUsers;
        var removed_users:Array<Dynamic> = e.params.removedUsers;
        for(i in 0...added_users.length) {
            //Get new user data
            var u:User = added_users[i];
            var vec:Vec3D = u.aoiEntryPoint;
            var pos_spawn:Array<Float> = [vec.px, vec.py, vec.pz];
            var io_spawn:IOCommand = new IOCommand();
            io_spawn.build(IOCommand.CMD_SPAWNUNIT, [IOCommand.ENTITY_PLAYER, u.name, pos_spawn]);
            passIO(io_spawn);
        }
        for(i in 0...removed_users.length) {
            //Get removed user data
            var u:User = removed_users[i];
            var io_despawn:IOCommand = new IOCommand();
            io_despawn.build(IOCommand.CMD_DESPAWNUNIT, [u.name]);
            passIO(io_despawn);
        }
    }

    private function onUserVariablesUpdate(e:SFSEvent):Void {
        var u:User = e.params.user;
        if(u.name == sfs.mySelf.name) return; //Exit if self
        //Parse variables
        var vars:Array<Dynamic> = e.params.changedVars;
        for(v in vars) {
            var uv:UserVariable = u.getVariable(v);
            switch(v) {
                case POSITION_DATA:
                    updateUserPosition(u.name, uv);
            }
        }
    }
}