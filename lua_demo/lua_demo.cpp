// lua_demo.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

using namespace std;

int main()
{
	printf("This is Lua demo Project.\n");

	lua_State *L = luaL_newstate();	//创建luaState

	char chArr[MAX_PATH] = { 0 };
	char* pTemp = NULL;

	GetModuleFileNameA(NULL, chArr, MAX_PATH);
	pTemp = strrchr(chArr, '\\');
	if (pTemp)
	{
		*pTemp = '\0';
	}

	strcat_s(chArr, "\\script\\demo.lua");


	int bRet = -1;

	bRet = luaL_loadfile(L, chArr); //加载lua文件
	if (bRet)
	{
		cout << "load file error!" << endl;
		return -1;
	}

	bRet = -1;

	bRet = lua_pcall(L, 0, 0, 0); //运行lua文件
	if (bRet)
	{
		cout << "pcall error!" << endl;
		return -1;
	}

	lua_getglobal(L, "var1"); //读取var1
	int var1 = lua_tonumber(L, -1);
	cout << "var1 = " << var1 << endl;

	lua_getglobal(L, "var2"); //读取var2
	int var2 = lua_tonumber(L, -1);
	cout << "var2 = " << var2 << endl;

	//--------function add---------------
	lua_getglobal(L, "Add"); //读取函数
	lua_pushnumber(L, var1); //压入堆栈(1)
	lua_pushnumber(L, var2); //压入堆栈(2)

	int nRet = -1;

	nRet = lua_pcall(L, 2, 1, 0);
	if (nRet)
	{
		cout << "pcall error!" << endl;
		return -1;
	}

	int Sum = lua_tonumber(L, -1);
	cout << "var1 + var2 = " << Sum << endl;

	//--------function sub---------------
	lua_getglobal(L, "Sub"); //读取函数
	lua_pushnumber(L, var1); //压入堆栈(1)
	lua_pushnumber(L, var2); //压入堆栈(2)

	nRet = -1;

	nRet = lua_pcall(L, 2, 1, 0);
	if (nRet)
	{
		cout << "pcall error!" << endl;
		return -1;
	}

	int Sub = lua_tonumber(L, -1);
	cout << "var1 - var2 = " << Sub << endl;

	//--------function mul---------------
	lua_getglobal(L, "Mul"); //读取函数
	lua_pushnumber(L, var1); //压入堆栈(1)
	lua_pushnumber(L, var2); //压入堆栈(2)

	nRet = -1;

	nRet = lua_pcall(L, 2, 1, 0);
	if (nRet)
	{
		cout << "pcall error!" << endl;
		return -1;
	}

	int Mul = lua_tonumber(L, -1);
	cout << "var1 * var2 = " << Mul << endl;

	//--------function dev---------------
	lua_getglobal(L, "Dev"); //读取函数
	lua_pushnumber(L, var1); //压入堆栈(1)
	lua_pushnumber(L, var2); //压入堆栈(2)

	nRet = -1;

	nRet = lua_pcall(L, 2, 1, 0);
	if (nRet)
	{
		cout << "pcall error!" << endl;
		return -1;
	}

	float Dev = lua_tonumber(L, -1);
	cout << "var1 / var2 = " << Dev << endl;

	lua_close(L);

	system("pause");
    return 0;
}

