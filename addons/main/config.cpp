#include "UI\RscChatUI.hpp"

class CfgPatches {
	class sfs2x_rv {
		name = "SFS2X RV";
		units[] = {};
		weapons[] = {};
		requiredVersion = 2.06;
		requiredAddons[] = {"intercept_core"};
		author = "ZeroIP";
		authors[] = {"Avery Broere"};
		url = "";
		version = "1.0";
		versionStr = "1.0";
		versionAr[] = {1,0};
	};
};
class Intercept {
    class sfs2x_rv_plugins {
        class sfs2x_rv_hook {
            pluginName = "sfs2x-rv";
        };
    };
};
