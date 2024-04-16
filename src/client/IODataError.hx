package;

import IOCommand;

class IODataError
{
    public static inline var IS_FATAL:Bool = true;

    public var reason:String;
    public var fatal:Bool;

    public function new(r:String = "", f:Bool = false)
    {
        build(r, f);
    }

    public function get():Array<Dynamic> {
        return([reason, fatal]);
    }

    public function build(r:String, f:Bool = false) {
        reason = r;
        fatal = f;
    }

    public function toCmd(): IOCommand {
        var error:IOCommand = new IOCommand();
        error.build(IOCommand.CMD_ERROR, get());
        return error;
    }
}
