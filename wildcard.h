#ifndef FOO_H_   /* Include guard */
#define FOO_H_

int ptrlen(void **ptr);

void ptrcpy(void **dst, void**src);

void ptrcat(void **dst, void **src);

int containsWildcard(char *arg);

char **generateMatches(char* wildcard);

#endif // FOO_H_
