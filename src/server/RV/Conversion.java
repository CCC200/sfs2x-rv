package server.RV;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import com.smartfoxserver.v2.mmo.Vec3D;

public class Conversion {
	public static Vec3D floatCollectionToVec3D(Collection<Float> c) {
		List<Float> list = new ArrayList<Float>();
		for(Float f : c) {
			list.add(f);
		}
		Vec3D vec = Vec3D.fromFloatArray(list);
		return vec;
	}
}
