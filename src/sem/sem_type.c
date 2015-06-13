#include <sem/sem_type.h>
#include <sem/type_table.h>
#include <sem/id_table.h>

void sem_init(void)
{
    // Добавляем стандартные идентификаторы и типы
    type_init();
    id_init();
}
