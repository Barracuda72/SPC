#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list.h>

#include <errors.h>

#include <spc_i18n.h>

#include <sem/sem_type.h>
#include <sem/id_table.h>

extern int syn_line, syn_c;

/*
static id_table_elem *id_table = 0;
static id_table_elem *fake_id_table = 0;
*/
static list *id_table = 0;
static list *fake_id_table = 0;

static id_table_elem *_search_id_table(list *table, unsigned long hash, char *name, enum USE_CLASS *uclass);

/*
    Добавление элемента в таблицу идентификаторов
*/
static id_table_elem *_add_to_id_table(list **table, unsigned long hash, char *name, enum USE_CLASS uclass)
{
    // Если идентификатор уже есть - вернем NULL
    // Иначе - создадим и добавим
    id_table_elem *res = 0, *src = 0;
    enum USE_CLASS _uclass = uclass;

    if (!(src = _search_id_table(id_table, hash, name, &_uclass)) && (uclass == _uclass))
    {
        res = malloc(sizeof(id_table_elem));
        res->hash = hash;
        res->name = name;
        res->type = -1;
        res->uclass = uclass;
        //res->left = *table;
        //*table = res;
        add_to_list(table, res);
    } else {
        err_include(ERROR, _("duplicate identifier: "), name, syn_line, syn_c);
        if (uclass == UC_VAR)
            src -> type = 0;   // Ugly hack!
    }

    return res;
}

/*
    Поиск определяющего вхождения для соответствующего прикладного
*/
static id_table_elem *_search_id_table(list *table, unsigned long hash, char *name, enum USE_CLASS *uclass)
{
    //while (table != 0)
    list_for_each(table)
    {
        id_table_elem *q = list_tval(table);
        if ((q->hash == hash) && (!strcasecmp(q->name, name)) &&
                ((q->uclass == *uclass) || (*uclass == UC_UNK)))
        {
            if (*uclass == UC_UNK)
                *uclass = q->uclass;

            return q;
        }
        list_next(table);
    }
    return 0;
}

/*
    Обертка
*/
id_table_elem *add_to_id_table(unsigned long hash, char *name, enum USE_CLASS uclass)
{
    return _add_to_id_table(&id_table, hash, name, uclass);
}

/*
    Обертка
*/
id_table_elem *search_id_table(unsigned long hash, char *name, enum USE_CLASS *uclass)
{
    // Ищем в глобальной
    id_table_elem *res = _search_id_table(id_table, hash, name, uclass);
    // Не нашли в глобальной - ищем в фиктивной
    if (res == 0)
        res = _search_id_table(fake_id_table, hash, name, uclass);
    return res;
}

void id_init(void)
{
    // Добавляем стандартные идентификаторы
    id_table_elem * tmp;
    tmp = _add_to_id_table(&fake_id_table, 0, "char", UC_TYPE);
    tmp->type = chartype;

    tmp = _add_to_id_table(&fake_id_table, 0, "boolean", UC_TYPE);
    tmp->type = booltype;

    tmp = _add_to_id_table(&fake_id_table, 0, "integer", UC_TYPE);
    tmp->type = inttype;

    tmp = _add_to_id_table(&fake_id_table, 0, "real", UC_TYPE);
    tmp->type = realtype;

    tmp = _add_to_id_table(&fake_id_table, 0, "text", UC_TYPE);
    tmp->type = texttype;

    tmp = _add_to_id_table(&fake_id_table, 0, "string", UC_TYPE);
    tmp->type = stringtype;

    tmp = _add_to_id_table(&fake_id_table, 0, "pointer", UC_TYPE);
    tmp->type = ptrtype;
}
