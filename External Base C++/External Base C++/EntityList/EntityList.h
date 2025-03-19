#pragma once

class desiredPlayerValues
{
public:
	uintptr_t playerPawn;
	uintptr_t playerController;
	std::string playerName;
	int playerIndex;
};

class desiredOtherValues
{
public:
	uintptr_t handle;
	std::string otherName;
	int ownerID;
};

class EntityList
{
public:
	uintptr_t client;

	std::vector<desiredPlayerValues> playerList;
	std::vector<desiredOtherValues> otherList;

	void StartEntityLoop();

private:
	void ReadEntityList();
}; inline EntityList List;