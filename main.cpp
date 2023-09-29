#include "PluginAPI.h"
#include "SQImports.h"
#include <stdio.h> //For printing text on console
#include <time.h> //For printing time on console
#include <string.h> //For calculating length of 
#include <math.h> //For calculating distance between players
#define __REMEXEC_VERSION__ "v1.1.1"
#ifdef WIN32
#include <Windows.h> //For SetConsoleTextAttribute
#endif
PluginFuncs* Server;
PluginCallbacks* Callbacks;
HSQAPI sq;
HSQUIRRELVM v; int32_t token;
HSQOBJECT closuretable; HSQOBJECT delegatetable;
enum OPERATION{
	ADDITION,SUBTRACTION,MULTIPLICATION,DIVISION,MODULO,UNARY_MINUS
};
#define REMEXEC_ERROR_VARIABLE "REMEXEC_ERROR"
#define REMEXEC_IDENTIFIER_EXECUTE 0x40ffffe1
#define REMEXEC_IDENTIFIER_EXECUTE_WITHTOKEN 0x40ffffe2
#define REMEXEC_IDENTIFIER_REPLY 0x40ffffe3

uint8_t REMOTE_ERROR = 0;
#include "util.h" //include util.h here so that variables sq and v will be available to it
size_t InsertSQObject(uint8_t* &buffer, size_t &size, size_t index, SQObject* key);
uint8_t RemExec_OnServerInitialize()
{
	return 1;
}

SQInteger fn_CallRemoteFuncEx(HSQUIRRELVM v)
{
	SQInteger top = sq->gettop(v);

	uint8_t* buffer = (uint8_t*)calloc(5, sizeof(uint8_t));
	size_t size = 5;
	if (buffer)
	{
		*buffer = 'E';// code for call 
		size_t s = 5, s1; int i = 2;
		do
		{
			SQObject obj;
			sq->getstackobj(v, i++, &obj);
			s1 = InsertSQObject(buffer, size, s, &obj);
			//s1 is no:of bytes inserted
			if (s1 == 0)
			{
				free(buffer);
				return sq->throwerror(v, "Error writing object");
			}
			s += s1;
		} while (i <= top);

		//size is the total size of the buffer.
		*(uint32_t*)(buffer + 1) = (uint32_t)size - 5;
		//buffer is ready, now return it.
		SQUserPointer data = sq->newuserdata(v, size);
		memcpy(data, buffer, size);
		free(buffer);
		sq->pushobject(v, delegatetable);
		sq->setdelegate(v, -2);
		return 1;
	}
	else return sq->throwerror(v, "Error when allocating memory");
}

SQInteger fn_CallRemoteFunc(HSQUIRRELVM v)
{
	 SQInteger top = sq->gettop(v);
	
	uint8_t* buffer = (uint8_t*)calloc(5, sizeof(uint8_t));
	size_t size = 5;
	if (buffer)
	{
		*buffer = 'F';// code for call 
		size_t s = 5, s1; int i = 2;
		do
		{
			SQObject obj; 
			sq->getstackobj(v, i++, &obj);
			s1 = InsertSQObject(buffer, size, s, &obj);
			//s1 is no:of bytes inserted
			if (s1 == 0)
			{
				free(buffer);
				return sq->throwerror(v, "Error writing object");
			}
			s += s1;
		} while (i <= top);

		//size is the total size of the buffer.
		*(uint32_t*)(buffer + 1) = (uint32_t)size - 5;
		//buffer is ready, now return it.
		SQUserPointer data = sq->newuserdata(v, size);
		memcpy(data, buffer, size);
		free(buffer);
		sq->pushobject(v, delegatetable);
		sq->setdelegate(v, -2);
		return 1;
	}
	else return sq->throwerror(v, "Error when allocating memory");
}
SQInteger fn_GetRemoteValue(HSQUIRRELVM v)
{
	SQObject key;
	sq->getstackobj(v, 2, &key);
	uint8_t* buffer = (uint8_t*)calloc(5, sizeof(uint8_t));
	size_t size = 5;
	if (buffer)
	{
		*buffer = 'g';// code for get val 
		size_t s1 = InsertSQObject(buffer, size, 5, &key);
		//s1 is no:of bytes inserted
		if (s1 == 0)
		{
			free(buffer);
			return sq->throwerror(v, "Error writing key");
		}
		//size is the total size of the buffer.
		*(uint32_t*)(buffer + 1) = (uint32_t)size - 5;
		//buffer is ready, now return it.
		SQUserPointer data = sq->newuserdata(v, size);
		memcpy(data, buffer, size);
		free(buffer);
		sq->pushobject(v, delegatetable);
		sq->setdelegate(v, -2);
		return 1;
	}
	else return sq->throwerror(v, "Error when allocating memory");
}

SQInteger fn_GetRemoteValueEx(HSQUIRRELVM v)
{
	SQObject obj, key;
	sq->getstackobj(v, 2, &obj);
	sq->getstackobj(v, 3, &key);
	uint8_t* buffer = (uint8_t*)calloc(5, sizeof(uint8_t));
	size_t size = 5;
	if (buffer)
	{
		*buffer = 'k';// code for get val ex
		size_t s1 = InsertSQObject(buffer, size, 5, &obj);
		//s1 is no:of bytes inserted
		if (s1 == 0)
		{
			free(buffer);
			return sq->throwerror(v, "Error writing object");
		}
		size_t s2 = InsertSQObject(buffer, size, 5 + s1, &key);
		if (s2 == 0)
		{
			free(buffer);
			return sq->throwerror(v, "Error writing key");
		}
		//size is the total size of the buffer.
		*(uint32_t*)(buffer + 1) = (uint32_t)size - 5;
		//buffer is ready, now return it.
		SQUserPointer data = sq->newuserdata(v, size);
		memcpy(data, buffer, size);
		free(buffer);
		sq->pushobject(v, delegatetable);
		sq->setdelegate(v, -2);
		return 1;
	}
	else return sq->throwerror(v, "Error when allocating memory");
}

SQInteger fn_SetRemoteValueEx(HSQUIRRELVM v)
{
	SQObject obj, key, value;
	sq->getstackobj(v, 2, &obj);
	sq->getstackobj(v, 3, &key);
	sq->getstackobj(v, 4, &value);
	uint8_t* buffer = (uint8_t*)calloc(5, sizeof(uint8_t));
	size_t size = 5;
	if (buffer)
	{
		*buffer = 'j';// code for set val ex
		size_t s1 = InsertSQObject(buffer, size, 5, &obj);
		//s1 is no:of bytes inserted
		if (s1 == 0)
		{
			free(buffer);
			return sq->throwerror(v, "Error writing object");
		}
		size_t s2 = InsertSQObject(buffer, size, 5 + s1, &key);
		if (s2 == 0)
		{
			free(buffer);
			return sq->throwerror(v, "Error writing key");
		}
		size_t s3 = InsertSQObject(buffer, size, 5 + s1 + s2, &value);
		if (s3 == 0)
		{
			free(buffer);
			return sq->throwerror(v, "Error writing value");
		}
		//size is the total size of the buffer.
		*(uint32_t*)(buffer + 1) = (uint32_t)size - 5;
		//buffer is ready, now return it.
		SQUserPointer data = sq->newuserdata(v, size);
		memcpy(data, buffer, size);
		free(buffer);
		sq->pushobject(v, delegatetable);
		sq->setdelegate(v, -2);
		return 1;
	}
	else return sq->throwerror(v, "Error when allocating memory");
}

SQInteger fn_SetRemoteValue(HSQUIRRELVM v)
{
	SQObject key,value;
	sq->getstackobj(v, 2, &key);
	sq->getstackobj(v, 3, &value);
	uint8_t* buffer = (uint8_t*)calloc(5, sizeof(uint8_t));
	size_t size = 5;
	if (buffer)
	{
		*buffer = 'h';// code for set val
		size_t s=InsertSQObject(buffer,size, 5, &key);
		//s is no:of bytes inserted
		if (s == 0)
		{
			free(buffer);
			return sq->throwerror(v, "Error writing key");
		}
		s = InsertSQObject(buffer, size, 5 + s, &value);
		if (s == 0)
		{
			free(buffer);
			return sq->throwerror(v, "Error writing value");
		}
		//size is the total size of the buffer.
		*(uint32_t*)(buffer + 1) = (uint32_t)size - 5;
		//buffer is ready, now return it.
		SQUserPointer data=sq->newuserdata(v, size);
		memcpy(data, buffer, size);
		free(buffer);
		sq->pushobject(v, delegatetable);
		sq->setdelegate(v, -2);
		return 1;
	}
	else return sq->throwerror(v, "Error when allocating memory");
}
SQInteger fn_RemExec(HSQUIRRELVM v)
{
	bool ack = false, bClosure = false; SQObject closure;
	if (sq->gettop(v) >= 4 && sq->gettype(v, 4) == OT_BOOL)
	{
		SQBool b; sq->getbool(v, 4, &b);
		if (b == SQTrue)
		{
			ack = true;
			if (sq->gettop(v) >= 5)
			{
				if (sq->gettype(v, 5) == OT_CLOSURE ||
					sq->gettype(v, 5) == OT_NATIVECLOSURE)
				{
					sq->getstackobj(v, 5, &closure);
					bClosure = true;
				}
				else return sq->throwerror(v, "Parameter 4 is not function");
			}
		}
	}
	
	SQInteger playerId;
	//Get ID of the player ( 3 )
	sq->pushstring(v, "ID", -1);
	if (SQ_SUCCEEDED(sq->get(v, 3)))
	{
		sq->getinteger(v, -1, &playerId);
	}
	else return sq->throwerror(v, "Error getting player ID");
	//Get the user pointer ( 2 ) first
	SQUserPointer up=NULL; SQInteger sizeUsrPtr;
	if (sq->gettype(v, 2) == OT_USERDATA)
	{
		if (SQ_FAILED(sq->getuserdata(v, 2, &up, NULL)))
		{
			return sq->throwerror(v, "Error in getting user data");
		}
		sizeUsrPtr = sq->getsize(v, 2);
	}
	
	//Get the actual size of it
	//0th - 'F'/G, etc
	//size is 1-4
	try
	{
		//size+5 because of index 0 and 1-4
		SQInteger size = *(uint32_t*)((uint8_t*)up + 1)+5;
		
		if (size < sizeUsrPtr)
		{
			//Allocate 4 bytes for writing identifier
			void* serverdata = NULL; size_t csdatasize;//client script data size
			if (ack == false) {
				if (size+4 >= 4096)
					return sq->throwerror(v, "Client Script Data size more than 4095 bytes");
				serverdata = calloc(size + 4, sizeof(uint8_t));
				csdatasize = size + 4;
			}
			else
			{
				if (size+8 >= 4096)
					return sq->throwerror(v, "Client Script Data size more than 4095 bytes"); 
				serverdata = calloc(size + 8, sizeof(uint8_t));//4 bytes for identifier and 4 bytes for ack id
				csdatasize = size + 8;
			}
			
			if (serverdata)
			{
				if (ack == false)
				{
					memcpy((uint8_t*)serverdata + 4, up, size);
					*(uint32_t*)serverdata = REMEXEC_IDENTIFIER_EXECUTE;
				}
				else
				{
					memcpy((uint8_t*)serverdata + 8, up, size);
					*(uint32_t*)serverdata = REMEXEC_IDENTIFIER_EXECUTE_WITHTOKEN;
					token == -1 ? token++ : token;//-1 is used to signal error
					*(int32_t*)((uint8_t*)serverdata+4) = token;
				}
				
				vcmpError e = Server->SendClientScriptData((int32_t)playerId, serverdata, csdatasize );
				free(serverdata);
				if (e != vcmpErrorNone)
				{
					return sq->throwerror(v, "Error when sending ClientScriptData");
				}
				else {
					if (ack && bClosure)
					{
						//Set key in closure table
						sq->pushobject(v, closuretable);
						sq->pushinteger(v, token);
						sq->pushobject(v, closure);
						if (SQ_SUCCEEDED(sq->rawset(v, -3)))
						{
							sq->pop(v, 1);//pops closuretable
						}
						else
						{
							sq->pop(v, 1);
							return sq->throwerror(v, "Error in setting reply callback");
						}
					}
					if(ack)token++;
					return 0;
				}
			}
			else
				return sq->throwerror(v, "Error when allocating memory");
		}
		else return sq->throwerror(v, "Invalid size inside userdata");
	}
	catch (...)
	{
		return sq->throwerror(v, "Error in reading userdata");
	}
	return 0;
}
SQInteger RemExec_RegisterSquirrelConst(HSQUIRRELVM v, const SQChar* cname, const SQChar* cvalue) {
	sq->pushconsttable(v);
	sq->pushstring(v, cname, -1);
	sq->pushstring(v, cvalue, -1);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);
	return 0;
}
SQInteger RemExec_RegisterSquirrelConst(HSQUIRRELVM v, const SQChar* cname, SQInteger value) {
	sq->pushconsttable(v);
	sq->pushstring(v, cname, -1);
	sq->pushinteger(v, value);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);
	return 0;
}
SQInteger fn_arithmetic(HSQUIRRELVM v)
{
	//one free variable
	SQInteger top = sq->gettop(v);
	SQInteger operation;
	//free variable always at top
	if (SQ_SUCCEEDED(sq->getinteger(v, top, &operation)))
	{
		uint8_t* buffer = (uint8_t*)calloc(5, sizeof(uint8_t));
		size_t size = 5;
		if (buffer)
		{
			switch (operation)
			{
			case ADDITION: *buffer = '+';// code for additioin 
				break;
			case SUBTRACTION: *buffer = '-';// code for subtraction 
				break;
			case MULTIPLICATION: *buffer = '*';// code for multiplication 
				break;
			case DIVISION: *buffer = '/';//code for division
				break;
			case MODULO: *buffer = '%';//code for modulo
				break;
			case UNARY_MINUS: *buffer = 'u';//code for unary minus
				break;
			default: sq->throwerror(v, "Unknown operation");
			}
			size_t s = 5, s1; int i = 1;
			do
			{
				SQObject obj;
				sq->getstackobj(v, i++, &obj);
				s1 = InsertSQObject(buffer, size, s, &obj);
				//s1 is no:of bytes inserted
				if (s1 == 0)
				{
					free(buffer);
					return sq->throwerror(v, "Error writing object");
				}
				s += s1;
			} while (i <= top - 1);//1 will be this(userdata), 2 will be other
			*(uint32_t*)(buffer + 1) = (uint32_t)size - 5;
			//buffer is ready, now return it.
			SQUserPointer data = sq->newuserdata(v, size);
			memcpy(data, buffer, size);
			free(buffer);
			sq->pushobject(v, delegatetable);
			sq->setdelegate(v, -2);
			return 1;
		}
		else return sq->throwerror(v, "Error when allocating memory");
	}
	else return sq->throwerror(v, "remexec: Error when performing arithmetic operation");
}
SQInteger fn_get(HSQUIRRELVM v)
{
	//1-userdata, 2-key
	SQInteger top = sq->gettop(v);
	//Check if userdata is good.
	SQUserPointer p;
	if (SQ_SUCCEEDED(sq->getuserdata(v, 1, &p, NULL)))
	{
		//Optionally check if userdata is good
		sq->pushroottable(v);
		sq->pushstring(v, "GetRemoteValueEx", -1);
		if (SQ_SUCCEEDED(sq->get(v, -2)))
		{
			sq->pushroottable(v);
			sq->push(v, 1);//pushes userdata
			sq->push(v, 2);//pushes the 'key'
			if (SQ_SUCCEEDED(sq->call(v, 3, 1, 1)))
			{
				//Now userdata on top.
				sq->remove(v, -2);//the function 'GetRemoteValueEx'
				sq->remove(v, -2);//the roottable
				return 1;
			}else {
				sq->settop(v, top);
				return sq->throwerror(v, "Could not call GetRemoteValueEx");
			}
		}
		else {
			sq->settop(v, top);
			return sq->throwerror(v, "Function 'GetRemoteValueEx' not found in roottable");
		}
	}
	else return sq->throwerror(v, "Error in getting userdata");
	return 1;
}
SQInteger fn_call(HSQUIRRELVM v)
{
	
	//1-userdata, 2-will be roottable, or see below
	// a<-GetRemoteValue("getroottable") 
	// a() -- in this call, 2-userdata
	// a().World.FindLocalPlayer() -- in the second call 'FindLocalPlayer()',
	// it is: 2- is a().World and is a userdata and not roottable
	//3,4,.. are parameters
	SQInteger top = sq->gettop(v);

	//Find if param 2 is roottable or userdata
	//Check if userdata is good.
	SQUserPointer p;
	if (SQ_SUCCEEDED(sq->getuserdata(v, 1, &p, NULL)))
	{
		//Optionally check if userdata is good
		sq->pushroottable(v);
		if (sq->gettype(v, 2) == OT_TABLE) //roottable
			sq->pushstring(v, "CallRemoteFunc", -1);
		else
			sq->pushstring(v, "CallRemoteFuncEx", -1);
		if (SQ_SUCCEEDED(sq->get(v, -2)))
		{
			sq->pushroottable(v);
			sq->push(v, 1);//pushes userdata, which is the function
			if (sq->gettype(v, 2) != OT_TABLE)
				sq->push(v, 2);//push the userdata which will be first parameter of CallRemoteFuncEx
			for (int i = 3; i <= top; i++)
				sq->push(v, i);//push each argument
			if (SQ_SUCCEEDED(sq->call(v, sq->gettype(v,2)==OT_TABLE?top:top+1, 1, 1)))//top because of roottable
			{
				//Now userdata on top.
				sq->remove(v, -2);//the function 'CallRemoteFunc'
				sq->remove(v, -2);//the roottable
				return 1;
			}
			else {
				sq->settop(v, top);
				return sq->throwerror(v, "Calling CallRemoteFunc failed");
			}
		}
		else {
			sq->settop(v, top);
			return sq->throwerror(v, "Function 'CallRemoteFunc' not found in roottable");
		}
	}
	else return sq->throwerror(v, "Error in getting userdata");
	return 1;
}
uint8_t RemExec_OnPluginCommand(uint32_t commandIdentifier, const char* message)
{
	if (commandIdentifier == 0x7D6E22D8)
	{
		int32_t id =Server->FindPlugin("SQHost2");
		if (id != -1)
		{
			size_t size;
			const void** exports= Server->GetPluginExports(id, &size);
			if (Server->GetLastError() == vcmpErrorNone)
			{
				if (exports != NULL && size > 0)
				{
					SquirrelImports** s = (SquirrelImports**)exports;
					SquirrelImports* f = (SquirrelImports*)(*s);
					if (f)
					{
						sq=*(f->GetSquirrelAPI());
						v=*(f->GetSquirrelVM());
						
						sq->pushroottable(v);
						sq->pushstring(v, "GetRemoteValue", -1);
						sq->newclosure(v, fn_GetRemoteValue, 0);
						sq->setparamscheck(v, 2, "t.");
						sq->setnativeclosurename(v, -1, "GetRemoteValue");
						sq->newslot(v, -3, SQFalse);
						sq->pop(v, 1);

						sq->pushroottable(v);
						sq->pushstring(v, "GetRemoteValueEx", -1);
						sq->newclosure(v, fn_GetRemoteValueEx, 0);
						sq->setparamscheck(v, 3, "t..");
						sq->setnativeclosurename(v, -1, "GetRemoteValueEx");
						sq->newslot(v, -3, SQFalse);
						sq->pop(v, 1);

						sq->pushroottable(v);
						sq->pushstring(v, "SetRemoteValue", -1);
						sq->newclosure(v, fn_SetRemoteValue, 0);
						sq->setparamscheck(v, 3, "t..");
						sq->setnativeclosurename(v, -1, "SetRemoteValue");
						sq->newslot(v, -3, SQFalse);
						sq->pop(v, 1);

						sq->pushroottable(v);
						sq->pushstring(v, "SetRemoteValueEx", -1);
						sq->newclosure(v, fn_SetRemoteValueEx, 0);
						sq->setparamscheck(v, 4, "t...");
						sq->setnativeclosurename(v, -1, "SetRemoteValueEx");
						sq->newslot(v, -3, SQFalse);
						sq->pop(v, 1);

						sq->pushroottable(v);
						sq->pushstring(v, "CallRemoteFunc", -1);
						sq->newclosure(v, fn_CallRemoteFunc, 0);
						sq->setparamscheck(v, -2, "t.");
						sq->setnativeclosurename(v, -1, "CallRemoteFunc");
						sq->newslot(v, -3, SQFalse);
						sq->pop(v, 1);

						sq->pushroottable(v);
						sq->pushstring(v, "CallRemoteFuncEx", -1);
						sq->newclosure(v, fn_CallRemoteFuncEx, 0);
						sq->setparamscheck(v, -3, "t..");
						sq->setnativeclosurename(v, -1, "CallRemoteFuncEx");
						sq->newslot(v, -3, SQFalse);
						sq->pop(v, 1);

						sq->pushroottable(v);
						sq->pushstring(v, "RemoteExec", -1);
						sq->newclosure(v, fn_RemExec, 0);
						sq->setparamscheck(v, -3, "tt|ux");
						sq->setnativeclosurename(v, -1, "RemoteExec");
						sq->newslot(v, -3, SQFalse);
						sq->pop(v, 1);

						sq->pushregistrytable(v);
						sq->pushstring(v, "RemExecClosures",-1);
						sq->newtable(v);
						sq->getstackobj(v, -1, &closuretable);
						sq->newslot(v, -3, SQFalse);
						sq->pop(v, 1);

						//Create Delegate Table;
						SQInteger top = sq->gettop(v);
						sq->newtable(v);
						sq->resetobject(&delegatetable);
						if (SQ_SUCCEEDED(sq->getstackobj(v, -1, &delegatetable)))
						{
							sq->pushstring(v, "_add", -1);
							sq->pushinteger(v, ADDITION);
							sq->newclosure(v, fn_arithmetic, 1);
							sq->setparamscheck(v, 2, "u.");
							if (SQ_FAILED(sq->newslot(v, -3, SQFalse)))
							{
								printf("remexec: Failed to create _add method");
							}
							
							sq->pushstring(v, "_sub", -1);
							sq->pushinteger(v, SUBTRACTION);
							sq->newclosure(v, fn_arithmetic, 1);
							sq->setparamscheck(v, 2, "u.");
							if (SQ_FAILED(sq->newslot(v, top+1, SQFalse)))//table will be on top+1
							{
								printf("remexec: Failed to create _sub method");
							}

							sq->pushstring(v, "_mul", -1);
							sq->pushinteger(v, MULTIPLICATION);
							sq->newclosure(v, fn_arithmetic, 1);
							sq->setparamscheck(v, 2, "u.");
							if (SQ_FAILED(sq->newslot(v, top + 1, SQFalse)))//table will be on top+1
							{
								printf("remexec: Failed to create _mul method");
							}

							sq->pushstring(v, "_div", -1);
							sq->pushinteger(v, DIVISION);
							sq->newclosure(v, fn_arithmetic, 1);
							sq->setparamscheck(v, 2, "u.");
							if (SQ_FAILED(sq->newslot(v, top + 1, SQFalse)))//table will be on top+1
							{
								printf("remexec: Failed to create _div method");
							}

							sq->pushstring(v, "_modulo", -1);
							sq->pushinteger(v, MODULO);
							sq->newclosure(v, fn_arithmetic, 1);
							sq->setparamscheck(v, 2, "u.");
							if (SQ_FAILED(sq->newslot(v, top + 1, SQFalse)))//table will be on top+1
							{
								printf("remexec: Failed to create _modulo method");
							}

							sq->pushstring(v, "_unm", -1);
							sq->pushinteger(v, UNARY_MINUS);
							sq->newclosure(v, fn_arithmetic, 1);
							sq->setparamscheck(v, 1, "u");
							if (SQ_FAILED(sq->newslot(v, top + 1, SQFalse)))//table will be on top+1
							{
								printf("remexec: Failed to create _unm method");
							}

							//Get Method
							sq->pushstring(v, "_get", -1);
							sq->newclosure(v, fn_get, 0);
							sq->setparamscheck(v, 2, "u.");
							if (SQ_FAILED(sq->newslot(v, top + 1, SQFalse)))
							{
								printf("remexec: Failed to create _get method in delegate table");
							}

							//Call Method
							//Get Method
							sq->pushstring(v, "_call", -1);
							sq->newclosure(v, fn_call, 0);
							sq->setparamscheck(v, -1, "u");
							if (SQ_FAILED(sq->newslot(v, top + 1, SQFalse)))
							{
								printf("remexec: Failed to create _get method in delegate table");
							}
						}
						else {
							printf("remexec error: could not set delegate table");
							return 0;
						}
						//Add reference
						sq->addref(v, &delegatetable);
						sq->settop(v, top);

#ifdef WIN32
						HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
						CONSOLE_SCREEN_BUFFER_INFO before;
						GetConsoleScreenBufferInfo(hstdout, &before);
						SetConsoleTextAttribute(hstdout, FOREGROUND_BLUE|FOREGROUND_INTENSITY);
						fputs("[MODULE]  ", stdout);
						SetConsoleTextAttribute(hstdout, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
						printf("Remote Exec %s by habi loaded\n", __REMEXEC_VERSION__);
						SetConsoleTextAttribute(hstdout, before.wAttributes);
#else
						printf("%c[1;32m[MODULE] %c[0;37mLoaded Remote Exec by habi (%s)\n %c[0m", 27, 27, __REMEXEC_VERSION__, 27);
#endif
					}
					
				}
			}
		}
	}
	return 1;
}
int GetSquirrelObjectFromBuffer(const uint8_t* data, size_t size, size_t index, HSQOBJECT* obj)
{
	if (index >= size)return 0;
	int i, len, n, m, counter, k; float j,x,y,z; HSQOBJECT _obj;
	
	switch (*(data + index))
	{
	case 'o':sq->pushnull(v);  k = 1; break;
	case 'i':
		if (size < index + 5)return 0;
		i = *(uint32_t*)(data + index + 1); sq->pushinteger(v, i); k = 5; break;
	case 'f':
		if (size < index + 5)return 0;
		j = *(float*)(data + index + 1); sq->pushfloat(v, j); k = 5; break;
	case 's':
		if (size < index + 3)return 0;//for reading len
		len = swap2(*(uint16_t*)(data + index + 1)); 
		//Check if len is less than size. otherwise it crash server.
		if (size < index + 3 + len ) { return 0; }
		sq->pushstring(v, (char*)data + index + 3, len); k = 1 + 2 + len; break;
	case 'b':
		/*Check if size is atleast index + 2
			b0
		*/
		if (index+2 < size) { return 0; }
		i = *(data + index + 1); sq->pushbool(v, i == 0 ? SQFalse : SQTrue); k = 2; break;
	
	case 'a':
		if (size < index + 5)return 0;//for the below line
		n = *(uint32_t*)(data + index + 1); 
		sq->newarray(v, n);//create array with size n
		m = 0; counter = 0;
		for (int l = 0; l < n; l++)
		{
			m = GetSquirrelObjectFromBuffer(data, size, index + 5 + counter, &_obj);
			if (m == 0)return 0;
			counter += m;
			sq->pushobject(v, _obj);
			sq->release(v, &_obj);
			if (SQ_SUCCEEDED(sq->arrayappend(v, -2)))
			{

			}
			else return 0;
		}
		k = 1 + 4 + counter;
		break;
	case 'v':
		/*Check if size is atleast index + 13
		vxxxxyyyyzzzz*/
		if (size < index + 13 ) { return 0; }

		x=*(float*)(data + index + 1); 
		y = *(float*)(data + index + 5); z = *(float*)(data + index + 9);
		sq->pushroottable(v); sq->pushstring(v, "Vector", -1);
		if (SQ_SUCCEEDED(sq->get(v, -2)))
		{
			sq->pushroottable(v); sq->pushfloat(v, x);
			sq->pushfloat(v, y); sq->pushfloat(v, z);
			if (SQ_SUCCEEDED(sq->call(v, 4, 1, 1)))
			{
				//vector on top (-1)
				sq->remove(v, -2);//The constructor of Vector
				sq->remove(v, -2);//The roottable pushed earlier.
			}
			else {
				sq->pop(v, 2);//roottable and constructor
				return 0;
			}
		}
		else {
			sq->pop(v, 1);//roottable
			return 0;
		}
		k = 13; break;
	case 'e'://error string
		if (size < index + 3)return 0;//for reading len;
		len = swap2(*(uint16_t*)(data + index + 1)); 
		//Check if len is less than size. otherwise it crash server.
		if (size < index + 3 + len ) { return 0; }
		sq->pushstring(v, (char*)data + index + 3, len);
		const SQChar* str;
		sq->getstring(v, -1, &str);
		printf("remexec (remote): %s\n", str);
		k = 1 + 2 + len;
		REMOTE_ERROR = 1;
		break;

		//Table
	case 't':
		if (size < index + 5)return 0;
		n = *(uint32_t*)(data + index + 1); sq->newtable(v);
		m = 0; counter = 0;
		for (int l = 0; l < n; l++)
		{
			//Process the key
			m = GetSquirrelObjectFromBuffer(data, size, index + 5 + counter, &_obj);
			if (m == 0)return 0;
			counter += m;
			sq->pushobject(v, _obj);
			sq->release(v, obj);
			//Now, process value
			sq->resetobject(&_obj);
			m = GetSquirrelObjectFromBuffer(data, size, index + 5 + counter, &_obj);
			if (m == 0)return 0;
			counter += m;
			sq->pushobject(v, _obj);
			sq->release(v, obj);

			if (SQ_SUCCEEDED(sq->rawset(v, -3)))
			{

			}
			else return 0;
		}
		k = 1 + 4 + counter;
		break;
	//Class
	case 'c':
		if (size < index + 5)return 0;
		n = *(uint32_t*)(data + index + 1); sq->newclass(v,SQFalse);
		m = 0; counter = 0;
		for (int l = 0; l < n; l++)
		{
			//Process the key
			m = GetSquirrelObjectFromBuffer(data, size, index + 5 + counter, &_obj);
			if (m == 0)return 0;
			counter += m;
			sq->pushobject(v, _obj);
			sq->release(v, obj);
			//Now, process value
			sq->resetobject(&_obj);
			m = GetSquirrelObjectFromBuffer(data, size, index + 5 + counter, &_obj);
			if (m == 0)return 0;
			counter += m;
			sq->pushobject(v, _obj);
			sq->release(v, obj);


			if (SQ_SUCCEEDED(sq->newslot(v, -3,SQFalse)))
			{

			}
			else { return 0; }
		}
		k = 1 + 4 + counter;
		break;
	default: return 0;
	}
	sq->getstackobj(v, -1, obj);
	sq->addref(v, obj);
	sq->poptop(v);
	return k;
}
void RemExec_OnClientScriptData(int32_t playerId, const uint8_t* data, size_t size)
{
	if (size > 4)
	{
		if (*(uint32_t*)data == REMEXEC_IDENTIFIER_REPLY)
		{
			HSQOBJECT obj; 
			if (size < 8)return;
			int32_t token_t = *(int32_t*)(data + 4); 
			sq->resetobject(&obj); 
			
			//Set Error Level to 0
			REMOTE_ERROR = 0;
			int m = GetSquirrelObjectFromBuffer(data, size, 8, &obj);
			if (m != 0)
			{
				if (token_t == -1)
				{
					//This means client encountered error and
					//parameter 3 of RemExec was false, i.e. no return callback
					//Just return;//Error already printed on console
					sq->release(v, &obj); 
					return;
				}
				//First set error level
				SQInteger t = sq->gettop(v);
				sq->pushroottable(v);
				sq->pushstring(v, REMEXEC_ERROR_VARIABLE, -1);
				sq->pushinteger(v, REMOTE_ERROR);
				if (SQ_FAILED(sq->rawset(v, -3)))
				{
					printf("remexec: Unknown error when setting error variable\n");
					sq->settop(v, t);
					return;
				}
				sq->poptop(v);//pops roottable
				
				sq->pushobject(v, closuretable);
				sq->pushinteger(v, token_t);
				if (SQ_SUCCEEDED(sq->rawget(v, -2)))
				{
					//Closure found
					sq->pushroottable(v);
					sq->pushobject(v, obj);
					sq->call(v, 2, 0, 1);
					sq->release(v, &obj);
					sq->pop(v, 1);//the closure

					//Now rawdelete the token_t from closure table
					sq->pushinteger(v, token_t);
					if (SQ_FAILED(sq->rawdeleteslot(v, -2, SQFalse)))
					{
						printf("remexec warning: failed to delete slot from closure table");
					}
				}
				else {
					sq->pushroottable(v);
					sq->pushstring(v, "onRemoteExecReply", -1);
					if (SQ_SUCCEEDED(sq->get(v, -2)))
					{
						sq->pushroottable(v);
						sq->pushinteger(v, token_t);
						sq->pushobject(v, obj);
						sq->call(v, 3, 0, 1);
						//sq->release(v, &obj);
						sq->poptop(v);//pop the function
					}
					sq->release(v, &obj);
					sq->poptop(v);//pop the roottable
				}
				sq->pop(v, 1);//the closuretable	
			}

			else printf("remexec: Error in getting squirrel object\n");
		}
	}
}
#ifdef WIN32
extern "C" __declspec(dllexport) unsigned int VcmpPluginInit(PluginFuncs * pluginFuncs, PluginCallbacks * pluginCallbacks, PluginInfo * pluginInfo) {
#else
extern "C"  unsigned int VcmpPluginInit(PluginFuncs * pluginFuncs, PluginCallbacks * pluginCallbacks, PluginInfo * pluginInfo) {
#endif
	Server=pluginFuncs;
	Callbacks = pluginCallbacks;
	// Plugin information
	pluginInfo->pluginVersion = 0x1;
	pluginInfo->apiMajorVersion = PLUGIN_API_MAJOR;
	pluginInfo->apiMinorVersion = PLUGIN_API_MINOR;
	memcpy(pluginInfo->name, "RemExec", 7); pluginInfo->name[7] = 0;
	pluginCallbacks->OnServerInitialise = RemExec_OnServerInitialize;
	pluginCallbacks->OnPluginCommand = RemExec_OnPluginCommand;
	pluginCallbacks->OnClientScriptData = RemExec_OnClientScriptData;
	token = 0;
	return 1;
}
