package;

class IOCommand
{
    //Core
    public static inline var CMD_STUB:String = "STUB";
    public static inline var CMD_ERROR:String = "ERROR";
    public static inline var CMD_STARTCONNECT:String = "STARTCONNECT";
    public static inline var CMD_FINISHLOGIN:String = "FINISHLOGIN";
    public static inline var CMD_JOINEDROOM:String = "JOINEDROOM";
    public static inline var CMD_FINISHSPAWN:String = "FINISHSPAWN";

    //Chat
    public static inline var CMD_SENDCHAT:String = "SENDCHAT";
    public static inline var CMD_HANDLECHAT:String = "HANDLECHAT";

    //Entity
    public static inline var CMD_SPAWNPOS:String = "SPAWNPOS";
    public static inline var CMD_SENDPOS:String = "SENDPOS";
    public static inline var CMD_UPDATEUSER:String = "UPDATEUSER";
    public static inline var CMD_UPDATEAI:String = "UPDATEAI";
    public static inline var CMD_SPAWNUNIT:String = "SPAWNUNIT";
    public static inline var CMD_DESPAWNUNIT:String = "DESPAWNUNIT";

    public static inline var ENTITY_PLAYER:Int = 1;
    public static inline var ENTITY_AI:Int = 2;

    public static final DATA_NULL:Array<Dynamic> = ["NULL"];

    public var name:String;
    public var data:Array<Dynamic>;

    public function new()
    {
        name = CMD_STUB;
        data = [];
    }

    public function fromString(s:String):Void {
        var IN_ARRAY_DATA:Array<Dynamic> = new Array<Dynamic>();
        //Name-Data split
        IN_ARRAY_DATA = s.split(",");
        name = IN_ARRAY_DATA[0];
        //Multi-data structure
        if(IN_ARRAY_DATA[1].indexOf(";") > -1) {
            data = IN_ARRAY_DATA[1].split(";");
        } else {
            data.push(IN_ARRAY_DATA[1]);
        }
        var data_convert = new Array<Dynamic>();
        for(v in data) {
            if(v.indexOf(":") > -1) {
                var split = v.split(":");
                data_convert.push(split);
            } else {
                data_convert.push(v);
            }
        }
        data = data_convert;
    }

    public function toString(): String {
        var build_string = name + ",";
        if(data.length > 1) {
            for(i in 0...data.length) {
                if(data[i] is Array) {
                    for(i2 in 0...data[i].length) {
                        build_string += Std.string(data[i][i2]);
                        if(i2 < data[i].length - 1) {
                            build_string += ":";
                        }
                    }
                } else {
                    build_string += Std.string(data[i]);
                }
                if(i < data.length - 1) {
                    build_string += ";";
                }
            }
        } else {
            build_string += Std.string(data[0]);
        }
        return build_string;
    }

    public function build(n:String, d:Array<Dynamic>):Void {
        name = n;
        data = d;
    }

    public function dataToNumbers():Array<Dynamic> {
        //Parse and return
        for(i in 0...data.length) {
            if(data[i] is Array) {
                for(i2 in 0...data[i].length) {
                    if(stringTestValue(data[i][i2])) {
                        data[i][i2] = convertNumber(data[i][i2]);
                    }
                }
            } else if(stringTestValue(data[i])) {
                data[i] = convertNumber(data[i]);
            }
        }
        return data;
    }

    private function convertNumber(s:String):Any {
        if(s.indexOf(".") > -1) {
            return Std.parseFloat(s);
        } else {
            return Std.parseInt(s);
        }
    }

    private function stringTestValue(s:String):Bool {
        if(Std.parseInt(s) == null) return false;
        return true;
    }
}
