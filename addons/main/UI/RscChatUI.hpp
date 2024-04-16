class RscEdit;

#define GUI_GRID_CENTER_X	((safezoneX + (safezoneW - ((safezoneW / safezoneH) min 1.2))/2))
#define GUI_GRID_CENTER_Y	((safezoneY + (safezoneH - (((safezoneW / safezoneH) min 1.2) / 1.2))/2))
#define GUI_GRID_CENTER_W	((((safezoneW / safezoneH) min 1.2) / 40))
#define GUI_GRID_CENTER_H	(((((safezoneW / safezoneH) min 1.2) / 1.2) / 25))
#define GUI_GRID_CENTER_WAbs	(((safezoneW / safezoneH) min 1.2))
#define GUI_GRID_CENTER_HAbs	((((safezoneW / safezoneH) min 1.2) / 1.2))

class RscChatUI
{
	idd = -1;
	class ControlsBackground {};
	class Controls
	{
		class RscChatBox: RscEdit
		{
			idc = 1400;
			text = "";
			onKeyUp = "if(_this select 1 == 28) then { parse_chat_input; };";
			x = -11 * GUI_GRID_CENTER_W + GUI_GRID_CENTER_X;
			y = 28.5 * GUI_GRID_CENTER_H + GUI_GRID_CENTER_Y;
			w = 25.5 * GUI_GRID_CENTER_W;
			h = 1 * GUI_GRID_CENTER_H;
			colorText[] = {255,255,255,1};
			colorBackground[] = {0,0,0,0.56};
		};
	};
};
