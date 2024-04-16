package;

import openfl.filesystem.*;
import openfl.events.Event;
import openfl.events.EventDispatcher;
import openfl.errors.IOError;
import RequestEvent;
import IOCommand;

class Monitor extends EventDispatcher
{
    private static inline var IO_IN:Int = 0;
    private static inline var IO_OUT:Int = 1;
    private static inline var IO_BOTH:Int = 2;

    private var in_memory:String = "";
    private var file_lock:FileStream = new FileStream();

    public var data_in:File;
    public var data_out:File;
    public var fileR:FileStream = new FileStream();
    public var fileW:FileStream = new FileStream();
    public var pendingIO:Array<String> = new Array<String>();

    public function new()
    {
        super();
        data_in = File.applicationStorageDirectory.resolvePath("in/io");
        data_out = File.applicationStorageDirectory.resolvePath("out/io");
        emptyIO();
    }

    public function writeIO(io:IOCommand):Void {
        pendingIO.push(io.toString());
    }

    public function writeOUT():Void {
        if(pendingIO.length <= 0) return; //No data, skip cycle
        try {
            fileW.open(data_out, FileMode.WRITE);
        }
        catch(IOError) {
            //File in use, skip cycle
            return;
        }
        var write_bytes:String = "";
        var io_length:Int = pendingIO.length;
        for(i in 0...io_length) {
            write_bytes += pendingIO.shift();
            if(i < io_length - 1) {
                write_bytes += "\n";
            }
        }
        fileW.writeUTFBytes(write_bytes);
        fileW.close();
    }

    public function checkDataIN():Void {
        var IN_TEXT:String = "";
        try {
            file_lock.open(data_in, FileMode.UPDATE);
            fileR.open(data_in, FileMode.READ);
        }
        catch(IOError) {
            //File is in use, skip cycle
            trace("IOError");
            return;
        }
        if(fileR.bytesAvailable > 0) {
            try {
                IN_TEXT = fileR.readUTFBytes(fileR.bytesAvailable);
            }
            catch(e) {
                //Bad input, skip cycle (Temp)
                trace("Eof");
                return;
            }
        }
        file_lock.close();
        fileR.close();
        if(IN_TEXT.length > 0 && IN_TEXT != in_memory) {
            in_memory = IN_TEXT;
            parseIO(IN_TEXT);
        }
    }

    private function emptyIO(n:Int = IO_BOTH):Void {
        var DATA_ARRAY:Array<Dynamic> = [data_in, data_out];
        if(n == IO_BOTH) {
            for(i in 0...DATA_ARRAY.length) {
                fileW.open(DATA_ARRAY[i], FileMode.WRITE);
                fileW.writeUTFBytes("");
            }
        } else {
            fileW.open(DATA_ARRAY[n], FileMode.WRITE);
            fileW.writeUTFBytes("");
        }
        fileW.close();
    }

    private function parseIO(IN_TEXT:String):Void {
        var IN_ARRAY_STRINGS:Array<String> = IN_TEXT.split(String.fromCharCode(13)); //newline split
        for(v in IN_ARRAY_STRINGS) {
            //Parse
            var ioObj:IOCommand = new IOCommand();
            ioObj.fromString(v);
            //Broadcast
            var outEvent:RequestEvent = new RequestEvent(RequestEvent.DATA_IN);
            outEvent.data = ioObj;
            dispatchEvent(outEvent);
        }
    }
}
