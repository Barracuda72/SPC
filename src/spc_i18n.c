#include <libintl.h>
#include <locale.h>
#include <spc_i18n.h>

static char dec_point = '.';

void init_gettext()
{
#ifdef SPC_I18N
    setlocale(LC_ALL, "");
    bindtextdomain(TEXT_DOMAIN, "./i18n/locale");
    //bindtextdomain(TEXT_DOMAIN, "/usr/share/locale");
    textdomain(TEXT_DOMAIN);

    dec_point = *(localeconv()->decimal_point);
#endif
}

char get_dec_point()
{
    return dec_point;
}
