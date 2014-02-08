#ifndef __REC_H__
#define __REC_H__

typedef struct _rec_desc
{
	char data[16];	// 16 байт для MD5-хэша
	uint32_t class;	// Класс объекта
	uint32_t type;	// Тип объекта - переменная, функция, процедура
	uint32_t id;	// Идентификатор
	uint32_t level;	// Уровень вложенности (0 - глобальный, 1 - внутри функции, и тд)
	struct _rec_desc *next;	// Следующая запись
	void *param;
} rec_desc;

enum
{
	REC_NOTFOUND = -1,
	REC_CLASSERR = -2,
	REC_TYPEERR = -3
};

enum
{
	REC_PROCEDURE = 1000,
	REC_FUNCTION,
	REC_VARIABLE,
	REC_CONST
};

enum
{	REC_ARGUMENT = 700,
	REC_VARARG,
	REC_NOARG
};

typedef struct
{
	//char *name;		// Имя
	char buf2[16];	// FUCK!!
	uint32_t argcnt;	// Количество аргументов
	rec_desc *area;		// Указатель на область описания переменных
} rec_func_param;

int rec_put(const char *name, uint32_t type, uint32_t ro, rec_desc **arr);
void rec_set_type(uint32_t id, uint32_t type, rec_desc *arr);
void rec_set_param(uint32_t id, void *param, rec_desc *arr);
int rec_search(const char *name, uint32_t rclass, uint32_t type, rec_desc *arr);
int rec_get_func_arg(int fid, int num, rec_desc *arr);	// USE WITH CARE
int rec_get_type(uint32_t id, rec_desc *arr);
int rec_get_class(uint32_t id, rec_desc *arr);
rec_desc *rec_get(int id, rec_desc *arr);
//void rec_set_isarg(uint32_t id, uint32_t isarg, rec_desc *arr);
//uint32_t rec_get_isarg(uint32_t id, rec_desc *arr);
void *rec_get_param(uint32_t id, rec_desc *arr);
uint32_t rec_level_push(void);
uint32_t rec_level_pop(void);

#endif //__REC_H__