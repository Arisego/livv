#include "ModelInfo.h"



FModelInfo::FModelInfo()
	: Moc(nullptr)
	, Table(nullptr)
	, UserData(nullptr)
	, Physics(nullptr)
	, Animation(nullptr)
	, Model(nullptr)
{

}

FModelInfo::~FModelInfo()
{
	if (UserData) Deallocate(UserData);
	if (Physics) Deallocate(Physics);
	if (Animation)Deallocate(Animation);
	if (Table)Deallocate(Table);
	//if (Model) DeallocateAligned(Model);
	if (Moc) DeallocateAligned(Moc);

	Moc = nullptr;
	Table = nullptr;
	UserData = nullptr;
	Physics = nullptr;
	Animation = nullptr;
	Model = nullptr;
}
