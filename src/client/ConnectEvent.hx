package;

import openfl.events.Event;
import IOCommand;
import IODataError;

class ConnectEvent extends Event
{
    public static inline var CONNECTION_LOST:String = "onConnectionLost";
    public static inline var CONNECTION_REFUSED:String = "onConnectionRefused";
    public static inline var LOGIN_ERROR:String = "onLoginError";
    public static inline var LOGIN_SUCCESS:String = "onLoginSuccess";
    public static inline var ROOM_ERROR:String = "onRoomError";
    public static inline var PASS_IO:String = "onPassIO";
    public static inline var PASS_CONSOLE:String = "onPassConsole";

    public var data:IOCommand;
    public var error:IODataError;
    public var message:String;

    public function new(type:String, bubbles:Bool = false, cancelable:Bool = false)
    {
        super(type, bubbles, cancelable);
    }

    public override function clone():Event
    {
        var out:ConnectEvent = new ConnectEvent(type, bubbles, cancelable);
        out.data = data;
        out.error = error;
        out.message = message;
        return out;
    }
}
