uint32_t swap4(uint32_t i) { //aa bb cc dd
	//00 00 00 aa | 00 aa bb cc | bb cc dd 00 | dd 00 00 00
	uint32_t u = (i >> 24) |
		((i >> 8) & 0x0000FF00) |
		((i << 8) & 0x00FF0000) |
		(i << 24);
	return u;
}
uint16_t swap2(uint16_t i) {
	uint16_t u = (i >> 8) | ((i << 8) & 0xFF00);
	return u;
}
SQRESULT sq_isobjVector(SQObject obj, SQBool *b)
{
	SQInteger top = sq->gettop(v); 
	sq->pushroottable(v);
	sq->pushstring(v, "Vector", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->pushroottable(v);
		sq->pushfloat(v, 1.0);
		sq->pushfloat(v, 1.0);
		sq->pushfloat(v, 1.0);
		if (SQ_SUCCEEDED(sq->call(v, 4, 1, 1)))
		{
			if (SQ_SUCCEEDED(sq->getclass(v, -1)))
			{
				sq->pushobject(v, obj);
				*b = sq->instanceof(v);
				sq->settop(v, top);
				return 0;
			}
		}
	}
	return sq->throwerror(v, "Error while checking for Vector");
}
SQRESULT sq_isobjEntityVector(SQObject obj, SQBool *b)
{
	SQInteger top = sq->gettop(v);
	sq->pushroottable(v);
	sq->pushstring(v, "EntityVector", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->pushroottable(v);
		sq->pushinteger(v, -1);
		sq->pushinteger(v, -1);
		sq->pushinteger(v, -1);
		sq->pushfloat(v, 1.0);
		sq->pushfloat(v, 1.0);
		sq->pushfloat(v, 1.0);
		if (SQ_SUCCEEDED(sq->call(v, 7, 1, 1)))
		{
			if (SQ_SUCCEEDED(sq->getclass(v, -1)))
			{
				sq->pushobject(v, obj);
				*b = sq->instanceof(v);
				sq->settop(v, top);
				return 0;
			}
		}
	}
	return sq->throwerror(v, "Error while checking for EntityVector");
}
SQRESULT sq_isobjQuaternion(SQObject obj, SQBool *b)
{
	SQInteger top = sq->gettop(v);
	sq->pushroottable(v);
	sq->pushstring(v, "Quaternion", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->pushroottable(v);
		sq->pushfloat(v, 0.0);
		sq->pushfloat(v, 0.0);
		sq->pushfloat(v, 0.0);
		sq->pushfloat(v, 1.0);
		if (SQ_SUCCEEDED(sq->call(v, 5, 1, 1)))
		{
			if (SQ_SUCCEEDED(sq->getclass(v, -1)))
			{
				sq->pushobject(v, obj);
				*b = sq->instanceof(v);
				sq->settop(v, top);
				return 0;
			}
		}
	}
	return sq->throwerror(v, "Error while checking for Quaternion");
}
SQRESULT sq_getVector(SQObject obj, float* x, float*y, float*z)
{
	SQInteger top = sq->gettop(v);
	sq->pushobject(v, obj);
	sq->pushstring(v, "x", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->getfloat(v, -1, x); sq->poptop(v);
		sq->pushstring(v, "y", -1);
		if (SQ_SUCCEEDED(sq->get(v, -2)))
		{
			sq->getfloat(v, -1, y); sq->poptop(v);
			sq->pushstring(v, "z", -1);
			if (SQ_SUCCEEDED(sq->get(v, -2)))
			{
				sq->getfloat(v, -1, z); sq->poptop(v);
				sq->pop(v, 1);//pops obj
				return 0;
			}
		}
	}
	return sq->throwerror(v, "Error getting Vector");
}
SQRESULT sq_getQuaternion(SQObject obj, float* x, float* y, float* z, float* w)
{
	SQInteger top = sq->gettop(v);
	sq->pushobject(v, obj);
	sq->pushstring(v, "x", -1);
	if (SQ_SUCCEEDED(sq->get(v, -2)))
	{
		sq->getfloat(v, -1, x); sq->poptop(v);
		sq->pushstring(v, "y", -1);
		if (SQ_SUCCEEDED(sq->get(v, -2)))
		{
			sq->getfloat(v, -1, y); sq->poptop(v);
			sq->pushstring(v, "z", -1);
			if (SQ_SUCCEEDED(sq->get(v, -2)))
			{
				sq->getfloat(v, -1, z); sq->poptop(v);
				sq->pushstring(v, "w", -1);
				if (SQ_SUCCEEDED(sq->get(v, -2)))
				{
					sq->getfloat(v, -1, w); sq->poptop(v);
					sq->pop(v, 1);//pops obj
					return 0;
				}
			}
		}
	}
	return sq->throwerror(v, "Error getting Quaternion");
}