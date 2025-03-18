#pragma once

enum bones : int {
	head = 6,
	neck = 5,
	chest = 4,
	chest_1 = 2,
	shoulderRight = 8,
	shoulderLeft = 13,
	elbowRight = 9,
	elbowLeft = 14,
	handRight = 11,
	handLeft = 16,
	crotch = 0,
	leftCrotch = 22,
	rightCrotch = 25,
	kneeRight = 26,
	kneeLeft = 23,
	ankleRight = 27,
	ankleLeft = 24,
};

struct BoneConnection {
	int bone1;
	int bone2;

	BoneConnection(int b1, int b2) : bone1(b1), bone2(b2) {}
};

inline BoneConnection boneConnections[] = {
	BoneConnection(bones::neck,bones::crotch),
	BoneConnection(bones::neck, bones::shoulderRight),
	BoneConnection(bones::shoulderRight, bones::elbowRight),
	BoneConnection(bones::elbowRight, bones::handRight),
	BoneConnection(bones::neck, bones::shoulderLeft),
	BoneConnection(bones::shoulderLeft, bones::elbowLeft),
	BoneConnection(bones::elbowLeft, bones::handLeft),
	BoneConnection(bones::crotch, bones::leftCrotch),
	BoneConnection(bones::crotch, bones::rightCrotch),
	BoneConnection(bones::leftCrotch, bones::kneeLeft),
	BoneConnection(bones::kneeLeft,bones::ankleLeft),
	BoneConnection(bones::rightCrotch, bones::kneeRight),
	BoneConnection(bones::kneeRight, bones::ankleRight)
};