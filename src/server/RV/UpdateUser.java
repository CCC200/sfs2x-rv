package server.RV;

import java.util.Arrays;
import java.util.Collection;

import com.smartfoxserver.v2.SmartFoxServer;
import com.smartfoxserver.v2.api.SFSMMOApi;
import com.smartfoxserver.v2.entities.User;
import com.smartfoxserver.v2.entities.data.ISFSObject;
import com.smartfoxserver.v2.entities.variables.SFSUserVariable;
import com.smartfoxserver.v2.entities.variables.UserVariable;
import com.smartfoxserver.v2.extensions.BaseClientRequestHandler;
import com.smartfoxserver.v2.mmo.Vec3D;

public class UpdateUser extends BaseClientRequestHandler {
	SFSMMOApi mmoApi = (SFSMMOApi) SmartFoxServer.getInstance().getAPIManager().getMMOApi();
	
	@Override
	public void handleClientRequest(User u, ISFSObject p) {
		//Set user variable
		UserVariable user_pos = new SFSUserVariable("pd", p);
		getApi().setUserVariables(u, Arrays.asList(user_pos));
		//Update user position
		Collection<Float> pos = p.getFloatArray("pos");
		Vec3D pos_vec = Conversion.floatCollectionToVec3D(pos);
		mmoApi.setUserPosition(u, pos_vec, u.getLastJoinedRoom());
	}
	
	
	
}
