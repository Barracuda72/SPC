#ifndef __CONSTR_H__
#define __CONSTR_H__

#include <rec.h>
#include <tree.h>

tree *constr_if(rec_desc **area);
tree *constr_func_call(rec_desc **area);
tree *constr_proc_call(rec_desc **area);
tree *constr_while(rec_desc **area);
tree *constr_repeat(rec_desc **area);

#endif //__CONSTR_H__
