package;

import openfl.text.TextField;
import openfl.text.TextFormat;
import openfl.filesystem.*;

class Console
{
    public var c_output:TextField = new TextField();
    public var c_format:TextFormat = new TextFormat();
    public var c_log:File;
    public var file: FileStream = new FileStream();

    public function new()
    {
        var log_name = log_date();
        c_log = File.applicationStorageDirectory.resolvePath("log_" + log_name + ".txt");
        c_output.embedFonts = true;
        c_format.font = "Hack Regular";
        c_output.multiline = true;
        c_output.wordWrap = true;
        c_output.x = 0;
        c_output.y = 0;
        c_output.text = "SFS2X RV Client has launched.";
    }

    public function spawn(w:Int, h:Int, s:Int = 12):TextField {
        c_output.width = w;
        c_output.height = h;
        c_format.size = s;
        c_output.setTextFormat(c_format);
        return c_output;
    }

    public function output(t:String):Void {
        //Record to log
        file.open(c_log, FileMode.APPEND);
        file.writeUTFBytes(t + "\n");
        file.close();
        //Display in console
        #if debug
        if(c_output.numLines >= 50) {
            c_output.text = "--See log file for more details--";
        }
        c_output.appendText("\n" + t);
        #end
    }

    public function format_data(d:Array<Dynamic>):String {
        var s:String = "";
        for(i in 0...d.length) {
            s += Std.string(d[i]);
            if(i < d.length - 1) {
                s += ";";
            }
        }
        return s;
    }

    private function log_date():String {
        var date:Date = Date.now();
        return date.getFullYear() + "_" + (date.getMonth() + 1) + "_" + date.getDate() + "__" + date.getHours() + "_" + date.getMinutes() + "_" + date.getSeconds();
    }
}
