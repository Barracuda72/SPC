#ifndef __TREE_H__
#define __TREE_H__

#include <rec.h>

/*
	Чтобы не забыть:
	Для паскаля узлы дерева могут быть следующих видов:
	- Присваивание значения переменной (Левый подузел - переменная, правый - выражение)
	- Вызов процедуры
	- Оператор ветвления
	- Оператор переключения (case)
	- Цикл while
	- Цикл repeat
	- Цикл for
	- Конструкция with - имеет смысл реализовать ее на уровне видимости
*/
enum
{
	TREE_FUNC_DECL = 600,	// Объявление функции
	TREE_PROC_DECL,		// Объявление процедуры
	TREE_FUNC_CALL,		// Вызов функции
	TREE_PROC_CALL,		// Вызов процедуры
	TREE_EXPR,		// Выражение
	TREE_LOGIC_EXPR,
	TREE_ASSIGN,		// Присваивание
	TREE_UCONST,		// Беззнаковая константа
	TREE_ICONST,		// Константа со знаком
	TREE_FCONST,		// Константа с плавающей точкой
	TREE_SCONST,		// Строковая константа
	TREE_VAR,		// Переменная
	TREE_OPERATOR,		// Оператор
	TREE_DECL_LIST,		// Список объявлений
	TREE_DECLARATION,	// Собственно объявление
	TREE_EXPR_LIST		// Список выражений
};

typedef struct _tree
{
	uint32_t type;	// Тип элемента
	uint32_t id;	// Идентификатор в таблице символов
	struct _tree *left;	// Левое поддерево
	struct _tree *right;	// Правое поддерево
	void *param;	// Параметр - подробное описание сущности
} tree;

tree *tree_new(void);
tree *tree_var(int id);
tree *tree_const(char *value, int type);
tree *tree_proc_desc(int id, void *param, tree *body, tree *decl);
tree *tree_func_desc(int id, void *param, tree *body, tree *decl);
tree *tree_logic_expr(int x);
int tree_get_type(tree *t, rec_desc *area);
tree *tree_assign(tree *left, tree *right);
tree *tree_func_call(int id, tree *params, rec_desc *area);
tree *tree_proc_call(int id, tree *params, rec_desc *area);
tree *tree_expr_list(void);

#endif //__TREE_H__