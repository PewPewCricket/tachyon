#ifndef PARSE_H
#define PARSE_H

#ifndef ASM_FILE
void mb2_parse_tags(void *tags);
#else
.extern mb2_parse_tags
#endif

#endif //PARSE_H
