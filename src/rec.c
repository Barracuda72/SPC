#include <stdint.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <string.h>

#include <rec.h>
#include <errors.h>
#include <debug.h>

extern int lex_line, lex_c;

static uint32_t free_id = 0;
uint32_t rec_level = 0;

int rec_put(const char *name, uint32_t class, uint32_t type, rec_desc **arr)
{
	//DBG("Putting %s\n", name);
	rec_desc *x = *arr;
	*arr = malloc(sizeof(rec_desc));
	(*arr) -> type = type;
	(*arr) -> class = class;
	(*arr) -> id = free_id++;
	(*arr) -> level = rec_level;
	(*arr) -> param = 0;
	MD5((uint8_t *)name, strlen(name), (uint8_t *)(*arr)->data);
	(*arr) -> next = x;
	return (*arr) -> id;
}

void rec_set_type(uint32_t id, uint32_t type, rec_desc *arr)
{
	while(arr != 0)
	{
		if(arr -> id == id)
		{
			arr -> type = type;
			return;
		}
		arr = arr -> next;
	}
	DBG("Set_Type: Identifier %d not found : %d:%d\n", id, lex_line, lex_c);
}

int rec_get_type(uint32_t id, rec_desc *arr)
{
	while(arr != 0)
	{
		if(arr -> id == id)
		{
			return arr -> type;
		}
		arr = arr -> next;
	}
	DBG("Get_Type: Identifier %d not found : %d:%d\n", id, lex_line, lex_c);
	return 0;
}

int rec_get_class(uint32_t id, rec_desc *arr)
{
	while(arr != 0)
	{
		if(arr -> id == id)
		{
			return arr -> class;
		}
		arr = arr -> next;
	}
	//DBG("rec_get_class: Identifier %d not found : %d:%d\n", id, lex_line, lex_c);
	return 0;
}

int rec_search(const char *name, uint32_t class, uint32_t type, rec_desc *arr)
{
	/*
		-1 - не найдено
		-2 - запрос на запись к объекту, помеченному "ro"
		-3 - тип не совпадает
		Возвращает идентификатор записи
	*/
	uint8_t tmp[16];
	MD5((uint8_t *)name, strlen(name), tmp);
	while(arr != 0)
	{
		if(!memcmp(arr->data, tmp, 16))
		{
			if((class != 0)&&(arr -> class != class))
				return REC_CLASSERR;
			// Если нужно сопоставить тип
			if((type != 0)&&(arr->type != type))
				return REC_TYPEERR;
			return arr->id;
			//return 0;
		}
		arr = arr->next;
	}
	return REC_NOTFOUND;
}

rec_desc *rec_get(int id, rec_desc *arr)
{
	while(arr != 0)
	{
		if(arr->id == id)
		{
			return arr;
			//return 0;
		}
		arr = arr->next;
	}
	return REC_NOTFOUND;
}

void rec_set_param(uint32_t id, void *param, rec_desc *arr)
{
	while(arr != 0)
	{
		if(arr -> id == id)
		{
			arr -> param = param;
			return;
		}
		arr = arr -> next;
	}
	DBG("Set_Param: Identifier %d not found\n", id);
}
/*
void rec_set_isarg(uint32_t id, uint32_t isarg, rec_desc *arr)
{
	while(arr != 0)
	{
		if(arr -> id == id)
		{
			arr -> is_arg = isarg;
			return;
		}
		arr = arr -> next;
	}
	DBG("Identifier %d not found\n", id);
}*/

void *rec_get_param(uint32_t id, rec_desc *arr)
{
	while(arr != 0)
	{
		if(arr -> id == id)
		{
			return arr -> param;
		}
		arr = arr -> next;
	}
	DBG("rec_get_param: Identifier %d not found\n", id);
}
/*
uint32_t rec_get_isarg(uint32_t id, rec_desc *arr)
{
	while(arr != 0)
	{
		if(arr -> id == id)
		{
			return arr -> is_arg;
		}
		arr = arr -> next;
	}
	DBG("Identifier %d not found\n", id);
}*/

uint32_t rec_level_push(void)
{
	rec_level++;
	return rec_level;
}

uint32_t rec_level_pop(void)
{
	if(rec_level != 0)
		rec_level--;
	else
		DBG("Rec level = 0, pop called!\n");
	return rec_level;
}

int rec_get_func_arg(int fid, int num, rec_desc *arr)
{
	rec_desc *r = rec_get(fid, arr);
	int l = r->level + 1;
	int res = -1;
	while((arr != 0) && (arr->level >= l))
	{
		if((arr -> param == REC_VARARG) || (arr -> param == REC_ARGUMENT))
		{
			if(num > 1)
				num--;
			else
			{
				res = arr -> id;
				break;
			}
		}
		arr = arr -> next;
	}
	return res;
	//DBG("Set_Param: Identifier %d not found\n", id);
}
