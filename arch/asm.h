#ifndef ASM_H
#define ASM_H

#define __ASM_FORM(x) " " #x " "
#define _ASM_PTR   __ASM_FORM(.quad)
#define _ASM_ALIGN __ASM_FORM(.balign 8)

#endif /* ASM_H */
