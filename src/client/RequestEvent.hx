package;

import openfl.events.Event;
import IOCommand;

class RequestEvent extends Event
{
    public static inline var DATA_IN:String = "onDataIn";

    public var data:IOCommand;

    public function new(type:String, bubbles:Bool = false, cancelable:Bool = false)
    {
        super(type, bubbles, cancelable);
    }

    public override function clone():Event
    {
        var out:RequestEvent = new RequestEvent(type, bubbles, cancelable);
        out.data = data;
        return out;
    }
}
