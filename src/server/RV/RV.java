package server.RV;

import com.smartfoxserver.v2.SmartFoxServer;
import com.smartfoxserver.v2.extensions.SFSExtension;

public class RV extends SFSExtension {
	SmartFoxServer sfs = SmartFoxServer.getInstance();
	
	@Override
	public void init() {
		//Requests
		//Entity
		addRequestHandler("EUU", UpdateUser.class);
	}
	
}
