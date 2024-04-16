package;

import haxe.Timer;
import openfl.Lib;
import openfl.display.Sprite;
import openfl.display.Stage;
import Console;
import IOCommand;
import RequestEvent;
import ConnectEvent;
import Monitor;
import Connection;

class Main extends Sprite
{
	public var c_window:Stage = Lib.current.stage;
	public var console:Console = new Console();
	public var monitor:Monitor;
	public var connection:Connection;

	private var timer_out:Timer;
	private var timer_in:Timer;

	public function new()
	{
		super();
		//Create window
		#if debug
		addChild(console.spawn(c_window.stageWidth, c_window.stageHeight));
		#end
		//Setup connection
		connection = new Connection();
		connection.addEventListener(ConnectEvent.CONNECTION_REFUSED, onConnectionError);
        connection.addEventListener(ConnectEvent.CONNECTION_LOST, onConnectionError);
        connection.addEventListener(ConnectEvent.LOGIN_ERROR, onConnectionError);
        connection.addEventListener(ConnectEvent.LOGIN_SUCCESS, onLoginSuccess);
        connection.addEventListener(ConnectEvent.ROOM_ERROR, onConnectionError);
        connection.addEventListener(ConnectEvent.PASS_IO, onPassIO);
        connection.addEventListener(ConnectEvent.PASS_CONSOLE, onPassConsole);
		//Start monitor
		monitor = new Monitor();
		monitor.addEventListener(RequestEvent.DATA_IN, onRequest);
		//Start timers
		timer_out = new Timer(1);
		timer_in = new Timer(1);
		timer_out.run = threadOut;
		timer_in.run = threadIn;
	}

	public function addIO(io:IOCommand):Void {
		console.output("OUT: " + io.name + " -> " + console.format_data(io.data));
		monitor.writeIO(io);
	}

	private function threadOut():Void {
		//Write OUT file
		monitor.writeOUT();
	}

	private function threadIn():Void {
		//Read IN file
		monitor.checkDataIN();
	}

	private function executeIO(io:IOCommand):Void {
		console.output("IN: " + io.name + " -> " + console.format_data(io.data));
		//Execute
		var _data:Array<Dynamic>;
		switch(io.name) {
			case IOCommand.CMD_STARTCONNECT:
                connection.connect(io.data[0]);
            case IOCommand.CMD_SENDCHAT:
                connection.sendChat(io.data[0]);
            case IOCommand.CMD_SPAWNPOS:
                _data = io.dataToNumbers();
                connection.sendPosition_initial(_data);
            case IOCommand.CMD_SENDPOS:
				_data = io.dataToNumbers();
                connection.sendPosition(_data);
		}
	}

	//Listeners
	private function onRequest(e:RequestEvent):Void {
		var request_io:IOCommand = e.data;
		executeIO(request_io);
	}

	private function onConnectionError(e:ConnectEvent):Void {
		console.output("CONNECTION ERROR: " + e.error.reason);
		addIO(e.error.toCmd());
	}

	private function onLoginSuccess(e:ConnectEvent):Void {
		connection.removeEventListener(ConnectEvent.CONNECTION_REFUSED, onConnectionError);
		connection.removeEventListener(ConnectEvent.LOGIN_ERROR, onConnectionError);
		connection.removeEventListener(ConnectEvent.LOGIN_SUCCESS, onLoginSuccess);
		addIO(e.data);
	}

	private function onPassIO(e:ConnectEvent):Void {
		addIO(e.data);
	}

	private function onPassConsole(e:ConnectEvent):Void {
		console.output(e.message);
	}
}
