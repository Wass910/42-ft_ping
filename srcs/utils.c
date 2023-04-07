#include "../inc/ft_ping.h"

char *delete_space(char *chaine) 
{
    int i, j = 0;

    for (i = 0; chaine[i] != '\0'; i++) {
        if (chaine[i] != ' ') {
            chaine[j] = chaine[i];
            j++;
        }
    }
    chaine[j] = '\0';
    return( chaine );
}

int		ft_strncmp(const char *s1, const char *s2, size_t n)
{
	if (n <= 0)
		return (0);
	while (n > 1 && (*s1 != '\0' && *s2 != '\0') && *s1 == *s2)
	{
		s1++;
		s2++;
		n--;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

int		ft_strlen(char *s)
{
	int i;

	i = 0;
	while (s[i] != '\0')
	{
		i++;
	}
	return (i);
}
