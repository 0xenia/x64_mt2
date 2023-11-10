// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

struct TPixelPosition
{
	float x;
	float y;
	float z;
};

static int CountDistanceTwoPoints(TPixelPosition startPoint, TPixelPosition endPoint)
{
	// Calculating distance
	return sqrt(pow(endPoint.x - startPoint.x, 2) +
		pow(endPoint.y - startPoint.y, 2) * 1.0);
}
static int CountDistanceTwoPoints(int x1, int y1, int x2, int y2)
{
	// Calculating distance
	return sqrt(pow(x2 - x1, 2) +
		pow(y2 - y1, 2) * 1.0);
}

uint64_t entryAddr = (uint64_t)GetModuleHandleA(NULL);

static uint64_t* PlayerNEW_GetMainActorPtr()
{
	DWORD_PTR* MainActorPtr = *(DWORD_PTR**)(*reinterpret_cast<DWORD_PTR*>(entryAddr + 0x22EE110) + 0x20);
	uint64_t* MainActor = reinterpret_cast<uint64_t*>(MainActorPtr);
	return MainActor;
}

typedef std::map<uint32_t, uint64_t*> TCharacterInstanceMap;

void CreateConsole()
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONOUT$", "w", stderr);
	freopen_s(&f, "CONIN$", "r", stdin);
}

static int GetInstanceType(uint64_t* Instance)
{
	typedef int(__fastcall* GetInstanceType_t)(uint64_t* This);
	uintptr_t calladdr = entryAddr + 0x8586C;
	GetInstanceType_t GetInstanceType = (GetInstanceType_t)calladdr;
	return GetInstanceType(Instance);
}

static uint32_t GetMainCharacterVID()
{
	DWORD_PTR* MainActorPtr = *(DWORD_PTR**)(*reinterpret_cast<DWORD_PTR*>(entryAddr + 0x2296F88) + 0x70);
	uint32_t MainActor = reinterpret_cast<uint32_t>(MainActorPtr);
	return MainActor;
}

uint32_t etMainCharacterVID()
{
	uint64_t mainCharacter = *reinterpret_cast<uint64_t*>(entryAddr + 0x2296F88);
	uint32_t mainCharacterVID = *reinterpret_cast<uint32_t*>(mainCharacter + 0x70);
	return mainCharacterVID;
}

static const char* GetNameString(uint64_t* Instance)
{
	typedef const char* (__fastcall* GetNameString_t)(uint64_t* This);
	uintptr_t calladdr = entryAddr + 0x85878;
	GetNameString_t GetNameString = (GetNameString_t)calladdr;
	return GetNameString(Instance);
}

static uint64_t* GetInstancePtr(uint32_t dwVID)
{
	typedef uint64_t* (__fastcall* GetMainActorPtr_t)(uint64_t This, uint32_t dwVID);
	uintptr_t calladdr = entryAddr + 0xD22CC;
	uint64_t PythonCharacterManager = *reinterpret_cast<uint64_t*>(entryAddr + 0x2297018);
	printf("PythonCharacterManager: %p\n", PythonCharacterManager);
	GetMainActorPtr_t GetMainActorPtr = (GetMainActorPtr_t)calladdr;
	printf("GetMainActorPtr: %p\n", GetMainActorPtr);
	return GetMainActorPtr(PythonCharacterManager, dwVID);
}

static void GetPosition(uint64_t* Instance, TPixelPosition* pPixelPosition)
{
	typedef void(__fastcall* GetPosition_t)(uint64_t* This, TPixelPosition* pPixelPosition);
	uintptr_t calladdr = entryAddr + 0x9336C;
	GetPosition_t GetPosition = (GetPosition_t)calladdr;
	return GetPosition(Instance, pPixelPosition);
}

static bool IsDead(uint64_t* Instance)
{
	typedef bool(__fastcall* tIsDead)(uint64_t* This);
	uintptr_t calladdr = entryAddr + 0x85E5C;
	tIsDead IsDead = (tIsDead)calladdr;
	return IsDead(Instance);
}

static void Attack(uint32_t VID, bool isauto)
{
	typedef void(__fastcall* tAttack)(uint64_t This, uint64_t Instance, uint32_t VID, bool isauto);
	uintptr_t calladdr = entryAddr + 0x186E54;
	uint64_t PythonPlayer = *reinterpret_cast<uint64_t*>(entryAddr + 0x2296F88);
	uint64_t Instance = *reinterpret_cast<uint64_t*>(entryAddr + 0x22EE110) + 0x20;
	tAttack Attack = (tAttack)calladdr;
	if (Instance != NULL)
	{
		Attack(PythonPlayer, Instance, VID, isauto);
	}
}

static uint64_t mCallAddres = 0x0;
static uint64_t mCPythonInstance = 0x0;
static bool initialized = false;

static void PickCloseItem()
{
	typedef void(__fastcall* tPickCloseItem)(uint64_t This);
	mCallAddres = entryAddr + 0x12c2f0;
	mCPythonInstance = *reinterpret_cast<uint64_t*>(entryAddr + 0x8FEE60);
	tPickCloseItem PickCloseItem = (tPickCloseItem)mCallAddres;
	PickCloseItem(mCPythonInstance);
}

static int falseCount = 0;
std::map<uint32_t, uint64_t*> GetObjectList()
{
	std::map<uint32_t, uint64_t*> objectList;
	uint64_t m_kAliveInstMapAddr = *reinterpret_cast<uint64_t*>(entryAddr + 0x22EE110) + 0x30;
	TCharacterInstanceMap* m_kAliveInstMap = reinterpret_cast<TCharacterInstanceMap*>(m_kAliveInstMapAddr);
	uint32_t VIDC = 0;
	uint64_t* c = nullptr;
	int lowestDistance = 200000;
	bool foundClosest = false;

	uint64_t* MyIns = PlayerNEW_GetMainActorPtr();
	TPixelPosition MyPos;
	GetPosition(MyIns, &MyPos);

	for (auto itor = m_kAliveInstMap->begin(); itor != m_kAliveInstMap->end(); ++itor)
	{
		auto Index = itor->first;
		auto Instance = itor->second;

		if (GetInstanceType(Instance) == 2)
		{
			TPixelPosition PixelPosition;
			GetPosition(Instance, &PixelPosition);
			int distance = CountDistanceTwoPoints(MyPos, PixelPosition);

			if (!foundClosest || distance < lowestDistance)
			{
				lowestDistance = distance;
				VIDC = Index;
				c = Instance;
				foundClosest = true;
			}
		}
	}
	if (MyIns != nullptr)
	{
		falseCount++;
		while (!IsDead(c))
		{
			Attack(VIDC, 0);

			Sleep(120);
		}
		while (IsDead(c))
		{
			Sleep(400);
			PickCloseItem();
			foundClosest = false;
		}
	}
	return objectList;
}
//pythonplayer 2296F88

void Thread(LPVOID lpParam)
{
	CreateConsole();
	std::cout << "DLL Injected" << std::endl;
	while (true)
	{
		PickCloseItem();
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&Thread, hModule, NULL, NULL);
	}
	case DLL_THREAD_ATTACH: 
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}