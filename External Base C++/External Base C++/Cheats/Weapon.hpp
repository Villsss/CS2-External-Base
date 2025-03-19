#pragma once

inline const char* __fastcall GunIcon(const std::string weapon) {
	std::map<std::string, const char*> gunIcons = {
		{"ct_knife", "]"},
		{"t_knife", "["},
		{"deagle", "A"},
		{"elite", "B"},
		{"fiveseven", "C"},
		{"glock", "D"},
		{"revolver", "J"},
		{"p2000", "E"},
		{"p250", "F"},
		{"usp", "G"},
		{"tec9", "H"},
		{"cz75a", "I"},
		{"mac10", "K"},
		{"ump45", "L"},
		{"bizon", "M"},
		{"mp7", "N"},
		{"mp9", "R"},
		{"p90", "O"},
		{"galilar", "Q"},
		{"famas", "R"},
		{"m4a1_silencer", "T"},
		{"m4a1", "S"},
		{"aug", "U"},
		{"sg556", "V"},
		{"ak47", "W"},
		{"g3sg1", "X"},
		{"scar20", "Y"},
		{"awp", "Z"},
		{"ssg08", "a"},
		{"xm1014", "b"},
		{"sawedoff", "c"},
		{"mag7", "d"},
		{"nova", "e"},
		{"negev", "f"},
		{"m249", "g"},
		{"taser", "h"},
		{"flashbang", "i"},
		{"hegrenade", "j"},
		{"smokegrenade", "k"},
		{"molotov", "l"},
		{"decoy", "m"},
		{"incgrenade", "n"},
		{"c4", "o"},
		{"kit", "r"}
	};

	auto it = gunIcons.find(weapon);
	return (it != gunIcons.end()) ? it->second : "";
}

inline std::map<int, std::string> weaponNames = {
	{1, "deagle"},
	{2, "elite"},
	{3, "fiveseven"},
	{4, "glock"},
	{7, "ak47"},
	{8, "aug"},
	{9, "awp"},
	{10, "famas"},
	{11, "g3Sg1"},
	{13, "galilar"},
	{14, "m249"},
	{17, "mac10"},
	{19, "p90"},
	{23, "mp5sd"},
	{24, "ump45"},
	{25, "xm1014"},
	{26, "bizon"},
	{27, "mag7"},
	{28, "negev"},
	{29, "sawedoff"},
	{30, "tec9"},
	{31, "zeus"},
	{32, "p2000"},
	{33, "mp7"},
	{34, "mp9"},
	{35, "nova"},
	{36, "p250"},
	{38, "scar20"},
	{39, "sg556"},
	{40, "ssg08"},
	{42, "ct_knife"},
	{43, "flashbang"},
	{44, "hegrenade"},
	{45, "smokegrenade"},
	{46, "molotov"},
	{47, "decoy"},
	{48, "incgrenade"},
	{49, "c4"},
	{16, "m4a1"},
	{61, "usp"},
	{60, "m4a1_silencer"},
	{63, "cz75a"},
	{64, "revolver"},
	{59, "t_knife"}
};

inline std::string __fastcall GetWeaponName(int weaponID) {
	auto it = weaponNames.find(weaponID);
	if (it != weaponNames.end()) {
		return it->second;
	}
	return "Weapon_None";
}

enum class e_weapon_type : uint32_t
{
	knife,
	pistol,
	submachinegun,
	rifle,
	shotgun,
	sniper_rifle,
	machinegun,
	c4,
	taser,
	grenade,
	equipment,
	stackableitem,
	fists,
	breachcharge,
	bumpmine,
	tablet,
	melee,
	shield,
	zone_repulsor,
	unknown
};