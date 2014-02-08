#ifndef __SYNTAX_H__
#define __SYNTAX_H__

#include <rec.h>
#include <tree.h>

tree *program(void);
void accept(int s);
tree *operator(rec_desc **area);
tree *operators(rec_desc **area);

#endif //__SYNTAX_H__