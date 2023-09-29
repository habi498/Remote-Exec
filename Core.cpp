#include <stdint.h>
#include "squirrel.h"
#include "SQModule.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
extern HSQAPI sq;
extern HSQUIRRELVM v;
uint16_t swap2(uint16_t i);
SQRESULT sq_isobjVector(SQObject obj, SQBool *b);
SQRESULT sq_isobjEntityVector(SQObject obj, SQBool *b);
SQRESULT sq_isobjQuaternion(SQObject obj, SQBool *b);
SQRESULT sq_getVector(SQObject obj, float* x, float* y, float* z);
SQRESULT sq_getQuaternion(SQObject obj, float* x, float* y, float* z, float* w);
/*Returns no:of bytes inserted*/
size_t InsertSQObject(uint8_t* &buffer, size_t &size, size_t index, SQObject* obj)
{
	if (index > size)
	{
		return 0;
	}
	uint8_t* temp;
	if (index == size)
	{	//Allocate one byte for 'h','g', etc
		temp = (uint8_t*)realloc(buffer, size + 1);
		if (!temp) return 0; 
		buffer = temp; size += 1;
	}
	
	sq->pushobject(v, *obj);
	SQObjectType objtype = sq->gettype(v, -1); 
	/*bool PseudoTableUsed = false;
	SQObject ud;
	if (objtype == OT_TABLE)
	{
		sq->pushstring(v, "ud", -1);
		if (SQ_SUCCEEDED(sq->get(v, -2)))
		{
			//Userdata at -1
			if (sq->gettype(v, -1) == OT_USERDATA)
			{
				PseudoTableUsed = true;
				sq->getstackobj(v, -1, &ud);
			}
			sq->poptop(v);//pops userdata
		}
	}*/
	sq->poptop(v);//pops the object

	SQInteger i; SQFloat f; const SQChar* str = NULL; HSQOBJECT _obj;
	size_t len; SQUserPointer udata = NULL; SQBool b1,b2,b3;
	float x, y, z,w;
	int top, counter, result;
	
	switch (objtype)
	{
	case OT_NULL:
		buffer[index] = (char)('o');
		return 1;
	case OT_INTEGER:
		buffer[index] = (char)('i');
		if (index + 5 > size)//four bytes going to written
		{
			temp = (uint8_t*)realloc(buffer, index + 5);
			if (!temp)
				return 0;
			buffer = temp; size += 4;
		}
		i=sq->objtointeger(obj);
		*(uint32_t*)(buffer + index + 1) = i;
		return 5;
	case OT_FLOAT:
		buffer[index] = (char)('f');
		if (index + 5 > size)
		{
			temp = (uint8_t*)realloc(buffer, index + 5);
			if (!temp)
				return 0;
			buffer = temp; size += 4;
		}
		f = sq->objtofloat(obj);
		*(float*)(buffer + index + 1) = f;
		return 5;
	case OT_BOOL:
		buffer[index] = (char)('b');
		if (index + 2 > size)
		{
			temp = (uint8_t*)realloc(buffer, index + 2);
			if (!temp)
				return 0;
			buffer = temp; size += 1;
		}
		b1 = sq->objtobool(obj);
		if(b1 == SQTrue)
		*(buffer + index +1) = 1;
		else
		*(buffer + index +1) = 0;
		return 2;
	case OT_STRING:
		buffer[index] = (char)('s');
		str=sq->objtostring(obj);
		len = (uint16_t)strlen(str);
		if (len < 0)
		{
			return 0;
		}
		if (index + 1 + 2 + len > size)
		{
			temp = (uint8_t*)realloc(buffer, size + 2 + len);
			if (!temp) { return 0; }
			buffer = temp; size += 2 + len;
		}
		*(uint16_t*)(buffer+index+1) = swap2((uint16_t)len);
		memcpy(buffer+index+1+2, str, len);
		return 1+2+len;
	case OT_USERDATA: {
		//One byte has already been allocated
		sq->pushobject(v, *obj);
		sq->getuserdata(v, -1, &udata, NULL);
		i = sq->getsize(v, -1);
		sq->pop(v, 1);
		if (!udata)
			return 0;
		char h = *(char*)udata;
		if (h != 'E' && h != 'F' && h != 'g' && h != 'k' && h != 'j' && h != 'h'&&
			h != '+' && h!='-' && h != '*' && h != '/' && h != '%' &&
			h != 'u'//unary minus
			)
		{
			printf("remexec: Error unknown userdata passed\n");
			return 0;
		}
		try
		{
			len = *(uint32_t*)((uint8_t*)udata + 1) + 5;
			if (len > (uint32_t)i)return 0;
			if (index + len > size)
			{
				temp = (uint8_t*)realloc(buffer, index + len);
				if (!temp)return 0;
				buffer = temp; size = index + len;
			}
			memcpy(buffer + index, udata, len);
			return len;
		}
		catch (...)
		{
			printf("remexec: Error in copying  userdata\n");
			return 0;
		}
	}
	case OT_INSTANCE:
		//instance can be Vector, EntityVector or Quaternion
		if (SQ_SUCCEEDED(sq_isobjVector(*obj, &b1))
			&& SQ_SUCCEEDED(sq_isobjEntityVector(*obj, &b2))
			&&SQ_SUCCEEDED(sq_isobjQuaternion(*obj, &b3)))
		{
			if (b1 == SQTrue || b2 == SQTrue)
			{
				//It is Vector or EntityVector
				if (SQ_SUCCEEDED(sq_getVector(*obj, &x, &y, &z)))
				{
					buffer[index] = (char)'v';
					//Allocate memory - 12 bytes
					if (index + 13 > size)
					{
						temp = (uint8_t*)realloc(buffer, index + 13);
						if (!temp)return 0;
						buffer = temp; size += 12;
					}
					*(float*)(buffer + index + 1) = x;
					*(float*)(buffer + index + 5) = y;
					*(float*)(buffer + index + 9) = z;
					return 13;
				}
				else return 0 * printf("remexec: Error getting Vector");
			}
			else if (b3 == SQTrue)
			{
				//It is Quaternion
				if (SQ_SUCCEEDED(sq_getQuaternion(*obj, &x, &y, &z,&w)))
				{
					buffer[index] = (char)'q';
					//Allocate memory - 16 bytes
					if (index + 17 > size)
					{
						temp = (uint8_t*)realloc(buffer, index + 17);
						if (!temp)return 0;
						buffer = temp; size += 16;
					}
					*(float*)(buffer + index + 1) = x;
					*(float*)(buffer + index + 5) = y;
					*(float*)(buffer + index + 9) = z;
					*(float*)(buffer + index + 13) = w;
					return 17;
				}
				else return 0 * printf("remexec: Error getting Quaternion");
			}
			else return 0 * printf("remexec: Instance neither Vector nor Quaternion\n");
		}
		else
		{
			printf("remexec: Error while checking for Vector/Quaternion");
			return 0;
		}
	case OT_ARRAY: {
		top = sq->gettop(v);
		sq->pushobject(v, *obj);
		len = sq->getsize(v, -1);
		buffer[index] = 'a';
		if (index + 5 > size)//four bytes going to written
		{
			temp = (uint8_t*)realloc(buffer, index + 5);
			if (!temp)
				return 0;
			buffer = temp; size += 4;
		}
		counter = 0;
		for (unsigned int i = 0; i < len; i++)
		{
			sq->pushinteger(v, i);
			if (SQ_FAILED(sq->get(v, -2)))
			{
				sq->settop(v, top);
				return 0 * printf("remexec: Error processing array\n");
			}
			sq->getstackobj(v, -1, &_obj);
			sq->poptop(v); 
			result = InsertSQObject(buffer, size, index + 5 + counter, &_obj);
			if (result == 0)
			{
				sq->settop(v, top);
				return 0 * printf("remexec: Error processing array\n");
			}
			//Otherwise, result contains no:of bytes written to buffer 
			counter += result;
		}
		*(uint32_t*)(buffer + index + 1) = (uint32_t)(counter);
		sq->poptop(v);//pop the object
		return 5 + counter;
	}
	default:
		printf("Unknown datatype\n");
		return 0;
	}

}