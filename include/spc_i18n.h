#ifndef __SPC_I18N_H__
#define __SPC_I18N_H__

#include <libintl.h>

void init_gettext();
char get_dec_point();

#if (SPC_I18N == 1)

# define _(String) gettext(String)

#else

# define _(String) String

#endif // SPC_I18N

#endif // __SPC_I18N_H__
